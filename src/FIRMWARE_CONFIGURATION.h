// ----------------------------------------------------------------------------
// BSD 3-Clause License

// Copyright (c) 2019-2020, Centro "E.Piaggio"
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
 *  \file       FIRMWARE_CONFIGURATION.h
 *
 *  \brief      Definitions for SoftHand and Other Devices commands, parameters and packages.
 *
 *  \date       March 20th, 2020
 *  \author     _Centro "E.Piaggio"_
 *  \copyright  (C) 2019-2020 Centro "E.Piaggio". All rights reserved.
 *  \details
 *  This file is included in the firmware, in its libraries and
 *  applications. It contains all definitions that are necessary to discriminate the right firmware.
 *
**/

#ifndef FIRMWARE_CONFIGURATION_H_INCLUDED
#define FIRMWARE_CONFIGURATION_H_INCLUDED

//====================================================================================
//====================================================================================
// SOFTHAND OR GENERIC CONFIGURATION IS HARDCODED IN BUILD OPTIONS USING 
// PREPROCESSOR DEFINES (SOURCE AND HEADERS FILES ARE SHARED BETWEEN THE TWO PROJECTS)
//    
// SOFTHAND FW: #define SOFTHAND_FW
// GENERIC FW:  #define GENERIC_FW 
// AIR_CHAMBERS_FB_FW:      #define AIR_CHAMBERS_FB_FW
// OTBK_ACT_WRIST_MS_FW:    #define OTBK_ACT_WRIST_MS_FW    
//====================================================================================
//====================================================================================
    
    
// Macro related to different firmware configurations
#ifdef SOFTHAND_FW
    #define VERSION                 "SoftHand PRO firmware v. 1.10.1 (PSoC5)"
#else
    #ifdef GENERIC_FW
        #define VERSION                 "Generic firmware v. 1.10.1 (PSoC5)"
    #else   
        #define MASTER_FW
        #ifdef AIR_CHAMBERS_FB_FW
            #define VERSION                 "Air Chambers Haptic Feedback firmware v. 1.10.1 (PSoC5) - Master configuration"
        #else //OTBK_ACT_WRIST_MS_FW
            #define VERSION                 "Ottobock Active Wrist firmware v. 1.10.1 (PSoC5) - Master configuration"
        #endif
    #endif
#endif      


// Default number of parameters configuration
// Handle multiple configuration with mutually exclusive macro preprocessor defines
// Note: an if..else structure is preferred, but with a large number of exclusive configuration this is also good
#ifdef SOFTHAND_FW          
    #define NUM_OF_DEV_PARAMS           (NUM_OF_PARAMS - 39 - NUM_OF_WR_PARAMS - NUM_OF_MS_PARAMS - NUM_OF_FB_PARAMS)  // Number of parameters saved in the EEPROM for SOFTHAND FIRMWARE
                                                              // All parameters except: additional first motor parameters (6), second motor configuration and parameters (23),
                                                              // Encoder configuration (2), ADC configuration (2), Joystick configuration (3), Read additional ADC port, 
                                                              // Record EMG on SD, Device type, Wrist parameters (3), Master parameters (2), Feedback parameters (3)
    #define NUM_OF_DEV_PARAM_MENUS      (NUM_OF_PARAMS_MENU - 4) // Number of parameters menu for SOFTHAND FIRMWARE
                                                                 // All menus except: Motor driver type, Device type, Wrist mode, Wrist direction
#endif
#ifdef GENERIC_FW
    #define NUM_OF_DEV_PARAMS           (NUM_OF_PARAMS - 1 - NUM_OF_MS_PARAMS - NUM_OF_FB_PARAMS)  // Number of parameters saved in the EEPROM for GENERIC FIRMWARE
                                                             // All parameters except: Wrist parameters (only 1), Master parameters (2), Feedback parameters (3)
    #define NUM_OF_DEV_PARAM_MENUS      (NUM_OF_PARAMS_MENU - 1)  // Number of parameters menu
                                                                  // All menus except: Wrist direction
#endif
//#ifdef MASTER_FW (Not necessary statements for the moment)
//    #define NUM_OF_DEV_PARAMS           (NUM_OF_PARAMS - 1 - NUM_OF_FB_PARAMS)  // Number of parameters saved in the EEPROM for MASTER FIRMWARE
//                                                             // All parameters except:  Wrist parameters (only 1), Feedback parameters (3)
//    #define NUM_OF_DEV_PARAM_MENUS      NUM_OF_PARAMS_MENU   // Number of parameters menu
//#endif
#ifdef OTBK_ACT_WRIST_MS_FW
    #define NUM_OF_DEV_PARAMS           (NUM_OF_PARAMS - NUM_OF_FB_PARAMS)  // Number of parameters saved in the EEPROM for MASTER FIRMWARE
                                                                            // All parameters except: Feedback parameters (3)
    #define NUM_OF_DEV_PARAM_MENUS      NUM_OF_PARAMS_MENU     // Number of parameters menu
#endif
#ifdef AIR_CHAMBERS_FB_FW
    #define NUM_OF_DEV_PARAMS           NUM_OF_PARAMS          // Number of parameters saved in the EEPROM for AIR_CHAMBERS_FB_FW FIRMWARE
                                                               // All parameters except: Wrist parameters (3)
    #define NUM_OF_DEV_PARAM_MENUS      NUM_OF_PARAMS_MENU     // Number of parameters menu
#endif


// Default number of IMUs configuration
#ifdef SOFTHAND_FW
    #define NUM_DEV_IMU         1       /*!< Number of device IMU for SOFTHAND FIRMWARE.*/
#else
    #define NUM_DEV_IMU         N_IMU_MAX
#endif


#endif
/* [] END OF FILE */