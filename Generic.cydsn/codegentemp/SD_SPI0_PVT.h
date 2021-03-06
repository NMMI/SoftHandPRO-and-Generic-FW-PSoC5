/*******************************************************************************
* File Name: .h
* Version 2.40
*
* Description:
*  This private header file contains internal definitions for the SPIM
*  component. Do not use these definitions directly in your application.
*
* Note:
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SPIM_PVT_SD_SPI0_H)
#define CY_SPIM_PVT_SD_SPI0_H

#include "SD_SPI0.h"


/**********************************
*   Functions with external linkage
**********************************/


/**********************************
*   Variables with external linkage
**********************************/

extern volatile uint8 SD_SPI0_swStatusTx;
extern volatile uint8 SD_SPI0_swStatusRx;

#if(SD_SPI0_TX_SOFTWARE_BUF_ENABLED)
    extern volatile uint8 SD_SPI0_txBuffer[SD_SPI0_TX_BUFFER_SIZE];
    extern volatile uint8 SD_SPI0_txBufferRead;
    extern volatile uint8 SD_SPI0_txBufferWrite;
    extern volatile uint8 SD_SPI0_txBufferFull;
#endif /* (SD_SPI0_TX_SOFTWARE_BUF_ENABLED) */

#if(SD_SPI0_RX_SOFTWARE_BUF_ENABLED)
    extern volatile uint8 SD_SPI0_rxBuffer[SD_SPI0_RX_BUFFER_SIZE];
    extern volatile uint8 SD_SPI0_rxBufferRead;
    extern volatile uint8 SD_SPI0_rxBufferWrite;
    extern volatile uint8 SD_SPI0_rxBufferFull;
#endif /* (SD_SPI0_RX_SOFTWARE_BUF_ENABLED) */

#endif /* CY_SPIM_PVT_SD_SPI0_H */


/* [] END OF FILE */
