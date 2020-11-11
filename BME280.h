/*
 * BME280.h
 *
 * Created: 09/11/2020 11:03:18
 *  Author: Alexey Ivanov
 *  BME280 driver 
 *  v1.0
 * GitHub: https://github.com/AlexAlexFr
 */ 


#ifndef BME280_H_
#define BME280_H_
#include <avr/io.h>
#include "i2c.h"

#ifndef F_CPU
#define F_CPU	16000000UL
#endif

#define BME280_I2C_CLK		400000							//define SCL I2C frequency, maximum 400kHz
#define BME280_I2C_BITRATE	((F_CPU/BME280_I2C_CLK)-16)/2	//calculation for TWBR register with prescale 1

/**
BME280 registers
*/

#define	BME280_HUM_LSB		0xFE
#define BME280_HUM_MSB		0xFD
#define BME280_TEMP_XLSB	0xFC
#define BME280_TEMP_LSB		0xFB
#define BME280_TEMP_MSB		0xFA
#define BME280_PRES_XLSB	0xF9
#define BME280_PRES_LSB		0xF8
#define BME280_PRES_MSB		0xF7

/************************************************************************/
/* CONFIG REGISTER                                                      */
/************************************************************************/
#define BME280_CONF_REG		0xF5
#define BME280_TSTDBY		5	//position in CONF_REG
//define standby time in ms
#define BME280_TSTDBY_01	0
#define BME280_TSTDBY_62	1
#define BME280_TSTDBY_125	2
#define BME280_TSTDBY_250	3
#define BME280_TSTDBY_500	4
#define BME280_TSTDBY_1000	5
#define BME280_TSTDBY_10	6
#define BME280_TSTDBY_20	7

#define BME280_FILTER		2 //position in CONF_REG
//define filter coef
#define BME280_FILTER_NO	0
#define BME280_FILTER_02	1
#define BME280_FILTER_04	2
#define BME280_FILTER_08	3
#define BME280_FILTER_16	4
/************************************************************************/
/* CONTROL REGISTER                                                     */
/************************************************************************/
#define BME280_CTRL_MES		0xF4
//Define oversampling                                         
#define BME280_OVSMPxNO		0	//no oversampling, output set to 0x800000
#define BME280_OVSMPx1		1
#define BME280_OVSMPx2		2
#define BME280_OVSMPx4		3
#define BME280_OVSMPx8		4
#define BME280_OVSMPx16		5
//Position in CTRL register of oversampling value
#define BME280_CTRL_T		5	//bits 7,6,5
#define BME280_CTRL_P		2	//bits 4,3,2
//Define modes
#define BME280_SLEEP_M		0
#define BME280_FORCED_M		1
#define BME280_NORMAL_M		3

#define BME280_STATUS		0xF3

#define BME280_CTRL_HUM		0xF2 //oversampling bits 2,1,0

#define BME280_RST_REG		0xE0
#define BME280_RST_VALUE	0xB6	//value to write 0xB6 to make a reset

#define BME280_ID			0xD0	//CHIPID = 0x60

#define BME280_I2C_ADR		0b11101100


signed long t_fine;			//Global variable needed for compensation calculations

/******************************
calibration parameters needed for compensation calculations
******************************/
unsigned int dig_T1;
signed int dig_T2;
signed int dig_T3;

unsigned int dig_P1;
signed int dig_P2;
signed int dig_P3;
signed int dig_P4;
signed int dig_P5;
signed int dig_P6;
signed int dig_P7;
signed int dig_P8;
signed int dig_P9;

unsigned char dig_H1;
signed int dig_H2;
unsigned char dig_H3;
signed int dig_H4;
signed int dig_H5;
signed char dig_H6;

/************************************************************************/
/* Initialize BME280.                                                   */
/* Set i2c bitrate, measures oversampling, standby,						*/
/* filter and sensor mode												*/
/************************************************************************/
void BME280_Init();

/************************************************************************/
/* Read sensor ID, for BME280 value is 0x60                             */
/************************************************************************/
char BME280_ReadID();

