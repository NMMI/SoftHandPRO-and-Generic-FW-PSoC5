/*******************************************************************************
* File Name: Chip_Select_ENCODER_LINE_PM.c
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

#include "Chip_Select_ENCODER_LINE.h"

/* Check for removal by optimization */
#if !defined(Chip_Select_ENCODER_LINE_Sync_ctrl_reg__REMOVED)

static Chip_Select_ENCODER_LINE_BACKUP_STRUCT  Chip_Select_ENCODER_LINE_backup = {0u};

    
/*******************************************************************************
* Function Name: Chip_Select_ENCODER_LINE_SaveConfig
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
void Chip_Select_ENCODER_LINE_SaveConfig(void) 
{
    Chip_Select_ENCODER_LINE_backup.controlState = Chip_Select_ENCODER_LINE_Control;
}


/*******************************************************************************
* Function Name: Chip_Select_ENCODER_LINE_RestoreConfig
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
void Chip_Select_ENCODER_LINE_RestoreConfig(void) 
{
     Chip_Select_ENCODER_LINE_Control = Chip_Select_ENCODER_LINE_backup.controlState;
}


/*******************************************************************************
* Function Name: Chip_Select_ENCODER_LINE_Sleep
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
void Chip_Select_ENCODER_LINE_Sleep(void) 
{
    Chip_Select_ENCODER_LINE_SaveConfig();
}


/*******************************************************************************
* Function Name: Chip_Select_ENCODER_LINE_Wakeup
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
void Chip_Select_ENCODER_LINE_Wakeup(void)  
{
    Chip_Select_ENCODER_LINE_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
