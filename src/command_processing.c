// ----------------------------------------------------------------------------
// BSD 3-Clause License

// Copyright (c) 2016, qbrobotics
// Copyright (c) 2017-2019, Centro "E.Piaggio"
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
* \file         command_processing.c
*

* \brief        Command processing functions.
* \date         October 01, 2017
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2019 Centro "E.Piaggio". All rights reserved.
*/
//=================================================================     includes
#include "command_processing.h"
#include "interruptions.h"
#include "SPI_functions.h"
#include <STDIO.H>
#include "utils.h"
#include "commands.h"

//================================================================     variables

reg8 * EEPROM_ADDR = (reg8 *) CYDEV_EE_BASE;

//==============================================================================
//                                                            RX DATA PROCESSING
//==============================================================================
//  This function checks for the availability of a data packet and process it:
//      - Verify checksum;
//      - Process commands;
//==============================================================================

void commProcess(void){

    uint8 CYDATA rx_cmd;
    rx_cmd = g_rx.buffer[0];

//==========================================================     verify checksum

    if (!(LCRChecksum(g_rx.buffer, g_rx.length - 1) == g_rx.buffer[g_rx.length - 1])){
        // Wrong checksum
        g_rx.ready = 0;
        return;
    }

    switch(rx_cmd) {

//=============================================================     CMD_ACTIVATE
        case CMD_ACTIVATE:
            cmd_activate();
            break;

//===========================================================     CMD_SET_INPUTS

        case CMD_SET_INPUTS:
            cmd_set_inputs();
            break;

//=====================================================     CMD_GET_MEASUREMENTS

        case CMD_GET_MEASUREMENTS:
            cmd_get_measurements();
            break;

//=====================================================     CMD_GET_CURR_AND_MEAS

        case CMD_GET_CURR_AND_MEAS:
            cmd_get_curr_and_meas();
            break;
            
//=========================================================     CMD_GET_CURRENTS

        case CMD_GET_CURRENTS:
            cmd_get_currents();
            break;

//=========================================================     CMD_GET_CURR_DIFF
         
        case CMD_GET_CURR_DIFF:
            cmd_get_currents_for_cuff();
            break;
            
//=========================================================     CMD_GET_CURR_DIFF
        
        case CMD_GET_VELOCITIES:
            cmd_get_velocities();
            break;
            
//=========================================================     CMD_GET_CURR_DIFF

        case CMD_GET_ACCEL:
            cmd_get_accelerations();
            break;
            
//=========================================================     CMD_GET_EMG

        case CMD_GET_EMG:
            cmd_get_emg();
            break;
            
//=========================================================     CMD_GET_ACTIVATE
            
        case CMD_GET_ACTIVATE:
            cmd_get_activate();
            break;
            
//=========================================================     CMD_SET_BAUDRATE
            
        case CMD_SET_BAUDRATE:
            cmd_set_baudrate();
            break;  
            
//============================================================     CMD_GET_INPUT

        case CMD_GET_INPUTS:
            cmd_get_inputs();
            break;

//=============================================================     CMD_GET_INFO

        case CMD_GET_INFO:
            infoGet(  __REV16(*((uint16 *) &g_rx.buffer[1])) );
            break;

//============================================================     CMD_SET_PARAM

        case CMD_SET_ZEROS:
            setZeros();
            break;

//============================================================     CMD_GET_PARAM

        case CMD_GET_PARAM_LIST:
            manage_param_list( __REV16(*((uint16 *) &g_rx.buffer[1])) );
            //get_IMU_param_list( __REV16(*((uint16 *) &g_rx.buffer[1])) );
            break;

//=================================================================     CMD_PING
            
        case CMD_PING:
            cmd_ping();
            break;

//=========================================================     CMD_STORE_PARAMS
            
        case CMD_STORE_PARAMS:
            cmd_store_params();
            break;

//=================================================     CMD_STORE_DEFAULT_PARAMS

        case CMD_STORE_DEFAULT_PARAMS:
            if(memStore(DEFAULT_EEPROM_DISPLACEMENT))
                sendAcknowledgment(ACK_OK);
            else
                sendAcknowledgment(ACK_ERROR);
            break;

//=======================================================     CMD_RESTORE_PARAMS

        case CMD_RESTORE_PARAMS:
            if(memRestore())
                sendAcknowledgment(ACK_OK);
            else
                sendAcknowledgment(ACK_ERROR);
            break;

//=============================================================     CMD_INIT_MEM

        case CMD_INIT_MEM:
            if(memInit())
                sendAcknowledgment(ACK_OK);
            else
                sendAcknowledgment(ACK_ERROR);
            break;

//===========================================================     CMD_BOOTLOADER

        case CMD_BOOTLOADER:
            //Not sure if ACK_OK is correct, should check
            sendAcknowledgment(ACK_OK);
            CyDelay(1000);
            FTDI_ENABLE_REG_Write(0x00);
            CyDelay(1000);
            Bootloadable_Load();
            break;

//============================================================     CMD_HAND_CALIBRATE

        case CMD_HAND_CALIBRATE:
            calib.speed = (int16)(g_rx.buffer[1]<<8 | g_rx.buffer[2]);
            calib.repetitions = (int16)(g_rx.buffer[3]<<8 | g_rx.buffer[4]);
            //calib.speed = *((int16 *) &g_rx.buffer[1]);
            //calib.repetitions = *((int16 *) &g_rx.buffer[3]);
            
            if(calib.speed == -1 && calib.repetitions == -1) {
                calib.enabled = FALSE;
                calib.speed = 0;
                calib.repetitions = 0;
                break;
            }
            // Speed & repetitions saturations
            if (calib.speed < 0) {
                calib.speed = 0;
            } else if (calib.speed > 200) {
                calib.speed = 200;
            }
            if (calib.repetitions < 0) {
                calib.repetitions = 0;
            } else if (calib.repetitions > 32767) {
                calib.repetitions = 32767;
            }
            
            g_refNew.pos = 0;
            calib.enabled = TRUE;
         
            sendAcknowledgment(ACK_OK);
            break;
            
//=====================================================     CMD_GET_IMU_READINGS

        case CMD_GET_IMU_READINGS:
            cmd_get_imu_readings();
            break;    
            
//=========================================================== ALL OTHER COMMANDS
        default:
            break;

    }
}


//==============================================================================
//                                                                     INFO SEND
//==============================================================================

void infoSend(void){
    char packet_string[1500];
    
    prepare_generic_info(packet_string);
    UART_RS485_PutString(packet_string);
}


//==============================================================================
//                                                              COMMAND GET INFO
//==============================================================================

void infoGet(uint16 info_type) {
    char packet_string[4000] = "";
    
    //==================================     choose info type and prepare string

    switch (info_type) {
        case INFO_ALL:
            prepare_generic_info(packet_string);
            UART_RS485_ClearTxBuffer(); 
            UART_RS485_PutString(packet_string);
            break;
        case CYCLES_INFO:
            prepare_counter_info(packet_string);
            UART_RS485_ClearTxBuffer();
            UART_RS485_PutString(packet_string);
            break;
        default:
            break;
    }
}


//==============================================================================
//                                                                GET PARAM LIST
//==============================================================================

