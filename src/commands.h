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
 *  \file       commands.h
 *
 *  \brief      Definitions for SoftHand commands, parameters and packages.
 *
 *  \date        March 20th, 2020
 *  \author       _Centro "E.Piaggio"_
 *  \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
 *  \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
 *  \details
 *  This file is included in the firmware, in its libraries and
 *  applications. It contains all definitions that are necessary for the
 *  contruction of communication packages.
 *
 *  It includes definitions for all of the device commands, parameters and also
 *  the size of answer packages.
 *
**/

#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED


//==============================================================================
//                                                                      COMMANDS
//==============================================================================


/** \name SoftHand Commands
 * \{
**/

enum SH_command
{

//=========================================================     general commands

    CMD_PING                    = 0,    ///< Asks for a ping message.
    CMD_SET_ZEROS               = 1,    ///< Command for setting the encoders zero position.
    CMD_STORE_PARAMS            = 3,    ///< Stores all parameters in memory and
                                        ///  loads them.
    CMD_STORE_DEFAULT_PARAMS    = 4,    ///< Store current parameters as factory parameters.
    CMD_RESTORE_PARAMS          = 5,    ///< Restore default factory parameters.
    CMD_GET_INFO                = 6,    ///< Asks for a string of information about.

    CMD_BOOTLOADER              = 9,    ///< Sets the bootloader modality to update the
                                        ///  firmware.
    CMD_INIT_MEM                = 10,   ///< Initialize the memory with the defalut values.
    CMD_GET_PARAM_LIST          = 12,   ///< Command to get the parameters list or to set
                                        ///  a defined value chosen by the use.
    CMD_HAND_CALIBRATE          = 13,   ///< Starts a series of opening and closures of the SoftHand.

//=========================================================     specific commands

    CMD_ACTIVATE                = 128,  ///< Command for activating/deactivating
                                        ///  the device.
    CMD_GET_ACTIVATE            = 129,  ///< Command for getting device activation
                                        ///  state.
    CMD_SET_INPUTS              = 130,  ///< Command for setting reference inputs.
    CMD_GET_INPUTS              = 131,  ///< Command for getting reference inputs.
    CMD_GET_MEASUREMENTS        = 132,  ///< Command for asking device's
                                        ///  position measurements.
    CMD_GET_CURRENTS            = 133,  ///< Command for asking device's
                                        ///  current measurements.
    CMD_GET_CURR_AND_MEAS       = 134,  ///< Command for asking device's
                                        ///  measurements and currents.
    CMD_GET_EMG                 = 136,  ///< Command for asking device's emg sensors 
                                        ///  measurements.
    CMD_GET_VELOCITIES          = 137,  ///< Command for asking device's
                                        ///  velocity measurements.
    CMD_GET_ACCEL               = 139,  ///< Command for asking device's
                                        ///  acceleration measurements.
    CMD_GET_CURR_DIFF           = 140,  ///< Command for asking device's 
                                        ///  current difference between a measured
                                        ///  one and an estimated one.
    CMD_SET_CUFF_INPUTS         = 142,  ///< Command used to set Cuff device inputs .
    CMD_SET_BAUDRATE            = 144,  ///< Command for setting baudrate
                                        ///  of communication.
    CMD_GET_JOYSTICK            = 146,   ///< Command to get the joystick measurements (Only 
                                        ///  for devices driven by a joystick)
    
    // SoftHand PRO custom commands
    CMD_GET_IMU_READINGS        = 161,  // Retrieve accelerometers, gyroscopes and magnetometers readings    
    CMD_GET_IMU_PARAM           = 162,  // Retrieve and set IMU parameters
    CMD_GET_ENCODER_CONF        = 163,  // Get encoder configuration
    CMD_GET_ENCODER_RAW         = 164,  // Get all encoder raw values
    CMD_GET_ADC_CONF            = 165,  // Get ADC configuration
    CMD_GET_ADC_RAW             = 166,  // Get ADC raw values 
    CMD_GET_SD_SINGLE_FILE      = 167,  // Get a single file of the SD card given the path
    CMD_REMOVE_SD_SINGLE_FILE   = 168   // Remove a single file of the SD card given the path
};

