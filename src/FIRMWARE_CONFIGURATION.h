// ----------------------------------------------------------------------------
// BSD 3-Clause License

// Copyright (c) 2019-2024, Centro "E.Piaggio"
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
 *  \date       Jun 05th, 2024
 *  \author     _Centro "E.Piaggio"_
 *  \copyright  (C) 2019-2024 Centro "E.Piaggio". All rights reserved.
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
    
#define VERSION         "v. 1.16"    
    
// Macro related to different firmware configurations
#ifdef SOFTHAND_FW
    #define FW_NAME                 "SoftHand PRO firmware"    
    
    #ifdef SH_XPRIZE            // SoftHand for AlterEgo XPRIZE version
        #define FW_NAME             "SoftHand AlterEgo XPRIZE"
    #endif
#else
    #ifdef GENERIC_FW
        #define FW_NAME                 "Generic firmware (PSoC5)"
    #else   
        #define MASTER_FW
        #ifdef AIR_CHAMBERS_FB_FW
            #define FW_NAME                 "Air Chambers Haptic Feedback firmware - Master configuration"
        #else //OTBK_ACT_WRIST_MS_FW
            #define FW_NAME                 "Ottobock Active Wrist firmware - Master configuration"
        #endif
    #endif
#endif      

// TODO
// SOFTHAND_PRO_2MOT firmware at the moment is just for USB control
// To be implemented: rest position, usage counters and EMG behavior

// Default number of IMUs configuration
#if defined(SOFTHAND_FW) && !defined(SH_XPRIZE)
    #define NUM_DEV_IMU         1       /*!< Number of device IMU for SOFTHAND FIRMWARE.*/
#else
    #define NUM_DEV_IMU         N_IMU_MAX
#endif


#endif
/* [] END OF FILE */