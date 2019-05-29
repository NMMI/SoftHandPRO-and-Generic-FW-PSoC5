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
* \file         SD_RTC_functions.c
*
* \brief        Implementation of SD and RTC module functions.
* \date         February 13, 2019
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2019 Centro "E.Piaggio". All rights reserved.
*/

#include "SD_RTC_functions.h"

// --------------------------------------------------------
// DS1302_write
void DS1302_write(uint8 address, uint8 data_wr) {

  CLK_RTC_Write(0);  
  CS_RTC_Write(1);
  CyDelayUs(5);
  DS1302_writeByte(address);
  DS1302_writeByte(data_wr);
  
  CS_RTC_Write(0);
  CyDelayUs(5);
}

void DS1302_writeByte(uint8 data_wr) {
  
  shiftOut_RTC(data_wr);

}

uint8 DS1302_read(uint8 address) {

  uint8 readValue;
  CLK_RTC_Write(0); 
  CS_RTC_Write(1);
  CyDelayUs(5);
  DS1302_writeByte(address);
  readValue = DS1302_readByte();
  CS_RTC_Write(0);
  CyDelayUs(5);
  return readValue;
  
}

uint8 DS1302_readByte() {

  uint8 value = 0;
  uint8 currentBit = 0;
  int i=0;
  for (i = 0; i < 8; ++i)
  {
    currentBit = MISO_RTC_Read();
    value |= (currentBit << i);
    CLK_RTC_Write(1);
    CyDelayUs(1);
    CLK_RTC_Write(0);
  }
  return value;
}

void shiftOut_RTC(uint8 val)
{
    uint8 i;
    for (i = 0; i < 8; i++)  {
        MOSI_RTC_Write(!!(val & (1 << i)));
        CLK_RTC_Write(1);
        CLK_RTC_Write(0);
    }
}

void store_RTC_current_time(){
    uint8 rtc_data;
    
    // Update current time
    rtc_data = DS1302_read(DS1302_DATE_RD);
    g_mem.exp.curr_time[0] = (rtc_data/16) * 10 + rtc_data%16;
    rtc_data = DS1302_read(DS1302_MONTH_RD);
    g_mem.exp.curr_time[1] = (rtc_data/16) * 10 + rtc_data%16;
    rtc_data = DS1302_read(DS1302_YEAR_RD);
    g_mem.exp.curr_time[2] = (rtc_data/16) * 10 + rtc_data%16;


    rtc_data = DS1302_read(DS1302_HOUR_RD);
    g_mem.exp.curr_time[3] = (rtc_data/16) * 10 + rtc_data%16;
    rtc_data = DS1302_read(DS1302_MINUTES_RD);
    g_mem.exp.curr_time[4] = (rtc_data/16) * 10 + rtc_data%16;
    rtc_data = DS1302_read(DS1302_SECONDS_RD);
    g_mem.exp.curr_time[5] = (rtc_data/16) * 10 + rtc_data%16;
}

void set_RTC_time(){
    DS1302_write(DS1302_DATE_WR, (((g_mem.exp.curr_time[0] / 10)*16) + (g_mem.exp.curr_time[0] % 10)));
    DS1302_write(DS1302_MONTH_WR, (((g_mem.exp.curr_time[1] / 10)*16) + (g_mem.exp.curr_time[1] % 10)));
    DS1302_write(DS1302_YEAR_WR, (((g_mem.exp.curr_time[2] / 10)*16) + (g_mem.exp.curr_time[2] % 10)));
    DS1302_write(DS1302_HOUR_WR, (((g_mem.exp.curr_time[3] / 10)*16) + (g_mem.exp.curr_time[3] % 10)));
    DS1302_write(DS1302_MINUTES_WR, (((g_mem.exp.curr_time[4] / 10)*16) + (g_mem.exp.curr_time[4] % 10)));
    DS1302_write(DS1302_SECONDS_WR, (((g_mem.exp.curr_time[5] / 10)*16) + (g_mem.exp.curr_time[5] % 10)));
}