/************************************************************************/
/* Read ADC values from all ADC                                         */
/* Burst read of ADC regs from 0xF7 to 0xFE. Result -char array [8]		*/
/************************************************************************/
void BME280_BurstADC(char* cMeasure);

/************************************************************************/
/* Read ADC value of temperature sensor                                 */
/* BME280_TempComp() function must be used for temperature compensation */
/************************************************************************/
long BME280_ReadADCTemperature();

/************************************************************************/
/* Read value of one register											*/
/************************************************************************/
char BME280_ReadReg(char cAdr);

/************************************************************************/
/* Write register at address cAdr with value cData                      */
/************************************************************************/
void BME280_WriteReg(char cAdr, char cData);

/************************************************************************/
/* Write BME280 config register
cMode : 0 - SleepMode; 1 - Forced Mode, 2 - Normal Mode
cPresOVSMP, cTempOVSMP: see BME280_OVSMPxYY definition					*/
/************************************************************************/
void BME280_WriteCtrlMeasReg(char cMode, char cPresOVSMP, char cTempOVSMP);

/************************************************************************/
/* Write Ctrl_Hum register
cHumOVSMP: see BME280_OVSMPxYY definition								*/
/************************************************************************/
void BME280_WriteCtrlHumReg(char cHumOVSMP);

/************************************************************************/
/* Write Config register
cStandByTime - see BME280_TSTDBY_XX definition
cIIRFilter - see BME280_FILTER_XX definition							*/
/************************************************************************/
void BME280_WriteConfigReg(char cStandbyTime, char cIIRFilter);

/***********************************************************************/
/*Read Status Register												   */
/***********************************************************************/
char BME280_ReadStatus();

/************************************************************************/
/* Function for reading calibration values                            */
/************************************************************************/
static void BME280_ReadCalibrationData();

/************************************************************************/
/* Get integer and decimal values of temperature                        */
/* Function reads temperature ADC and gives final result				*/
/* if cTempInt=23 and cTempDec=56, temperature is 23.56 deg C			*/
/*----------------------------------------------------------------------*/
/* NOTA! not to be used for negative values of temperature				*/
/* function to be adapted with signed int for temperature integer part	*/ 
/************************************************************************/
void BME280_GetTemperature(char* cTempInt, char* cTempDec);

/************************************************************************/
/* Temperature compensation function                                    */
/* output is in 0.01 deg C. Output value of 2356 equals to 23.56 degC   */
/************************************************************************/
signed long BME280_TempComp(signed long slTempADC);

/************************************************************************/
/* Read ADC value of pressure sensor                                    */
/* BME280_PresComp() function must be used for temperature compensation */
/************************************************************************/
long BME280_ReadADCPressure();

/************************************************************************/
/* Read ADC value of humidity sensor                                    */
/* BME280_HumComp() function must be used for temperature compensation  */
/************************************************************************/
long BME280_ReadADCHumidity();

/************************************************************************/
/* Humidity compensation function                                       */
/* Returns humidity in %RH in Q22.10 format								*/
/* Output value of “47445” represents 47445/1024 = 46.33%RH				*/
/************************************************************************/
unsigned long BME280_HumComp(long slHumADC);


/************************************************************************/
/* Get integer and decimal values of humidity	                        */
/* Function reads humidity ADC and gives final result					*/
/* if cHumInt=51 and cHulDec=48, humidity is 51.48 %RH					*/
/************************************************************************/
void BME280_GetHumidity(char* cHumInt, char* cHumDec);

/************************************************************************/
/* Pressure compensation function                                       */
/* Returns pressure in Pa as unsigned 32 bit integer					*/
/* Output value of “96386” represents									*/
/* 96386 Pa = 963.86 hPa												*/
/************************************************************************/
unsigned long BME280_PresComp(signed long slPresADC);

/************************************************************************/
/* Get integer and decimal values of pressure	                        */
/* Function reads pressure ADC and gives final result					*/
/* if iPresInt=1011 and iPresDec=485, pressure is 1011.485 hPa			*/
/************************************************************************/
void BME280_GetPressure(int* iPresInt, int* iPresDec);

#endif /* BME280_H_ */