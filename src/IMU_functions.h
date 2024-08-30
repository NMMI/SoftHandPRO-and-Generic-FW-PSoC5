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
* \file         IMU_functions.h
*
* \brief        Definition of IMU module functions.
* \date         March 20th, 2020
* \author       _Centro "E.Piaggio"_
* \copyright    (C) 2012-2016 qbrobotics. All rights reserved.
* \copyright    (C) 2017-2020 Centro "E.Piaggio". All rights reserved.
*/

#include <project.h>
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include <SPI_IMU.h>

#define MPU9250 0

#define LSM6DSRX 1

#define READBIT 			0x80  //Flag of READ  (control register)
#define WRITEBIT 			0x00  //Flag of WRITE (control register)

          
//LSM6DSRX Register definition
#define EXT_SENS_ADDR 0b00111100

#define LSM6DSRX_INT1_CTRL              0x0D
#define LSM6DSRX_INT2_CTRL              0x0E
#define LSM6DSRX_WHO_AM_I               0x0F
#define LSM6DSRX_WHO_AM_I_VALUE         0x6B    // valore decimale = 107
#define LSM6DSRX_CTRL1_XL               0x10
#define LSM6DSRX_CTRL2_G                0x11      
#define LSM6DSRX_CTRL3_C                0x12
#define LSM6DSRX_CTRL4_C                0x13
#define LSM6DSRX_CTRL5_C                0x14
#define LSM6DSRX_CTRL6_C                0x15
#define LSM6DSRX_CTRL7_G                0x16
#define LSM6DSRX_CTRL8_XL               0x17
#define LSM6DSRX_CTRL9_XL               0x18
#define LSM6DSRX_CTRL10_C               0x19


#define LSM6DSRX_STATUS_REG             0x1E
#define LSM6DSRX_OUT_TEMP_L             0x20
#define LSM6DSRX_OUT_TEMP_H             0x21
#define LSM6DSRX_OUTX_L_G               0x22
#define LSM6DSRX_OUTX_H_G               0x23
#define LSM6DSRX_OUTY_L_G               0x24
#define LSM6DSRX_OUTY_H_G               0x25
#define LSM6DSRX_OUTZ_L_G               0x26
#define LSM6DSRX_OUTZ_H_G               0x27
#define LSM6DSRX_OUTX_L_A               0x28
#define LSM6DSRX_OUTX_H_A               0x29
#define LSM6DSRX_OUTY_L_A               0x2A
#define LSM6DSRX_OUTY_H_A               0x2B
#define LSM6DSRX_OUTZ_L_A               0x2C
#define LSM6DSRX_OUTZ_H_A               0x2D
#define LSM6DSRX_STATUS_MASTER_MAINPAGE 0x39
#define LSM6DSRX_TIMESTAMP0             0x40
#define LSM6DSRX_TIMESTAMP1             0x41
#define LSM6DSRX_TIMESTAMP2             0x42
#define LSM6DSRX_TIMESTAMP3             0x43

#define LSM6DSRX_MD1_CFG                0x5E
#define LSM6DSRX_MD2_CFG                0x5F

#define LSM6DSRX_INTERNAL_FREQ_FINE     0x63

#define LSM6DSRX_X_OFS_USR              0x73
#define LSM6DSRX_Y_OFS_USR              0x74
#define LSM6DSRX_Z_OFS_USR              0x75

#define LSM6DSRX_FUNC_CFG_ACCESS        0x01
#define LSM6DSRX_SENSOR_HUB_1           0x02
#define LSM6DSRX_SENSOR_HUB_2           0x03
#define LSM6DSRX_SENSOR_HUB_3           0x04
#define LSM6DSRX_SENSOR_HUB_4           0x05
#define LSM6DSRX_SENSOR_HUB_5           0x06
#define LSM6DSRX_SENSOR_HUB_6           0x07
#define LSM6DSRX_MASTER_CONFIG          0x14
#define LSM6DSRX_SLV0_ADD               0x15
#define LSM6DSRX_SLV0_SUBADD            0x16
#define LSM6DSRX_SLV0_CONFIG            0x17
#define LSM6DSRX_DATAWRITE_SLV0         0x21
#define LSM6DSRX_STATUS_MASTER          0x22



//LIS2MDL Register definition

#define LIS2MDL_WHO_AM_I               0x4F
#define LIS2MDL_WHO_AM_I_VALUE         0x40 // valore decimale = 64
#define LIS2MDL_CFG_REG_A              0x60
#define LIS2MDL_CFG_REG_B              0x61
#define LIS2MDL_CFG_REG_C              0x62
#define LIS2MDL_INT_CRTL_REG           0x63
#define LIS2MDL_INT_SOURCE_REG         0x64
#define LIS2MDL_INT_THS_L_REG          0x65
#define LIS2MDL_INT_THS_H_REG          0x66
#define LIS2MDL_STATUS_REG             0x67
#define LIS2MDL_OUTX_L_REG             0x68
#define LIS2MDL_OUTX_H_REG             0x69
#define LIS2MDL_OUTY_L_REG             0x6A
#define LIS2MDL_OUTY_H_REG             0x6B
#define LIS2MDL_OUTZ_L_REG             0x6C
#define LIS2MDL_OUTZ_H_REG             0x6D
#define LIS2MDL_TEMP_OUT_L_REG         0x6E
#define LIS2MDL_TEMP_OUT_H_REG         0x6F


//MPU9250 Register definition