/*******************************************************************************
* Function Name: Init SD Filesystem
*********************************************************************************/
void InitSD_FS()
{
    //char sdFile[12] = "";              // No long file name (LFN) support enabled. Max 12 chars for file name
    char sdFile[100] = "";
    char sdParam[100] = "";
    char sdDir[100] = "";
    char lastsdParam[100] = "";
    char info_[2500] = "";
    char info_read_[2500] = "";
    char Data_filename[10] = "UseStats";
    char Param_filename[7] = "Param";
    uint8 Write_new_info = TRUE;
    uint8 num_files = 0;
    FS_FILE* pParam;
    FS_FILE* pFile_read;
    FS_DIR *pDir;
    uint32 i = 0;
    
    FS_Init();
    FS_FAT_SupportLFN();
        
    // Create Filesystem with USER\YYYY\MM\DD folder
    sprintf(sdDir, "\\%s", g_mem.exp.user_code_string); 
    FS_MkDir(sdDir);
    sprintf(sdDir, "%s\\20%02d", sdDir, g_mem.exp.curr_time[2]); 
    FS_MkDir(sdDir);
    sprintf(sdDir, "%s\\%02d", sdDir, g_mem.exp.curr_time[1]);
    FS_MkDir(sdDir);
    sprintf(sdDir, "%s\\%02d", sdDir, g_mem.exp.curr_time[0]);
    FS_MkDir(sdDir);
    
    // Create param info
    prepare_SD_param_info(info_);
            
    // Open folder and count the number of files, it should be even (1 param and 1 data file)
    pDir = FS_OpenDir(sdDir);
    num_files = FS_GetNumFiles(pDir);    

    // Prepare filenames

    if (num_files != 0) {
        // The directory is not empty
         
        // Decide whether to create new param and data files or not
        sprintf(lastsdParam, "%s\\%s_%d.csv", sdDir, Param_filename, (num_files/2) - 1);

        pFile_read = FS_FOpen(lastsdParam, "r"); 
        if (pFile_read != 0) {
            i = FS_FRead(info_read_, 1, sizeof(info_read_) - 1, pFile_read);
            info_read_[i] = 0;
            if (strcmp(info_, info_read_)) {    // if different
                Write_new_info = TRUE;
                sprintf(sdParam, "%s\\%s_%d.csv", sdDir, Param_filename, num_files/2);  // new param filename
                sprintf(sdFile, "%s\\%s_%d.csv", sdDir, Data_filename, num_files/2);    // new data filename
            }
            else {
                sprintf(sdParam, "%s\\%s_%d.csv", sdDir, Param_filename, (num_files/2)-1);  // last param filename
                sprintf(sdFile, "%s\\%s_%d.csv", sdDir, Data_filename, (num_files/2)-1);    // last data filename
                Write_new_info = FALSE;
            }
        }
        FS_FClose(pFile_read);
    } 
    else {
        // The directory is empty
        sprintf(sdParam, "%s\\%s_%d.csv", sdDir, Param_filename, 0);  // first param filename
        sprintf(sdFile, "%s\\%s_%d.csv", sdDir, Data_filename, 0);    //first data filename
        Write_new_info = TRUE;
    } 
    FS_CloseDir(pDir);
    
    // Write chosen data in SD card        
    if (Write_new_info) {            
        pParam = FS_FOpen(sdParam, "a");
        //info_ string has been already created
        FS_Write(pParam, info_, strlen(info_));
        FS_FClose(pParam);
    }
    
    // Open chosen data file in append mode
    pFile = FS_FOpen(sdFile, "a");
    if (Write_new_info) { 
        // The file doesn't exist, so write legend info inside
        prepare_SD_legend(info_);
        FS_Write(pFile, info_, strlen(info_));
    }
    //FS_FClose(pFile); 
}

/*******************************************************************************
* Function Name: Write SD Param File
*********************************************************************************/
void Write_SD_Param_file(){
    char info_[2500] = "";
    
    prepare_SD_param_info(info_);
    FS_Write(pFile, info_, strlen(info_));
}