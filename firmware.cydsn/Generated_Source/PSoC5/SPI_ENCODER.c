/*******************************************************************************
* File Name: SPI_ENCODER.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the SPI Master component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SPI_ENCODER_PVT.h"

#if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED)
    volatile uint16 SPI_ENCODER_txBuffer[SPI_ENCODER_TX_BUFFER_SIZE];
    volatile uint8 SPI_ENCODER_txBufferFull;
    volatile uint8 SPI_ENCODER_txBufferRead;
    volatile uint8 SPI_ENCODER_txBufferWrite;
#endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */

#if(SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)
    volatile uint16 SPI_ENCODER_rxBuffer[SPI_ENCODER_RX_BUFFER_SIZE];
    volatile uint8 SPI_ENCODER_rxBufferFull;
    volatile uint8 SPI_ENCODER_rxBufferRead;
    volatile uint8 SPI_ENCODER_rxBufferWrite;
#endif /* (SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED) */

uint8 SPI_ENCODER_initVar = 0u;

volatile uint8 SPI_ENCODER_swStatusTx;
volatile uint8 SPI_ENCODER_swStatusRx;


/*******************************************************************************
* Function Name: SPI_ENCODER_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default SPIM configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  When this function is called it initializes all of the necessary parameters
*  for execution. i.e. setting the initial interrupt mask, configuring the
*  interrupt service routine, configuring the bit-counter parameters and
*  clearing the FIFO and Status Register.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_Init(void) 
{
    /* Initialize the Bit counter */
    SPI_ENCODER_COUNTER_PERIOD_REG = SPI_ENCODER_BITCTR_INIT;

    /* Init TX ISR  */
    #if(0u != SPI_ENCODER_INTERNAL_TX_INT_ENABLED)
        CyIntDisable         (SPI_ENCODER_TX_ISR_NUMBER);
        CyIntSetPriority     (SPI_ENCODER_TX_ISR_NUMBER,  SPI_ENCODER_TX_ISR_PRIORITY);
        (void) CyIntSetVector(SPI_ENCODER_TX_ISR_NUMBER, &SPI_ENCODER_TX_ISR);
    #endif /* (0u != SPI_ENCODER_INTERNAL_TX_INT_ENABLED) */

    /* Init RX ISR  */
    #if(0u != SPI_ENCODER_INTERNAL_RX_INT_ENABLED)
        CyIntDisable         (SPI_ENCODER_RX_ISR_NUMBER);
        CyIntSetPriority     (SPI_ENCODER_RX_ISR_NUMBER,  SPI_ENCODER_RX_ISR_PRIORITY);
        (void) CyIntSetVector(SPI_ENCODER_RX_ISR_NUMBER, &SPI_ENCODER_RX_ISR);
    #endif /* (0u != SPI_ENCODER_INTERNAL_RX_INT_ENABLED) */

    /* Clear any stray data from the RX and TX FIFO */
    SPI_ENCODER_ClearFIFO();

    #if(SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)
        SPI_ENCODER_rxBufferFull  = 0u;
        SPI_ENCODER_rxBufferRead  = 0u;
        SPI_ENCODER_rxBufferWrite = 0u;
    #endif /* (SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED) */

    #if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED)
        SPI_ENCODER_txBufferFull  = 0u;
        SPI_ENCODER_txBufferRead  = 0u;
        SPI_ENCODER_txBufferWrite = 0u;
    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */

    (void) SPI_ENCODER_ReadTxStatus(); /* Clear Tx status and swStatusTx */
    (void) SPI_ENCODER_ReadRxStatus(); /* Clear Rx status and swStatusRx */

    /* Configure TX and RX interrupt mask */
    SPI_ENCODER_TX_STATUS_MASK_REG = SPI_ENCODER_TX_INIT_INTERRUPTS_MASK;
    SPI_ENCODER_RX_STATUS_MASK_REG = SPI_ENCODER_RX_INIT_INTERRUPTS_MASK;
}


