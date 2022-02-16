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
* \file         globals.c
*
* \brief        Global variables.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

//=================================================================     includes
#include "globals.h"

//=============================================      global variables definition

struct st_ref       g_ref[NUM_OF_MOTORS], g_refNew[NUM_OF_MOTORS], g_refOld[NUM_OF_MOTORS];  // Motor reference variables.
struct st_meas      g_meas[N_ENCODER_LINE_MAX], g_measOld[N_ENCODER_LINE_MAX];          // Measurements.
struct st_adc_meas  g_adc_meas, g_adc_measOld;  // EMG Measurements.
struct st_fb_meas   g_fb_meas;                  // Haptic Feedback Measurements.
struct st_data      g_rx;                       // Income data.
struct st_eeprom    g_mem, c_mem;               // Memory variables.
struct st_calib     calib;                      // Calibration variables.
struct st_filter    filt_v[NUM_OF_MOTORS], filt_curr_diff[NUM_OF_MOTORS], filt_i[NUM_OF_MOTORS];     // Voltage and current filter variables.
struct st_filter    filt_vel[NUM_OF_SENSORS];                // Velocity filter variables.
struct st_filter    filt_emg[NUM_OF_INPUT_EMGS+NUM_OF_ADDITIONAL_EMGS];                // EMG filter variables.
struct st_filter    filt_detect_pc;             // Battery tension filter to detect a new power cycle.


// Timer value for debug field
uint16  timer_value;
uint16  timer_value0;
float   cycle_time;

// Device Data
int32   dev_tension[NUM_OF_MOTORS];         /*!< Power supply tension.*/
uint16  dev_pwm_limit[NUM_OF_MOTORS];       /*!< Device pwm limit. It may change during execution.*/
uint16  dev_pwm_sat[NUM_OF_MOTORS];         /*!< Device pwm saturation. By default the saturation value must not exceed 100.*/
int32   dev_tension_f[NUM_OF_MOTORS];       /*!< Filtered power supply tension.*/
int32   pow_tension[NUM_OF_MOTORS];         /*!< Computed power supply tension.*/
int32   detect_power_cycle = 0;             /*!< Variable used to detect a new power cycle.*/

counter_status CYDATA cycles_status = NONE;     /*!< Cycles counter state machine status.*/
adc_status CYDATA emg_1_status = RESET;         /*!< First EMG sensor status.*/
adc_status CYDATA emg_2_status = RESET;         /*!< Second EMG sensor status.*/   
adc_status CYDATA joy_UD_status = RESET;        /*!< Joystick UP/DOWN status.*/
adc_status CYDATA joy_LR_status = RESET;        /*!< Joystick LEFT/RIGHT status.*/

    
// Bit Flag
CYBIT reset_last_value_flag[N_ENCODER_LINE_MAX]; /*!< This flag is set when the encoders last values must be resetted.*/
CYBIT tension_valid;                        /*!< Tension validation bit.*/
CYBIT interrupt_flag = FALSE;               /*!< Interrupt flag enabler.*/
CYBIT cycles_interrupt_flag = FALSE;        /*!< Cycles timer interrupt flag enabler.*/
uint8 maintenance_flag = FALSE;             /*!< Maintenance flag.*/
CYBIT can_write = TRUE;                     /*!< Write to EEPROM flag.*/
uint8 rest_enabled;                         /*!< Rest position flag.*/
uint8 forced_open;                          /*!< Forced open flag (used in position with rest position control).*/                               
uint8 battery_low_SoC = FALSE;              /*!< Battery low State of Charge flag (re-open terminal device when active).*/
uint8 change_ext_ref_flag = FALSE;          /*!< This flag is set when an external reference command is received.*/
CYBIT reset_PSoC_flag = FALSE;              /*!< This flag is set when a board fw reset is necessary.*/

// ADC Buffer
int16 ADC_buf[NUM_OF_ADC_CHANNELS_MAX];     /*! ADC measurements buffer.*/
uint8 NUM_OF_ANALOG_INPUTS = 4;             /*! ADC currently configured channels.*/

// PWM value
int8 pwm_sign[NUM_OF_MOTORS];               /*!< Sign of pwm driven. Used to obtain current sign.*/

// Encoder variables
uint32 data_encoder_raw[N_ENCODER_LINE_MAX][N_ENCODERS_PER_LINE_MAX];
uint8 N_Encoder_Line_Connected[N_ENCODER_LINE_MAX]; // Used to map how many encoders are connected to each CS pin, there are N_ENCODER_LINE_MAX CS on the board and each of them can contain N_ENCODERS_PER_LINE_MAX encoders
uint16 Encoder_Value[N_ENCODER_LINE_MAX][N_ENCODERS_PER_LINE_MAX];
uint8 Encoder_Check[N_ENCODER_LINE_MAX][N_ENCODERS_PER_LINE_MAX];
CYBIT pos_reconstruct[N_ENCODER_LINE_MAX] = {FALSE, FALSE};

// Rest Position variables
int32 rest_pos_curr_ref;                     /*!< Rest position current reference.*/

// SD variables
FS_FILE * pFile;
char sdFile[100] = "";
char sdParam[100] = "";
FS_FILE * pEMGHFile;
char sdEMGHFile[100] = "\\EMG_History.csv";
char sdR01File[100] = "\\R01_Summary.csv";

// IMU variables
uint8 N_IMU_Connected;
uint8 IMU_connected[N_IMU_MAX];
int imus_data_size;
int single_imu_size[N_IMU_MAX];
struct st_imu_data g_imu[N_IMU_MAX];
struct st_imu_data g_imuNew[N_IMU_MAX];
uint8 Accel[N_IMU_MAX][6];
uint8 Gyro[N_IMU_MAX][6];
uint8 Mag[N_IMU_MAX][6];
uint8 MagCal[N_IMU_MAX][3];
uint8 Temp[N_IMU_MAX][2];
float Quat[N_IMU_MAX][4];

// MASTER variables
uint8 master_mode;               /*!< Flag used to set/unset master mode to send messages to other boards.*/

// EMG HISTORY
uint16 emg_history[SAMPLES_FOR_EMG_HISTORY][NUM_OF_INPUT_EMGS];   /*!< EMG data with the history of last activity.*/
uint32 emg_history_next_idx;    /*!< Vector index of last (newest) element.*/

/* END OF FILE */