/*
 * BME280.c
 *
 * Created: 09/11/2020 11:03:06
 *  Author: Alexey Ivanov
 *  BME280 driver
 v	1.0
 * GitHub: https://github.com/AlexAlexFr
 */ 


#include "BME280.h"


void BME280_Init()
{
	TWBR = BME280_I2C_BITRATE;
	TWSR = 0;
	TWCR |= (1<<TWEN);
	
	BME280_WriteCtrlHumReg(BME280_OVSMPx8);
	BME280_WriteCtrlMeasReg(BME280_NORMAL_M, BME280_OVSMPx8, BME280_OVSMPx8);
	BME280_WriteConfigReg(BME280_TSTDBY_500, BME280_FILTER_NO);
	
	BME280_ReadCalibrationData();
}

char BME280_ReadID()
{
	return BME280_ReadReg(BME280_ID);
}

char BME280_ReadReg(char cAdr)
{
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_WRITE);
	i2c_SendByte(cAdr);

	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_READ);
	char cResult = i2c_ReadByteNOACK();
	i2c_Stop();
	return cResult;
}

void BME280_BurstADC(char* cMeasure)
{
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_WRITE);
	i2c_SendByte(BME280_PRES_MSB);
	
	
	
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_READ);
	
	for (char cCount=0;cCount++;cCount<7)
	{
		cMeasure[cCount] = i2c_ReadByteACK();
	}
	cMeasure[7] = i2c_ReadByteNOACK();
	i2c_Stop();
	
}

long BME280_ReadADCTemperature()
{
	
	long lResult =0;
	char cTemp[4];
	
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_WRITE);
	i2c_SendByte(BME280_TEMP_MSB);
	
	i2c_Start();
	
	i2c_SendByte(BME280_I2C_ADR|I2C_READ);
	
	for (char cCount = 0; cCount<2; cCount++)
	{
		cTemp[cCount] = i2c_ReadByteACK();
	}
	cTemp[2] = i2c_ReadByteNOACK();
	i2c_Stop();
	
	long lMSB, lLSB, lXLSB;
	
	lMSB = cTemp[0];
	lMSB = lMSB << 12;
	lLSB = cTemp[1];
	lLSB = lLSB<<4;
	lXLSB = cTemp[2];
	lXLSB = lXLSB>>4;
	
	lResult = lMSB|lLSB|lXLSB;
	
	return lResult;
}

void BME280_WriteReg(char cAdr, char cData)
{
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_WRITE);
	i2c_SendByte(cAdr);
	i2c_SendByte(cData);
	i2c_Stop();
}

void BME280_WriteCtrlMeasReg(char cMode, char cPresOVSMP, char cTempOVSMP)
{
	BME280_WriteReg(BME280_CTRL_MES,cMode|(cPresOVSMP<<BME280_CTRL_P)|(cTempOVSMP<<BME280_CTRL_T));
}

char BME280_ReadStatus()
{
	return BME280_ReadReg(BME280_STATUS);
}

void BME280_WriteCtrlHumReg(char cHumOVSMP)
{
	BME280_WriteReg(BME280_CTRL_HUM, cHumOVSMP);
}

void BME280_WriteConfigReg(char cStandbyTime, char cIIRFilter)
{
	BME280_WriteReg(BME280_CONF_REG, (cStandbyTime<<BME280_TSTDBY)|(cIIRFilter<<BME280_FILTER));
}

static void BME280_ReadCalibrationData()
{
	char cDigT[24];
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_WRITE);
	i2c_SendByte(0x88);
	
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_READ);
	
	for (char c=0; c<23; c++)
	{
		cDigT[c] = i2c_ReadByteACK();
	}
	cDigT[23] = i2c_ReadByteNOACK();
	i2c_Stop();
	
	dig_T1 = cDigT[1];
	dig_T1 = (dig_T1<<8)|cDigT[0];
	
	dig_T2 = cDigT[3];
	dig_T2 = (dig_T2<<8)|cDigT[2];
	
	dig_T3 = cDigT[5];
	dig_T3 = (dig_T3<<8)|cDigT[4];
	
	dig_P1 = cDigT[7];
	dig_P1 = (dig_P1<<8)|cDigT[6];
	
	dig_P2 = cDigT[9];
	dig_P2 = (dig_P2<<8)|cDigT[8];
	
	dig_P3 = cDigT[11];
	dig_P3 = (dig_P3<<8)|cDigT[10];
	
	dig_P4 = cDigT[13];
	dig_P4 = (dig_P4<<8)|cDigT[12];
	
	dig_P5 = cDigT[15];
	dig_P5 = (dig_P5<<8)|cDigT[14];
	
	dig_P6 = cDigT[17];
	dig_P6 = (dig_P6<<8)|cDigT[16];
	
	dig_P7 = cDigT[19];
	dig_P7 = (dig_P7<<8)|cDigT[18];
	
	dig_P8 = cDigT[21];
	dig_P8 = (dig_P8<<8)|cDigT[20];
	
	dig_P9 = cDigT[23];
	dig_P9 = (dig_P9<<8)|cDigT[22];
	
	dig_H1 = BME280_ReadReg(0xA1);
	
	
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_WRITE);
	i2c_SendByte(0xE1);
		
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_READ);
		
	for (char c=0;c<6;c++)
	{
		cDigT[c] = i2c_ReadByteACK();
	}
	
	cDigT[6] = i2c_ReadByteNOACK();
	i2c_Stop();
	
	dig_H2 = cDigT[1];
	dig_H2 = (dig_H2<<8)|cDigT[0];
	
	dig_H3 = cDigT[2];
	
	dig_H4 = cDigT[3];
	dig_H4 = (dig_H4<<4)|(cDigT[4]&0x0F);
	
	dig_H5 = cDigT[5];
	dig_H5 = (dig_H5<<4)|((cDigT[4]&0xF0)>>4);
	
	dig_H6 = cDigT[6];

}

