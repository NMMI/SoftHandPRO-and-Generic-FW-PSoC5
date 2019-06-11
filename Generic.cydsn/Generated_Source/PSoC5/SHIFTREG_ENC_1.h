/*******************************************************************************
* File Name: SHIFTREG_ENC_1.h
* Version 2.30
*
* Description:
*  This header file contains definitions associated with the Shift Register
*  component.
*
* Note: none
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/


#if !defined(CY_SHIFTREG_SHIFTREG_ENC_1_H)
#define CY_SHIFTREG_SHIFTREG_ENC_1_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define SHIFTREG_ENC_1_FIFO_SIZE          (4u)
#define SHIFTREG_ENC_1_USE_INPUT_FIFO     (0u)
#define SHIFTREG_ENC_1_USE_OUTPUT_FIFO    (1u)
#define SHIFTREG_ENC_1_SR_SIZE            (32u)


/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;

    uint32 saveSrA0Reg;
    uint32 saveSrA1Reg;

    #if(CY_UDB_V0)
        uint32 saveSrIntMask;
    #endif /* (CY_UDB_V0) */

} SHIFTREG_ENC_1_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  SHIFTREG_ENC_1_Start(void)                              ;
void  SHIFTREG_ENC_1_Stop(void)                               ;
void  SHIFTREG_ENC_1_Init(void)                               ;
void  SHIFTREG_ENC_1_Enable(void)                             ;
void  SHIFTREG_ENC_1_RestoreConfig(void)                      ;
void  SHIFTREG_ENC_1_SaveConfig(void)                         ;
void  SHIFTREG_ENC_1_Sleep(void)                              ;
void  SHIFTREG_ENC_1_Wakeup(void)                             ;
void  SHIFTREG_ENC_1_EnableInt(void)                          ;
void  SHIFTREG_ENC_1_DisableInt(void)                         ;
void  SHIFTREG_ENC_1_SetIntMode(uint8 interruptSource)        ;
uint8 SHIFTREG_ENC_1_GetIntStatus(void)                       ;
void  SHIFTREG_ENC_1_WriteRegValue(uint32 shiftData) \
                                                                ;
uint32 SHIFTREG_ENC_1_ReadRegValue(void) ;
uint8    SHIFTREG_ENC_1_GetFIFOStatus(uint8 fifoId)           ;

#if(0u != SHIFTREG_ENC_1_USE_INPUT_FIFO)
    cystatus SHIFTREG_ENC_1_WriteData(uint32 shiftData) \
                                                                ;
#endif /* (0u != SHIFTREG_ENC_1_USE_INPUT_FIFO) */

#if(0u != SHIFTREG_ENC_1_USE_OUTPUT_FIFO)
    uint32 SHIFTREG_ENC_1_ReadData(void) ;
#endif /* (0u != SHIFTREG_ENC_1_USE_OUTPUT_FIFO) */


/**********************************
*   Variable with external linkage
**********************************/

extern uint8 SHIFTREG_ENC_1_initVar;


/***************************************
*           API Constants
***************************************/

#define SHIFTREG_ENC_1_LOAD                   (0x01u)
#define SHIFTREG_ENC_1_STORE                  (0x02u)
#define SHIFTREG_ENC_1_RESET                  (0x04u)

#define SHIFTREG_ENC_1_IN_FIFO                (0x01u)
#define SHIFTREG_ENC_1_OUT_FIFO               (0x02u)

#define SHIFTREG_ENC_1_RET_FIFO_FULL          (0x00u)

/* This define is obsolete */
#define SHIFTREG_ENC_1_RET_FIFO_NOT_EMPTY     (0x01u)

#define SHIFTREG_ENC_1_RET_FIFO_PARTIAL       (0x01u)
#define SHIFTREG_ENC_1_RET_FIFO_EMPTY         (0x02u)
#define SHIFTREG_ENC_1_RET_FIFO_NOT_DEFINED   (0xFEu)


/***************************************
*    Enumerated Types and Parameters
***************************************/

#define SHIFTREG_ENC_1__LEFT 0
#define SHIFTREG_ENC_1__RIGHT 1



/***************************************
*    Initial Parameter Constants
***************************************/

#define SHIFTREG_ENC_1_SR_MASK    (0xFFFFFFFFu) /* Unsigned is added to parameter */
#define SHIFTREG_ENC_1_INT_SRC    (2u)
#define SHIFTREG_ENC_1_DIRECTION  (0u)


/***************************************
*             Registers
***************************************/

/* Control register */
#define SHIFTREG_ENC_1_SR_CONTROL_REG (* (reg8 *) \
                                           SHIFTREG_ENC_1_bSR_SyncCtl_CtrlReg__CONTROL_REG)
