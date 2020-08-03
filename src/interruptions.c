// ----------------------------------------------------------------------------
// BSD 3-Clause License

// Copyright (c) 2016, qbrobotics
// Copyright (c) 2017-2020, Centro "E.Piaggio"
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

/**
* \file         interruptions.c
*
* \brief        Interruption handling and firmware core functions
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/


//=================================================================     includes
#include "interruptions.h"

//==============================================================================
//                                                            RS485 RX INTERRUPT
//==============================================================================
// Processing RS-485 data frame:
//
// - 0:     Waits for beginning characters
// - 1:     Waits for ID;
// - 2:     Data length;
// - 3:     Receive all bytes;
// - 4:     Wait for another device end of transmission;
//
//==============================================================================

// PWM values needed to obtain 11.5 V given a certain input tension
// Numbers are sperimentally calculated //[index] (millivolts)
static const uint8 pwm_preload_values[29] = {100,    //0 (11500)
                                              83,
                                              78,
                                              76,
                                              74,
                                              72,    //5 (14000)
                                              70,
                                              68,
                                              67,
                                              65,
                                              64,    //10 (16500)
                                              63,
                                              62,
                                              61,
                                              60,
                                              59,    //15 (19000)
                                              58,
                                              57,
                                              56,
                                              56,
                                              55,    //20 (21500)
                                              54,
                                              54,
                                              53,
                                              52,
                                              52,    //25 (24000)
                                              52,
                                              51,
                                              51};   //28 (25500)

//==============================================================================
//                                                            RS485 RX INTERRUPT
//==============================================================================

CY_ISR(ISR_RS485_RX_ExInterrupt) {

    // Set RS485 flag
    
    interrupt_flag = TRUE;
     
}

//==============================================================================
//                                                        CYCLES TIMER INTERRUPT
//==============================================================================
CY_ISR(ISR_CYCLES_Handler){

    // Set cycles interrupt flag
    
    cycles_interrupt_flag = TRUE;
    
    CYCLES_TIMER_STATUS;

}

//==============================================================================
//                                                             INTERRUPT MANAGER
//==============================================================================
// Manage interrupt from RS485 
//==============================================================================
// Processing RS-485 data frame:
//
// - WAIT_START:    Waits for beginning characters;
// - WAIT_ID:       Waits for ID;
// - WAIT_LENGTH:   Data length;
// - RECEIVE:       Receive all bytes;
// - UNLOAD:        Wait for another device end of transmission;
//
//==============================================================================

void interrupt_manager(){

    
    //===========================================     local variables definition

    static uint8 CYDATA state = WAIT_START;                      // state
    
    //------------------------------------------------- local data packet
    static uint8 CYDATA data_packet_index;
    static uint8 CYDATA data_packet_length;
    static uint8 data_packet_buffer[128];                     
    static uint8 CYDATA rx_queue[3];                    // last 2 bytes received
    //-------------------------------------------------

    uint8 CYDATA    rx_data;                            // RS485 UART rx data
    CYBIT           rx_data_type = TRUE;                       // my id?
    uint8 CYDATA    package_count = 0;                     

    //======================================================     receive routine
    
    while(UART_RS485_GetRxBufferSize() && (package_count < 100)){
        // 100 - estimated maximum number of packets we can read without blocking firmware execution

        // Get next char
        rx_data = UART_RS485_GetChar();
        
        switch (state) {
            //-----     wait for frame start     -------------------------------
            case WAIT_START:
            
                rx_queue[0] = rx_queue[1];
                rx_queue[1] = rx_queue[2];
                rx_queue[2] = rx_data;
                
                // Check for header configuration package
                if ((rx_queue[1] == 58) && (rx_queue[2] == 58)) {
                    rx_queue[0] = 0;
                    rx_queue[1] = 0;
                    rx_queue[2] = 0;
                    state       = WAIT_ID;                    
                }else
                    if ((rx_queue[0] == 63) &&      //ASCII - ?
                        (rx_queue[1] == 13) &&      //ASCII - CR
                        (rx_queue[2] == 10))        //ASCII - LF)
                        infoGet(INFO_ALL);
                break;

            //-----     wait for id     ----------------------------------------
            case  WAIT_ID:

                // packet is for my ID or is broadcast
#ifdef MASTER_FW
                if (rx_data == c_mem.dev.id || rx_data == 0 || (c_mem.MS.slave_comm_active && rx_data == c_mem.MS.slave_ID))
                   rx_data_type = FALSE;
                else                //packet is for others
                    rx_data_type = TRUE;
#else
                if (rx_data == c_mem.dev.id || rx_data == 0)
                    rx_data_type = FALSE;
                else                //packet is for others
                    rx_data_type = TRUE;
#endif                
                data_packet_length = 0;
                state = WAIT_LENGTH;
                break;

            //-----     wait for length     ------------------------------------
            case  WAIT_LENGTH:

                data_packet_length = rx_data;
                // check validity of pack length
                if (data_packet_length <= 1) {
                    data_packet_length = 0;
                    state = WAIT_START;
                } else if (data_packet_length > 128) {
                    data_packet_length = 0;
                    state = WAIT_START;
                } else {
                    data_packet_index = 0;
                    
                    if(rx_data_type == FALSE)
                        state = RECEIVE;          // packet for me or broadcast
                    else
                        state = UNLOAD;           // packet for others
                }
                break;

            //-----     receiving     -------------------------------------------
            case RECEIVE:

                data_packet_buffer[data_packet_index] = rx_data;
                data_packet_index++;
                
                // check end of transmission
                if (data_packet_index >= data_packet_length) {
                    // verify if frame ID corresponded to the device ID
                    if (rx_data_type == FALSE) {
                        // copying data from buffer to global packet
                        memcpy(g_rx.buffer, data_packet_buffer, data_packet_length);
                        g_rx.length = data_packet_length;
                        g_rx.ready  = 1;
                        commProcess();
                    }
                    
                    data_packet_index  = 0;
                    data_packet_length = 0;
                    state              = WAIT_START;
                    package_count++;
                
                }
                break;

            //-----     other device is receving     ---------------------------
            case UNLOAD:
                if (!(--data_packet_length)) {
                    data_packet_index  = 0;
                    data_packet_length = 0;
                    RS485_CTS_Write(1);
                    RS485_CTS_Write(0);
                    state              = WAIT_START;
                    package_count++;
                }
                break;
        }
    }
}
//==============================================================================
//                                                            FUNCTION SCHEDULER
//==============================================================================
// Call all the function with the right frequency
//==============================================================================
// Base frequency 1000 Hz
//==============================================================================

void function_scheduler(void) {
 
    static uint16 counter_calibration = DIV_INIT_VALUE;
    static uint16 counter_tension_func = DIV_INIT_VALUE;
    char info_[2500] = "";
    
    uint8 MOTOR_IDX = 0;
    uint8 SECOND_MOTOR_IDX = 1;
    
    MY_TIMER_REG_Write(0x00);
    timer_value0 = (uint32)MY_TIMER_ReadCounter();

    // Start ADC Conversion, SOC = 1

    ADC_SOC_Write(0x01); 
    
    // Check Interrupt 

    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }
    
    switch(c_mem.dev.dev_type){
        case SOFTHAND_PRO:
        
            //---------------------------------- Get Encoders
            
            // Get CS0 encoder line for RIGHT HAND and CS1 line for LEFT HAND
            if (N_Encoder_Line_Connected[c_mem.motor[MOTOR_IDX].encoder_line] > 0) {
                // Change CS only if there are attached encoders on the line
                Change_CS_EncoderLine(c_mem.motor[MOTOR_IDX].encoder_line);
                
                encoder_reading_SPI(c_mem.motor[MOTOR_IDX].encoder_line, 0);    // assoc_motor = 0 for SoftHand

                // Check Interrupt     
                
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
            }
            
            //---------------------------------- Control SH Motor
            
            motor_control_SH();
            
            // Check external reference before processing other serial data and right after motor control
            if (c_mem.motor[MOTOR_IDX].not_revers_motor_flag == TRUE) {
                if (c_mem.motor[MOTOR_IDX].input_mode == INPUT_MODE_EXTERNAL) {
                    change_ext_ref_flag = FALSE;
                }   
            }

            // Check Interrupt 

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            break;
            
        case GENERIC_2_MOTORS:
            
            //---------------------------------- Get Encoders
            for (uint8 i = 0; i < N_ENCODER_LINE_MAX; i++) {
                if (N_Encoder_Line_Connected[i] > 0) {
                    // Change CS only if there are attached encoders on the line
                    Change_CS_EncoderLine(i);
                              
                    if (c_mem.motor[MOTOR_IDX].encoder_line == i) {
                        // Check if the line is used to control motor or not
                        encoder_reading_SPI(i, MOTOR_IDX); 
                    }
                    else {
                        if (c_mem.motor[SECOND_MOTOR_IDX].encoder_line == i) {
                            // Check if the line is used to control motor or not
                            encoder_reading_SPI(i, SECOND_MOTOR_IDX); 
                        }
                        else {
                            // Get only raw encoder value
                            ReadEncoderLine(N_Encoder_Line_Connected[i], i);
                            // Values are stored in Encoder_Value[i] vector
                        }
                    }

                    // Check Interrupt     
                    
                    if (interrupt_flag){
                        interrupt_flag = FALSE;
                        interrupt_manager();
                    }
                }
            } 
            
            //---------------------------------- Control Motor
            
            // Control MOTOR_IDX motor (always active) according to motor driver type
            motor_control_generic(MOTOR_IDX);
            
            // Check external reference before processing other serial data and right after motor control
            if (c_mem.motor[MOTOR_IDX].not_revers_motor_flag == TRUE) {
                if (c_mem.motor[MOTOR_IDX].input_mode == INPUT_MODE_EXTERNAL) {
                    change_ext_ref_flag = FALSE;
                }   
            }
            
            // Check Interrupt 

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            
            // Control 2nd motor (if necessary) according to motor driver type
            if (c_mem.dev.use_2nd_motor_flag == TRUE){
                motor_control_generic(SECOND_MOTOR_IDX);
                
                // Check external reference before processing other serial data and right after motor control
                if (c_mem.motor[SECOND_MOTOR_IDX].not_revers_motor_flag == TRUE) {
                    if (c_mem.motor[SECOND_MOTOR_IDX].input_mode == INPUT_MODE_EXTERNAL) {
                        change_ext_ref_flag = FALSE;
                    }   
                }
                
                // Check Interrupt 

                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
            }  
            break;
            
        case AIR_CHAMBERS_FB:
            
            //---------------------------------- Force Feedback control
            if (master_mode && c_mem.MS.slave_comm_active) {
                
                air_chambers_control();
            
                // Check Interrupt 
                
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                // Drive slave with reference generated on second motor index
                // Use second motor structures and parameters, only to generate position reference not for PID control
                // IMPORTANT: configure second motor parameters with proper slave parameters
                motor_control_generic(SECOND_MOTOR_IDX);
                
                // Check Interrupt 

                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                drive_slave(SECOND_MOTOR_IDX, c_mem.MS.slave_ID);
            
                // Check Interrupt 

                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
            }
            if (!master_mode){
                
                if (c_mem.MS.slave_comm_active) {
                    // Stop feedback motors
                    stop_feedback();
                }
                // Disable slave or motors
                deactivate_slaves();
                        
                // Check Interrupt 
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                } 
            }
    
            // Control MOTOR_IDX motor [PUMP] with PWM control
            motor_control_generic(MOTOR_IDX);
            
            // Check Interrupt 

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            
            break;
    }

    //---------------------------------- Read conversion buffer - LOCK function

    analog_read_end();

    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }

    //---------------------------------- Control Overcurrent

    overcurrent_control();
    
    // Check Interrupt 
    
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }

    //---------------------------------- Calibration 

    // Divider 10, freq = 500 Hz
    if (calib.enabled == TRUE) {
        if (counter_calibration == CALIBRATION_DIV) {
            calibration();
            counter_calibration = 0;
        }
        counter_calibration++;
    }

    // Check Interrupt 
    
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }
    
    if (c_mem.dev.dev_type == SOFTHAND_PRO){   
    
        //---------------------------------- Rest position check

        // Divider 10, freq = 500 Hz
        if (c_mem.SH.rest_position_flag == TRUE){
            if (counter_calibration == CALIBRATION_DIV) {
                    check_rest_position();
                    counter_calibration = 0;
            }
            counter_calibration++;

            // Check Interrupt     
            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
        }
        
    }
    //---------------------------------- Control Cycles Counter

    cycles_counter_update();

    // Check Cycles Interrupt 
    
    if (cycles_interrupt_flag){
        cycles_interrupt_flag = FALSE;

        // Cycles are written only every 120 seconds (CYCLES_TIMER interrupt)
        // to save EEPROM writings (1M maximum)
        if(can_write) {
            cycles_status = PREPARE_DATA;
            
            //Update time variable
            g_mem.cnt.total_time_on = g_mem.cnt.total_time_on + 120;  // Add 120 seconds.
            
            if (c_mem.exp.read_exp_port_flag == EXP_SD_RTC) {
                
                store_RTC_current_time();
                
                //Write in SD card
                prepare_SD_info(info_);
                FS_Write(pFile, info_, strlen(info_));
            }
        }
    }
    
    // Check Interrupt 
    
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }
    
    
    //---------------------------------- Check battery

    // Divider 10, freq = 500 Hz
    if (counter_tension_func == CALIBRATION_DIV) {
        battery_management();   
        counter_tension_func = 0;
    }
    counter_tension_func++;

    // Check Interrupt 
    
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }
   
    //---------------------------------- Read IMUs
    if (c_mem.imu.read_imu_flag) {
        ReadAllIMUs();      // IMU reading is atomic, no RS485 request is handled
        
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
        }
    }
   
    //---------------------------------- Update States
    
    // Load k-1 state
    memcpy( &g_emg_measOld, &g_emg_meas, sizeof(g_emg_meas) );
    memcpy( &g_measOld, &g_meas, sizeof(g_meas) );
    memcpy( &g_refOld, &g_ref, sizeof(g_ref) );

    // Load k+1 state        
    memcpy( &g_ref, &g_refNew, sizeof(g_ref) );
    memcpy( &g_imu, &g_imuNew, sizeof(g_imu) );
                
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }

    timer_value = (uint16)MY_TIMER_ReadCounter();
    cycle_time = ((float)(timer_value0 - timer_value)/1000000.0);
    MY_TIMER_REG_Write(0x01);   // reset timer

}

//==============================================================================
//                                                        MOTOR CONTROL SOFTHAND
//==============================================================================
void motor_control_SH() {

    int32 CYDATA pwm_input = 0;
    int32 CYDATA pos_error;         // position error
    int32 CYDATA curr_error;        // current error
    int32 CYDATA i_ref;             // current reference
    int32 CYDATA handle_value;
    int32 CYDATA err_emg_1, err_emg_2;

    uint8 MOTOR_IDX = 0;
    
    struct st_motor* SH_MOT = &c_mem.motor[MOTOR_IDX];      // SoftHand default motor
    uint8 SH_ENC_L = SH_MOT->encoder_line;          // Associated encoder line
    
    int32 CYDATA k_p = SH_MOT->k_p;  
    int32 CYDATA k_i = SH_MOT->k_i; 
    int32 CYDATA k_d = SH_MOT->k_d; 

    int32 CYDATA k_p_c = SH_MOT->k_p_c;  
    int32 CYDATA k_i_c = SH_MOT->k_i_c; 
    int32 CYDATA k_d_c = SH_MOT->k_d_c;   

    int32 CYDATA k_p_dl = SH_MOT->k_p_dl;  
    int32 CYDATA k_i_dl = SH_MOT->k_i_dl; 
    int32 CYDATA k_d_dl = SH_MOT->k_d_dl;
    
    int32 CYDATA k_p_c_dl = SH_MOT->k_p_c_dl;  
    int32 CYDATA k_i_c_dl = SH_MOT->k_i_c_dl; 
    int32 CYDATA k_d_c_dl = SH_MOT->k_d_c_dl;   

    // Static Variables

    static int32 pos_error_sum;     // position error sum for integral
    static int32 curr_error_sum;    // current error sum for integral
    
    static int32 prev_pos_err;      // previous position error for deriv. control
    static int32 prev_curr_err;     // previous current error for deriv. control
    static int32 prev_pwm;

    static CYBIT motor_dir = FALSE;
    static uint32 position_counter = 0;
    static uint8 current_emg = 0;   // 0 NONE
                                    // 1 EMG 1
                                    // 2 EMG 2
                                    // wait for both to get down
    

    err_emg_1 = g_emg_meas.emg[0] - c_mem.emg.emg_threshold[0];
    err_emg_2 = g_emg_meas.emg[1] - c_mem.emg.emg_threshold[1];

    // =========================== POSITION INPUT ==============================            
    switch(SH_MOT->input_mode) {
        case INPUT_MODE_ENCODER3:

            // Calculate handle value based on position of handle in the
            // sensor chain and multiplication factor between handle and motor position
            if (c_mem.enc[SH_ENC_L].double_encoder_on_off) 
                handle_value = (g_meas[SH_ENC_L].pos[2] * c_mem.enc[SH_ENC_L].motor_handle_ratio) + SH_MOT->pos_lim_inf;
            else
                handle_value = (g_meas[SH_ENC_L].pos[1] * c_mem.enc[SH_ENC_L].motor_handle_ratio) + SH_MOT->pos_lim_inf;
            

            // Read handle and use it as reference for the motor
            if (((handle_value - g_refOld[0].pos) > SH_MOT->max_step_pos) && (SH_MOT->max_step_pos != 0))
                g_ref[0].pos += SH_MOT->max_step_pos;
            else {
                if (((handle_value - g_refOld[0].pos) < SH_MOT->max_step_neg) && (SH_MOT->max_step_neg != 0))
                    g_ref[0].pos += SH_MOT->max_step_neg;
                else
                    g_ref[0].pos = handle_value;
            }
            break;
            
        case INPUT_MODE_EMG_PROPORTIONAL:
            if (err_emg_1 > 0)
                g_ref[0].pos = (err_emg_1 * g_mem.motor[0].pos_lim_sup) / (1024 - c_mem.emg.emg_threshold[0]);
            else
                g_ref[0].pos = 0;
            break;
                
        case INPUT_MODE_EMG_PROPORTIONAL_NC:
            if (err_emg_1 > 0)
                g_ref[0].pos = g_mem.motor[0].pos_lim_sup - (err_emg_1 * g_mem.motor[0].pos_lim_sup) / (1024 - c_mem.emg.emg_threshold[0]);
            else
                g_ref[0].pos = g_mem.motor[0].pos_lim_sup;
            break;

        case INPUT_MODE_EMG_INTEGRAL:
            g_ref[0].pos = g_mem.motor[0].pos_lim_sup - g_refOld[0].pos;
            if (err_emg_1 > 0) {
                g_ref[0].pos = g_refOld[0].pos + (err_emg_1 * (int)g_mem.emg.emg_speed * 2) / (1024 - c_mem.emg.emg_threshold[0]);
            }
            if (err_emg_2 > 0) {
                g_ref[0].pos = g_refOld[0].pos - (err_emg_2 * (int)g_mem.emg.emg_speed * 2) / (1024 - c_mem.emg.emg_threshold[1]);
            }
            break;


        case INPUT_MODE_EMG_FCFS:
            g_ref[0].pos = g_refOld[0].pos;
            switch (current_emg) {
                case 0:
                    // Look for the first EMG passing the threshold
                    if (err_emg_1 > 0 && err_emg_1 > err_emg_2) {
                        current_emg = 1;
                        break;
                    }
                    if (err_emg_2 > 0 && err_emg_2 > err_emg_1) {
                        current_emg = 2;
                        break;
                    }
                    break;

                case 1:
                    // EMG 1 is first
                    if (err_emg_1 < 0) {
                        current_emg = 0;
                        break;
                    }
                    g_ref[0].pos = g_refOld[0].pos + (err_emg_1 * g_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[0]);
                    break;

                case 2:
                    // EMG 2 is first
                    if (err_emg_2 < 0) {
                        current_emg = 0;
                        break;
                    }
                    g_ref[0].pos = g_refOld[0].pos - (err_emg_2 * g_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[1]);
                    break;

                default:
                    break;
            }
            break;

        case INPUT_MODE_EMG_FCFS_ADV:
            g_ref[0].pos = g_refOld[0].pos;
            switch (current_emg) {
                // Look for the first EMG passing the threshold
                case 0:
                    if (err_emg_1 > 0 && err_emg_1 > err_emg_2) {
                        current_emg = 1;
                        break;
                    }
                    if (err_emg_2 > 0 && err_emg_2 > err_emg_1) {
                        current_emg = 2;
                        break;
                    }
                    break;

                // EMG 1 is first
                case 1:
                    // If both signals are under threshold go back to status 0
                    if ((err_emg_1 < 0) && (err_emg_2 < 0)) {
                        current_emg = 0;
                        break;
                    }
                    // but if the current signal come back over threshold, continue using it
                    if (err_emg_1 > 0) 
                        g_ref[0].pos = g_refOld[0].pos + (err_emg_1 * g_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[0]);
                    
                    break;

                // EMG 2 is first
                case 2:
                    // If both signals are under threshold go back to status 0
                    if ((err_emg_1 < 0) && (err_emg_2 < 0)) {
                        current_emg = 0;
                        break;
                    }
                    // but if the current signal come back over threshold, continue using it
                    if (err_emg_2 > 0) {
                        g_ref[0].pos = g_refOld[0].pos - (err_emg_2 * c_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[1]);
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    // Position limit saturation
    if (SH_MOT->pos_lim_flag) {
        if (g_ref[0].pos < SH_MOT->pos_lim_inf) 
            g_ref[0].pos = SH_MOT->pos_lim_inf;
        if (g_ref[0].pos > SH_MOT->pos_lim_sup) 
            g_ref[0].pos = SH_MOT->pos_lim_sup;
    }
    
    if (battery_low_SoC == TRUE) {
        //Reopen the terminal device before disabling motor
        g_ref[0].pos = 0;
    }
        
    if (c_mem.SH.rest_position_flag == TRUE) {
        if (rest_enabled == 1){
            // Change position reference to drive motor to rest position smoothly
            g_ref[0].pos = rest_pos_curr_ref;
        }
        
        if (forced_open == 1) {
            // Open the SoftHand as EMG PROPORTIONAL input mode 
            if (err_emg_2 > 0)
                g_ref[0].pos = g_mem.SH.rest_pos - (err_emg_2 * g_mem.SH.rest_pos) / (1024 - c_mem.emg.emg_threshold[1]);
            else {
                g_ref[0].pos = g_mem.SH.rest_pos;
                forced_open = 0;
            }
        }
    }

    switch(SH_MOT->control_mode) {
        // ======================= CURRENT AND POSITION CONTROL =======================
        case CURR_AND_POS_CONTROL:
            pos_error = g_ref[0].pos - g_meas[SH_ENC_L].pos[0];

            pos_error_sum += pos_error;

            // error_sum saturation
            if (pos_error_sum > POS_INTEGRAL_SAT_LIMIT)
                pos_error_sum = POS_INTEGRAL_SAT_LIMIT;
            else {
                if (pos_error_sum < -POS_INTEGRAL_SAT_LIMIT) 
                    pos_error_sum = -POS_INTEGRAL_SAT_LIMIT;
            }
            
            // ------ position PID control ------

            i_ref = 0;
            
            // Proportional
            if (k_p_dl != 0)
                i_ref += (int32)(k_p_dl * pos_error) >> 16;

            // Integral
            if (k_i_dl != 0)
                i_ref += (int32)(k_i_dl * pos_error_sum) >> 16;

            // Derivative
            if (k_d_dl != 0)
                i_ref += (int32)(k_d_dl * (pos_error - prev_pos_err)) >> 16;
                        
            // Update previous position
            prev_pos_err = pos_error;

            // motor direction depends on i_ref
            if (i_ref >= 0)
                motor_dir = TRUE;
            else
                motor_dir = FALSE;

            // saturate max current
            if (i_ref > SH_MOT->current_limit)
                i_ref = SH_MOT->current_limit;
            else {
                if (i_ref < -SH_MOT->current_limit)
                    i_ref = -SH_MOT->current_limit;
        	}

            // current error and curr error sum
            curr_error = i_ref - g_meas[SH_ENC_L].curr;
            curr_error_sum += curr_error;
            
            if (curr_error_sum > CURR_INTEGRAL_SAT_LIMIT)
                curr_error_sum = CURR_INTEGRAL_SAT_LIMIT;
            else {
                if (curr_error_sum < -CURR_INTEGRAL_SAT_LIMIT)
                    curr_error_sum = -CURR_INTEGRAL_SAT_LIMIT;
            }

            // ----- current PID control -----

            pwm_input = 0;

            // Proportional
            if (k_p_c_dl != 0)
                pwm_input += (int32)(k_p_c_dl * curr_error) >> 16;

            // Integral
            if (k_i_c_dl != 0)
                pwm_input += (int32)(k_i_c_dl * curr_error_sum) >> 16;

            // Derivative
            if (k_d_c_dl != 0)
                pwm_input += (int32)(k_d_c_dl * (curr_error - prev_curr_err)) >> 16;

            // pwm_input saturation
            if (pwm_input < -PWM_MAX_VALUE) 
                pwm_input = -PWM_MAX_VALUE;
            else {
                if (pwm_input > PWM_MAX_VALUE)
                    pwm_input = PWM_MAX_VALUE;
            }

            // Update previous current
            prev_curr_err = curr_error;

        break;

        // ============================== POSITION CONTROL =====================
        case CONTROL_ANGLE:
            pos_error = g_ref[0].pos - g_meas[SH_ENC_L].pos[0];

            pos_error_sum += pos_error;

            // anti-windup (for integral control)
            if (pos_error_sum > ANTI_WINDUP)
                pos_error_sum = ANTI_WINDUP;
            else {
				if (pos_error_sum < -ANTI_WINDUP)
                	pos_error_sum = -ANTI_WINDUP;
            }

            // Proportional
            if (k_p != 0) 
                pwm_input = (int32)(k_p * pos_error) >> 16;
            

            // Integral
            if (k_i != 0) 
                pwm_input += (int32)(k_i * pos_error_sum) >> 16;
            

            // Derivative
            if (k_d != 0) 
                pwm_input += (int32)(k_d * (pos_error - prev_pos_err)) >> 16;
            

            // Update measure
            prev_pos_err = pos_error;

            if (pwm_input > 0)
                motor_dir = TRUE;
            else
                motor_dir = FALSE;

        break;

        // ========================== CURRENT CONTROL ==========================
        case CONTROL_CURRENT:
            if(g_ref[0].onoff && tension_valid) {
                
                i_ref = g_ref[0].curr;

                if (i_ref > SH_MOT->current_limit) 
                    i_ref = SH_MOT->current_limit;
                else {
                    if (i_ref < -SH_MOT->current_limit)
                        i_ref = -SH_MOT->current_limit;
                }
                
                // current error
                curr_error = i_ref - g_meas[SH_ENC_L].curr;            
                curr_error_sum += curr_error;
                
                if (curr_error_sum > CURR_INTEGRAL_SAT_LIMIT)
                    curr_error_sum = CURR_INTEGRAL_SAT_LIMIT;
                else {
                    if (curr_error_sum < -CURR_INTEGRAL_SAT_LIMIT) 
                        curr_error_sum = -CURR_INTEGRAL_SAT_LIMIT;
                }

                pwm_input = 0;

                // Proportional
                if (k_p_c != 0)
                    pwm_input += (int32)(k_p_c * curr_error) >> 16;

                // Integral
                if (k_i_c != 0)
                    pwm_input += (int32)(k_i_c * curr_error_sum) >> 16;

                // Derivative
                if (k_d_c != 0)
                    pwm_input += (int32)(k_d_c * (curr_error - prev_curr_err)) >> 16;
                
                prev_curr_err = curr_error;
                
                if (pwm_input >= 0) 
                    motor_dir = TRUE;
                else
                    motor_dir = FALSE;
            }
        break;
        
        // ================= DIRECT PWM CONTROL ================================
        case CONTROL_PWM:

            pwm_input = g_ref[0].pwm;

            if (pwm_input > 0) 
                motor_dir = TRUE;
            else 
                motor_dir = FALSE;
            

            // pwm_input saturation
            if (pwm_input < -PWM_MAX_VALUE) 
                pwm_input = -PWM_MAX_VALUE;
            else {
				if (pwm_input > PWM_MAX_VALUE) 
                	pwm_input = PWM_MAX_VALUE;
        	}

        break;
            
    }

    ////////////////////////////////////////////////////////////////////////////

    if(pwm_input >  PWM_MAX_VALUE) 
        pwm_input =  PWM_MAX_VALUE;
    if(pwm_input < -PWM_MAX_VALUE) 
        pwm_input = -PWM_MAX_VALUE;

    if (SH_MOT->control_mode != CONTROL_PWM) 
        pwm_input = (((pwm_input << 10) / PWM_MAX_VALUE) * dev_pwm_limit[0]) >> 10;
 
    //// RATE LIMITER ////
    if((pwm_input-prev_pwm) > SH_MOT->pwm_rate_limiter){
        pwm_input =  prev_pwm + SH_MOT->pwm_rate_limiter;
    }
    else {
        if((pwm_input-prev_pwm) < -SH_MOT->pwm_rate_limiter)
            pwm_input =  prev_pwm - SH_MOT->pwm_rate_limiter;
    }
    
    if(pwm_input >  PWM_MAX_VALUE) 
        pwm_input =  PWM_MAX_VALUE;
    if(pwm_input < -PWM_MAX_VALUE) 
        pwm_input = -PWM_MAX_VALUE;
    
    prev_pwm = pwm_input;
	
    pwm_sign = SIGN(pwm_input);   
    
    if (SH_MOT->not_revers_motor_flag == TRUE) {
        // Block motor with pwm = 0 to exploit not reversible motor behaviour 
    	if ( SH_MOT->control_mode != CONTROL_PWM && ((g_measOld[SH_ENC_L].pos[0]-g_meas[SH_ENC_L].pos[0]) < 50 && 
            (g_measOld[SH_ENC_L].pos[0]-g_meas[SH_ENC_L].pos[0]) > -50) && 
            ((g_refOld[0].pos - g_ref[0].pos) < 100 && (g_refOld[0].pos - g_ref[0].pos) > -100) ) {     // if used with EMG input, 100 is related to an emg speed greater than 25 (<< 2)
            position_counter++;
            
            if (position_counter >= 250) { 
                if (SH_MOT->input_mode == INPUT_MODE_EXTERNAL && change_ext_ref_flag == FALSE) {
                    g_refNew[0].pos = g_meas[SH_ENC_L].pos[0];       // Needed only when USB input mode is used, since g_refNew structure is used only with this input
                }
                g_ref[0].pos = g_meas[SH_ENC_L].pos[0];
                
                if (position_counter == 250){
                    // To do only once
                    g_meas[SH_ENC_L].hold_curr = g_meas[SH_ENC_L].curr;
                }
                pwm_input = 0;            
            }
        }
        else {
            position_counter = 0;
        }
    }
    
    if (motor_dir)
        MOTOR_DIR_1_Write(0x01);
    else
        MOTOR_DIR_1_Write(0x00);

    PWM_MOTORS_WriteCompare1(abs(pwm_input));
}

//==============================================================================
//                                                         MOTOR CONTROL GENERIC
//==============================================================================
void motor_control_generic(uint8 idx) {
    
    int32 CYDATA pwm_input = 0;
    int32 CYDATA pos_error;         // position error
    int32 CYDATA curr_error;        // current error
    int32 CYDATA i_ref;             // current reference
    int32 CYDATA handle_value;
    int32 CYDATA err_emg_1, err_emg_2;

    struct st_motor* MOT = &c_mem.motor[idx]; // Motor struct
    uint8 ENC_L = MOT->encoder_line;          // Associated encoder line
    
    int32 CYDATA k_p = MOT->k_p;  
    int32 CYDATA k_i = MOT->k_i; 
    int32 CYDATA k_d = MOT->k_d; 

    int32 CYDATA k_p_c = MOT->k_p_c;  
    int32 CYDATA k_i_c = MOT->k_i_c; 
    int32 CYDATA k_d_c = MOT->k_d_c;   

    int32 CYDATA k_p_dl = MOT->k_p_dl;  
    int32 CYDATA k_i_dl = MOT->k_i_dl; 
    int32 CYDATA k_d_dl = MOT->k_d_dl;
    
    int32 CYDATA k_p_c_dl = MOT->k_p_c_dl;  
    int32 CYDATA k_i_c_dl = MOT->k_i_c_dl; 
    int32 CYDATA k_d_c_dl = MOT->k_d_c_dl;   

    // Static Variables

    static int32 pos_error_sum[NUM_OF_MOTORS];     // position error sum for integral
    static int32 curr_error_sum[NUM_OF_MOTORS];    // current error sum for integral
    
    static int32 prev_pos_err[NUM_OF_MOTORS];      // previous position error for deriv. control
    static int32 prev_curr_err[NUM_OF_MOTORS];     // previous current error for deriv. control
    static int32 prev_pwm[NUM_OF_MOTORS];

    static CYBIT motor_dir[NUM_OF_MOTORS] = {FALSE, FALSE};
    static uint32 position_counter[NUM_OF_MOTORS] = {0, 0};
    static uint8 current_emg[NUM_OF_MOTORS] = {0, 0};   // 0 NONE
                                                        // 1 EMG 1
                                                        // 2 EMG 2
                                                        // wait for both to get down
    
    err_emg_1 = g_emg_meas.emg[0] - c_mem.emg.emg_threshold[0];
    err_emg_2 = g_emg_meas.emg[1] - c_mem.emg.emg_threshold[1];

    // =========================== POSITION INPUT ==============================            
    switch(MOT->input_mode) {
        case INPUT_MODE_ENCODER3:

            // Calculate handle value based on position of handle in the
            // sensor chain and multiplication factor between handle and motor position
            if (c_mem.enc[ENC_L].double_encoder_on_off) 
                handle_value = (g_meas[ENC_L].pos[2] * c_mem.enc[ENC_L].motor_handle_ratio) + MOT->pos_lim_inf;
            else
                handle_value = (g_meas[ENC_L].pos[1] * c_mem.enc[ENC_L].motor_handle_ratio) + MOT->pos_lim_inf;
            

            // Read handle and use it as reference for the motor
            if (((handle_value - g_refOld[idx].pos) > MOT->max_step_pos) && (MOT->max_step_pos != 0))
                g_ref[idx].pos += MOT->max_step_pos;
            else {
                if (((handle_value - g_refOld[idx].pos) < MOT->max_step_neg) && (MOT->max_step_neg != 0))
                    g_ref[idx].pos += MOT->max_step_neg;
                else
                    g_ref[idx].pos = handle_value;
            }
            break;
            
        case INPUT_MODE_EMG_PROPORTIONAL:
            if (err_emg_1 > 0)
                g_ref[idx].pos = (err_emg_1 * g_mem.motor[idx].pos_lim_sup) / (1024 - c_mem.emg.emg_threshold[0]);
            else
                g_ref[idx].pos = 0;
            break;

        case INPUT_MODE_EMG_INTEGRAL:
            g_ref[idx].pos = g_refOld[idx].pos;
            if (err_emg_1 > 0) {
                g_ref[idx].pos = g_refOld[idx].pos + (err_emg_1 * (int)g_mem.emg.emg_speed * 2) / (1024 - c_mem.emg.emg_threshold[0]);
            }
            if (err_emg_2 > 0) {
                g_ref[idx].pos = g_refOld[idx].pos - (err_emg_2 * (int)g_mem.emg.emg_speed * 2) / (1024 - c_mem.emg.emg_threshold[1]);
            }
            break;

        case INPUT_MODE_EMG_FCFS:
            g_ref[idx].pos = g_refOld[idx].pos;
            switch (current_emg[idx]) {
                case 0:
                    // Look for the first EMG passing the threshold
                    if (err_emg_1 > 0 && err_emg_1 > err_emg_2) {
                        current_emg[idx] = 1;
                        break;
                    }
                    if (err_emg_2 > 0 && err_emg_2 > err_emg_1) {
                        current_emg[idx] = 2;
                        break;
                    }
                    break;

                case 1:
                    // EMG 1 is first
                    if (err_emg_1 < 0) {
                        current_emg[idx] = 0;
                        break;
                    }
                    g_ref[idx].pos = g_refOld[idx].pos + (err_emg_1 * g_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[0]);
                    break;

                case 2:
                    // EMG 2 is first
                    if (err_emg_2 < 0) {
                        current_emg[idx] = 0;
                        break;
                    }
                    g_ref[idx].pos = g_refOld[idx].pos - (err_emg_2 * g_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[1]);
                    break;

                default:
                    break;
            }
            break;

        case INPUT_MODE_EMG_FCFS_ADV:
            g_ref[idx].pos = g_refOld[idx].pos;
            switch (current_emg[idx]) {
                // Look for the first EMG passing the threshold
                case 0:
                    if (err_emg_1 > 0 && err_emg_1 > err_emg_2) {
                        current_emg[idx] = 1;
                        break;
                    }
                    if (err_emg_2 > 0 && err_emg_2 > err_emg_1) {
                        current_emg[idx] = 2;
                        break;
                    }
                    break;

                // EMG 1 is first
                case 1:
                    // If both signals are under threshold go back to status 0
                    if ((err_emg_1 < 0) && (err_emg_2 < 0)) {
                        current_emg[idx] = 0;
                        break;
                    }
                    // but if the current signal come back over threshold, continue using it
                    if (err_emg_1 > 0) 
                        g_ref[idx].pos = g_refOld[idx].pos + (err_emg_1 * g_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[0]);
                    
                    break;

                // EMG 2 is first
                case 2:
                    // If both signals are under threshold go back to status 0
                    if ((err_emg_1 < 0) && (err_emg_2 < 0)) {
                        current_emg[idx] = 0;
                        break;
                    }
                    // but if the current signal come back over threshold, continue using it
                    if (err_emg_2 > 0) {
                        g_ref[idx].pos = g_refOld[idx].pos - (err_emg_2 * c_mem.emg.emg_speed << 2) / (1024 - c_mem.emg.emg_threshold[1]);
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    // Position limit saturation
    if (MOT->pos_lim_flag) {
        if (g_ref[idx].pos < MOT->pos_lim_inf) 
            g_ref[idx].pos = MOT->pos_lim_inf;
        if (g_ref[idx].pos > MOT->pos_lim_sup) 
            g_ref[idx].pos = MOT->pos_lim_sup;
    }
    
    // SAFETY
    if (battery_low_SoC == TRUE) {
        //Reopen the terminal device before disabling motor
        g_ref[idx].pos = 0;
    }        

    switch(MOT->control_mode) {
        // ======================= CURRENT AND POSITION CONTROL =======================
        case CURR_AND_POS_CONTROL:
            pos_error = g_ref[0].pos - g_meas[ENC_L].pos[0];

            pos_error_sum[idx] += pos_error;

            // error_sum saturation
            if (pos_error_sum[idx] > POS_INTEGRAL_SAT_LIMIT)
                pos_error_sum[idx] = POS_INTEGRAL_SAT_LIMIT;
            else {
                if (pos_error_sum[idx] < -POS_INTEGRAL_SAT_LIMIT) 
                    pos_error_sum[idx] = -POS_INTEGRAL_SAT_LIMIT;
            }
            
            // ------ position PID control ------

            i_ref = 0;
            
            // Proportional
            if (k_p_dl != 0)
                i_ref += (int32)(k_p_dl * pos_error) >> 16;

            // Integral
            if (k_i_dl != 0)
                i_ref += (int32)(k_i_dl * pos_error_sum[idx]) >> 16;

            // Derivative
            if (k_d_dl != 0)
                i_ref += (int32)(k_d_dl * (pos_error - prev_pos_err[idx])) >> 16;
                        
            // Update previous position
            prev_pos_err[idx] = pos_error;

            // motor direction depends on i_ref
            if (i_ref >= 0)
                motor_dir[idx] = TRUE;
            else
                motor_dir[idx] = FALSE;

            // saturate max current
            if (i_ref > MOT->current_limit)
                i_ref = MOT->current_limit;
            else {
                if (i_ref < -MOT->current_limit)
                    i_ref = -MOT->current_limit;
        	}

            // current error and curr error sum
            curr_error = i_ref - g_meas[ENC_L].curr;
            curr_error_sum[idx] += curr_error;
            
            if (curr_error_sum[idx] > CURR_INTEGRAL_SAT_LIMIT)
                curr_error_sum[idx] = CURR_INTEGRAL_SAT_LIMIT;
            else {
                if (curr_error_sum[idx] < -CURR_INTEGRAL_SAT_LIMIT)
                    curr_error_sum[idx] = -CURR_INTEGRAL_SAT_LIMIT;
            }

            // ----- current PID control -----

            pwm_input = 0;

            // Proportional
            if (k_p_c_dl != 0)
                pwm_input += (int32)(k_p_c_dl * curr_error) >> 16;

            // Integral
            if (k_i_c_dl != 0)
                pwm_input += (int32)(k_i_c_dl * curr_error_sum[idx]) >> 16;

            // Derivative
            if (k_d_c_dl != 0)
                pwm_input += (int32)(k_d_c_dl * (curr_error - prev_curr_err[idx])) >> 16;

            // pwm_input saturation
            if (pwm_input < -PWM_MAX_VALUE) 
                pwm_input = -PWM_MAX_VALUE;
            else {
                if (pwm_input > PWM_MAX_VALUE)
                    pwm_input = PWM_MAX_VALUE;
            }

            // Update previous current
            prev_curr_err[idx] = curr_error;

        break;

        // ============================== POSITION CONTROL =====================
        case CONTROL_ANGLE:
            pos_error = g_ref[idx].pos - g_meas[ENC_L].pos[0];

            pos_error_sum[idx] += pos_error;

            // anti-windup (for integral control)
            if (pos_error_sum[idx] > ANTI_WINDUP)
                pos_error_sum[idx] = ANTI_WINDUP;
            else {
				if (pos_error_sum[idx] < -ANTI_WINDUP)
                	pos_error_sum[idx] = -ANTI_WINDUP;
            }

            // Proportional
            if (k_p != 0) 
                pwm_input = (int32)(k_p * pos_error) >> 16;
            

            // Integral
            if (k_i != 0) 
                pwm_input += (int32)(k_i * pos_error_sum[idx]) >> 16;
            

            // Derivative
            if (k_d != 0) 
                pwm_input += (int32)(k_d * (pos_error - prev_pos_err[idx])) >> 16;
            

            // Update measure
            prev_pos_err[idx] = pos_error;

            if (pwm_input > 0)
                motor_dir[idx] = TRUE;
            else
                motor_dir[idx] = FALSE;

        break;

        // ========================== CURRENT CONTROL ==========================
        case CONTROL_CURRENT:
            if(g_ref[idx].onoff && tension_valid) {
                
                i_ref = g_ref[idx].curr;

                if (i_ref > MOT->current_limit) 
                    i_ref = MOT->current_limit;
                else {
                    if (i_ref < -MOT->current_limit)
                        i_ref = -MOT->current_limit;
                }
                
                // current error
                curr_error = i_ref - g_meas[ENC_L].curr;            
                curr_error_sum[idx] += curr_error;
                
                if (curr_error_sum[idx] > CURR_INTEGRAL_SAT_LIMIT)
                    curr_error_sum[idx] = CURR_INTEGRAL_SAT_LIMIT;
                else {
                    if (curr_error_sum[idx] < -CURR_INTEGRAL_SAT_LIMIT) 
                        curr_error_sum[idx] = -CURR_INTEGRAL_SAT_LIMIT;
                }

                pwm_input = 0;

                // Proportional
                if (k_p_c != 0)
                    pwm_input += (int32)(k_p_c * curr_error) >> 16;

                // Integral
                if (k_i_c != 0)
                    pwm_input += (int32)(k_i_c * curr_error_sum[idx]) >> 16;

                // Derivative
                if (k_d_c != 0)
                    pwm_input += (int32)(k_d_c * (curr_error - prev_curr_err[idx])) >> 16;
                
                prev_curr_err[idx] = curr_error;
                
                if (pwm_input >= 0) 
                    motor_dir[idx] = TRUE;
                else
                    motor_dir[idx] = FALSE;
            }
        break;
        
        // ================= DIRECT PWM CONTROL ================================
        case CONTROL_PWM:

            pwm_input = g_ref[idx].pwm;

            if (pwm_input > 0) 
                motor_dir[idx] = TRUE;
            else 
                motor_dir[idx] = FALSE;
            

            // pwm_input saturation
            if (pwm_input < -PWM_MAX_VALUE) 
                pwm_input = -PWM_MAX_VALUE;
            else {
				if (pwm_input > PWM_MAX_VALUE) 
                	pwm_input = PWM_MAX_VALUE;
        	}

        break;
            
    }

    ////////////////////////////////////////////////////////////////////////////
     ////////////////////////////////////////////////////////////////////////////

    if(pwm_input >  PWM_MAX_VALUE) 
        pwm_input =  PWM_MAX_VALUE;
    if(pwm_input < -PWM_MAX_VALUE) 
        pwm_input = -PWM_MAX_VALUE;

    if (MOT->control_mode != CONTROL_PWM) 
        pwm_input = (((pwm_input << 10) / PWM_MAX_VALUE) * dev_pwm_limit[0]) >> 10;
 
    //// RATE LIMITER ////
       if((pwm_input-prev_pwm[idx]) > MOT->pwm_rate_limiter){
        pwm_input =  prev_pwm[idx] + MOT->pwm_rate_limiter;
    }
    else {
        if((pwm_input-prev_pwm[idx]) < -MOT->pwm_rate_limiter)
            pwm_input =  prev_pwm[idx] - MOT->pwm_rate_limiter;
    }
    
    if(pwm_input >  PWM_MAX_VALUE) 
        pwm_input =  PWM_MAX_VALUE;
    if(pwm_input < -PWM_MAX_VALUE) 
        pwm_input = -PWM_MAX_VALUE;
    
    prev_pwm[idx] = pwm_input;
	
    pwm_sign = SIGN(pwm_input);   
    
    if (MOT->not_revers_motor_flag == TRUE) {
        // Block motor with pwm = 0 to exploit not reversible motor behaviour 
    	if ( MOT->control_mode != CONTROL_PWM && ((g_measOld[ENC_L].pos[idx]-g_meas[ENC_L].pos[idx]) < 50 && 
            (g_measOld[ENC_L].pos[idx]-g_meas[ENC_L].pos[idx]) > -50) && 
            ((g_refOld[idx].pos - g_ref[idx].pos) < 100 && (g_refOld[idx].pos - g_ref[idx].pos) > -100) ) {
            position_counter[idx]++;
            
            if (position_counter[idx] >= 250) { 
                if (MOT->input_mode == INPUT_MODE_EXTERNAL && change_ext_ref_flag == FALSE) {
                    g_refNew[idx].pos = g_meas[ENC_L].pos[0];       // Needed only when USB input mode is used, since g_refNew structure is used only with this input
                }
                g_ref[idx].pos = g_meas[ENC_L].pos[0];
                
                if (position_counter[idx] == 250){
                    // To do only once
                    g_meas[ENC_L].hold_curr = g_meas[ENC_L].curr;
                }
                pwm_input = 0;            
            }
        }
        else {
            position_counter[idx] = 0;
        }
    }
    
    // Set motor direction and write pwm value
    switch (idx) {
        case 0:         // Motor 1
            if (motor_dir[idx])
                MOTOR_DIR_1_Write(0x01);
            else
                MOTOR_DIR_1_Write(0x00);
            
            if (MOT->motor_driver_type == DRIVER_VNH5019) {
                PWM_MOTORS_WriteCompare1(PWM_MAX_VALUE - abs(pwm_input));
            }
            else {  // DRIVER_MC33887 standard
                PWM_MOTORS_WriteCompare1(abs(pwm_input));    
            }
            
            break;
        case 1:         // Motor 2
            
            if (motor_dir[idx])
                MOTOR_DIR_2_Write(0x01);
            else
                MOTOR_DIR_2_Write(0x00);
            
            if (MOT->motor_driver_type == DRIVER_VNH5019) {
                PWM_MOTORS_WriteCompare2(PWM_MAX_VALUE - abs(pwm_input));
            }
            else {  // DRIVER_MC33887 standard
                PWM_MOTORS_WriteCompare2(abs(pwm_input));    
            }
            
            break;
        default:
            break;
    }
    
}

//==============================================================================
//                                                           ENCODER READING SPI
//==============================================================================

void encoder_reading_SPI(uint8 n_line, uint8 assoc_motor) {

    uint8 CYDATA index = 0;
    
    // Encoder Variables  
    uint8 jj;
     
    uint32 aux_encoder[NUM_OF_SENSORS];
    int16 tmp_value_encoder;
    int32 tmp_value_encoder_32;
    int32 value_encoder;
    int32 speed_encoder;
    int32 value_diff;
	int8 init_rot = 0;

    static int32 last_value_encoder[N_ENCODER_LINE_MAX][NUM_OF_SENSORS];
    static int32 comp_value_encoder[N_ENCODER_LINE_MAX][NUM_OF_SENSORS];
    static uint8 error[N_ENCODER_LINE_MAX][NUM_OF_SENSORS];
    
    static CYBIT only_first_time[N_ENCODER_LINE_MAX] = {TRUE, TRUE};
    static CYBIT safe_startup_motor_activation[N_ENCODER_LINE_MAX] = {FALSE, FALSE};
    static uint8 one_time_execute[N_ENCODER_LINE_MAX] = {0, 0};
    static uint32 count_startup_motor[N_ENCODER_LINE_MAX] = {0, 0};
    static CYBIT pos_reconstruct[N_ENCODER_LINE_MAX] = {FALSE, FALSE};

    static int32 v_value[N_ENCODER_LINE_MAX][N_ENCODERS];   //last value for velocity
    static int32 vv_value[N_ENCODER_LINE_MAX][N_ENCODERS];  //last last value for velocity
    static int32 vvv_value[N_ENCODER_LINE_MAX][N_ENCODERS];  //last last last value for velocity
//    
    if (reset_last_value_flag) {
        for (jj = N_ENCODERS; jj--;) 
            last_value_encoder[n_line][jj] = 0;
        
        reset_last_value_flag = 0;
    }

    //======================================================     reading sensors
    for (index = 0; index < N_ENCODERS; index++) {
        aux_encoder[index] = 0;
    }   
    
    ReadEncoderLine(N_Encoder_Line_Connected[n_line] , n_line);         //CS0 for right hand
                                                                        //CS1 for left hand
    for (int j = 0; j < N_ENCODERS; j++) {    
        
        // As default, index=0 reads SoftHand Pro encoder positioned on screw, while index=1 reads encoder positioned on gear    
        index = c_mem.enc[n_line].Enc_idx_use_for_control[j];       // take encoder idx used for motor control
        
        if (Encoder_Check[n_line][index] > 15){  // check on encoder data
            aux_encoder[j] = (uint32)Encoder_Value[n_line][index];     // 00000000000000[20] XXXXXXXXXXXX[12]
        }
    }
    
    // S = SIGN BIT
    // X = ENCODER VALUE BIT
    // 0 = 0 BIT
    // C = CONTROL BIT
    
    for (index = 0; index < N_ENCODERS; index++) {
        
        data_encoder_raw[index] = aux_encoder[index];
        
        tmp_value_encoder = (int16)(aux_encoder[index] - (uint16)g_mem.enc[n_line].m_off[index]);
        if (tmp_value_encoder < 0){
            tmp_value_encoder = tmp_value_encoder + 4096;   //SSSS[4] XXXXXXXXXXXX[12] worst case range [-4096, 4095]
        }       // Range [0, 4096]

        if (tmp_value_encoder >= 2048) {
           tmp_value_encoder = tmp_value_encoder - 4096;    //SSSS[4] XXXXXXXXXXXX[12] range [-2048, 2047]
        }       // Range [-2048, 2047]

        tmp_value_encoder_32 = (((int32)(tmp_value_encoder)) << 4);     //SSSSSSSSSSSSSSSS[16] XXXXXXXXXXXX[12] 0000[4] range [-32768, 32767]
        comp_value_encoder[n_line][index] = tmp_value_encoder_32;

        // Initialize last_value_encoder
        if (only_first_time[n_line]) {
            last_value_encoder[n_line][index] = tmp_value_encoder_32;
            if (index == 2)
                only_first_time[n_line] = 0;
        }

        // Take care of rotations
        value_diff = tmp_value_encoder_32 - last_value_encoder[n_line][index];     // worst case SSSSSSSSSSSSSSS[15] XXXXXXXXXXXXX[13] 0000[4]

        // ====================== 1 TURN ======================
        // -32768                    0                    32767 -32768                   0                     32767
        // |-------------------------|-------------------------|-------------------------|-------------------------|
        //              |                         |      |           |      |                         |
        //           -16384                     16383    |           |   -16384                     16383
        //                                               |           |
        //                                           24575           -24576
        //                                               |___________|
        //                                                   49152

        // if we are in the right interval, take care of rotation
        // and update the variable only if the difference between
        // one measure and another is less than 1/4 of turn

        // Considering we are sampling at 1kHz, this means that our shaft needs
        // to go slower than 1/4 turn every ms -> 1 turn every 4ms
        // equal to 250 turn/s -> 15000 RPM

        if (value_diff > 49152)
            g_meas[n_line].rot[index]--;
        else{ 
            if (value_diff < -49152)
                g_meas[n_line].rot[index]++;
            else{
                if (abs(value_diff) > 16384) { // if two measures are too far
                    error[n_line][index]++;
                                
                    if (error[n_line][index] < 10)
                        // Discard
                        return;
                }
            }
        }

        error[n_line][index] = 0;
        
        last_value_encoder[n_line][index] = tmp_value_encoder_32;

        value_encoder = (int32)tmp_value_encoder_32;   // SSSSSSSSSSSSSSSS[16] XXXXXXXXXXXX[12] 0000[4]

        value_encoder += ((int32)g_meas[n_line].rot[index] << 16);    
        
        if (c_mem.enc[n_line].m_mult[index] != 1.0) {
            value_encoder *= c_mem.enc[n_line].m_mult[index];
        }

        if (c_mem.dev.dev_type == SOFTHAND_PRO) {
            // Right / Left hand turn
            if (index == 0) {
                if (c_mem.dev.right_left == RIGHT_HAND){
                    value_encoder *= -1;        
                }
            }
        }
 

        g_meas[n_line].pos[index] = value_encoder;
    
        speed_encoder = (int16)filter((11*value_encoder - 18* v_value[n_line][index] + 9 * vv_value[n_line][index] -2 * vvv_value[n_line][index]), &filt_vel[index]);

        //Update current speed
        speed_encoder = speed_encoder / (6*cycle_time);
        g_meas[n_line].vel[index] = speed_encoder;

        // update old velocity and acceleration values
        vvv_value[n_line][index] = vv_value[n_line][index];
        vv_value[n_line][index] = v_value[n_line][index];
        v_value[n_line][index] = value_encoder;

        // wait at least 3 * max_num_of_error (10) + 5 = 35 cycles to reconstruct the right turn
        if (pos_reconstruct[n_line] == FALSE){
            if (one_time_execute[n_line] < 34) 
                one_time_execute[n_line]++;
            else {
                //Double encoder translation
                if (c_mem.enc[n_line].double_encoder_on_off){
                    init_rot = calc_turns_fcn(comp_value_encoder[n_line][0],comp_value_encoder[n_line][1], 
                                c_mem.enc[n_line].gears_params[0], c_mem.enc[n_line].gears_params[1], c_mem.enc[n_line].gears_params[2]);

                    if (c_mem.enc[n_line].m_mult[0] < 0)
                        init_rot = -init_rot;
                    
                    g_meas[n_line].rot[0] = (int8)init_rot;
                }    

                if (c_mem.enc[n_line].m_mult[0] != 1.0)
                    g_meas[n_line].pos[0] /= c_mem.enc[n_line].m_mult[0];
                
                g_meas[n_line].pos[0] += (int32)(init_rot << 16);
            
                if (c_mem.enc[n_line].m_mult[0] != 1.0)
                    g_meas[n_line].pos[0] *= c_mem.enc[n_line].m_mult[0];
            
                g_refNew[assoc_motor].pos = g_meas[n_line].pos[0];

                // If necessary activate motor
    			safe_startup_motor_activation[n_line] = TRUE;
                // Activate the motor associated to this encoder line
                g_refNew[assoc_motor].onoff = c_mem.motor[assoc_motor].activ;
                enable_motor(assoc_motor, g_refNew[assoc_motor].onoff);                
                
                pos_reconstruct[n_line] = TRUE;
            }
        }
    } 

	
	// Wait for 35+SAFE_STARTUP_MOTOR_READINGS cycles before starting motors
    if (safe_startup_motor_activation[n_line]){
        count_startup_motor[n_line]++;
        if (count_startup_motor[n_line] >= (uint32)SAFE_STARTUP_MOTOR_READINGS) {                            
            g_refNew[assoc_motor].pos = 0;
            
            // Activate the motor associated to this encoder line
            g_refNew[assoc_motor].onoff = c_mem.motor[assoc_motor].activ;
            enable_motor(assoc_motor, g_refNew[assoc_motor].onoff); 
                        
            safe_startup_motor_activation[n_line] = FALSE;
        }
    }
 
}

//==============================================================================
//                                                           ANALOG MEASUREMENTS
//==============================================================================

void analog_read_end() {

    /* =========================================================================
    /   Ideal formulation to calculate tension and current
    /
    /   tension = ((read_value_mV - offset) * 101) / gain -> [mV]
    /   current = ((read_value_mV - offset) * 375) / (gain * resistor) -> [mA]
    /
    /   Definition:
    /   read_value_mV = counts_read / 0.819 -> conversion from counts to mV
    /   offset = 2000 -> hardware amplifier bias in mV
    /   gain = 8.086 -> amplifier gain
    /   resistor = 18 -> resistor of voltage divider in KOhm 
    /
    /   Real formulation: tradeoff in good performance and accurancy, ADC_buf[] 
    /   and offset unit of measurement is counts, instead dev_tension and
    /   g_meas.curr[] are converted in properly unit.
    /  =========================================================================
    */

    int32 CYDATA i_aux;

    static uint16 emg_counter_1 = 0;
    static uint16 emg_counter_2 = 0;
	static uint8 first_tension_valid = TRUE;
    //static int32 pow_tension = 12000;       //12000 mV (12 V)
    static uint16 count = 0;
    static uint32 v_count = 0;
    static uint8 idx = 0;
    
    // Wait for conversion end
    
    while(!ADC_STATUS_Read()){
        
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
        }
    }
    
