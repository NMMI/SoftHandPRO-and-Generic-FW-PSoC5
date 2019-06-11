/*******************************************************************************
* File Name: SD_SPI0_PM.c
* Version 2.40
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SD_SPI0_PVT.h"

static SD_SPI0_BACKUP_STRUCT SD_SPI0_backup =
{
    SD_SPI0_DISABLED,
    SD_SPI0_BITCTR_INIT,
    #if(CY_UDB_V0)
        SD_SPI0_TX_INIT_INTERRUPTS_MASK,
        SD_SPI0_RX_INIT_INTERRUPTS_MASK
    #endif /* CY_UDB_V0 */
};


/*******************************************************************************
* Function Name: SD_SPI0_SaveConfig
********************************************************************************
*
* Summary:
*  Saves SPIM configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SD_SPI0_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPI0_SaveConfig(void) 
{
    /* Store Status Mask registers */
    #if(CY_UDB_V0)
       SD_SPI0_backup.cntrPeriod      = SD_SPI0_COUNTER_PERIOD_REG;
       SD_SPI0_backup.saveSrTxIntMask = SD_SPI0_TX_STATUS_MASK_REG;
       SD_SPI0_backup.saveSrRxIntMask = SD_SPI0_RX_STATUS_MASK_REG;
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: SD_SPI0_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores SPIM configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SD_SPI0_backup - used when non-retention registers are restored.
*
* Side Effects:
*  If this API is called without first calling SaveConfig then in the following
*  registers will be default values from Customizer:
*  SD_SPI0_STATUS_MASK_REG and SD_SPI0_COUNTER_PERIOD_REG.
*
*******************************************************************************/
void SD_SPI0_RestoreConfig(void) 
{
    /* Restore the data, saved by SaveConfig() function */
    #if(CY_UDB_V0)
        SD_SPI0_COUNTER_PERIOD_REG = SD_SPI0_backup.cntrPeriod;
        SD_SPI0_TX_STATUS_MASK_REG = ((uint8) SD_SPI0_backup.saveSrTxIntMask);
        SD_SPI0_RX_STATUS_MASK_REG = ((uint8) SD_SPI0_backup.saveSrRxIntMask);
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: SD_SPI0_Sleep
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
*  SD_SPI0_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPI0_Sleep(void) 
{
    /* Save components enable state */
    SD_SPI0_backup.enableState = ((uint8) SD_SPI0_IS_ENABLED);

    SD_SPI0_Stop();
    SD_SPI0_SaveConfig();
}


/*******************************************************************************
* Function Name: SD_SPI0_Wakeup
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
*  SD_SPI0_backup - used when non-retention registers are restored.
*  SD_SPI0_txBufferWrite - modified every function call - resets to
*  zero.
*  SD_SPI0_txBufferRead - modified every function call - resets to
*  zero.
*  SD_SPI0_rxBufferWrite - modified every function call - resets to
*  zero.
*  SD_SPI0_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPI0_Wakeup(void) 
{
    SD_SPI0_RestoreConfig();

    #if(SD_SPI0_RX_SOFTWARE_BUF_ENABLED)
        SD_SPI0_rxBufferFull  = 0u;
        SD_SPI0_rxBufferRead  = 0u;
        SD_SPI0_rxBufferWrite = 0u;
    #endif /* (SD_SPI0_RX_SOFTWARE_BUF_ENABLED) */

    #if(SD_SPI0_TX_SOFTWARE_BUF_ENABLED)
        SD_SPI0_txBufferFull  = 0u;
        SD_SPI0_txBufferRead  = 0u;
        SD_SPI0_txBufferWrite = 0u;
    #endif /* (SD_SPI0_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    SD_SPI0_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SD_SPI0_backup.enableState)
    {
        SD_SPI0_Enable();
    }
}


/* [] END OF FILE */
