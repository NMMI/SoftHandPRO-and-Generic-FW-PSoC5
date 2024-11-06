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
* \file         IMU_functions.c
*
* \brief        Implementation of IMU module functions.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

#include "IMU_functions.h"

extern uint8 Accel[N_IMU_MAX][6];
extern uint8 Gyro[N_IMU_MAX][6];
extern uint8 Mag[N_IMU_MAX][6];
extern uint8 MagCal[N_IMU_MAX][3];

/*******************************************************************************
* Function Name: Imus Reset
*********************************************************************************/	
void ImusReset() {
    uint8 i;
    
    CyDelay(10);
    InitIMUgeneral();

    CyDelay(10);
    
    // Initialize quaternion
    for (i = 0; i<N_IMU_MAX; i++) {
        Quat[i][0] = 0.999;
        Quat[i][1] = 0.01;
        Quat[i][2] = 0.01;
        Quat[i][3] = 0.01;
    }
}

/*******************************************************************************
* Function Name: UpdateIMUDefine
*********************************************************************************/
void UpdateIMUDefine(){
}
/*******************************************************************************
* Function Name: IMU Initialization
*********************************************************************************/
void InitIMU(uint8 n){	
    
    // ################################# IMPORTANTE #######################################
    // ####################################################################################
    // ####################################################################################
    
    // Rendere compatibili performance di MPU9250 e LSM6DSRX in termini di ODR, FSR, LPF...
    
    // ####################################################################################
    // ####################################################################################
    // ####################################################################################
    
    switch (g_imu[n].dev_type) {
        case MPU9250:
        	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x10); 
        	CyDelay(10);	
            WriteControlRegisterIMU(MPU9250_CONFIG, 0x05); //Gyro & Temp Low Pass Filter 0x01 = 184Hz, 0x02 = 92Hz, 0x04 = 20Hz, 0x05 = 10Hz ...
            CyDelay(10);	
        	WriteControlRegisterIMU(MPU9250_GYRO_CONFIG , GYRO_SF_2000); //Gyro full scale select 0x00=250°/s 0x80=500°/s 0x18=2000°/s 
        	CyDelay(10);
            WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG, ACC_SF_2G); // Acc full scale select 0x00 = 2g 0x08 = 4g 0x10 = 8g 0x18 = 16g
            CyDelay(10);
            WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG2, NO_ACC_FIL); // Acc LPF BW: 0x00 = no LPF, 0x05 = 10 Hz 
            CyDelay(10);
            //WriteControlRegisterIMU(MPU9250_PWR_MGMT_2, 0x07); //0x00 = Gyro enabled, 0x07 = Gyro disabled.
            //To set axel ODR = 4KHZ: (MPU9250_PWR_MGMT_2, 0x07) + (MPU9250_ACCEL_CONFIG2, 0x00) ;
            WriteControlRegisterIMU(MPU9250_USER_CTRL, 0x20);  //Enable I2C master
        	CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_MST_CTRL, 0x0D); //set slave I2C speed, 0x4D = DRDY interrupt when ext sensor register are loaded , 0x0D = DRDY @axelODR
            CyDelay(10);
            
        	//WRITE slave
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x0C); // WCR  | AK8963_address (0x0C) - compass
        	CyDelay(10);			
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, AK8936_CNTL); //select slave register to write
        	CyDelay(10);	
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_D0, 0x16); //value to be written: Compass mode = 0x12 continuous mode1(8Hz ODR), 0x16 continuous mode2(100Hz ODR)
        	CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x81); //1 bit to write + enable this slave
        	CyDelay(10);
            
        	//READ slave
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x8C); // RCR  | AK8963_address (0x0C) 
        	CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, 0x03); // Address to start reading: 0x03 = Xout Low, 0x02 = STATUS_REG_1-->DRDY
        	CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x87);   //0x87 = Read 7 bytes, 0x03 to 0x09(STATUS_REG_2, needed to complete the reading). (Xout from EXT_SENS_DATA_00)
                                                                    //0x88 = Read 8 bytes, 0x02(STATUS_REG_1 = DRDY) to 0x09. (Xout from EXT_SENS_DATA_01)
                                                                    //0xD should be needed to swap the value acquired by the magnetometer (which are L to H), but it works also otherwise, so boh
        	CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x00); 
        	CyDelay(20);
            break;
            
            case LSM6DSRX:
                WriteControlRegisterIMU(LSM6DSRX_CTRL1_XL,0x90);    //ODR = 3.3 kHz,    FSR = +- 2g
                CyDelay(20);
                WriteControlRegisterIMU(LSM6DSRX_CTRL3_C,0x40);     //BDU
                CyDelay(20);
                WriteControlRegisterIMU(LSM6DSRX_CTRL2_G,0x4C);     //ODR = 104 Hz,     FSR = +- 2000dps
                CyDelay(20);
                Temp[0][0] =0;
                Temp[0][1] = OneShot_ReadRoutine(EXT_SENS_ADDR,LIS2MDL_WHO_AM_I); //LIS2MDL -->valore WHO_AM_I = 64;
                CyDelay(20);
                OneShot_WriteRoutine(EXT_SENS_ADDR,LIS2MDL_CFG_REG_A,0x0C); //100Hz, continuous mode
                CyDelay(20);
                OneShot_WriteRoutine(EXT_SENS_ADDR,LIS2MDL_CFG_REG_B,0x00); //0x02 = offset canc enabled (need to store value in dedicated register),  
                                                                            //0x00 = offset canc disabled
                CyDelay(20);
                OneShot_WriteRoutine(EXT_SENS_ADDR,LIS2MDL_CFG_REG_C,0x10); //BDU
                CyDelay(20);
                Continuous_ReadRoutine(EXT_SENS_ADDR,LIS2MDL_OUTX_L_REG,0x06); //Read 6 bytes
                CyDelay(20);
                
            break;
            default:
            break;
    }
}