void get_param_list() {
    
    //Package to be sent variables
    uint8 packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + PARAM_MENU_SLOT*7 + PARAM_BYTE_SLOT] = "";      //50*NUM_OF_PARAMS + 150*NUM_DIFFERENT_MENUS
    uint16 packet_lenght = PARAM_BYTE_SLOT*NUM_OF_PARAMS + PARAM_MENU_SLOT*7 + PARAM_BYTE_SLOT;

    //Auxiliary variables
    uint8 CYDATA i, j;
    uint8 CYDATA idx = 0;       //Parameter number
    uint8 CYDATA idx_menu = 0;
    uint8 CYDATA sod = 0;       //sizeof data
    uint8 CYDATA string_lenght;
    char CYDATA aux_str[50] = "";
    uint8* m_addr = (uint8*)&(c_mem.id);
    uint8* m_tmp = m_addr;
    float aux_float;
    int16 aux_int16;
    uint16 aux_uint16;
    int32 aux_int32;
    uint32 aux_uint32;

    // Arrays
    const uint8 TYPES[NUM_OF_PARAMS] = {
        TYPE_UINT8, TYPE_FLOAT, TYPE_FLOAT, TYPE_FLAG, TYPE_FLAG, TYPE_FLAG, TYPE_UINT8, TYPE_INT16, 
        TYPE_FLOAT, TYPE_FLAG, TYPE_INT32, TYPE_INT32, TYPE_INT16, TYPE_UINT16, TYPE_FLAG, TYPE_UINT32, 
        TYPE_UINT8, TYPE_FLAG, TYPE_INT8, TYPE_FLAG, TYPE_FLOAT, TYPE_INT32, TYPE_INT32, TYPE_INT32,
        TYPE_FLAG, TYPE_FLAG, TYPE_FLAG, TYPE_FLAG, TYPE_FLAG, TYPE_FLAG, TYPE_UINT8
    };

    const uint8 NUM_ITEMS[NUM_OF_PARAMS] = {
        1, 3, 3, 1, 1, 1, 3, 3,
        3, 1, 2, 2, 1, 2, 1, 2,
        1, 1, 1, 1, 6, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 6
    };

    const uint8 NUM_MENU[13] = {3, 1, 2, 3, 3, 3, 3, 3, 3, 4, 5, 6, 3};
    
    const char* PARAMS_STR[NUM_OF_PARAMS] = {
        "1 - Device ID:", "2 - Position PID [P, I, D]:", "3 - Current PID [P, I, D]:", "4 - Startup Activation:",
        "5 - Input mode:", "6 - Control mode:", "7 - Resolutions:", "8 - Measurement Offsets:", 
        "9 - Multipliers:", "10 - Pos. limit active:", "11 - Pos. limits [inf, sup]:", "12 - Max steps [neg, pos]:",
        "13 - Current limit:", "14 - EMG thresholds:", "15 - EMG calibration on startup:", "16 - EMG max values:",
        "17 - EMG max speed:", "18 - Absolute encoder position:", "19 - Motor handle ratio:", "20 - PWM rescaling:",
        "21 - Current lookup:", "22 - Rest position:", "23 - Rest position time delay (ms):", "24 - Rest vel closure (ticks/sec):", 
        "25 - Rest position enabled:", "26 - EMG inversion:",  "27 - Hand side:", "28 - Read all IMUs:", 
        "29 - Read Expansion port:", "30 - Reset counters:", "31 - Current Time [DD/MM/YY HH:MM:SS]:"
    };

    //Parameters menu
    const char input_mode_menu[99] = "0 -> Usb\n1 -> Handle\n2 -> EMG proportional\n3 -> EMG Integral\n4 -> EMG FCFS\n5 -> EMG FCFS Advanced\n";
    const char control_mode_menu[64] = "0 -> Position\n1 -> PWM\n2 -> Current\n3 -> Position and Current\n";
    const char yes_no_menu[42] = "0 -> Deactivate [NO]\n1 -> Activate [YES]\n";
    const char right_left_menu[22] = "0 -> Right\n1 -> Left\n";
    const char on_off_menu[51] = "0 -> OFF\n1 -> ON\nYou must restart the board now\n";
    const char exp_port_menu[87] = "0 -> None\n1 -> SD/RTC board\n2 -> WiFi board\n3 -> Other\nYou must restart the board now\n";
    char spi_delay_menu[118] = " ";
    
    sprintf(spi_delay_menu, "0 -> None\n1 -> Low (%u us delay for each 8-bit register read)\n2 -> High (%u us delay for each 8-bit register read)\n", (int)SPI_DELAY_LOW, (int)SPI_DELAY_HIGH);
    
    packet_data[0] = CMD_GET_PARAM_LIST;
    packet_data[1] = NUM_OF_PARAMS;
    
    for (idx = 0; idx < NUM_OF_PARAMS; idx++) {
        
        // Add parameter type and size to packet
        packet_data[2 + PARAM_BYTE_SLOT*idx] = TYPES[idx];
        packet_data[3 + PARAM_BYTE_SLOT*idx] = NUM_ITEMS[idx];
        
        // Find size of data
        switch (TYPES[idx]) {
            case TYPE_FLAG: case TYPE_INT8: case TYPE_UINT8:
                sod = 1; break;
            case TYPE_INT16: case TYPE_UINT16:
                sod = 2; break;
            case TYPE_INT32: case TYPE_UINT32: case TYPE_FLOAT:
                sod = 4; break;
        }
        
        // Add parameter data to packet
        switch (TYPES[idx]) {
            case TYPE_FLAG: case TYPE_UINT8:
                for (i=0; i<NUM_ITEMS[idx]; i++){
                    m_tmp = m_addr + i*sod;
                    packet_data[4 + PARAM_BYTE_SLOT*idx + i*sod] = *m_tmp;
                }
                break;                
            case TYPE_INT8:
                for (i=0; i<NUM_ITEMS[idx]; i++){
                    m_tmp = m_addr + i*sod;
                    packet_data[4 + PARAM_BYTE_SLOT*idx + i*sod] = *m_tmp;
                }
                break;
            case TYPE_INT16:
                for (i=0; i<NUM_ITEMS[idx]; i++){
                    if (idx == 7) {     //Offset measurements
                        aux_int16 = (c_mem.m_off[i] >> c_mem.res[i]);
                        for(j = 0; j < sod; j++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + i*sod) + sod - j -1] = ((char*)(&aux_int16))[j];
                        }
                        //*((int16*)(packet_data + (4 + PARAM_BYTE_SLOT*idx) + i*sod)) = (int16) (c_mem.m_off[i] >> c_mem.res[i]);
                    }
                    else {
                        m_tmp = m_addr + i*sod;
                        aux_int16 = *((int16*)m_tmp);
                        for(j = 0; j < sod; j++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + i*sod) + sod - j -1] = ((char*)(&aux_int16))[j];
                        }
                        //*((int16*)(packet_data + (4 + PARAM_BYTE_SLOT*idx) + i*sod)) = *((int16*)m_tmp);
                    }
                }
                break;
            case TYPE_UINT16:
                for (i=0; i<NUM_ITEMS[idx]; i++){
                    m_tmp = m_addr + i*sod;
                    aux_uint16 = *((uint16*)m_tmp);
                    for(j = 0; j < sod; j++) {
                        packet_data[(4 + PARAM_BYTE_SLOT*idx + i*sod) + sod - j -1] = ((char*)(&aux_uint16))[j];
                    }
                    //*((uint16*)(packet_data + (4 + PARAM_BYTE_SLOT*idx) + i*sod)) = *((uint16*)m_tmp);
                }
                break;
            case TYPE_INT32: 
                for (i=0; i<NUM_ITEMS[idx]; i++){
                    switch (idx){
                        case 10:    // POSITION LIMITS (param 11)
                            aux_int32 = (c_mem.pos_lim_inf >> c_mem.res[0]);
                            for(j = 0; j < sod; j++) {
                                packet_data[(4 + PARAM_BYTE_SLOT*idx) + sod - j -1] = ((char*)(&aux_int32))[j];
                            }
                            aux_int32 = (c_mem.pos_lim_sup >> c_mem.res[0]);
                            for(j = 0; j < sod; j++) {
                                packet_data[(4 + PARAM_BYTE_SLOT*idx + sod) + sod - j -1] = ((char*)(&aux_int32))[j];
                            }
                            //*((int32 *)( packet_data + (4 + PARAM_BYTE_SLOT*idx))) = (c_mem.pos_lim_inf >> c_mem.res[0]);
                            //*((int32 *)( packet_data + (4 + PARAM_BYTE_SLOT*idx) + sod)) = (c_mem.pos_lim_sup >> c_mem.res[0]);
                            break;
                        case 21:    // REST POSITION (param 22)
                            aux_int32 = (c_mem.rest_pos >> c_mem.res[0]);
                            for(j = 0; j < sod; j++) {
                                packet_data[(4 + PARAM_BYTE_SLOT*idx) + sod - j -1] = ((char*)(&aux_int32))[j];
                            }
                            //*((int32 *)( packet_data + (4 + PARAM_BYTE_SLOT*idx))) = (c_mem.rest_pos >> c_mem.res[0]);
                            break;
                        default:
                            m_tmp = m_addr + i*sod;
                            aux_int32 = *((int32*)m_tmp);
                            for(j = 0; j < sod; j++) {
                                packet_data[(4 + PARAM_BYTE_SLOT*idx + i*sod) + sod - j -1] = ((char*)(&aux_int32))[j];
                            }
                            //*((int32*)(packet_data + (4 + PARAM_BYTE_SLOT*idx) + i*sod)) = *((int32*)m_tmp);
                            break;
                    }
                }
                break;
            case TYPE_UINT32: 
                for (i=0; i<NUM_ITEMS[idx]; i++){
                    m_tmp = m_addr + i*sod;
                    aux_uint32 = *((uint32*)m_tmp);
                    for(j = 0; j < sod; j++) {
                        packet_data[(4 + PARAM_BYTE_SLOT*idx + i*sod) + sod - j -1] = ((char*)(&aux_uint32))[j];
                    }
                    //*((uint32*)(packet_data + (4 + PARAM_BYTE_SLOT*idx) + i*sod)) = *((uint32*)m_tmp);
                }
                break;
                
            case TYPE_FLOAT:
                switch (idx) {
                
                case 1:     // POSITION PID (param 2)
                    if(c_mem.control_mode != CURR_AND_POS_CONTROL) {
                        aux_float = (float) c_mem.k_p / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_i / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_d / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + 2*sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx))) = aux_float;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + sod)) = (float) c_mem.k_i / 65536;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + 2*sod)) = (float) c_mem.k_d / 65536;
                    
                    }
                    else {
                        aux_float = (float) c_mem.k_p_dl / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_i_dl / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_d_dl / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + 2*sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx))) = (float) c_mem.k_p_dl / 65536;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + sod)) = (float) c_mem.k_i_dl / 65536;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + 2*sod)) = (float) c_mem.k_d_dl / 65536;
                    }
                    
                    break;
                case 2:     // POSITION PID (param 3)
                    if(c_mem.control_mode != CURR_AND_POS_CONTROL) {
                        aux_float = (float) c_mem.k_p_c / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_i_c / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_d_c / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + 2*sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx))) = (float) c_mem.k_p_c / 65536;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + sod)) = (float) c_mem.k_i_c / 65536;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + 2*sod)) = (float) c_mem.k_d_c / 65536;
                    }
                    else {
                        aux_float = (float) c_mem.k_p_c_dl / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_i_c_dl / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        aux_float = (float) c_mem.k_d_c_dl / 65536;
                        for(i = 0; i < sod; i++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + 2*sod) + sod - i -1] = ((char*)(&aux_float))[i];
                        }
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx))) = (float) c_mem.k_p_c_dl / 65536;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + sod)) = (float) c_mem.k_i_c_dl / 65536;
                        //*((float *) (packet_data + (4 + PARAM_BYTE_SLOT*idx) + 2*sod)) = (float) c_mem.k_d_c_dl / 65536;
                    }
                    break;
                default:
                    for (i=0; i<NUM_ITEMS[idx]; i++){
                        m_tmp = m_addr + i*sod;
                        aux_float = *((float*)m_tmp);
                        for(j = 0; j < sod; j++) {
                            packet_data[(4 + PARAM_BYTE_SLOT*idx + i*sod) + sod - j -1] = ((char*)(&aux_float))[j];
                        }
                        //*((float*)(packet_data + (4 + PARAM_BYTE_SLOT*idx) + i*sod)) = *((float*)m_tmp);
                    }
                    break;
                }
                break;          
        }
        
        sprintf(aux_str, (char*)PARAMS_STR[idx]);
        string_lenght = strlen(aux_str);
        
        // Parameters with a menu
        if (TYPES[idx] == TYPE_FLAG){
            switch(NUM_MENU[idx_menu]){
                case 1:     // input mode menu
                    switch(*m_addr) {
                        case INPUT_MODE_EXTERNAL:
                            strcat(aux_str, " Usb");
                        break;
                        case INPUT_MODE_ENCODER3:
                            strcat(aux_str, " Handle");
                        break;
                        case INPUT_MODE_EMG_PROPORTIONAL:
                            strcat(aux_str, " EMG proportional");
                        break;
                        case INPUT_MODE_EMG_INTEGRAL:
                            strcat(aux_str, " EMG integral");
                        break;
                        case INPUT_MODE_EMG_FCFS:
                            strcat(aux_str, " EMG FCFS");
                        break;
                        case INPUT_MODE_EMG_FCFS_ADV:
                            strcat(aux_str, " EMG FCFS Advanced");
                        break;
                    }
                    break;
                case 2:     // control mode menu
                    switch(*m_addr){
                        case CONTROL_ANGLE:
                            strcat(aux_str, " Position");
                        break;
                        case CONTROL_PWM:
                            strcat(aux_str, " PWM");
                        break;
                        case CONTROL_CURRENT:
                            strcat(aux_str, " Current");
                        break;
                        case CURR_AND_POS_CONTROL:
                            strcat(aux_str, " Position and Current");
                        break;
                    }
                    break;
                case 3:     // yes/no menu
                    if(*m_addr){
                        strcat(aux_str, " YES\0");
                    }
                    else {
                        strcat(aux_str, " NO\0");
                    }
                    break;    
                case 4:     // right/lef menu
                    switch(*m_addr){
                        case RIGHT_HAND:
                            strcat(aux_str, " Right\0");
                        break;
                        case LEFT_HAND:
                            strcat(aux_str, " Left\0");
                        break;
                    }
                    break;
                case 5:
                    switch(*m_addr){
                        case 0:
                            strcat(aux_str, " OFF\0");
                        break;
                        case 1:
                            strcat(aux_str, " ON\0");
                        break;
                    }
                    break;
                case 6:
                    switch(*m_addr){
                        case EXP_NONE:
                            strcat(aux_str, " None\0");
                            break;
                        case EXP_SD_RTC:
                            strcat(aux_str, " SD/RTC board\0");
                            break;
                        case EXP_WIFI:
        					strcat(aux_str, " WiFi board\0");
        					break;
                        case EXP_OTHER:
        					strcat(aux_str, " Other\0");
        					break;
                    }
                    break;
                case 7:
                    switch(*m_addr){
                        case 0: 
                            strcat(aux_str, " None\0"); 
                            break;
                        case 1: 
                            strcat(aux_str, " Low\0"); 
                            break;
                        case 2: 
                            strcat(aux_str, " High\0");
                            break;
                        default:
        					strcat(aux_str, " Undefined\0");
                            break;
                    }
                    break;
            }
            //Recomputes string lenght
            string_lenght = strlen(aux_str)+1;
        }

        // Add parameter string to packet
        for(i = string_lenght; i != 0; i--)
            packet_data[(4 + PARAM_BYTE_SLOT*idx) + (sod*NUM_ITEMS[idx]) + string_lenght - i] = aux_str[string_lenght - i];
        //The following byte indicates the number of menus at the end of the packet to send
        if (TYPES[idx] == TYPE_FLAG){
            packet_data[(4 + PARAM_BYTE_SLOT*idx) + (sod*NUM_ITEMS[idx]) + string_lenght] = NUM_MENU[idx_menu];
            idx_menu = idx_menu + 1;
        }
        
        
        // Adjust m_addr increment, according to c_mem structure
        switch (idx) {
            case 0:
                // Add 2 bytes free, since PSOC5 ARM memory is 4-bytes aligned
                m_addr = (uint8*)&c_mem.k_p;
                break;
            case 2:
                // double loop PID controller parameters 
                //(increment after CURRENT PID parameters)
                //m_addr = m_addr + NUM_ITEMS[idx]*sod + 6*sod;
                m_addr = &c_mem.activ;
                break; 
            case 6:
                // Add 1 byte free, since PSOC5 ARM memory is 4-bytes aligned
                m_addr = (uint8*)&c_mem.m_off[0];
                break;
            case 7:
                //(increment after OFFSET parameters)
                //m_addr = m_addr + NUM_ITEMS[idx]*4;     //int16->int32 conversion
                m_addr = (uint8*)&c_mem.m_mult[0];
                break;
            case 9:
                // Add 3 bytes free, since PSOC5 ARM memory is 4-bytes aligned
                m_addr = (uint8*)&c_mem.pos_lim_inf;
                break;
             case 14:
                // Add 1 byte free, since PSOC5 ARM memory is 4-bytes aligned
                m_addr = (uint8*)&c_mem.emg_max_value[0];
                break;
            case 20:
                //(increment after LOOKUP TABLE parameter)
                //m_addr = m_addr + NUM_ITEMS[idx]*sod + 1 + 3;   //1 = baudrate, 3 = maintenance date
                m_addr = (uint8*)&c_mem.rest_pos;
                break;
            case 29:
                //(increment after RESET COUNTERS parameter)
                //m_addr = m_addr + NUM_ITEMS[idx]*sod + 25 + 80;   //25 = unused bytes, 80 = 20*4 counters
                m_addr = (uint8*)&c_mem.curr_time[0];
                break;                
            default:
                m_addr = m_addr + NUM_ITEMS[idx]*sod;
                break;
                
            // reset counters reads g_mem.unused_bytes[0]
        }
    }

    string_lenght = strlen((char*)input_mode_menu);
    for(i = string_lenght; i != 0; i--)
        packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + 2 + string_lenght - i] = input_mode_menu[string_lenght - i];

    string_lenght = strlen((char*)control_mode_menu);
    for(i = string_lenght; i != 0; i--)
        packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + 2 + PARAM_MENU_SLOT + string_lenght - i] = control_mode_menu[string_lenght - i];

    string_lenght = strlen((char*)yes_no_menu);
    for(i = string_lenght; i!= 0; i--)
        packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + 2 + 2*PARAM_MENU_SLOT + string_lenght - i] = yes_no_menu[string_lenght - i];

    string_lenght = strlen((char*)right_left_menu);
    for(i = string_lenght; i!= 0; i--)
        packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + 2 + 3*PARAM_MENU_SLOT + string_lenght - i] = right_left_menu[string_lenght - i];

    string_lenght = strlen((char*)on_off_menu);
    for(i = string_lenght; i!= 0; i--)
        packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + 2 + 4*PARAM_MENU_SLOT + string_lenght - i] = on_off_menu[string_lenght - i];

    string_lenght = strlen((char*)exp_port_menu);
    for(i = string_lenght; i!= 0; i--)
        packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + 2 + 5*PARAM_MENU_SLOT + string_lenght - i] = exp_port_menu[string_lenght - i];

    string_lenght = strlen((char*)spi_delay_menu);
    for(i = string_lenght; i!= 0; i--)
        packet_data[PARAM_BYTE_SLOT*NUM_OF_PARAMS + 2 + 6*PARAM_MENU_SLOT + string_lenght - i] = spi_delay_menu[string_lenght - i];
                
    packet_data[packet_lenght - 1] = LCRChecksum(packet_data,packet_lenght - 1);
    commWrite(packet_data, packet_lenght);
}