#define SHIFTREG_ENC_1_SR_CONTROL_PTR (  (reg8 *) \
                                           SHIFTREG_ENC_1_bSR_SyncCtl_CtrlReg__CONTROL_REG)

/* Status register */
#define SHIFTREG_ENC_1_SR_STATUS_REG      (* (reg8 *) SHIFTREG_ENC_1_bSR_StsReg__STATUS_REG)
#define SHIFTREG_ENC_1_SR_STATUS_PTR      (  (reg8 *) SHIFTREG_ENC_1_bSR_StsReg__STATUS_REG)

/* Interrupt status register */
#define SHIFTREG_ENC_1_SR_STATUS_MASK_REG (* (reg8 *) SHIFTREG_ENC_1_bSR_StsReg__MASK_REG)
#define SHIFTREG_ENC_1_SR_STATUS_MASK_PTR (  (reg8 *) SHIFTREG_ENC_1_bSR_StsReg__MASK_REG)

/* Aux control register */
#define SHIFTREG_ENC_1_SR_AUX_CONTROL_REG (* (reg8 *) SHIFTREG_ENC_1_bSR_StsReg__STATUS_AUX_CTL_REG)
#define SHIFTREG_ENC_1_SR_AUX_CONTROL_PTR (  (reg8 *) SHIFTREG_ENC_1_bSR_StsReg__STATUS_AUX_CTL_REG)

/* A1 register: only used to implement capture function */
#define SHIFTREG_ENC_1_SHIFT_REG_CAPTURE_PTR    ( (reg8 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__A1_REG )

#if(CY_PSOC3 || CY_PSOC5)
    #define SHIFTREG_ENC_1_IN_FIFO_VAL_LSB_PTR        ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__F0_REG )

    #define SHIFTREG_ENC_1_SHIFT_REG_LSB_PTR          ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__A0_REG )

    #define SHIFTREG_ENC_1_SHIFT_REG_VALUE_LSB_PTR    ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__A1_REG )

    #define SHIFTREG_ENC_1_OUT_FIFO_VAL_LSB_PTR       ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__F1_REG )

#else
    #if(SHIFTREG_ENC_1_SR_SIZE <= 8u) /* 8bit - ShiftReg */
        #define SHIFTREG_ENC_1_IN_FIFO_VAL_LSB_PTR        ( (reg8 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__F0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_LSB_PTR          ( (reg8 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__A0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_VALUE_LSB_PTR    ( (reg8 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__A1_REG )

        #define SHIFTREG_ENC_1_OUT_FIFO_VAL_LSB_PTR       ( (reg8 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__F1_REG )

    #elif(SHIFTREG_ENC_1_SR_SIZE <= 16u) /* 16bit - ShiftReg */
        #define SHIFTREG_ENC_1_IN_FIFO_VAL_LSB_PTR        ( (reg16 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__16BIT_F0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_LSB_PTR          ( (reg16 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__16BIT_A0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_VALUE_LSB_PTR    ( (reg16 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__16BIT_A1_REG )

        #define SHIFTREG_ENC_1_OUT_FIFO_VAL_LSB_PTR       ( (reg16 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__16BIT_F1_REG )


    #elif(SHIFTREG_ENC_1_SR_SIZE <= 24u) /* 24bit - ShiftReg */
        #define SHIFTREG_ENC_1_IN_FIFO_VAL_LSB_PTR        ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__F0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_LSB_PTR          ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__A0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_VALUE_LSB_PTR    ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__A1_REG )

        #define SHIFTREG_ENC_1_OUT_FIFO_VAL_LSB_PTR       ( (reg32 *) \
                                        SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__F1_REG )

    #else /* 32bit - ShiftReg */
        #define SHIFTREG_ENC_1_IN_FIFO_VAL_LSB_PTR        ( (reg32 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__32BIT_F0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_LSB_PTR          ( (reg32 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__32BIT_A0_REG )

        #define SHIFTREG_ENC_1_SHIFT_REG_VALUE_LSB_PTR    ( (reg32 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__32BIT_A1_REG )

        #define SHIFTREG_ENC_1_OUT_FIFO_VAL_LSB_PTR       ( (reg32 *) \
                                  SHIFTREG_ENC_1_bSR_sC32_BShiftRegDp_u0__32BIT_F1_REG )

    #endif  /* (SHIFTREG_ENC_1_SR_SIZE <= 8u) */
#endif      /* (CY_PSOC3 || CY_PSOC5) */


/***************************************
*       Register Constants
***************************************/

