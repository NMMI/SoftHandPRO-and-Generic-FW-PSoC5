/*******************************************************************************
* File Name: COUNTER_ENC_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "COUNTER_ENC.h"

static COUNTER_ENC_backupStruct COUNTER_ENC_backup;


/*******************************************************************************
* Function Name: COUNTER_ENC_SaveConfig
********************************************************************************
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
*  COUNTER_ENC_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void COUNTER_ENC_SaveConfig(void) 
{
    #if (!COUNTER_ENC_UsingFixedFunction)

        COUNTER_ENC_backup.CounterUdb = COUNTER_ENC_ReadCounter();

        #if(!COUNTER_ENC_ControlRegRemoved)
            COUNTER_ENC_backup.CounterControlRegister = COUNTER_ENC_ReadControlRegister();
        #endif /* (!COUNTER_ENC_ControlRegRemoved) */

    #endif /* (!COUNTER_ENC_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_RestoreConfig
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
*  COUNTER_ENC_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void COUNTER_ENC_RestoreConfig(void) 
{      
    #if (!COUNTER_ENC_UsingFixedFunction)

       COUNTER_ENC_WriteCounter(COUNTER_ENC_backup.CounterUdb);

        #if(!COUNTER_ENC_ControlRegRemoved)
            COUNTER_ENC_WriteControlRegister(COUNTER_ENC_backup.CounterControlRegister);
        #endif /* (!COUNTER_ENC_ControlRegRemoved) */

    #endif /* (!COUNTER_ENC_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_Sleep
********************************************************************************
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
*  COUNTER_ENC_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void COUNTER_ENC_Sleep(void) 
{
    #if(!COUNTER_ENC_ControlRegRemoved)
        /* Save Counter's enable state */
        if(COUNTER_ENC_CTRL_ENABLE == (COUNTER_ENC_CONTROL & COUNTER_ENC_CTRL_ENABLE))
        {
            /* Counter is enabled */
            COUNTER_ENC_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            COUNTER_ENC_backup.CounterEnableState = 0u;
        }
    #else
        COUNTER_ENC_backup.CounterEnableState = 1u;
        if(COUNTER_ENC_backup.CounterEnableState != 0u)
        {
            COUNTER_ENC_backup.CounterEnableState = 0u;
        }
    #endif /* (!COUNTER_ENC_ControlRegRemoved) */
    
    COUNTER_ENC_Stop();
    COUNTER_ENC_SaveConfig();
}


/*******************************************************************************
* Function Name: COUNTER_ENC_Wakeup
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
*  COUNTER_ENC_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void COUNTER_ENC_Wakeup(void) 
{
    COUNTER_ENC_RestoreConfig();
    #if(!COUNTER_ENC_ControlRegRemoved)
        if(COUNTER_ENC_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            COUNTER_ENC_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!COUNTER_ENC_ControlRegRemoved) */
    
}


/* [] END OF FILE */