#ifdef AIR_CHAMBERS_FB_FW
    if (c_mem.dev.dev_type == AIR_CHAMBERS_FB){
        // Read pressure in any case
        g_fb_meas.pressure  = (int32)(ADC_buf[0] -1540);    //0 - 4096  
        g_fb_meas.pressure = (((float)g_fb_meas.pressure/4096.0)/0.004);       // datasheet transfer function ticks->kPa
        if (g_fb_meas.pressure < 0) g_fb_meas.pressure = 0;
    }
#endif
 
    // Convert tension read
    if (g_mem.dev.dev_type != AIR_CHAMBERS_FB){
        dev_tension[0] = 5000;
    }
    else {
        dev_tension[0] = ((int32)(ADC_buf[0] - 1621) * 1990) >> 7;
    }
    
    // Read also 2nd power tension (if necessary)
    if (NUM_OF_ANALOG_INPUTS > 4) {
        dev_tension[1] = ((int32)(ADC_buf[4] - 1621) * 1990) >> 7;
    }
    
    if (interrupt_flag){
        interrupt_flag = FALSE;                                                                                                                                                                                                                                                                                                                                                                                                                                                 
        interrupt_manager();
    }
    
    // VOLTAGE READING
    // Once firmware starts, first_tension_valid flag is set to TRUE while tension_valid status is FALSE
    // Step 1) Wait for battery voltage stabilization and filter convergence for 1000 cycles (v_count counter), after that tension_valid flag is set to TRUE
    // Step 2) Wait for another 1000 cycles (count counter) to decide which is full charge power tension, than set first_tension_valid to FALSE
    // Low voltage condition) Whenever dev_tension ADC value is under 7000 mV, tension_valid flag is set to FALSE and dev_tension assigned to 5000 mV, then Step 1 has to be repeated
    
	if (first_tension_valid && tension_valid) {     // Voltage reading (Step 2)
        count = count + 1;
        
        if (count == 1000){
            for (idx = 0; idx < NUM_OF_MOTORS; idx++) {
                if (dev_tension[idx] < 9000) {   // 8 V case
                    pow_tension[idx] = 8000;
                }
                else {      // 12 V - 24 V cases
                    if (dev_tension[idx] < 13000) {
                        pow_tension[idx] = 12000;
                    }
                    else
                        pow_tension[idx] = 24000;
                }
            }
            first_tension_valid = FALSE;
        }
    }

    // Until there is no valid input tension repeat this measurement

    if (dev_tension[0] < 7000 && (NUM_OF_ANALOG_INPUTS<=4 || dev_tension[1] < 7000)) {       // Voltage reading (Low voltage condition)
        // PSoC is powered through uUSB
        
        tension_valid = FALSE;
            
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
        }
        
        for (idx = 0; idx < NUM_OF_MOTORS; idx++){
            if (c_mem.emg.emg_calibration_flag) {
                if ((c_mem.motor[idx].input_mode == INPUT_MODE_EMG_PROPORTIONAL) ||
                    (c_mem.motor[idx].input_mode == INPUT_MODE_EMG_INTEGRAL) ||
                    (c_mem.motor[idx].input_mode == INPUT_MODE_EMG_FCFS) ||
                    (c_mem.motor[idx].input_mode == INPUT_MODE_EMG_FCFS_ADV)) {
                    g_ref[idx].onoff = 0x00;
                    enable_motor(idx, g_ref[idx].onoff); 
                    
                }
            }

            // Assign dev_tension and reset current
            dev_tension[idx] = 5000;
            g_meas[g_mem.motor[idx].encoder_line].curr = 0;
        }
        
        // Reset emg
        for (idx = 0; idx < NUM_OF_INPUT_EMGS; idx++){
            g_emg_meas.emg[idx] = 0;
        }
        for (idx = 0; c_mem.exp.read_ADC_sensors_port_flag && idx < NUM_OF_ADDITIONAL_EMGS; idx++){
            g_emg_meas.add_emg[idx] = 0;
        }
        
    }
    else {
        // PSoC is powered through battery or power source
        // (at least > 7.36 V (92% of 8 V) that is minimum charge of smallest battery)
        
        // Read ADC sampled value of power source tension and motor current
        
        if (v_count == 1000 && !tension_valid){     // Voltage reading (Step 1)
            // After 1000 v_count cycles, dev_tension_f is stable
            tension_valid = TRUE;   
            count = 0;
            v_count = 0;
        }
        else {  
            // wait for battery voltage stabilization
            if (v_count < 1000) {
                v_count = v_count + 1;
            }
            for(idx = 0; idx < NUM_OF_MOTORS; idx++) {
                dev_tension_f[idx] = filter(dev_tension[idx], &filt_v[idx]);
            }
        }

        for (idx = 0; idx < NUM_OF_MOTORS; idx++){
            if(g_mem.motor[idx].activate_pwm_rescaling)        //pwm rescaling is activated
                pwm_limit_search(idx);                 //only for 12V motor
        }
        
        // Filter and Set currents
        if (g_mem.motor[0].motor_driver_type == DRIVER_MC33887) { // [GS]
            g_meas[g_mem.motor[0].encoder_line].curr = filter((int16) (((int32)(ADC_buf[1] - 1648) * 22634) >> 13) * pwm_sign, &filt_i[0]);
        } else { // [GS]
            g_meas[g_mem.motor[0].encoder_line].curr = ((int16) ((int32)((ADC_buf[1] - 1635) * 480) >> 4) * pwm_sign); // filter((int16) (((int32)(ADC_buf[1] - 1648) * 43125) >> 13) * pwm_sign, &filt_i[0]); // [GS]
        }  

        // Calculate current estimation and put it in the second part of the current measurement array;
		g_meas[g_mem.motor[0].encoder_line].estim_curr = (int16) filter(((int32) g_meas[g_mem.motor[0].encoder_line].curr) - curr_estim(0, g_meas[g_mem.motor[0].encoder_line].pos[0] >> g_mem.enc[g_mem.motor[0].encoder_line].res[0], g_meas[g_mem.motor[0].encoder_line].vel[0] >> g_mem.enc[g_mem.motor[0].encoder_line].res[0], g_ref[0].pos >> g_mem.enc[g_mem.motor[0].encoder_line].res[0]), &filt_curr_diff[0]);
        
        // Read also 2nd power current (if necessary)
        if (NUM_OF_ANALOG_INPUTS > 4) {
            // Filter and Set currents
            if (g_mem.motor[1].motor_driver_type == DRIVER_MC33887) { // [GS]
                g_meas[g_mem.motor[1].encoder_line].curr = filter((int16) (((int32)(ADC_buf[5] - 1648) * 22634) >> 13) * pwm_sign, &filt_i[1]);
            } else { // [GS]
                g_meas[g_mem.motor[1].encoder_line].curr = ((int16) ((int32)((ADC_buf[5] - 1635) * 480) >> 4) * pwm_sign); // filter((int16) (((int32)(ADC_buf[1] - 1648) * 43125) >> 13) * pwm_sign, &filt_i[0]); // [GS]
            }  
            //g_meas[g_mem.motor[1].encoder_line].curr = filter((int16) (((int32)(ADC_buf[5] - 1648) * 22634) >> 13) * pwm_sign, &filt_i[1]);
            
            g_meas[g_mem.motor[1].encoder_line].estim_curr = (int16) filter(((int32) g_meas[g_mem.motor[1].encoder_line].curr) - curr_estim(1, g_meas[g_mem.motor[1].encoder_line].pos[0] >> g_mem.enc[g_mem.motor[1].encoder_line].res[0], g_meas[g_mem.motor[1].encoder_line].vel[0] >> g_mem.enc[g_mem.motor[1].encoder_line].res[0], g_ref[1].pos >> g_mem.enc[g_mem.motor[1].encoder_line].res[0]), &filt_curr_diff[1]);
        }
    
        // Check Interrupt 
    
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
        }

    }
    
    // Read EMG (always even if the board is not powered)
        
    // if calibration is not needed go to "normal execution"
    if (!c_mem.emg.emg_calibration_flag){
        emg_1_status = NORMAL; // normal execution
        emg_2_status = NORMAL; // normal execution
    }

    // EMG 1 calibration state machine
   
    // Calibration state machine
    switch(emg_1_status) {
        case NORMAL: // normal execution
            
            if (g_mem.dev.dev_type != AIR_CHAMBERS_FB){
                i_aux = ((int32)(ADC_buf[2 + c_mem.emg.switch_emg] - 1639) * 87) >> 5;  //map range to 0-4096
            }
            else {  // Use additional ADC channels, so the signal does not pass through AVAGO isolators
                i_aux = (int32)(ADC_buf[2 + c_mem.emg.switch_emg]);
            }
            
            if (i_aux < 0) 
                i_aux = 0;
            i_aux = filter(i_aux, &filt_emg[0]);
            i_aux = (i_aux << 10) / g_mem.emg.emg_max_value[0];

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            //Saturation
            if (i_aux < 0)
                i_aux = 0;
            else 
                if (i_aux > 1024) 
                    i_aux = 1024;
            
            g_emg_meas.emg[0] = i_aux;

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            
            break;

        case RESET: // reset variables
            emg_counter_1 = 0;
            g_mem.emg.emg_max_value[0] = 0;
            emg_1_status = DISCARD; // goto next status  
            break;

        case DISCARD: // discard first EMG_SAMPLE_TO_DISCARD samples
            emg_counter_1++;
            if (emg_counter_1 == EMG_SAMPLE_TO_DISCARD) {
                emg_counter_1 = 0;          // reset counter
                LED_control(1);
				
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                emg_1_status = SUM_AND_MEAN;    // sum and mean status
            }
            break;

        case SUM_AND_MEAN: // sum first SAMPLES_FOR_EMG_MEAN samples
            // NOTE max(value)*SAMPLES_FOR_EMG_MEAN must fit in 32bit
            emg_counter_1++;
            if (ADC_buf[2 + c_mem.emg.switch_emg] < 0) 
                ADC_buf[2 + c_mem.emg.switch_emg] = 0;
            g_mem.emg.emg_max_value[0] += filter((int32)ADC_buf[2 + c_mem.emg.switch_emg], &filt_emg[0]);
            
            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            
            if (emg_counter_1 == SAMPLES_FOR_EMG_MEAN) {
                g_mem.emg.emg_max_value[0] = g_mem.emg.emg_max_value[0] / SAMPLES_FOR_EMG_MEAN; // calc mean

                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }                    
                
                LED_control(0);
				
                emg_counter_1 = 0;          // reset counter

                emg_1_status = NORMAL;      // goto normal execution
            }
            break;

        default:
            break;
    }

    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }
    // EMG 2 calibration state machine
    switch(emg_2_status) {
        case NORMAL: // normal execution
        
            if (g_mem.dev.dev_type != AIR_CHAMBERS_FB){
                i_aux = ((int32)(ADC_buf[3 - c_mem.emg.switch_emg] - 1639) * 87) >> 5;  //map range to 0-4096
            }
            else {  // Use additional ADC channels, so the signal does not pass through AVAGO isolators
                i_aux = (int32)(ADC_buf[3 - c_mem.emg.switch_emg]);
            }

            if (i_aux < 0)
                i_aux = 0;
            i_aux = filter(i_aux, &filt_emg[1]);
            i_aux = (i_aux << 10) / g_mem.emg.emg_max_value[1];

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            
            if (i_aux < 0) 
                i_aux = 0;
            else 
                if (i_aux > 1024)
                    i_aux = 1024;
            
            g_emg_meas.emg[1] = i_aux;

            break;

        case RESET: // reset variables
            emg_counter_2 = 0;
            g_mem.emg.emg_max_value[1] = 0;

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            
            emg_2_status = WAIT; // wait for EMG 1 calibration
            break;

        case DISCARD: // discard first EMG_SAMPLE_TO_DISCARD samples
            emg_counter_2++;
            if (emg_counter_2 == EMG_SAMPLE_TO_DISCARD) {
                emg_counter_2 = 0;          // reset counter
                LED_control(1);

                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                emg_2_status = SUM_AND_MEAN;           // sum and mean status
            }
            break;

        case SUM_AND_MEAN: // sum first SAMPLES_FOR_EMG_MEAN samples
            // NOTE max(value)*SAMPLES_FOR_EMG_MEAN must fit in 32bit
            emg_counter_2++;
            if (ADC_buf[3 - c_mem.emg.switch_emg] < 0)
                ADC_buf[3 - c_mem.emg.switch_emg] = 0;
            g_mem.emg.emg_max_value[1] += filter((int32)ADC_buf[3 - c_mem.emg.switch_emg], &filt_emg[1]);

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            
            if (emg_counter_2 == SAMPLES_FOR_EMG_MEAN) {
                g_mem.emg.emg_max_value[1] = g_mem.emg.emg_max_value[1] / SAMPLES_FOR_EMG_MEAN; // calc mean
                LED_control(0);
                emg_counter_2 = 0;          // reset counter
            
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                emg_2_status = WAIT_EoC;    // goto end of calibration wait
            }
            break;

        case WAIT: // wait for EMG calibration to be done
            if (emg_1_status == NORMAL)
                emg_2_status = DISCARD;           // goto discard sample
            break;

        case WAIT_EoC:  // wait for end of calibration procedure (only for LED visibility reasons)
            emg_counter_2++;
            if (emg_counter_2 == EMG_SAMPLE_TO_DISCARD) {
                emg_counter_2 = 0;          // reset counter
                
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                for (idx = 0; idx < NUM_OF_MOTORS; idx++) {
                    if ((c_mem.motor[idx].input_mode == INPUT_MODE_EMG_PROPORTIONAL) ||
                        (c_mem.motor[idx].input_mode == INPUT_MODE_EMG_INTEGRAL) ||
                        (c_mem.motor[idx].input_mode == INPUT_MODE_EMG_FCFS) ||
                        (c_mem.motor[idx].input_mode == INPUT_MODE_EMG_FCFS_ADV)) {
                        if (c_mem.motor[idx].control_mode == CONTROL_ANGLE) {
                            g_ref[idx].pos = g_meas[g_mem.motor[idx].encoder_line].pos[0];
                        }
                        g_ref[idx].onoff = c_mem.motor[idx].activ;
                        enable_motor(idx, g_ref[idx].onoff); 
                    }
                }
                    
                memStore(0);        // Store emg_max_value
                    
                emg_2_status = NORMAL;           // goto normal execution
            }
            break;
        default:
            break;
    }
        
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }
    
    // Read also EMG additional sensors port
    if (NUM_OF_ANALOG_INPUTS > 6) {
        
        for (idx = 0; idx < NUM_OF_ADDITIONAL_EMGS; idx++){
            
            i_aux = (int32)(ADC_buf[6 + idx]);
            if (i_aux < 0) 
                i_aux = 0;
            i_aux = filter(i_aux, &filt_emg[2+idx]);
            i_aux = (i_aux << 10) / 1024;

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
            //Saturation
            if (i_aux < 0)
                i_aux = 0;
            else 
                if (i_aux > 1024) 
                    i_aux = 1024;
            
            g_emg_meas.add_emg[idx] = i_aux;

            if (interrupt_flag){
                interrupt_flag = FALSE;
                interrupt_manager();
            }
        }
    }
    
}

