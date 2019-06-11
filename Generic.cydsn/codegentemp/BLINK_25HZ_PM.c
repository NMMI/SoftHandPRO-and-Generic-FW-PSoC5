/*******************************************************************************
* File Name: BLINK_25HZ_PM.c
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

#include "BLINK_25HZ.h"

static BLINK_25HZ_backupStruct BLINK_25HZ_backup;


/*******************************************************************************
* Function Name: BLINK_25HZ_SaveConfig
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
*  BLINK_25HZ_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void BLINK_25HZ_SaveConfig(void) 
{

    #if(!BLINK_25HZ_UsingFixedFunction)
        #if(!BLINK_25HZ_PWMModeIsCenterAligned)
            BLINK_25HZ_backup.PWMPeriod = BLINK_25HZ_ReadPeriod();
        #endif /* (!BLINK_25HZ_PWMModeIsCenterAligned) */
        BLINK_25HZ_backup.PWMUdb = BLINK_25HZ_ReadCounter();
        #if (BLINK_25HZ_UseStatus)
            BLINK_25HZ_backup.InterruptMaskValue = BLINK_25HZ_STATUS_MASK;
        #endif /* (BLINK_25HZ_UseStatus) */

        #if(BLINK_25HZ_DeadBandMode == BLINK_25HZ__B_PWM__DBM_256_CLOCKS || \
            BLINK_25HZ_DeadBandMode == BLINK_25HZ__B_PWM__DBM_2_4_CLOCKS)
            BLINK_25HZ_backup.PWMdeadBandValue = BLINK_25HZ_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(BLINK_25HZ_KillModeMinTime)
             BLINK_25HZ_backup.PWMKillCounterPeriod = BLINK_25HZ_ReadKillTime();
        #endif /* (BLINK_25HZ_KillModeMinTime) */

        #if(BLINK_25HZ_UseControl)
            BLINK_25HZ_backup.PWMControlRegister = BLINK_25HZ_ReadControlRegister();
        #endif /* (BLINK_25HZ_UseControl) */
    #endif  /* (!BLINK_25HZ_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BLINK_25HZ_RestoreConfig
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
*  BLINK_25HZ_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void BLINK_25HZ_RestoreConfig(void) 
{
        #if(!BLINK_25HZ_UsingFixedFunction)
            #if(!BLINK_25HZ_PWMModeIsCenterAligned)
                BLINK_25HZ_WritePeriod(BLINK_25HZ_backup.PWMPeriod);
            #endif /* (!BLINK_25HZ_PWMModeIsCenterAligned) */

            BLINK_25HZ_WriteCounter(BLINK_25HZ_backup.PWMUdb);

            #if (BLINK_25HZ_UseStatus)
                BLINK_25HZ_STATUS_MASK = BLINK_25HZ_backup.InterruptMaskValue;
            #endif /* (BLINK_25HZ_UseStatus) */

            #if(BLINK_25HZ_DeadBandMode == BLINK_25HZ__B_PWM__DBM_256_CLOCKS || \
                BLINK_25HZ_DeadBandMode == BLINK_25HZ__B_PWM__DBM_2_4_CLOCKS)
                BLINK_25HZ_WriteDeadTime(BLINK_25HZ_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(BLINK_25HZ_KillModeMinTime)
                BLINK_25HZ_WriteKillTime(BLINK_25HZ_backup.PWMKillCounterPeriod);
            #endif /* (BLINK_25HZ_KillModeMinTime) */

            #if(BLINK_25HZ_UseControl)
                BLINK_25HZ_WriteControlRegister(BLINK_25HZ_backup.PWMControlRegister);
            #endif /* (BLINK_25HZ_UseControl) */
        #endif  /* (!BLINK_25HZ_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: BLINK_25HZ_Sleep
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
*  BLINK_25HZ_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void BLINK_25HZ_Sleep(void) 
{
    #if(BLINK_25HZ_UseControl)
        if(BLINK_25HZ_CTRL_ENABLE == (BLINK_25HZ_CONTROL & BLINK_25HZ_CTRL_ENABLE))
        {
            /*Component is enabled */
            BLINK_25HZ_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            BLINK_25HZ_backup.PWMEnableState = 0u;
        }
    #endif /* (BLINK_25HZ_UseControl) */

    /* Stop component */
    BLINK_25HZ_Stop();

    /* Save registers configuration */
    BLINK_25HZ_SaveConfig();
}


/*******************************************************************************
* Function Name: BLINK_25HZ_Wakeup
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
*  BLINK_25HZ_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void BLINK_25HZ_Wakeup(void) 
{
     /* Restore registers values */
    BLINK_25HZ_RestoreConfig();

    if(BLINK_25HZ_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        BLINK_25HZ_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
