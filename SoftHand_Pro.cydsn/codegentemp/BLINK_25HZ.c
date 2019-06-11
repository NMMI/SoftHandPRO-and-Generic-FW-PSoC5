/*******************************************************************************
* File Name: BLINK_25HZ.c
* Version 3.30
*
* Description:
*  The PWM User Module consist of an 8 or 16-bit counter with two 8 or 16-bit
*  comparitors. Each instance of this user module is capable of generating
*  two PWM outputs with the same period. The pulse width is selectable between
*  1 and 255/65535. The period is selectable between 2 and 255/65536 clocks.
*  The compare value output may be configured to be active when the present
*  counter is less than or less than/equal to the compare value.
*  A terminal count output is also provided. It generates a pulse one clock
*  width wide when the counter is equal to zero.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "BLINK_25HZ.h"

/* Error message for removed <resource> through optimization */
#ifdef BLINK_25HZ_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* BLINK_25HZ_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 BLINK_25HZ_initVar = 0u;


/*******************************************************************************
* Function Name: BLINK_25HZ_Start
********************************************************************************
*
* Summary:
*  The start function initializes the pwm with the default values, the
*  enables the counter to begin counting.  It does not enable interrupts,
*  the EnableInt command should be called if interrupt generation is required.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  BLINK_25HZ_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void BLINK_25HZ_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(BLINK_25HZ_initVar == 0u)
    {
        BLINK_25HZ_Init();
        BLINK_25HZ_initVar = 1u;
    }
    BLINK_25HZ_Enable();

}


/*******************************************************************************
* Function Name: BLINK_25HZ_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  BLINK_25HZ_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void BLINK_25HZ_Init(void) 
{
    #if (BLINK_25HZ_UsingFixedFunction || BLINK_25HZ_UseControl)
        uint8 ctrl;
    #endif /* (BLINK_25HZ_UsingFixedFunction || BLINK_25HZ_UseControl) */

    #if(!BLINK_25HZ_UsingFixedFunction)
        #if(BLINK_25HZ_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 BLINK_25HZ_interruptState;
        #endif /* (BLINK_25HZ_UseStatus) */
    #endif /* (!BLINK_25HZ_UsingFixedFunction) */

    #if (BLINK_25HZ_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        BLINK_25HZ_CONTROL |= BLINK_25HZ_CFG0_MODE;
        #if (BLINK_25HZ_DeadBand2_4)
            BLINK_25HZ_CONTROL |= BLINK_25HZ_CFG0_DB;
        #endif /* (BLINK_25HZ_DeadBand2_4) */

        ctrl = BLINK_25HZ_CONTROL3 & ((uint8 )(~BLINK_25HZ_CTRL_CMPMODE1_MASK));
        BLINK_25HZ_CONTROL3 = ctrl | BLINK_25HZ_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        BLINK_25HZ_RT1 &= ((uint8)(~BLINK_25HZ_RT1_MASK));
        BLINK_25HZ_RT1 |= BLINK_25HZ_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        BLINK_25HZ_RT1 &= ((uint8)(~BLINK_25HZ_SYNCDSI_MASK));
        BLINK_25HZ_RT1 |= BLINK_25HZ_SYNCDSI_EN;

    #elif (BLINK_25HZ_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = BLINK_25HZ_CONTROL & ((uint8)(~BLINK_25HZ_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~BLINK_25HZ_CTRL_CMPMODE1_MASK));
        BLINK_25HZ_CONTROL = ctrl | BLINK_25HZ_DEFAULT_COMPARE2_MODE |
                                   BLINK_25HZ_DEFAULT_COMPARE1_MODE;
    #endif /* (BLINK_25HZ_UsingFixedFunction) */

    #if (!BLINK_25HZ_UsingFixedFunction)
        #if (BLINK_25HZ_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            BLINK_25HZ_AUX_CONTROLDP0 |= (BLINK_25HZ_AUX_CTRL_FIFO0_CLR);
        #else /* (BLINK_25HZ_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            BLINK_25HZ_AUX_CONTROLDP0 |= (BLINK_25HZ_AUX_CTRL_FIFO0_CLR);
            BLINK_25HZ_AUX_CONTROLDP1 |= (BLINK_25HZ_AUX_CTRL_FIFO0_CLR);
        #endif /* (BLINK_25HZ_Resolution == 8) */

        BLINK_25HZ_WriteCounter(BLINK_25HZ_INIT_PERIOD_VALUE);
    #endif /* (!BLINK_25HZ_UsingFixedFunction) */

    BLINK_25HZ_WritePeriod(BLINK_25HZ_INIT_PERIOD_VALUE);

        #if (BLINK_25HZ_UseOneCompareMode)
            BLINK_25HZ_WriteCompare(BLINK_25HZ_INIT_COMPARE_VALUE1);
        #else
            BLINK_25HZ_WriteCompare1(BLINK_25HZ_INIT_COMPARE_VALUE1);
            BLINK_25HZ_WriteCompare2(BLINK_25HZ_INIT_COMPARE_VALUE2);
        #endif /* (BLINK_25HZ_UseOneCompareMode) */

        #if (BLINK_25HZ_KillModeMinTime)
            BLINK_25HZ_WriteKillTime(BLINK_25HZ_MinimumKillTime);
        #endif /* (BLINK_25HZ_KillModeMinTime) */

        #if (BLINK_25HZ_DeadBandUsed)
            BLINK_25HZ_WriteDeadTime(BLINK_25HZ_INIT_DEAD_TIME);
        #endif /* (BLINK_25HZ_DeadBandUsed) */

    #if (BLINK_25HZ_UseStatus || BLINK_25HZ_UsingFixedFunction)
        BLINK_25HZ_SetInterruptMode(BLINK_25HZ_INIT_INTERRUPTS_MODE);
    #endif /* (BLINK_25HZ_UseStatus || BLINK_25HZ_UsingFixedFunction) */

    #if (BLINK_25HZ_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        BLINK_25HZ_GLOBAL_ENABLE |= BLINK_25HZ_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        BLINK_25HZ_CONTROL2 |= BLINK_25HZ_CTRL2_IRQ_SEL;
    #else
        #if(BLINK_25HZ_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            BLINK_25HZ_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            BLINK_25HZ_STATUS_AUX_CTRL |= BLINK_25HZ_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(BLINK_25HZ_interruptState);

            /* Clear the FIFO to enable the BLINK_25HZ_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            BLINK_25HZ_ClearFIFO();
        #endif /* (BLINK_25HZ_UseStatus) */
    #endif /* (BLINK_25HZ_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BLINK_25HZ_Enable
