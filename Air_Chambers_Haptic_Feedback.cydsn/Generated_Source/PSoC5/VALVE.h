/*******************************************************************************
* File Name: VALVE.h  
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

#if !defined(CY_PINS_VALVE_H) /* Pins VALVE_H */
#define CY_PINS_VALVE_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "VALVE_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 VALVE__PORT == 15 && ((VALVE__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    VALVE_Write(uint8 value);
void    VALVE_SetDriveMode(uint8 mode);
uint8   VALVE_ReadDataReg(void);
uint8   VALVE_Read(void);
void    VALVE_SetInterruptMode(uint16 position, uint16 mode);
uint8   VALVE_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the VALVE_SetDriveMode() function.
     *  @{
     */
        #define VALVE_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define VALVE_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define VALVE_DM_RES_UP          PIN_DM_RES_UP
        #define VALVE_DM_RES_DWN         PIN_DM_RES_DWN
        #define VALVE_DM_OD_LO           PIN_DM_OD_LO
        #define VALVE_DM_OD_HI           PIN_DM_OD_HI
        #define VALVE_DM_STRONG          PIN_DM_STRONG
        #define VALVE_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define VALVE_MASK               VALVE__MASK
#define VALVE_SHIFT              VALVE__SHIFT
#define VALVE_WIDTH              1u

/* Interrupt constants */
#if defined(VALVE__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in VALVE_SetInterruptMode() function.
     *  @{
     */
        #define VALVE_INTR_NONE      (uint16)(0x0000u)
        #define VALVE_INTR_RISING    (uint16)(0x0001u)
        #define VALVE_INTR_FALLING   (uint16)(0x0002u)
        #define VALVE_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define VALVE_INTR_MASK      (0x01u) 
#endif /* (VALVE__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define VALVE_PS                     (* (reg8 *) VALVE__PS)
/* Data Register */
#define VALVE_DR                     (* (reg8 *) VALVE__DR)
/* Port Number */
#define VALVE_PRT_NUM                (* (reg8 *) VALVE__PRT) 
/* Connect to Analog Globals */                                                  
#define VALVE_AG                     (* (reg8 *) VALVE__AG)                       
/* Analog MUX bux enable */
#define VALVE_AMUX                   (* (reg8 *) VALVE__AMUX) 
/* Bidirectional Enable */                                                        
#define VALVE_BIE                    (* (reg8 *) VALVE__BIE)
/* Bit-mask for Aliased Register Access */
#define VALVE_BIT_MASK               (* (reg8 *) VALVE__BIT_MASK)
/* Bypass Enable */
#define VALVE_BYP                    (* (reg8 *) VALVE__BYP)
/* Port wide control signals */                                                   
#define VALVE_CTL                    (* (reg8 *) VALVE__CTL)
/* Drive Modes */
#define VALVE_DM0                    (* (reg8 *) VALVE__DM0) 
#define VALVE_DM1                    (* (reg8 *) VALVE__DM1)
#define VALVE_DM2                    (* (reg8 *) VALVE__DM2) 
/* Input Buffer Disable Override */
#define VALVE_INP_DIS                (* (reg8 *) VALVE__INP_DIS)
/* LCD Common or Segment Drive */
#define VALVE_LCD_COM_SEG            (* (reg8 *) VALVE__LCD_COM_SEG)
/* Enable Segment LCD */
#define VALVE_LCD_EN                 (* (reg8 *) VALVE__LCD_EN)
/* Slew Rate Control */
#define VALVE_SLW                    (* (reg8 *) VALVE__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define VALVE_PRTDSI__CAPS_SEL       (* (reg8 *) VALVE__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define VALVE_PRTDSI__DBL_SYNC_IN    (* (reg8 *) VALVE__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define VALVE_PRTDSI__OE_SEL0        (* (reg8 *) VALVE__PRTDSI__OE_SEL0) 
#define VALVE_PRTDSI__OE_SEL1        (* (reg8 *) VALVE__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define VALVE_PRTDSI__OUT_SEL0       (* (reg8 *) VALVE__PRTDSI__OUT_SEL0) 
#define VALVE_PRTDSI__OUT_SEL1       (* (reg8 *) VALVE__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define VALVE_PRTDSI__SYNC_OUT       (* (reg8 *) VALVE__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(VALVE__SIO_CFG)
    #define VALVE_SIO_HYST_EN        (* (reg8 *) VALVE__SIO_HYST_EN)
    #define VALVE_SIO_REG_HIFREQ     (* (reg8 *) VALVE__SIO_REG_HIFREQ)
    #define VALVE_SIO_CFG            (* (reg8 *) VALVE__SIO_CFG)
    #define VALVE_SIO_DIFF           (* (reg8 *) VALVE__SIO_DIFF)
#endif /* (VALVE__SIO_CFG) */

/* Interrupt Registers */
#if defined(VALVE__INTSTAT)
    #define VALVE_INTSTAT            (* (reg8 *) VALVE__INTSTAT)
    #define VALVE_SNAP               (* (reg8 *) VALVE__SNAP)
    
	#define VALVE_0_INTTYPE_REG 		(* (reg8 *) VALVE__0__INTTYPE)
#endif /* (VALVE__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_VALVE_H */


/* [] END OF FILE */
