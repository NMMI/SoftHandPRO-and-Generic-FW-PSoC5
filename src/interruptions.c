// ----------------------------------------------------------------------------
// BSD 3-Clause License

// Copyright (c) 2016, qbrobotics
// Copyright (c) 2017, Centro "E.Piaggio"
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
* \date         October 01, 2017
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017 Centro "E.Piaggio". All rights reserved.
*/


//=================================================================     includes
#include "interruptions.h"
#include "command_processing.h"
#include "SPI_functions.h"
#include "globals.h"
#include "utils.h"


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
    
    // Get data until buffer is not empty 
    
    while(UART_RS485_GetRxBufferSize() && (package_count < 6)){
        // 6 - estimated maximum number of packets we can read without blocking firmware execution
        
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
                if (rx_data == c_mem.id || rx_data == 0)
                    rx_data_type = FALSE;
                else                //packet is for others
                    rx_data_type = TRUE;
                
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
    CYBIT write_cycles = FALSE;
    timer_value0 = (uint32)MY_TIMER_ReadCounter();
    uint8 rtc_data;
    char info_[2500] = "";

    // Start ADC Conversion, SOC = 1

    ADC_SOC_Write(0x01); 
    
    // Check Interrupt 

    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
        write_cycles = TRUE;
    }

    //---------------------------------- Get Encoders

    encoder_reading_SPI(); 
    
    // Check Interrupt     
    
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
        write_cycles = TRUE;
    }

    //---------------------------------- Control Motor
   
    motor_control();

    // Check Interrupt 

    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
        write_cycles = TRUE;
    }

    //---------------------------------- Read conversion buffer - LOCK function

    analog_read_end();

    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
        write_cycles = TRUE;
    }

    //---------------------------------- Control Overcurrent

    overcurrent_control();
    
    // Check Interrupt 
    
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
        write_cycles = TRUE;
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
        write_cycles = TRUE;
    }
    
     
    //---------------------------------- Rest position check
    
    // Divider 10, freq = 500 Hz
    if (c_mem.rest_position_flag == TRUE){
        if (counter_calibration == CALIBRATION_DIV) {
                check_rest_position();
                counter_calibration = 0;
        }
        counter_calibration++;

        // Check Interrupt     
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
			write_cycles = TRUE;
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
            g_mem.total_time_on = g_mem.total_time_on + 120;  // Add 120 seconds.
            
            rtc_data = DS1302_read(DS1302_DATE_RD);
            g_mem.curr_time[0] = (rtc_data/16) * 10 + rtc_data%16;
            rtc_data = DS1302_read(DS1302_MONTH_RD);
            g_mem.curr_time[1] = (rtc_data/16) * 10 + rtc_data%16;
            rtc_data = DS1302_read(DS1302_YEAR_RD);
            g_mem.curr_time[2] = (rtc_data/16) * 10 + rtc_data%16;

            rtc_data = DS1302_read(DS1302_HOUR_RD);
            g_mem.curr_time[3] = (rtc_data/16) * 10 + rtc_data%16;
            rtc_data = DS1302_read(DS1302_MINUTES_RD);
            g_mem.curr_time[4] = (rtc_data/16) * 10 + rtc_data%16;
            rtc_data = DS1302_read(DS1302_SECONDS_RD);
            g_mem.curr_time[5] = (rtc_data/16) * 10 + rtc_data%16;
            
            //Write in SD card
            prepare_counter_info(info_);
            write_bytes = FS_Write(pFile, info_, strlen(info_));
            if (write_bytes == 0) {
                g_refNew.onoff = 0x00;
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
        write_cycles = TRUE;
    }
   
    //---------------------------------- Read IMUs
    ReadAllIMUs();      // IMU reading is atomic, no RS485 request is handled
        
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
    }
    //---------------------------------- Update States
    
    // Load k-1 state
    memcpy( &g_measOld, &g_meas, sizeof(g_meas) );
    memcpy( &g_refOld, &g_ref, sizeof(g_ref) );

    // Load k+1 state
    memcpy( &g_ref, &g_refNew, sizeof(g_ref) );
    memcpy( &g_imu, &g_imuNew, sizeof(g_imu) );
                
    if (interrupt_flag){
        interrupt_flag = FALSE;
        interrupt_manager();
        write_cycles = TRUE;
    }

    timer_value = (uint32)MY_TIMER_ReadCounter();
    cycle_time = (float)((timer_value0 - timer_value)/1000000.0);
    MY_TIMER_WriteCounter(5000000);

}

