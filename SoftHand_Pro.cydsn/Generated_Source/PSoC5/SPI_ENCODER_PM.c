/*******************************************************************************
* File Name: SPI_ENCODER_PM.c
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

#include "SPI_ENCODER_PVT.h"

static SPI_ENCODER_BACKUP_STRUCT SPI_ENCODER_backup =
{
    SPI_ENCODER_DISABLED,
    SPI_ENCODER_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: SPI_ENCODER_SaveConfig
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
void SPI_ENCODER_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_ENCODER_RestoreConfig
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
void SPI_ENCODER_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPI_ENCODER_Sleep
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
*  SPI_ENCODER_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_Sleep(void) 
{
    /* Save components enable state */
    SPI_ENCODER_backup.enableState = ((uint8) SPI_ENCODER_IS_ENABLED);

    SPI_ENCODER_Stop();
}


/*******************************************************************************
* Function Name: SPI_ENCODER_Wakeup
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
*  SPI_ENCODER_backup - used when non-retention registers are restored.
*  SPI_ENCODER_txBufferWrite - modified every function call - resets to
*  zero.
*  SPI_ENCODER_txBufferRead - modified every function call - resets to
*  zero.
*  SPI_ENCODER_rxBufferWrite - modified every function call - resets to
*  zero.
*  SPI_ENCODER_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_Wakeup(void) 
{
    #if(SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)
        SPI_ENCODER_rxBufferFull  = 0u;
        SPI_ENCODER_rxBufferRead  = 0u;
        SPI_ENCODER_rxBufferWrite = 0u;
    #endif /* (SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED) */

    #if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED)
        SPI_ENCODER_txBufferFull  = 0u;
        SPI_ENCODER_txBufferRead  = 0u;
        SPI_ENCODER_txBufferWrite = 0u;
    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    SPI_ENCODER_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SPI_ENCODER_backup.enableState)
    {
        SPI_ENCODER_Enable();
    }
}


/* [] END OF FILE */
