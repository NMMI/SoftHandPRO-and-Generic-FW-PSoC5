/*******************************************************************************
* File Name: SPI_IMU_1_PM.c
* Version 2.50
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SPI_IMU_1_PVT.h"

static SPI_IMU_1_BACKUP_STRUCT SPI_IMU_1_backup =
{
    SPI_IMU_1_DISABLED,
    SPI_IMU_1_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: SPI_IMU_1_SaveConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SPI_IMU_1_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_IMU_1_RestoreConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SPI_IMU_1_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_IMU_1_Sleep
********************************************************************************
*
* Summary:
*  Prepare SPIM Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SPI_IMU_1_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_IMU_1_Sleep(void) 
{
    /* Save components enable state */
    SPI_IMU_1_backup.enableState = ((uint8) SPI_IMU_1_IS_ENABLED);

    SPI_IMU_1_Stop();
}


/*******************************************************************************
* Function Name: SPI_IMU_1_Wakeup
********************************************************************************
*
* Summary:
*  Prepare SPIM Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SPI_IMU_1_backup - used when non-retention registers are restored.
*  SPI_IMU_1_txBufferWrite - modified every function call - resets to
*  zero.
*  SPI_IMU_1_txBufferRead - modified every function call - resets to
*  zero.
*  SPI_IMU_1_rxBufferWrite - modified every function call - resets to
*  zero.
*  SPI_IMU_1_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_IMU_1_Wakeup(void) 
{
    #if(SPI_IMU_1_RX_SOFTWARE_BUF_ENABLED)
        SPI_IMU_1_rxBufferFull  = 0u;
        SPI_IMU_1_rxBufferRead  = 0u;
        SPI_IMU_1_rxBufferWrite = 0u;
    #endif /* (SPI_IMU_1_RX_SOFTWARE_BUF_ENABLED) */

    #if(SPI_IMU_1_TX_SOFTWARE_BUF_ENABLED)
        SPI_IMU_1_txBufferFull  = 0u;
        SPI_IMU_1_txBufferRead  = 0u;
        SPI_IMU_1_txBufferWrite = 0u;
    #endif /* (SPI_IMU_1_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    SPI_IMU_1_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SPI_IMU_1_backup.enableState)
    {
        SPI_IMU_1_Enable();
    }
}


/* [] END OF FILE */