void BME280_GetTemperature(char* cTempInt, char* cTempDec)
{
	long lTemp = BME280_ReadADCTemperature();
	signed long lTempComp;
	lTempComp = BME280_TempComp(lTemp);
	*cTempInt = (char)(lTempComp / 100);
	*cTempDec = (char)(lTempComp - *cTempInt * 100);
}

signed long BME280_TempComp(signed long slTempADC)
{
		signed long var1, var2, T;
		
		var1 = ((((slTempADC>>3) - (dig_T1<<1))) * (dig_T2)) >> 11;
		var2 = (((((slTempADC>>4) - (dig_T1)) * ((slTempADC>>4) - (dig_T1))) >> 12) *dig_T3) >> 14;
		t_fine = var1 + var2;
		T = (t_fine * 5 + 128) >> 8;
		
		return T;
}

long BME280_ReadADCHumidity()
{
	char cHumMSB = BME280_ReadReg(BME280_HUM_MSB);
	char cHumLSB = BME280_ReadReg(BME280_HUM_LSB);
	
	long lHum = 0;
	lHum = cHumMSB;
	lHum = lHum<<8;
	lHum = lHum|cHumLSB;
	
	return lHum;
}

void BME280_GetHumidity(char* cHumInt, char* cHumDec)
{
	long lHum = BME280_ReadADCHumidity();
	signed long lHumComp = BME280_HumComp(lHum);
	*cHumInt = (char)(lHumComp/1024);
	*cHumDec = (char)((lHumComp - *cHumInt*1024)*1000/1024);
}

unsigned long BME280_HumComp(signed long slHumADC)
{
	signed long v_x1_u32r;

	v_x1_u32r = t_fine - 76800;

	v_x1_u32r = (((((slHumADC << 14) - ((signed long)(dig_H4) << 20) - ((signed long)(dig_H5) * v_x1_u32r)) + 16384) >> 15) *(((((((v_x1_u32r * (signed long)(dig_H6)) >> 10) *
		(((v_x1_u32r * (signed long)(dig_H3)) >> 11) + 32768)) >> 10) + 2097152) * (signed long)(dig_H2) + 8192) >> 14));

v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * (signed long)(dig_H1)) >>4));

v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;


	return (v_x1_u32r >>12);
}

long BME280_ReadADCPressure()
{
	
	long lResult =0;
	char cPres[4];
	
	i2c_Start();
	i2c_SendByte(BME280_I2C_ADR|I2C_WRITE);
	i2c_SendByte(BME280_PRES_MSB);
	
	i2c_Start();
	
	i2c_SendByte(BME280_I2C_ADR|I2C_READ);
	
	for (char cCount = 0; cCount<2; cCount++)
	{
		cPres[cCount] = i2c_ReadByteACK();
	}
	cPres[2] = i2c_ReadByteNOACK();
	i2c_Stop();
	
	long lMSB, lLSB, lXLSB;
	
	lMSB = cPres[0];
	lMSB = lMSB << 12;
	lLSB = cPres[1];
	lLSB = lLSB<<4;
	lXLSB = cPres[2];
	lXLSB = lXLSB>>4;
	
	lResult = lMSB|lLSB|lXLSB;
	
	return lResult;
}

unsigned long BME280_PresComp(signed long slPresADC)
{
	signed long var1, var2;
	unsigned long p;
	
	var1 = (((signed long)t_fine)>>1) - (signed long)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((signed long)dig_P6);
	var2 = var2 + ((var1*((signed long)dig_P5))<<1);
	var2 = (var2>>2)+(((signed long)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((signed long)dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((signed long)dig_P1))>>15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((unsigned long)(((signed long)1048576)-slPresADC)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((unsigned long)var1);
	}
	else
	{
		p = (p / (unsigned long)var1) * 2;
	}
	var1 = (((signed long)dig_P9) * ((signed long)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((signed long)(p>>2)) * ((signed long)dig_P8))>>13;
	p = (unsigned long)((signed long)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}

void BME280_GetPressure(int* iPresInt, int* iPresDec)
{
	unsigned long ulPres = BME280_ReadADCPressure();
	unsigned long ulPresComp = BME280_PresComp(ulPres);
	
	long lPresInt = (ulPresComp);
		
	*iPresInt = (int)(lPresInt / 100);
	*iPresDec = (int)(lPresInt - *iPresInt*100);
	
}