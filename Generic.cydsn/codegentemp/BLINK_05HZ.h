/*******************************************************************************
* File Name: BLINK_05HZ.h
* Version 3.30
*
* Description:
*  Contains the prototypes and constants for the functions available to the
*  PWM user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_PWM_BLINK_05HZ_H)
#define CY_PWM_BLINK_05HZ_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 BLINK_05HZ_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define BLINK_05HZ_Resolution                     (16u)
#define BLINK_05HZ_UsingFixedFunction             (0u)
#define BLINK_05HZ_DeadBandMode                   (0u)
#define BLINK_05HZ_KillModeMinTime                (0u)
#define BLINK_05HZ_KillMode                       (0u)
#define BLINK_05HZ_PWMMode                        (0u)
#define BLINK_05HZ_PWMModeIsCenterAligned         (0u)
#define BLINK_05HZ_DeadBandUsed                   (0u)
#define BLINK_05HZ_DeadBand2_4                    (0u)

#if !defined(BLINK_05HZ_PWMUDB_genblk8_stsreg__REMOVED)
    #define BLINK_05HZ_UseStatus                  (0u)
#else
    #define BLINK_05HZ_UseStatus                  (0u)
#endif /* !defined(BLINK_05HZ_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(BLINK_05HZ_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define BLINK_05HZ_UseControl                 (1u)
#else
    #define BLINK_05HZ_UseControl                 (0u)
#endif /* !defined(BLINK_05HZ_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define BLINK_05HZ_UseOneCompareMode              (1u)
#define BLINK_05HZ_MinimumKillTime                (1u)
#define BLINK_05HZ_EnableMode                     (2u)

#define BLINK_05HZ_CompareMode1SW                 (0u)
#define BLINK_05HZ_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define BLINK_05HZ__B_PWM__DISABLED 0
#define BLINK_05HZ__B_PWM__ASYNCHRONOUS 1
#define BLINK_05HZ__B_PWM__SINGLECYCLE 2
#define BLINK_05HZ__B_PWM__LATCHED 3
#define BLINK_05HZ__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define BLINK_05HZ__B_PWM__DBMDISABLED 0
#define BLINK_05HZ__B_PWM__DBM_2_4_CLOCKS 1
#define BLINK_05HZ__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define BLINK_05HZ__B_PWM__ONE_OUTPUT 0
#define BLINK_05HZ__B_PWM__TWO_OUTPUTS 1
#define BLINK_05HZ__B_PWM__DUAL_EDGE 2
#define BLINK_05HZ__B_PWM__CENTER_ALIGN 3
#define BLINK_05HZ__B_PWM__DITHER 5
#define BLINK_05HZ__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define BLINK_05HZ__B_PWM__LESS_THAN 1
#define BLINK_05HZ__B_PWM__LESS_THAN_OR_EQUAL 2
#define BLINK_05HZ__B_PWM__GREATER_THAN 3
#define BLINK_05HZ__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define BLINK_05HZ__B_PWM__EQUAL 0
#define BLINK_05HZ__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!BLINK_05HZ_UsingFixedFunction)
        uint16 PWMUdb;               /* PWM Current Counter value  */
        #if(!BLINK_05HZ_PWMModeIsCenterAligned)
            uint16 PWMPeriod;
        #endif /* (!BLINK_05HZ_PWMModeIsCenterAligned) */
        #if (BLINK_05HZ_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (BLINK_05HZ_UseStatus) */

        /* Backup for Deadband parameters */
        #if(BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_256_CLOCKS || \
            BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(BLINK_05HZ_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (BLINK_05HZ_KillModeMinTime) */

        /* Backup control register */
        #if(BLINK_05HZ_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (BLINK_05HZ_UseControl) */

    #endif /* (!BLINK_05HZ_UsingFixedFunction) */

}BLINK_05HZ_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    BLINK_05HZ_Start(void) ;
void    BLINK_05HZ_Stop(void) ;

#if (BLINK_05HZ_UseStatus || BLINK_05HZ_UsingFixedFunction)
    void  BLINK_05HZ_SetInterruptMode(uint8 interruptMode) ;
    uint8 BLINK_05HZ_ReadStatusRegister(void) ;