#define SHIFTREG_ENC_1_INTERRUPTS_ENABLE      (0x10u)
#define SHIFTREG_ENC_1_LOAD_INT_EN            (0x01u)
#define SHIFTREG_ENC_1_STORE_INT_EN           (0x02u)
#define SHIFTREG_ENC_1_RESET_INT_EN           (0x04u)
#define SHIFTREG_ENC_1_CLK_EN                 (0x01u)

#define SHIFTREG_ENC_1_RESET_INT_EN_MASK      (0xFBu)
#define SHIFTREG_ENC_1_LOAD_INT_EN_MASK       (0xFEu)
#define SHIFTREG_ENC_1_STORE_INT_EN_MASK      (0xFDu)
#define SHIFTREG_ENC_1_INTS_EN_MASK           (0x07u)

#define SHIFTREG_ENC_1_OUT_FIFO_CLR_BIT       (0x02u)

#if(0u != SHIFTREG_ENC_1_USE_INPUT_FIFO)

    #define SHIFTREG_ENC_1_IN_FIFO_MASK       (0x18u)

    #define SHIFTREG_ENC_1_IN_FIFO_FULL       (0x00u)
    #define SHIFTREG_ENC_1_IN_FIFO_EMPTY      (0x01u)
    #define SHIFTREG_ENC_1_IN_FIFO_PARTIAL    (0x02u)
    
    /* This define is obsolete */
    #define SHIFTREG_ENC_1_IN_FIFO_NOT_EMPTY  (0x02u)
    
#endif /* (0u != SHIFTREG_ENC_1_USE_INPUT_FIFO) */

#define SHIFTREG_ENC_1_OUT_FIFO_MASK          (0x60u)

#define SHIFTREG_ENC_1_OUT_FIFO_EMPTY         (0x00u)
#define SHIFTREG_ENC_1_OUT_FIFO_FULL          (0x01u)
#define SHIFTREG_ENC_1_OUT_FIFO_PARTIAL       (0x02u)

/* This define is obsolete */
#define SHIFTREG_ENC_1_OUT_FIFO_NOT_EMPTY     (0x02u)

#define SHIFTREG_ENC_1_IN_FIFO_SHIFT_MASK     (0x03u)
#define SHIFTREG_ENC_1_OUT_FIFO_SHIFT_MASK    (0x05u)

#define SHIFTREG_ENC_1_DISABLED               (0u)
#define SHIFTREG_ENC_1_DEFAULT_A0             (0u)
#define SHIFTREG_ENC_1_DEFAULT_A1             (0u)


/***************************************
*       Macros
***************************************/

#define SHIFTREG_ENC_1_IS_ENABLED         (0u != (SHIFTREG_ENC_1_SR_CONTROL & SHIFTREG_ENC_1_CLK_EN))

#define SHIFTREG_ENC_1_GET_OUT_FIFO_STS   ((SHIFTREG_ENC_1_SR_STATUS & SHIFTREG_ENC_1_OUT_FIFO_MASK) >> \
                                              SHIFTREG_ENC_1_OUT_FIFO_SHIFT_MASK)

#define SHIFTREG_ENC_1_GET_IN_FIFO_STS    ((SHIFTREG_ENC_1_SR_STATUS & SHIFTREG_ENC_1_IN_FIFO_MASK)  >> \
                                              SHIFTREG_ENC_1_IN_FIFO_SHIFT_MASK)


/***************************************
*       Obsolete
***************************************/

/* Following code are OBSOLETE and must not be used 
 * starting from ShiftReg 2.20 
 */

#define SHIFTREG_ENC_1_SR_CONTROL     (SHIFTREG_ENC_1_SR_CONTROL_REG)
#define SHIFTREG_ENC_1_SR_STATUS      (SHIFTREG_ENC_1_SR_STATUS_REG)
#define SHIFTREG_ENC_1_SR_STATUS_MASK (SHIFTREG_ENC_1_SR_STATUS_MASK_REG)
#define SHIFTREG_ENC_1_SR_AUX_CONTROL (SHIFTREG_ENC_1_SR_AUX_CONTROL_REG)

#define SHIFTREG_ENC_1_IN_FIFO_SHFT_MASK  (SHIFTREG_ENC_1_IN_FIFO_SHIFT_MASK)
#define SHIFTREG_ENC_1_OUT_FIFO_SHFT_MASK (SHIFTREG_ENC_1_OUT_FIFO_SHFIT_MASK)

#define SHIFTREG_ENC_1_RET_FIFO_BAD_PARAM (0xFFu)

#endif /* (CY_SHIFTREG_SHIFTREG_ENC_1_H) */


/* [] END OF FILE */