/*******************************************************************************
* Function Name: IMU Mag Cal Initialization
*********************************************************************************/
void InitIMUMagCal(uint8 k_imu){	
    switch (g_imu[k_imu].dev_type){
        case MPU9250:
        	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x10); 
            //gyro drive and pll circuitry enabled, sense paths are disabled. This is a low power mode that allows quick enabling of the gyros.
        	CyDelay(10);	
        	WriteControlRegisterIMU(MPU9250_USER_CTRL, 0x20);  
            //Enable the I2C Master I/F module; pins ES_DA and ES_SCL are isolated from pins SDA/SDI and SCL/ SCLK.
            CyDelay(10);
            WriteControlRegisterIMU(MPU9250_CONFIG, 0x06); 
            //Gyro & Temp Low Pass Filter 0x01 = 184Hz, 0x02 = 92Hz, 0x04 = 20Hz, 0x05 = 10Hz, 0x06 = 5Hz
            CyDelay(10);	
        	WriteControlRegisterIMU(MPU9250_GYRO_CONFIG , GYRO_SF_2000); //Gyro full scale select 0x00=250°/s 0x80=500°/s 0x18=2000°/s 
        	CyDelay(10);
            WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG, ACC_SF_2G); // Acc full scale select 0x00 = 2g 0x08 = 4g 0x10 = 8g 0x18 = 16g
            CyDelay(10);
            WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG2, 0x05); // 10Hz LPF
            CyDelay(10);
        	//mag register
        	WriteControlRegisterIMU(MPU9250_I2C_MST_CTRL, 0x0D); //set slave I2C speed to 400KHz
        	CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x0C); //set compass address + WriteBit
        	CyDelay(10);			
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, AK8936_CNTL); //compass address for register CTRL1
        	CyDelay(10);	
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_D0, 0x1F); 
            ////Value to write in register CTRL1 (0x1F Fuse ROM access + select 16 bit output)
        	//Values of addresses from 10H to 12H can be read only in Fuse access mode.
            CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x81); 
            ///Enable reading data from this slave at the sample rate and storing data at the first available EXT_SENS_DATA register, 
            //which is always EXT_SENS_DATA_00 for I2C slave 0. + 1 byte to read
            CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x8C); // ReadBit  | AK8963_address (0x0C) 
        	CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, 0x10); // 0x10:start from ASAX
            CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x83);
        	//Enable reading data from this slave at the sample rate and storing data at the first available EXT_SENS_DATA register, 
            //Switch byte order from EXT_SENS_DATA and couples registers
            //which is always EXT_SENS_DATA_00 for I2C slave 0. + 3 byte to read
            CyDelay(10);
        	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x00); 
        	CyDelay(20);
        break;
        case LSM6DSRX:
            break;
              default:
            break;
        }
}

