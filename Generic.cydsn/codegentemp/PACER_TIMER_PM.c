/*******************************************************************************
* File Name: PACER_TIMER_PM.c
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

#include "PACER_TIMER.h"

static PACER_TIMER_backupStruct PACER_TIMER_backup;


/*******************************************************************************
* Function Name: PACER_TIMER_SaveConfig
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
*  PACER_TIMER_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PACER_TIMER_SaveConfig(void) 
{
    #if (!PACER_TIMER_UsingFixedFunction)
        PACER_TIMER_backup.TimerUdb = PACER_TIMER_ReadCounter();
        PACER_TIMER_backup.InterruptMaskValue = PACER_TIMER_STATUS_MASK;
        #if (PACER_TIMER_UsingHWCaptureCounter)
            PACER_TIMER_backup.TimerCaptureCounter = PACER_TIMER_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!PACER_TIMER_UDB_CONTROL_REG_REMOVED)
            PACER_TIMER_backup.TimerControlRegister = PACER_TIMER_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: PACER_TIMER_RestoreConfig
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
*  PACER_TIMER_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PACER_TIMER_RestoreConfig(void) 
{   
    #if (!PACER_TIMER_UsingFixedFunction)

        PACER_TIMER_WriteCounter(PACER_TIMER_backup.TimerUdb);
        PACER_TIMER_STATUS_MASK =PACER_TIMER_backup.InterruptMaskValue;
        #if (PACER_TIMER_UsingHWCaptureCounter)
            PACER_TIMER_SetCaptureCount(PACER_TIMER_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!PACER_TIMER_UDB_CONTROL_REG_REMOVED)
            PACER_TIMER_WriteControlRegister(PACER_TIMER_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: PACER_TIMER_Sleep
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
*  PACER_TIMER_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void PACER_TIMER_Sleep(void) 
{
    #if(!PACER_TIMER_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(PACER_TIMER_CTRL_ENABLE == (PACER_TIMER_CONTROL & PACER_TIMER_CTRL_ENABLE))
        {
            /* Timer is enabled */
            PACER_TIMER_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            PACER_TIMER_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    PACER_TIMER_Stop();
    PACER_TIMER_SaveConfig();
}


/*******************************************************************************
* Function Name: PACER_TIMER_Wakeup
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
*  PACER_TIMER_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PACER_TIMER_Wakeup(void) 
{
    PACER_TIMER_RestoreConfig();
    #if(!PACER_TIMER_UDB_CONTROL_REG_REMOVED)
        if(PACER_TIMER_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                PACER_TIMER_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
