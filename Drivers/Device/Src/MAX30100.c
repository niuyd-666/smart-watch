/**
  ******************************************************************************
  * @file    MAX30100.c
  * @author  Deson
  * @version V1.0.0
  * @date    2019-12-28
  * @brief   传感器基础驱动程序
  ******************************************************************************
  */
/*--Include-start-------------------------------------------------------------*/
#include "MAX30100.h"
#include "IICcom.h"



/*Global data space ----------------------------------------------------------*/

uint8_t max30100_Bus_Write(uint8_t Register_Address, uint8_t Word_Data,uint8_t address)
{

    /* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    I2C_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(address | I2C_WR);	/* 此处是写指令 */

    /* 第3步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址 */
    I2C_SendByte(Register_Address);
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第5步：开始写入数据 */
    I2C_SendByte(Word_Data);

    /* 第6步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }
//	printf()

    /* 发送I2C总线停止信号 */
    I2C_Stop();
//	printf("成功");
    return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    I2C_Stop();
//	printf("失败2");
    return 0;
}


uint8_t max30100_Bus_Read(uint8_t Register_Address,uint8_t address)
{
    uint8_t  data;


    /* 第1步：发起I2C总线启动信号 */
    I2C_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(address | I2C_WR);	/* 此处是写指令 */

    /* 第3步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
//		printf("找不到模块");
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    I2C_SendByte((uint8_t)Register_Address);
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }


    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    I2C_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(address | I2C_RD);	/* 此处是读指令 */

    /* 第8步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    {
        data = I2C_RadeByte();	/* 读1个字节 */

        I2C_NoAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
    }
    /* 发送I2C总线停止信号 */
    I2C_Stop();
//	printf("成功");
    return data;	/* 执行成功 返回data值 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
//	printf("失败1");
    I2C_Stop();
    return 0;
}

void max30100_FIFO_Read(uint8_t Register_Address,uint16_t  Word_Data[][2],uint8_t count)
{
    uint8_t i=0;
    uint8_t no = count;
    uint16_t data1, data2,data3;
    uint16_t data4,data5,data6;
    /* 第1步：发起I2C总线启动信号 */
    I2C_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(max30100_WR_address | I2C_WR);	/* 此处是写指令 */

    /* 第3步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    I2C_SendByte((uint8_t)Register_Address);
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }


    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    I2C_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(max30100_WR_address | I2C_RD);	/* 此处是读指令 */

    /* 第8步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    while (no)
    {
        data1 = I2C_RadeByte();
        I2C_Ack();
        data2 = I2C_RadeByte();
        I2C_Ack();
        data3 = I2C_RadeByte();
        I2C_Ack();
        data4 = I2C_RadeByte();
        I2C_Ack();
        data5 = I2C_RadeByte();
        I2C_Ack();
        data6= I2C_RadeByte();
        data1<<=14;
        data2<<=6;
        data3>>=2;
        data4<<=14;
        data5<<=6;
        data6>>=2;
        Word_Data[i][0]= data1+data2+data3;
        Word_Data[i][1]= data4+data5+data6;

        if(1==no)
            I2C_NoAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        else
            I2C_Ack();

        no--;
        i++;
    }
    /* 发送I2C总线停止信号 */
    I2C_Stop();

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    I2C_Stop();
}