/*******************************************************************************
* Function Name: SPI_ENCODER_Enable
********************************************************************************
*
* Summary:
*  Enable SPIM component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SPI_ENCODER_Enable(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    SPI_ENCODER_COUNTER_CONTROL_REG |= SPI_ENCODER_CNTR_ENABLE;
    SPI_ENCODER_TX_STATUS_ACTL_REG  |= SPI_ENCODER_INT_ENABLE;
    SPI_ENCODER_RX_STATUS_ACTL_REG  |= SPI_ENCODER_INT_ENABLE;
    CyExitCriticalSection(enableInterrupts);

    #if(0u != SPI_ENCODER_INTERNAL_CLOCK)
        SPI_ENCODER_IntClock_Enable();
    #endif /* (0u != SPI_ENCODER_INTERNAL_CLOCK) */

    SPI_ENCODER_EnableTxInt();
    SPI_ENCODER_EnableRxInt();
}


/*******************************************************************************
* Function Name: SPI_ENCODER_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPI_ENCODER_initVar - used to check initial configuration, modified on
*  first function call.
*
* Theory:
*  Enable the clock input to enable operation.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_Start(void) 
{
    if(0u == SPI_ENCODER_initVar)
    {
        SPI_ENCODER_Init();
        SPI_ENCODER_initVar = 1u;
    }

    SPI_ENCODER_Enable();
}


/*******************************************************************************
* Function Name: SPI_ENCODER_Stop
********************************************************************************
*
* Summary:
*  Disable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the clock input to enable operation.
*
*******************************************************************************/
void SPI_ENCODER_Stop(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    SPI_ENCODER_TX_STATUS_ACTL_REG &= ((uint8) ~SPI_ENCODER_INT_ENABLE);
    SPI_ENCODER_RX_STATUS_ACTL_REG &= ((uint8) ~SPI_ENCODER_INT_ENABLE);
    CyExitCriticalSection(enableInterrupts);

    #if(0u != SPI_ENCODER_INTERNAL_CLOCK)
        SPI_ENCODER_IntClock_Disable();
    #endif /* (0u != SPI_ENCODER_INTERNAL_CLOCK) */

    SPI_ENCODER_DisableTxInt();
    SPI_ENCODER_DisableRxInt();
}


