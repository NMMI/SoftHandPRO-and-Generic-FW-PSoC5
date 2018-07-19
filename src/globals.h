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
* \file         globals.h
*
* \brief        Global definitions and macros are set in this file.
* \date         February 01, 2018
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2018 Centro "E.Piaggio". All rights reserved.
*
*/

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED
// -----------------------------------------------------------------------------

//=================================================================     includes
#include "device.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "commands.h"
#include "FS.h"

//==============================================================================
//                                                                        DEVICE
//==============================================================================

#define VERSION                 "SoftHand PRO firmware v. 1.2 (PSoC5)"

#define NUM_OF_SENSORS          3       /*!< Number of encoders.*/
#define NUM_OF_EMGS             2       /*!< Number of emg channels.*/
#define NUM_OF_ANALOG_INPUTS    4       /*!< Total number of analogic inputs.*/
#define NUM_OF_PARAMS           29      /*!< Number of parameters saved in the EEPROM.*/
#define N_IMU_MAX               3    
#define NUM_OF_IMU_DATA         5       // accelerometers, gyroscopes, magnetometers, quaternion and temperature data
    
#define N_ENCODERS              NUM_OF_SENSORS
#define N_BITS_ENCODER          18
//==============================================================================
//                                                               SYNCHRONIZATION
//==============================================================================

//Main frequency 1000 Hz
#define CALIBRATION_DIV         10      /*!< Frequency divisor for hand calibration (100Hz).*/
#define DIV_INIT_VALUE          1       /*!< Initial value for hand counter calibration.*/

//==============================================================================
//                                                                           DMA
//==============================================================================
    
#define DMA_BYTES_PER_BURST 2
#define DMA_REQUEST_PER_BURST 1
#define DMA_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_DST_BASE (CYDEV_SRAM_BASE)  
//==============================================================================
//                                                                     INTERRUPT
//==============================================================================

#define    WAIT_START   0               /*!< Package start waiting status.*/
#define    WAIT_ID      1               /*!< Package ID waiting status.*/
#define    WAIT_LENGTH  2               /*!< Package lenght waiting status.*/
#define    RECEIVE      3               /*!< Package data receiving status.*/
#define    UNLOAD       4               /*!< Package data flush status.*/

//==============================================================================
//                                                                 CYCLE COUNTER
//==============================================================================
        
#define STATE_INACTIVE  0               /*!< Open SoftHand position / EMG Inactive.*/
#define STATE_ACTIVE    1               /*!< Closed SoftHand position / EMG Active.*/
#define COUNTER_INC     2               /*!< Counter cycle increment.*/

//==============================================================================
//                                                      SPI DELAY (MICROSECONDS)
//==============================================================================
#define SPI_DELAY_LOW       10
#define SPI_DELAY_HIGH      100
    
//==============================================================================
//                                                                         OTHER
//==============================================================================

#define FALSE                   0
#define TRUE                    1

#define DEFAULT_EEPROM_DISPLACEMENT 50  /*!< Number of pages occupied by the EEPROM.*/
#define EEPROM_BYTES_ROW        16      /*!< EEPROM number of bytes per row.*/
#define EEPROM_COUNTERS_ROWS    6       /*!< EEPROM number of rows dedicated to store counters.*/

#define PWM_MAX_VALUE           100     /*!< Maximum value of the PWM signal.*/

#define ANTI_WINDUP             1000    /*!< Anti windup saturation.*/ 
#define DEFAULT_CURRENT_LIMIT   1500    /*!< Default Current limit, 0 stands for unlimited.*/

#define CURRENT_HYSTERESIS      10      /*!< milliAmperes of hysteresis for current control.*/

#define EMG_SAMPLE_TO_DISCARD   500     /*!< Number of sample to discard before calibration.*/
#define SAMPLES_FOR_MEAN        100     /*!< Number of samples used to mean current values.*/
#define SAMPLES_FOR_EMG_MEAN    1000    /*!< Number of samples used to mean emg values.*/

#define POS_INTEGRAL_SAT_LIMIT  50000000    /*!< Anti windup on position control.*/
#define CURR_INTEGRAL_SAT_LIMIT 100000      /*!< Anti windup on current control.*/

#define LOOKUP_DIM              6           /*!< Dimension of the current lookup table.*/
    
#define PREREVISION_CYCLES      400000      /*!< Number of SoftHand Pro cycles before maintenance.*/    

//==============================================================================
//                                                        structures definitions
//==============================================================================

