/************************************************************************************
*         File: MPF4279x.h																														*
*					Description: Header for the MPF4279x_I2C.c file												 		*
*					CPU: STM32F03xx Medium density devices																		*
*					Project:	MPS BCN, EVKT-BMU1xx MP279x+MPF4279x Main MCU										*
*					Version:	1.0																															*
*					Author:		Albert Corbella & Miguel A. Sanchez															*
*					Date:			01/09/2021																											*
*************************************************************************************/

#include "n32h47x_48x.h"
#include "n32h47x_48x_i2c.h"

#define bool _Bool
//MPF4279x Commands MAP
#define FG_Trigger          	   		0x7FFE

#define MPF4279x_Address						0x08
#define MPF4279x_CRC_EN							1
#define NO_STOP											1
#define ADD_STOP										0

#ifdef FG_NewGen

typedef enum{
	CHARGE, DISCHARGE, REST
}def_fg_status;

typedef enum{
	Cell, Pack, Chgr, Smrtchgr
}def_fg_lim;

typedef enum{
	Disabled, Paused, Ongoing
}def_fg_lrn;

typedef enum{
	Default, Updated, Updated2Max, Updated2Min
}def_fg_rslt;

typedef struct __attribute__((packed)){
	unsigned char id;
	unsigned chg_cc:			1;
	unsigned chg_end:			1;
	unsigned dis:				1;
}def_fg_OT;

typedef struct __attribute__((packed)){
	unsigned iteration:		1;
	unsigned status: 		1;
	unsigned emty_lim:		1;
	unsigned full_lim:		1;
	unsigned soc_pack_rslt: 1;
	unsigned OT_warn:		1;
	unsigned idis_avg:  	1;
	unsigned idis_end:  	1;
	unsigned ichg_cc:  		1;
	unsigned ichg_end:  	1;
	unsigned vchg_cv:  		1;
	unsigned soresr_cells:  1;
	unsigned rcxn_cells:  	1;
	unsigned hconv:  		1;
	unsigned soh_cells:  	1;
	unsigned etac_cells:  	1;
	unsigned pdis_lim:  	1;
	unsigned pchg_lim:  	1;
}def_fg_intr;

typedef __packed struct __attribute__((packed)){

	def_fg_intr		intr;

	//	STATUS
	def_fg_status	status;

	// 	SOC CELLS
	unsigned char 	soc_abs_cells[SETMAX_NCELLS_SER];

	//	EMPTY
	unsigned char	emty_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	emty_id;
	unsigned short 	emty_rtime;
	signed short	emty_dT;
	def_fg_lim		emty_lim;
	
	//	FULL
	unsigned char	full_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	full_id;
	unsigned short 	full_rtime;
	unsigned short 	full_rtime_cc;
	signed short	full_dT;
	signed short	full_dT_cc;
	def_fg_lim		full_lim;

	// 	SOC PACK
	unsigned short	soc_pack;
	unsigned short 	soc_pack_unavbl;

	// -------------------
	// 		LEARNINGS
	// -------------------

	//	IDIS_AVG
	unsigned short	idis_avg;
	def_fg_lrn		idis_avg_lrn;
	def_fg_rslt		idis_avg_rslt;

	//	IDIS_END
	unsigned short	idis_end;
	def_fg_lrn		idis_end_lrn;
	def_fg_rslt		idis_end_rslt;

	//	ICHG_CC
	unsigned short	ichg_cc;
	def_fg_lrn		ichg_cc_lrn;
	def_fg_rslt		ichg_cc_rslt;

	//	ICHG_END
	unsigned short	ichg_end;
	def_fg_lrn		ichg_end_lrn;
	def_fg_rslt		ichg_end_rslt;

	//	VCHG_CV
	unsigned short	vchg_cv;
	def_fg_lrn		vchg_cv_lrn;
	def_fg_rslt		vchg_cv_rslt;

	//	ESR
	float		 	soresr_dis_cells[SETMAX_NCELLS_SER];
	float		 	soresr_chg_cells[SETMAX_NCELLS_SER];
	def_fg_lrn		soresr_lrn_cells[SETMAX_NCELLS_SER];
	def_fg_rslt		soresr_rslt_cells[SETMAX_NCELLS_SER];

	// RCXN_CELLS
	unsigned short 	rcxn_cells[SETMAX_NCELLS_SER];
	def_fg_lrn		rcxn_lrn_cells[SETMAX_NCELLS_SER];
	def_fg_rslt		rcxn_rslt_cells[SETMAX_NCELLS_SER];

	// HCONV
	unsigned short	hconv;
	def_fg_lrn		hconv_lrn;
	def_fg_rslt		hconv_rslt;

	// SOH
	unsigned long 	soh_pack;
	unsigned long 	soh_cells[SETMAX_NCELLS_SER];

	// -------------------
	// 		POWER
	// -------------------

	// POWER ESTIMATION
	unsigned long	prdg;
	unsigned long	pdis;
	unsigned char	pdis_id;
	def_fg_lim		pdis_lim;
	unsigned long	pchg;
	unsigned char	pchg_id;
	def_fg_lim		pchg_lim;

	// WARNINGS
	def_fg_OT		OT_warn;

	// OTHER
	unsigned long 	iteration;

}def_fg_out;
#define fg_out_size sizeof(def_fg_out)
	