********************************************************************************
*
* Summary:
*  Enables the PWM block operation
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This works only if software enable mode is chosen
*
*******************************************************************************/
void BLINK_25HZ_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (BLINK_25HZ_UsingFixedFunction)
        BLINK_25HZ_GLOBAL_ENABLE |= BLINK_25HZ_BLOCK_EN_MASK;
        BLINK_25HZ_GLOBAL_STBY_ENABLE |= BLINK_25HZ_BLOCK_STBY_EN_MASK;
    #endif /* (BLINK_25HZ_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (BLINK_25HZ_UseControl || BLINK_25HZ_UsingFixedFunction)
        BLINK_25HZ_CONTROL |= BLINK_25HZ_CTRL_ENABLE;
    #endif /* (BLINK_25HZ_UseControl || BLINK_25HZ_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BLINK_25HZ_Stop
********************************************************************************
*
* Summary:
*  The stop function halts the PWM, but does not change any modes or disable
*  interrupts.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  If the Enable mode is set to Hardware only then this function
*  has no effect on the operation of the PWM
*
*******************************************************************************/
void BLINK_25HZ_Stop(void) 
{
    #if (BLINK_25HZ_UseControl || BLINK_25HZ_UsingFixedFunction)
        BLINK_25HZ_CONTROL &= ((uint8)(~BLINK_25HZ_CTRL_ENABLE));
    #endif /* (BLINK_25HZ_UseControl || BLINK_25HZ_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (BLINK_25HZ_UsingFixedFunction)
        BLINK_25HZ_GLOBAL_ENABLE &= ((uint8)(~BLINK_25HZ_BLOCK_EN_MASK));
        BLINK_25HZ_GLOBAL_STBY_ENABLE &= ((uint8)(~BLINK_25HZ_BLOCK_STBY_EN_MASK));
    #endif /* (BLINK_25HZ_UsingFixedFunction) */
}