//==============================================================================
//                                                                 MOTOR CONTROL
//==============================================================================

void motor_control() {

    int32 CYDATA pwm_input = 0;

    int32 CYDATA pos_error;         // position error
    int32 CYDATA curr_error;        // current error

    int32 CYDATA i_ref;             // current reference
    int32 CYDATA handle_value;

    int32 CYDATA err_emg_1, err_emg_2;

    int32 CYDATA k_p = c_mem.k_p;  
    int32 CYDATA k_i = c_mem.k_i; 
    int32 CYDATA k_d = c_mem.k_d; 

    int32 CYDATA k_p_c = c_mem.k_p_c;  
    int32 CYDATA k_i_c = c_mem.k_i_c; 
    int32 CYDATA k_d_c = c_mem.k_d_c;   

    int32 CYDATA k_p_dl = c_mem.k_p_dl;  
    int32 CYDATA k_i_dl = c_mem.k_i_dl; 
    int32 CYDATA k_d_dl = c_mem.k_d_dl;
    
    int32 CYDATA k_p_c_dl = c_mem.k_p_c_dl;  
    int32 CYDATA k_i_c_dl = c_mem.k_i_c_dl; 
    int32 CYDATA k_d_c_dl = c_mem.k_d_c_dl;   

    // Static Variables

    static int32 pos_error_sum;     // position error sum for integral
    static int32 curr_error_sum;    // current error sum for integral
    
    static int32 prev_pos_err;      // previous position error for deriv. control
    static int32 prev_curr_err;     // previous current error for deriv. control

    static CYBIT motor_dir = FALSE;

    static uint8 current_emg = 0;   // 0 NONE
                                    // 1 EMG 1
                                    // 2 EMG 2
                                    // wait for both to get down

    err_emg_1 = g_meas.emg[0] - c_mem.emg_threshold[0];
    err_emg_2 = g_meas.emg[1] - c_mem.emg_threshold[1];

    // =========================== POSITION INPUT ==============================
    switch(c_mem.input_mode) {

        case INPUT_MODE_ENCODER3:

            // Calculate handle value based on position of handle in the
            // sensor chain and multiplication factor between handle and motor position
            if (c_mem.double_encoder_on_off) 
                handle_value = g_meas.pos[2] * c_mem.motor_handle_ratio;
            else
                handle_value = g_meas.pos[1] * c_mem.motor_handle_ratio;
            

            // Read handle and use it as reference for the motor
            if (((handle_value - g_refOld.pos) > c_mem.max_step_pos) && (c_mem.max_step_pos != 0))
                g_ref.pos += c_mem.max_step_pos;
            else {
                if (((handle_value - g_refOld.pos) < c_mem.max_step_neg) && (c_mem.max_step_neg != 0))
                    g_ref.pos += c_mem.max_step_neg;
                else
                    g_ref.pos = handle_value;
            }
            break;

        case INPUT_MODE_EMG_PROPORTIONAL:
            if (err_emg_1 > 0)
                g_ref.pos = (err_emg_1 * g_mem.pos_lim_sup) / (1024 - c_mem.emg_threshold[0]);
            else
                g_ref.pos = 0;
            break;

        case INPUT_MODE_EMG_INTEGRAL:
            g_ref.pos = g_refOld.pos;
            if (err_emg_1 > 0) {
                g_ref.pos = g_refOld.pos + (err_emg_1 * (int)g_mem.emg_speed * 2) / (1024 - c_mem.emg_threshold[0]);
            }
            if (err_emg_2 > 0) {
                g_ref.pos = g_refOld.pos - (err_emg_2 * (int)g_mem.emg_speed * 2) / (1024 - c_mem.emg_threshold[1]);
            }
            break;

        case INPUT_MODE_EMG_FCFS:
            g_ref.pos = g_refOld.pos;
            switch (current_emg) {
                case 0:
                    // Look for the first EMG passing the threshold
                    if (err_emg_1 > 0) {
                        current_emg = 1;
                        break;
                    }
                    if (err_emg_2 > 0) {
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
                    g_ref.pos = g_refOld.pos + (err_emg_1 * g_mem.emg_speed * 2) / (1024 - c_mem.emg_threshold[0]);
                    break;

                case 2:
                    // EMG 2 is first
                    if (err_emg_2 < 0) {
                        current_emg = 0;
                        break;
                    }
                    g_ref.pos = g_refOld.pos - (err_emg_2 * g_mem.emg_speed * 2) / (1024 - c_mem.emg_threshold[1]);
                    break;

                default:
                    break;
            }

            break;

        case INPUT_MODE_EMG_FCFS_ADV:
            g_ref.pos = g_refOld.pos;
            switch (current_emg) {
                // Look for the first EMG passing the threshold
                case 0:
                    if (err_emg_1 > 0) {
                        current_emg = 1;
                        break;
                    }
                    if (err_emg_2 > 0) {
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
                        g_ref.pos = g_refOld.pos + (err_emg_1 * g_mem.emg_speed << 2) / (1024 - c_mem.emg_threshold[0]);
                    
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
                        g_ref.pos = g_refOld.pos - (err_emg_2 * c_mem.emg_speed << 2) / (1024 - c_mem.emg_threshold[1]);
                    }
                    break;

                default:
                    break;
            }

        default:
            break;
    }

    // Position limit saturation
    if (c_mem.pos_lim_flag) {
        if (g_ref.pos < c_mem.pos_lim_inf) 
            g_ref.pos = c_mem.pos_lim_inf;
        if (g_ref.pos > c_mem.pos_lim_sup) 
            g_ref.pos = c_mem.pos_lim_sup;
    }
    
    if (battery_low_SoC == 1) {
        //Reopen the terminal device before disabling motor
        g_ref.pos = 0;
    }
        
    if (c_mem.rest_position_flag == TRUE) {
        if (rest_enabled == 1){
            // Change position reference to drive motor to rest position smoothly
            g_ref.pos = rest_pos_curr_ref;
        }
        
        if (forced_open == 1) {
            // Open the SoftHand as EMG PROPORTIONAL input mode 
            if (err_emg_2 > 0)
                g_ref.pos = g_mem.rest_pos - (err_emg_2 * g_mem.rest_pos) / (1024 - c_mem.emg_threshold[1]);
            else {
                g_ref.pos = g_mem.rest_pos;
                forced_open = 0;
            }
        }
    }

    switch(c_mem.control_mode) {
        // ======================= CURRENT AND POSITION CONTROL =======================
        case CURR_AND_POS_CONTROL:
            pos_error = g_ref.pos - g_meas.pos[0];

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
            if (i_ref > c_mem.current_limit)
                i_ref = c_mem.current_limit;
            else {
                if (i_ref < -c_mem.current_limit)
                    i_ref = -c_mem.current_limit;
        	}

            // current error and curr error sum
            curr_error = i_ref - g_meas.curr;
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
            pos_error = g_ref.pos - g_meas.pos[0];

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
            if(g_ref.onoff && tension_valid) {
                
                i_ref = g_ref.curr;

                if (i_ref > c_mem.current_limit) 
                    i_ref = c_mem.current_limit;
                else {
                    if (i_ref < -c_mem.current_limit)
                        i_ref = -c_mem.current_limit;
                }
                
                // current error
                curr_error = i_ref - g_meas.curr;            
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

            pwm_input = g_ref.pwm;

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

    if (c_mem.control_mode != CONTROL_PWM) 
        pwm_input = (((pwm_input << 10) / PWM_MAX_VALUE) * dev_pwm_limit) >> 10;
 
    pwm_sign = SIGN(pwm_input);

    if (motor_dir)
        MOTOR_DIR_Write(0x01);
    else
        MOTOR_DIR_Write(0x00);

    PWM_MOTORS_WriteCompare(abs(pwm_input));
}

//==============================================================================
//                                                           ENCODER READING SPI
//==============================================================================

void encoder_reading_SPI() {

    uint8 CYDATA index = 0;
    int32 aux_encoders[N_ENCODERS];
    
    // Encoder Variables  
    int jj = 0;
    int32 speed_encoder;
    static int32 previous_val;
    int32 aux;
	int32 init_rot = 0;

    static int32 last_value_encoder[N_ENCODERS];

    static uint8 error[N_ENCODERS];
    
    static CYBIT only_first_time = TRUE;

    static uint8 one_time_execute = 0;
    static CYBIT pos_reconstruct = FALSE;

    static int32 v_value[N_ENCODERS];   //last value for velocity
    static int32 vv_value[N_ENCODERS];  //last last value for velocity
    static int32 vvv_value[N_ENCODERS];  //last last last value for velocity
    
    if (reset_last_value_flag) {
        for (jj = N_ENCODERS; jj--;) 
            last_value_encoder[jj] = 0;
        
        reset_last_value_flag = 0;
    }

    //======================================================     reading sensors
    for (index = 0; index < N_ENCODERS; index++) {
        aux_encoders[index] = 0;
    }   
    
    encoder_read(&aux_encoders[0]);

    for (index = 0; index < N_ENCODERS; index++) {
        if ( (c_mem.double_encoder_on_off && index == 2) || (!c_mem.double_encoder_on_off && index >= 1) ) {
            // Handle measures
            aux_encoders[index] = (int16)aux_encoders[index] + g_mem.m_off[index];
        } 
        else {
            // Encoder measures has to be changed in sign due to mechanical chip positioning on SoftHand Pro
            aux_encoders[index] = (int16) -(aux_encoders[index]) + g_mem.m_off[index];
        }

        // Initialize last_value_encoder
        if (only_first_time) {
            last_value_encoder[index] = aux_encoders[index];
            if (index == 2)
                only_first_time = 0;
        }

        // Take care of rotations
        aux = aux_encoders[index] - last_value_encoder[index];

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

        if (aux > 49152)
            g_meas.rot[index]--;
        else{ 
            if (aux < -49152)
                g_meas.rot[index]++;
            else{
                if (abs(aux) > 16384) { // if two measure are too far
                    error[index]++;
                    if (error[index] < 10)
                        // Discard
                        return;
                }
            }
        }

        error[index] = 0;

        last_value_encoder[index] = aux_encoders[index];

        aux_encoders[index] += (int32)g_meas.rot[index] << 16;

        if (c_mem.m_mult[index] != 1.0) {
            aux_encoders[index] *= c_mem.m_mult[index];
        }
        
        previous_val = (g_meas.pos[0] >> g_mem.res[0]); 

        g_meas.pos[index] = aux_encoders[index];


        speed_encoder = (int16)filter((11*aux_encoders[index] - 18* v_value[index] + 9 * vv_value[index] -2 * vvv_value[index]), &filt_vel[index]);

        //Update current speed
        speed_encoder = speed_encoder / (6*cycle_time);
        g_meas.vel[index] = speed_encoder;

        // update old velocity and acceleration values
        vvv_value[index] = vv_value[index];
        vv_value[index] = v_value[index];
        v_value[index] = aux_encoders[index];

        // wait at least 3 * max_num_of_error (10) + 5 = 35 cycles to reconstruct the right turn
        if (pos_reconstruct == FALSE){
            if (one_time_execute < 34) 
                one_time_execute++;
            else {
                //Double encoder translation
                if (c_mem.double_encoder_on_off){

                    init_rot = calc_turns_fcn(g_meas.pos[0],g_meas.pos[1]);
                    
                    if (c_mem.m_mult[0] < 0)
                        init_rot = -init_rot;
                    
                    g_meas.rot[0] = (int8)init_rot;

                }

                if (c_mem.m_mult[0] != 1.0)
                    g_meas.pos[0] /= c_mem.m_mult[0];
                
                g_meas.pos[0] += (int32)(init_rot << 16);
                
                if (c_mem.m_mult[0] != 1)
                    g_meas.pos[0] *= c_mem.m_mult[0];
                        
                g_refNew.pos = g_meas.pos[0];

                // If necessary activate motor
                MOTOR_ON_OFF_Write(g_ref.onoff);
                
                pos_reconstruct = TRUE;
            }
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
    /   and offset unit of measurment is counts, instead dev_tension and
    /   g_meas.curr[] are converted in properly unit.
    /  =========================================================================
    */

    int32 CYDATA i_aux;

    static uint16 emg_counter_1 = 0;
    static uint16 emg_counter_2 = 0;
	static uint8 first_tension_valid = TRUE;
    static int32 pow_tension = 12000;       //12000 mV (12 V)
    static uint16 count = 0;
    static uint32 v_count = 0;
    
    // Wait for conversion end
    
    while(!ADC_STATUS_Read()){
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
        }
    }
    
    // Convert tension read
    dev_tension = ((int32)(ADC_buf[0] - 1621) * 1990) >> 7;
    
    if (interrupt_flag){
        interrupt_flag = FALSE;                                                                                                                                                                                                                                                                                                                                                                                                                                                 
        interrupt_manager();
    }
    
	if (first_tension_valid && tension_valid) {
        count = count + 1;
        
        if (count == 1000){
            if (dev_tension < 9000) {   // 8 V case
                pow_tension = 8000;
            }
            else {      // 12 V - 24 V cases
                if (dev_tension < 13000) {
                    pow_tension = 12000;
                }
                else
                    pow_tension = 24000;
            }

            first_tension_valid = FALSE;
        }
    }

    // Until there is no valid input tension repeat this measurement

    if (dev_tension < 7000) {
        // PSoC is powered through uUSB

        tension_valid = FALSE;
            
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
        }
        
        if (c_mem.emg_calibration_flag) {
            if ((c_mem.input_mode == INPUT_MODE_EMG_PROPORTIONAL) ||
                (c_mem.input_mode == INPUT_MODE_EMG_INTEGRAL) ||
                (c_mem.input_mode == INPUT_MODE_EMG_FCFS) ||
                (c_mem.input_mode == INPUT_MODE_EMG_FCFS_ADV)) {
                g_ref.onoff = 0x00;
                MOTOR_ON_OFF_Write(g_ref.onoff);
            }
        }
        
        // Assign dev_tension     
        dev_tension = 5000;

        // Reset current
        g_meas.curr = 0;

        // Reset emg
        g_meas.emg[0] = 0;
        g_meas.emg[1] = 0;
        
    }
    else {
        // PSoC is powered through battery or power source
        // (at least > 7.36 V (92% of 8 V) that is minimum charge of smallest battery)
        
        // Read ADC sampled value of power source tension and motor current
        
        if (v_count == 1000 && !tension_valid){
            // After 1000 v_count cycles, dev_tension_f is stable
            tension_valid = TRUE;   
            count = 0;
        }
        else {  
            // wait for battery voltage stabilization
            v_count = v_count + 1;
            dev_tension_f = filter(dev_tension, &filt_v);
        }

        if(g_mem.activate_pwm_rescaling)        //pwm rescaling is activated
            pwm_limit_search();                 //only for 12V motor

        // Filter and Set currents
        g_meas.curr = filter((int16) (((int32)(ADC_buf[1] - 1648) * 22634) >> 13) * pwm_sign, &filt_i);
        
		// Calculate current estimation and put it in the second part of the current measurement array;
		g_meas.estim_curr = (int16) filter(((int32) g_meas.curr) - curr_estim(g_meas.pos[0] >> g_mem.res[0], g_meas.vel[0] >> g_mem.res[0], g_ref.pos >> g_mem.res[0]), &filt_curr_diff);

        // Check Interrupt 
    
        if (interrupt_flag){
            interrupt_flag = FALSE;
            interrupt_manager();
        }

    }
    
    
    // Read EMG (if necessary)
    if ((c_mem.input_mode == INPUT_MODE_EMG_PROPORTIONAL) ||
        (c_mem.input_mode == INPUT_MODE_EMG_INTEGRAL) ||
        (c_mem.input_mode == INPUT_MODE_EMG_FCFS) ||
        (c_mem.input_mode == INPUT_MODE_EMG_FCFS_ADV)) {
        
        // if calibration is not needed go to "normal execution"
        if (!c_mem.emg_calibration_flag){
            emg_1_status = NORMAL; // normal execution
            emg_2_status = NORMAL; // normal execution
        }

        // EMG 1 calibration state machine
       
        // Calibration state machine
        switch(emg_1_status) {
            case NORMAL: // normal execution
                //i_aux = ((int32)(ADC_buf[2 + c_mem.switch_emg] - 1639) * 87) >> 5;  //map range to 0-4096
                i_aux = (int32)(ADC_buf[2 + c_mem.switch_emg]);
                if (i_aux < 0) 
                    i_aux = 0;
                i_aux = filter(i_aux, &filt_emg1);
                i_aux = (i_aux << 10) / g_mem.emg_max_value[0];
    
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
                
                g_meas.emg[0] = i_aux;
    
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                break;

            case RESET: // reset variables
                emg_counter_1 = 0;
                g_mem.emg_max_value[0] = 0;
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
                if (ADC_buf[2 + c_mem.switch_emg] < 0) 
                    ADC_buf[2 + c_mem.switch_emg] = 0;
                g_mem.emg_max_value[0] += filter((int32)ADC_buf[2 + c_mem.switch_emg], &filt_emg1);
                
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                if (emg_counter_1 == SAMPLES_FOR_EMG_MEAN) {
                    g_mem.emg_max_value[0] = g_mem.emg_max_value[0] / SAMPLES_FOR_EMG_MEAN; // calc mean
    
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
                //i_aux = ((int32)(ADC_buf[3 - c_mem.switch_emg] - 1640) * 87) >> 5;  //map range to 0-4096
                i_aux = (int32)(ADC_buf[3 - c_mem.switch_emg]);
                if (i_aux < 0)
                    i_aux = 0;
                i_aux = filter(i_aux, &filt_emg2);
                i_aux = (i_aux << 10) / g_mem.emg_max_value[1];
    
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                if (i_aux < 0) 
                    i_aux = 0;
                else 
                    if (i_aux > 1024)
                        i_aux = 1024;
                
                g_meas.emg[1] = i_aux;

                break;

            case RESET: // reset variables
                emg_counter_2 = 0;
                g_mem.emg_max_value[1] = 0;
    
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
                if (ADC_buf[3 - c_mem.switch_emg] < 0)
                    ADC_buf[3 - c_mem.switch_emg] = 0;
                g_mem.emg_max_value[1] += filter((int32)ADC_buf[3 - c_mem.switch_emg], &filt_emg2);
    
                if (interrupt_flag){
                    interrupt_flag = FALSE;
                    interrupt_manager();
                }
                
                if (emg_counter_2 == SAMPLES_FOR_EMG_MEAN) {
                    g_mem.emg_max_value[1] = g_mem.emg_max_value[1] / SAMPLES_FOR_EMG_MEAN; // calc mean
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
                    
                    // if EMG control mode active, activate motor accordingly with startup value
                    if ((c_mem.input_mode == INPUT_MODE_EMG_PROPORTIONAL) ||
                        (c_mem.input_mode == INPUT_MODE_EMG_INTEGRAL) ||
                        (c_mem.input_mode == INPUT_MODE_EMG_FCFS) ||
                        (c_mem.input_mode == INPUT_MODE_EMG_FCFS_ADV)) {
                        if (c_mem.control_mode == CONTROL_ANGLE) {
                            g_ref.pos = g_meas.pos[0];
                        }
                        g_ref.onoff = c_mem.activ;
                        MOTOR_ON_OFF_Write(g_ref.onoff);
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
    }
}

//==============================================================================
//                                                           OVERCURRENT CONTROL
//==============================================================================

void overcurrent_control() {
    if (c_mem.current_limit != 0) {
        // if the current is over the limit
        if (g_meas.curr > c_mem.current_limit) {
            //decrease pwm_limit
            dev_pwm_limit--;
        // if the current is in the safe zone
        } else if (g_meas.curr < (c_mem.current_limit - CURRENT_HYSTERESIS)) {
            //increase pwm_limit
            dev_pwm_limit++;
        }

        // Bound pwm_limit
        if (dev_pwm_limit < 0) 
            dev_pwm_limit = 0;
        else 
            if (dev_pwm_limit > dev_pwm_sat) 
                dev_pwm_limit = dev_pwm_sat;
    }
}

//==============================================================================
//                                                              PWM_LIMIT_SEARCH
//==============================================================================

void pwm_limit_search() {

    uint8 CYDATA index;
    uint16 CYDATA max_tension = 25500;
    uint16 CYDATA min_tension = 11500;
    
    if (dev_tension > max_tension) {
        dev_pwm_sat = 0;
    } else if (dev_tension < min_tension) {
        dev_pwm_sat = 100;
    } else {
        index = (uint8)((dev_tension - min_tension) >> 9);
        dev_pwm_sat = pwm_preload_values[index];
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
    uint8 i, bin_st, bin_max;
    int32 curr_pos;
    int32 step;
    static uint32 timer_value_s, timer_value_e;
    
    curr_pos = (int32)(g_meas.pos[0] >> g_mem.res[0]);
      
    // State machine - Evaluate position counter update
    switch (pos_cycle_status){
        case STATE_INACTIVE:
            if (pwm_sign == 1){
                thr_pos = curr_pos; 
                g_mem.wire_disp = g_mem.wire_disp + abs(max_pos - thr_pos);     //sum opening track
                pos_cycle_status = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            if (pwm_sign == -1){
                max_pos = curr_pos;
                g_mem.wire_disp = g_mem.wire_disp + abs(max_pos - thr_pos);     //sum closure track
                pos_cycle_status = COUNTER_INC;
            }
            break;
        case COUNTER_INC:
            if (abs(max_pos - thr_pos) > bin_threshold){
                //update position histogram
                step = ((int32)(g_mem.pos_lim_sup >> g_mem.res[0]) / 10);
                bin_st  = (uint8)(thr_pos/step);
                bin_max = (uint8)(max_pos/step);
                for (i=bin_st; i<= bin_max; i++){
                    //position_hist counts how many times the SoftHand stays in bin while closing
                    g_mem.position_hist[i] = g_mem.position_hist[i] + 1;
                }
                
                //update current histogram
                step = ((int32)(g_mem.current_limit) / 4);
                if (g_meas.curr > g_mem.current_limit)
                    g_mem.current_hist[3] = g_mem.current_hist[3] + 1; 
                else
                    g_mem.current_hist[(uint8)(g_meas.curr/step)] = g_mem.current_hist[(uint8)(g_meas.curr/step)] + 1;
            }
            pos_cycle_status = STATE_INACTIVE;
            break;
    }
    
    // State machine - Evaluate EMG counter update
    for (i=0; i<2 && emg_1_status == NORMAL && emg_2_status == NORMAL; i++){
        switch (emg_cycle_status[i]){
            case STATE_INACTIVE:
                if (g_meas.emg[i] > g_mem.emg_threshold[i]){
                    emg_cycle_status[i] = STATE_ACTIVE;
                }
                break;
            case STATE_ACTIVE:
                if (g_meas.emg[i] < 10){
                    emg_cycle_status[i] = COUNTER_INC;
                }
                break;
            case COUNTER_INC:
                g_mem.emg_counter[i] = g_mem.emg_counter[i] + 1;
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
            g_mem.total_time_rest = g_mem.total_time_rest + (uint32)((timer_value_s - timer_value_e)/10);
            g_mem.rest_counter = g_mem.rest_counter + 1;
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
    uint8 row_start = 12;
    uint8* addr_start   = (uint8*)&g_mem.emg_counter[0];    //Data at beginning of the row 12
    uint8* addr_start_c = (uint8*)&c_mem.emg_counter[0];
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