#endif /* (BLINK_05HZ_UseStatus || BLINK_05HZ_UsingFixedFunction) */

#define BLINK_05HZ_GetInterruptSource() BLINK_05HZ_ReadStatusRegister()

#if (BLINK_05HZ_UseControl)
    uint8 BLINK_05HZ_ReadControlRegister(void) ;
    void  BLINK_05HZ_WriteControlRegister(uint8 control)
          ;
#endif /* (BLINK_05HZ_UseControl) */

#if (BLINK_05HZ_UseOneCompareMode)
   #if (BLINK_05HZ_CompareMode1SW)
       void    BLINK_05HZ_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (BLINK_05HZ_CompareMode1SW) */
#else
    #if (BLINK_05HZ_CompareMode1SW)
        void    BLINK_05HZ_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (BLINK_05HZ_CompareMode1SW) */
    #if (BLINK_05HZ_CompareMode2SW)
        void    BLINK_05HZ_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (BLINK_05HZ_CompareMode2SW) */
#endif /* (BLINK_05HZ_UseOneCompareMode) */

#if (!BLINK_05HZ_UsingFixedFunction)
    uint16   BLINK_05HZ_ReadCounter(void) ;
    uint16 BLINK_05HZ_ReadCapture(void) ;

    #if (BLINK_05HZ_UseStatus)
            void BLINK_05HZ_ClearFIFO(void) ;
    #endif /* (BLINK_05HZ_UseStatus) */

    void    BLINK_05HZ_WriteCounter(uint16 counter)
            ;
#endif /* (!BLINK_05HZ_UsingFixedFunction) */

void    BLINK_05HZ_WritePeriod(uint16 period)
        ;
uint16 BLINK_05HZ_ReadPeriod(void) ;

#if (BLINK_05HZ_UseOneCompareMode)
    void    BLINK_05HZ_WriteCompare(uint16 compare)
            ;
    uint16 BLINK_05HZ_ReadCompare(void) ;
#else
    void    BLINK_05HZ_WriteCompare1(uint16 compare)
            ;
    uint16 BLINK_05HZ_ReadCompare1(void) ;
    void    BLINK_05HZ_WriteCompare2(uint16 compare)
            ;
    uint16 BLINK_05HZ_ReadCompare2(void) ;
#endif /* (BLINK_05HZ_UseOneCompareMode) */


#if (BLINK_05HZ_DeadBandUsed)
    void    BLINK_05HZ_WriteDeadTime(uint8 deadtime) ;
    uint8   BLINK_05HZ_ReadDeadTime(void) ;
#endif /* (BLINK_05HZ_DeadBandUsed) */

#if ( BLINK_05HZ_KillModeMinTime)
    void BLINK_05HZ_WriteKillTime(uint8 killtime) ;
    uint8 BLINK_05HZ_ReadKillTime(void) ;
#endif /* ( BLINK_05HZ_KillModeMinTime) */