#define MPU9250_CONFIG              0x1A //CONFIG
#define MPU9250_GYRO_CONFIG 		0x1B //CONFIG
#define MPU9250_ACCEL_CONFIG		0x1C
#define MPU9250_ACCEL_CONFIG2 		0x1D
#define MPU9250_ACCEL_XOUT_H 		0x3B //ACCEL
#define MPU9250_ACCEL_XOUT_L		0x3C
#define MPU9250_ACCEL_YOUT_H		0x3D
#define MPU9250_ACCEL_YOUT_L		0x3E
#define MPU9250_ACCEL_ZOUT_H		0x3F
#define MPU9250_ACCEL_ZOUT_L		0x40
#define MPU9250_TEMP_OUT_H			0x41 //TEMP
#define MPU9250_TEMP_OUT_L			0x42
#define MPU9250_GYRO_XOUT_H			0x43 //GYRO
#define MPU9250_GYRO_XOUT_L			0x44
#define MPU9250_GYRO_YOUT_H			0x45
#define MPU9250_GYRO_YOUT_L			0x46
#define MPU9250_GYRO_ZOUT_H			0x47
#define MPU9250_GYRO_ZOUT_L			0x48
#define MPU9250_USER_CTRL			0x6A //USER CONTROL 
#define MPU9250_PWR_MGMT_1 			0x6B //POWER MANAGEMENT
#define MPU9250_WHO_AM_I			0x75 //WHO I AM
#define MPU9250_WHO_AM_I_VALUE      0x071
#define MPU9250_FIFO_EN				0x23 //FIFO
#define MPU9250_I2C_MST_CTRL		0x24 
#define MPU9250_I2C_SLV0_ADDR		0x25 //SLV0 
#define MPU9250_I2C_SLV0_REG		0x26
#define MPU9250_I2C_SLV0_CTRL		0x27
#define MPU9250_I2C_SLV1_ADDR		0x28 //SLV1
#define MPU9250_I2C_SLV1_REG		0x29
#define MPU9250_I2C_SLV1_CTRL		0x2A
#define MPU9250_EXT_SENS_DATA_00 	0x49 //EXT DATA
#define MPU9250_EXT_SENS_DATA_01 	0x4A
#define MPU9250_EXT_SENS_DATA_02 	0x4B
#define MPU9250_EXT_SENS_DATA_03 	0x4C
#define MPU9250_EXT_SENS_DATA_04 	0x4D
#define MPU9250_EXT_SENS_DATA_05 	0x4E
#define MPU9250_EXT_SENS_DATA_06 	0x4F
#define MPU9250_EXT_SENS_DATA_07 	0x50
#define MPU9250_I2C_SLV0_D0		 	0x63
#define MPU9250_I2C_SLV1_D0		 	0x64
#define MPU9250_I2C_MST_DELAY_CTRL  0x67



#define AK8936_ADDRESS				0x0C //COMPASS AK8936
#define AK8936_WIA					0x00
#define AK8936_INFO					0x01
#define AK8936_ST1					0x02
#define AK8936_XOUT_L				0x03 
#define AK8936_XOUT_H				0x04
#define AK8936_YOUT_L				0x05
#define AK8936_YOUT_H				0x06
#define AK8936_ZOUT_L				0x07
#define AK8936_ZOUT_H				0x08
#define AK8936_ST2					0x09
#define AK8936_CNTL					0x0A
#define AK8963_CNTL2				0x0B
#define AK8936_ASTC					0x0C
#define AK8936_I2CDIS				0x0F

#define ACC_SF_2G                   0x00     //+-2g
#define ACC_SF_4G                   0x08
#define ACC_SF_8G                   0x10
#define ACC_SF_16G                  0x18

#define GYRO_SF_250                 0x00    // 250 °/s
#define GYRO_SF_500                 0x80
#define GYRO_SF_2000                0x18

#define G_TO_MS2                    9.79
#define DEG_TO_RAD                  (3.14159265359 / 180.0)

//low pass accelerometers filter
#define LP_ACC_FREQ_460             0x00
#define LP_ACC_FREQ_184             0x01
#define LP_ACC_FREQ_92              0x02
#define LP_ACC_FREQ_41              0x03
#define LP_ACC_FREQ_20              0x04
#define LP_ACC_FREQ_10              0x05
#define LP_ACC_FREQ_5               0x06
#define NO_ACC_FIL                  0x08

// macro for quaternion estimation
#define TICK2GYRO                   0.000133158     // rad->deg conversion is included
#define TICK2ACC                    0.000061037
#define BETA                        100000.0
#define GYRO_THR                    0.2618          // 15 deg

//function declaration
void ImusReset(); // Imus Reset 
void InitIMU(uint8 n); //IMU setup
void InitIMUMagCal(uint8 n_imu);
void InitIMUgeneral();
void ReadAcc(int n); //reads acc of IMU n
void ReadGyro(int n);//reads gyro of IMU n
void ReadMag(int n);//reads compas of IMU n
void ReadMagCal(int n);//reads compas cal of IMU n
void ReadQuat(int n);//reads quaternion of IMU n
void ReadTemp(int n);//reads temperature of IMU n
void ReadIMU(int n); //reads all of data from IMU n
void ReadAllIMUs(); //reads all data from all connected IMUs
uint8 ReadControlRegisterIMU(uint8 address);
void WriteControlRegisterIMU(uint8 address, uint8 dta);
void ChipSelectorIMU(int n);
void SPI_delay();
void OneShot_WriteRoutine (uint8 address, uint8 subaddress, uint8 data);
uint8 OneShot_ReadRoutine(uint8 address, uint8 subaddress);
void Continuous_ReadRoutine(uint8 address, uint8 subaddress, uint8 slave_config);
void UpdateIMUDefine();
/* [] END OF FILE */
