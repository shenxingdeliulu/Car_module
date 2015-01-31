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
#define DEVICE_NAME "DS18B20"  
#define DS18B20_MAJOR 250  
#define DS_PIN S5PV210_GPH2(0)  
#define OUT S3C_GPIO_OUTPUT  
#define IN S3C_GPIO_INPUT  
#define DIS_UP 1  
#define EN_UP 0   
//#define Search 0x00F0  
#define S3C_GPIO_INPUT (S3C_GPIO_SPECIAL(0))
#define S3C_GPIO_OUTPUT (S3C_GPIO_SPECIAL(1))
#define Read_ROM 0x0033  //just for one  
#define Skip_ROM 0x00CC   //ds18b20的固定时序， 跳过读序号列号的操作 
#define Convert 0x0044  //ds18b20的固定时序，管脚识别到该信号是开始对测到的温度进行转换
#define Write 0x004E  //TH---TL---Config  
#define Read 0x00BE   //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
//#define bit_9 0x001F  
//#define bit_10 0x003F  
//#define bit_11 0x005F  
#define bit_12 0x007F  
#define uint16 unsigned int  
//unsigned int ROM_DATA[8];  
void usdelay(unsigned int i)


{  
unsigned int j;  
for(i=i;i>0;i--)  
  for(j=90;j>0;j--);  
}  
void msdelay(unsigned int i) //延时 i ms  
{  
for(i=i;i>0;i--)  
  usdelay(1000);  
}  
void SetL(void)  
{  
 gpio_direction_output(DS_PIN,0);  
}  
void SetH(void)  
{  
gpio_direction_output(DS_PIN,1); 
}  
unsigned int Read_DS(void)  
{  
unsigned int i;  
gpio_direction_input(DS_PIN);  
//gpio_set_value(DS_PIN,EN_UP);
  
__asm("nop");  
__asm("nop");  
__asm("nop");  
i=gpio_get_value(DS_PIN);  
if(i!=0)  
  i=1;  
return i;  
}  
unsigned int ds_start(void)  //初始化ds18b20
{  
unsigned int flag=1;
int err=0;
SetH();
udelay(2); 
SetL();  
udelay(600); //560延时要大于480u  
SetH();  
udelay(60);   //稍作延时
while(Read_DS()!=0)   //ds18B20初始化成功会返回一个低电平，此时跳出循环，执行下面的操作
{  
  printk(DEVICE_NAME "Wait....\n"); 
  udelay(5);  
  err++;   //初始化的最多次数吧,超过规定的次数，说明初始化失败
  if(err==20)  
  {  
   printk(DEVICE_NAME "start fail\n");  
   return -1;  
  }  
}  
//printk(DEVICE_NAME "start sucess\n");  
flag=0;  
SetH();//初始化成功后赋为高电平准备从外界读入温度  
udelay(400);  
return flag;  
}  
void ds_send(unsigned int uidata)   //向18b20写入一个字节的数据
{  
//printk("the send data is %d\n",uidata);
 int i;  
 for(i=0;i<8;i++)  
 {  
  SetL();
  udelay(1); 
  if((uidata&1)!=0)
   {    
    SetH();  
    udelay(80);    //等待18b20进行数据采集
   }  
  else 
   {  
    udelay(80);  //等待18b20进行数据采集  
    SetH();     
    }  
  uidata>>=1;  
 }  
}  
unsigned int ds_read(void)    //从18b20读一个字节的数据
{  
 unsigned int uidata=0;unsigned int i;  
 for(i=0;i<8;i++)  
  { 
   uidata>>=1; 
   SetL();  
   udelay(1);   
   gpio_set_value(DS_PIN,1);
   s3c_gpio_cfgpin(DS_PIN,IN);
   udelay(10);    
   if(gpio_get_value(DS_PIN))
       uidata=(uidata|0x80);         
   udelay(65); 
   SetH();
   }  
// printk("ds_read success\n");
 return uidata;   
}  
 
unsigned int read_tem(void)  
{  
unsigned int th,tl;
//int err=0;  
//ds_init(100,0,bit_12);
th=tl=0;   
ds_start();     
ds_send(Skip_ROM);   //跳过读序号列号的操作
ds_send(Convert);    //启动温度转换
mdelay(50);    
ds_start();  
ds_send(Skip_ROM);   //跳过读序号列号的操作
ds_send(Read);       //准备读温度
tl=ds_read();  
th=ds_read();      
th<<=8;             //温度在低两个字节中
//printk("the tl data is %d\n",tl);  
tl|=th;            //获取温度
//printk("the th data is %d\n",th);
//printk("the tl2 data is %d\n",tl);
//printk("read_tmp success\n"); 
return tl;  
}  
 
static int ds18b20_ioctl( struct file *file,   
unsigned int cmd,unsigned long arg)  
{  
return 0;  
}  
static int ds18b20_read(struct file *pFile, uint16 __user *pData, size_t count, loff_t *off )  
{  
    uint16 tmp,ret;  
      
    tmp =read_tem();
//    printk("the tmpk data is %d\n",tmp); 
    ret=copy_to_user(pData, &tmp, sizeof(tmp));    //将读取得的DS18B20数值复制到用户区  
if(ret>0)  
{  
  printk("copy data failed\n");  
  return -1;  
}
//else
 // printk("copy data succese\n"); 
return 0;  
}  
static struct file_operations ds18b20_fops = {  
.owner = THIS_MODULE,  
.unlocked_ioctl = ds18b20_ioctl,  
.read = ds18b20_read,  
};  
static int __init ds18b20_init(void)  
{  
int ret;  
ret = register_chrdev(DS18B20_MAJOR, DEVICE_NAME, &ds18b20_fops);  
if (ret < 0) {  
   printk(DEVICE_NAME " can't register major number\n");  
   return ret;  
}    
s3c_gpio_cfgpin(DS_PIN, OUT);  
gpio_set_value(DS_PIN, 1);  
printk(DEVICE_NAME " initialized\n");  
return 0;  
}  
static void __exit ds18b20_exit(void)  
{     
unregister_chrdev(DS18B20_MAJOR, DEVICE_NAME);  
printk(DEVICE_NAME " rmmodule\n");  
}  
module_init(ds18b20_init);  
module_exit(ds18b20_exit);  
MODULE_AUTHOR("benjamin_xc@163.com");             // 驱动程序的作者  
MODULE_DESCRIPTION("DS18B20 Driver");   // 一些描述信息  
MODULE_LICENSE("GPL"); 