//===================================================     resolution definitions

enum SH_resolution
{
    RESOLUTION_360      = 0,
    RESOLUTION_720      = 1,
    RESOLUTION_1440     = 2,
    RESOLUTION_2880     = 3,
    RESOLUTION_5760     = 4,
    RESOLUTION_11520    = 5,
    RESOLUTION_23040    = 6,
    RESOLUTION_46080    = 7,
    RESOLUTION_92160    = 8
};

//==============================================================     input modes

enum SH_input_mode
{
    INPUT_MODE_EXTERNAL         = 0,    ///< References through external
                                        ///  commands (default).
    INPUT_MODE_ENCODER3         = 1,    ///< Encoder 3 drives all inputs.
    INPUT_MODE_EMG_PROPORTIONAL = 2,    ///< Use EMG measure to proportionally.
                                        ///  drive the position of the motor.
    INPUT_MODE_EMG_INTEGRAL     = 3,    ///< Use 2 EMG signals to drive motor
                                        ///  position.
    INPUT_MODE_EMG_FCFS         = 4,    ///< Use 2 EMG. First reaching threshold.
                                        ///  wins and its value defines hand closure.
    INPUT_MODE_EMG_FCFS_ADV     = 5,     ///< Use 2 EMG. First reaching threshold.
                                        ///  wins and its value defines hand closure.
                                        ///  Wait for both EMG to lower under threshold.
    INPUT_MODE_JOYSTICK         = 6,    ///< Joystick input mode
    INPUT_MODE_EMG_PROPORTIONAL_NC     = 7    ///< EMG Normally closed.
};

//============================================================     control modes

enum SH_control_mode {

    CONTROL_ANGLE           = 0,        ///< Classic position control.
    CONTROL_PWM             = 1,        ///< Direct PWM value.
    CONTROL_CURRENT         = 2,        ///< Current control.
    CURR_AND_POS_CONTROL    = 3        ///< Current and position control.
};

//======================================================== motor supply voltage values

enum motor_supply_type {
    MAXON_24V               = 0,
    MAXON_12V               = 1
};

enum acknowledgment_values
{
    ACK_ERROR           = 0,
    ACK_OK              = 1
};

//==============================================    data types enumeration

enum data_types {
    TYPE_FLAG    = 0,
    TYPE_INT8    = 1,
    TYPE_UINT8   = 2,
    TYPE_INT16   = 3,
    TYPE_UINT16  = 4,
    TYPE_INT32   = 5,
    TYPE_UINT32  = 6,
    TYPE_FLOAT   = 7,
    TYPE_DOUBLE  = 8,
    TYPE_STRING  = 9            // Custom data type (uint8 but with string meaning)
};

#define PARAM_BYTE_SLOT     50      ///< Number of bytes reserved to a param information.

#define PARAM_MENU_SLOT     150     ///< Number of bytes reserved to a param menu.

//==============================================================================
//                                                                   INFORMATION
//==============================================================================
/** \name SoftHand Information Strings */
/** \{ */
#define INFO_ALL        0           ///< Generic device information.
#define CYCLES_INFO     1           ///< Cycles counter information.
#define GET_SD_PARAM    2           ///< Read Firmware Parameters from SD file
#define GET_SD_DATA     3           ///< Read Usage Data from SD file
#define GET_SD_FS_TREE  4           ///< Get info on SD card filesystem tree
#define GET_SD_EMG_HIST 5           ///< Read recent EMG history from SD file
#define GET_SD_R01_SUMM 6           ///< Get R01 project summary statistics

/** \} */

// ----------------------------------------------------------------------------
#endif
/* [] END OF FILE */