//=========================================================     motor references
/** \brief Motor Reference structure
 * 
**/
struct st_ref {
    int32 pos;                      /*!< Motor position reference.*/
    int32 curr;                     /*!< Motor current reference.*/
    int32 pwm;                      /*!< Motor direct pwm control.*/
    uint8 onoff;                    /*!< Motor drivers enable.*/
};

//=============================================================     measurements
/** \brief Measurements structure
 * 
**/
struct st_meas {
    int32 pos[NUM_OF_SENSORS];      /*!< Encoder sensor position.*/
    int32 curr;                     /*!< Motor current.*/
    int32 estim_curr;               /*!< Current estimation.*/
    int8 rot[NUM_OF_SENSORS];       /*!< Encoder sensor rotations.*/

    int32 emg[NUM_OF_EMGS];         /*!< EMG sensors values.*/
    int32 vel[NUM_OF_SENSORS];      /*!< Encoder rotational velocity.*/
    int32 acc[NUM_OF_SENSORS];      /*!< Encoder rotational acceleration.*/
};

//==============================================================     data packet
/** \brief Data sent/received structure
 *
**/
struct st_data {
    uint8   buffer[128];            /*!< Data buffer [CMD | DATA | CHECKSUM].*/
    int16   length;                 /*!< Data buffer length.*/
    int16   ind;                    /*!< Data buffer index.*/
    uint8   ready;                  /*!< Data buffer flag to see if the data is ready.*/
};