void max30100_init(void)
{
	max30100_Bus_Write(REG_INTR_ENABLE_1,0xc0,MAX30102_Device_address);//// INTR setting
//	  printf("REG_INTR_ENABLE_1:0x%X \r\n",ui8Status2);

	max30100_Bus_Write(REG_INTR_ENABLE_2,0x00,MAX30102_Device_address);//
	max30100_Bus_Write(REG_FIFO_WR_PTR,0x00,MAX30102_Device_address);//FIFO_WR_PTR[4:0]
	max30100_Bus_Write(REG_OVF_COUNTER,0x00,MAX30102_Device_address);//OVF_COUNTER[4:0]
	max30100_Bus_Write(REG_FIFO_RD_PTR,0x00,MAX30102_Device_address);//FIFO_RD_PTR[4:0]
	max30100_Bus_Write(REG_FIFO_CONFIG,0x0f,MAX30102_Device_address);//sample avg = 1, fifo rollover=false, fifo almost full = 17
	max30100_Bus_Write(REG_MODE_CONFIG,0x03,MAX30102_Device_address);//0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	max30100_Bus_Write(REG_SPO2_CONFIG,0x23,MAX30102_Device_address);	// SPO2_ADC range = 4096nA, SPO2 sample rate (50 Hz), LED pulseWidth (400uS)  
//	max30100_Bus_Write(REG_LED1_PA,0x7f,MAX30102_Device_address);//Choose value for ~ 10mA for LED1
//	max30100_Bus_Write(REG_LED2_PA,0x33,MAX30102_Device_address);// Choose value for ~ 10mA for LED2
	max30100_Bus_Write(REG_PILOT_PA,0x7f,MAX30102_Device_address);// Choose value for ~ 25mA for Pilot LED
}
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i,m;
    uint16_t usAddr;

    /*
    	写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
    	对于24xx02，page size = 8
    	简单的处理方法为：按字节写操作模式，没写1个字节，都发送地址
    	为了提高连续写的效率: 本函数采用page wirte操作。
    */

    usAddr = _usAddress;
    for (i = 0; i < _usSize; i++)
    {
        /* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
        if ((i == 0) || (usAddr & (7)) == 0)
        {
            /*　第０步：发停止信号，启动内部写操作　*/
            I2C_Stop();

            /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
            	CLK频率为200KHz时，查询次数为30次左右
            */
            for (m = 0; m < 1000; m++)
            {
                /* 第1步：发起I2C总线启动信号 */
                I2C_Start();

                /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
                I2C_SendByte(EEPROM_DEV_ADDR | 0);	/* 此处是写指令 */

                /* 第3步：发送一个时钟，判断器件是否正确应答 */
                if (I2C_WaitAck() == 0)
                {
                    break;
                }
            }
            if (m  == 1000)
            {
                goto cmd_fail;	/* EEPROM器件写超时 */
            }

            /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
            I2C_SendByte((uint8_t)usAddr);

            /* 第5步：等待ACK */
            if (I2C_WaitAck() != 0)
            {
                goto cmd_fail;	/* EEPROM器件无应答 */
            }
        }

        /* 第6步：开始写入数据 */
        I2C_SendByte(_pWriteBuf[i]);

        /* 第7步：发送ACK */
        if (I2C_WaitAck() != 0)
        {
            goto cmd_fail;	/* EEPROM器件无应答 */
        }

        usAddr++;	/* 地址增1 */
    }

    /* 命令执行成功，发送I2C总线停止信号 */
    I2C_Stop();
    return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    I2C_Stop();
    return 0;
}
/*
*********************************************************************************************************
*	函 数 名: ee_ReadBytes
*	功能说明: 从串行EEPROM指定地址处开始读取若干数据
*	形    参：_usAddress : 起始地址
*			 _usSize : 数据长度，单位为字节
*			 _pReadBuf : 存放读到的数据的缓冲区指针
*	返 回 值: 0 表示失败，1表示成功
*********************************************************************************************************
*/
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i;

    /* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    I2C_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(EEPROM_DEV_ADDR | 0);	/* 此处是写指令 */

    /* 第3步：等待ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
    I2C_SendByte((uint8_t)_usAddress);

    /* 第5步：等待ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */
    I2C_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    I2C_SendByte(EEPROM_DEV_ADDR | 1);	/* 此处是读指令 */

    /* 第8步：发送ACK */
    if (I2C_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第9步：循环读取数据 */
    for (i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = I2C_RadeByte();	/* 读1个字节 */

        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if (i != _usSize - 1)
        {
            I2C_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            I2C_NoAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
    }
    /* 发送I2C总线停止信号 */
    I2C_Stop();
    return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    I2C_Stop();
    return 0;
}
