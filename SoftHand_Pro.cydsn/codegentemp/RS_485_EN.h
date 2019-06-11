/*******************************************************************************
* File Name: RS_485_EN.h  
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

#if !defined(CY_PINS_RS_485_EN_H) /* Pins RS_485_EN_H */
#define CY_PINS_RS_485_EN_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "RS_485_EN_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 RS_485_EN__PORT == 15 && ((RS_485_EN__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    RS_485_EN_Write(uint8 value);
void    RS_485_EN_SetDriveMode(uint8 mode);
uint8   RS_485_EN_ReadDataReg(void);
uint8   RS_485_EN_Read(void);
void    RS_485_EN_SetInterruptMode(uint16 position, uint16 mode);
uint8   RS_485_EN_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the RS_485_EN_SetDriveMode() function.
     *  @{
     */
        #define RS_485_EN_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define RS_485_EN_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define RS_485_EN_DM_RES_UP          PIN_DM_RES_UP
        #define RS_485_EN_DM_RES_DWN         PIN_DM_RES_DWN
        #define RS_485_EN_DM_OD_LO           PIN_DM_OD_LO
        #define RS_485_EN_DM_OD_HI           PIN_DM_OD_HI
        #define RS_485_EN_DM_STRONG          PIN_DM_STRONG
        #define RS_485_EN_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define RS_485_EN_MASK               RS_485_EN__MASK
#define RS_485_EN_SHIFT              RS_485_EN__SHIFT
#define RS_485_EN_WIDTH              1u

/* Interrupt constants */
#if defined(RS_485_EN__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in RS_485_EN_SetInterruptMode() function.
     *  @{
     */
        #define RS_485_EN_INTR_NONE      (uint16)(0x0000u)
        #define RS_485_EN_INTR_RISING    (uint16)(0x0001u)
        #define RS_485_EN_INTR_FALLING   (uint16)(0x0002u)
        #define RS_485_EN_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define RS_485_EN_INTR_MASK      (0x01u) 
#endif /* (RS_485_EN__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define RS_485_EN_PS                     (* (reg8 *) RS_485_EN__PS)
/* Data Register */
#define RS_485_EN_DR                     (* (reg8 *) RS_485_EN__DR)
/* Port Number */
#define RS_485_EN_PRT_NUM                (* (reg8 *) RS_485_EN__PRT) 
/* Connect to Analog Globals */                                                  
#define RS_485_EN_AG                     (* (reg8 *) RS_485_EN__AG)                       
/* Analog MUX bux enable */
#define RS_485_EN_AMUX                   (* (reg8 *) RS_485_EN__AMUX) 
/* Bidirectional Enable */                                                        
#define RS_485_EN_BIE                    (* (reg8 *) RS_485_EN__BIE)
/* Bit-mask for Aliased Register Access */
#define RS_485_EN_BIT_MASK               (* (reg8 *) RS_485_EN__BIT_MASK)
/* Bypass Enable */
#define RS_485_EN_BYP                    (* (reg8 *) RS_485_EN__BYP)
/* Port wide control signals */                                                   
#define RS_485_EN_CTL                    (* (reg8 *) RS_485_EN__CTL)
/* Drive Modes */
#define RS_485_EN_DM0                    (* (reg8 *) RS_485_EN__DM0) 
#define RS_485_EN_DM1                    (* (reg8 *) RS_485_EN__DM1)
#define RS_485_EN_DM2                    (* (reg8 *) RS_485_EN__DM2) 
/* Input Buffer Disable Override */
#define RS_485_EN_INP_DIS                (* (reg8 *) RS_485_EN__INP_DIS)
/* LCD Common or Segment Drive */
#define RS_485_EN_LCD_COM_SEG            (* (reg8 *) RS_485_EN__LCD_COM_SEG)
/* Enable Segment LCD */
#define RS_485_EN_LCD_EN                 (* (reg8 *) RS_485_EN__LCD_EN)
/* Slew Rate Control */
#define RS_485_EN_SLW                    (* (reg8 *) RS_485_EN__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define RS_485_EN_PRTDSI__CAPS_SEL       (* (reg8 *) RS_485_EN__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define RS_485_EN_PRTDSI__DBL_SYNC_IN    (* (reg8 *) RS_485_EN__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define RS_485_EN_PRTDSI__OE_SEL0        (* (reg8 *) RS_485_EN__PRTDSI__OE_SEL0) 
#define RS_485_EN_PRTDSI__OE_SEL1        (* (reg8 *) RS_485_EN__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define RS_485_EN_PRTDSI__OUT_SEL0       (* (reg8 *) RS_485_EN__PRTDSI__OUT_SEL0) 
#define RS_485_EN_PRTDSI__OUT_SEL1       (* (reg8 *) RS_485_EN__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define RS_485_EN_PRTDSI__SYNC_OUT       (* (reg8 *) RS_485_EN__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(RS_485_EN__SIO_CFG)
    #define RS_485_EN_SIO_HYST_EN        (* (reg8 *) RS_485_EN__SIO_HYST_EN)
    #define RS_485_EN_SIO_REG_HIFREQ     (* (reg8 *) RS_485_EN__SIO_REG_HIFREQ)
    #define RS_485_EN_SIO_CFG            (* (reg8 *) RS_485_EN__SIO_CFG)
    #define RS_485_EN_SIO_DIFF           (* (reg8 *) RS_485_EN__SIO_DIFF)
#endif /* (RS_485_EN__SIO_CFG) */

/* Interrupt Registers */
#if defined(RS_485_EN__INTSTAT)
    #define RS_485_EN_INTSTAT            (* (reg8 *) RS_485_EN__INTSTAT)
    #define RS_485_EN_SNAP               (* (reg8 *) RS_485_EN__SNAP)
    
	#define RS_485_EN_0_INTTYPE_REG 		(* (reg8 *) RS_485_EN__0__INTTYPE)
#endif /* (RS_485_EN__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_RS_485_EN_H */


/* [] END OF FILE */