typedef struct __attribute__((packed)){
	unsigned hard:			1;
	unsigned fg:				1;
	unsigned log:				1;
	unsigned wkup:			1;
	unsigned status:		1;
	unsigned soh:				1;
	unsigned RES00:			1;
	unsigned soc:				1;
	unsigned full:			1;
	unsigned emty:			1;
	unsigned idis_avg:	1;
	unsigned idis_end:	1;
	unsigned ichg_end:	1;
	unsigned ichg_cc:		1;
	unsigned vchg_cv:		1;
	unsigned soresr:		1;
	unsigned hconv:			1;
	unsigned RES01:				1;
}def_fg_rst;

typedef struct __attribute__((packed)){
	unsigned pchg:			1;
	unsigned pdis:			1;
}def_fg_shw;

typedef struct __attribute__((packed)) //74		74
{
	uint16_t 		Cells[SETMAX_NCELLS_SER];			//0x0000
	uint16_t 		VPack;
	int32_t 		Current[SETMAX_NCELLS_SER];		//0x0020
	int32_t 		PackCurrent;
	int32_t 		I_CC;
	int16_t			Tamb;
	int16_t 		TempVect[4];		//0x0040
	int16_t 		TChip;
	uint16_t 		CellsBalancing;	//0x0044
	def_fg_rst 	rst;
	def_fg_shw	shw;
} Meas_REG;

#else 

typedef enum{
	CHARGE, DISCHARGE, REST
}def_fg_status;

typedef enum{
	Cell, Pack
}def_fg_lim;

typedef struct __attribute__((packed)){

	//	EMPTY
	unsigned char	emty_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	emty_id;
	unsigned short 	emty_rtime;
	def_fg_lim		emty_lim;

	//	FULL
	unsigned char	full_soc_cells[SETMAX_NCELLS_SER];
	unsigned char	full_id;
	unsigned short 	full_rtime;
	def_fg_lim		full_lim;

	//	IDIS_AVG 
	unsigned short	idis_avg;

	//	IDIS_END
	unsigned short	idis_end;

	//	ICHG_CC
	unsigned short	ichg_cc;

	//	ICHG_END
	unsigned short	ichg_end;

	//	VCHG_CV 
	unsigned short	vchg_cv;

	// 	SOC (CELL & PACK)
	unsigned short 	soc_pack_unavbl;
	unsigned short	soc_pack;

	unsigned char 	soc_abs_cells[SETMAX_NCELLS_SER];

	// SOH
	unsigned long 	soh_pack;
	unsigned long 	soh_cells[SETMAX_NCELLS_SER];

	// POWER ESTIMATION
	unsigned long	prdg;
	unsigned long	pdis;
	unsigned char	pdis_id;
	def_fg_lim		pdis_lim;
	unsigned long	pchg;
	unsigned char	pchg_id;
	def_fg_lim		pchg_lim;

	// OTHER (1 bytes)
	def_fg_status	status;
	unsigned long iteration;
}def_fg_out;
#define fg_out_size sizeof(def_fg_out)

typedef struct __attribute__((packed)){
	unsigned hard:			1;
	unsigned fg:			1;
	unsigned log:			1;
	unsigned wkup:			1;
	unsigned status:		1;
	unsigned soh:			1;
	unsigned RES00:			1;
	unsigned soc:			1;
	unsigned full:			1;
	unsigned emty:			1;
	unsigned idis_avg:		1;
	unsigned idis_end:		1;
	unsigned ichg_end:		1;
	unsigned ichg_cc:		1;
	unsigned vchg_cv:		1;
	unsigned RES01:			1;
}def_fg_rst;

typedef struct __attribute__((packed)){
	unsigned pchg:			1;
	unsigned pdis:			1;
	unsigned emty_rtime:	1;
	unsigned full_rtime:	1;
}def_fg_shw;

typedef struct __attribute__((packed)) //74		74
{
	uint16_t Cells[SETMAX_NCELLS_SER];			//0x0000
	uint16_t VPack;
	int32_t Current[SETMAX_NCELLS_SER];		//0x0020
	int32_t PackCurrent;
	int32_t I_CC;
	int16_t TempVect[4];		//0x0040
	int16_t TChip;
	uint16_t CellsBalancing;	//0x0044
	def_fg_rst rst;
	def_fg_shw shw;
} Meas_REG;

#endif

//I2C MPF4279x Functions
uint8_t I2C_MPF4279x_Write(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t *Data, uint8_t len, uint8_t Activate, uint8_t CRC_En);
uint8_t I2C_MPF4279x_Read(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t *Data, uint8_t len, uint8_t Activate, uint8_t CRC_En);
uint8_t MPF4279x_to_Active_Mode(I2C_TypeDef* I2Cx, uint8_t Address, uint16_t Register, uint8_t len, uint8_t no_stop);
