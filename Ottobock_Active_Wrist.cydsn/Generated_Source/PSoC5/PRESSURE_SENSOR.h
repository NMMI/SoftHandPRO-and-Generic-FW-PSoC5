/*******************************************************************************
* File Name: PRESSURE_SENSOR.h  
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

#if !defined(CY_PINS_PRESSURE_SENSOR_H) /* Pins PRESSURE_SENSOR_H */
#define CY_PINS_PRESSURE_SENSOR_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "PRESSURE_SENSOR_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 PRESSURE_SENSOR__PORT == 15 && ((PRESSURE_SENSOR__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    PRESSURE_SENSOR_Write(uint8 value);
void    PRESSURE_SENSOR_SetDriveMode(uint8 mode);
uint8   PRESSURE_SENSOR_ReadDataReg(void);
uint8   PRESSURE_SENSOR_Read(void);
void    PRESSURE_SENSOR_SetInterruptMode(uint16 position, uint16 mode);
uint8   PRESSURE_SENSOR_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the PRESSURE_SENSOR_SetDriveMode() function.
     *  @{
     */
        #define PRESSURE_SENSOR_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define PRESSURE_SENSOR_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define PRESSURE_SENSOR_DM_RES_UP          PIN_DM_RES_UP
        #define PRESSURE_SENSOR_DM_RES_DWN         PIN_DM_RES_DWN
        #define PRESSURE_SENSOR_DM_OD_LO           PIN_DM_OD_LO
        #define PRESSURE_SENSOR_DM_OD_HI           PIN_DM_OD_HI
        #define PRESSURE_SENSOR_DM_STRONG          PIN_DM_STRONG
        #define PRESSURE_SENSOR_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define PRESSURE_SENSOR_MASK               PRESSURE_SENSOR__MASK
#define PRESSURE_SENSOR_SHIFT              PRESSURE_SENSOR__SHIFT
#define PRESSURE_SENSOR_WIDTH              1u

/* Interrupt constants */
#if defined(PRESSURE_SENSOR__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in PRESSURE_SENSOR_SetInterruptMode() function.
     *  @{
     */
        #define PRESSURE_SENSOR_INTR_NONE      (uint16)(0x0000u)
        #define PRESSURE_SENSOR_INTR_RISING    (uint16)(0x0001u)
        #define PRESSURE_SENSOR_INTR_FALLING   (uint16)(0x0002u)
        #define PRESSURE_SENSOR_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define PRESSURE_SENSOR_INTR_MASK      (0x01u) 
#endif /* (PRESSURE_SENSOR__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define PRESSURE_SENSOR_PS                     (* (reg8 *) PRESSURE_SENSOR__PS)
/* Data Register */
#define PRESSURE_SENSOR_DR                     (* (reg8 *) PRESSURE_SENSOR__DR)
/* Port Number */
#define PRESSURE_SENSOR_PRT_NUM                (* (reg8 *) PRESSURE_SENSOR__PRT) 
/* Connect to Analog Globals */                                                  
#define PRESSURE_SENSOR_AG                     (* (reg8 *) PRESSURE_SENSOR__AG)                       
/* Analog MUX bux enable */
#define PRESSURE_SENSOR_AMUX                   (* (reg8 *) PRESSURE_SENSOR__AMUX) 
/* Bidirectional Enable */                                                        
#define PRESSURE_SENSOR_BIE                    (* (reg8 *) PRESSURE_SENSOR__BIE)
/* Bit-mask for Aliased Register Access */
#define PRESSURE_SENSOR_BIT_MASK               (* (reg8 *) PRESSURE_SENSOR__BIT_MASK)
/* Bypass Enable */
#define PRESSURE_SENSOR_BYP                    (* (reg8 *) PRESSURE_SENSOR__BYP)
/* Port wide control signals */                                                   
#define PRESSURE_SENSOR_CTL                    (* (reg8 *) PRESSURE_SENSOR__CTL)
/* Drive Modes */
#define PRESSURE_SENSOR_DM0                    (* (reg8 *) PRESSURE_SENSOR__DM0) 
#define PRESSURE_SENSOR_DM1                    (* (reg8 *) PRESSURE_SENSOR__DM1)
#define PRESSURE_SENSOR_DM2                    (* (reg8 *) PRESSURE_SENSOR__DM2) 
/* Input Buffer Disable Override */
#define PRESSURE_SENSOR_INP_DIS                (* (reg8 *) PRESSURE_SENSOR__INP_DIS)
/* LCD Common or Segment Drive */
#define PRESSURE_SENSOR_LCD_COM_SEG            (* (reg8 *) PRESSURE_SENSOR__LCD_COM_SEG)
/* Enable Segment LCD */
#define PRESSURE_SENSOR_LCD_EN                 (* (reg8 *) PRESSURE_SENSOR__LCD_EN)
/* Slew Rate Control */
#define PRESSURE_SENSOR_SLW                    (* (reg8 *) PRESSURE_SENSOR__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define PRESSURE_SENSOR_PRTDSI__CAPS_SEL       (* (reg8 *) PRESSURE_SENSOR__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define PRESSURE_SENSOR_PRTDSI__DBL_SYNC_IN    (* (reg8 *) PRESSURE_SENSOR__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define PRESSURE_SENSOR_PRTDSI__OE_SEL0        (* (reg8 *) PRESSURE_SENSOR__PRTDSI__OE_SEL0) 
#define PRESSURE_SENSOR_PRTDSI__OE_SEL1        (* (reg8 *) PRESSURE_SENSOR__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define PRESSURE_SENSOR_PRTDSI__OUT_SEL0       (* (reg8 *) PRESSURE_SENSOR__PRTDSI__OUT_SEL0) 
#define PRESSURE_SENSOR_PRTDSI__OUT_SEL1       (* (reg8 *) PRESSURE_SENSOR__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define PRESSURE_SENSOR_PRTDSI__SYNC_OUT       (* (reg8 *) PRESSURE_SENSOR__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(PRESSURE_SENSOR__SIO_CFG)
    #define PRESSURE_SENSOR_SIO_HYST_EN        (* (reg8 *) PRESSURE_SENSOR__SIO_HYST_EN)
    #define PRESSURE_SENSOR_SIO_REG_HIFREQ     (* (reg8 *) PRESSURE_SENSOR__SIO_REG_HIFREQ)
    #define PRESSURE_SENSOR_SIO_CFG            (* (reg8 *) PRESSURE_SENSOR__SIO_CFG)
    #define PRESSURE_SENSOR_SIO_DIFF           (* (reg8 *) PRESSURE_SENSOR__SIO_DIFF)
#endif /* (PRESSURE_SENSOR__SIO_CFG) */

/* Interrupt Registers */
#if defined(PRESSURE_SENSOR__INTSTAT)
    #define PRESSURE_SENSOR_INTSTAT            (* (reg8 *) PRESSURE_SENSOR__INTSTAT)
    #define PRESSURE_SENSOR_SNAP               (* (reg8 *) PRESSURE_SENSOR__SNAP)
    
	#define PRESSURE_SENSOR_0_INTTYPE_REG 		(* (reg8 *) PRESSURE_SENSOR__0__INTTYPE)
#endif /* (PRESSURE_SENSOR__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_PRESSURE_SENSOR_H */


/* [] END OF FILE */
