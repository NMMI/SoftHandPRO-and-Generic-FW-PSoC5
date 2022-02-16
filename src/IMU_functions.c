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
* Function Name: IMU Initialization
*********************************************************************************/
void InitIMU(){	
    
	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x10); 
	CyDelay(10);	
	WriteControlRegisterIMU(MPU9250_USER_CTRL, 0x20);  //I2C master enable - disable I2C (prima 0x30)
	CyDelay(10);
    WriteControlRegisterIMU(MPU9250_CONFIG, 0x06); //Gyro & Temp Low Pass Filter 0x01 = 184Hz, 0x02 = 92Hz, 0x04 = 20Hz, 0x05 = 10Hz
    CyDelay(10);	
	WriteControlRegisterIMU(MPU9250_GYRO_CONFIG , GYRO_SF_2000); //Gyro full scale select 0x00=250°/s 0x80=500°/s 0x18=2000°/s 
	CyDelay(10);
    WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG, ACC_SF_2G); // Acc full scale select 0x00 = 2g 0x08 = 4g 0x10 = 8g 0x18 = 16g
    CyDelay(10);
   // WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG2, LP_ACC_FREQ_10);   
    WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG2, NO_ACC_FIL);   
    CyDelay(10);
	//mag register
	WriteControlRegisterIMU(MPU9250_I2C_MST_CTRL, 0x0D); //set slave I2C speed
	CyDelay(10);
	//SLV0 (use to write)
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x0C); //set compass address
	CyDelay(10);			
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, AK8936_CNTL); //compass mode register
	CyDelay(10);	
	// Istruction used to read Compass
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_D0, 0x16); //0x12 continuous mode1  0x16 continuous mode2
	CyDelay(10);
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x81); //enable data from register + 1 bit to write
	CyDelay(10);
	//SLV0 (use to read)
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x8C); // RCR  | AK8963_address (0x0C) 
	CyDelay(10);
	// Istruction used to read Compass
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, 0x03); // 0x03:start from Xout Low in case of calibration 0x10:start from ASAX
	CyDelay(10);
	// Istruction used to read Compass
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x8D); //How many bits read  SEMPRE DISPARI 0x8D era quella che funzionava
	CyDelay(10);
	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x00); 
	CyDelay(20);
}	

/*******************************************************************************
* Function Name: IMU Mag Cal Initialization
*********************************************************************************/
void InitIMUMagCal(){	
    
	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x10); 
	CyDelay(10);	
	WriteControlRegisterIMU(MPU9250_USER_CTRL, 0x20);  //I2C master enable - disable I2C (prima 0x30)
	CyDelay(10);
    WriteControlRegisterIMU(MPU9250_CONFIG, 0x06); //Gyro & Temp Low Pass Filter 0x01 = 184Hz, 0x02 = 92Hz, 0x04 = 20Hz, 0x05 = 10Hz
    CyDelay(10);	
	WriteControlRegisterIMU(MPU9250_GYRO_CONFIG , GYRO_SF_2000); //Gyro full scale select 0x00=250°/s 0x80=500°/s 0x18=2000°/s 
	CyDelay(10);
    WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG, ACC_SF_2G); // Acc full scale select 0x00 = 2g 0x08 = 4g 0x10 = 8g 0x18 = 16g
    CyDelay(10);
    WriteControlRegisterIMU(MPU9250_ACCEL_CONFIG2, 0x05);
    CyDelay(10);
	//mag register
	WriteControlRegisterIMU(MPU9250_I2C_MST_CTRL, 0x0D); //set slave I2C speed
	CyDelay(10);
	//SLV0 (use to write)
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x0C); //set compass address
	CyDelay(10);			
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, AK8936_CNTL); //compass mode register
	CyDelay(10);	
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_D0, 0x1F); //0x1F ROM access
	CyDelay(10);
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x81); //enable data from register + 1 bit to write
	CyDelay(10);
	//SLV0 (use to read)
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_ADDR, 0x8C); // RCR  | AK8963_address (0x0C) 
	CyDelay(10);
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_REG, 0x10); // 0x10:start from ASAX
	CyDelay(10);
	WriteControlRegisterIMU(MPU9250_I2C_SLV0_CTRL, 0x83);
	CyDelay(10);
	WriteControlRegisterIMU(MPU9250_PWR_MGMT_1, 0x00); 
	CyDelay(20);
}

