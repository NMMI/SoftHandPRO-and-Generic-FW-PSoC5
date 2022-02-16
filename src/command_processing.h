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
 * \file        command_processing.h
 *
 * \brief       Received commands processing functions             
 * \date        March 20th, 2020
 * \author      _Centro "E.Piaggio"_
 * \copyright   (C) 2012-2016 qbrobotics. All rights reserved.
 * \copyright   (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
 * \details
 *
 *  This file contains all the definitions of the functions used to 
 * 	process the commands sent from the user interfaces (simulink, command line, GUI)
**/

// ----------------------------------------------------------------------------
#ifndef COMMAND_PROCESSING_H_INCLUDED
#define COMMAND_PROCESSING_H_INCLUDED
// ----------------------------------------------------------------------------

//=================================================================     includes
#include "globals.h" // ALL GLOBAL DEFINITIONS, STRUCTURES AND MACROS HERE
#include "IMU_functions.h"
#include "Encoder_functions.h"
#include "SD_RTC_functions.h"
#include "interruptions.h"
#include "utils.h"
#include "commands.h"
#include <stdio.h>

//==============================================================================
//                                                          function definitions
//==============================================================================

/** \name Firmware information functions */
/** \{ */

//============================================================  prepare_generic_info
/** This function is used to prepare a generic information string on the device parameters
 *  and measurements.
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_generic_info(char *info_string);

//============================================================  prepare_counter_info
/** This function is used to prepare an information string about the cycles counter
 *  of the hand.
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_counter_info(char *info_string);

//============================================================  prepare_R01_info
/** This function is used to prepare an information string about the cycles counter
 *  of the hand used in R01 project.
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_R01_info(char *info_string);

//============================================================  prepare_SD_info
/** This function is used to prepare an information string to be on a SD card
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_SD_info(char *info_string);

//============================================================  prepare_SD_param_info
/** This function is used to prepare an information string to be on a SD card
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_SD_param_info(char *info_string);

//============================================================  prepare_SD_EMG_history
/** This function is used to prepare an information string to be on a SD card
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_SD_EMG_history(char *info_string);

//============================================================  prepare_SD_legend
/** This function is used to prepare an information string to be on a SD card
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_SD_legend(char *info_string);

//============================================================  prepare_SD_EMG_History_legend
/** This function is used to prepare an information string to be on a SD card
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    prepare_SD_EMG_History_legend(char *info_string);

//============================================================  IMU_reading_info
/** This function is used to prepare an information string about the IMU sensors
 *  last reading.
 *
 * \param 	info_string 	An array of chars containing the requested information.
**/
void    IMU_reading_info(char *info_string);

//============================================================  infoSend
/** This function sends the firmware information prepared with \ref infoPrepare 
 *  "infoPrepare" through the serial port to the user interface. Is used when no 
 *  ID is specified.
**/
void    infoSend           ();

//============================================================  infoGet
/** This function sends the firmware information prepared with \ref prepare_general_info 
 *  "prepare_general_info" or \ref prepare_counter_info "prepare_counter_info" through the 
 *  serial port to the user interface. Is used when the ID is specified.
 *
 * 	\param info_type	The type of the information needed.
**/
void    infoGet            (uint16 info_type);

/** \} */


/** \name Command receiving and sending functions */
/** \{ */

//============================================================  commProcess
/** This function unpacks the received package, depending on the command received. 	
**/
void    commProcess        ();

//============================================================  commWrite_old_id
/** This function writes on the serial port the package that needs to be sent
 * to the user. Is used only when a new is set, to communicate back to the APIs that
 * the new ID setting went fine or there was an error.
 *
 *	\param packet_data 		The array of data that must be written.
 *	\param packet_lenght	The lenght of the data array.
 *  \param old_id           The previous id of the board, before setting a new one.
 *
**/
void    commWrite_old_id   (uint8 *packet_data, uint16 packet_lenght, uint8 old_id);

//============================================================  commWrite
/** This function writes on the serial port the package that needs to be sent
 * to the user.
 *
 *	\param packet_data 		The array of data that must be written.
 *	\param packet_lenght	The lenght of the data array.
 *
**/
void    commWrite         (uint8 *packet_data, uint16 packet_lenght);

//============================================================  commWriteID
 /** This function writes on the serial port the package that needs to be sent
 * to another board.
 *
 *	\param packet_data 		The array of data that must be written.
 *	\param packet_lenght	The lenght of the data array.
 *
**/
void    commWriteID  (uint8 *packet_data, uint16 packet_lenght, uint8 id);
/** \} */


/** \name Memory management functions */
/** \{ */

//============================================================  manage_param_list
/** This function, depending on the \ref index received, gets the list of
 *  parameters with their values and sends them to user or sets a parameter
 *  from all the parameters of the device.
 *
 *	\param index 			The index of the parameters to be setted. If 0 gets
 *							full parameters list.
 *
**/
void manage_param_list 		(uint16 index);

//============================================================  get_param_list
/** This function, depending on the \ref index received, gets the list of
 *  parameters with their values.
 *
**/
void get_param_list 		(uint8* VAR_P[NUM_OF_PARAMS], uint8 TYPES[NUM_OF_PARAMS], 
                             uint8 NUM_ITEMS[NUM_OF_PARAMS], uint8 NUM_STRUCT[NUM_OF_PARAMS],
                             uint8* NUM_MENU, const char* PARAMS_STR[NUM_OF_PARAMS], 
                             uint8 CUSTOM_PARAM_SET[NUM_OF_PARAMS], const char* MENU_STR[NUM_OF_PARAMS_MENU]);

//============================================================  set_custom_param
/** This function, depending on the \ref index received, sets the specific
 *  parameters with their values and sends them to user or sets a parameter
 *  from all the parameters of the device.
 *
 *	\param index 			The index of the parameters to be setted.
 *
**/
void set_custom_param 		(uint16 index);

//============================================================  get_IMU_param_list
/** This function, depending on the \ref index received, gets the list of
 *  parameters with their values.
 *
**/
void get_IMU_param_list 		(uint16 index);
 

//============================================================  setZeros
/** This function sets the encoders zero position.
 *
**/
void setZeros				();

//============================================================  memStore
/** This function stores the setted parameters to the internal EEPROM memory.
 * 	It is usually called, by the user, after a parameter is set.
 *
 *	\param displacement 	The address where the parameters will be written.
 *
 *	\return A true value if the memory is correctly stored, false otherwise.
**/
uint8   memStore           (int displacement);

//============================================================  memRecall
/** This function loads user's settings from the EEPROM.
**/
void    memRecall          ();

//============================================================  memRestore
/** This function loads default settings from the EEPROM.
 * 
 *	\return A true value if the memory is correctly restored, false otherwise.
**/
uint8   memRestore         ();

//============================================================  memInit
/** This functions initializes the memory. It is used also to restore the
 * 	the parameters to their default values.
 *
 *	\return A true value if the memory is correctly initialized, false otherwise.
**/
uint8   memInit            ();

//============================================================  memInitSoftHandPro
/** This functions initializes the memory. It is used also to restore the
 * 	the parameters to their default values. Specific for SoftHand firmware
 *
**/
void   memInit_SoftHandPro ();

//============================================================  memInitMaster
/** This functions initializes the memory. It is used also to restore the
 * 	the parameters to their default values. Specific for Master firmware
 *
**/
void   memInit_Master ();

//============================================================  memInitAirChambersFb
/** This functions initializes the memory. It is used also to restore the
 * 	the parameters to their default values. Specific for Air Chambers firmware
 *
**/
void   memInit_AirChambersFb ();

//============================================================  memInitOtbkActWristMs
/** This functions initializes the memory. It is used also to restore the
 * 	the parameters to their default values. Specific for Ottobock Active Wrist Master firmware
 *
**/
void   memInit_OtbkActWristMs ();

/** \} */


/** \name Utility functions */
/** \{ */

//============================================================  LCRChecksum
/** This function calculates a checksum of the array to see if the received data 
 *  is consistent.
 *
 *	\param data_array		The array of data that must be checked.	
 *	\param data_lenght		Lenght of the data array that must be checked.
 *
 *	\return The calculated checksum for the relative data_array.
**/
uint8   LCRChecksum        (uint8 *data_array, uint8 data_length);

//============================================================  sendAcknoledgment
/** This functions sends an acknowledgment to see if a command has been executed 
 * 	properly or not.
 *
 *	\param value 		An ACK_OK(1) or ACK_ERROR(0) value.
**/
void    sendAcknowledgment (uint8 value);

//==============================================================================
//                                            Service Routine interrupt function
//==============================================================================

/** \name Command processing functions */
/** \{ */

//============================================================  cmd_activate
/** This function activates the board
**/
void cmd_activate();

//============================================================  cmd_set_inputs
/** This function gets the inputs from the received package and sets them as
	motor reference.
**/
void cmd_set_inputs();

//============================================================  cmd_get_measurements
/** This function gets the encoders measurements and puts them in the package
	to be sent.
**/
void cmd_get_measurements();

//============================================================  cmd_get_curr_and_meas
/** This function gets the currents and encoders measurements and puts them in the package
	to be sent.
**/
void cmd_get_curr_and_meas();

//============================================================  cmd_get_velocities
/** This function gets the encoders velocities and puts them in the package
	to be sent.
**/
void cmd_get_velocities();

//============================================================  cmd_get_accelerations
/** This function gets the encoders accelerations and puts them in the package
	to be sent.
**/
void cmd_get_accelerations();

//============================================================  cmd_get_joystick
/** This function gets the joystick analog values and puts them in the package
	to be sent.
**/
void cmd_get_joystick();

//============================================================  cmd_get_currents
/** This function gets the motor current and puts it in the package to 
	be sent to the user.
**/
void cmd_get_currents();

//============================================================  cmd_get_currents_for_cuff()
/** This function gets the motor current and puts it in the package to 
	be sent to the Cuff device, using the \ref commWrite_to_cuff "commWrite_to_cuff"
    function.
**/
void cmd_get_currents_for_cuff();

//============================================================  cmd_get_emg
/** This function gets the electromyographic sensors measurements and puts
	them in the package to be sent.
**/
void cmd_get_emg();

//============================================================  cmd_get_activate
/** This function gets the board activation status and puts it in the package
	to be sent.
**/
void cmd_get_activate();

//============================================================  cmd_set_baudrate
/** This function sets the desired communication baudrate. It is possible to
	select a value equal to 460800 or 2000000.
**/
void cmd_set_baudrate();

//============================================================  cmd_get_inputs
/** This function gets the current motor reference inputs and puts them in the
	package to be sent.
**/
void cmd_get_inputs();

//============================================================  cmd_store_params
/** This function stores the parameters to the EEPROM memory
**/
void cmd_store_params();

//============================================================  cmd_ping
/** This function is used to ping the device and see if is connected.
**/
void cmd_ping();

//============================================================  cmd_get_imu_readings
/** This function gets IMU readings
**/
void cmd_get_imu_readings();

//============================================================  cmd_get_encoder_map
/** This function gets Encoder map
**/
void cmd_get_encoder_map();

//============================================================  cmd_get_encoder_raw
/** This function gets Encoder raw values
**/
void cmd_get_encoder_raw();

//============================================================  cmd_get_ADC_map
/** This function gets ADC map
**/
void cmd_get_ADC_map();

//============================================================  cmd_get_ADC_raw
/** This function gets Additional emg raw values
**/
void cmd_get_ADC_raw();

//============================================================  cmd_get_SD_file
/** This function gets a file on the SD
**/
void cmd_get_SD_file(uint16 filename_length);

//============================================================  cmd_remove_SD_file
/** This function removes a file on the SD
**/
void cmd_remove_SD_file(uint16 filename_length);

//============================================================  air_chamber_control
/* This function is used to drive air chambers feedback device */
void air_chambers_control(int slave_motor_idx);

//============================================================  emg_activation_velocity_fsm
/* This function is used to decide which is the current emg activation velocity */
uint8 emg_activation_velocity_fsm();

//============================================================  active_wrist_control
/* This function is used to drive Ottobock active wrist device */
void otbk_act_wrist_control(int slave_motor_idx);

//============================================================  commReadResCurrFromSH
/** This function reads on the serial port the residual current from SH.
 *
**/
int16 commReadResCurrFromSH();

//=====================================================     drive_slave
/** This function is used to create a package and send it to another device, only
 *  if the actual board is in master mode.
**/
void drive_slave(uint8 motor_idx, uint8 slave_ID);

//=====================================================     stop_master_device
/** This function is used to deactivate master device motors when occurred.
**/
void stop_master_device();

//=====================================================     deactivate_slaves
/** This function is used to create a package to deactivate motors on all the other devices.
**/
void deactivate_slaves();

#endif

/* [] END OF FILE */