//==============================================================================
//                                                           OVERCURRENT CONTROL
//==============================================================================

void overcurrent_control() {
    
    for (uint8 i = 0; i <NUM_OF_MOTORS; i++) {
        if (c_mem.motor[i].current_limit != 0) {
            // if the current is over the limit
            if (g_meas[g_mem.motor[i].encoder_line].curr > c_mem.motor[i].current_limit) {
                //decrease pwm_limit
                dev_pwm_limit[i]--;
            // if the current is in the safe zone
            } else if (g_meas[i].curr < (c_mem.motor[i].current_limit - CURRENT_HYSTERESIS)) {
                //increase pwm_limit
                dev_pwm_limit[i]++;
            }

            // Bound pwm_limit
            if (dev_pwm_limit[i] < 0) 
                dev_pwm_limit[i] = 0;
            else 
                if (dev_pwm_limit[i] > dev_pwm_sat[i]) 
                    dev_pwm_limit[i] = dev_pwm_sat[i];
        }
    }
}

//==============================================================================
//                                                              PWM_LIMIT_SEARCH
//==============================================================================

void pwm_limit_search(uint8 mot_idx) {

    uint8 CYDATA index;
    uint16 CYDATA max_tension = 25500;
    uint16 CYDATA min_tension = 11500;
    
    if (dev_tension[mot_idx] > max_tension) {
        dev_pwm_sat[mot_idx] = 0;
    } else if (dev_tension[mot_idx] < min_tension) {
        dev_pwm_sat[mot_idx] = 100;
    } else {
        index = (uint8)((dev_tension[mot_idx] - min_tension) >> 9);
        dev_pwm_sat[mot_idx] = pwm_preload_values[index];
    }
}

