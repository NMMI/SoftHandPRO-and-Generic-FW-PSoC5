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
* \file         globals.c
*
* \brief        Global variables.
* \date         October 01, 2017
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2019 Centro "E.Piaggio". All rights reserved.
*/

//=================================================================     includes
#include "globals.h"

//=============================================      global variables definition


struct st_ref       g_ref, g_refNew, g_refOld;  // Motor reference variables.
struct st_meas      g_meas, g_measOld;          // Measurements.
struct st_data      g_rx;                       // Income data.
struct st_mem       g_mem, c_mem;               // Memory variables.
struct st_calib     calib;                      // Calibration variables.
struct st_filter    filt_v, filt_curr_diff, filt_i;     // Voltage and current filter variables.
struct st_filter    filt_vel[3];               // Velocity filter variables.
struct st_filter    filt_emg1, filt_emg2;      // EMG filter variables.

// Timer value for debug field
uint32  timer_value;
uint32  timer_value0;
float   cycle_time;

// Device Data
int32   dev_tension;                        /*!< Power supply tension.*/
uint8   dev_pwm_limit;                      /*!< Device pwm limit. It may change during execution.*/
uint8   dev_pwm_sat = 100;                  /*!< Device pwm saturation. By default the saturation value must not exceed 100.*/
int32   dev_tension_f;                      /*!< Filtered power supply tension.*/
int32   pow_tension;                        /*!< Computed power supply tension.*/


counter_status CYDATA cycles_status = NONE;     /*!< Cycles counter state machine status.*/
emg_status CYDATA emg_1_status = RESET;         /*!< First EMG sensor status.*/
emg_status CYDATA emg_2_status = RESET;         /*!< Second EMG sensor status.*/                               
    
// Bit Flag
CYBIT reset_last_value_flag;                /*!< This flag is set when the encoders last values must be resetted.*/
CYBIT tension_valid;                        /*!< Tension validation bit.*/
CYBIT interrupt_flag = FALSE;               /*!< Interrupt flag enabler.*/
CYBIT cycles_interrupt_flag = FALSE;        /*!< Cycles timer interrupt flag enabler.*/
uint8 maintenance_flag = FALSE;             /*!< Maintenance flag.*/
CYBIT can_write = TRUE;                     /*!< Write to EEPROM flag.*/
uint8 rest_enabled;                         /*!< Rest position flag.*/
uint8 forced_open;                          /*!< Forced open flag (used in position with rest position control).*/                               
uint8 battery_low_SoC = FALSE;              /*!< Battery low State of Charge flag (re-open terminal device when active).*/
uint8 change_ext_ref_flag = FALSE;          /*!< This flag is set when an external reference command is received.*/

// DMA Buffer
int16 ADC_buf[4];                           /*! ADC measurements buffer.*/
uint32 Enc_buf[N_ENCODERS];

// PWM value
int8 pwm_sign;                               /*!< Sign of pwm driven. Used to obtain current sign.*/

// Encoder variables
uint32 data_encoder_raw[NUM_OF_SENSORS];

// Rest Position variables
int32 rest_pos_curr_ref;                     /*!< Rest position current reference.*/

// SD variables
FS_FILE * pFile;
unsigned long write_bytes = 1000;

// IMU variables
uint8 N_IMU_Connected;
uint8 IMU_connected[N_IMU_MAX];
int imus_data_size;
int single_imu_size[N_IMU_MAX];

struct st_imu g_imu[N_IMU_MAX];
struct st_imu g_imuNew[N_IMU_MAX];

uint8 Accel[N_IMU_MAX][6];
uint8 Gyro[N_IMU_MAX][6];
uint8 Mag[N_IMU_MAX][6];
uint8 MagCal[N_IMU_MAX][3];
uint8 Temp[N_IMU_MAX][2];
float Quat[4] = {1,0,0,0};

/* END OF FILE */