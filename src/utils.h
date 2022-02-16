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
* \file         utils.h
*
* \brief        Utility functions declaration.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "globals.h"
#include <math.h>
    
//------------------------------------------------------------------     DEFINES
#define ZMAX 5				/*!< Constant useful for current estimation procedure.*/
#define ZERO_TOL 100		/*!< Deadband used to put to zero the virtual position
								 due to the fact that the friction model has errors when the
								 position is near to zero.*/
#define REFSPEED 20			/*!< Constant depending on PID values.*/

#define SIGN(A) (((A) >=0) ? (1) : (-1))	/*!< Sign calculation function.*/

//-------------------------------------------------------------     DECLARATIONS

/** \name Filters */
/** \{ */

/** Generic low pass filter. The weighted average between the 
 *	old value and the new one is executed.
 * 	
 *	\param value	New value of the filter.
 *  \param f        Pointer to specific struct of type st_filter.    
 *
 * 	\return Returns the filtered current value
**/
int32 filter(int32 value, struct st_filter *f);


/** \name Estimating current and difference */
/** \{ */

/** Function used to obtain current estimation through current lookup table.
 *
 * 	\param idx 			Index of motor.
 * 	\param pos 			Position of the encoder in ticks.
 *	\param vel 			Speed of the encoder.
 *	\param accel 		Acceleration of the encoder
 *
 * 	\return Returns an estimation of the motor current, depending on its position, velocity
 * 			and acceleration.
**/
int32 curr_estim( uint8 idx, int32 pos, int32 vel, int32 acc);


/** \name Utility functions */
/** \{ */

/** This function computes the module function, returning positive values regardless
 * 	of wheter the value passed is negative
 * 
 * 	\param val 		The value of which the module needs to be calculated
 * 	\param divisor	The divisor according to which the module is calculated
**/
uint32 my_mod(int32 val, int32 divisor);

/**	This function is used at startup to reconstruct the correct turn of the shaft
 *  connected to the motor. Only for SoftHand 3.0. It need two encoders to work.
 *
 * \param pos1		First encoder position
 * \param pos2		Second encoder position
 * 
 * \return Returns the number of turns of motor pulley at startup
**/
int calc_turns_fcn_SH(const int32 pos1, const int32 pos2, const int N1, const int N2, const int I1);

/**	This function is used at startup to reconstruct the correct turn of the shaft
 *  connected to the motor. Generic. It need two encoders to work.
 *
 * \param pos1		First encoder position
 * \param pos2		Second encoder position
 * 
 * \return Returns the number of turns of motor pulley at startup
**/
int calc_turns_fcn(const int32 pos1, const int32 pos2, const int N1, const int N2, const int I1);

/** This function counts a series of hand opening and closing used to execute a 
 * calibration of the device.
**/
void calibration();

/** This function checks for rest position and, in case, gives a position
 * reference to SoftHand.
**/
void check_rest_position();

/** This function switches between different LEDs condition depending
 * on battery level of charge or needed maintenance.
**/
void LED_control(uint8 mode);

void battery_management();

void ADC_Set_N_Channels();

void enable_motor(uint8 idx, uint8 val);

/** This function reset statistics counters **/
void reset_counters();

float invSqrt(float x);
void v3_normalize(float v3_in[3]);
void v4_normalize(float v4_in[4]);

/** This function updates the EMG history values with last availables.
**/
void update_EMG_history();

void set_motor_driver_type();

#endif

/* [] END OF FILE */
