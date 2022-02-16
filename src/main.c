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
* \file         main.c
*
* \brief        Firmware main file.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2020 Centro "E.Piaggio". All rights reserved.
* \mainpage     Firmware
* \brief        This is the firmware of PSoC5 logic board.
* \version      1.9.4
*
* \details      This is the firmware of PSoC5 logic board. Depending on the configuration, 
*               it can control up to two motors and read its encoders. Also can read and
*               convert analog measurements connected to the PSoC microcontroller.                
*
*/


// ----------------------------------------------------------------------------
// This version changes:
//      - not reported


//=================================================================     includes

#include "device.h"
#include "globals.h"
#include "interruptions.h"
#include "command_processing.h"
#include "IMU_functions.h"
#include "Encoder_functions.h"
#include "SD_RTC_functions.h"
#include "utils.h"
#include "project.h"
#include "FS.h"

//==============================================================================
//                                                                 MAIN FUNCTION
//==============================================================================

int main()
{
    // Iterator    
    uint8 i, j;
    
    // Variable declarations for DMA     
    uint8 DMA_Chan;
    uint8 DMA_TD[1];
    
    //================================     initializations - psoc and components

    // EEPROM
    EEPROM_Start();
    memRecall();                                        // Recall configuration.  
    
    // FTDI chip enable
    CyDelay(100);
    FTDI_ENABLE_Write(0x01);
    
	// LED Enable   
    LED_control(1);     // Green fixed light
    BLINK_05HZ_Start();
    BLINK_25HZ_Start();
    BLINK_05HZ_WriteCompare(499);
    BLINK_25HZ_WriteCompare(99);
    
    // RS485
    UART_RS485_Stop();
    UART_RS485_Start();
    UART_RS485_Init();
    UART_RS485_ClearRxBuffer();
    UART_RS485_ClearTxBuffer();
    ISR_RS485_RX_StartEx(ISR_RS485_RX_ExInterrupt);
    
    // CYCLES TIMER   
    CYCLES_TIMER_Start();
    ISR_CYCLES_StartEx(ISR_CYCLES_Handler);
    
    // PWM
    PWM_MOTORS_Start();
    PWM_MOTORS_WriteCompare1(0);
    MOTOR_DIR_1_Write(0);
    enable_motor(0, 0x00);
    MOTOR_DIR_2_Write(0);
    enable_motor(1, 0x00);

#ifdef AIR_CHAMBERS_FB_FW
    if (c_mem.dev.dev_type == AIR_CHAMBERS_FB){
        VALVE_Write(0);
    }
#endif

    // Init AS5045 devices  
    COUNTER_ENC_Start();
    SHIFTREG_ENC_1_Start();
    SHIFTREG_ENC_2_Start();
    SHIFTREG_ENC_3_Start();

    // ADC
    ADC_Set_N_Channels();           // Set right number of ADC channels to sample
    ADC_Start();
    ADC_SOC_Write(0x01);            // Force first read cycle.

    // ADC DMA    
    DMA_Chan = DMA_DmaInitialize(DMA_BYTES_PER_BURST, DMA_REQUEST_PER_BURST, HI16(DMA_SRC_BASE), HI16(DMA_DST_BASE));
    DMA_TD[0] = CyDmaTdAllocate();                                                                          // Allocate TD.
    CyDmaTdSetConfiguration(DMA_TD[0], 2 * NUM_OF_ANALOG_INPUTS, DMA_TD[0], DMA__TD_TERMOUT_EN | TD_INC_DST_ADR); // DMA Configurations.
    CyDmaTdSetAddress(DMA_TD[0], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)ADC_buf));                    // Set Register Address.
    CyDmaChSetInitialTd(DMA_Chan, DMA_TD[0]);                                                               // Initialize Channel.
    CyDmaChEnable(DMA_Chan, 1);                                                                             // Enable DMA.
    
    // TIMER
    MY_TIMER_Start();           
    PACER_TIMER_Start();    
    
    CYGlobalIntEnable;              // Enable interrupts.

    // Init AS5045 devices
    InitEncoderGeneral();

    //SPI IMU module
    if (c_mem.imu.read_imu_flag) {

    	SPI_IMU_Start();
    	SPI_IMU_Init();
    	SPI_IMU_Enable();
    	SPI_IMU_ClearRxBuffer();
    	SPI_IMU_ClearTxBuffer();
    	SPI_IMU_ClearFIFO();							
        CyDelay(10);
        
        // Init MPU9250 devices
        InitIMUgeneral();
        
        // Initialize quaternion
        for (i = 0; i<N_IMU_MAX; i++) {
            Quat[i][0] = 0.999;
            Quat[i][1] = 0.01;
            Quat[i][2] = 0.01;
            Quat[i][3] = 0.01;
        }
    }
    
    //========================================     initializations - clean variables
    
    //---------------------------------------------------  Initialize filters structure 
    for(i=0;i<NUM_OF_MOTORS;i++) {
        filt_i[i].gain = 32;    // Current filter constant.
        filt_curr_diff[i].gain = 16;   // Current residual filter constant.       
        filt_v[i].old_value    = 12000;// Initial voltage filter value.
        filt_v[i].gain         = 2;    // Voltage filter constant.
    }
    for(i=0;i<NUM_OF_SENSORS;i++) {
        filt_vel[i].gain = 128; // Velocity filters constant.
    }
    
    for(i=0;i<NUM_OF_INPUT_EMGS+NUM_OF_ADDITIONAL_EMGS;i++) {
        filt_emg[i].gain      = 50;   // Emg channels filter constant.
    }
    filt_detect_pc.gain = 500;    
    
    //---------------------------------------------------  Initialize reference structure 
    for (i = 0; i< NUM_OF_MOTORS; i++) {
        g_ref[i].pos = 0;
    }
    
    //---------------------------------------------------  Initialize measurement structure
    for (j = 0; j< N_ENCODER_LINE_MAX; j++) { 
        for (i = 0; i < NUM_OF_SENSORS; i++) { 
            g_meas[j].pos[i] = 0;
            g_meas[j].rot[i] = (int8)0;
        }
    }

    for (i = 0; i< NUM_OF_MOTORS; i++) {
        g_refNew[i] = g_ref[i];                         // Initialize k+1 reference structure.
    }
    
    //---------------------------------------------------  Initialize emg structure
    g_adc_meas.emg[0] = 0;
    g_adc_meas.emg[1] = 0;
    g_adc_meas.joystick[0] = 0;
    g_adc_meas.joystick[1] = 0;
    for (uint16 k = 0; k<SAMPLES_FOR_EMG_HISTORY; k++){
        for (j = 0; j<NUM_OF_INPUT_EMGS; j++){
            emg_history[k][j] = (uint16)0;
        }
    }
    emg_history_next_idx = 0;
    
    set_motor_driver_type();
    
    for (i = 0; i< NUM_OF_MOTORS; i++) {
        if (c_mem.emg.emg_calibration_flag) {
            if ((c_mem.motor[i].input_mode == INPUT_MODE_EMG_PROPORTIONAL) ||
                (c_mem.motor[i].input_mode == INPUT_MODE_EMG_INTEGRAL) ||
                (c_mem.motor[i].input_mode == INPUT_MODE_EMG_FCFS) ||
                (c_mem.motor[i].input_mode == INPUT_MODE_EMG_FCFS_ADV) ||
                (c_mem.motor[i].input_mode == INPUT_MODE_EMG_PROPORTIONAL_NC))
                g_ref[i].onoff = 0x00;
            else
                g_ref[i].onoff = c_mem.motor[i].activ;
        } 
        else
            g_ref[i].onoff = c_mem.motor[i].activ;

        if (!g_mem.enc[g_mem.motor[i].encoder_line].double_encoder_on_off) {
            g_ref[i].onoff = c_mem.motor[i].activ;                          // Initialize Activation.
        } 
        else {
            // Do not activate motor until position reconstruction has finished
            g_ref[i].onoff = 0x00;
        }
        enable_motor(i, g_ref[i].onoff); 
        
        dev_pwm_limit[i] = dev_pwm_sat[i];              // Init PWM limit.
	
        pow_tension[i] = 12000;                         // 12000 mV (12 V)
    }
    
    tension_valid = FALSE;                              // Init tension_valid BIT.

    for (j = 0; j< N_ENCODER_LINE_MAX; j++) { 
        reset_last_value_flag[j] = 0;                   // Do not reset encoder last value.
    }

    //------------------------------------------------- Initialize package on receive from RS485
    g_rx.length = 0;
    g_rx.ready  = 0;
    
    //------------------------------------------------- Initialize rest position variables    
	rest_enabled = 0;
    forced_open = 0;
    
    LED_control(5);     // Default - red light
    
