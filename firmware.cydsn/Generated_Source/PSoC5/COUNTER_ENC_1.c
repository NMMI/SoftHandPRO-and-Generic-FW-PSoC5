/*******************************************************************************
* File Name: COUNTER_ENC_1.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
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

#include "COUNTER_ENC_1.h"

uint8 COUNTER_ENC_1_initVar = 0u;


/*******************************************************************************
* Function Name: COUNTER_ENC_1_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void COUNTER_ENC_1_Init(void) 
{
        #if (!COUNTER_ENC_1_UsingFixedFunction && !COUNTER_ENC_1_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!COUNTER_ENC_1_UsingFixedFunction && !COUNTER_ENC_1_ControlRegRemoved) */
        
        #if(!COUNTER_ENC_1_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 COUNTER_ENC_1_interruptState;
        #endif /* (!COUNTER_ENC_1_UsingFixedFunction) */
        
        #if (COUNTER_ENC_1_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            COUNTER_ENC_1_CONTROL &= COUNTER_ENC_1_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                COUNTER_ENC_1_CONTROL2 &= ((uint8)(~COUNTER_ENC_1_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                COUNTER_ENC_1_CONTROL3 &= ((uint8)(~COUNTER_ENC_1_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (COUNTER_ENC_1_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                COUNTER_ENC_1_CONTROL |= COUNTER_ENC_1_ONESHOT;
            #endif /* (COUNTER_ENC_1_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            COUNTER_ENC_1_CONTROL2 |= COUNTER_ENC_1_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            COUNTER_ENC_1_RT1 &= ((uint8)(~COUNTER_ENC_1_RT1_MASK));
            COUNTER_ENC_1_RT1 |= COUNTER_ENC_1_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            COUNTER_ENC_1_RT1 &= ((uint8)(~COUNTER_ENC_1_SYNCDSI_MASK));
            COUNTER_ENC_1_RT1 |= COUNTER_ENC_1_SYNCDSI_EN;

        #else
            #if(!COUNTER_ENC_1_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = COUNTER_ENC_1_CONTROL & ((uint8)(~COUNTER_ENC_1_CTRL_CMPMODE_MASK));
            COUNTER_ENC_1_CONTROL = ctrl | COUNTER_ENC_1_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = COUNTER_ENC_1_CONTROL & ((uint8)(~COUNTER_ENC_1_CTRL_CAPMODE_MASK));
            
            #if( 0 != COUNTER_ENC_1_CAPTURE_MODE_CONF)
                COUNTER_ENC_1_CONTROL = ctrl | COUNTER_ENC_1_DEFAULT_CAPTURE_MODE;
            #else
                COUNTER_ENC_1_CONTROL = ctrl;
            #endif /* 0 != COUNTER_ENC_1_CAPTURE_MODE */ 
            
            #endif /* (!COUNTER_ENC_1_ControlRegRemoved) */
        #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!COUNTER_ENC_1_UsingFixedFunction)
            COUNTER_ENC_1_ClearFIFO();
        #endif /* (!COUNTER_ENC_1_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        COUNTER_ENC_1_WritePeriod(COUNTER_ENC_1_INIT_PERIOD_VALUE);
        #if (!(COUNTER_ENC_1_UsingFixedFunction && (CY_PSOC5A)))
            COUNTER_ENC_1_WriteCounter(COUNTER_ENC_1_INIT_COUNTER_VALUE);
        #endif /* (!(COUNTER_ENC_1_UsingFixedFunction && (CY_PSOC5A))) */
        COUNTER_ENC_1_SetInterruptMode(COUNTER_ENC_1_INIT_INTERRUPTS_MASK);
        
        #if (!COUNTER_ENC_1_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)COUNTER_ENC_1_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            COUNTER_ENC_1_WriteCompare(COUNTER_ENC_1_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            COUNTER_ENC_1_interruptState = CyEnterCriticalSection();
            
            COUNTER_ENC_1_STATUS_AUX_CTRL |= COUNTER_ENC_1_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(COUNTER_ENC_1_interruptState);
            
        #endif /* (!COUNTER_ENC_1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void COUNTER_ENC_1_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (COUNTER_ENC_1_UsingFixedFunction)
        COUNTER_ENC_1_GLOBAL_ENABLE |= COUNTER_ENC_1_BLOCK_EN_MASK;
        COUNTER_ENC_1_GLOBAL_STBY_ENABLE |= COUNTER_ENC_1_BLOCK_STBY_EN_MASK;
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!COUNTER_ENC_1_ControlRegRemoved || COUNTER_ENC_1_UsingFixedFunction)
        COUNTER_ENC_1_CONTROL |= COUNTER_ENC_1_CTRL_ENABLE;                
    #endif /* (!COUNTER_ENC_1_ControlRegRemoved || COUNTER_ENC_1_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  COUNTER_ENC_1_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void COUNTER_ENC_1_Start(void) 
{
    if(COUNTER_ENC_1_initVar == 0u)
    {
        COUNTER_ENC_1_Init();
        
        COUNTER_ENC_1_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    COUNTER_ENC_1_Enable();        
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void COUNTER_ENC_1_Stop(void) 
{
    /* Disable Counter */
    #if(!COUNTER_ENC_1_ControlRegRemoved || COUNTER_ENC_1_UsingFixedFunction)
        COUNTER_ENC_1_CONTROL &= ((uint8)(~COUNTER_ENC_1_CTRL_ENABLE));        
    #endif /* (!COUNTER_ENC_1_ControlRegRemoved || COUNTER_ENC_1_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (COUNTER_ENC_1_UsingFixedFunction)
        COUNTER_ENC_1_GLOBAL_ENABLE &= ((uint8)(~COUNTER_ENC_1_BLOCK_EN_MASK));
        COUNTER_ENC_1_GLOBAL_STBY_ENABLE &= ((uint8)(~COUNTER_ENC_1_BLOCK_STBY_EN_MASK));
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void COUNTER_ENC_1_SetInterruptMode(uint8 interruptsMask) 
{
    COUNTER_ENC_1_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   COUNTER_ENC_1_ReadStatusRegister(void) 
{
    return COUNTER_ENC_1_STATUS;
}


#if(!COUNTER_ENC_1_ControlRegRemoved)
/*******************************************************************************
* Function Name: COUNTER_ENC_1_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   COUNTER_ENC_1_ReadControlRegister(void) 
{
    return COUNTER_ENC_1_CONTROL;
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    COUNTER_ENC_1_WriteControlRegister(uint8 control) 
{
    COUNTER_ENC_1_CONTROL = control;
}

#endif  /* (!COUNTER_ENC_1_ControlRegRemoved) */


#if (!(COUNTER_ENC_1_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: COUNTER_ENC_1_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void COUNTER_ENC_1_WriteCounter(uint8 counter) \
                                   
{
    #if(COUNTER_ENC_1_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (COUNTER_ENC_1_GLOBAL_ENABLE & COUNTER_ENC_1_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        COUNTER_ENC_1_GLOBAL_ENABLE |= COUNTER_ENC_1_BLOCK_EN_MASK;
        CY_SET_REG16(COUNTER_ENC_1_COUNTER_LSB_PTR, (uint16)counter);
        COUNTER_ENC_1_GLOBAL_ENABLE &= ((uint8)(~COUNTER_ENC_1_BLOCK_EN_MASK));
    #else
        CY_SET_REG8(COUNTER_ENC_1_COUNTER_LSB_PTR, counter);
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
}
#endif /* (!(COUNTER_ENC_1_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: COUNTER_ENC_1_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint8) The present value of the counter.
*
*******************************************************************************/
uint8 COUNTER_ENC_1_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(COUNTER_ENC_1_UsingFixedFunction)
		(void)CY_GET_REG16(COUNTER_ENC_1_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(COUNTER_ENC_1_COUNTER_LSB_PTR_8BIT);
	#endif/* (COUNTER_ENC_1_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(COUNTER_ENC_1_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(COUNTER_ENC_1_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG8(COUNTER_ENC_1_STATICCOUNT_LSB_PTR));
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) Present Capture value.
*
*******************************************************************************/
uint8 COUNTER_ENC_1_ReadCapture(void) 
{
    #if(COUNTER_ENC_1_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(COUNTER_ENC_1_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG8(COUNTER_ENC_1_STATICCOUNT_LSB_PTR));
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint8) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void COUNTER_ENC_1_WritePeriod(uint8 period) 
{
    #if(COUNTER_ENC_1_UsingFixedFunction)
        CY_SET_REG16(COUNTER_ENC_1_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG8(COUNTER_ENC_1_PERIOD_LSB_PTR, period);
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint8) Present period value.
*
*******************************************************************************/
uint8 COUNTER_ENC_1_ReadPeriod(void) 
{
    #if(COUNTER_ENC_1_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(COUNTER_ENC_1_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(COUNTER_ENC_1_PERIOD_LSB_PTR));
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
}


#if (!COUNTER_ENC_1_UsingFixedFunction)
/*******************************************************************************
* Function Name: COUNTER_ENC_1_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void COUNTER_ENC_1_WriteCompare(uint8 compare) \
                                   
{
    #if(COUNTER_ENC_1_UsingFixedFunction)
        CY_SET_REG16(COUNTER_ENC_1_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG8(COUNTER_ENC_1_COMPARE_LSB_PTR, compare);
    #endif /* (COUNTER_ENC_1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: COUNTER_ENC_1_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint8) Present compare value.
*
*******************************************************************************/
uint8 COUNTER_ENC_1_ReadCompare(void) 
{
    return (CY_GET_REG8(COUNTER_ENC_1_COMPARE_LSB_PTR));
}


#if (COUNTER_ENC_1_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: COUNTER_ENC_1_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void COUNTER_ENC_1_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    COUNTER_ENC_1_CONTROL &= ((uint8)(~COUNTER_ENC_1_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    COUNTER_ENC_1_CONTROL |= compareMode;
}
#endif  /* (COUNTER_ENC_1_COMPARE_MODE_SOFTWARE) */


#if (COUNTER_ENC_1_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: COUNTER_ENC_1_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void COUNTER_ENC_1_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    COUNTER_ENC_1_CONTROL &= ((uint8)(~COUNTER_ENC_1_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    COUNTER_ENC_1_CONTROL |= ((uint8)((uint8)captureMode << COUNTER_ENC_1_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (COUNTER_ENC_1_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: COUNTER_ENC_1_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void COUNTER_ENC_1_ClearFIFO(void) 
{

    while(0u != (COUNTER_ENC_1_ReadStatusRegister() & COUNTER_ENC_1_STATUS_FIFONEMP))
    {
        (void)COUNTER_ENC_1_ReadCapture();
    }

}
#endif  /* (!COUNTER_ENC_1_UsingFixedFunction) */


/* [] END OF FILE */

