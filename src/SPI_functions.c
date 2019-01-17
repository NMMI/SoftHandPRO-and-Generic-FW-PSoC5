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
* \file         SPI_functions.c
*
* \brief        Implementation of SPI module functions.
* \date         February 01, 2018
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2019 Centro "E.Piaggio". All rights reserved.
*/

#include "SPI_functions.h"
#include "utils.h"
#include <SPI_IMU.h>

extern uint8 Accel[N_IMU_MAX][6];
extern uint8 Gyro[N_IMU_MAX][6];
extern uint8 Mag[N_IMU_MAX][6];
extern uint8 MagCal[N_IMU_MAX][3];

/*******************************************************************************
* Function Name: Imus Reset
*********************************************************************************/	
void ImusReset() {

    CyDelay(10);
    InitIMUgeneral();

    CyDelay(10);
    
}

/*******************************************************************************
* Function Name: IMU Initialization
*********************************************************************************/
void InitIMU(){	
    
	WriteControlRegister(MPU9250_PWR_MGMT_1, 0x10); 
	CyDelay(10);	
	WriteControlRegister(MPU9250_USER_CTRL, 0x20);  //I2C master enable - disable I2C (prima 0x30)
	CyDelay(10);
    WriteControlRegister(MPU9250_CONFIG, 0x06); //Gyro & Temp Low Pass Filter 0x01 = 184Hz, 0x02 = 92Hz, 0x04 = 20Hz, 0x05 = 10Hz
    CyDelay(10);	
	WriteControlRegister(MPU9250_GYRO_CONFIG , GYRO_SF_2000); //Gyro full scale select 0x00=250°/s 0x80=500°/s 0x18=2000°/s 
	CyDelay(10);
    WriteControlRegister(MPU9250_ACCEL_CONFIG, ACC_SF_2G); // Acc full scale select 0x00 = 2g 0x08 = 4g 0x10 = 8g 0x18 = 16g
    CyDelay(10);
    WriteControlRegister(MPU9250_ACCEL_CONFIG2, LP_ACC_FREQ_10);   
    CyDelay(10);
	//mag register
	WriteControlRegister(MPU9250_I2C_MST_CTRL, 0x0D); //set slave I2C speed
	CyDelay(10);
	//SLV0 (use to write)
	WriteControlRegister(MPU9250_I2C_SLV0_ADDR, 0x0C); //set compass address
	CyDelay(10);			
	WriteControlRegister(MPU9250_I2C_SLV0_REG, AK8936_CNTL); //compass mode register
	CyDelay(10);	
	// Istruction used to read Compass
	WriteControlRegister(MPU9250_I2C_SLV0_D0, 0x16); //0x12 continuous mode1  0x16 continuous mode2
	CyDelay(10);
	WriteControlRegister(MPU9250_I2C_SLV0_CTRL, 0x81); //enable data from register + 1 bit to write
	CyDelay(10);
	//SLV0 (use to read)
	WriteControlRegister(MPU9250_I2C_SLV0_ADDR, 0x8C); // RCR  | AK8963_address (0x0C) 
	CyDelay(10);
	// Istruction used to read Compass
	WriteControlRegister(MPU9250_I2C_SLV0_REG, 0x03); // 0x03:start from Xout Low in case of calibration 0x10:start from ASAX
	CyDelay(10);
	// Istruction used to read Compass
	WriteControlRegister(MPU9250_I2C_SLV0_CTRL, 0x8D); //How many bits read  SEMPRE DISPARI 0x8D era quella che funzionava
	CyDelay(10);
	WriteControlRegister(MPU9250_PWR_MGMT_1, 0x00); 
	CyDelay(20);
}	

