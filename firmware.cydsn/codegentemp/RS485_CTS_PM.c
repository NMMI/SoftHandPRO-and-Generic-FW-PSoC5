/*******************************************************************************
* File Name: RS485_CTS_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "RS485_CTS.h"

/* Check for removal by optimization */
#if !defined(RS485_CTS_Sync_ctrl_reg__REMOVED)

static RS485_CTS_BACKUP_STRUCT  RS485_CTS_backup = {0u};

    
/*******************************************************************************
* Function Name: RS485_CTS_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void RS485_CTS_SaveConfig(void) 
{
    RS485_CTS_backup.controlState = RS485_CTS_Control;
}


/*******************************************************************************
* Function Name: RS485_CTS_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void RS485_CTS_RestoreConfig(void) 
{
     RS485_CTS_Control = RS485_CTS_backup.controlState;
}


/*******************************************************************************
* Function Name: RS485_CTS_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void RS485_CTS_Sleep(void) 
{
    RS485_CTS_SaveConfig();
}


/*******************************************************************************
* Function Name: RS485_CTS_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void RS485_CTS_Wakeup(void)  
{
    RS485_CTS_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