/*******************************************************************************
* Function Name: SPI_ENCODER_EnableTxInt
********************************************************************************
*
* Summary:
*  Enable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPI_ENCODER_EnableTxInt(void) 
{
    #if(0u != SPI_ENCODER_INTERNAL_TX_INT_ENABLED)
        CyIntEnable(SPI_ENCODER_TX_ISR_NUMBER);
    #endif /* (0u != SPI_ENCODER_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SPI_ENCODER_EnableRxInt
********************************************************************************
*
* Summary:
*  Enable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPI_ENCODER_EnableRxInt(void) 
{
    #if(0u != SPI_ENCODER_INTERNAL_RX_INT_ENABLED)
        CyIntEnable(SPI_ENCODER_RX_ISR_NUMBER);
    #endif /* (0u != SPI_ENCODER_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SPI_ENCODER_DisableTxInt
********************************************************************************
*
* Summary:
*  Disable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPI_ENCODER_DisableTxInt(void) 
{
    #if(0u != SPI_ENCODER_INTERNAL_TX_INT_ENABLED)
        CyIntDisable(SPI_ENCODER_TX_ISR_NUMBER);
    #endif /* (0u != SPI_ENCODER_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SPI_ENCODER_DisableRxInt
********************************************************************************
*
* Summary:
*  Disable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPI_ENCODER_DisableRxInt(void) 
{
    #if(0u != SPI_ENCODER_INTERNAL_RX_INT_ENABLED)
        CyIntDisable(SPI_ENCODER_RX_ISR_NUMBER);
    #endif /* (0u != SPI_ENCODER_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SPI_ENCODER_SetTxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SPI_ENCODER_SetTxInterruptMode(uint8 intSrc) 
{
    SPI_ENCODER_TX_STATUS_MASK_REG = intSrc;
}


/*******************************************************************************
* Function Name: SPI_ENCODER_SetRxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SPI_ENCODER_SetRxInterruptMode(uint8 intSrc) 
{
    SPI_ENCODER_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: SPI_ENCODER_ReadTxStatus
********************************************************************************
*
* Summary:
*  Read the Tx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Tx status register.
*
* Global variables:
*  SPI_ENCODER_swStatusTx - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Tx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SPI_ENCODER_ReadTxStatus(void) 
{
    uint8 tmpStatus;

    #if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        SPI_ENCODER_DisableTxInt();

        tmpStatus = SPI_ENCODER_GET_STATUS_TX(SPI_ENCODER_swStatusTx);
        SPI_ENCODER_swStatusTx = 0u;

        SPI_ENCODER_EnableTxInt();

    #else

        tmpStatus = SPI_ENCODER_TX_STATUS_REG;

    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: SPI_ENCODER_ReadRxStatus
********************************************************************************
*
* Summary:
*  Read the Rx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Rx status register.
*
* Global variables:
*  SPI_ENCODER_swStatusRx - used to store in software Rx status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Rx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Rx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SPI_ENCODER_ReadRxStatus(void) 
{
    uint8 tmpStatus;

    #if(SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        SPI_ENCODER_DisableRxInt();

        tmpStatus = SPI_ENCODER_GET_STATUS_RX(SPI_ENCODER_swStatusRx);
        SPI_ENCODER_swStatusRx = 0u;

        SPI_ENCODER_EnableRxInt();

    #else

        tmpStatus = SPI_ENCODER_RX_STATUS_REG;

    #endif /* (SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: SPI_ENCODER_WriteTxData
********************************************************************************
*
* Summary:
*  Write a byte of data to be sent across the SPI.
*
* Parameters:
*  txDataByte: The data value to send across the SPI.
*
* Return:
*  None.
*
* Global variables:
*  SPI_ENCODER_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call if TX Software Buffer is used.
*  SPI_ENCODER_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*  SPI_ENCODER_txBuffer[SPI_ENCODER_TX_BUFFER_SIZE] - used to store
*  data to sending, modified every function call if TX Software Buffer is used.
*
* Theory:
*  Allows the user to transmit any byte of data in a single transfer.
*
* Side Effects:
*  If this function is called again before the previous byte is finished then
*  the next byte will be appended to the transfer with no time between
*  the byte transfers. Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_WriteTxData(uint16 txData) 
{
    #if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED)

        uint8 tempStatus;
        uint8 tmpTxBufferRead;

        /* Block if TX buffer is FULL: don't overwrite */
        do
        {
            tmpTxBufferRead = SPI_ENCODER_txBufferRead;
            if(0u == tmpTxBufferRead)
            {
                tmpTxBufferRead = (SPI_ENCODER_TX_BUFFER_SIZE - 1u);
            }
            else
            {
                tmpTxBufferRead--;
            }

        }while(tmpTxBufferRead == SPI_ENCODER_txBufferWrite);

        /* Disable TX interrupt to protect global veriables */
        SPI_ENCODER_DisableTxInt();

        tempStatus = SPI_ENCODER_GET_STATUS_TX(SPI_ENCODER_swStatusTx);
        SPI_ENCODER_swStatusTx = tempStatus;


        if((SPI_ENCODER_txBufferRead == SPI_ENCODER_txBufferWrite) &&
           (0u != (SPI_ENCODER_swStatusTx & SPI_ENCODER_STS_TX_FIFO_NOT_FULL)))
        {
            /* Put data element into the TX FIFO */
            CY_SET_REG16(SPI_ENCODER_TXDATA_PTR, txData);
        }
        else
        {
            /* Add to the TX software buffer */
            SPI_ENCODER_txBufferWrite++;
            if(SPI_ENCODER_txBufferWrite >= SPI_ENCODER_TX_BUFFER_SIZE)
            {
                SPI_ENCODER_txBufferWrite = 0u;
            }

            if(SPI_ENCODER_txBufferWrite == SPI_ENCODER_txBufferRead)
            {
                SPI_ENCODER_txBufferRead++;
                if(SPI_ENCODER_txBufferRead >= SPI_ENCODER_TX_BUFFER_SIZE)
                {
                    SPI_ENCODER_txBufferRead = 0u;
                }
                SPI_ENCODER_txBufferFull = 1u;
            }

            SPI_ENCODER_txBuffer[SPI_ENCODER_txBufferWrite] = txData;

            SPI_ENCODER_TX_STATUS_MASK_REG |= SPI_ENCODER_STS_TX_FIFO_NOT_FULL;
        }

        SPI_ENCODER_EnableTxInt();

    #else
        /* Wait until TX FIFO has a place */
        while(0u == (SPI_ENCODER_TX_STATUS_REG & SPI_ENCODER_STS_TX_FIFO_NOT_FULL))
        {
        }

        /* Put data element into the TX FIFO */
        CY_SET_REG16(SPI_ENCODER_TXDATA_PTR, txData);

    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: SPI_ENCODER_ReadRxData