//============================================     settings stored on the memory
/** \brief EEPROM stored structure
 * 
**/
// Since PSOC5 ARM memory is 4-bytes aligned, st_mem structure variables are not contiguous
struct st_mem {
    uint8   flag;                       /*!< If checked the device has been configured.*/                   //1
    uint8   id;                         /*!< Device id.*/                                                   //1
                                                                                                            //2 bytes free (word-aligned)
    int32   k_p;                        /*!< Position controller proportional constant.*/                   //4
    int32   k_i;                        /*!< Position controller integrative constant.*/                    //4
    int32   k_d;                        /*!< Position controller derivative constant.*/                     //4
    // End of row one.
    int32   k_p_c;                      /*!< Current controller proportional constant.*/                    //4
    int32   k_i_c;                      /*!< Current controller integrative constant.*/                     //4
    int32   k_d_c;                      /*!< Current controller derivative constant.*/                      //4 
    int32   k_p_dl;                     /*!< Double loop position controller prop. constant.*/              //4
    // End of row two.
    int32   k_i_dl;                     /*!< Double loop position controller integr. constant.*/            //4     
    int32   k_d_dl;                     /*!< Double loop position controller deriv. constant.*/             //4
    int32   k_p_c_dl;                   /*!< Double loop current controller prop. constant.*/               //4
    int32   k_i_c_dl;                   /*!< Double loop current controller integr. constant.*/             //4
    // End of row three.
    int32   k_d_c_dl;                   /*!< Double loop current controller deriv. constant.*/              //4     
    uint8   activ;                      /*!< Startup activation.*/                                          //1
    uint8   input_mode;                 /*!< Motor Input mode.*/                                            //1 
    uint8   control_mode;               /*!< Motor Control mode.*/                                          //1
    uint8   res[NUM_OF_SENSORS];        /*!< Angle resolution.*/                                            //3
                                                                                                            //2 bytes free (word-aligned)
    int32   m_off[NUM_OF_SENSORS];      /*!< Measurement offset.*/                                          //12
    //End of row four. Second and third offset is on row five.  
    float32   m_mult[NUM_OF_SENSORS];     /*!< Measurement multiplier.*/                                    //12
    //End of row five. Third multiplier is on row six.
    uint8   pos_lim_flag;               /*!< Position limit active/inactive.*/                              //1
                                                                                                            //3 bytes free (word-aligned)
    int32   pos_lim_inf;                /*!< Inferior position limit for motor.*/                           //4     
    int32   pos_lim_sup;                /*!< Superior position limit for motor.*/                           //4
    // End of row six.
    int32   max_step_neg;               /*!< Maximum number of steps per cycle for negative positions.*/    //4
    int32   max_step_pos;               /*!< Maximum number of steps per cycle for positive positions.*/    //4          
    int16   current_limit;              /*!< Limit for absorbed current.*/                                  //2
    uint16  emg_threshold[NUM_OF_EMGS]; /*!< Minimum value for activation of EMG control.*/                 //4
    uint8   emg_calibration_flag;       /*!< Enable emg calibration on startup.*/                           //1  
                                                                                                            //1 bytes free (word-aligned)    
    // End of row seven.
    uint32  emg_max_value[NUM_OF_EMGS]; /*!< Maximum value for EMG.*/                                       //8     
    uint8   emg_speed;                  /*!< Maximum closure speed when using EMG.*/                        //1
    uint8   double_encoder_on_off;      /*!< Double encoder ON/OFF.*/                                       //1
    int8    motor_handle_ratio;         /*!< Discrete multiplier for handle device.*/                       //1 
    uint8   activate_pwm_rescaling;     /*!< Activation of PWM rescaling for 12V motor.*/                   //1
    float   curr_lookup[LOOKUP_DIM];    /*!< Table of values to get estimated curr.*/                       //24
    // End of row eight. curr_lookup[0] is on row eight but curr_lookup[1:4] is on row nine. 
    // End of row nine. curr_lookup[5] is on row ten. 
    uint8   baud_rate;                  /*!< Baud Rate setted.*/                                            //1
    uint8   maint_day;                  /*!< Date of last maintenance.*/                                    //1
    uint8   maint_month;                /*!< Date of last maintenance.*/                                    //1
    uint8   maint_year;                 /*!< Date of last maintenance.*/                                    //1
    int32   rest_pos;                   /*!< Hand rest position while in EMG mode.*/                        //4
    float   rest_delay;                 /*!< Hand rest position delay while in EMG mode.*/                  //4
    // End of row ten.
    float   rest_vel;                   /*!< Hand velocity closure for rest position reaching.*/             //4
    float   rest_ratio;                 /*!< Hand rest ratio between vel closure and rest position error.*/  //4
    uint8   rest_position_flag;         /*!< Enable rest position feature.*/                                //1
    uint8   switch_emg;                 /*!< EMG opening/closure switch.*/                                  //1    
    uint8   unused_bytes[22];
    // End of row twelve. unused_bytes[0:5] are on row eleven, while the others in row twelve.
    uint32  emg_counter[2];             /*!< Counter for EMG activation - both channels.*/                  //8
    //End of row thirteen. position_hist[0] and position_hist[1] are on row thirteen.
    uint32  position_hist[10];          /*!< Positions histogram - 10 zones.*/                              //40
    //End of row fifteen.
    uint32  current_hist[4];            /*!< Current histogram - 4 zones.*/                                 //16
    //End of row sixteen.
    uint32  rest_counter;               /*!< Counter for rest position occurrences.*/                       //4
    uint32  wire_disp;                  /*!< Counter for total wire displacement measurement.*/             //4
    uint32  total_time_on;              /*!< Total time of system power (in seconds).*/                     //4
    uint32  total_time_rest;            /*!< Total time of system while rest position is maintained.*/      //4
    //End of row seventeen.
    uint8   curr_time[6];               /*!< Current time from RTC (DD/MM/YY HH:MM:SS).*/                   //6 
    uint8   unused_bytes1[10];
    // End of row eighteen.
    // IMU
    uint8   SPI_read_delay;             // delay on SPI reading
    uint8   IMU_conf[N_IMU_MAX][NUM_OF_IMU_DATA];     // IMU default configuration               85
};

//=================================================     IMU variables
struct st_imu {
    uint8 flags;        // Flags to know what we are reading (0/1) from each imu [ accel | gyro | magn ]
    int16 accel_value[3];
    int16 gyro_value[3];
    int16 mag_value[3];
    float quat_value[4];
    int16 temp_value;
};

//=================================================     filter variables
/** \brief Filter structure
 *
**/ 
struct st_filter{
    int32 old_value;                /*!< Old variable value.*/
    int32 gain;                     /*!< New value filter weight.*/
};

//==============================================     hand calibration parameters
/** \brief Hand calibration structure
 *
**/ 
struct st_calib {
    uint8   enabled;                /*!< Calibration enabling flag.*/
    uint8   direction;              /*!< Direction of motor winding.*/
    int16   speed;                  /*!< Speed of hand opening/closing.*/
    int16   repetitions;            /*!< Number of cycles of hand closing/opening.*/
};

//=================================================     emg status
typedef enum {

    NORMAL        = 0,              /*!< Normal execution.*/
    RESET         = 1,              /*!< Reset analog measurements.*/
    DISCARD       = 2,              /*!< Discard first samples to obtain a correct value.*/
    SUM_AND_MEAN  = 3,              /*!< Sum and mean a definite value of samples.*/
    WAIT          = 4,              /*!< The second emg waits until the first emg has a valid value.*/
    WAIT_EoC      = 5               /*!< The second emg waits for end of calibration.*/
} emg_status;                       /*!< EMG status enumeration.*/