void BLINK_05HZ_Init(void) ;
void BLINK_05HZ_Enable(void) ;
void BLINK_05HZ_Sleep(void) ;
void BLINK_05HZ_Wakeup(void) ;
void BLINK_05HZ_SaveConfig(void) ;
void BLINK_05HZ_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define BLINK_05HZ_INIT_PERIOD_VALUE          (999u)
#define BLINK_05HZ_INIT_COMPARE_VALUE1        (499u)
#define BLINK_05HZ_INIT_COMPARE_VALUE2        (0u)
#define BLINK_05HZ_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    BLINK_05HZ_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    BLINK_05HZ_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    BLINK_05HZ_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    BLINK_05HZ_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define BLINK_05HZ_DEFAULT_COMPARE2_MODE      (uint8)((uint8)2u <<  BLINK_05HZ_CTRL_CMPMODE2_SHIFT)
#define BLINK_05HZ_DEFAULT_COMPARE1_MODE      (uint8)((uint8)2u <<  BLINK_05HZ_CTRL_CMPMODE1_SHIFT)
#define BLINK_05HZ_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (BLINK_05HZ_UsingFixedFunction)
   #define BLINK_05HZ_PERIOD_LSB              (*(reg16 *) BLINK_05HZ_PWMHW__PER0)
   #define BLINK_05HZ_PERIOD_LSB_PTR          ( (reg16 *) BLINK_05HZ_PWMHW__PER0)
   #define BLINK_05HZ_COMPARE1_LSB            (*(reg16 *) BLINK_05HZ_PWMHW__CNT_CMP0)
   #define BLINK_05HZ_COMPARE1_LSB_PTR        ( (reg16 *) BLINK_05HZ_PWMHW__CNT_CMP0)
   #define BLINK_05HZ_COMPARE2_LSB            (0x00u)
   #define BLINK_05HZ_COMPARE2_LSB_PTR        (0x00u)
   #define BLINK_05HZ_COUNTER_LSB             (*(reg16 *) BLINK_05HZ_PWMHW__CNT_CMP0)
   #define BLINK_05HZ_COUNTER_LSB_PTR         ( (reg16 *) BLINK_05HZ_PWMHW__CNT_CMP0)
   #define BLINK_05HZ_CAPTURE_LSB             (*(reg16 *) BLINK_05HZ_PWMHW__CAP0)
   #define BLINK_05HZ_CAPTURE_LSB_PTR         ( (reg16 *) BLINK_05HZ_PWMHW__CAP0)
   #define BLINK_05HZ_RT1                     (*(reg8 *)  BLINK_05HZ_PWMHW__RT1)
   #define BLINK_05HZ_RT1_PTR                 ( (reg8 *)  BLINK_05HZ_PWMHW__RT1)

#else
   #if (BLINK_05HZ_Resolution == 8u) /* 8bit - PWM */

       #if(BLINK_05HZ_PWMModeIsCenterAligned)
           #define BLINK_05HZ_PERIOD_LSB      (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
           #define BLINK_05HZ_PERIOD_LSB_PTR  ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #else
           #define BLINK_05HZ_PERIOD_LSB      (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F0_REG)
           #define BLINK_05HZ_PERIOD_LSB_PTR  ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F0_REG)
       #endif /* (BLINK_05HZ_PWMModeIsCenterAligned) */

       #define BLINK_05HZ_COMPARE1_LSB        (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D0_REG)
       #define BLINK_05HZ_COMPARE1_LSB_PTR    ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D0_REG)
       #define BLINK_05HZ_COMPARE2_LSB        (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #define BLINK_05HZ_COMPARE2_LSB_PTR    ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #define BLINK_05HZ_COUNTERCAP_LSB      (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A1_REG)
       #define BLINK_05HZ_COUNTERCAP_LSB_PTR  ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A1_REG)
       #define BLINK_05HZ_COUNTER_LSB         (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A0_REG)
       #define BLINK_05HZ_COUNTER_LSB_PTR     ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A0_REG)
       #define BLINK_05HZ_CAPTURE_LSB         (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F1_REG)
       #define BLINK_05HZ_CAPTURE_LSB_PTR     ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(BLINK_05HZ_PWMModeIsCenterAligned)
               #define BLINK_05HZ_PERIOD_LSB      (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
               #define BLINK_05HZ_PERIOD_LSB_PTR  ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #else
               #define BLINK_05HZ_PERIOD_LSB      (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F0_REG)
               #define BLINK_05HZ_PERIOD_LSB_PTR  ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F0_REG)
            #endif /* (BLINK_05HZ_PWMModeIsCenterAligned) */

            #define BLINK_05HZ_COMPARE1_LSB       (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D0_REG)
            #define BLINK_05HZ_COMPARE1_LSB_PTR   ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D0_REG)
            #define BLINK_05HZ_COMPARE2_LSB       (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #define BLINK_05HZ_COMPARE2_LSB_PTR   ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #define BLINK_05HZ_COUNTERCAP_LSB     (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A1_REG)
            #define BLINK_05HZ_COUNTERCAP_LSB_PTR ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A1_REG)
            #define BLINK_05HZ_COUNTER_LSB        (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A0_REG)
            #define BLINK_05HZ_COUNTER_LSB_PTR    ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A0_REG)
            #define BLINK_05HZ_CAPTURE_LSB        (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F1_REG)
            #define BLINK_05HZ_CAPTURE_LSB_PTR    ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__F1_REG)
        #else
            #if(BLINK_05HZ_PWMModeIsCenterAligned)
               #define BLINK_05HZ_PERIOD_LSB      (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
               #define BLINK_05HZ_PERIOD_LSB_PTR  ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #else
               #define BLINK_05HZ_PERIOD_LSB      (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_F0_REG)
               #define BLINK_05HZ_PERIOD_LSB_PTR  ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_F0_REG)
            #endif /* (BLINK_05HZ_PWMModeIsCenterAligned) */

            #define BLINK_05HZ_COMPARE1_LSB       (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_D0_REG)
            #define BLINK_05HZ_COMPARE1_LSB_PTR   ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_D0_REG)
            #define BLINK_05HZ_COMPARE2_LSB       (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #define BLINK_05HZ_COMPARE2_LSB_PTR   ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #define BLINK_05HZ_COUNTERCAP_LSB     (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_A1_REG)
            #define BLINK_05HZ_COUNTERCAP_LSB_PTR ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_A1_REG)
            #define BLINK_05HZ_COUNTER_LSB        (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_A0_REG)
            #define BLINK_05HZ_COUNTER_LSB_PTR    ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_A0_REG)
            #define BLINK_05HZ_CAPTURE_LSB        (*(reg16 *) BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_F1_REG)
            #define BLINK_05HZ_CAPTURE_LSB_PTR    ((reg16 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define BLINK_05HZ_AUX_CONTROLDP1          (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u1__DP_AUX_CTL_REG)
       #define BLINK_05HZ_AUX_CONTROLDP1_PTR      ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (BLINK_05HZ_Resolution == 8) */

   #define BLINK_05HZ_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__A1_REG)
   #define BLINK_05HZ_AUX_CONTROLDP0          (*(reg8 *)  BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__DP_AUX_CTL_REG)
   #define BLINK_05HZ_AUX_CONTROLDP0_PTR      ((reg8 *)   BLINK_05HZ_PWMUDB_sP16_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (BLINK_05HZ_UsingFixedFunction) */