/*******************************************************************************
* Function Name: IMU Mag Cal Initialization
*********************************************************************************/
void InitIMUMagCal(){	
    
	WriteControlRegister(MPU9250_PWR_MGMT_1, 0x10); 
	CyDelay(10);	
	WriteControlRegister(MPU9250_USER_CTRL, 0x20);  //I2C master enable - disable I2C (prima 0x30)
	CyDelay(10);
    WriteControlRegister(MPU9250_CONFIG, 0x06); //Gyro & Temp Low Pass Filter 0x01 = 184Hz, 0x02 = 92Hz, 0x04 = 20Hz, 0x05 = 10Hz
    CyDelay(10);	
	WriteControlRegister(MPU9250_GYRO_CONFIG , GYRO_SF_2000); //Gyro full scale select 0x00=250°/s 0x80=500°/s 0x18=2000°/s 
	CyDelay(10);
    WriteControlRegister(MPU9250_ACCEL_CONFIG, ACC_SF_2G); // Acc full scale select 0x00 = 2g 0x08 = 4g 0x10 = 8g 0x18 = 16g
    CyDelay(10);
    WriteControlRegister(MPU9250_ACCEL_CONFIG2, 0x05);
    CyDelay(10);
	//mag register
	WriteControlRegister(MPU9250_I2C_MST_CTRL, 0x0D); //set slave I2C speed
	CyDelay(10);
	//SLV0 (use to write)
	WriteControlRegister(MPU9250_I2C_SLV0_ADDR, 0x0C); //set compass address
	CyDelay(10);			
	WriteControlRegister(MPU9250_I2C_SLV0_REG, AK8936_CNTL); //compass mode register
	CyDelay(10);	
	WriteControlRegister(MPU9250_I2C_SLV0_D0, 0x1F); //0x1F ROM access
	CyDelay(10);
	WriteControlRegister(MPU9250_I2C_SLV0_CTRL, 0x81); //enable data from register + 1 bit to write
	CyDelay(10);
	//SLV0 (use to read)
	WriteControlRegister(MPU9250_I2C_SLV0_ADDR, 0x8C); // RCR  | AK8963_address (0x0C) 
	CyDelay(10);
	WriteControlRegister(MPU9250_I2C_SLV0_REG, 0x10); // 0x10:start from ASAX
	CyDelay(10);
	WriteControlRegister(MPU9250_I2C_SLV0_CTRL, 0x83);
	CyDelay(10);
	WriteControlRegister(MPU9250_PWR_MGMT_1, 0x00); 
	CyDelay(20);
}

/********************************************************************************
* Function Name: ChipSelector                                                   *
*********************************************************************************/
void ChipSelector(int n)
{
    Chip_Select_A_Write(3);
    
    Chip_Select_A_Write(n);
}

/********************************************************************************
* Function Name: InitIMUGeneral                                                 *
*********************************************************************************/
void InitIMUgeneral()
{
    uint8 k_imu;
    uint8 count = 0;
    int IMU_ack;
    int tmp[N_IMU_MAX];

    // Initialize Memory Structure 
    memset(&g_imu, 0, sizeof(struct st_imu));
    memset(&IMU_connected, 0, sizeof(IMU_connected));

    // First ping to be sure to wakeup IMUs
    for (k_imu=0; k_imu < N_IMU_MAX; k_imu++) 
    {	
	    ChipSelector(k_imu);
	    CyDelay(10);
	    ReadControlRegister(MPU9250_WHO_AM_I);
    }
    
    // Identify IMU connected
    ChipSelector(0);
    CyDelay(10);
    IMU_ack = ReadControlRegister(MPU9250_WHO_AM_I);
    IMU_ack = 0;
    N_IMU_Connected = 0;    
    for (k_imu = 0; k_imu < N_IMU_MAX; k_imu++) 
    {      
    	ChipSelector(k_imu);
        CyDelay(10);
        IMU_ack = ReadControlRegister(MPU9250_WHO_AM_I);
        if (IMU_ack == 0x71) 
        {
            N_IMU_Connected++;
            IMU_ack = 0;
            tmp[k_imu] = 1;
        }
        else
            tmp[k_imu] = 0;
    }

    for(k_imu = 0; k_imu < N_IMU_MAX; k_imu++)
    {
        if(tmp[k_imu] == 1)
        {
            IMU_connected[count] = k_imu;
            count++;
        }
    }
            
    // Standard Initialization of the IMU
    CyDelay(10);
    for (k_imu=0; k_imu < N_IMU_MAX; k_imu++) 
    {	
	    ChipSelector(k_imu);
	    CyDelay(10);
	    InitIMU();
	    CyDelay(10); 
        InitIMUMagCal();      // Initialize IMU to Read MagCal Parameters
	    CyDelay(10); 
    }
    CyDelay(50);
    
    // Reading of MagCal Parameters
    for (k_imu = 0; k_imu < N_IMU_MAX; k_imu++){
        ChipSelector(k_imu);
        ReadMagCal(k_imu); 
    }
    CyDelay(50);
    
    // Set Standard Read for the IMU
    memset(&single_imu_size, 0, sizeof(single_imu_size));
    imus_data_size = 1; //header    
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++)
    {
        single_imu_size[IMU_connected[k_imu]] = 1 + 6*c_mem.IMU_conf[IMU_connected[k_imu]][0] + 6*c_mem.IMU_conf[IMU_connected[k_imu]][1] + 6*c_mem.IMU_conf[IMU_connected[k_imu]][2]+ 16*c_mem.IMU_conf[IMU_connected[k_imu]][3] + 2*c_mem.IMU_conf[IMU_connected[k_imu]][4]+ 1;
        imus_data_size = imus_data_size + single_imu_size[IMU_connected[k_imu]];
    }
    imus_data_size = imus_data_size + 1;    //checksum*/
}