//==============================================================================
//                                                         CYCLES COUNTER UPDATE
//==============================================================================

void cycles_counter_update() {
	static uint8 pos_cycle_status = STATE_INACTIVE;
    static uint8 emg_cycle_status[2] = {STATE_INACTIVE, STATE_INACTIVE};
    static uint8 rest_cycle_status = STATE_INACTIVE;
    static int32 bin_threshold = 250;
    static int32 thr_pos = 0, max_pos = 0;
    uint8 i, bin_st, bin_max, bin_1, bin_2;
    int32 curr_pos, curr_off, curr_ref;
    int32 step;
    static uint32 timer_value_s, timer_value_e;

    curr_pos = (int32)(g_meas[g_mem.motor[0].encoder_line].pos[0] >> g_mem.enc[g_mem.motor[0].encoder_line].res[0]);
    
    // State machine - Evaluate position counter update
    switch (pos_cycle_status){
        case STATE_INACTIVE:
            if ((g_mem.motor[0].input_mode != INPUT_MODE_EMG_PROPORTIONAL_NC && pwm_sign == 1) || (g_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL_NC && pwm_sign == -1)){
                thr_pos = curr_pos; 
                curr_off = (max_pos>thr_pos)?(max_pos-thr_pos):(thr_pos-max_pos);
                g_mem.cnt.wire_disp = g_mem.cnt.wire_disp + curr_off;     //sum opening track
                pos_cycle_status = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            if ((g_mem.motor[0].input_mode != INPUT_MODE_EMG_PROPORTIONAL_NC && pwm_sign == -1) || (g_mem.motor[0].input_mode == INPUT_MODE_EMG_PROPORTIONAL_NC && pwm_sign == 1)){
                max_pos = curr_pos;
                curr_off = (max_pos>thr_pos)?(max_pos-thr_pos):(thr_pos-max_pos);
                g_mem.cnt.wire_disp = g_mem.cnt.wire_disp + curr_off;     //sum closure track
                pos_cycle_status = COUNTER_INC;
            }
            break;
        case COUNTER_INC:

            curr_off = (max_pos>thr_pos)?(max_pos-thr_pos):(thr_pos-max_pos);
            if (curr_off > bin_threshold){
                //update position histogram
                step = ((int32)(g_mem.motor[0].pos_lim_sup >> g_mem.enc[g_mem.motor[0].encoder_line].res[0]) / 10);
                bin_st  = (uint8)(thr_pos/step);
                bin_max = (uint8)(max_pos/step);
                
                // Bin computation valid for both NO and NC working
                bin_1 = (bin_st<bin_max)?bin_st:bin_max;
                bin_2 = (bin_st<bin_max)?bin_max:bin_st;    
                for (i=bin_1; i<= bin_2; i++){
                    //position_hist counts how many times the SoftHand stays in bin while moving
                    g_mem.cnt.position_hist[i] = g_mem.cnt.position_hist[i] + 1;
                }
                
                //update current histogram (only positive current measures)
                step = ((int32)(g_mem.motor[0].current_limit) / 4);
                if (g_mem.motor[0].not_revers_motor_flag == TRUE)
                    curr_ref = g_meas[g_mem.motor[0].encoder_line].hold_curr; 
                else
                    curr_ref = g_meas[g_mem.motor[0].encoder_line].curr;
                    
                if (curr_ref > g_mem.motor[0].current_limit)
                    g_mem.cnt.current_hist[3] = g_mem.cnt.current_hist[3] + 1; 
                else
                    g_mem.cnt.current_hist[(uint8)(curr_ref/step)] = g_mem.cnt.current_hist[(uint8)(curr_ref/step)] + 1;
            }
            pos_cycle_status = STATE_INACTIVE;
            break;
    }
    
    // State machine - Evaluate EMG counter update
    for (i=0; i<2 && emg_1_status == NORMAL && emg_2_status == NORMAL; i++){
        switch (emg_cycle_status[i]){
            case STATE_INACTIVE:
                if (g_emg_meas.emg[i] > g_mem.emg.emg_threshold[i]){
                    emg_cycle_status[i] = STATE_ACTIVE;
                }
                break;
            case STATE_ACTIVE:
                if (g_emg_meas.emg[i] < g_mem.emg.emg_threshold[i]-10){
                    emg_cycle_status[i] = COUNTER_INC;
                }
                break;
            case COUNTER_INC:
                g_mem.cnt.emg_counter[i] = g_mem.cnt.emg_counter[i] + 1;
                emg_cycle_status[i] = STATE_INACTIVE;
                break;
        }
    }
    
    // State machine - Evaluate rest counter update
    switch (rest_cycle_status){
        case STATE_INACTIVE:
            if (rest_enabled){
                timer_value_s = (uint32)CYCLES_TIMER_ReadCounter();
                rest_cycle_status = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            if (!rest_enabled){
                timer_value_e = (uint32)CYCLES_TIMER_ReadCounter();
                if (timer_value_s < timer_value_e) {
                    timer_value_s = timer_value_s + (uint32)1200;
                }
                rest_cycle_status = COUNTER_INC;
            }
            break;
        case COUNTER_INC: 
            g_mem.cnt.total_time_rest = g_mem.cnt.total_time_rest + (uint32)((timer_value_s - timer_value_e)/10);
            g_mem.cnt.rest_counter = g_mem.cnt.rest_counter + 1;
            rest_cycle_status = STATE_INACTIVE;
            break;
    }
          
    // This function writes rows [row_start, row_end] on EEPROM
    save_cycles_eeprom();
    
}

//==============================================================================
//                                                            SAVE CYCLES EEPROM
//==============================================================================

void save_cycles_eeprom() {

    cystatus status;
    static uint8 row_number;
    uint8 row_start = 1;
    uint8* addr_start   = (uint8*)&g_mem.cnt.emg_counter[0];    //Data at beginning of the row 1
    uint8* addr_start_c = (uint8*)&c_mem.cnt.emg_counter[0];
    uint8 row_end   = row_start + EEPROM_COUNTERS_ROWS - 1;
    static uint8* m_addr = NULL; 

    // This part of code writes rows [row_start, row_end] on EEPROM    
    switch(cycles_status) {
        case PREPARE_DATA:
            // Store data in c_mem structure to have consistent counters
            memcpy( addr_start_c, addr_start, EEPROM_BYTES_ROW*EEPROM_COUNTERS_ROWS );            
            m_addr = addr_start;
            row_number = row_start;
            cycles_status = WRITE_CYCLES;
            break;
            
        case WRITE_CYCLES:
            EEPROM_UpdateTemperature();     //Check temperature of chip before writing
            status = EEPROM_StartWrite((uint8*) m_addr, row_number);           
            if(status == CYRET_STARTED || status == CYRET_SUCCESS) {
                cycles_status = WAIT_QUERY;
                can_write = FALSE;
            }
            break;
            
        case WAIT_QUERY:
            status = EEPROM_Query();
            if(status == CYRET_SUCCESS) {
                if (row_number < row_end) {
                    m_addr = m_addr + EEPROM_BYTES_ROW;
                    row_number = row_number + 1;
                    cycles_status = WRITE_CYCLES;
                }
                else {
                    cycles_status = WRITE_END;
                }
            }
            break;
           
        case WRITE_END:
            can_write = TRUE;            
            cycles_status = NONE;
            
            // Restore data saved in c_mem structure
            memcpy( addr_start, addr_start_c, EEPROM_BYTES_ROW*EEPROM_COUNTERS_ROWS );
            break;
            
        case NONE:
            break;
    }
    
}
/* [] END OF FILE */