/********************************************************************************
* Function Name: ChipSelectorIMU                                                   *
*********************************************************************************/
void ChipSelectorIMU(int n)
{
    Chip_Select_IMU_Write(IMU_IDs[n]);
}

/********************************************************************************
* Function Name: InitIMUGeneral                                                 *
*********************************************************************************/
void InitIMUgeneral()
{       
    uint8 k_imu, j;
    uint8 count = 0;
    int tmp[N_IMU_MAX];
    uint8 WhoAmI_MPU = 0;
    uint8 WhoAmI_LSM = 0;
    
    N_IMU_Connected = 0; 
    
    // Initialize Memory Structure 
    memset(&g_imu, 0, sizeof(struct st_imu));
    memset(&IMU_IDs, 0, sizeof(IMU_IDs));
               
    // Initialize magnetometer
    for (k_imu = 0; k_imu < N_IMU_MAX; k_imu++) {
        for(j = 0; j < 3; j++){
            offset[k_imu][j] = 0;
            scale[k_imu][j] = 1;
        }
        avg[k_imu] = 1;
    }
    
    // First ping to be sure to wakeup IMUs
    for (k_imu = 0; k_imu < N_IMU_MAX; k_imu++) {	
	    Chip_Select_IMU_Write(k_imu);
	    CyDelay(10);
        ReadControlRegisterIMU(LSM6DSRX_WHO_AM_I);
        ReadControlRegisterIMU(MPU9250_WHO_AM_I);
        Chip_Select_IMU_Write(7);
    }
    count = 0;
 
    for (k_imu = 0; k_imu < NUM_DEV_IMU; k_imu++) {      
    	Chip_Select_IMU_Write(k_imu);
        CyDelay(10);
    	WhoAmI_MPU = ReadControlRegisterIMU(MPU9250_WHO_AM_I);
        CyDelay(10);
        WhoAmI_LSM = ReadControlRegisterIMU(LSM6DSRX_WHO_AM_I);
        CyDelay(10);
        if (WhoAmI_MPU == MPU9250_WHO_AM_I_VALUE && WhoAmI_LSM != LSM6DSRX_WHO_AM_I_VALUE){
            g_imu[count].dev_type = MPU9250;
            g_imuNew[count].dev_type = MPU9250;
            N_IMU_Connected++;
            count++;
            tmp[k_imu] = 1;
        }
        else if (WhoAmI_LSM == LSM6DSRX_WHO_AM_I_VALUE && WhoAmI_MPU != MPU9250_WHO_AM_I_VALUE){
            g_imu[count].dev_type = LSM6DSRX;
            g_imuNew[count].dev_type = LSM6DSRX;
            N_IMU_Connected++;
            count++;
            tmp[k_imu] = 1;
        }
        else{
            tmp[k_imu] = 0;
        }
        Chip_Select_IMU_Write(7);
    }
           count =0;
    
    
    for(k_imu = 0; k_imu < N_IMU_MAX; k_imu++)
    {
        if(tmp[k_imu] == 1)
        {
            IMU_IDs[count] = k_imu;
            count++;
        }
    }            

    // Initialize IMU to Read MagCal Parameters
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++) 
    {	
	    ChipSelectorIMU(k_imu);
	    InitIMUMagCal(k_imu);
        Chip_Select_IMU_Write(7);
    }
    
    // Reading of MagCal Parameters
    CyDelay(50);
    for (k_imu = 0; k_imu <  N_IMU_Connected; k_imu++){
        ChipSelectorIMU(k_imu);
        ReadMagCal(k_imu);  
        Chip_Select_IMU_Write(7);
    }
    
    // Standard Initialization of the IMU
    CyDelay(10);
    for (k_imu = 0; k_imu <  N_IMU_Connected; k_imu++) 
    {	
	    ChipSelectorIMU(k_imu);
	    InitIMU(k_imu);
        Chip_Select_IMU_Write(7);
	    
    }
    CyDelay(50);
    
    // Set Standard Read for the IMU
    memset(&single_imu_size, 0, sizeof(single_imu_size));
    imus_data_size = 1; //header    
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++)
    {
        single_imu_size[k_imu] = 1 + 6*c_mem.imu.IMU_conf[k_imu][0] + 6*c_mem.imu.IMU_conf[k_imu][1] + 6*c_mem.imu.IMU_conf[k_imu][2]+ 16*c_mem.imu.IMU_conf[k_imu][3] + 2*c_mem.imu.IMU_conf[k_imu][4]+ 1;
        imus_data_size = imus_data_size + single_imu_size[k_imu];
    }
    imus_data_size = imus_data_size + 1;    //checksum*/
}