typedef enum {
    
    PREPARE_DATA    = 0,            /*!< Prepare data to be written on EEPROM.*/
    WRITE_CYCLES    = 1,            /*!< Cycles writing on EEPROM is enabled and control is passed to query.*/
    WAIT_QUERY      = 2,            /*!< Wait until EEPROM_Query() has finished writing on EEPROM and then disable cycles writing.*/
    WRITE_END       = 3,            /*!< End of EEPROM writing.*/
    NONE            = 4             /*!< Cycles writing on EEPROM is disabled.*/
} counter_status;                   /*!< Cycles counter state machine status.*/ 

//====================================      external global variables definition

extern struct st_ref    g_ref, g_refNew, g_refOld;  /*!< Reference variables.*/
extern struct st_meas   g_meas, g_measOld;          /*!< Measurements.*/
extern struct st_data   g_rx;                       /*!< Incoming/Outcoming data.*/
extern struct st_mem    g_mem, c_mem;               /*!< Memory parameters.*/
extern struct st_calib  calib;                      /*!< Calibration variables.*/
extern struct st_filter filt_v, filt_curr_diff, filt_i;     /*!< Voltage and current filter variables.*/
extern struct st_filter filt_vel[3];                /*!< Velocity filter variables.*/
extern struct st_filter filt_emg1, filt_emg2;       /*!< EMG filter variables.*/

extern uint32 timer_value;                          /*!< End time of the firmware main loop.*/
extern uint32 timer_value0;                         /*!< Start time of the firmware main loop.*/
extern float cycle_time;							/*!< Variable used to calculate how much time a cycle takes.*/

extern int32    dev_tension;                        /*!< Power supply tension.*/
extern uint8    dev_pwm_limit;                      /*!< Device pwm limit. It may change during execution.*/
extern uint8    dev_pwm_sat;                        /*!< Device pwm saturation.*/
extern int32    dev_tension_f;                      /*!< Filtered power supply tension.*/
extern int32    pow_tension;                        /*!< Computed power supply tension.*/

extern counter_status CYDATA cycles_status;         /*!< Cycles counter state machine status.*/
extern emg_status CYDATA emg_1_status;              /*!< First EMG sensor status.*/
extern emg_status CYDATA emg_2_status;              /*!< Second EMG sensor status.*/                               
    
// Bit Flag

extern CYBIT reset_last_value_flag;                 /*!< This flag is set when the encoders last values must be resetted.*/
extern CYBIT tension_valid;                         /*!< Tension validation bit.*/
extern CYBIT interrupt_flag;                        /*!< Interrupt flag enabler.*/
extern CYBIT cycles_interrupt_flag;                 /*!< Cycles timer interrupt flag enabler.*/
extern uint8 maintenance_flag;                      /*!< Maintenance flag.*/
extern CYBIT can_write;                             /*!< Write to EEPROM flag.*/
extern uint8 rest_enabled;                          /*!< Rest position flag.*/
extern uint8 forced_open;                           /*!< Forced open flag (used in position with rest position control).*/
extern uint8 battery_low_SoC;                       /*!< Battery low State of Charge flag (re-open terminal device when active).*/

// DMA Buffer

extern int16 ADC_buf[4];                            /*! ADC measurements buffer.*/

// PWM value

extern int8 pwm_sign;                               /*!< Sign of pwm driven. Used to obtain current sign.*/

// Rest Position variables

extern int32 rest_pos_curr_ref;                     /*!< Rest position current reference.*/

// SD variables
extern FS_FILE * pFile;
extern unsigned long write_bytes;

// IMU variables
extern uint8 N_IMU_Connected;
extern uint8 IMU_connected[N_IMU_MAX];
extern int imus_data_size;
extern int single_imu_size[N_IMU_MAX];
extern struct st_imu g_imu[N_IMU_MAX], g_imuNew[N_IMU_MAX];

extern uint8 Accel[N_IMU_MAX][6];
extern uint8 Gyro[N_IMU_MAX][6];
extern uint8 Mag[N_IMU_MAX][6];
extern uint8 MagCal[N_IMU_MAX][3];
extern uint8 Temp[N_IMU_MAX][2];
extern float Quat[4];

// -----------------------------------------------------------------------------


#endif

//[] END OF FILE