/*******************************************************************************
* Function Name: IMU Read
*********************************************************************************/	
void ReadIMU(int n)
{
    if (c_mem.IMU_conf[n][0]) ReadAcc(n);
    if (c_mem.IMU_conf[n][1]) ReadGyro(n);
    if (c_mem.IMU_conf[n][2]) ReadMag(n);
    if (c_mem.IMU_conf[n][3]) ReadQuat(n);
    if (c_mem.IMU_conf[n][4]) ReadTemp(n);
}

/*******************************************************************************
* Function Name: Read Acc's Data of IMU n
*********************************************************************************/
void ReadAcc(int n)
{
	uint8 low=0, high=0;
	
	int row = n;
	
	//read X
    low=ReadControlRegister(MPU9250_ACCEL_XOUT_L);
    SPI_delay();
    high=ReadControlRegister(MPU9250_ACCEL_XOUT_H);
    SPI_delay();
    
	Accel[row][0] = high; 
	Accel[row][1] = low; 
	low=0, high=0;
			
	//read Y
    low=ReadControlRegister(MPU9250_ACCEL_YOUT_L);
    SPI_delay();
	high=ReadControlRegister(MPU9250_ACCEL_YOUT_H);
    SPI_delay();

	Accel[row][2] = high; 
	Accel[row][3] = low; 
	low=0, high=0;
		
	//read Z
    low=ReadControlRegister(MPU9250_ACCEL_ZOUT_L);
    SPI_delay();
    high=ReadControlRegister(MPU9250_ACCEL_ZOUT_H);
    SPI_delay();
    
	Accel[row][4] = high; 
	Accel[row][5] = low;
	low=0, high=0;
}

/*******************************************************************************
* Function Name: Read Gyro's Data of IMU n
*********************************************************************************/
void ReadGyro(int n){
	uint8 low=0, high=0;
 
	int row = n;
	
	//read X
	low=ReadControlRegister(MPU9250_GYRO_XOUT_L);
    SPI_delay();
	high=ReadControlRegister(MPU9250_GYRO_XOUT_H);
    SPI_delay();
    
	Gyro[row][0] = high; 
	Gyro[row][1] = low;
	low=0, high=0;
    
	//read Y
	low=ReadControlRegister(MPU9250_GYRO_YOUT_L);
    SPI_delay();
	high=ReadControlRegister(MPU9250_GYRO_YOUT_H);
    SPI_delay();
    
	Gyro[row][2] = high; 
	Gyro[row][3] = low;
	low=0, high=0;

	//read Z
    low=ReadControlRegister(MPU9250_GYRO_ZOUT_L);
    SPI_delay();
	high=ReadControlRegister(MPU9250_GYRO_ZOUT_H);
    SPI_delay();
    
	Gyro[row][4] = high; 
	Gyro[row][5] = low;        

	low=0, high=0;
}

