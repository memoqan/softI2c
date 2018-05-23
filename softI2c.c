/**************************************************************************************************
  Filename:       softI2c.c
  Revised:        $Date: 2018-05-21 08:38:22  $
  Revision:       $Revision: 00001 $

  Description:    
**************************************************************************************************/

#include "gpio.h"
#include "softI2c.h"

/*********************************************************************
 * @fn         i2cInit
 *
 * @brief      Config I2C clock,pins,pins input and output,io clock 
 *            
 *       
 * @param     none 
 *
 * @return     none
 */
void i2cInit(void)
{	
	CONFIG_SCL_OUT();
	CONFIG_SDA_OUT();

	I2C_SDA_H();
    I2C_SCL_H();
}


/**************************************************************************************
* FunctionName   : Start_I2c()
* Description    : I2C start bit
* EntryParameter : none
* ReturnValue    : none
**************************************************************************************/
static void i2c_start(void)
{
    I2C_SDA_H();   //发送起始条件的数据信号
    I2C_SCL_H();
    //起始条件建立时间大于4.7us,延时
    Delay_us(10);

    I2C_SDA_L();     //发送起始信号
    //起始条件建立时间大于4.7us,延时
    Delay_us(10);

    I2C_SCL_L();    //钳住I2C总线，准备发送或接收数据
}

/**************************************************************************************
* FunctionName   : i2c_restart()
* Description    : I2C start bit
* EntryParameter : none
* ReturnValue    : none
**************************************************************************************/
//static void i2c_restart(void)
//{
//		I2C_SCL_L();
//		Delay_us(10);
//    I2C_SDA_H();   //发送起始条件的数据信号
//		Delay_us(10);
//    I2C_SCL_H();
//    //起始条件建立时间大于4.7us,延时
//    Delay_us(10);
//
//    I2C_SDA_L();     //发送起始信号
//    //起始条件建立时间大于4.7us,延时
//    Delay_us(10);
//
//    I2C_SCL_L();    //钳住I2C总线，准备发送或接收数据
//}


/**************************************************************************************
* FunctionName   : Stop_I2c()
* Description    : stop bit
* EntryParameter : none
* ReturnValue    : None
**************************************************************************************/
static void i2c_stop(void)
{
    I2C_SDA_L();    //发送结束条件的数据信号
    I2C_SCL_H();    //结束条件建立时间大于4μ
    Delay_us(10);

    I2C_SDA_H();    //发送I2C总线结束信号
    Delay_us(10);
}

/**************************************************************************************
* FunctionName   : I2C_SendACK()
* Description    : 发送应答
* EntryParameter : 0：ack,1:noAck
* ReturnValue    : None
**************************************************************************************/
static void i2c_send_ack(uint8 ack)
{
    if(ack)
		{
			I2C_SDA_H();
		}
		else
		{
			I2C_SDA_L();
		}
    I2C_SCL_H();          //拉高时钟
    Delay_us(10);

    I2C_SCL_L();    //发送I2C总线结束信号
    Delay_us(10);
}

/**************************************************************************************
* FunctionName   : SendByte()
* Description    : send 1 byte
* EntryParameter : send byte
* ReturnValue    : None
**************************************************************************************/
static void  i2c_send_byte(uint8 byte)
{
    uint8 BitCnt, time_out = 200;

    for(BitCnt = 0; BitCnt < 8; BitCnt++) //要传送的数据长度为8位
    {
        if((byte << BitCnt) & 0x80)         //判断发送位
        {
            I2C_SDA_H();
        }
        else
        {
            I2C_SDA_L();
        }

        I2C_SCL_H();               //置时钟线为高，通知被控器开始接收数据位
        //保证时钟高电平周期大于4μ
        Delay_us(10);

        I2C_SCL_L();
        Delay_us(10);
    }
	
	CONFIG_SDA_IN();
    Delay_us(5);
    I2C_SCL_H();
    Delay_us(5);
    while(READ_SDA_PIN() == 0x01)
    {
        if(time_out--)
        {
            break;
        }

        Delay_us(5);
    }

    I2C_SCL_L();
    Delay_us(5);
	CONFIG_SDA_OUT();
}

/**************************************************************************************
* FunctionName   : I2C_RcvByte
* Description    : recive 1 byte
* EntryParameter : none
* ReturnValue    : recive byte
**************************************************************************************/
static uint8 i2c_recv_byte(void)
{
    uint8 retc = 0;

    CONFIG_SDA_IN();              //置数据线为输入方式

    for(uint8 i = 0; i < 8; i++)
    {
        I2C_SCL_H();       //置时钟线为高使数据线上数据有效
        Delay_us(5);
				
		retc <<= 1;

        if(READ_SDA_PIN() == 0x40)
        {
            retc |= 0x01; //读数据位,接收的数据位放入retc中
        }

        I2C_SCL_L();       //置时钟线为低，准备接收数据位
        Delay_us(10);			//时钟低电平周期大于4.7us
    }

    CONFIG_SDA_OUT();
    Delay_us(10);

    return retc;
}

/**************************************************************************************
* FunctionName   : i2cSendString
* Description    : string send
* EntryParameter : slaveAddr:slave device I2C addr,string:send string,len:string lenth
* ReturnValue    : -1:fail,0:success
**************************************************************************************/
int8 i2cSendString(uint8 slaveAddr,uint8 regAddr,uint8 *data,uint16 dataLen)
{
    if(dataLen == 0 || data == NULL)
    {
        return -1;
    }

    i2c_start();               	           //启动总线
    i2c_send_byte(slaveAddr << 1);       //发送器件地址
	
	i2c_send_byte(regAddr);

    for(uint8 cnt = 0; cnt < dataLen; cnt++)
    {
        i2c_send_byte(data[cnt]);
    }

    i2c_stop();                  //结束总线

    return 0;
}

/**************************************************************************************
* FunctionName   : i2cRecvString
* Description    : string recive
* EntryParameter : slave_addr:slave device I2C addr,string:recive string,len:string lenth
* ReturnValue    : -1:fail,not -1:success
**************************************************************************************/
int8 i2cRecvString(uint8 slaveAddr,uint8 regAddr,uint8 *buff,uint16 buffSize)
{	
    if(buffSize == 0 || buff == NULL)
    {
        return -1;
    }

    i2c_start();                                 //启动总线
	i2c_send_byte(slaveAddr << 1);
	i2c_send_byte(regAddr);
	
	i2c_start(); 
    i2c_send_byte((slaveAddr << 1) + 1);                //发送器件地址

    for(uint8 i = 0; i < buffSize; i++)
    {
        buff[i] = i2c_recv_byte();                   //接收数据
        if(i == (buffSize - 1))
        {
            i2c_send_ack(1);
        }
        else
        {
            i2c_send_ack(0);                      //发送就答位
        }
    }

    i2c_stop();                                  //结束总线

    Delay_us(10);

    return 0;
}