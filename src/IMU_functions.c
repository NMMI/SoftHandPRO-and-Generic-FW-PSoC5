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
                 OneShot_WriteRoutine(EXT_SENS_ADDR,LIS2MDL_CFG_REG_A,0x00); //10Hz, continuous mode
               CyDelay(20);
                OneShot_WriteRoutine(EXT_SENS_ADDR,LIS2MDL_CFG_REG_B,0x02); //Offset canc
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
    Chip_Select_IMU_Write(7);
    Chip_Select_IMU_Write(n);
}

/********************************************************************************
* Function Name: InitIMUGeneral                                                 *
*********************************************************************************/
void InitIMUgeneral()
{
    uint8 k_imu ;
    uint8 count = 0;
    int tmp[N_IMU_MAX];
    uint8 WhoAmI_MPU = 0;
    uint8 WhoAmI_LSM = 0;
    // Initialize Memory Structure 
    memset(&g_imu, 0, sizeof(struct st_imu));
    memset(&IMU_connected, 0, sizeof(IMU_connected));
        
    
    // First ping to be sure to wakeup IMUs
    for (k_imu=0; k_imu < N_IMU_MAX; k_imu++) 
    {	
	    ChipSelectorIMU(k_imu);
	    CyDelay(10);
        //WHO_AM_I = g_imu[k_imu].dev_type ? LSM6DSRX_WHO_AM_I : MPU9250_WHO_AM_I;
        ReadControlRegisterIMU(LSM6DSRX_WHO_AM_I);
        ReadControlRegisterIMU(MPU9250_WHO_AM_I);
    }
    
    
    N_IMU_Connected = 0;    
    for (k_imu = 0; k_imu < NUM_DEV_IMU; k_imu++) 
    {      
    	ChipSelectorIMU(k_imu);
        CyDelay(10);
    	WhoAmI_MPU = ReadControlRegisterIMU(MPU9250_WHO_AM_I);
        CyDelay(10);
        WhoAmI_LSM = ReadControlRegisterIMU(LSM6DSRX_WHO_AM_I);
        CyDelay(10);
        if (WhoAmI_MPU == MPU9250_WHO_AM_I_VALUE && WhoAmI_LSM != LSM6DSRX_WHO_AM_I_VALUE){
            g_imu[k_imu].dev_type = MPU9250;
            g_imuNew[k_imu].dev_type = MPU9250;
            N_IMU_Connected++;
            
            tmp[k_imu] = 1;
        }
        else if (WhoAmI_LSM == LSM6DSRX_WHO_AM_I_VALUE && WhoAmI_MPU != MPU9250_WHO_AM_I_VALUE){
            g_imu[k_imu].dev_type = LSM6DSRX;
            g_imuNew[k_imu].dev_type = LSM6DSRX;
            N_IMU_Connected++;
            tmp[k_imu] = 1;
        }
        else{
            tmp[k_imu] = 0;
            g_imu[k_imu].dev_type = 3;
        }
    }
        
            
    for(k_imu = 0; k_imu < N_IMU_MAX; k_imu++)
    {
        if(tmp[k_imu] == 1)
        {
            IMU_connected[count] = k_imu;
            count++;
        }
    }            

    // Initialize IMU to Read MagCal Parameters
    for (k_imu = 0; k_imu < N_IMU_MAX; k_imu++) 
    {	
	    ChipSelectorIMU(k_imu);
	    CyDelay(10);
	    InitIMUMagCal(k_imu);
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
        
               
    // Standard Initialization of the IMU
    CyDelay(10);
    for (k_imu=0; k_imu < N_IMU_MAX; k_imu++) 
    {	
	    ChipSelectorIMU(k_imu);
	    CyDelay(10);
	    InitIMU(k_imu);
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
{   static uint8 i;
	static uint8 AccStartAddress;
    AccStartAddress = g_imu[n].dev_type ? LSM6DSRX_OUTX_L_A : MPU9250_ACCEL_XOUT_H;
    for (i = 0; i < 6; i++){
        //	Accel[row][0] = high; 
	    //  Accel[row][1] = low; 
        //  Order of LSM6DSRX register are inverted to be compatible with the ones of  MPU9250
        Accel[n][ i + g_imu[n].dev_type*(1 - 2 *( i % 2 ))] = ReadControlRegisterIMU(AccStartAddress + i);  
    }
    
}

/*******************************************************************************
* Function Name: Read Gyro's Data of IMU n
*********************************************************************************/
void ReadGyro(int n){
    static uint8 i;
	static uint8 GyroStartAddress;
    GyroStartAddress = g_imu[n].dev_type ? LSM6DSRX_OUTX_L_G : MPU9250_GYRO_XOUT_H;
    for (i = 0; i < 6; i++){
        //	Accel[row][0] = high; 
	    //  Accel[row][1] = low; 
        //  Order of LSM6DSRX register are inverted to be compatible with the ones of  MPU9250
        Gyro[n][ i + g_imu[n].dev_type*(1 - 2 *( i % 2 ))] = ReadControlRegisterIMU(GyroStartAddress + i);  
    }
    /*
    
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
    */
}

/*******************************************************************************
* Function Name: Read Compas' Data of IMU n
*********************************************************************************/
void ReadMag(int n){	
    uint8 low, high;
    uint8 i;
	int row = n;
        uint8 XLDA;
    uint8 SENS_HUB_ENDOP;
    uint8 Data;
	switch (g_imu[n].dev_type){
        case MPU9250:
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
    break;
    case LSM6DSRX:
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
            CyDelayUs(100);  
            ReadControlRegisterIMU(LSM6DSRX_WHO_AM_I); 
               
            for (i = 0; i < 6; i++){
                
                Mag[n][i + g_imu[n].dev_type*(1 - 2 *( i % 2 ))]= ReadControlRegisterIMU(LSM6DSRX_SENSOR_HUB_1 + i);  
            }
           
            WriteControlRegisterIMU(LSM6DSRX_FUNC_CFG_ACCESS, 0x00);   
            CyDelayUs(100);
    
    break;}
    
}

/********************************************************************************
* Function Name: ReadMagCal
*********************************************************************************/
void ReadMagCal(int n){ 
    uint8 read = 0; 
	int row = n;	
	switch (g_imu[n].dev_type){
        case MPU9250:
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
            break;

        case LSM6DSRX:
            MagCal[row][0] = 1; 
            MagCal[row][1] = 1; 
            MagCal[row][2] = 1; 
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
{   // MPU9250:
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
}

/********************************** *********************************************
* Function Name: Write Control Register
*********************************************************************************/
void WriteControlRegisterIMU(uint8 address, uint8 dta){
	
	SPI_IMU_ClearRxBuffer();
	SPI_IMU_ClearTxBuffer();
	SPI_IMU_ClearFIFO();
	SPI_IMU_WriteByte(WRITEBIT | address);
	while(!( SPI_IMU_ReadStatus() & SPI_IMU_STS_TX_FIFO_EMPTY));		
	SPI_IMU_WriteByte(dta);
	while(!( SPI_IMU_ReadStatus() & SPI_IMU_STS_TX_FIFO_EMPTY));
}

/*******************************************************************************
* Function Name: Read Control Register
*********************************************************************************/
uint8 ReadControlRegisterIMU(uint8 address){
	uint8 controlreg = 0;
	
	SPI_IMU_WriteByte(READBIT | address);
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

