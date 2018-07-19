/*******************************************************************************
* File Name: CS0.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_CS0_H) /* Pins CS0_H */
#define CY_PINS_CS0_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CS0_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 CS0__PORT == 15 && ((CS0__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    CS0_Write(uint8 value);
void    CS0_SetDriveMode(uint8 mode);
uint8   CS0_ReadDataReg(void);
uint8   CS0_Read(void);
void    CS0_SetInterruptMode(uint16 position, uint16 mode);
uint8   CS0_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the CS0_SetDriveMode() function.
     *  @{
     */
        #define CS0_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define CS0_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define CS0_DM_RES_UP          PIN_DM_RES_UP
        #define CS0_DM_RES_DWN         PIN_DM_RES_DWN
        #define CS0_DM_OD_LO           PIN_DM_OD_LO
        #define CS0_DM_OD_HI           PIN_DM_OD_HI
        #define CS0_DM_STRONG          PIN_DM_STRONG
        #define CS0_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define CS0_MASK               CS0__MASK
#define CS0_SHIFT              CS0__SHIFT
#define CS0_WIDTH              1u

/* Interrupt constants */
#if defined(CS0__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in CS0_SetInterruptMode() function.
     *  @{
     */
        #define CS0_INTR_NONE      (uint16)(0x0000u)
        #define CS0_INTR_RISING    (uint16)(0x0001u)
        #define CS0_INTR_FALLING   (uint16)(0x0002u)
        #define CS0_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define CS0_INTR_MASK      (0x01u) 
#endif /* (CS0__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define CS0_PS                     (* (reg8 *) CS0__PS)
/* Data Register */
#define CS0_DR                     (* (reg8 *) CS0__DR)
/* Port Number */
#define CS0_PRT_NUM                (* (reg8 *) CS0__PRT) 
/* Connect to Analog Globals */                                                  
#define CS0_AG                     (* (reg8 *) CS0__AG)                       
/* Analog MUX bux enable */
#define CS0_AMUX                   (* (reg8 *) CS0__AMUX) 
/* Bidirectional Enable */                                                        
#define CS0_BIE                    (* (reg8 *) CS0__BIE)
/* Bit-mask for Aliased Register Access */
#define CS0_BIT_MASK               (* (reg8 *) CS0__BIT_MASK)
/* Bypass Enable */
#define CS0_BYP                    (* (reg8 *) CS0__BYP)
/* Port wide control signals */                                                   
#define CS0_CTL                    (* (reg8 *) CS0__CTL)
/* Drive Modes */
#define CS0_DM0                    (* (reg8 *) CS0__DM0) 
#define CS0_DM1                    (* (reg8 *) CS0__DM1)
#define CS0_DM2                    (* (reg8 *) CS0__DM2) 
/* Input Buffer Disable Override */
#define CS0_INP_DIS                (* (reg8 *) CS0__INP_DIS)
/* LCD Common or Segment Drive */
#define CS0_LCD_COM_SEG            (* (reg8 *) CS0__LCD_COM_SEG)
/* Enable Segment LCD */
#define CS0_LCD_EN                 (* (reg8 *) CS0__LCD_EN)
/* Slew Rate Control */
#define CS0_SLW                    (* (reg8 *) CS0__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define CS0_PRTDSI__CAPS_SEL       (* (reg8 *) CS0__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define CS0_PRTDSI__DBL_SYNC_IN    (* (reg8 *) CS0__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define CS0_PRTDSI__OE_SEL0        (* (reg8 *) CS0__PRTDSI__OE_SEL0) 
#define CS0_PRTDSI__OE_SEL1        (* (reg8 *) CS0__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define CS0_PRTDSI__OUT_SEL0       (* (reg8 *) CS0__PRTDSI__OUT_SEL0) 
#define CS0_PRTDSI__OUT_SEL1       (* (reg8 *) CS0__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define CS0_PRTDSI__SYNC_OUT       (* (reg8 *) CS0__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(CS0__SIO_CFG)
    #define CS0_SIO_HYST_EN        (* (reg8 *) CS0__SIO_HYST_EN)
    #define CS0_SIO_REG_HIFREQ     (* (reg8 *) CS0__SIO_REG_HIFREQ)
    #define CS0_SIO_CFG            (* (reg8 *) CS0__SIO_CFG)
    #define CS0_SIO_DIFF           (* (reg8 *) CS0__SIO_DIFF)
#endif /* (CS0__SIO_CFG) */

/* Interrupt Registers */
#if defined(CS0__INTSTAT)
    #define CS0_INTSTAT            (* (reg8 *) CS0__INTSTAT)
    #define CS0_SNAP               (* (reg8 *) CS0__SNAP)
    
	#define CS0_0_INTTYPE_REG 		(* (reg8 *) CS0__0__INTTYPE)
#endif /* (CS0__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_CS0_H */


/* [] END OF FILE */