#if (BLINK_25HZ_UseOneCompareMode)
    #if (BLINK_25HZ_CompareMode1SW)


        /*******************************************************************************
        * Function Name: BLINK_25HZ_SetCompareMode
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm output when in Dither mode,
        *  Center Align Mode or One Output Mode.
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void BLINK_25HZ_SetCompareMode(uint8 comparemode) 
        {
            #if(BLINK_25HZ_UsingFixedFunction)

                #if(0 != BLINK_25HZ_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << BLINK_25HZ_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != BLINK_25HZ_CTRL_CMPMODE1_SHIFT) */

                BLINK_25HZ_CONTROL3 &= ((uint8)(~BLINK_25HZ_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                BLINK_25HZ_CONTROL3 |= comparemodemasked;

            #elif (BLINK_25HZ_UseControl)

                #if(0 != BLINK_25HZ_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << BLINK_25HZ_CTRL_CMPMODE1_SHIFT)) &
                                                BLINK_25HZ_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & BLINK_25HZ_CTRL_CMPMODE1_MASK;
                #endif /* (0 != BLINK_25HZ_CTRL_CMPMODE1_SHIFT) */

                #if(0 != BLINK_25HZ_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << BLINK_25HZ_CTRL_CMPMODE2_SHIFT)) &
                                               BLINK_25HZ_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & BLINK_25HZ_CTRL_CMPMODE2_MASK;
                #endif /* (0 != BLINK_25HZ_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                BLINK_25HZ_CONTROL &= ((uint8)(~(BLINK_25HZ_CTRL_CMPMODE1_MASK |
                                            BLINK_25HZ_CTRL_CMPMODE2_MASK)));
                BLINK_25HZ_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (BLINK_25HZ_UsingFixedFunction) */
        }
    #endif /* BLINK_25HZ_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (BLINK_25HZ_CompareMode1SW)


        /*******************************************************************************
        * Function Name: BLINK_25HZ_SetCompareMode1
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm or pwm1 output
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void BLINK_25HZ_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != BLINK_25HZ_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << BLINK_25HZ_CTRL_CMPMODE1_SHIFT)) &
                                           BLINK_25HZ_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & BLINK_25HZ_CTRL_CMPMODE1_MASK;
            #endif /* (0 != BLINK_25HZ_CTRL_CMPMODE1_SHIFT) */

            #if (BLINK_25HZ_UseControl)
                BLINK_25HZ_CONTROL &= ((uint8)(~BLINK_25HZ_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                BLINK_25HZ_CONTROL |= comparemodemasked;
            #endif /* (BLINK_25HZ_UseControl) */
        }
    #endif /* BLINK_25HZ_CompareMode1SW */

#if (BLINK_25HZ_CompareMode2SW)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_SetCompareMode2
    ********************************************************************************
    *
    * Summary:
    *  This function writes the Compare Mode for the pwm or pwm2 output
    *
    * Parameters:
    *  comparemode:  The new compare mode for the PWM output. Use the compare types
    *                defined in the H file as input arguments.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void BLINK_25HZ_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != BLINK_25HZ_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << BLINK_25HZ_CTRL_CMPMODE2_SHIFT)) &
                                                 BLINK_25HZ_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & BLINK_25HZ_CTRL_CMPMODE2_MASK;
        #endif /* (0 != BLINK_25HZ_CTRL_CMPMODE2_SHIFT) */

        #if (BLINK_25HZ_UseControl)
            BLINK_25HZ_CONTROL &= ((uint8)(~BLINK_25HZ_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            BLINK_25HZ_CONTROL |= comparemodemasked;
        #endif /* (BLINK_25HZ_UseControl) */
    }
    #endif /*BLINK_25HZ_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!BLINK_25HZ_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_WriteCounter
    ********************************************************************************
    *
    * Summary:
    *  Writes a new counter value directly to the counter register. This will be
    *  implemented for that currently running period and only that period. This API
    *  is valid only for UDB implementation and not available for fixed function
    *  PWM implementation.
    *
    * Parameters:
    *  counter:  The period new period counter value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  The PWM Period will be reloaded when a counter value will be a zero
    *
    *******************************************************************************/
    void BLINK_25HZ_WriteCounter(uint8 counter) \
                                       
    {
        CY_SET_REG8(BLINK_25HZ_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadCounter
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current value of the counter.  It doesn't matter
    *  if the counter is enabled or running.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  The current value of the counter.
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(BLINK_25HZ_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG8(BLINK_25HZ_CAPTURE_LSB_PTR));
    }

    #if (BLINK_25HZ_UseStatus)


        /*******************************************************************************
        * Function Name: BLINK_25HZ_ClearFIFO
        ********************************************************************************
        *
        * Summary:
        *  This function clears all capture data from the capture FIFO
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void BLINK_25HZ_ClearFIFO(void) 
        {
            while(0u != (BLINK_25HZ_ReadStatusRegister() & BLINK_25HZ_STATUS_FIFONEMPTY))
            {
                (void)BLINK_25HZ_ReadCapture();
            }
        }

    #endif /* BLINK_25HZ_UseStatus */

#endif /* !BLINK_25HZ_UsingFixedFunction */


/*******************************************************************************
* Function Name: BLINK_25HZ_WritePeriod
********************************************************************************
*
* Summary:
*  This function is used to change the period of the counter.  The new period
*  will be loaded the next time terminal count is detected.
*
* Parameters:
*  period:  Period value. May be between 1 and (2^Resolution)-1.  A value of 0
*           will result in the counter remaining at zero.
*
* Return:
*  None
*
*******************************************************************************/
void BLINK_25HZ_WritePeriod(uint8 period) 
{
    #if(BLINK_25HZ_UsingFixedFunction)
        CY_SET_REG16(BLINK_25HZ_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG8(BLINK_25HZ_PERIOD_LSB_PTR, period);
    #endif /* (BLINK_25HZ_UsingFixedFunction) */
}

#if (BLINK_25HZ_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_WriteCompare
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value when the PWM is in Dither
    *  mode. The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  compared to the compare value based on the compare mode defined in
    *  Dither Mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  This function is only available if the PWM mode parameter is set to
    *  Dither Mode, Center Aligned Mode or One Output Mode
    *
    *******************************************************************************/
    void BLINK_25HZ_WriteCompare(uint8 compare) \
                                       
    {
        #if(BLINK_25HZ_UsingFixedFunction)
            CY_SET_REG16(BLINK_25HZ_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(BLINK_25HZ_COMPARE1_LSB_PTR, compare);
        #endif /* (BLINK_25HZ_UsingFixedFunction) */

        #if (BLINK_25HZ_PWMMode == BLINK_25HZ__B_PWM__DITHER)
            #if(BLINK_25HZ_UsingFixedFunction)
                CY_SET_REG16(BLINK_25HZ_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG8(BLINK_25HZ_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (BLINK_25HZ_UsingFixedFunction) */
        #endif /* (BLINK_25HZ_PWMMode == BLINK_25HZ__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: BLINK_25HZ_WriteCompare1
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value.  The compare output will
    *  reflect the new value on the next UDB clock.  The compare output will be
    *  driven high when the present counter value is less than or less than or
    *  equal to the compare register, depending on the mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void BLINK_25HZ_WriteCompare1(uint8 compare) \
                                        
    {
        #if(BLINK_25HZ_UsingFixedFunction)
            CY_SET_REG16(BLINK_25HZ_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(BLINK_25HZ_COMPARE1_LSB_PTR, compare);
        #endif /* (BLINK_25HZ_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: BLINK_25HZ_WriteCompare2
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare value, for compare1 output.
    *  The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  less than or less than or equal to the compare register, depending on the
    *  mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void BLINK_25HZ_WriteCompare2(uint8 compare) \
                                        
    {
        #if(BLINK_25HZ_UsingFixedFunction)
            CY_SET_REG16(BLINK_25HZ_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG8(BLINK_25HZ_COMPARE2_LSB_PTR, compare);
        #endif /* (BLINK_25HZ_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (BLINK_25HZ_DeadBandUsed)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_WriteDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function writes the dead-band counts to the corresponding register
    *
    * Parameters:
    *  deadtime:  Number of counts for dead time
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void BLINK_25HZ_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!BLINK_25HZ_DeadBand2_4)
            CY_SET_REG8(BLINK_25HZ_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            BLINK_25HZ_DEADBAND_COUNT &= ((uint8)(~BLINK_25HZ_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(BLINK_25HZ_DEADBAND_COUNT_SHIFT)
                BLINK_25HZ_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << BLINK_25HZ_DEADBAND_COUNT_SHIFT)) &
                                                    BLINK_25HZ_DEADBAND_COUNT_MASK;
            #else
                BLINK_25HZ_DEADBAND_COUNT |= deadtime & BLINK_25HZ_DEADBAND_COUNT_MASK;
            #endif /* (BLINK_25HZ_DEADBAND_COUNT_SHIFT) */

        #endif /* (!BLINK_25HZ_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function reads the dead-band counts from the corresponding register
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Dead Band Counts
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!BLINK_25HZ_DeadBand2_4)
            return (CY_GET_REG8(BLINK_25HZ_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(BLINK_25HZ_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(BLINK_25HZ_DEADBAND_COUNT & BLINK_25HZ_DEADBAND_COUNT_MASK)) >>
                                                                           BLINK_25HZ_DEADBAND_COUNT_SHIFT));
            #else
                return (BLINK_25HZ_DEADBAND_COUNT & BLINK_25HZ_DEADBAND_COUNT_MASK);
            #endif /* (BLINK_25HZ_DEADBAND_COUNT_SHIFT) */
        #endif /* (!BLINK_25HZ_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (BLINK_25HZ_UseStatus || BLINK_25HZ_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_SetInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  This function configures the interrupts mask control of theinterrupt
    *  source status register.
    *
    * Parameters:
    *  uint8 interruptMode: Bit field containing the interrupt sources enabled
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void BLINK_25HZ_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(BLINK_25HZ_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadStatusRegister
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current state of the status register.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current status register value. The status register bits are:
    *  [7:6] : Unused(0)
    *  [5]   : Kill event output
    *  [4]   : FIFO not empty
    *  [3]   : FIFO full
    *  [2]   : Terminal count
    *  [1]   : Compare output 2
    *  [0]   : Compare output 1
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(BLINK_25HZ_STATUS_PTR));
    }

#endif /* (BLINK_25HZ_UseStatus || BLINK_25HZ_UsingFixedFunction) */


#if (BLINK_25HZ_UseControl)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the control register. This API is available
    *  only if the control register is not removed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current control register value
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(BLINK_25HZ_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: BLINK_25HZ_WriteControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Sets the bit field of the control register. This API is available only if
    *  the control register is not removed.
    *
    * Parameters:
    *  uint8 control: Control register bit field, The status register bits are:
    *  [7]   : PWM Enable
    *  [6]   : Reset
    *  [5:3] : Compare Mode2
    *  [2:0] : Compare Mode2
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void BLINK_25HZ_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(BLINK_25HZ_CONTROL_PTR, control);
    }

#endif /* (BLINK_25HZ_UseControl) */


#if (!BLINK_25HZ_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadCapture
    ********************************************************************************
    *
    * Summary:
    *  Reads the capture value from the capture FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: The current capture value
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadCapture(void) 
    {
        return (CY_GET_REG8(BLINK_25HZ_CAPTURE_LSB_PTR));
    }

#endif /* (!BLINK_25HZ_UsingFixedFunction) */


#if (BLINK_25HZ_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadCompare
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare output when the PWM Mode parameter is
    *  set to Dither mode, Center Aligned mode, or One Output mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadCompare(void) 
    {
        #if(BLINK_25HZ_UsingFixedFunction)
            return ((uint8)CY_GET_REG16(BLINK_25HZ_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG8(BLINK_25HZ_COMPARE1_LSB_PTR));
        #endif /* (BLINK_25HZ_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadCompare1
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare1 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadCompare1(void) 
    {
        return (CY_GET_REG8(BLINK_25HZ_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadCompare2
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare2 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadCompare2(void) 
    {
        return (CY_GET_REG8(BLINK_25HZ_COMPARE2_LSB_PTR));
    }

#endif /* (BLINK_25HZ_UseOneCompareMode) */


/*******************************************************************************
* Function Name: BLINK_25HZ_ReadPeriod
********************************************************************************
*
* Summary:
*  Reads the period value used by the PWM hardware.
*
* Parameters:
*  None
*
* Return:
*  uint8/16: Period value
*
*******************************************************************************/
uint8 BLINK_25HZ_ReadPeriod(void) 
{
    #if(BLINK_25HZ_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(BLINK_25HZ_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(BLINK_25HZ_PERIOD_LSB_PTR));
    #endif /* (BLINK_25HZ_UsingFixedFunction) */
}

#if ( BLINK_25HZ_KillModeMinTime)


    /*******************************************************************************
    * Function Name: BLINK_25HZ_WriteKillTime
    ********************************************************************************
    *
    * Summary:
    *  Writes the kill time value used by the hardware when the Kill Mode
    *  is set to Minimum Time.
    *
    * Parameters:
    *  uint8: Minimum Time kill counts
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void BLINK_25HZ_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(BLINK_25HZ_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: BLINK_25HZ_ReadKillTime
    ********************************************************************************
    *
    * Summary:
    *  Reads the kill time value used by the hardware when the Kill Mode is set
    *  to Minimum Time.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8: The current Minimum Time kill counts
    *
    *******************************************************************************/
    uint8 BLINK_25HZ_ReadKillTime(void) 
    {
        return (CY_GET_REG8(BLINK_25HZ_KILLMODEMINTIME_PTR));
    }

#endif /* ( BLINK_25HZ_KillModeMinTime) */

/* [] END OF FILE */