/*******************************************************************************
* Function Name: IMU Read
*********************************************************************************/	
void ReadIMU(int n)
{
    if (c_mem.imu.IMU_conf[n][0]) ReadAcc(n);
    if (c_mem.imu.IMU_conf[n][1]) ReadGyro(n);
    if (c_mem.imu.IMU_conf[n][2]) ReadMag(n);
    if (c_mem.imu.IMU_conf[n][3]) ReadQuat(n);
    if (c_mem.imu.IMU_conf[n][4]) ReadTemp(n);
}

/*******************************************************************************
* Function Name: Read Acc's Data of IMU n
*********************************************************************************/
void ReadAcc(int n)
{   static uint8 i;
    uint8 tmp = 0,DRDY ;
	static uint8 AccStartAddress;

    AccStartAddress = g_imu[n].dev_type ? LSM6DSRX_OUTX_L_A : MPU9250_ACCEL_XOUT_H;
     // DRDY = ReadControlRegisterIMU(0x3A);
         //  if (DRDY & 0x01){
    for (i = 0; i < 6; i++){
        Accel[n][ i + g_imu[n].dev_type*(1 - 2 *( i % 2 ))] = ReadControlRegisterIMU(AccStartAddress + i);  
    }  
    for (i = 0; i < 3; i++) {
        tmp = Accel[n][2*i];
        //  Order of LSM6DSRX register are inverted to be compatible with the ones of  MPU9250
        g_imuNew[n].accel_value[i] = (int16)((uint16)tmp <<8 | Accel[n][2*i + 1]);
        
    }
   // }
}

/*******************************************************************************
* Function Name: Read Gyro's Data of IMU n
*********************************************************************************/
void ReadGyro(int n){
    static uint8 i;
    uint8 tmp = 0;
	static uint8 GyroStartAddress;
    
    GyroStartAddress = g_imu[n].dev_type ? LSM6DSRX_OUTX_L_G : MPU9250_GYRO_XOUT_H;
    for (i = 0; i < 6; i++){
        //  Order of LSM6DSRX register are inverted to be compatible with the ones of  MPU9250
        Gyro[n][ i + g_imu[n].dev_type*(1 - 2 *( i % 2 ))] = ReadControlRegisterIMU(GyroStartAddress + i);  
    }
            
    for (i = 0; i < 3; i++) {
        tmp = Gyro[n][2*i];
        g_imuNew[n].gyro_value[i] = (int16)((uint16)tmp<<8 | Gyro[n][2*i + 1]);
    }
}
        
