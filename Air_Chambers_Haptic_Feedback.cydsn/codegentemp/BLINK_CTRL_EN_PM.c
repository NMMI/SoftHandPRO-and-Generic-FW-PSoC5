/*******************************************************************************
* File Name: BLINK_CTRL_EN_PM.c
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

#include "BLINK_CTRL_EN.h"

/* Check for removal by optimization */
#if !defined(BLINK_CTRL_EN_Sync_ctrl_reg__REMOVED)

static BLINK_CTRL_EN_BACKUP_STRUCT  BLINK_CTRL_EN_backup = {0u};

    
/*******************************************************************************
* Function Name: BLINK_CTRL_EN_SaveConfig
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
void BLINK_CTRL_EN_SaveConfig(void) 
{
    BLINK_CTRL_EN_backup.controlState = BLINK_CTRL_EN_Control;
}


/*******************************************************************************
* Function Name: BLINK_CTRL_EN_RestoreConfig
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
void BLINK_CTRL_EN_RestoreConfig(void) 
{
     BLINK_CTRL_EN_Control = BLINK_CTRL_EN_backup.controlState;
}


/*******************************************************************************
* Function Name: BLINK_CTRL_EN_Sleep
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
void BLINK_CTRL_EN_Sleep(void) 
{
    BLINK_CTRL_EN_SaveConfig();
}


/*******************************************************************************
* Function Name: BLINK_CTRL_EN_Wakeup
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
void BLINK_CTRL_EN_Wakeup(void)  
{
    BLINK_CTRL_EN_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