********************************************************************************
*
* Summary:
*  Read the next byte of data received across the SPI.
*
* Parameters:
*  None.
*
* Return:
*  The next byte of data read from the FIFO.
*
* Global variables:
*  SPI_ENCODER_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  SPI_ENCODER_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function
*  call if RX Software Buffer is used.
*  SPI_ENCODER_rxBuffer[SPI_ENCODER_RX_BUFFER_SIZE] - used to store
*  received data.
*
* Theory:
*  Allows the user to read a byte of data received.
*
* Side Effects:
*  Will return invalid data if the FIFO is empty. The user should Call
*  GetRxBufferSize() and if it returns a non-zero value then it is safe to call
*  ReadByte() function.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint16 SPI_ENCODER_ReadRxData(void) 
{
    uint16 rxData;

    #if(SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        SPI_ENCODER_DisableRxInt();

        if(SPI_ENCODER_rxBufferRead != SPI_ENCODER_rxBufferWrite)
        {
            if(0u == SPI_ENCODER_rxBufferFull)
            {
                SPI_ENCODER_rxBufferRead++;
                if(SPI_ENCODER_rxBufferRead >= SPI_ENCODER_RX_BUFFER_SIZE)
                {
                    SPI_ENCODER_rxBufferRead = 0u;
                }
            }
            else
            {
                SPI_ENCODER_rxBufferFull = 0u;
            }
        }

        rxData = SPI_ENCODER_rxBuffer[SPI_ENCODER_rxBufferRead];

        SPI_ENCODER_EnableRxInt();

    #else

        rxData = CY_GET_REG16(SPI_ENCODER_RXDATA_PTR);

    #endif /* (SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED) */

    return(rxData);
}


/*******************************************************************************
* Function Name: SPI_ENCODER_GetRxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the RX buffer.
*  If RX Software Buffer not used then function return 0 if FIFO empty or 1 if
*  FIFO not empty. In another case function return size of RX Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the RX buffer.
*
* Global variables:
*  SPI_ENCODER_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  SPI_ENCODER_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8 SPI_ENCODER_GetRxBufferSize(void) 
{
    uint8 size;

    #if(SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        SPI_ENCODER_DisableRxInt();

        if(SPI_ENCODER_rxBufferRead == SPI_ENCODER_rxBufferWrite)
        {
            size = 0u;
        }
        else if(SPI_ENCODER_rxBufferRead < SPI_ENCODER_rxBufferWrite)
        {
            size = (SPI_ENCODER_rxBufferWrite - SPI_ENCODER_rxBufferRead);
        }
        else
        {
            size = (SPI_ENCODER_RX_BUFFER_SIZE - SPI_ENCODER_rxBufferRead) + SPI_ENCODER_rxBufferWrite;
        }

        SPI_ENCODER_EnableRxInt();

    #else

        /* We can only know if there is data in the RX FIFO */
        size = (0u != (SPI_ENCODER_RX_STATUS_REG & SPI_ENCODER_STS_RX_FIFO_NOT_EMPTY)) ? 1u : 0u;

    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: SPI_ENCODER_GetTxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the TX buffer.