/*******************************************************************************
* Function Name: Read Compas' Data of IMU n
*********************************************************************************/
void ReadMag(int n){	
    uint8 i, tmp;
    uint8 XLDA;
    uint8 DRDY;
    uint8 SENS_HUB_ENDOP;
       
	switch (g_imu[n].dev_type){
        case MPU9250:
            DRDY = ReadControlRegisterIMU(0x3A);
            if (DRDY & 0x01){
                for (i = 0; i < 6; i++){
                    Mag[n][ i + (1 - 2 *( i % 2 ))] = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_00 + i);
            	}
            }
        break;
    
        case LSM6DSRX:
            do {
                XLDA = ReadControlRegisterIMU(LSM6DSRX_STATUS_REG);          
            }
            while ((XLDA & 0b00000001) == 0);   
        
            do {
                SENS_HUB_ENDOP = ReadControlRegisterIMU(LSM6DSRX_STATUS_MASTER_MAINPAGE);   
            }
            while ((SENS_HUB_ENDOP & 0b00000001) == 0);

            WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x40);   
            CyDelayUs(100);  
            ReadControlRegisterIMU(LSM6DSRX_WHO_AM_I); 
               
            for (i = 0; i < 6; i++){
                Mag[n][i + g_imu[n].dev_type*(1 - 2 *( i % 2 ))]= ReadControlRegisterIMU(LSM6DSRX_SENSOR_HUB_1 + i);  
            }
           
            WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x00);   
            CyDelayUs(100);
        
        break;
    }
           
    for (i = 0; i < 3; i++) {
        tmp = Mag[n][2*i];
        g_imuNew[n].mag_value[i] = (int16)((uint16)tmp <<8 | Mag[n][2*i + 1]);
        if (!MAGcal)
        g_imuNew[n].mag_value[i] = (int16)(((( (float)g_imuNew[n].mag_value[i] - offset[n][i])/scale[n][i]))*0.15*factor[n][i]*avg[n]);
    }  
}


/********************************************************************************
* Function Name: MagCalibration
*********************************************************************************/
void MagCalibration(){
    MAGcal = 1;
    uint8 k_imu, j;
    int16 max, min;
    
    LED_control(YELLOW_BLINKING);
    
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++){ 
        for (j = 0; j < 3; j++) {
            Mag_maxval[k_imu][j]= -32000;
            Mag_minval[k_imu][j]= 32000;
        }
    }
    
    MY_TIMER_OVF_Cnt = 0;
    MY_TIMER_WriteCounter(65535);   // Reset counter
    while (MY_TIMER_OVF_Cnt < 200){ //65,536 ms *1000 = 60s
    uint8 k_imu, j, tmp ;
            for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++){ 
            // Read k_imu IMU
            ChipSelectorIMU(k_imu);
            ReadMag(k_imu);
            for (j = 0; j < 3; j++) {

                if (g_imuNew[k_imu].mag_value[j] < Mag_minval[k_imu][j]){
                    Mag_minval[k_imu][j] = (float)(g_imuNew[k_imu].mag_value[j]);
                }
                
                if (g_imuNew[k_imu].mag_value[j] > Mag_maxval[k_imu][j]){   
                    Mag_maxval[k_imu][j] = (float)(g_imuNew[k_imu].mag_value[j]);
                }
                max = (int16)(Mag_maxval[k_imu][j]);
                min = (int16)(Mag_minval[k_imu][j]);
             /*   UART_RS485_PutChar(((char*)(&max))[1]);
                UART_RS485_PutChar(((char*)(&max))[0]);
                UART_RS485_PutChar(((char*)(&min))[1]);
                UART_RS485_PutChar(((char*)(&min))[0]);
                CyDelay(10);*/
            }
            Chip_Select_IMU_Write(7);
        }
     //   UART_RS485_PutChar('a');
     //    UART_RS485_PutChar('a');
    }
    
    
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++){ 
        for (j = 0; j < 3; j++) {
            //if (g_imu[k_imu].dev_type == LSM6DSRX ) 
             //   offset[k_imu][j] = 0;
           // else 
                offset[k_imu][j] = ((Mag_maxval[k_imu][j] + Mag_minval[k_imu][j])/2 );
            scale[k_imu][j] = ((Mag_maxval[k_imu][j] - Mag_minval[k_imu][j]))/2;
            min = (int16)offset[k_imu][j];
            max = (int16)scale[k_imu][j];
       /*     UART_RS485_PutChar(((char*)(&max))[1]);
            UART_RS485_PutChar(((char*)(&max))[0]);
            UART_RS485_PutChar(((char*)(&min))[1]);
            UART_RS485_PutChar(((char*)(&min))[0]);
          CyDelay(10); */
        }
       //UART_RS485_PutChar('a');
        //UART_RS485_PutChar('a');
        avg[k_imu] = (scale[k_imu][0]+scale[k_imu][1]+scale[k_imu][2])/3;
    }
    LED_control(OFF);
    MAGcal = 0;
}