/********************************************************************************
* Function Name: ChipSelectorIMU                                                   *
*********************************************************************************/
void ChipSelectorIMU(int n)
{
    Chip_Select_IMU_Write(7);
    
    Chip_Select_IMU_Write(n);
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

    // Initialize IMU to Read MagCal Parameters
    for (k_imu=0; k_imu < N_IMU_MAX; k_imu++) 
    {	
	    ChipSelectorIMU(k_imu);
	    CyDelay(10);
	    InitIMUMagCal();
	    CyDelay(10); 
    }
    
    // Reading of MagCal Parameters
    CyDelay(50);
    for (k_imu = 0; k_imu < N_IMU_MAX; k_imu++){
        ChipSelectorIMU(k_imu);
        CyDelay(10);
        ReadMagCal(k_imu);  
        CyDelay(10);
    }

    ChipSelectorIMU(0); // Read again first IMU MagCal Parameters
    CyDelay(10);
    ReadMagCal(0);  
    CyDelay(10);
        
    // First ping to be sure to wakeup IMUs
    for (k_imu=0; k_imu < N_IMU_MAX; k_imu++) 
    {	
	    ChipSelectorIMU(k_imu);
	    CyDelay(10);
	    ReadControlRegisterIMU(MPU9250_WHO_AM_I);
    }
    
    // Identify IMU connected
    ChipSelectorIMU(0);
    CyDelay(10);
    IMU_ack = ReadControlRegisterIMU(MPU9250_WHO_AM_I);
    IMU_ack = 0;
    N_IMU_Connected = 0;    
    for (k_imu = 0; k_imu < NUM_DEV_IMU; k_imu++) 
    {      
    	ChipSelectorIMU(k_imu);
        CyDelay(10);
        IMU_ack = ReadControlRegisterIMU(MPU9250_WHO_AM_I);
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
	    ChipSelectorIMU(k_imu);
	    CyDelay(10);
	    InitIMU();
	    CyDelay(10); 
    }
    CyDelay(50);
    
    // Set Standard Read for the IMU
    memset(&single_imu_size, 0, sizeof(single_imu_size));
    imus_data_size = 1; //header    
    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++)
    {
        single_imu_size[IMU_connected[k_imu]] = 1 + 6*c_mem.imu.IMU_conf[IMU_connected[k_imu]][0] + 6*c_mem.imu.IMU_conf[IMU_connected[k_imu]][1] + 6*c_mem.imu.IMU_conf[IMU_connected[k_imu]][2]+ 16*c_mem.imu.IMU_conf[IMU_connected[k_imu]][3] + 2*c_mem.imu.IMU_conf[IMU_connected[k_imu]][4]+ 1;
        imus_data_size = imus_data_size + single_imu_size[IMU_connected[k_imu]];
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
{
	uint8 low=0, high=0;
	
	int row = n;
	
	//read X
    low=ReadControlRegisterIMU(MPU9250_ACCEL_XOUT_L);
    SPI_delay();
    high=ReadControlRegisterIMU(MPU9250_ACCEL_XOUT_H);
    SPI_delay();
    
	Accel[row][0] = high; 
	Accel[row][1] = low; 
	low=0, high=0;
			
	//read Y
    low=ReadControlRegisterIMU(MPU9250_ACCEL_YOUT_L);
    SPI_delay();
	high=ReadControlRegisterIMU(MPU9250_ACCEL_YOUT_H);
    SPI_delay();

	Accel[row][2] = high; 
	Accel[row][3] = low; 
	low=0, high=0;
		
	//read Z
    low=ReadControlRegisterIMU(MPU9250_ACCEL_ZOUT_L);
    SPI_delay();
    high=ReadControlRegisterIMU(MPU9250_ACCEL_ZOUT_H);
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
	low=ReadControlRegisterIMU(MPU9250_GYRO_XOUT_L);
    SPI_delay();
	high=ReadControlRegisterIMU(MPU9250_GYRO_XOUT_H);
    SPI_delay();
    
	Gyro[row][0] = high; 
	Gyro[row][1] = low;
	low=0, high=0;
    
	//read Y
	low=ReadControlRegisterIMU(MPU9250_GYRO_YOUT_L);
    SPI_delay();
	high=ReadControlRegisterIMU(MPU9250_GYRO_YOUT_H);
    SPI_delay();
    
	Gyro[row][2] = high; 
	Gyro[row][3] = low;
	low=0, high=0;

	//read Z
    low=ReadControlRegisterIMU(MPU9250_GYRO_ZOUT_L);
    SPI_delay();
	high=ReadControlRegisterIMU(MPU9250_GYRO_ZOUT_H);
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
	low = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_00);
    SPI_delay();
	high = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_01);		
    SPI_delay();
    
	Mag[row][0] = high; 
	Mag[row][1] = low;
	low=0, high=0;
    
	//read Y
	low = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_02);
    SPI_delay();
	high = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_03);		
    SPI_delay();
    
	Mag[row][2] = high; 
	Mag[row][3] = low;
	low=0, high=0;
    
	//read Z
	low = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_04);
    SPI_delay();
	high = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_05);
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
	read = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_00);
    MagCal[row][0] = read; 
    read = 0;
    
	//read Y
	read = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_01);			
	MagCal[row][1] = read; 
	read = 0;
    
	//read Z
	read = ReadControlRegisterIMU(MPU9250_EXT_SENS_DATA_02);			
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
}

/********************************************************************************
* Function Name: ReadAllIMUs
*********************************************************************************/
void ReadAllIMUs(){
    static uint8 k_imu = 0;
    uint16 tmp = 0, j = 0;

    for (k_imu = 0; k_imu < N_IMU_Connected; k_imu++){ 
        // Read k_imu IMU
        ChipSelectorIMU(IMU_connected[k_imu]);
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

        for (j = 0; j < 4; j++) {
            g_imuNew[k_imu].quat_value[j] = (float)(Quat[IMU_connected[k_imu]][j]);
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
    low=ReadControlRegisterIMU(MPU9250_TEMP_OUT_L);
    SPI_delay();
	high=ReadControlRegisterIMU(MPU9250_TEMP_OUT_H);
    SPI_delay();

	Temp[row][0] = high; 
	Temp[row][1] = low; 
	low=0, high=0;
}

/********************************** *********************************************
* Function Name: Write Control Register
*********************************************************************************/
void WriteControlRegisterIMU(uint8 address, uint8 dta){
	
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
uint8 ReadControlRegisterIMU(uint8 address){
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