/*******************************************************************************
* Function Name: Read Compas' Data of IMU n
*********************************************************************************/
void ReadMag(int n){
	
	uint8 low, high;
	int row = n;	
	//read X
	low = ReadControlRegister(MPU9250_EXT_SENS_DATA_00);
    SPI_delay();
	high = ReadControlRegister(MPU9250_EXT_SENS_DATA_01);		
    SPI_delay();
    
	Mag[row][0] = high; 
	Mag[row][1] = low;
	low=0, high=0;
    
	//read Y
	low = ReadControlRegister(MPU9250_EXT_SENS_DATA_02);
    SPI_delay();
	high = ReadControlRegister(MPU9250_EXT_SENS_DATA_03);		
    SPI_delay();
    
	Mag[row][2] = high; 
	Mag[row][3] = low;
	low=0, high=0;
    
	//read Z
	low = ReadControlRegister(MPU9250_EXT_SENS_DATA_04);
    SPI_delay();
	high = ReadControlRegister(MPU9250_EXT_SENS_DATA_05);
    SPI_delay();
		
	Mag[row][4] = high; 
	Mag[row][5] = low;
	low=0, high=0;
}

/********************************************************************************
* Function Name: ReadMagCal
*********************************************************************************/
void ReadMagCal(int n){
	
    uint8 read = 0; 
	int row = n;	
	//read X
	read = ReadControlRegister(MPU9250_EXT_SENS_DATA_00);
    MagCal[row][0] = read; 
    read = 0;
    
	//read Y
	read = ReadControlRegister(MPU9250_EXT_SENS_DATA_01);			
	MagCal[row][1] = read; 
	read = 0;
    
	//read Z
	read = ReadControlRegister(MPU9250_EXT_SENS_DATA_02);			
	MagCal[row][2] = read; 
	read = 0;
    
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
    float time = 0;
        
    if (N_IMU_Connected == 1) {
        // Compute quaternion only if 1 IMU is connected
        
        if (!c_mem.IMU_conf[n][0]) ReadAcc(n);
        if (!c_mem.IMU_conf[n][1]) ReadGyro(n);
        
        // assume cycle time is the same of last function_scheduler execution,
        // without adding another counter
        time = (float)((uint32)timer_value0 - (uint32)timer_value) / 1000000.0;
        
        // how N sees P; N is 'Earth' equivalent, P is for sensor
        // e.g. N = imu1, P = imu0, how imu1 sees imu0

        var_h = Accel[n][0];
        aP[0] = (int16)(var_h << 8 | Accel[n][1]); 
        var_h = Accel[n][2];
        aP[1] = (int16)(var_h << 8 | Accel[n][3]); 
        var_h = Accel[n][4];
        aP[2] = (int16)(var_h << 8 | Accel[n][5]);
        
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
        qL[0] = Quat[0];
        qL[1] = Quat[1];
        qL[2] = Quat[2];
        qL[3] = Quat[3];   
        
        // Napla computation
        Napla[0] = (-qL[2]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) + qL[1]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])))*TICK2ACC*BETA;
        Napla[1] = (qL[3]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) + qL[0]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])) -2*qL[1]*((2*(0.5 - qL[1]*qL[1] - qL[2]*qL[2]) - aP[2])))*TICK2ACC*BETA;
        Napla[2] = (-qL[0]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) +qL[3]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])) -2*qL[2]*((2*(0.5 - qL[1]*qL[1] - qL[2]*qL[2]) - aP[2])))*TICK2ACC*BETA;
        Napla[3] = (qL[1]*((2*(qL[1]*qL[3]-qL[0]*qL[2]) - aP[0])) + qL[2]*((2*(qL[0]*qL[1] + qL[2]*qL[3]) - aP[1])))*TICK2ACC*BETA;
        
        qd0 = 0.5*(-(qL[1]*g[1] + qL[2]*g[2] + qL[3]*g[3])) - Napla[0];
        qd1 = 0.5*(qL[0]*g[1] + (qL[2]*g[3] - qL[3]*g[2])) - Napla[1];
        qd2 = 0.5*(qL[0]*g[2] + (qL[3]*g[1] - qL[1]*g[3])) - Napla[2];
        qd3 = 0.5*(qL[0]*g[3] + (qL[1]*g[2] - qL[2]*g[1])) - Napla[3];
                
        qL[0] = qL[0] + qd0*time;
        qL[1] = qL[1] + qd1*time;
        qL[2] = qL[2] + qd2*time;
        qL[3] = qL[3] + qd3*time;
        v4_normalize(qL);
        
        // Quaternion update      
        Quat[0] = qL[0];
        Quat[1] = qL[1];
        Quat[2] = qL[2];
        Quat[3] = qL[3];
    }
    else {
        return;
    }
}