/********************************************************************************
* Function Name: ReadMagCal
*********************************************************************************/
void ReadMagCal(int n){ 
    
    uint8 i;
	
	switch (g_imu[n].dev_type){
        case MPU9250:
            for ( i = 0; i < 3; i++){
            	MagCal[n][i] = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_00 + i);
                factor[n][i] = (((float)MagCal[n][i]+128)*1.0)/256 ;
            }
            break;

        case LSM6DSRX:
            for ( i = 0; i < 3; i++){
                MagCal[n][i] = 128; 
                factor[n][i] = 1;
            }
            break;
            
        default:
            break;
    }
}
    
/*******************************************************************************
* Function Name: Read Quaternion of IMU n
*********************************************************************************/
void ReadQuat(int n)
{
    int16 var_h;
    float qd0,qd1,qd2,qd3;        
    float aP[3]; //, fa[3];
    float Napla[4],g[4],qL[4];
    uint8 i;
    
    if (!c_mem.imu.IMU_conf[n][0]) ReadAcc(n);
    if (!c_mem.imu.IMU_conf[n][1]) ReadGyro(n);
    
    // assume cycle time is the same of last function_scheduler execution,
    // without adding another counter
    
    // how N sees P; N is 'Earth' equivalent, P is for sensor
    // e.g. N = imu1, P = imu0, how imu1 sees imu0

    var_h = Accel[n][0];
    aP[0] = (int16_t)(var_h << 8 | Accel[n][1])*TICK2ACC; 
    var_h = Accel[n][2];
    aP[1] = (int16_t)(var_h << 8 | Accel[n][3])*TICK2ACC; 
    var_h = Accel[n][4];
    aP[2] = (int16_t)(var_h << 8 | Accel[n][5])*TICK2ACC;
    
    g[0] = 0;
    var_h = Gyro[n][0];
    g[1] = ((int16)(var_h << 8 | Gyro[n][1]))*TICK2GYRO;
    if ( fabs(g[1]) < GYRO_THR ) g[1] = 0;
    var_h = Gyro[n][2];
    g[2] = ((int16)(var_h << 8 | Gyro[n][3]))*TICK2GYRO; 
    if ( fabs(g[2]) < GYRO_THR ) g[2] = 0;
    var_h = Gyro[n][4];
    g[3] = ((int16)(var_h << 8 | Gyro[n][5]))*TICK2GYRO;
    if ( fabs(g[3]) < GYRO_THR ) g[3] = 0;
    
    // Normalize to 1
    v3_normalize(aP);
    
    // Current quaternion assignment
    qL[0] = Quat[n][0];
    qL[1] = Quat[n][1];
    qL[2] = Quat[n][2];
    qL[3] = Quat[n][3];   
    
    // Napla computation
    Napla[0] = (-qL[2]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) + qL[1]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])))*BETA;
    Napla[1] = (qL[3]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) + qL[0]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])) -2*qL[1]*((2*(0.5 - qL[1]*qL[1] - qL[2]*qL[2]) - aP[2])))*BETA;
    Napla[2] = (-qL[0]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) +qL[3]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])) -2*qL[2]*((2*(0.5 - qL[1]*qL[1] - qL[2]*qL[2]) - aP[2])))*BETA;
    Napla[3] = (qL[1]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) + qL[2]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])))*BETA;
    
    qd0 = 0.5*(-(qL[1]*g[1] + qL[2]*g[2] + qL[3]*g[3])) - Napla[0];
    qd1 = 0.5*(qL[0]*g[1] + (qL[2]*g[3] - qL[3]*g[2])) - Napla[1];
    qd2 = 0.5*(qL[0]*g[2] + (qL[3]*g[1] - qL[1]*g[3])) - Napla[2];
    qd3 = 0.5*(qL[0]*g[3] + (qL[1]*g[2] - qL[2]*g[1])) - Napla[3];
            
    qL[0] = qL[0] + qd0*cycle_time;
    qL[1] = qL[1] + qd1*cycle_time;
    qL[2] = qL[2] + qd2*cycle_time;
    qL[3] = qL[3] + qd3*cycle_time;
    
    if (sqrt(qL[0]*qL[0] + qL[1]*qL[1] + qL[2]*qL[2] + qL[3]*qL[3]) == 0) {
	    qL[0] = Quat[n][0];
	    qL[1] = Quat[n][1];
	    qL[2] = Quat[n][2];
	    qL[3] = Quat[n][3];
    }
        
    v4_normalize(qL);
    
    // Quaternion update      
    Quat[n][0] = qL[0];
    Quat[n][1] = qL[1];
    Quat[n][2] = qL[2];
    Quat[n][3] = qL[3];
    
    for (i = 0; i < 4; i++) {
    g_imuNew[n].quat_value[i] = (float)(Quat[n][i]);
    }
}

