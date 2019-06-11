/*******************************************************************************
* File Name: MY_TIMER_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "MY_TIMER.h"

static MY_TIMER_backupStruct MY_TIMER_backup;


/*******************************************************************************
* Function Name: MY_TIMER_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  MY_TIMER_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void MY_TIMER_SaveConfig(void) 
{
    #if (!MY_TIMER_UsingFixedFunction)
        MY_TIMER_backup.TimerUdb = MY_TIMER_ReadCounter();
        MY_TIMER_backup.InterruptMaskValue = MY_TIMER_STATUS_MASK;
        #if (MY_TIMER_UsingHWCaptureCounter)
            MY_TIMER_backup.TimerCaptureCounter = MY_TIMER_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!MY_TIMER_UDB_CONTROL_REG_REMOVED)
            MY_TIMER_backup.TimerControlRegister = MY_TIMER_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: MY_TIMER_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  MY_TIMER_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void MY_TIMER_RestoreConfig(void) 
{   
    #if (!MY_TIMER_UsingFixedFunction)

        MY_TIMER_WriteCounter(MY_TIMER_backup.TimerUdb);
        MY_TIMER_STATUS_MASK =MY_TIMER_backup.InterruptMaskValue;
        #if (MY_TIMER_UsingHWCaptureCounter)
            MY_TIMER_SetCaptureCount(MY_TIMER_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!MY_TIMER_UDB_CONTROL_REG_REMOVED)
            MY_TIMER_WriteControlRegister(MY_TIMER_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: MY_TIMER_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  MY_TIMER_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void MY_TIMER_Sleep(void) 
{
    #if(!MY_TIMER_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(MY_TIMER_CTRL_ENABLE == (MY_TIMER_CONTROL & MY_TIMER_CTRL_ENABLE))
        {
            /* Timer is enabled */
            MY_TIMER_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            MY_TIMER_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    MY_TIMER_Stop();
    MY_TIMER_SaveConfig();
}


/*******************************************************************************
* Function Name: MY_TIMER_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  MY_TIMER_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void MY_TIMER_Wakeup(void) 
{
    MY_TIMER_RestoreConfig();
    #if(!MY_TIMER_UDB_CONTROL_REG_REMOVED)
        if(MY_TIMER_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                MY_TIMER_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
