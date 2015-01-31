#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <asm/uaccess.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
#include <linux/gpio.h>
#define DEVICE_NAME "HMC5983"
#define HMC5983_MAJOR 250
#define SCL S5PV210_GPH3(2)
#define SDA S5PV210_GPH3(3)
#define uchar unsigned char
#define uint unsigned int
#define SlaveAddress 0x3c
typedef unsigned char BYTE;
typedef unsigned char WORD;
BYTE BUF[6];


int dis_data;

void Init_HMC5983(void);
void Single_Write_HMC5983(uchar REG_Address,uchar REG_data);
uchar Single_Read_HMC5983(uchar REG_Address);                   //单个读取内部寄存器数据
void  Multiple_Read_HMC5983(void);
//以下是模拟iic使用函数-------------

void HMC5983_Start(void);
void HMC5983_Stop(void);
void HMC5983_SendACK(int ack);
int  HMC5983_RecvACK(void);
void HMC5983_SendByte(BYTE dat);
BYTE HMC5983_RecvByte(void);
void HMC5983_ReadPage(void);
void HMC5983_WritePage(void);
//*********************************************************

//起始信号
/**************************************/
void HMC5983_Start(void)
{
    gpio_direction_output(SDA,1);                    //拉高数据线
    gpio_direction_output(SCL,1);                    //拉高时钟线
    udelay(5);                 //延时
    gpio_set_value(SDA,0);                    //产生下降沿
    udelay(5);                 //延时
    gpio_set_value(SCL,0);                    //
}

void HMC5983_Stop(void)
{
	gpio_direction_output(SDA,0);                    //拉低数据线
   	 gpio_set_value(SCL,1);  
	udelay(5);
	gpio_set_value(SDA,1);
	udelay(5);

}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void HMC5983_SendACK(int ack)
{
    gpio_direction_output(SDA,ack);                  //写应答信号
    gpio_set_value(SCL,1);                    //拉高时钟线
    udelay(5);                 //延时
    gpio_set_value(SCL,0);                  //拉低时钟线
    udelay(5);                 //延时

}

/**************************************
接收应答信号
**************************************/
int HMC5983_RecvACK(void)

{
	int CY;
    gpio_set_value(SCL,1);                  //拉高时钟线
    udelay(5);					 //延时
	gpio_direction_input(SDA);
   CY = gpio_get_value(SDA);                   //读应答信号

    gpio_set_value(SCL,0);                    //拉低时钟线
    udelay(5);                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void HMC5983_SendByte(BYTE dat)
{
    BYTE i;
	int tmp;
	int CY;
    for (i=0; i<8; i++)         //8位计数器
    {

        tmp=dat&0x80;
	if(tmp==0x80)
	{
		CY=1;	
	}
	else
	CY=0;
	dat <<= 1;              //移出数据的最高位
        gpio_direction_output(SDA,CY);               //送数据口
        gpio_set_value(SCL,1);                //拉高时钟线
        udelay(5);             //延时
        gpio_set_value(SCL,0);                //拉低时钟线
        udelay(5);             //延时
    }
    HMC5983_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
BYTE HMC5983_RecvByte(void)

{
    BYTE i;
    BYTE dat = 0;
	int j;
    gpio_direction_output(SDA,1);                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
       gpio_set_value(SCL,1);                //拉高时钟线
        udelay(5);             //延时
	gpio_direction_input(SDA);
	j=gpio_get_value(SDA);
        dat |= j;             //读数据               
        gpio_set_value(SCL,0);                //拉低时钟线
        udelay(5);             //延时
    }
    return dat;
}

void Single_Write_HMC5983(uchar REG_Address,uchar REG_data)
{
    HMC5983_Start();                  //起始信号
    HMC5983_SendByte(SlaveAddress);   //发送设备地址+写信号
    HMC5983_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf 
    HMC5983_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf
    HMC5983_Stop();                   //发送停止信号
}

//********单字节读取内部寄存器*************************
uchar Single_Read_HMC5983(uchar REG_Address)
{  uchar REG_data;
    HMC5983_Start();                          //起始信号
    HMC5983_SendByte(SlaveAddress);           //发送设备地址+写信号
    HMC5983_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    HMC5983_Start();                          //起始信号
    HMC5983_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=HMC5983_RecvByte();              //读出寄存器数据
	HMC5983_SendACK(1);   
	HMC5983_Stop();                           //停止信号
    return REG_data; 
}

//******************************************************
//
//连续读出HMC5883内部角度数据，地址范围0x3~0x5
//
//******************************************************
void Multiple_read_HMC5983(void)
{   uchar i;
    HMC5983_Start();                          //起始信号
    HMC5983_SendByte(SlaveAddress);           //发送设备地址+写信号
    HMC5983_SendByte(0x03);                   //发送存储单元地址，从0x3开始	
    HMC5983_Start();                          //起始信号
    HMC5983_SendByte(SlaveAddress+1);         //发送设备地址+读信号
	 for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = HMC5983_RecvByte();          //BUF[0]存储数据
	
        if (i == 5)
        {
           HMC5983_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          HMC5983_SendACK(0);                //回应ACK
       }
   }
    HMC5983_Stop();                          //停止信号
    mdelay(5);
}

//初始化HMC5883，根据需要请参考pdf进行修改****
void Init_HMC5983(void)
{
	uchar reg_dataA,reg_dataB,reg_dataC;
    Single_Write_HMC5983(0x02,0x00); 
	reg_dataA=Single_Read_HMC5983(0x0A);
	reg_dataB=Single_Read_HMC5983(0x0B);
	reg_dataC=Single_Read_HMC5983(0x0C);
	printk("%c,%c,%c",reg_dataA,reg_dataB,reg_dataC);
}

static long hmc5983_ioctl( struct file *file,unsigned int cmd,unsigned long arg)
{
return 0;
}

static int hmc5983_read(struct file *pFile, char  __user *pData, size_t count, loff_t *off )
{
//    unsigned char tmp[8];
	int ret,i;

     Multiple_read_HMC5983();
//    printk("the tmpk data is %d\n",tmp); 
	for(i=0;i<6;i++)
	{
    ret=copy_to_user(pData+i, &BUF[i], sizeof(BUF[i]));    //将读取得的DS18B20数值复制到>用户区  
	}
if(ret>0)
{
  printk("copy data failed\n");
  return -1;
}
return 0;
}

static struct file_operations hmc5983_fops = {
.owner = THIS_MODULE,
.unlocked_ioctl = hmc5983_ioctl,
.read = hmc5983_read,
};
static int __init hmc5983_init(void)
{

int ret;
ret = register_chrdev(HMC5983_MAJOR, DEVICE_NAME, &hmc5983_fops);
if (ret < 0) {
   printk(DEVICE_NAME " can't register major number\n");
   return ret;

}
  mdelay(65);
 Init_HMC5983();

printk(DEVICE_NAME " initialized\n");
return 0;
}


static void __exit hmc5983_exit(void)
{
unregister_chrdev(HMC5983_MAJOR, DEVICE_NAME);
printk(DEVICE_NAME " rmmodule\n");
}
module_init(hmc5983_init);
module_exit(hmc5983_exit);
MODULE_AUTHOR("wangside");             // 驱动程序的作者  
MODULE_DESCRIPTION("HMC5983 Driver");   // 一些描述信息  
MODULE_LICENSE("GPL");