/********************************************************************************
* Function Name: ReadAllIMUs
*********************************************************************************/
void ReadAllIMUs(){
    static uint8 k_imu = 0;
    
     
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++){ 
        ChipSelectorIMU(k_imu);
        ReadIMU(k_imu);
        Chip_Select_IMU_Write(7);
    }
}

/*******************************************************************************
* Function Name: Read Temperature Data of IMU n
*********************************************************************************/
void ReadTemp(int n)
{   uint8 tmp;
    // MPU9250:
    // sensitivity  333.87 LSB/°C, ADC resolution = 16
    // ODR = 52 Hz
    
    //LSM6DSRX:
    // sensitivity  256 LSB/°C, ADC resolution = 16
    // ODR = 52 Hz
    
    
    /*static uint8 i;
	static uint8 TempStartAddress;
    TempStartAddress = g_imu[n].dev_type ? LSM6DSRX_OUT_TEMP_L : MPU9250_TEMP_OUT_H;
    for (i = 0; i < 2; i++){
        //	Accel[row][0] = high; 
	    //  Accel[row][1] = low; 
        //  Order of LSM6DSRX register are inverted to be compatible with the ones of  MPU9250
        Temp[n][ i + g_imu[n].dev_type*(1 - 2 *( i % 2 ))] = ReadControlRegisterIMU(TempStartAddress + i);  
    }
    */

        tmp = Temp[n][0];
        g_imuNew[n].temp_value = (int16)(tmp<<8 | Temp[n][1]);
}



/********************************** *********************************************
* Function Name: Write Control Register
*********************************************************************************/
void WriteControlRegisterIMU(uint8 address, uint8 dta){
	SPI_IMU_WriteByte(WRITEBIT | address);
    SPI_IMU_WriteByte(dta);
	while(!( SPI_IMU_ReadTxStatus() & SPI_IMU_STS_TX_FIFO_EMPTY));
}

/*******************************************************************************
* Function Name: Read Control Register
*********************************************************************************/
uint8 ReadControlRegisterIMU(uint8 address){
	uint8 controlreg = 0;
	SPI_IMU_WriteByte(READBIT | address);  
    SPI_IMU_WriteByte(0x00);
    while(!( SPI_IMU_ReadTxStatus() & SPI_IMU_STS_SPI_DONE));
	controlreg = SPI_IMU_ReadByte();        //real data
    return controlreg;
}

/*******************************************************************************
* Function Name: SPI delay
*********************************************************************************/
void SPI_delay(){
    switch( c_mem.imu.SPI_read_delay ) {
        case 1:     // Low
            CyDelayUs((uint16)SPI_DELAY_LOW);
            break;
        case 2:     // High
            CyDelayUs((uint16)SPI_DELAY_HIGH);
            break;
        default:    // None
            break;
    }
}



