/*******************************************************************************
* File Name: CYCLES_TIMER_PM.c
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

#include "CYCLES_TIMER.h"

static CYCLES_TIMER_backupStruct CYCLES_TIMER_backup;


/*******************************************************************************
* Function Name: CYCLES_TIMER_SaveConfig
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
*  CYCLES_TIMER_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void CYCLES_TIMER_SaveConfig(void) 
{
    #if (!CYCLES_TIMER_UsingFixedFunction)
        CYCLES_TIMER_backup.TimerUdb = CYCLES_TIMER_ReadCounter();
        CYCLES_TIMER_backup.InterruptMaskValue = CYCLES_TIMER_STATUS_MASK;
        #if (CYCLES_TIMER_UsingHWCaptureCounter)
            CYCLES_TIMER_backup.TimerCaptureCounter = CYCLES_TIMER_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!CYCLES_TIMER_UDB_CONTROL_REG_REMOVED)
            CYCLES_TIMER_backup.TimerControlRegister = CYCLES_TIMER_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: CYCLES_TIMER_RestoreConfig
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
*  CYCLES_TIMER_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void CYCLES_TIMER_RestoreConfig(void) 
{   
    #if (!CYCLES_TIMER_UsingFixedFunction)

        CYCLES_TIMER_WriteCounter(CYCLES_TIMER_backup.TimerUdb);
        CYCLES_TIMER_STATUS_MASK =CYCLES_TIMER_backup.InterruptMaskValue;
        #if (CYCLES_TIMER_UsingHWCaptureCounter)
            CYCLES_TIMER_SetCaptureCount(CYCLES_TIMER_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!CYCLES_TIMER_UDB_CONTROL_REG_REMOVED)
            CYCLES_TIMER_WriteControlRegister(CYCLES_TIMER_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: CYCLES_TIMER_Sleep
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
*  CYCLES_TIMER_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void CYCLES_TIMER_Sleep(void) 
{
    #if(!CYCLES_TIMER_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(CYCLES_TIMER_CTRL_ENABLE == (CYCLES_TIMER_CONTROL & CYCLES_TIMER_CTRL_ENABLE))
        {
            /* Timer is enabled */
            CYCLES_TIMER_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            CYCLES_TIMER_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    CYCLES_TIMER_Stop();
    CYCLES_TIMER_SaveConfig();
}


/*******************************************************************************
* Function Name: CYCLES_TIMER_Wakeup
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
*  CYCLES_TIMER_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void CYCLES_TIMER_Wakeup(void) 
{
    CYCLES_TIMER_RestoreConfig();
    #if(!CYCLES_TIMER_UDB_CONTROL_REG_REMOVED)
        if(CYCLES_TIMER_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                CYCLES_TIMER_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