*  If TX Software Buffer not used then function return 0 - if FIFO empty, 1 - if
*  FIFO not full, 4 - if FIFO full. In another case function return size of TX
*  Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the TX buffer.
*
* Global variables:
*  SPI_ENCODER_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  SPI_ENCODER_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8  SPI_ENCODER_GetTxBufferSize(void) 
{
    uint8 size;

    #if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        SPI_ENCODER_DisableTxInt();

        if(SPI_ENCODER_txBufferRead == SPI_ENCODER_txBufferWrite)
        {
            size = 0u;
        }
        else if(SPI_ENCODER_txBufferRead < SPI_ENCODER_txBufferWrite)
        {
            size = (SPI_ENCODER_txBufferWrite - SPI_ENCODER_txBufferRead);
        }
        else
        {
            size = (SPI_ENCODER_TX_BUFFER_SIZE - SPI_ENCODER_txBufferRead) + SPI_ENCODER_txBufferWrite;
        }

        SPI_ENCODER_EnableTxInt();

    #else

        size = SPI_ENCODER_TX_STATUS_REG;

        if(0u != (size & SPI_ENCODER_STS_TX_FIFO_EMPTY))
        {
            size = 0u;
        }
        else if(0u != (size & SPI_ENCODER_STS_TX_FIFO_NOT_FULL))
        {
            size = 1u;
        }
        else
        {
            size = SPI_ENCODER_FIFO_SIZE;
        }

    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: SPI_ENCODER_ClearRxBuffer
********************************************************************************
*
* Summary:
*  Clear the RX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPI_ENCODER_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer, modified every function
*  call - resets to zero.
*  SPI_ENCODER_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any received data not read from the RAM buffer will be lost when overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_ClearRxBuffer(void) 
{
    /* Clear Hardware RX FIFO */
    while(0u !=(SPI_ENCODER_RX_STATUS_REG & SPI_ENCODER_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG16(SPI_ENCODER_RXDATA_PTR);
    }

    #if(SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        SPI_ENCODER_DisableRxInt();

        SPI_ENCODER_rxBufferFull  = 0u;
        SPI_ENCODER_rxBufferRead  = 0u;
        SPI_ENCODER_rxBufferWrite = 0u;

        SPI_ENCODER_EnableRxInt();
    #endif /* (SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: SPI_ENCODER_ClearTxBuffer
********************************************************************************
*
* Summary:
*  Clear the TX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPI_ENCODER_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call - resets to zero.
*  SPI_ENCODER_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any data not yet transmitted from the RAM buffer will be lost when
*  overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_ClearTxBuffer(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    SPI_ENCODER_AUX_CONTROL_DP0_REG |= ((uint8)  SPI_ENCODER_TX_FIFO_CLR);
    SPI_ENCODER_AUX_CONTROL_DP0_REG &= ((uint8) ~SPI_ENCODER_TX_FIFO_CLR);

    #if(SPI_ENCODER_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        SPI_ENCODER_AUX_CONTROL_DP1_REG |= ((uint8)  SPI_ENCODER_TX_FIFO_CLR);
        SPI_ENCODER_AUX_CONTROL_DP1_REG &= ((uint8) ~SPI_ENCODER_TX_FIFO_CLR);
    #endif /* (SPI_ENCODER_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);

    #if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        SPI_ENCODER_DisableTxInt();

        SPI_ENCODER_txBufferFull  = 0u;
        SPI_ENCODER_txBufferRead  = 0u;
        SPI_ENCODER_txBufferWrite = 0u;

        /* Buffer is EMPTY: disable TX FIFO NOT FULL interrupt */
        SPI_ENCODER_TX_STATUS_MASK_REG &= ((uint8) ~SPI_ENCODER_STS_TX_FIFO_NOT_FULL);

        SPI_ENCODER_EnableTxInt();
    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED) */
}


#if(0u != SPI_ENCODER_BIDIRECTIONAL_MODE)
    /*******************************************************************************
    * Function Name: SPI_ENCODER_TxEnable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to transmit.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SPI_ENCODER_TxEnable(void) 
    {
        SPI_ENCODER_CONTROL_REG |= SPI_ENCODER_CTRL_TX_SIGNAL_EN;
    }


    /*******************************************************************************
    * Function Name: SPI_ENCODER_TxDisable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to receive.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SPI_ENCODER_TxDisable(void) 
    {
        SPI_ENCODER_CONTROL_REG &= ((uint8) ~SPI_ENCODER_CTRL_TX_SIGNAL_EN);
    }

#endif /* (0u != SPI_ENCODER_BIDIRECTIONAL_MODE) */


/*******************************************************************************
* Function Name: SPI_ENCODER_PutArray
********************************************************************************
*
* Summary:
*  Write available data from ROM/RAM to the TX buffer while space is available
*  in the TX buffer. Keep trying until all data is passed to the TX buffer.
*
* Parameters:
*  *buffer: Pointer to the location in RAM containing the data to send
*  byteCount: The number of bytes to move to the transmit buffer.
*
* Return:
*  None.
*
* Side Effects:
*  Will stay in this routine until all data has been sent.  May get locked in
*  this loop if data is not being initiated by the master if there is not
*  enough room in the TX FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPI_ENCODER_PutArray(const uint16 buffer[], uint8 byteCount)
                                                                          
{
    uint8 bufIndex;

    bufIndex = 0u;

    while(byteCount > 0u)
    {
        SPI_ENCODER_WriteTxData(buffer[bufIndex]);
        bufIndex++;
        byteCount--;
    }
}


/*******************************************************************************
* Function Name: SPI_ENCODER_ClearFIFO
********************************************************************************
*
* Summary:
*  Clear the RX and TX FIFO's of all data for a fresh start.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
void SPI_ENCODER_ClearFIFO(void) 
{
    uint8 enableInterrupts;

    /* Clear Hardware RX FIFO */
    while(0u !=(SPI_ENCODER_RX_STATUS_REG & SPI_ENCODER_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG16(SPI_ENCODER_RXDATA_PTR);
    }

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    SPI_ENCODER_AUX_CONTROL_DP0_REG |= ((uint8)  SPI_ENCODER_TX_FIFO_CLR);
    SPI_ENCODER_AUX_CONTROL_DP0_REG &= ((uint8) ~SPI_ENCODER_TX_FIFO_CLR);

    #if(SPI_ENCODER_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        SPI_ENCODER_AUX_CONTROL_DP1_REG |= ((uint8)  SPI_ENCODER_TX_FIFO_CLR);
        SPI_ENCODER_AUX_CONTROL_DP1_REG &= ((uint8) ~SPI_ENCODER_TX_FIFO_CLR);
    #endif /* (SPI_ENCODER_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);
}


/* Following functions are for version Compatibility, they are obsolete.
*  Please do not use it in new projects.
*/


/*******************************************************************************
* Function Name: SPI_ENCODER_EnableInt
********************************************************************************
*
* Summary:
*  Enable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPI_ENCODER_EnableInt(void) 
{
    SPI_ENCODER_EnableRxInt();
    SPI_ENCODER_EnableTxInt();
}


/*******************************************************************************
* Function Name: SPI_ENCODER_DisableInt
********************************************************************************
*
* Summary:
*  Disable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SPI_ENCODER_DisableInt(void) 
{
    SPI_ENCODER_DisableTxInt();
    SPI_ENCODER_DisableRxInt();
}


/*******************************************************************************
* Function Name: SPI_ENCODER_SetInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SPI_ENCODER_SetInterruptMode(uint8 intSrc) 
{
    SPI_ENCODER_TX_STATUS_MASK_REG  = (intSrc & ((uint8) ~SPI_ENCODER_STS_SPI_IDLE));
    SPI_ENCODER_RX_STATUS_MASK_REG  =  intSrc;
}


/*******************************************************************************
* Function Name: SPI_ENCODER_ReadStatus
********************************************************************************
*
* Summary:
*  Read the status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the status register.
*
* Global variables:
*  SPI_ENCODER_swStatus - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the status register for error detection
*  and flow control.
*
* Side Effects:
*  Clear status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SPI_ENCODER_ReadStatus(void) 
{
    uint8 tmpStatus;

    #if(SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED || SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED)

        SPI_ENCODER_DisableInt();

        tmpStatus  = SPI_ENCODER_GET_STATUS_RX(SPI_ENCODER_swStatusRx);
        tmpStatus |= SPI_ENCODER_GET_STATUS_TX(SPI_ENCODER_swStatusTx);
        tmpStatus &= ((uint8) ~SPI_ENCODER_STS_SPI_IDLE);

        SPI_ENCODER_swStatusTx = 0u;
        SPI_ENCODER_swStatusRx = 0u;

        SPI_ENCODER_EnableInt();

    #else

        tmpStatus  = SPI_ENCODER_RX_STATUS_REG;
        tmpStatus |= SPI_ENCODER_TX_STATUS_REG;
        tmpStatus &= ((uint8) ~SPI_ENCODER_STS_SPI_IDLE);

    #endif /* (SPI_ENCODER_TX_SOFTWARE_BUF_ENABLED || SPI_ENCODER_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/* [] END OF FILE */