#if(BLINK_05HZ_KillModeMinTime )
    #define BLINK_05HZ_KILLMODEMINTIME        (*(reg8 *)  BLINK_05HZ_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define BLINK_05HZ_KILLMODEMINTIME_PTR    ((reg8 *)   BLINK_05HZ_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (BLINK_05HZ_KillModeMinTime ) */

#if(BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_256_CLOCKS)
    #define BLINK_05HZ_DEADBAND_COUNT         (*(reg8 *)  BLINK_05HZ_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define BLINK_05HZ_DEADBAND_COUNT_PTR     ((reg8 *)   BLINK_05HZ_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define BLINK_05HZ_DEADBAND_LSB_PTR       ((reg8 *)   BLINK_05HZ_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define BLINK_05HZ_DEADBAND_LSB           (*(reg8 *)  BLINK_05HZ_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (BLINK_05HZ_UsingFixedFunction)
        #define BLINK_05HZ_DEADBAND_COUNT         (*(reg8 *)  BLINK_05HZ_PWMHW__CFG0)
        #define BLINK_05HZ_DEADBAND_COUNT_PTR     ((reg8 *)   BLINK_05HZ_PWMHW__CFG0)
        #define BLINK_05HZ_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << BLINK_05HZ_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define BLINK_05HZ_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define BLINK_05HZ_DEADBAND_COUNT         (*(reg8 *)  BLINK_05HZ_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define BLINK_05HZ_DEADBAND_COUNT_PTR     ((reg8 *)   BLINK_05HZ_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define BLINK_05HZ_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << BLINK_05HZ_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define BLINK_05HZ_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (BLINK_05HZ_UsingFixedFunction) */
#endif /* (BLINK_05HZ_DeadBandMode == BLINK_05HZ__B_PWM__DBM_256_CLOCKS) */



