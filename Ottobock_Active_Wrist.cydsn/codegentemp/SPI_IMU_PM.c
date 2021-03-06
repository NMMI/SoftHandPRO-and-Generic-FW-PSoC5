/*******************************************************************************
* File Name: SPI_IMU_PM.c
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

#include "SPI_IMU_PVT.h"

static SPI_IMU_BACKUP_STRUCT SPI_IMU_backup =
{
    SPI_IMU_DISABLED,
    SPI_IMU_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: SPI_IMU_SaveConfig
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
void SPI_IMU_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_IMU_RestoreConfig
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
void SPI_IMU_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_IMU_Sleep
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
*  SPI_IMU_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_IMU_Sleep(void) 
{
    /* Save components enable state */
    SPI_IMU_backup.enableState = ((uint8) SPI_IMU_IS_ENABLED);

    SPI_IMU_Stop();
}


/*******************************************************************************
* Function Name: SPI_IMU_Wakeup
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
*  SPI_IMU_backup - used when non-retention registers are restored.
*  SPI_IMU_txBufferWrite - modified every function call - resets to
*  zero.
*  SPI_IMU_txBufferRead - modified every function call - resets to
*  zero.
*  SPI_IMU_rxBufferWrite - modified every function call - resets to
*  zero.
*  SPI_IMU_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_IMU_Wakeup(void) 
{
    #if(SPI_IMU_RX_SOFTWARE_BUF_ENABLED)
        SPI_IMU_rxBufferFull  = 0u;
        SPI_IMU_rxBufferRead  = 0u;
        SPI_IMU_rxBufferWrite = 0u;
    #endif /* (SPI_IMU_RX_SOFTWARE_BUF_ENABLED) */

    #if(SPI_IMU_TX_SOFTWARE_BUF_ENABLED)
        SPI_IMU_txBufferFull  = 0u;
        SPI_IMU_txBufferRead  = 0u;
        SPI_IMU_txBufferWrite = 0u;
    #endif /* (SPI_IMU_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    SPI_IMU_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SPI_IMU_backup.enableState)
    {
        SPI_IMU_Enable();
    }
}


/* [] END OF FILE */