void OneShot_WriteRoutine (uint8 address, uint8 subaddress, uint8 data){
    uint8 WR_ONCE_DONE;
    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x40);
    CyDelay(1);
    WriteControlRegisterIMU(LSM6DSRX_SLV0_ADD, address | 0x00);
    CyDelay(1);    
    WriteControlRegisterIMU(LSM6DSRX_SLV0_SUBADD, subaddress); 
    CyDelay(1);
    WriteControlRegisterIMU(LSM6DSRX_SLV0_CONFIG, 0x00); //ODR = 288Hz, FSR=+/-2g, LPF2 disabled
    CyDelay(1);
     
    WriteControlRegisterIMU(LSM6DSRX_DATAWRITE_SLV0, data); //ODR = 288Hz, FSR=+/-2g, LPF2 disabled
    CyDelay(1);
    WriteControlRegisterIMU(LSM6DSRX_MASTER_CONFIG, 0x4C); //ODR = 288Hz, FSR=+/-2g, LPF2 disabled
    CyDelay(1);
        do 
    {
        WR_ONCE_DONE = ReadControlRegisterIMU(LSM6DSRX_STATUS_MASTER);
    }
    while ((WR_ONCE_DONE & 0b10000000) == 0);
    
    WriteControlRegisterIMU(LSM6DSRX_MASTER_CONFIG, 0x08); 
    CyDelayUs(300);   
    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x00); 
    
}

uint8 OneShot_ReadRoutine(uint8 address, uint8 subaddress){
    uint8 XLDA;
    uint8 SENS_HUB_ENDOP;
    uint8 Data;
    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x40); 
    CyDelay(20);
    WriteControlRegisterIMU(LSM6DSRX_SLV0_ADD, address | 0x01); 
    CyDelay(20);   
    WriteControlRegisterIMU(LSM6DSRX_SLV0_SUBADD, subaddress); 
    CyDelay(20);
    WriteControlRegisterIMU(LSM6DSRX_SLV0_CONFIG, 0x01); //ODR = 288Hz, FSR=+/-2g, LPF2 disabled
    CyDelay(20);
    WriteControlRegisterIMU(LSM6DSRX_MASTER_CONFIG, 0x4C); //ODR = 288Hz, FSR=+/-2g, LPF2 disabled
    CyDelay(20);
    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x00); 
    CyDelay(20);
    ReadControlRegisterIMU(LSM6DSRX_OUTX_H_A); 
   
    do 
    {
        XLDA = ReadControlRegisterIMU(LSM6DSRX_STATUS_REG);  
        
    }
    while ((XLDA & 0b00000001) == 0); 
    
     
    do 
    {
        SENS_HUB_ENDOP = ReadControlRegisterIMU(LSM6DSRX_STATUS_MASTER_MAINPAGE);
        
    }
    while ((SENS_HUB_ENDOP & 0b00000001) == 0);

    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x40); 
    CyDelay(10);
    WriteControlRegisterIMU(LSM6DSRX_MASTER_CONFIG, 0x08); 
    CyDelay(10);
    CyDelayUs(300);  
    
    ReadControlRegisterIMU(LSM6DSRX_WHO_AM_I);  
    
    Data = ReadControlRegisterIMU(LSM6DSRX_SENSOR_HUB_1);  
    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x00); 
   // UART_PutChar(Data);
    return Data;
}

void Continuous_ReadRoutine(uint8 address, uint8 subaddress, uint8 slave_config){
 
    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x40);
    CyDelay(1);
    WriteControlRegisterIMU(LSM6DSRX_SLV0_ADD, address | 0x01);  
    CyDelay(1);  
    WriteControlRegisterIMU(LSM6DSRX_SLV0_SUBADD, subaddress); 
    CyDelay(1);
    WriteControlRegisterIMU(LSM6DSRX_SLV0_CONFIG, slave_config); //ODR = 288Hz, FSR=+/-2g, LPF2 disabled
    CyDelay(1);
    WriteControlRegisterIMU(LSM6DSRX_MASTER_CONFIG, 0x4C); //ODR = 288Hz, FSR=+/-2g, LPF2 disabled
    CyDelay(1);
    WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x00); 
    CyDelay(1);
 }