#if (BLINK_05HZ_UsingFixedFunction)
    #define BLINK_05HZ_STATUS                 (*(reg8 *) BLINK_05HZ_PWMHW__SR0)
    #define BLINK_05HZ_STATUS_PTR             ((reg8 *) BLINK_05HZ_PWMHW__SR0)
    #define BLINK_05HZ_STATUS_MASK            (*(reg8 *) BLINK_05HZ_PWMHW__SR0)
    #define BLINK_05HZ_STATUS_MASK_PTR        ((reg8 *) BLINK_05HZ_PWMHW__SR0)
    #define BLINK_05HZ_CONTROL                (*(reg8 *) BLINK_05HZ_PWMHW__CFG0)
    #define BLINK_05HZ_CONTROL_PTR            ((reg8 *) BLINK_05HZ_PWMHW__CFG0)
    #define BLINK_05HZ_CONTROL2               (*(reg8 *) BLINK_05HZ_PWMHW__CFG1)
    #define BLINK_05HZ_CONTROL3               (*(reg8 *) BLINK_05HZ_PWMHW__CFG2)
    #define BLINK_05HZ_GLOBAL_ENABLE          (*(reg8 *) BLINK_05HZ_PWMHW__PM_ACT_CFG)
    #define BLINK_05HZ_GLOBAL_ENABLE_PTR      ( (reg8 *) BLINK_05HZ_PWMHW__PM_ACT_CFG)
    #define BLINK_05HZ_GLOBAL_STBY_ENABLE     (*(reg8 *) BLINK_05HZ_PWMHW__PM_STBY_CFG)
    #define BLINK_05HZ_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) BLINK_05HZ_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define BLINK_05HZ_BLOCK_EN_MASK          (BLINK_05HZ_PWMHW__PM_ACT_MSK)
    #define BLINK_05HZ_BLOCK_STBY_EN_MASK     (BLINK_05HZ_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define BLINK_05HZ_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define BLINK_05HZ_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define BLINK_05HZ_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define BLINK_05HZ_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define BLINK_05HZ_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define BLINK_05HZ_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define BLINK_05HZ_CTRL_ENABLE            (uint8)((uint8)0x01u << BLINK_05HZ_CTRL_ENABLE_SHIFT)
    #define BLINK_05HZ_CTRL_RESET             (uint8)((uint8)0x01u << BLINK_05HZ_CTRL_RESET_SHIFT)
    #define BLINK_05HZ_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << BLINK_05HZ_CTRL_CMPMODE2_SHIFT)
    #define BLINK_05HZ_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << BLINK_05HZ_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define BLINK_05HZ_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define BLINK_05HZ_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << BLINK_05HZ_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define BLINK_05HZ_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define BLINK_05HZ_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define BLINK_05HZ_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define BLINK_05HZ_STATUS_TC_INT_EN_MASK_SHIFT            (BLINK_05HZ_STATUS_TC_SHIFT - 4u)
    #define BLINK_05HZ_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define BLINK_05HZ_STATUS_CMP1_INT_EN_MASK_SHIFT          (BLINK_05HZ_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define BLINK_05HZ_STATUS_TC              (uint8)((uint8)0x01u << BLINK_05HZ_STATUS_TC_SHIFT)
    #define BLINK_05HZ_STATUS_CMP1            (uint8)((uint8)0x01u << BLINK_05HZ_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define BLINK_05HZ_STATUS_TC_INT_EN_MASK              (uint8)((uint8)BLINK_05HZ_STATUS_TC >> 4u)
    #define BLINK_05HZ_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)BLINK_05HZ_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define BLINK_05HZ_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define BLINK_05HZ_RT1_MASK              (uint8)((uint8)0x03u << BLINK_05HZ_RT1_SHIFT)
    #define BLINK_05HZ_SYNC                  (uint8)((uint8)0x03u << BLINK_05HZ_RT1_SHIFT)
    #define BLINK_05HZ_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define BLINK_05HZ_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << BLINK_05HZ_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define BLINK_05HZ_SYNCDSI_EN            (uint8)((uint8)0x0Fu << BLINK_05HZ_SYNCDSI_SHIFT)