/********************************************************************************
* Function Name: ReadAllIMUs
*********************************************************************************/
void ReadAllIMUs(){
    static uint8 k_imu = 0;
    uint16 tmp = 0, j = 0;

    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++){ 
        // Read k_imu IMU
        ChipSelector(IMU_connected[k_imu]);
        ReadIMU(IMU_connected[k_imu]);
        
        for (j = 0; j < 3; j++) {
            tmp = Accel[IMU_connected[k_imu]][2*j];
            g_imuNew[k_imu].accel_value[j] = (int16)(tmp<<8 | Accel[IMU_connected[k_imu]][2*j + 1]);
        }
        
        for (j = 0; j < 3; j++) {
            tmp = Gyro[IMU_connected[k_imu]][2*j];
            g_imuNew[k_imu].gyro_value[j] = (int16)(tmp<<8 | Gyro[IMU_connected[k_imu]][2*j + 1]);
        }
                
        for (j = 0; j < 3; j++) {
            tmp = Mag[IMU_connected[k_imu]][2*j];
            g_imuNew[k_imu].mag_value[j] = (int16)(tmp<<8 | Mag[IMU_connected[k_imu]][2*j + 1]);
        }  
        
        // Save quaternion only for first IMU
        for (j = 0; k_imu == 0 && j < 4; j++) {
            g_imuNew[k_imu].quat_value[j] = (float)Quat[j];
        }
        
        tmp = Temp[IMU_connected[k_imu]][0];
        g_imuNew[k_imu].temp_value = (int16)(tmp<<8 | Temp[IMU_connected[k_imu]][1]);
    }
}

/*******************************************************************************
* Function Name: Read Temperature Data of IMU n
*********************************************************************************/
void ReadTemp(int n)
{
	uint8 low=0, high=0;	
	int row = n;
	
	//read X
    low=ReadControlRegister(MPU9250_TEMP_OUT_L);
    SPI_delay();
	high=ReadControlRegister(MPU9250_TEMP_OUT_H);
    SPI_delay();

	Temp[row][0] = high; 
	Temp[row][1] = low; 
	low=0, high=0;
}

/********************************** *********************************************
* Function Name: Write Control Register
*********************************************************************************/
void WriteControlRegister(uint8 address, uint8 dta){
	
	SPI_IMU_ClearRxBuffer();
	SPI_IMU_ClearTxBuffer();
	SPI_IMU_ClearFIFO();
	SPI_IMU_WriteByte(MPU9250_WCR | address);
	while(!( SPI_IMU_ReadStatus() & SPI_IMU_STS_TX_FIFO_EMPTY));		
	SPI_IMU_WriteByte(dta);
	while(!( SPI_IMU_ReadStatus() & SPI_IMU_STS_TX_FIFO_EMPTY));
}

