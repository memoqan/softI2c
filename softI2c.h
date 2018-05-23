/**************************************************************************************************
  Filename:       softI2c.h
  Revised:        $Date: 2018-05-21 08:38:22  $
  Revision:       $Revision: 00001 $

  Description:    GPIO port soft i2c
**************************************************************************************************/
#ifndef SOFTI2C_H
#define SOFTI2C_H

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_I2C_BASE		GPIO_A_BASE
#define I2C_SCL_PIN			GPIO_PIN_2
#define I2C_SDA_PIN			GPIO_PIN_3

#define CONFIG_SCL_OUT()	GPIOPinTypeGPIOOutput(GPIO_I2C_BASE, I2C_SCL_PIN)
#define CONFIG_SDA_OUT()	GPIOPinTypeGPIOOutput(GPIO_I2C_BASE, I2C_SDA_PIN)
#define CONFIG_SDA_IN()		GPIOPinTypeGPIOInput(GPIO_I2C_BASE, I2C_SDA_PIN)
#define READ_SDA_PIN()		GPIOPinRead(GPIO_I2C_BASE, I2C_SDA_PIN)

#define I2C_SCL_H()			GPIOPinWrite(GPIO_I2C_BASE,I2C_SCL_PIN, I2C_SCL_PIN)
#define I2C_SCL_L()			GPIOPinWrite(GPIO_I2C_BASE,I2C_SCL_PIN, 0)

#define I2C_SDA_H()			GPIOPinWrite(GPIO_I2C_BASE, I2C_SDA_PIN,I2C_SDA_PIN)
#define I2C_SDA_L()			GPIOPinWrite(GPIO_I2C_BASE, I2C_SDA_PIN,0)

void i2cInit(void);
int8 i2cSendString(uint8 slaveAddr,uint8 regAddr,uint8 *data,uint16 dataLen);
int8 i2cRecvString(uint8 slaveAddr,uint8 regAddr,uint8 *buff,uint16 buffSize);


#ifdef __cplusplus
}
#endif


#endif