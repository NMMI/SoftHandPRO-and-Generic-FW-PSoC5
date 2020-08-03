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
* \file         Encoder_functions.c
*
* \brief        Implementation of SPI module functions.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

#include "Encoder_functions.h"

/********************************************************************************
* Function Name: Change_CS_EncoderLine                                                   *
*********************************************************************************/
void Change_CS_EncoderLine(int n)
{        
    // Change CS (only if necessary)
    if (Chip_Select_ENCODER_LINE_Read() != n) {        
        RESET_COUNTERS_Write(0x01);     // Stop COUNTER_ENC
        CyDelayUs(10);
        Chip_Select_ENCODER_LINE_Write(n);
        RESET_COUNTERS_Write(0x00);    // Activate encoder counters. Must be made before initializing measurements to zero.
        CyDelayUs(200);                // Wait for encoders to have a valid value.
    }
}

/*******************************************************************************
* Function Name: Encoder Reset
*********************************************************************************/
void EncoderReset() {
	    
    CyDelay(10);
    InitEncoderGeneral();
    CyDelay(10);
}

/********************************************************************************
* Function Name: InitEncoderGeneral                                                 *
*********************************************************************************/
void InitEncoderGeneral()
{
    uint8 k_encoder_line=0;

    for (k_encoder_line=0; k_encoder_line<N_ENCODER_LINE_MAX; k_encoder_line++) {
    
        // SSI ENCODERS
        RESET_COUNTERS_Write(0x01);     // Stop COUNTER_ENC
        Chip_Select_ENCODER_LINE_Write(k_encoder_line);
        RESET_COUNTERS_Write(0x00);     // Activate encoder counters. Must be made before initializing measurements to zero.
        CyDelay(5);                // Wait for encoders to have a valid value.
        
    	InitEncoderLine(k_encoder_line);
    	CyDelay(5);
    }
    CyDelay(100);
}

/*******************************************************************************
* Function Name: Encoder Initialization
*********************************************************************************/
void InitEncoderLine(uint8 n){
    
    uint8 i = 0;
    
    ReadEncoderLine(N_ENCODERS_PER_LINE_MAX, n);
    for (i = 0; i < N_ENCODERS_PER_LINE_MAX; i++) {

        if (Encoder_Value[n][i] != 0xFFF && Encoder_Check[n][i] > 15){ // Encoder is present and ready
            N_Encoder_Line_Connected[n]++;
        }
        else {
            break;
        }
    }
}

/*******************************************************************************
* Function Name: Read Encoder
*********************************************************************************/
void ReadEncoderLine(int n_encoders, int n_line)
{   
	uint32 enc_rx_buffer[3] = {0,0,0};    // One for each Shift register
    uint8 i = 0;
        
    enc_rx_buffer[0] = SHIFTREG_ENC_1_ReadData();
    enc_rx_buffer[1] = SHIFTREG_ENC_2_ReadData();
    enc_rx_buffer[2] = SHIFTREG_ENC_3_ReadData();

    // Parse data into Encoder_Check and Encoder_Value vectors
    // Discard first bit of whole packet, parity bits and dummy bits between encoders data
    for (i = 0; i < n_encoders; i++) {
        switch(i){
            case 0:     // First encoder in the line
                Encoder_Value[n_line][0] = (uint16)((enc_rx_buffer[0] & 0x7FFFF000) >> 19);
                Encoder_Check[n_line][0] = (uint8)((enc_rx_buffer[0] & 0x0007C000) >> 14);
            break;
            case 1:     // Second encoder in the line
                Encoder_Value[n_line][1] = (uint16)((enc_rx_buffer[0] & 0x00000FFF));
                Encoder_Check[n_line][1] = (uint8)((enc_rx_buffer[1] & 0xF8000000) >> 27);
            break;
            case 2:     // Third encoder in the line
                Encoder_Value[n_line][2] = (uint16)((enc_rx_buffer[1] & 0x01FFE000) >> 13);
                Encoder_Check[n_line][2] = (uint8)((enc_rx_buffer[1] & 0x00001F00) >> 8);
            break;
            case 3:     // Fourth encoder in the line
                Encoder_Value[n_line][3] = (uint16)((enc_rx_buffer[1] & 0x0000003F));
                Encoder_Value[n_line][3] = (Encoder_Value[n_line][3] << 6) | (uint16)((enc_rx_buffer[2] & 0xFC000000) >> 26);
                Encoder_Check[n_line][3] = (uint8)((enc_rx_buffer[2] & 0x03E00000) >> 21);
            break;
            case 4:     // Fifth encoder in the line
                Encoder_Value[n_line][4] = (uint16)((enc_rx_buffer[2] & 0x0007FF80) >> 7);
                Encoder_Check[n_line][4] = (uint8)((enc_rx_buffer[2] & 0x0000007C) >> 2);
            break;
            default:
            break;
        }
    }
}