/*******************************************************************************
* Function Name: Read Control Register
*********************************************************************************/
uint8 ReadControlRegister(uint8 address){
	uint8 controlreg = 0;
	
	SPI_IMU_WriteByte(MPU9250_RCR | address);
    while(!( SPI_IMU_ReadStatus() & SPI_IMU_STS_TX_FIFO_EMPTY));
    SPI_IMU_WriteByte(0x00);
	while(!( SPI_IMU_ReadStatus() & SPI_IMU_STS_SPI_DONE));
	controlreg = SPI_IMU_ReadByte();        //real data
	return controlreg;
}

/*******************************************************************************
* Function Name: SPI delay
*********************************************************************************/
void SPI_delay(){
    switch( c_mem.SPI_read_delay ) {
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


// --------------------------------------------------------
// DS1302_write
void DS1302_write(uint8 address, uint8 data_wr) {

  RTC_Clock_Write(0);  
  RTC_CS_Write(1);
  CyDelayUs(5); // [GS] 
  DS1302_writeByte(address);
  DS1302_writeByte(data_wr);
  
  RTC_CS_Write(0);
  CyDelayUs(5); // [GS] 
}

void DS1302_writeByte(uint8 data_wr) {
  
  shiftOut_RTC(data_wr);

}

uint8 DS1302_read(uint8 address) {

  uint8 readValue;
  RTC_Clock_Write(0); 
  RTC_CS_Write(1);
  CyDelayUs(5); // [GS]
  DS1302_writeByte(address);
  readValue = DS1302_readByte();
  RTC_CS_Write(0);
  CyDelayUs(5); // [GS]
  return readValue;
  
}

uint8 DS1302_readByte() {

  uint8 value = 0;
  uint8 currentBit = 0;
  int i=0;
  for (i = 0; i < 8; ++i)
  {
    currentBit = RTC_IN_Read();
    value |= (currentBit << i);
    RTC_Clock_Write(1);
    CyDelayUs(1);
    RTC_Clock_Write(0);
  }
  return value;
}

void shiftOut_RTC(uint8 val)
{
    uint8 i;
    for (i = 0; i < 8; i++)  {
        RTC_OUT_Write(!!(val & (1 << i)));
        RTC_Clock_Write(1);
        RTC_Clock_Write(0);
    }
}

void encoder_read(int32* currValue) {

    uint32 currBit = 0;
    uint32 data_encoder[N_ENCODERS];
    uint8 i=0, j=0;
    // Start of SSI comunication
    ENC_CS_Write(1);
    ENC_SCLK_Write(1);
    ENC_CS_Write(0);
    CyDelayUs(1);           // Wait t_CLK_FE = 500 ns
    ENC_SCLK_Write(0);
    CyDelayUs(1);           // Wait T_CLK/2 = 500 ns

    for (i=0; i<N_ENCODERS; i++) {
        data_encoder[i] = 0;
        for (j=0; j<N_BITS_ENCODER; j++) {
            // Read bit between high and low edge of clock
            ENC_SCLK_Write(1);
            CyDelayUs(1);
            currBit = (uint32)ENC_MISO_Read();
            data_encoder[i] |= (currBit << (17-j));
            ENC_SCLK_Write(0);
            CyDelayUs(1);
        }
        
        if (i < N_ENCODERS-1) {
            //Skip bit between two consecutive encoders
            ENC_SCLK_Write(1);
            CyDelayUs(1);
            ENC_SCLK_Write(0);
            CyDelayUs(1);
        }
    }

    ENC_SCLK_Write(1);
    CyDelayUs(1);
    ENC_CS_Write(1); 
         
    // S = SIGN BIT
    // X = ENCODER VALUE BIT
    // 0 = 0 BIT
    // C = CONTROL BIT
    
    // data_encoder =  00000000000000[14] XXXXXXXXXXXX[12] CCCCCC[6]
    
    for (i=0; i<N_ENCODERS; i++) {
        if (check_enc_data(&data_encoder[i])) {
            currValue[i] = (data_encoder[i] & 0x3FFC0) >> 6;   // 00000000000000[20] XXXXXXXXXXXX[12]    
        }
        else {
            currValue[i] = 0;
        }
        data_encoder_raw[i] = currValue[i];
    }
}
