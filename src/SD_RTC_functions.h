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
* \file         SD_RTC_functions.h
*
* \brief        Definition of SD and RTC module functions.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

#include <project.h>
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include "command_processing.h"


#define DS1302_SECONDS_WR       0x80
#define DS1302_SECONDS_RD       0x81
#define DS1302_MINUTES_WR       0x82
#define DS1302_MINUTES_RD       0x83
#define DS1302_HOUR_WR          0x84
#define DS1302_HOUR_RD          0x85
#define DS1302_DATE_WR          0x86
#define DS1302_DATE_RD          0x87
#define DS1302_MONTH_WR         0x88
#define DS1302_MONTH_RD         0x89
#define DS1302_YEAR_WR          0x8C
#define DS1302_YEAR_RD          0x8D

void DS1302_write(uint8 address, uint8 data_wr);
void DS1302_writeByte(uint8 data_wr);
uint8 DS1302_read(uint8 address);
uint8 DS1302_readByte();
void shiftOut_RTC(uint8 val);
void store_RTC_current_time();
void set_RTC_time();

// SD functions
void InitSD_FS();
void Write_SD_Param_file();
void Read_SD_Closed_File(char*, char*, int);
int  Remove_SD_File(char*);
void Read_SD_Current_Data(char*, int);
void Read_SD_EMG_History_Data();
int  Get_DirectoriesList(char* path, char directories_list[3000][8], int first_idx);
void Get_SD_FS(char*);

/* [] END OF FILE */