//==============================================================================
//                                                             MANAGE PARAM LIST
//==============================================================================

void manage_param_list(uint16 index) {
    uint8 CYDATA i, j;
    uint8 aux_uchar;
    float aux_float, aux_float2;
    
    switch(index) {
        case 0:         //List of all parameters with relative types
            get_param_list();
        break;

//===================================================================     set_id
        case 1:         //ID - uint8
            g_mem.id = g_rx.buffer[3];
        break;
        
//=======================================================     set_pid_parameters
        case 2:         //Position PID - float[3]
            if(c_mem.control_mode != CURR_AND_POS_CONTROL) {
                aux_float = *((float *) &g_rx.buffer[3]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_p = aux_float2 * 65536;
                //g_mem.k_p = *((float *) &g_rx.buffer[3]) * 65536;
                
                aux_float = *((float *) &g_rx.buffer[3 + 4]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_i = aux_float2 * 65536;
                //g_mem.k_i = *((float *) &g_rx.buffer[3 + 4]) * 65536;
                
                aux_float = *((float *) &g_rx.buffer[3 + 8]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_d = aux_float2 * 65536;
                //g_mem.k_d = *((float *) &g_rx.buffer[3 + 8]) * 65536;
            }
            else {
                aux_float = *((float *) &g_rx.buffer[3]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_p_dl = aux_float2 * 65536;
                //g_mem.k_p_dl = *((float *) &g_rx.buffer[3]) * 65536;
                
                aux_float = *((float *) &g_rx.buffer[3 + 4]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_i_dl = aux_float2 * 65536;
                //g_mem.k_i_dl = *((float *) &g_rx.buffer[3 + 4]);
                
                aux_float = *((float *) &g_rx.buffer[3 + 8]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_d_dl = aux_float2 * 65536;
                //g_mem.k_d_dl = *((float *) &g_rx.buffer[3 + 8]) * 65536;
            }
        break;

//==================================================     set_curr_pid_parameters
        case 3:         //Current PID - float[3]
            if(c_mem.control_mode != CURR_AND_POS_CONTROL) {
                aux_float = *((float *) &g_rx.buffer[3]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_p_c = aux_float2 * 65536;
                //g_mem.k_p_c = *((float *) &g_rx.buffer[3]) * 65536;
                
                aux_float = *((float *) &g_rx.buffer[3 + 4]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_i_c = aux_float2 * 65536;
                //g_mem.k_i_c = *((float *) &g_rx.buffer[3 + 4]) * 65536;
                
                aux_float = *((float *) &g_rx.buffer[3 + 8]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_d_c = aux_float2 * 65536;
                //g_mem.k_d_c = *((float *) &g_rx.buffer[3 + 8]) * 65536;
            }
            else {
                aux_float = *((float *) &g_rx.buffer[3]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_p_c_dl = aux_float2 * 65536;
                //g_mem.k_p_c_dl = *((float *) &g_rx.buffer[3]) * 65536;
                
                aux_float = *((float *) &g_rx.buffer[3 + 4]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_i_c_dl = aux_float2 * 65536;
                //g_mem.k_i_c_dl = *((float *) &g_rx.buffer[3 + 4]) * 65536;
                
                aux_float = *((float *) &g_rx.buffer[3 + 8]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&aux_float2))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                g_mem.k_d_c_dl = aux_float2 * 65536;
                //g_mem.k_d_c_dl = *((float *) &g_rx.buffer[3 + 8]) * 65536;
            }
            
        break;

//===================================================     set_startup_activation        
        case 4:         //Startup flag - uint8
            if(g_rx.buffer[3])
                g_mem.activ = 0x01;
            else
                g_mem.activ = 0x00;
        break;

//===========================================================     set_input_mode        
        case 5:         //Input mode - uint8
            g_mem.input_mode = g_rx.buffer[3];
            
            // Hold the actual position
            g_refNew.pos = g_meas.pos[0];
        break;
        
//=========================================================     set_control_mode
        case 6:         //Control mode - uint8
            g_mem.control_mode = g_rx.buffer[3];
        break;
        
//===========================================================     set_resolution
        case 7:         //Resolution - uint8[3]
            for (i =0; i < NUM_OF_SENSORS; i++) {
                g_mem.res[i] = g_rx.buffer[i+3];
            }
        break;
        
//===============================================================     set_offset
        case 8:         //Measurement Offset - int32[3] 
            for(i = 0; i < NUM_OF_SENSORS; ++i) {
                g_mem.m_off[i] = (int16)(g_rx.buffer[3 + i*2]<<8 | g_rx.buffer[4 + i*2]);
                //g_mem.m_off[i] = *((int16 *) &g_rx.buffer[3 + i * 2]);
                g_mem.m_off[i] = g_mem.m_off[i] << g_mem.res[i];

                g_meas.rot[i] = 0;
            }
            reset_last_value_flag = 1;
        break;
        
//===========================================================     set_multiplier
        case 9:         //Multipliers - float[3]
            for(i = 0; i < NUM_OF_SENSORS; ++i){
                aux_float = *((float *) &g_rx.buffer[3 + i*4]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&g_mem.m_mult[i]))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                //g_mem.m_mult[i] = *((float *) &g_rx.buffer[3 + i * 4]);
            }
        break;
        
//=====================================================     set_pos_limit_enable
        case 10:        //Position limit flag - uint8
            g_mem.pos_lim_flag = *((uint8 *) &g_rx.buffer[3]);
        break;

//============================================================     set_pos_limit
        case 11:        //Position limits - int32[4]
            g_mem.pos_lim_inf = (int32)(g_rx.buffer[3]<<24 | g_rx.buffer[4]<<16 | g_rx.buffer[5]<<8 | g_rx.buffer[6]);
            g_mem.pos_lim_sup = (int32)(g_rx.buffer[7]<<24 | g_rx.buffer[8]<<16 | g_rx.buffer[9]<<8 | g_rx.buffer[10]);
            //g_mem.pos_lim_inf = *((int32 *) &g_rx.buffer[3]);
            //g_mem.pos_lim_sup = *((int32 *) &g_rx.buffer[7]);

            g_mem.pos_lim_inf = g_mem.pos_lim_inf << g_mem.res[0];
            g_mem.pos_lim_sup = g_mem.pos_lim_sup << g_mem.res[0];
        break;

//==================================================     set_max_steps_per_cycle
        case 12:        //Max steps - int32[2]
            g_mem.max_step_neg = (int32)(g_rx.buffer[3]<<24 | g_rx.buffer[4]<<16 | g_rx.buffer[5]<<8 | g_rx.buffer[6]);
            g_mem.max_step_pos = (int32)(g_rx.buffer[7]<<24 | g_rx.buffer[8]<<16 | g_rx.buffer[9]<<8 | g_rx.buffer[10]);
            
            //aux_int = *((int32 *) &g_rx.buffer[3]);
            //if (aux_int <= 0)
            //    g_mem.max_step_neg = aux_int;

            //aux_int = *((int32 *) &g_rx.buffer[3 + 4]);
            //if (aux_int >= 0) 
            //    g_mem.max_step_pos = aux_int;
            
        break;
        
//========================================================     set_current_limit
        case 13:        //Current limit - int16
            g_mem.current_limit = (int16)(g_rx.buffer[3]<<8 | g_rx.buffer[4]);
            //g_mem.current_limit = *((int16*) &g_rx.buffer[3]);
        break;
        
//========================================================     set_emg_threshold
        case 14:        //Emg threshold - uint16[2]
            g_mem.emg_threshold[0] = (uint16)(g_rx.buffer[3]<<8 | g_rx.buffer[4]);
            g_mem.emg_threshold[1] = (uint16)(g_rx.buffer[5]<<8 | g_rx.buffer[6]);
            //g_mem.emg_threshold[0] = *((uint16*) &g_rx.buffer[3]);
            //g_mem.emg_threshold[1] = *((uint16*) &g_rx.buffer[5]);
        break;
            
//=======================================================     set_emg_calib_flag
        case 15:        //Emg calibration flag - uint8
            g_mem.emg_calibration_flag = *((uint8*) &g_rx.buffer[3]);
        break;
            
//========================================================     set_emg_max_value
        case 16:        //Emg max value - uint32[2]
            g_mem.emg_max_value[0] = (uint32)(g_rx.buffer[3]<<24 | g_rx.buffer[4]<<16 | g_rx.buffer[5]<<8 | g_rx.buffer[6]);;
            g_mem.emg_max_value[1] = (uint32)(g_rx.buffer[7]<<24 | g_rx.buffer[8]<<16 | g_rx.buffer[9]<<8 | g_rx.buffer[10]);;
            //g_mem.emg_max_value[0] = *((uint32*) &g_rx.buffer[3]);
            //g_mem.emg_max_value[1] = *((uint32*) &g_rx.buffer[7]);
        break;
        
//============================================================     set_emg_speed
        case 17:        //Emg max speed - uint8
            g_mem.emg_speed = *((uint8*) &g_rx.buffer[3]);
        break;
        
//================================================     set_double_encoder_on_off
        case 18:        //Absolute encoder flag - uint8
            aux_uchar = *((uint8*) &g_rx.buffer[3]);
            if (aux_uchar) {
                g_mem.double_encoder_on_off = 1;
            } else {
                g_mem.double_encoder_on_off = 0;
            }
        break;
        
//===================================================     set_motor_handle_ratio
        case 19:        //Motor handle ratio - int8
            g_mem.motor_handle_ratio = *((int8*) &g_rx.buffer[3]);
        break;
        
//===================================================     set_motor_supply_type
        case 20:        //Motor type - uint8
            g_mem.activate_pwm_rescaling = g_rx.buffer[3];
        break;
            
//===================================================     set_curr_lookup_table
        case 21:        //Current lookup table - float
            for(i = 0; i < LOOKUP_DIM; i++){
                aux_float = *((float *) &g_rx.buffer[3 + i*4]);
                for(j = 0; j < 4; j++) {
                    ((char*)(&g_mem.curr_lookup[i]))[4 - j -1] = ((char*)(&aux_float))[j];
                }
                //g_mem.curr_lookup[i] = *((float *) &g_rx.buffer[3 + i*4]);
            }
        break;
            
//============================================================     set_rest_pos
        case 22:        //Rest Position - int32
            g_mem.rest_pos = (int32)(g_rx.buffer[3]<<24 | g_rx.buffer[4]<<16 | g_rx.buffer[5]<<8 | g_rx.buffer[6]);
            //g_mem.rest_pos = *((int32 *) &g_rx.buffer[3]);
            g_mem.rest_pos = g_mem.rest_pos << g_mem.res[0];
        break; 
            
//============================================================     set_rest_delay_pos
        case 23:        //Rest Position Time Delay - int32
           g_mem.rest_delay = (int32)(g_rx.buffer[3]<<24 | g_rx.buffer[4]<<16 | g_rx.buffer[5]<<8 | g_rx.buffer[6]);
           //g_mem.rest_delay = *((float *) &g_rx.buffer[3]);
		   if (g_mem.rest_delay < 10) g_mem.rest_delay = 10;
        break; 
            
//============================================================     set_rest_vel
        case 24:        //Rest Position Velocity - int32
            g_mem.rest_vel = (int32)(g_rx.buffer[3]<<24 | g_rx.buffer[4]<<16 | g_rx.buffer[5]<<8 | g_rx.buffer[6]);
			//g_mem.rest_vel = *((float *) &g_rx.buffer[3]);
        break;  
            
            
//================================================     set_rest_position_flag
        case 25:        //Rest position flag - uint8
            aux_uchar = *((uint8*) &g_rx.buffer[3]);
            if (aux_uchar) {
                g_mem.rest_position_flag = TRUE;
            } else {
                g_mem.rest_position_flag = FALSE;
            }
        break; 
            
//===================================================     set_switch_emg
        case 26:        //EMG inversion - uint8
            g_mem.switch_emg = g_rx.buffer[3];
        break; 
//================================================     set_right_left_flag
        case 27:        //Right/Left hand flag - uint8
            aux_uchar = *((uint8*) &g_rx.buffer[3]);
            if (aux_uchar) {    // 1
                g_mem.right_left = LEFT_HAND;
            } else {            // 0
                g_mem.right_left = RIGHT_HAND;
            }
			reset_last_value_flag = 1;
        break; 
            
//===================================================     set_read_imu_flag
        case 28:        //Read IMU flag - uint8
            g_mem.read_imu_flag = g_rx.buffer[3];
        break; 
            
//===================================================     set_read_exp_port
        case 29:        //Read Expansion Port - uint8
            g_mem.read_exp_port_flag = g_rx.buffer[3];
        break; 
            
//===================================================     reset_counters
        case 30:        //Reset counters - uint8
            aux_uchar = *((uint8*) &g_rx.buffer[3]);
            if (aux_uchar) {
                reset_counters();
            }
            
            if (c_mem.read_exp_port_flag == EXP_SD_RTC) {
                // Set date of maintenance from RTC
                aux_uchar = DS1302_read(DS1302_DATE_RD);
                g_mem.maint_day = (aux_uchar/16) * 10 + aux_uchar%16;
                aux_uchar = DS1302_read(DS1302_MONTH_RD);
                g_mem.maint_month = (aux_uchar/16) * 10 + aux_uchar%16;
                aux_uchar = DS1302_read(DS1302_YEAR_RD);
                g_mem.maint_year = (aux_uchar/16) * 10 + aux_uchar%16;  
            }
        break;             
            
//==================================================    set_current_time
        case 31:         //Current Time - uint8[3]
            for (i=0; i<6; i++){
                g_mem.curr_time[i] = g_rx.buffer[3 + i];
            }
            
            if (c_mem.read_exp_port_flag == EXP_SD_RTC) {
                DS1302_write(DS1302_DATE_WR, (((g_mem.curr_time[0] / 10)*16) + (g_mem.curr_time[0] % 10)));
                DS1302_write(DS1302_MONTH_WR, (((g_mem.curr_time[1] / 10)*16) + (g_mem.curr_time[1] % 10)));
                DS1302_write(DS1302_YEAR_WR, (((g_mem.curr_time[2] / 10)*16) + (g_mem.curr_time[2] % 10)));
                DS1302_write(DS1302_HOUR_WR, (((g_mem.curr_time[3] / 10)*16) + (g_mem.curr_time[3] % 10)));
                DS1302_write(DS1302_MINUTES_WR, (((g_mem.curr_time[4] / 10)*16) + (g_mem.curr_time[4] % 10)));
                DS1302_write(DS1302_SECONDS_WR, (((g_mem.curr_time[5] / 10)*16) + (g_mem.curr_time[5] % 10)));
            }
        break;  
    }                
}


//==============================================================================
//                                                        GET IMU PARAMETER LIST
//==============================================================================

void get_IMU_param_list(uint16 index)
{
    //Package to be sent variables
    uint8 packet_data[  PARAM_BYTE_SLOT   +     // Number of connected IMUs
                        6*PARAM_BYTE_SLOT +     // IMUs ID  (1 port instead of 6)
                        9*PARAM_BYTE_SLOT +     // Mag cal parameters (1 row instead of 9)
                        PARAM_BYTE_SLOT   +     // 1 - Device ID
                        PARAM_BYTE_SLOT*N_IMU_MAX + // IMU configurations
                        PARAM_BYTE_SLOT   +     // SPI read delay
                        PARAM_MENU_SLOT   + PARAM_BYTE_SLOT + 1 ] = "";
    uint16 num_imus_id_params = 6;      //(1 port instead of 6)
    uint16 num_mag_cal_params = 0;
    uint16 first_imu_parameter = 2;
    uint16 packet_length = PARAM_BYTE_SLOT +
                           num_imus_id_params*PARAM_BYTE_SLOT +
                           num_mag_cal_params*PARAM_BYTE_SLOT +
                           PARAM_BYTE_SLOT   + 
                           (uint16)(PARAM_BYTE_SLOT*N_IMU_Connected) +
                           PARAM_BYTE_SLOT + PARAM_MENU_SLOT + PARAM_BYTE_SLOT + 1;

    //Auxiliary variables
    uint16 CYDATA i, j, k, h;
    uint16 start_byte = 0;

    //Parameters menu string definitions
    char n_imu_str[26]          = "Number of connected IMUs:";
    char ids_str[11]            = "";
    char mag_param_str[20]      = "Mag cal parameters:";
    char id_str[16]             = "";
    char imu_table_str[42]      = "";
    char spi_read_delay_str[26] = "";
    
    //Strings lenghts
    uint8 CYDATA id_str_len = strlen(id_str);
    uint8 CYDATA n_imu_str_len = strlen(n_imu_str);
    uint8 CYDATA ids_str_len = strlen(ids_str);
    uint8 CYDATA mag_param_str_len = strlen(mag_param_str);
    uint8 CYDATA imu_table_str_len = strlen(imu_table_str);
    uint8 CYDATA spi_read_delay_str_len = strlen(spi_read_delay_str);
    
    char spi_delay_menu[118]    = "";
    uint8 CYDATA spi_delay_menu_len;
 
    sprintf(spi_delay_menu, "0 -> None\n1 -> Low (%u us delay for each 8-bit register read)\n2 -> High (%u us delay for each 8-bit register read)\n", (int)SPI_DELAY_LOW, (int)SPI_DELAY_HIGH);
    spi_delay_menu_len = strlen(spi_delay_menu);

    // Compute number of read parameters depending on N_IMU_Connected and
    // update packet_length
    num_mag_cal_params = (uint16)(N_IMU_Connected / 2);
    if ( (N_IMU_Connected - num_mag_cal_params*2) > 0 ) num_mag_cal_params++;
    
    packet_length = PARAM_BYTE_SLOT +
                    num_imus_id_params*PARAM_BYTE_SLOT +
                    num_mag_cal_params*PARAM_BYTE_SLOT +
                    PARAM_BYTE_SLOT   + 
                    (uint16)(PARAM_BYTE_SLOT*N_IMU_Connected) +
                    PARAM_MENU_SLOT + PARAM_BYTE_SLOT + 1;

    first_imu_parameter = 1 + num_imus_id_params + num_mag_cal_params + 2;
    packet_data[0] = CMD_GET_PARAM_LIST;
    packet_data[1] = 1 + num_imus_id_params + num_mag_cal_params + 1 + (uint8)N_IMU_Connected + 1;        // NUM_PARAMS

    switch(index) {
        case 0:         //List of all parameters with relative types
            /*-------------N IMU--------------*/
            start_byte = 0;
            packet_data[2] = TYPE_UINT8;
            packet_data[3] = 1;
            packet_data[4] = (uint8)N_IMU_Connected;
            for(i = n_imu_str_len; i != 0; i--)
                packet_data[5 + n_imu_str_len - i] = n_imu_str[n_imu_str_len - i];
                
            /*-------------IMUS ID--------------*/
            start_byte = start_byte + PARAM_BYTE_SLOT;
            i = 0;
            for (k = 0; k < num_imus_id_params; k++){
                sprintf(ids_str, "Port %u ID:", k);
                h = 4;
                ids_str_len = strlen(ids_str);
                packet_data[2+start_byte + PARAM_BYTE_SLOT*k] = TYPE_UINT8;
                packet_data[3+start_byte + PARAM_BYTE_SLOT*k] = 3;
                
                for (j = 3*k; j <= 3*k+2; j++) {  // for each possible imu on port k
                    if (IMU_connected[i] == j) {
                        packet_data[h+start_byte + PARAM_BYTE_SLOT*k] = (uint8)IMU_connected[i];               
                        i++;
                    } 
                    else {
                        packet_data[h+start_byte + PARAM_BYTE_SLOT*k] = 255;
                    }
                    h++;
                }

                //if (IMU_connected[i] >= 3*k && IMU_connected[i] <= 3*k + 2)
                for(j = ids_str_len; j != 0; j--)
                    packet_data[7+start_byte + PARAM_BYTE_SLOT*k + ids_str_len - j] = ids_str[ids_str_len - j];
            }
            
            /*-------------GET MAG PARAM--------------*/
            start_byte = start_byte + PARAM_BYTE_SLOT*num_imus_id_params;
            for (k = 0; k < num_mag_cal_params; k++){
                packet_data[2+start_byte + PARAM_BYTE_SLOT*k] = TYPE_UINT8;
                
                packet_data[3+start_byte + PARAM_BYTE_SLOT*k] = 3;
                packet_data[4+start_byte + PARAM_BYTE_SLOT*k] = (uint8) MagCal[IMU_connected[2*k]][0];
                packet_data[5+start_byte + PARAM_BYTE_SLOT*k] = (uint8) MagCal[IMU_connected[2*k]][1];
                packet_data[6+start_byte + PARAM_BYTE_SLOT*k] = (uint8) MagCal[IMU_connected[2*k]][2];
                
                // check if there is a second value
                if ( N_IMU_Connected < 2*(k+1) ) {
                    // there is only one value
                    for(j = mag_param_str_len; j != 0; j--)
                        packet_data[7+start_byte + PARAM_BYTE_SLOT*k + mag_param_str_len - j] = mag_param_str[mag_param_str_len - j];
                }
                else {
                    // fill the second value
                    packet_data[3+start_byte + PARAM_BYTE_SLOT*k] = 6;
                    packet_data[7+start_byte + PARAM_BYTE_SLOT*k] = (uint8) MagCal[IMU_connected[2*k+1]][0];
                    packet_data[8+start_byte + PARAM_BYTE_SLOT*k] = (uint8) MagCal[IMU_connected[2*k+1]][1];
                    packet_data[9+start_byte + PARAM_BYTE_SLOT*k] = (uint8) MagCal[IMU_connected[2*k+1]][2];
                
                    for(j = mag_param_str_len; j != 0; j--)
                        packet_data[10+start_byte + PARAM_BYTE_SLOT*k + mag_param_str_len - j] = mag_param_str[mag_param_str_len - j];
                }
            }
            
            /*-----------------ID-----------------*/
            
            start_byte = start_byte + PARAM_BYTE_SLOT*num_mag_cal_params;
            sprintf(id_str, "%u - Device ID:", first_imu_parameter-1);
            id_str_len = strlen(id_str);
            packet_data[2+start_byte] = TYPE_UINT8;
            packet_data[3+start_byte] = 1;
            packet_data[4+start_byte] = c_mem.id;
            for(i = id_str_len; i != 0; i--)
                packet_data[5+start_byte + id_str_len - i] = id_str[id_str_len - i];
                
            /*-------------GET IMUS MODE-------------*/
            
            start_byte = start_byte + PARAM_BYTE_SLOT;
            for (i = 0; i < (uint8)N_IMU_Connected; i++){
                sprintf(imu_table_str, "%u - IMU %d configuration:", first_imu_parameter + i, (int) IMU_connected[i]);
                imu_table_str_len = strlen(imu_table_str);
            
                packet_data[(uint16)(2 + start_byte + PARAM_BYTE_SLOT*i)] = TYPE_UINT8;
                packet_data[(uint16)(3 + start_byte + PARAM_BYTE_SLOT*i)] = 5;
                
                packet_data[(uint16)(4 + start_byte + PARAM_BYTE_SLOT*i)] = (uint8)(c_mem.IMU_conf[IMU_connected[i]][0]);
                packet_data[(uint16)(5 + start_byte + PARAM_BYTE_SLOT*i)] = (uint8)(c_mem.IMU_conf[IMU_connected[i]][1]);
                packet_data[(uint16)(6 + start_byte + PARAM_BYTE_SLOT*i)] = (uint8)(c_mem.IMU_conf[IMU_connected[i]][2]);
                packet_data[(uint16)(7 + start_byte + PARAM_BYTE_SLOT*i)] = (uint8)(c_mem.IMU_conf[IMU_connected[i]][3]);
                packet_data[(uint16)(8 + start_byte + PARAM_BYTE_SLOT*i)] = (uint8)(c_mem.IMU_conf[IMU_connected[i]][4]);

                for(j = imu_table_str_len; j != 0; j--)
                    packet_data[(uint16)(9 + start_byte + PARAM_BYTE_SLOT*i + imu_table_str_len - j)] = imu_table_str[imu_table_str_len - j];
            }  
          

            /*-----------------SPI DELAY-----------------*/
            
            start_byte = start_byte + (uint16)(PARAM_BYTE_SLOT*N_IMU_Connected);
            sprintf(spi_read_delay_str, "%u - SPI read delay:", first_imu_parameter+N_IMU_Connected);
            packet_data[2+start_byte] = TYPE_FLAG;
            packet_data[3+start_byte] = 1;
            packet_data[4+start_byte] = c_mem.SPI_read_delay;
            switch(c_mem.SPI_read_delay) {
                case 0: 
                    strcat(spi_read_delay_str, " None"); 
                    spi_read_delay_str_len = 26;
                    break;
                case 1: 
                    strcat(spi_read_delay_str, " Low"); 
                    spi_read_delay_str_len = 25;
                    break;
                case 2: 
                    strcat(spi_read_delay_str, " High");
                    spi_read_delay_str_len = 26;
                    break;
                default:
                    break;
            }            
            for(i = spi_read_delay_str_len; i != 0; i--)
                packet_data[5+start_byte + spi_read_delay_str_len - i] = spi_read_delay_str[spi_read_delay_str_len - i];
            //The following byte indicates the number of menus at the end of the packet to send
            packet_data[5+start_byte + spi_read_delay_str_len] = 1;
                  
            
            /*------------PARAMETERS MENU-----------*/
            start_byte = start_byte + PARAM_BYTE_SLOT;
            for(i = spi_delay_menu_len; i!= 0; i--)
                packet_data[(uint16)(2 + start_byte) + spi_delay_menu_len - i] = spi_delay_menu[spi_delay_menu_len - i];

            packet_data[packet_length - 1] = LCRChecksum(packet_data,packet_length - 1);
            commWrite(packet_data, packet_length);
            UART_RS485_ClearTxBuffer();
        break;

//=========================================================     other_params
        default: 
            
            if (index < first_imu_parameter-1)
                break;
            
            if (index == first_imu_parameter+N_IMU_Connected) {
                g_mem.SPI_read_delay = g_rx.buffer[3];  //SPI read delay - uint8
                break;
            }
            
            if (index == first_imu_parameter-1) {
                g_mem.id = g_rx.buffer[3];          //ID - uint8
            }
            else {
            
                //Set Imu table (index > = first_imu_parameter)
                g_mem.IMU_conf[IMU_connected[index-first_imu_parameter]][0] = g_rx.buffer[3];
                g_mem.IMU_conf[IMU_connected[index-first_imu_parameter]][1] = g_rx.buffer[4];
                g_mem.IMU_conf[IMU_connected[index-first_imu_parameter]][2] = g_rx.buffer[5];
                g_mem.IMU_conf[IMU_connected[index-first_imu_parameter]][3] = g_rx.buffer[6];
                g_mem.IMU_conf[IMU_connected[index-first_imu_parameter]][4] = g_rx.buffer[7];
                
                // Recompute IMU packets dimension
                imus_data_size = 1; //header    
                for (i = 0; i < N_IMU_Connected; i++)
                {
                    single_imu_size[IMU_connected[i]] = 1 + 6*g_mem.IMU_conf[IMU_connected[i]][0] + 6*g_mem.IMU_conf[IMU_connected[i]][1] + 6*g_mem.IMU_conf[IMU_connected[i]][2] + 16*g_mem.IMU_conf[IMU_connected[i]][3] + 2*g_mem.IMU_conf[IMU_connected[i]][4]+ 1;
                    imus_data_size = imus_data_size + single_imu_size[IMU_connected[i]];
                }
                imus_data_size = imus_data_size + 1;    //checksum
            
            }
            
        break;
    }
}

//==============================================================================
//                                                            COMMAND SET ZEROS
//==============================================================================

void setZeros()
{
    uint8 CYDATA i;        // iterator
    
    for(i = 0; i < NUM_OF_SENSORS; ++i) {
        g_mem.m_off[i] = data_encoder_raw[i];
        g_meas.rot[i] = 0;
    }
    reset_last_value_flag = 1;

    sendAcknowledgment(ACK_OK);
}

//==============================================================================
//                                                   PREPARE GENERIC DEVICE INFO
//==============================================================================

void prepare_generic_info(char *info_string)
{
    int i;

    if(c_mem.id != 250){                //To avoid dummy board ping
        char str[100];
        strcpy(info_string, "");
        strcat(info_string, "\r\n");
        strcat(info_string, "Firmware version: ");
        strcat(info_string, VERSION);
        strcat(info_string, ".\r\n\r\n");

        strcat(info_string, "DEVICE INFO\r\n");
        sprintf(str, "ID: %d\r\n", (int) c_mem.id);
        strcat(info_string, str);
        switch(c_mem.right_left){
            case RIGHT_HAND:
                strcat(info_string, "Hand side: RIGHT\r\n");
                break;
            case LEFT_HAND:
                strcat(info_string, "Hand side: LEFT\r\n");
                break;
        }
        strcat(info_string, "PWM rescaling activation: ");
        if(c_mem.activate_pwm_rescaling == MAXON_12V)
            strcat(info_string, "YES\n");
        else
            strcat(info_string, "NO\n");
        
        sprintf(str, "PWM Limit: %d\r\n", (int) dev_pwm_limit);
        strcat(info_string, str);
        strcat(info_string, "\r\n");

        strcat(info_string, "MOTOR INFO\r\n");
        strcat(info_string, "Motor reference");
        
        if(g_mem.control_mode == CONTROL_CURRENT)
            strcat(info_string," - Currents: ");
        else {
            if (g_mem.control_mode == CONTROL_PWM)
                strcat(info_string," - Pwm: ");
            else
                strcat(info_string," - Position: ");
        }

        if(g_mem.control_mode == CONTROL_CURRENT) {
            sprintf(str, "%d ", (int)(g_refOld.curr));
            strcat(info_string,str);
        }
        else {
            if(g_mem.control_mode == CONTROL_PWM) {
                sprintf(str, "%d ", (int)(g_refOld.pwm));
                strcat(info_string,str);
            }
            else {
                sprintf(str, "%d ", (int)(g_refOld.pos >> c_mem.res[0]));
                strcat(info_string,str);
            }
        }
        strcat(info_string,"\r\n");
        strcat(info_string, "\r\n");

        sprintf(str, "Motor enabled: ");
        if (g_ref.onoff & 0x01) {
            strcat(str, "YES\r\n");
        } else {
            strcat(str, "NO\r\n");
        }
        strcat(info_string, str);

        strcat(info_string, "\r\nMEASUREMENTS INFO\r\n");
        strcat(info_string, "Sensor value:\r\n");
        for (i = 0; i < NUM_OF_SENSORS; i++) {
            sprintf(str, "%d -> %d", i+1,
            (int)(g_meas.pos[i] >> c_mem.res[i]));
            strcat(info_string, str);
            strcat(info_string, "\r\n");
        }

        sprintf(str, "Battery Voltage (mV): %ld", (int32) dev_tension );
        strcat(info_string, str);
        strcat(info_string, "\r\n");
        
        sprintf(str, "Full charge power tension (mV): %ld", (int32) pow_tension );
        strcat(info_string, str);
        strcat(info_string, "\r\n");

        sprintf(str, "Current (mA): %ld", (int32) g_meas.curr );
        strcat(info_string, str);
        strcat(info_string, "\r\n");

        strcat(info_string, "\r\nDEVICE PARAMETERS\r\n");

        strcat(info_string, "PID Controller:\r\n");
        if(c_mem.control_mode != CURR_AND_POS_CONTROL) {
            sprintf(str, "P -> %f  ", ((double) c_mem.k_p / 65536));
            strcat(info_string, str);
            sprintf(str, "I -> %f  ", ((double) c_mem.k_i / 65536));
            strcat(info_string, str);
            sprintf(str, "D -> %f\r\n", ((double) c_mem.k_d / 65536));
            strcat(info_string, str);
        }
        else { 
            sprintf(str, "P -> %f  ", ((double) c_mem.k_p_dl / 65536));
            strcat(info_string, str);
            sprintf(str, "I -> %f  ", ((double) c_mem.k_i_dl / 65536));
            strcat(info_string, str);
            sprintf(str, "D -> %f\r\n", ((double) c_mem.k_d_dl / 65536));
            strcat(info_string, str);
        }

        strcat(info_string, "Current PID Controller:\r\n");
        if(c_mem.control_mode != CURR_AND_POS_CONTROL) {
            sprintf(str, "P -> %f  ", ((double) c_mem.k_p_c / 65536));
            strcat(info_string, str);
            sprintf(str, "I -> %f  ", ((double) c_mem.k_i_c / 65536));
            strcat(info_string, str);
            sprintf(str, "D -> %f\r\n", ((double) c_mem.k_d_c / 65536));
            strcat(info_string, str);

        }
        else {
            sprintf(str, "P -> %f  ", ((double) c_mem.k_p_c_dl / 65536));
            strcat(info_string, str);
            sprintf(str, "I -> %f  ", ((double) c_mem.k_i_c_dl / 65536));
            strcat(info_string, str);
            sprintf(str, "D -> %f\r\n", ((double) c_mem.k_d_c_dl / 65536));
            strcat(info_string, str);
        }

        strcat(info_string, "\r\n");

        if (c_mem.activ == 0x01)
            strcat(info_string, "Startup activation: YES\r\n");
        else
            strcat(info_string, "Startup activation: NO\r\n");

        switch(c_mem.input_mode) {
            case INPUT_MODE_EXTERNAL:
                strcat(info_string, "Input mode: USB\r\n");
                break;
            case INPUT_MODE_ENCODER3:
                strcat(info_string, "Input mode: Handle\r\n");
                break;
            case INPUT_MODE_EMG_PROPORTIONAL:
                strcat(info_string, "Input mode: EMG proportional\r\n");
                break;
            case INPUT_MODE_EMG_INTEGRAL:
                strcat(info_string, "Input mode: EMG integral\r\n");
                break;
            case INPUT_MODE_EMG_FCFS:
                strcat(info_string, "Input mode: EMG FCFS\r\n");
                break;
            case INPUT_MODE_EMG_FCFS_ADV:
                strcat(info_string, "Input mode: EMG FCFS ADV\r\n");
                break;
        }

        switch(c_mem.control_mode) {
            case CONTROL_ANGLE:
                strcat(info_string, "Control mode: Position\r\n");
                break;
            case CONTROL_PWM:
                strcat(info_string, "Control mode: PWM\r\n");
                break;
            case CONTROL_CURRENT:
                strcat(info_string, "Control mode: Current\r\n");
                break;
            case CURR_AND_POS_CONTROL:
                strcat(info_string, "Control mode: Position and Current\r\n");
                break;
            default:
                break;
        }

        if (c_mem.double_encoder_on_off)
            strcat(info_string, "Absolute encoder position: YES\r\n");
        else
            strcat(info_string, "Absolute encoder position: NO\r\n");

        sprintf(str, "Motor-Handle Ratio: %d\r\n", (int)c_mem.motor_handle_ratio);
        strcat(info_string, str);

        strcat(info_string, "Sensor resolution:\r\n");
        for (i = 0; i < NUM_OF_SENSORS; ++i) {
            sprintf(str, "%d -> %d", (int) (i + 1), (int) c_mem.res[i]);
            strcat(info_string, str);
            strcat(info_string, "\r\n");
        }

        strcat(info_string, "Measurement Offset:\r\n");
        for (i = 0; i < NUM_OF_SENSORS; ++i) {
            sprintf(str, "%d -> %ld", (int) (i + 1), (int32) c_mem.m_off[i] >> c_mem.res[i]);
            strcat(info_string, str);
            strcat(info_string, "\r\n");
        }
            
        strcat(info_string, "Measurement Multiplier:\r\n");
        for (i = 0; i < NUM_OF_SENSORS; ++i) {
            sprintf(str,"%d -> %f", (int)(i + 1), (float) c_mem.m_mult[i]);
            strcat(info_string, str);
            strcat(info_string,"\r\n");
        }
        
        strcat(info_string, "Current lookup table:\r\n");
        sprintf(str, "p[0] - p[2]: %f, %f, %f\n", c_mem.curr_lookup[0], c_mem.curr_lookup[1], c_mem.curr_lookup[2]);
        strcat(info_string, str);
        sprintf(str, "p[3] - p[5]: %f, %f, %f\n", c_mem.curr_lookup[3], c_mem.curr_lookup[4], c_mem.curr_lookup[5]);
        strcat(info_string, str);
        strcat(info_string,"\r\n");        

        sprintf(str, "Position limit active: %d", (int)g_mem.pos_lim_flag);
        strcat(info_string, str);
        strcat(info_string, "\r\n");

        sprintf(str, "Position limit motor: inf -> %ld  ", (int32)g_mem.pos_lim_inf >> g_mem.res[0]);
        strcat(info_string, str);
        sprintf(str, "sup -> %ld\r\n", (int32)g_mem.pos_lim_sup >> g_mem.res[0]);
        strcat(info_string, str);

        sprintf(str, "Max step pos and neg: %d %d", (int)g_mem.max_step_pos, (int)g_mem.max_step_neg);
        strcat(info_string, str);
        strcat(info_string, "\r\n");

        sprintf(str, "Current limit: %d", (int)g_mem.current_limit);
        strcat(info_string, str);
        strcat(info_string, "\r\n");

        if ((c_mem.input_mode == INPUT_MODE_EMG_PROPORTIONAL) ||
            (c_mem.input_mode == INPUT_MODE_EMG_INTEGRAL) ||
            (c_mem.input_mode == INPUT_MODE_EMG_FCFS) ||
            (c_mem.input_mode == INPUT_MODE_EMG_FCFS_ADV)) {
            sprintf(str, "EMG thresholds [0 - 1024]: %u, %u", g_mem.emg_threshold[0], g_mem.emg_threshold[1]);
            strcat(info_string, str);
            strcat(info_string, "\r\n");

            sprintf(str, "EMG max values [0 - 4096]: %lu, %lu", g_mem.emg_max_value[0], g_mem.emg_max_value[1]);
            strcat(info_string, str);
            strcat(info_string, "\r\n");

            if (g_mem.emg_calibration_flag)
                strcat(info_string, "Calibration enabled: YES\r\n");
            else
                strcat(info_string, "Calibration enabled: NO\r\n");

            sprintf(str, "EMG max speed: %d", (int)g_mem.emg_speed);
            strcat(info_string, str);
            strcat(info_string, "\r\n");
        }
        
        if (c_mem.rest_position_flag) {
    		sprintf(str, "Rest time delay (ms): %d", (int)g_mem.rest_delay);
            strcat(info_string, str);
            strcat(info_string, "\r\n");
            
            sprintf(str, "Rest velocity closure (ticks/sec): %d", (int)g_mem.rest_vel);
            strcat(info_string, str);
            strcat(info_string, "\r\n");
            
            sprintf(str, "Rest position: %d", (int)(g_mem.rest_pos >> c_mem.res[0]));
            strcat(info_string, str);
            strcat(info_string, "\r\n");  
        }

        prepare_counter_info(info_string);

        if (c_mem.read_imu_flag) {
            sprintf(str, "IMU Connected: %d\r\n", (int) N_IMU_Connected);
            strcat(info_string, str);
            
            strcat(info_string, "\r\n");
            
            strcat(info_string, "IMUs CONFIGURATION\r\n");
            for (i=0; i<N_IMU_Connected; i++){
                sprintf(str, "Imu %d \r\n\tID: %d\r\n", i, (int) IMU_connected[i]);
                strcat(info_string, str);
                
                sprintf(str, "\tAccelerometers: ");
                if ((c_mem.IMU_conf[IMU_connected[i]][0]))
                    strcat(str, "YES\r\n");
                else
                    strcat(str, "NO\r\n"); 
                strcat(str, "\tGyroscopes: ");
                if ((c_mem.IMU_conf[IMU_connected[i]][1]))
                    strcat(str, "YES\r\n");
                else
                    strcat(str, "NO\r\n"); 
                strcat(str, "\tMagnetometers: ");
                if ((c_mem.IMU_conf[IMU_connected[i]][2]))
                    strcat(str, "YES\r\n");
                else
                    strcat(str, "NO\r\n");
                strcat(str, "\tQuaternion: ");                
                if ((c_mem.IMU_conf[IMU_connected[i]][3]))
                    strcat(str, "YES\r\n");
                else
                    strcat(str, "NO\r\n");
                strcat(str, "\tTemperature: ");
                if ((c_mem.IMU_conf[IMU_connected[i]][4]))
                    strcat(str, "YES\r\n");
                else
                    strcat(str, "NO\r\n");
                     
                strcat(info_string, str);
            }       
            
            strcat(info_string, "\r\n");
          
            IMU_reading_info(info_string);
        }
        
        sprintf(str, "debug: %ld\r\n", (uint32)timer_value0 - (uint32)timer_value);
        strcat(info_string, str);
      
        strcat(info_string, "\r\n");
        
    }
}

//==============================================================================
//                                                   PREPARE GENERIC DEVICE INFO
//==============================================================================

void prepare_counter_info(char *info_string)
{
    char str[100];  
    int i;
    int step;

    strcat(info_string, "\r\nUSAGE STATISTICS\r\n");
    strcat(info_string, "\r\n");
    
    sprintf(str, "Date of maintenance: %d/%d/20%d\r\n", (int)g_mem.maint_day, (int)g_mem.maint_month, (int)g_mem.maint_year);
    strcat(info_string, str);
            
    sprintf(str, "Last save Time: %d/%d/20%d %d:%d:%d\r\n", (int)g_mem.curr_time[0], (int)g_mem.curr_time[1], (int)g_mem.curr_time[2], (int)g_mem.curr_time[3], (int)g_mem.curr_time[4], (int)g_mem.curr_time[5]);
    strcat(info_string, str);
    
    sprintf(str, "Positions histogram (ticks):\r\n");
    strcat(info_string, str);
    step = ( (int)(g_mem.pos_lim_sup >> g_mem.res[0]) / 10);
    for (i=1; i<=10;i++){
        sprintf(str, "Bin %d [%d-%d]: %lu\r\n", i, step*(i-1)+1, step*(i), g_mem.position_hist[i-1]); 
        strcat(info_string, str);
    }
    strcat(info_string, "\r\n");
    
    sprintf(str, "Current histogram (mA):\r\n");
    strcat(info_string, str);
    step = ( (int)(g_mem.current_limit) / 4);
    for (i=1; i<=4;i++){
        sprintf(str, "Threshold %d [%d-%d]: %lu\r\n", i, step*(i-1), step*(i), g_mem.current_hist[i-1]); 
        strcat(info_string, str);
    }
    strcat(info_string, "\r\n");
            
    sprintf(str, "EMG activations counter: %lu, %lu", g_mem.emg_counter[0], g_mem.emg_counter[1]);
    strcat(info_string, str);
    strcat(info_string, "\r\n");
    
    sprintf(str, "Rest position occurrences: %lu", g_mem.rest_counter);
    strcat(info_string, str);
    strcat(info_string, "\r\n");
    
    sprintf(str, "Angle total displacement (ticks): %lu", g_mem.wire_disp);
    strcat(info_string, str);
    strcat(info_string, "\r\n");
    
    sprintf(str, "Total power on time (sec): %lu", g_mem.total_time_on);
    strcat(info_string, str);
    strcat(info_string, "\r\n");
    
    sprintf(str, "Total rest position time (sec): %lu", g_mem.total_time_rest);
    strcat(info_string, str);
    strcat(info_string, "\r\n");

}

//==============================================================================
//                                                              IMU READING INFO
//==============================================================================

void IMU_reading_info(char *info_string)
{
    char str[300];  
    int i;
                 
    strcat(info_string, "SENSORS INFO\r\n");
    for (i=0; i<N_IMU_Connected; i++){
        sprintf(str, "Imu %d \r\n\tID: %d\r\n", i, (int) IMU_connected[i]);
        strcat(info_string, str);
        
        if ((c_mem.IMU_conf[IMU_connected[i]][0])){
            sprintf(str, "\tAcc: %d\t%d\t%d\r\n", (int16) g_imu[i].accel_value[0], (int16) g_imu[i].accel_value[1],(int16) g_imu[i].accel_value[2]);
            strcat(info_string, str);
        }

        if ((c_mem.IMU_conf[IMU_connected[i]][1])){
            sprintf(str, "\tGyro: %d\t%d\t%d\r\n", (int16) g_imu[i].gyro_value[0], (int16) g_imu[i].gyro_value[1],(int16) g_imu[i].gyro_value[2]);
            strcat(info_string, str);
        }

        if ((c_mem.IMU_conf[IMU_connected[i]][2])){
            sprintf(str, "\tMag: %d\t%d\t%d\r\n", (int16) g_imu[i].mag_value[0], (int16) g_imu[i].mag_value[1],(int16) g_imu[i].mag_value[2]);
            strcat(info_string, str);
        }
        
        if ((c_mem.IMU_conf[IMU_connected[i]][3])){
            sprintf(str, "\tQuat: %.3f\t%.3f\t%.3f\t%.3f\r\n", (float) g_imu[i].quat_value[0], (float) g_imu[i].quat_value[1],(float) g_imu[i].quat_value[2], (float) g_imu[i].quat_value[3]);
            strcat(info_string, str);
        }
        
        if ((c_mem.IMU_conf[IMU_connected[i]][4])){
            sprintf(str, "\tTemperature: %d\r\n", (int16) g_imu[i].temp_value);
            strcat(info_string, str);
        }
    }
    strcat(info_string, "\r\n");
}

//==============================================================================
//                                                     WRITE FUNCTIONS FOR RS485
//==============================================================================

void commWrite_old_id(uint8 *packet_data, uint16 packet_lenght, uint8 old_id)
{
    uint16 CYDATA index;    // iterator

    // frame - start
    UART_RS485_PutChar(':');
    UART_RS485_PutChar(':');
    // frame - ID
    //if(old_id)
        UART_RS485_PutChar(old_id);
    //else
        //UART_RS485_PutChar(g_mem.id);
        
    // frame - length
    UART_RS485_PutChar((uint8)packet_lenght);
    // frame - packet data
    for(index = 0; index < packet_lenght; ++index) {
        UART_RS485_PutChar(packet_data[index]);
    }

    index = 0;

    while(!(UART_RS485_ReadTxStatus() & UART_RS485_TX_STS_COMPLETE) && index++ <= 1000){}

    RS485_CTS_Write(1);
    RS485_CTS_Write(0);
}

void commWrite(uint8 *packet_data, uint16 packet_lenght)
{
    uint16 CYDATA index;    // iterator

    // frame - start
    UART_RS485_PutChar(':');
    UART_RS485_PutChar(':');
    // frame - ID
    UART_RS485_PutChar(g_mem.id);
    // frame - length
    UART_RS485_PutChar((uint8)packet_lenght);
    // frame - packet data
    for(index = 0; index < packet_lenght; ++index) {
        UART_RS485_PutChar(packet_data[index]);
    }

    index = 0;

    while(!(UART_RS485_ReadTxStatus() & UART_RS485_TX_STS_COMPLETE) && index++ <= 1000){}

    RS485_CTS_Write(1);
    RS485_CTS_Write(0);
}

void commWrite_to_cuff(uint8 *packet_data, uint16 packet_lenght)
{
    uint16 CYDATA index;    // iterator

    // frame - start
    UART_RS485_PutChar(':');
    UART_RS485_PutChar(':');
    // frame - ID
    UART_RS485_PutChar(g_mem.id - 1);
    // frame - length
    UART_RS485_PutChar((uint8)packet_lenght);
    // frame - packet data
    for(index = 0; index < packet_lenght; ++index) {
        UART_RS485_PutChar(packet_data[index]);
    }

    index = 0;

    while(!(UART_RS485_ReadTxStatus() & UART_RS485_TX_STS_COMPLETE) && index++ <= 1000){}
}

//==============================================================================
//                                                             CHECKSUM FUNCTION
//==============================================================================

// Performs a XOR byte by byte on the entire vector

uint8 LCRChecksum(uint8 *data_array, uint8 data_length) {
    
    uint8 CYDATA i;
    uint8 CYDATA checksum = 0x00;
    
    for(i = 0; i < data_length; ++i)
       checksum ^= data_array[i];
  
    return checksum;
}


//==============================================================================
//                                                       ACKNOWLEDGMENT FUNCTION
//==============================================================================

void sendAcknowledgment(uint8 value) {
    int packet_lenght = 2;
    uint8 packet_data[2];

    packet_data[0] = value;
    packet_data[1] = value;

    commWrite(packet_data, packet_lenght);
}

//==============================================================================
//                                                                  STORE MEMORY
//==============================================================================


uint8 memStore(int displacement)
{
    int i;  // iterator
    uint8 writeStatus;
    int pages;
    uint8 ret_val = 1;

    // Disable Interrupt
    ISR_RS485_RX_Disable();

    // Stop motor
    PWM_MOTORS_WriteCompare(0);

    // Retrieve temperature for better writing performance
    CySetTemp();

    memcpy( &c_mem, &g_mem, sizeof(g_mem) );

    pages = sizeof(g_mem) / 16 + (sizeof(g_mem) % 16 > 0);

    for(i = 0; i < pages; ++i) {
        writeStatus = EEPROM_Write(&g_mem.flag + 16 * i, i + displacement);
        if(writeStatus != CYRET_SUCCESS) {
            ret_val = 0;
            break;
        }
    }

    memcpy( &g_mem, &c_mem, sizeof(g_mem) );

    // Re-Enable Interrupt
    ISR_RS485_RX_Enable();

    return ret_val;
}


//==============================================================================
//                                                                 RECALL MEMORY
//==============================================================================


void memRecall(void)
{
    uint16 i;

    for (i = 0; i < sizeof(g_mem); i++) {
        ((reg8 *) &g_mem.flag)[i] = EEPROM_ADDR[i];
    }

    //check for initialization
    if (g_mem.flag == FALSE) {
        memRestore();
    } else {
        memcpy( &c_mem, &g_mem, sizeof(g_mem) );
    }
}


//==============================================================================
//                                                                RESTORE MEMORY
//==============================================================================


uint8 memRestore(void) {
    uint16 i;

    for (i = 0; i < sizeof(g_mem); i++) {
        ((reg8 *) &g_mem.flag)[i] = EEPROM_ADDR[i + (DEFAULT_EEPROM_DISPLACEMENT * 16)];
    }

    //check for initialization
    if (g_mem.flag == FALSE) {
        return memInit();
    } else {
        return memStore(0);
    }
}

//==============================================================================
//                                                                   MEMORY INIT
//==============================================================================

uint8 memInit(void)
{
    uint8 i;

    //initialize memory settings
    g_mem.id            = 1;

    g_mem.k_p           =0.0165 * 65536;
    g_mem.k_i           =     0 * 65536;
    g_mem.k_d           = 0.007 * 65536;  // changed in order to avoid metallic clatter, previous value 0.2
    g_mem.k_p_c         =     1 * 65536;
    g_mem.k_i_c         = 0.001 * 65536;
    g_mem.k_d_c         =     0 * 65536;

    g_mem.k_p_dl        =   0.1 * 65536;
    g_mem.k_i_dl        =     0 * 65536;
    g_mem.k_d_dl        =     0 * 65536;
    g_mem.k_p_c_dl      =   0.3 * 65536;
    g_mem.k_i_c_dl      =0.0002 * 65536;
    g_mem.k_d_c_dl      =     0 * 65536;

    g_mem.activ         = 1;
    g_mem.input_mode    = INPUT_MODE_EXTERNAL;
    g_mem.control_mode  = CONTROL_ANGLE;

    g_mem.pos_lim_flag = 1;

    g_mem.activate_pwm_rescaling = MAXON_24V;           //rescaling active for 12V motor

    g_mem.pos_lim_inf = 0;
    g_mem.pos_lim_sup = (int32)22000 << g_mem.res[0];

    for(i = 0; i < NUM_OF_SENSORS; ++i)
    {
        g_mem.res[i] = 3;
        g_mem.m_mult[i] = 1;
        g_mem.m_off[i] = (int32)0 << g_mem.res[i];
    }

    g_mem.max_step_pos = 0;
    g_mem.max_step_neg = 0;

    g_mem.current_limit = DEFAULT_CURRENT_LIMIT;

    // EMG calibration enabled by default
    g_mem.emg_calibration_flag = 0;

    g_mem.emg_max_value[0] = 1024;
    g_mem.emg_max_value[1] = 1024;

    g_mem.emg_threshold[0] = 200;
    g_mem.emg_threshold[1] = 200;

    g_mem.emg_speed = 100;

    g_mem.double_encoder_on_off = 0;
    g_mem.motor_handle_ratio = 22;
    
    for(i = 0; i < LOOKUP_DIM; i++) {
        g_mem.curr_lookup[i] = 0;
    }

    //Initialize rest position parameters        
    g_mem.rest_pos = (int32)7000 << g_mem.res[0]; // 56000
	g_mem.rest_delay = 10;
    g_mem.rest_vel = 10000;
    g_mem.switch_emg = 0;
	
    g_mem.right_left = RIGHT_HAND;
    g_mem.read_imu_flag = FALSE;
    g_mem.read_exp_port_flag = EXP_NONE;       // 0 - None
    
    for (i=0; i<23; i++){
        g_mem.unused_bytes[i] = 0;
    }

    //Initialize counters        
    reset_counters();
    for (i=0; i<10; i++){
        g_mem.unused_bytes1[i] = 0;
    }
    
    if (g_mem.read_exp_port_flag == EXP_SD_RTC) {
        // Set date of maintenance from RTC
        i = DS1302_read(DS1302_DATE_RD);
        g_mem.maint_day = (i/16) * 10 + i%16;
        i = DS1302_read(DS1302_MONTH_RD);
        g_mem.maint_month = (i/16) * 10 + i%16;
        i = DS1302_read(DS1302_YEAR_RD);
        g_mem.maint_year = (i/16) * 10 + i%16;
    }
    
    // IMU Default value
    for (i = 0; i< N_IMU_MAX; i++){
        g_mem.IMU_conf[i][0] = 1;
        g_mem.IMU_conf[i][1] = 1;
        g_mem.IMU_conf[i][2] = 0;
        g_mem.IMU_conf[i][3] = 0;
        g_mem.IMU_conf[i][4] = 0;
        //for (j=0; j< NUM_OF_DATA; j++) {
        //    g_mem.IMU_conf[i][j] = 0;
        //}
    }
    
    // set the initialized flag to show EEPROM has been populated
    g_mem.flag = TRUE;
    
    //write that configuration to EEPROM
    return ( memStore(0) && memStore(DEFAULT_EEPROM_DISPLACEMENT) );
}

//==============================================================================
//                                                    ROUTINE INTERRUPT FUNCTION
//==============================================================================
/**
* Bunch of functions used on request from UART communication
**/

void cmd_get_measurements(){
 
    uint8 CYDATA index;
    int16 aux_int16;
    // Packet: header + measure(int16) + crc
    
    uint8 packet_data[8]; 
    
    //Header package
    packet_data[0] = CMD_GET_MEASUREMENTS;   
    
    for (index = NUM_OF_SENSORS; index--;) {
        aux_int16 = (int16)(g_measOld.pos[index] >> g_mem.res[index]);
        packet_data[(index << 1) + 2] = ((char*)(&aux_int16))[0];
        packet_data[(index << 1) + 1] = ((char*)(&aux_int16))[1];
        //*((int16 *) &packet_data[(index << 1) + 1]) = (int16)(g_measOld.pos[index] >> g_mem.res[index]);
    }

    // Calculate Checksum and send message to UART 

    packet_data[7] = LCRChecksum (packet_data, 7);

    commWrite(packet_data, 8);
   
}

void cmd_get_velocities(){
 
    uint8 CYDATA index;
    int16 aux_int16;
   
    // Packet: header + measure(int16) + crc
    
    uint8 packet_data[8]; 
    
    //Header package
    packet_data[0] = CMD_GET_VELOCITIES;
    
    for (index = NUM_OF_SENSORS; index--;) {
        aux_int16 = (int16)(g_measOld.vel[index] >> g_mem.res[index]);
        packet_data[(index << 1) + 2] = ((char*)(&aux_int16))[0];
        packet_data[(index << 1) + 1] = ((char*)(&aux_int16))[1];
        //*((int16 *) &packet_data[(index << 1) + 1]) = (int16) (g_measOld.vel[index] >> g_mem.res[index]);
    }
            
    // Calculate Checksum and send message to UART 

    packet_data[7] = LCRChecksum (packet_data, 7);

    commWrite(packet_data, 8); 
}

void cmd_get_accelerations(){
 
    uint8 CYDATA index;
    int16 aux_int16;
    
    // Packet: header + measure(int16) + crc
    
    uint8 packet_data[8]; 
    
    //Header package
    packet_data[0] = CMD_GET_ACCEL;   
    
    for (index = NUM_OF_SENSORS; index--;) {
        aux_int16 = (int16)(g_measOld.acc[index] >> g_mem.res[index]);
        packet_data[(index << 1) + 2] = ((char*)(&aux_int16))[0];
        packet_data[(index << 1) + 1] = ((char*)(&aux_int16))[1];
        //*((int16 *) &packet_data[(index << 1) + 1]) = (int16)(g_measOld.acc[index] >> g_mem.res[index]);
    }
    
    // Calculate Checksum and send message to UART 

    packet_data[7] = LCRChecksum (packet_data, 7);

    commWrite(packet_data, 8); 
}

void cmd_set_inputs(){
    
    // Store position setted in right variables
    int16 aux_int16 = (int16)(g_rx.buffer[1]<<8 | g_rx.buffer[2]);
    
    if(g_mem.control_mode == CONTROL_CURRENT) {
        g_refNew.curr = aux_int16;
        //g_refNew.pwm = *((int16 *) &g_rx.buffer[1]);
    }
    else {
        if(g_mem.control_mode == CONTROL_PWM) {
            g_refNew.pwm = aux_int16;
            //g_refNew.pwm = *((int16 *) &g_rx.buffer[1]);
        }
        else {
            g_refNew.pos = aux_int16;   // motor 1
            //g_refNew.pos = *((int16 *) &g_rx.buffer[1]);   // motor 1
            g_refNew.pos = g_refNew.pos << g_mem.res[0];
        }
    }     

    // Check if the reference is nor higher or lower than the position limits
    if (c_mem.pos_lim_flag && (g_mem.control_mode == CURR_AND_POS_CONTROL || g_mem.control_mode == CONTROL_ANGLE)) { 
        
        if (g_refNew.pos < c_mem.pos_lim_inf) 
            g_refNew.pos = c_mem.pos_lim_inf;

        if (g_refNew.pos > c_mem.pos_lim_sup) 
            g_refNew.pos = c_mem.pos_lim_sup;
    }
    
    change_ext_ref_flag = TRUE;
}

void cmd_activate(){
    
    // Store new value reads
    g_refNew.onoff = g_rx.buffer[1];
    
    // Check type of control mode enabled
    if ((g_mem.control_mode == CONTROL_ANGLE) || (g_mem.control_mode == CURR_AND_POS_CONTROL)) {
        g_refNew.pos = g_meas.pos[0];
    }

    // Activate/Disactivate motor
    MOTOR_ON_OFF_Write(g_refNew.onoff);
}

void cmd_get_activate(){
    
    uint8 packet_data[3];

    // Header        
    packet_data[0] = CMD_GET_ACTIVATE;
    
    // Fill payload
    packet_data[1] = g_ref.onoff;
    
    // Calculate checksum
    packet_data[2] = LCRChecksum(packet_data, 2);
    
    // Send package to UART
    commWrite(packet_data, 3);

}

void cmd_get_curr_and_meas(){
    
    uint8 CYDATA index;
    int16 aux_int16;
   
    //Packet: header + curr_meas(int16) + pos_meas(int16) + CRC
    
    uint8 packet_data[12]; 

    //Header package
    packet_data[0] = CMD_GET_CURR_AND_MEAS;

    // Currents
    aux_int16 = (int16) g_measOld.curr; //Real current
    packet_data[2] = ((char*)(&aux_int16))[0];
    packet_data[1] = ((char*)(&aux_int16))[1];
    //*((int16 *) &packet_data[1]) = (int16) g_measOld.curr; //Real current
    
    aux_int16 = (int16) g_measOld.estim_curr; //Estimated current
    packet_data[4] = ((char*)(&aux_int16))[0];
    packet_data[3] = ((char*)(&aux_int16))[1];
    //*((int16 *) &packet_data[3]) = (int16) g_measOld.estim_curr; //Estimated current

    // Positions
    for (index = NUM_OF_SENSORS; index--;) {
        aux_int16 = (int16)(g_measOld.pos[index] >> g_mem.res[index]);
        packet_data[(index << 1) + 6] = ((char*)(&aux_int16))[0];
        packet_data[(index << 1) + 5] = ((char*)(&aux_int16))[1];
        //*((int16 *) &packet_data[(index << 1) + 5]) = (int16)(g_measOld.pos[index] >> g_mem.res[index]);
    }
    // Calculate Checksum and send message to UART 
        
    packet_data[11] = LCRChecksum (packet_data, 11);
    commWrite(packet_data, 12);
   
}

void cmd_get_currents(){
    
    // Packet: header + motor_measure(int16) + crc
    
    uint8 packet_data[6]; 
    int16 aux_int16;
    
    //Header package

    packet_data[0] = CMD_GET_CURRENTS;

    // Currents
    aux_int16 = (int16) g_measOld.curr; //Real current
    packet_data[2] = ((char*)(&aux_int16))[0];
    packet_data[1] = ((char*)(&aux_int16))[1];
    //*((int16 *) &packet_data[1]) = (int16) g_measOld.curr; //Real current
    
    aux_int16 = (int16) g_measOld.estim_curr; //Estimated current
    packet_data[4] = ((char*)(&aux_int16))[0];
    packet_data[3] = ((char*)(&aux_int16))[1];
    //*((int16 *) &packet_data[3]) = (int16) g_measOld.estim_curr; //Estimated current

    // Calculate Checksum and send message to UART 

    packet_data[5] = LCRChecksum (packet_data, 5);
    
    commWrite(packet_data, 6);
}

void cmd_get_currents_for_cuff(){
    
    // Packet: header + motor_measure(int16) + crc
    
    uint8 packet_data[4]; 
    int16 aux_int16;
    
    //Header package

    packet_data[0] = CMD_SET_CUFF_INPUTS;

    aux_int16 = (int16) g_measOld.estim_curr; //Estimated Current
    packet_data[2] = ((char*)(&aux_int16))[0];
    packet_data[1] = ((char*)(&aux_int16))[1];
   // *((int16 *) &packet_data[1]) = (int16) g_measOld.estim_curr; //Estimated Current

    // Calculate Checksum and send message to UART 

    packet_data[3] = LCRChecksum (packet_data, 3);
    
    commWrite_to_cuff(packet_data, 4);
}

void cmd_set_baudrate(){
    
    // Set BaudRate
    c_mem.baud_rate = g_rx.buffer[1];
    
    switch(g_rx.buffer[1]){
        case 13:
            CLOCK_UART_SetDividerValue(13);
            break;
        default:
            CLOCK_UART_SetDividerValue(3);
    }
}

void cmd_ping(){

    uint8 packet_data[2];

    // Header
    packet_data[0] = CMD_PING;
    
    // Load Payload
    packet_data[1] = CMD_PING;

    // Send Package to uart
    commWrite(packet_data, 2);
}

void cmd_get_inputs(){

    // Packet: header + motor_measure(int16) + crc

    uint8 packet_data[6]; 
    int16 aux_int16;
    
    //Header package

    packet_data[0] = CMD_GET_INPUTS;
        
    aux_int16 = (int16) (g_refOld.pos  >> g_mem.res[0]);
    packet_data[2] = ((char*)(&aux_int16))[0];
    packet_data[1] = ((char*)(&aux_int16))[1];
    //*((int16 *) &packet_data[1]) = (int16) (g_refOld.pos  >> g_mem.res[0]);
    
    // Calculate Checksum and send message to UART 

    packet_data[5] = LCRChecksum(packet_data, 5);

    commWrite(packet_data, 6);
}

void cmd_store_params(){
    
    // Check input mode enabled
    uint32 off_1;
    float mult_1;
    uint8 CYDATA packet_lenght = 2;
    uint8 CYDATA packet_data[2];
    uint8 CYDATA old_id;
    
    if( c_mem.input_mode == INPUT_MODE_EXTERNAL ) {
        off_1 = c_mem.m_off[0];
        mult_1 = c_mem.m_mult[0];

        g_refNew.pos = (int32)((float)g_refNew.pos / mult_1);

        g_refNew.pos = (int32)((float)g_refNew.pos * g_mem.m_mult[0]);

        g_refNew.pos += (g_mem.m_off[0] - off_1);
        
        // Check position Limits

        if (c_mem.pos_lim_flag) {                   // position limiting
            if (g_refNew.pos < c_mem.pos_lim_inf) 
                g_refNew.pos = c_mem.pos_lim_inf;

            if (g_refNew.pos > c_mem.pos_lim_sup) 
                g_refNew.pos = c_mem.pos_lim_sup;
        }
    }
    // Store params 

    if (c_mem.id != g_mem.id) {     //If a new id is going to be set we will lose communication 
        old_id = c_mem.id;          //after the memstore(0) and the ACK won't be recognised
        if(memStore(0)) {
            packet_data[0] = ACK_OK;
            packet_data[1] = ACK_OK;
            commWrite_old_id(packet_data, packet_lenght, old_id);
        }    
        else{
            packet_data[0] = ACK_ERROR;
            packet_data[1] = ACK_ERROR;
            commWrite_old_id(packet_data, packet_lenght, old_id);
        }
    }    
    else {
        if(memStore(0))
            sendAcknowledgment(ACK_OK);
        else
            sendAcknowledgment(ACK_ERROR);
    }
}

void cmd_get_emg(){
    
    uint8 packet_data[6];
    int16 aux_int16;
    
    // Header        
    packet_data[0] = CMD_GET_EMG;
    
    aux_int16 = (int16) g_measOld.emg[0];
    packet_data[2] = ((char*)(&aux_int16))[0];
    packet_data[1] = ((char*)(&aux_int16))[1];
    //*((int16 *) &packet_data[1]) = (int16) g_measOld.emg[0];
    
    aux_int16 = (int16) g_measOld.emg[1];
    packet_data[4] = ((char*)(&aux_int16))[0];
    packet_data[3] = ((char*)(&aux_int16))[1];
    //*((int16 *) &packet_data[3]) = (int16) g_measOld.emg[1];

    packet_data[5] = LCRChecksum (packet_data, 5);

    commWrite(packet_data, 6);

}


void cmd_get_imu_readings(){
    //Retrieve accelerometers, gyroscopes and magnetometers readings
    
    uint8 k_imu;
    uint8 c = 1;
    uint8 k = 0;
    uint8 gl_c = 1;
    int16 aux_int16;
    float aux_float;
    
    // Packet: header + imu id(uint8) + imu flags(uint8) + crc  
    uint8 packet_data[350];
    uint8 single_packet[22];
    
    //Header package 
    packet_data[0] = CMD_GET_IMU_READINGS;
 
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++) 
    {	
        
        single_packet[0] = (uint8) 0x3A; //':';
        if (c_mem.IMU_conf[IMU_connected[k_imu]][0]){
            aux_int16 = (int16) g_imu[k_imu].accel_value[0];
            single_packet[c + 1] = ((char*)(&aux_int16))[0];
            single_packet[c] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c]) = (int16) g_imu[k_imu].accel_value[0];
            aux_int16 = (int16) g_imu[k_imu].accel_value[1];
            single_packet[c + 3] = ((char*)(&aux_int16))[0];
            single_packet[c + 2] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c+2]) = (int16) g_imu[k_imu].accel_value[1];
            aux_int16 = (int16) g_imu[k_imu].accel_value[2];
            single_packet[c + 5] = ((char*)(&aux_int16))[0];
            single_packet[c + 4] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c+4]) = (int16) g_imu[k_imu].accel_value[2];
            c = c + 6;
        }
        if (c_mem.IMU_conf[IMU_connected[k_imu]][1]){
            aux_int16 = (int16) g_imu[k_imu].gyro_value[0];
            single_packet[c + 1] = ((char*)(&aux_int16))[0];
            single_packet[c] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c])   = (int16) g_imu[k_imu].gyro_value[0];
            aux_int16 = (int16) g_imu[k_imu].gyro_value[1];
            single_packet[c + 3] = ((char*)(&aux_int16))[0];
            single_packet[c + 2] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c+2]) = (int16) g_imu[k_imu].gyro_value[1];
            aux_int16 = (int16) g_imu[k_imu].gyro_value[2];
            single_packet[c + 5] = ((char*)(&aux_int16))[0];
            single_packet[c + 4] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c+4]) = (int16) g_imu[k_imu].gyro_value[2];
            c = c + 6;
        }
        if (c_mem.IMU_conf[IMU_connected[k_imu]][2]){
            aux_int16 = (int16) g_imu[k_imu].mag_value[0];
            single_packet[c + 1] = ((char*)(&aux_int16))[0];
            single_packet[c] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c])   = (int16) g_imu[k_imu].mag_value[0];
            aux_int16 = (int16) g_imu[k_imu].mag_value[1];
            single_packet[c + 3] = ((char*)(&aux_int16))[0];
            single_packet[c + 2] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c+2]) = (int16) g_imu[k_imu].mag_value[1];
            aux_int16 = (int16) g_imu[k_imu].mag_value[2];
            single_packet[c + 5] = ((char*)(&aux_int16))[0];
            single_packet[c + 4] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c+4]) = (int16) g_imu[k_imu].mag_value[2];
            c = c + 6;
        }
        if (c_mem.IMU_conf[IMU_connected[k_imu]][3]){
            aux_float = (float) g_imu[k_imu].quat_value[0];
            for(k = 0; k < 4; k++) {
                single_packet[c + 4 - k -1] = ((char*)(&aux_float))[k];
            }
            //*((float *) &single_packet[c])   = (float) g_imu[k_imu].quat_value[0];
            aux_float = (float) g_imu[k_imu].quat_value[1];
            for(k = 0; k < 4; k++) {
                single_packet[c + 4 - k -1] = ((char*)(&aux_float))[k];
            }
            //*((float *) &single_packet[c+4]) = (float) g_imu[k_imu].quat_value[1];
            aux_float = (float) g_imu[k_imu].quat_value[2];
            for(k = 0; k < 4; k++) {
                single_packet[c + 4 - k -1] = ((char*)(&aux_float))[k];
            }
            //*((float *) &single_packet[c+8]) = (float) g_imu[k_imu].quat_value[2];
            aux_float = (float) g_imu[k_imu].quat_value[3];
            for(k = 0; k < 4; k++) {
                single_packet[c + 4 - k -1] = ((char*)(&aux_float))[k];
            }
            //*((float *) &single_packet[c+12]) = (float) g_imu[k_imu].quat_value[3];
            c = c + 16;
        }
        if (c_mem.IMU_conf[IMU_connected[k_imu]][4]){
            aux_int16 = (int16) g_imu[k_imu].temp_value;
            single_packet[c + 1] = ((char*)(&aux_int16))[0];
            single_packet[c] = ((char*)(&aux_int16))[1];
            //*((int16 *) &single_packet[c])   = (int16) g_imu[k_imu].temp_value;
            c = c + 2;
        }
        single_packet[single_imu_size[IMU_connected[k_imu]] - 1] = (uint8) 0x3A; //':';
        c = 1;
               
        for(k=0; k < single_imu_size[IMU_connected[k_imu]]; k++) {
            packet_data[gl_c + k] = (uint8) single_packet[k]; 
        }
        gl_c = gl_c + single_imu_size[IMU_connected[k_imu]];  
        
        memset(&single_packet, 0, sizeof(single_packet));     
        
    }

    // Calculate Checksum and send message to UART 
    packet_data[imus_data_size-1] = LCRChecksum (packet_data, imus_data_size-1);
    commWrite(packet_data, imus_data_size);
}

/* [] END OF FILE */