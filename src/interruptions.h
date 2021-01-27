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
* \file         interruptions.h
*
* \brief        Interruptions header file.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

#ifndef INTERRUPTIONS_H_INCLUDED
#define INTERRUPTIONS_H_INCLUDED

//==================================================================     include
#include "device.h"   
#include "command_processing.h"
#include "IMU_functions.h"
#include "Encoder_functions.h"
#include "SD_RTC_functions.h"
#include "globals.h"
#include "utils.h"
    
//=====================================================        Interrupt Declaration
 
/** \name Interruptions */
/** \{ */
//====================================================     RS485 interruption
/** This interruption sets a flag to let the firmware know that a communication 
 *	interruption is pending and needs to be handled. The interruption will be
 *	handled in predefined moments during the firmware execution. 
 * 	When this interruption is handled, it unpacks the package received on the 
 *  RS485 communication bus.  
**/
CY_ISR_PROTO(ISR_RS485_RX_ExInterrupt);

//====================================================      Cycles timer interruption
/** This interruption sets a flag to let the firmware know that a cycles timer
 *	interruption is pending and needs to be handled. The interrpution will be 
 * 	handled in predefined moments during the firmware execution.
 *  When this interruption is handled, it updates cycles counters. 
**/
CY_ISR_PROTO(ISR_CYCLES_Handler);

/** \} */


//=====================================================     Functions Declarations

/** \name General function scheduler */
/** \{ */
//=====================================================     function_scheduler
/** This function schedules the other functions in an order that optimizes the 
 *	controller usage.	
**/
void function_scheduler(void);
/** \} */

/** \name Encoder reading SPI function */
/** \{ */
//=====================================================     encoder_reading_SPI
/** This functions reads the value from all the connected encoders.
**/
void encoder_reading_SPI(uint8 n_line, uint8 assoc_motor);
/** \} */


/** \name Motor control function */
/** \{ */
//=====================================================     compute_reference
/** This function computes the motor reference and stores it in the given 
 *  st_ref structure.
**/
void compute_reference(uint8 motor_idx, struct st_ref* st_ref_p, struct st_ref* st_refOld_p);

//=====================================================     compute_SoftHand_2_motors_joystick_reference
/** This function computes the motor reference for SoftHand_2_motors device with joystick input 
 * and stores it in the given st_ref structure.
**/
void compute_SoftHand_2_motors_joystick_reference(uint8 motor_idx, struct st_ref* st_ref_p, struct st_ref* st_refOld_p);

//=====================================================     compute_SoftHand_2_motors_emg_reference
/** This function computes the motor reference for SoftHand_2_motors device with EMG input 
 * and stores it in the given st_ref structure.
**/
void compute_SoftHand_2_motors_emg_reference(uint8 motor_idx, struct st_ref* st_ref_p, struct st_ref* st_refOld_p, int32 err_emg_1, int32 err_emg_2);

//=====================================================     motor_control_SH
/** This function controls the motor direction and velocity, depending on 
 * 	the input and control modality set. 
**/
void motor_control_SH();
/** \} */

//=====================================================     motor_control_generic
/** This function controls the motor direction and velocity, depending on 
 * 	the input and control modality set. 
**/
void motor_control_generic(uint8 index);
/** \} */

/** \name Analog readings */
/** \{ */
//=====================================================     analog_read_end
/** This function executes and terminates the analog readings.
**/
void analog_read_end();


/** \name Interrupt manager */
/** \{ */ 
//=====================================================     interrupt_manager
/** This function is called in predefined moments during firmware execution
 *  in order to unpack the received package. 
**/
void interrupt_manager();
/** \} */

/** \name Utility functions */
/** \{ */
//=====================================================     pwm_limit_search
/** This function scales the pwm value of the motor, depending on the power 
 * 	supply voltage, in order to not make the motor wind too fast.
**/
void pwm_limit_search(uint8 mot_idx);

//=====================================================     overcurrent_control
/** This function increases or decreases the pwm maximum value, depending on the current
 * 	absorbed by the motor.
**/
void overcurrent_control();

//=====================================================     cycles_counter_update
/** This function increases the cycles counters variables, depending on SoftHand position and 
 *  the current absorbed by the motor.
**/
void cycles_counter_update();
/** \} */

//=====================================================     save_cycles_eeprom
/** This function saves cycles counters variables into EEPROM memory.
**/
void save_cycles_eeprom();
/** \} */

// ----------------------------------------------------------------------------

#endif

/* [] END OF FILE */