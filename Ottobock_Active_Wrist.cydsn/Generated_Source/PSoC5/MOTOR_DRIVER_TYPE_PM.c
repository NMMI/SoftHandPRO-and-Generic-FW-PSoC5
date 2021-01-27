/*******************************************************************************
* File Name: MOTOR_DRIVER_TYPE_PM.c
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

#include "MOTOR_DRIVER_TYPE.h"

/* Check for removal by optimization */
#if !defined(MOTOR_DRIVER_TYPE_Sync_ctrl_reg__REMOVED)

static MOTOR_DRIVER_TYPE_BACKUP_STRUCT  MOTOR_DRIVER_TYPE_backup = {0u};

    
/*******************************************************************************
* Function Name: MOTOR_DRIVER_TYPE_SaveConfig
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
void MOTOR_DRIVER_TYPE_SaveConfig(void) 
{
    MOTOR_DRIVER_TYPE_backup.controlState = MOTOR_DRIVER_TYPE_Control;
}


/*******************************************************************************
* Function Name: MOTOR_DRIVER_TYPE_RestoreConfig
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
void MOTOR_DRIVER_TYPE_RestoreConfig(void) 
{
     MOTOR_DRIVER_TYPE_Control = MOTOR_DRIVER_TYPE_backup.controlState;
}


/*******************************************************************************
* Function Name: MOTOR_DRIVER_TYPE_Sleep
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
void MOTOR_DRIVER_TYPE_Sleep(void) 
{
    MOTOR_DRIVER_TYPE_SaveConfig();
}


/*******************************************************************************
* Function Name: MOTOR_DRIVER_TYPE_Wakeup
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
void MOTOR_DRIVER_TYPE_Wakeup(void)  
{
    MOTOR_DRIVER_TYPE_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
