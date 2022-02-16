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
* \file         SD_RTC_functions.c
*
* \brief        Implementation of SD and RTC module functions.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
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
    sprintf(sdDir, "\\%s", g_mem.user[g_mem.dev.user_id].user_code_string); 
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
     
    // Open file for EMG History in write mode (create it if it does not exist)
    if (c_mem.exp.record_EMG_history_on_SD){
        pEMGHFile = FS_FOpen(sdEMGHFile, "w");
        prepare_SD_EMG_History_legend(info_);
        FS_Write(pEMGHFile, info_, strlen(info_));
    }
    
    //Write updated data on R01 project
    FS_FILE* pR01File = FS_FOpen(sdR01File, "w");
    prepare_R01_info(info_);
    FS_Write(pR01File, info_, strlen(info_));
    FS_FClose(pR01File);
}

/*******************************************************************************
* Function Name: Write SD Param File
*********************************************************************************/
void Write_SD_Param_file(){
    char info_[2500] = "";
    
    prepare_SD_param_info(info_);
    FS_Write(pFile, info_, strlen(info_));
}

/*******************************************************************************
* Function Name: Read SD Closed File (Param or closed Data files)
*********************************************************************************/
void Read_SD_Closed_File(char* src_file, char* info_file, int n_p){
    int i;
    FS_FILE* pF;
    
    // Open param file in read mode
    pF = FS_FOpen(src_file, "r");
    if (pF != 0) {
        i = FS_FRead(info_file, 1, n_p, pF);
        info_file[i] = 0;
    }
    FS_FClose(pF);
}

/*******************************************************************************
* Function Name: Remove SD File
*********************************************************************************/
int Remove_SD_File(char* src_file){     
    return FS_Remove(src_file);  
}

/*******************************************************************************
* Function Name: Read SD Current Open Data File
*********************************************************************************/
void Read_SD_Current_Data(char* info_data, int n_d){
    int i;
    FS_FILE* pData;
    
    FS_FClose(pFile);
    
    // Open data file in read mode    
    pData = FS_FOpen(sdFile, "r");
    if (pData != 0) {
        i = FS_FRead(info_data, 1, n_d, pData);
        info_data[i] = 0;  
    }
    FS_FClose(pData);
    
    // Before exiting, reopen data file in append mode
    pFile = FS_FOpen(sdFile, "a");
}

/*******************************************************************************
* Function Name: Read SD Current Open EMG History File
*********************************************************************************/
void Read_SD_EMG_History_Data(){
    int i;
    FS_FILE* pData;
    char iData[10];
    
    FS_FClose(pEMGHFile);
    
    // Open data file in read mode    
    pData = FS_FOpen(sdEMGHFile, "r");
    
    UART_RS485_ClearTxBuffer();
    do {
        i = FS_FRead(iData, 1, 1, pData);
        UART_RS485_PutChar((uint8)iData[0]);
    }
    while(i);
    UART_RS485_PutChar((uint8)0);
    
    FS_FClose(pData);
    
    // Before exiting, reopen file in append mode
    pEMGHFile = FS_FOpen(sdEMGHFile, "a");
}

/*******************************************************************************
* Function Name: Get SD Directories List
*********************************************************************************/
int Get_DirectoriesList(char* path, char directories_list[3000][8], int first_idx){
    
    FS_FIND_DATA fd;
    int num_found_dirs = 0;
    char acDirname[20];    
    
    if (FS_FindFirstFile(&fd, path, acDirname, sizeof(acDirname)) == 0) {
        do {
            FS_DIR *pDir;
            char curr_path[100] = "";
            strcpy(curr_path, path);
            strcat(curr_path, "\\");
            strcat(curr_path, acDirname);
            strcat(curr_path, "\\");
            pDir = FS_OpenDir(curr_path);
            int num_files_dir = FS_GetNumFiles(pDir);
            FS_CloseDir(pDir);
            if (num_files_dir != -1){
  
                if (strcmp(acDirname, ".") && strcmp(acDirname, "..")){
                    strcpy(directories_list[first_idx + num_found_dirs], acDirname);                              
                    num_found_dirs++;
                }
            }    
 
        } while (FS_FindNextFile (&fd));
    }
    FS_FindClose(&fd);
    
    return num_found_dirs;
}

/*******************************************************************************
* Function Name: Get SD Filesystem
*********************************************************************************/
void Get_SD_FS(char* info_data){
        
    // Called with PING command
    // Get all the folders with path and contained files number in the CSV-like format
    // and send to API through info_data string
    // USER\YYYY\MM\DD, number_of_files
    
    
    strcpy(info_data, "");
    
    // Get users folders   
    int num_found_users = 0;
    char CYDATA found_users[20][8];         // Max. 20 users (each with 8 max. characters)
    char CYDATA folder_tree_data[3000][8];  //3000 days (>8 yrs) of everyday folders for each users (8=4+2+2, y+m+d)
    memset(&found_users, 0, 20*8);
    memset(&folder_tree_data, 0, 3000*8);
   
    num_found_users = Get_DirectoriesList("\\", folder_tree_data, 0);
    for (int i=0; i< num_found_users; i++){ // Copy user folders and free the tree structure
        strcpy(found_users[i], folder_tree_data[i]);
    }
    memset(&folder_tree_data, 0, 3000*8);
    
    int n_idx = 0;
    for (int i=0; i< num_found_users; i++){
        char y_path[100] = "\\";
        strcat(y_path, found_users[i]);
        strcat(y_path, "\\");
        
        int nyears = Get_DirectoriesList(y_path, folder_tree_data, n_idx);
        int nmonths = 0, ndays = 0;
        for (int j=0; j< nyears; j++){
            char m_path[100] = "\\";
            strcpy(m_path, y_path);
            strcat(m_path, folder_tree_data[n_idx + j]);
            strcat(m_path, "\\");
            nmonths = Get_DirectoriesList(m_path, folder_tree_data, n_idx + nyears);
            for (int k=0; k< nmonths; k++){
                char d_path[100] = "\\";
                strcpy(d_path, m_path);
                strcat(d_path, folder_tree_data[n_idx + nyears + k]);
                strcat(d_path, "\\");
                ndays = Get_DirectoriesList(d_path, folder_tree_data, n_idx + nyears + nmonths);
                for (int h=0; h< ndays; h++){
                    
                    char f_path[100] = "\\";
                    strcpy(f_path, d_path);
                    strcat(f_path, folder_tree_data[n_idx + nyears + nmonths + h]);
                    strcat(f_path, "\\");
                    
                    // A directory with data files cannot be empty
                    FS_DIR* pDir = FS_OpenDir(f_path);
                    int nfiles = FS_GetNumFiles(pDir);
                    FS_CloseDir(pDir);
                              
                    char str[100] = "";
                    sprintf(str, "\\%s\\%s\\%s\\%s,%d\r\n", found_users[i], folder_tree_data[n_idx+j], 
                            folder_tree_data[n_idx+nyears+k], folder_tree_data[n_idx+nyears+nmonths+h], nfiles);
                    strcat(info_data, str);
                }
            }
        }
        n_idx += nyears + nmonths + ndays;
    }    
}