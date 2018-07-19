/*******************************************************************************
* File Name: UART_RS485_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_RS485.h"


/***************************************
* Local data allocation
***************************************/

static UART_RS485_BACKUP_STRUCT  UART_RS485_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: UART_RS485_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the UART_RS485_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_RS485_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART_RS485_SaveConfig(void)
{
    #if(UART_RS485_CONTROL_REG_REMOVED == 0u)
        UART_RS485_backup.cr = UART_RS485_CONTROL_REG;
    #endif /* End UART_RS485_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: UART_RS485_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_RS485_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling UART_RS485_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void UART_RS485_RestoreConfig(void)
{
    #if(UART_RS485_CONTROL_REG_REMOVED == 0u)
        UART_RS485_CONTROL_REG = UART_RS485_backup.cr;
    #endif /* End UART_RS485_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: UART_RS485_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The UART_RS485_Sleep() API saves the current component state. Then it
*  calls the UART_RS485_Stop() function and calls 
*  UART_RS485_SaveConfig() to save the hardware configuration.
*  Call the UART_RS485_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_RS485_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART_RS485_Sleep(void)
{
    #if(UART_RS485_RX_ENABLED || UART_RS485_HD_ENABLED)
        if((UART_RS485_RXSTATUS_ACTL_REG  & UART_RS485_INT_ENABLE) != 0u)
        {
            UART_RS485_backup.enableState = 1u;
        }
        else
        {
            UART_RS485_backup.enableState = 0u;
        }
    #else
        if((UART_RS485_TXSTATUS_ACTL_REG  & UART_RS485_INT_ENABLE) !=0u)
        {
            UART_RS485_backup.enableState = 1u;
        }
        else
        {
            UART_RS485_backup.enableState = 0u;
        }
    #endif /* End UART_RS485_RX_ENABLED || UART_RS485_HD_ENABLED*/

    UART_RS485_Stop();
    UART_RS485_SaveConfig();
}


/*******************************************************************************
* Function Name: UART_RS485_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  UART_RS485_Sleep() was called. The UART_RS485_Wakeup() function
*  calls the UART_RS485_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  UART_RS485_Sleep() function was called, the UART_RS485_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UART_RS485_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UART_RS485_Wakeup(void)
{
    UART_RS485_RestoreConfig();
    #if( (UART_RS485_RX_ENABLED) || (UART_RS485_HD_ENABLED) )
        UART_RS485_ClearRxBuffer();
    #endif /* End (UART_RS485_RX_ENABLED) || (UART_RS485_HD_ENABLED) */
    #if(UART_RS485_TX_ENABLED || UART_RS485_HD_ENABLED)
        UART_RS485_ClearTxBuffer();
    #endif /* End UART_RS485_TX_ENABLED || UART_RS485_HD_ENABLED */

    if(UART_RS485_backup.enableState != 0u)
    {
        UART_RS485_Enable();
    }
}


/* [] END OF FILE */