#ifdef MASTER_FW
    master_mode = 1;    // Default - activate master mode at startup
#endif

    //============================================================    check if maintenance is due

    if ( (g_mem.cnt.wire_disp/(((g_mem.motor[0].pos_lim_sup>>g_mem.enc[0].res[0]) - (g_mem.motor[0].pos_lim_inf>>g_mem.enc[0].res[0]))*2)) > (uint32)(PREREVISION_CYCLES/2) )   // 50 %
        maintenance_flag = TRUE;    

    if (g_mem.exp.read_exp_port_flag == EXP_SD_RTC) {
        
        store_RTC_current_time();
        
        // SD file
        InitSD_FS();
    }
    
    //============================================================     main loop
    
    // All peripherals has started, now it is ok to start communication
    RS485_CTS_Write(0);             // Clear To Send on RS485.

    for(;;)

    {             
        // Put the FF reset pin to LOW
        RESET_FF_Write(0x00);

        // Call function scheduler
        function_scheduler(); 

        //  Wait until the FF is set to 1
        while(FF_STATUS_Read() == 0){
            // On interrupt from RS485
            if (interrupt_flag){
                // Reset flags
                interrupt_flag = FALSE;
                
                // Manage Interrupt on rs485
                interrupt_manager();
            }
        };

        // Command a FF reset
        RESET_FF_Write(0x01);

        // Wait for FF to be reset
        while(FF_STATUS_Read() == 1);

        if(UART_RS485_ReadRxStatus() & UART_RS485_RX_STS_SOFT_BUFF_OVER)
            UART_RS485_ClearRxBuffer();
    }
    return 0;
}

/* [] END OF FILE */