#else
    #define BLINK_05HZ_STATUS                (*(reg8 *)   BLINK_05HZ_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define BLINK_05HZ_STATUS_PTR            ((reg8 *)    BLINK_05HZ_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define BLINK_05HZ_STATUS_MASK           (*(reg8 *)   BLINK_05HZ_PWMUDB_genblk8_stsreg__MASK_REG)
    #define BLINK_05HZ_STATUS_MASK_PTR       ((reg8 *)    BLINK_05HZ_PWMUDB_genblk8_stsreg__MASK_REG)
    #define BLINK_05HZ_STATUS_AUX_CTRL       (*(reg8 *)   BLINK_05HZ_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define BLINK_05HZ_CONTROL               (*(reg8 *)   BLINK_05HZ_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define BLINK_05HZ_CONTROL_PTR           ((reg8 *)    BLINK_05HZ_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define BLINK_05HZ_CTRL_ENABLE_SHIFT      (0x07u)
    #define BLINK_05HZ_CTRL_RESET_SHIFT       (0x06u)
    #define BLINK_05HZ_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define BLINK_05HZ_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define BLINK_05HZ_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define BLINK_05HZ_CTRL_ENABLE            (uint8)((uint8)0x01u << BLINK_05HZ_CTRL_ENABLE_SHIFT)
    #define BLINK_05HZ_CTRL_RESET             (uint8)((uint8)0x01u << BLINK_05HZ_CTRL_RESET_SHIFT)
    #define BLINK_05HZ_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << BLINK_05HZ_CTRL_CMPMODE2_SHIFT)
    #define BLINK_05HZ_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << BLINK_05HZ_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define BLINK_05HZ_STATUS_KILL_SHIFT          (0x05u)
    #define BLINK_05HZ_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define BLINK_05HZ_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define BLINK_05HZ_STATUS_TC_SHIFT            (0x02u)
    #define BLINK_05HZ_STATUS_CMP2_SHIFT          (0x01u)
    #define BLINK_05HZ_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define BLINK_05HZ_STATUS_KILL_INT_EN_MASK_SHIFT          (BLINK_05HZ_STATUS_KILL_SHIFT)
    #define BLINK_05HZ_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (BLINK_05HZ_STATUS_FIFONEMPTY_SHIFT)
    #define BLINK_05HZ_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (BLINK_05HZ_STATUS_FIFOFULL_SHIFT)
    #define BLINK_05HZ_STATUS_TC_INT_EN_MASK_SHIFT            (BLINK_05HZ_STATUS_TC_SHIFT)
    #define BLINK_05HZ_STATUS_CMP2_INT_EN_MASK_SHIFT          (BLINK_05HZ_STATUS_CMP2_SHIFT)
    #define BLINK_05HZ_STATUS_CMP1_INT_EN_MASK_SHIFT          (BLINK_05HZ_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define BLINK_05HZ_STATUS_KILL            (uint8)((uint8)0x00u << BLINK_05HZ_STATUS_KILL_SHIFT )
    #define BLINK_05HZ_STATUS_FIFOFULL        (uint8)((uint8)0x01u << BLINK_05HZ_STATUS_FIFOFULL_SHIFT)
    #define BLINK_05HZ_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << BLINK_05HZ_STATUS_FIFONEMPTY_SHIFT)
    #define BLINK_05HZ_STATUS_TC              (uint8)((uint8)0x01u << BLINK_05HZ_STATUS_TC_SHIFT)
    #define BLINK_05HZ_STATUS_CMP2            (uint8)((uint8)0x01u << BLINK_05HZ_STATUS_CMP2_SHIFT)
    #define BLINK_05HZ_STATUS_CMP1            (uint8)((uint8)0x01u << BLINK_05HZ_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define BLINK_05HZ_STATUS_KILL_INT_EN_MASK            (BLINK_05HZ_STATUS_KILL)
    #define BLINK_05HZ_STATUS_FIFOFULL_INT_EN_MASK        (BLINK_05HZ_STATUS_FIFOFULL)
    #define BLINK_05HZ_STATUS_FIFONEMPTY_INT_EN_MASK      (BLINK_05HZ_STATUS_FIFONEMPTY)
    #define BLINK_05HZ_STATUS_TC_INT_EN_MASK              (BLINK_05HZ_STATUS_TC)
    #define BLINK_05HZ_STATUS_CMP2_INT_EN_MASK            (BLINK_05HZ_STATUS_CMP2)
    #define BLINK_05HZ_STATUS_CMP1_INT_EN_MASK            (BLINK_05HZ_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define BLINK_05HZ_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define BLINK_05HZ_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define BLINK_05HZ_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define BLINK_05HZ_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define BLINK_05HZ_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* BLINK_05HZ_UsingFixedFunction */

#endif  /* CY_PWM_BLINK_05HZ_H */


/* [] END OF FILE */
