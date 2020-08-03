/*******************************************************************************
* File Name: BLINK_05HZ_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "BLINK_05HZ.h"

static BLINK_05HZ_backupStruct BLINK_05HZ_backup;


/*******************************************************************************
* Function Name: BLINK_05HZ_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  BLINK_05HZ_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void BLINK_05HZ_SaveConfig(void) 
{

    #if(!BLINK_05HZ_UsingFixedFunction)
        #if(!BLINK_05HZ_PWMModeIsCenterAligned)
            BLINK_05HZ_backup.PWMPeriod = BLINK_05HZ_ReadPeriod();
        #endif /* (!BLINK_05HZ_PWMModeIsCenterAligned) */
        BLINK_05HZ_backup.PWMUdb = BLINK_05HZ_ReadCounter();
        #if (BLINK_05HZ_UseStatus)
            BLINK_05HZ_backup.InterruptMaskValue = BLINK_05HZ_STATUS_MASK;
        #endif /* (BLINK_05HZ_UseStatus) */

        #if(BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_256_CLOCKS || \
            BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_2_4_CLOCKS)
            BLINK_05HZ_backup.PWMdeadBandValue = BLINK_05HZ_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(BLINK_05HZ_KillModeMinTime)
             BLINK_05HZ_backup.PWMKillCounterPeriod = BLINK_05HZ_ReadKillTime();
        #endif /* (BLINK_05HZ_KillModeMinTime) */

        #if(BLINK_05HZ_UseControl)
            BLINK_05HZ_backup.PWMControlRegister = BLINK_05HZ_ReadControlRegister();
        #endif /* (BLINK_05HZ_UseControl) */
    #endif  /* (!BLINK_05HZ_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BLINK_05HZ_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  BLINK_05HZ_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void BLINK_05HZ_RestoreConfig(void) 
{
        #if(!BLINK_05HZ_UsingFixedFunction)
            #if(!BLINK_05HZ_PWMModeIsCenterAligned)
                BLINK_05HZ_WritePeriod(BLINK_05HZ_backup.PWMPeriod);
            #endif /* (!BLINK_05HZ_PWMModeIsCenterAligned) */

            BLINK_05HZ_WriteCounter(BLINK_05HZ_backup.PWMUdb);

            #if (BLINK_05HZ_UseStatus)
                BLINK_05HZ_STATUS_MASK = BLINK_05HZ_backup.InterruptMaskValue;
            #endif /* (BLINK_05HZ_UseStatus) */

            #if(BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_256_CLOCKS || \
                BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_2_4_CLOCKS)
                BLINK_05HZ_WriteDeadTime(BLINK_05HZ_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(BLINK_05HZ_KillModeMinTime)
                BLINK_05HZ_WriteKillTime(BLINK_05HZ_backup.PWMKillCounterPeriod);
            #endif /* (BLINK_05HZ_KillModeMinTime) */

            #if(BLINK_05HZ_UseControl)
                BLINK_05HZ_WriteControlRegister(BLINK_05HZ_backup.PWMControlRegister);
            #endif /* (BLINK_05HZ_UseControl) */
        #endif  /* (!BLINK_05HZ_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: BLINK_05HZ_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  BLINK_05HZ_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void BLINK_05HZ_Sleep(void) 
{
    #if(BLINK_05HZ_UseControl)
        if(BLINK_05HZ_CTRL_ENABLE == (BLINK_05HZ_CONTROL & BLINK_05HZ_CTRL_ENABLE))
        {
            /*Component is enabled */
            BLINK_05HZ_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            BLINK_05HZ_backup.PWMEnableState = 0u;
        }
    #endif /* (BLINK_05HZ_UseControl) */

    /* Stop component */
    BLINK_05HZ_Stop();

    /* Save registers configuration */
    BLINK_05HZ_SaveConfig();
}


/*******************************************************************************
* Function Name: BLINK_05HZ_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  BLINK_05HZ_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void BLINK_05HZ_Wakeup(void) 
{
     /* Restore registers values */
    BLINK_05HZ_RestoreConfig();

    if(BLINK_05HZ_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        BLINK_05HZ_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
