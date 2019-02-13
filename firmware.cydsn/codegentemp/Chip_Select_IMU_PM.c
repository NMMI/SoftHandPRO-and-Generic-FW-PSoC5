/*******************************************************************************
* File Name: Chip_Select_IMU_PM.c
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

#include "Chip_Select_IMU.h"

/* Check for removal by optimization */
#if !defined(Chip_Select_IMU_Sync_ctrl_reg__REMOVED)

static Chip_Select_IMU_BACKUP_STRUCT  Chip_Select_IMU_backup = {0u};

    
/*******************************************************************************
* Function Name: Chip_Select_IMU_SaveConfig
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
void Chip_Select_IMU_SaveConfig(void) 
{
    Chip_Select_IMU_backup.controlState = Chip_Select_IMU_Control;
}


/*******************************************************************************
* Function Name: Chip_Select_IMU_RestoreConfig
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
void Chip_Select_IMU_RestoreConfig(void) 
{
     Chip_Select_IMU_Control = Chip_Select_IMU_backup.controlState;
}


/*******************************************************************************
* Function Name: Chip_Select_IMU_Sleep
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
void Chip_Select_IMU_Sleep(void) 
{
    Chip_Select_IMU_SaveConfig();
}


/*******************************************************************************
* Function Name: Chip_Select_IMU_Wakeup
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
void Chip_Select_IMU_Wakeup(void)  
{
    Chip_Select_IMU_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
