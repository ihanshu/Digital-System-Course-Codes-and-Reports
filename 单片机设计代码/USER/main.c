#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "lsens.h"
#include "beep.h"
#include "remote.h"
#include "timer.h"

#define THRESHOLD 50     //设定光照强度感应的阈值
#define WARN_TEMP 5000   //设定温度报警的感应阈值
 int main(void)
 {
 	vu8 key=0;	         //接受按键
	u8 reKey;            //接受遥控
	int autoFlag;        //模式标志
	int inFlag;          //输入模式
	u8 light;            //读取光照强度
	short temp;          //读取温度 
	u16 brightness;
	
	uart_init(115200);	 	//串口初始化为115200
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	POINT_COLOR=RED;      //设置字体为红色*/
	delay_init();	    	 //延时函数初始化	  
 	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
	BEEP_Init();         //初始化蜂鸣器端口
  LCD_Init();				    //初始化LCD
	Lsens_Init(); 			  //初始化光敏传感器
	BEEP_Init();         	//初始化蜂鸣器端口（温控报警器）
	T_Adc_Init();		  		//ADC初始化	 	
	Remote_Init();			  //红外接收初始化	
	
	 
  autoFlag=1;           //模式设定指标（初始为自动模式）
	inFlag=1;             //默认为输入模式按键
	LED0=0;               //0号LED灯作为自动模式状态显示灯（初始亮起）
	LED1=1;               //1号LED灯作为照明灯（初始关闭）
	BEEP=0;               //蜂鸣器作为温控报警器（初始为不鸣响）
	brightness=0;
	
	//显示字幕信息											      
	LCD_ShowString(30,30,200,16,16,"Automatic Streetlight");	
	LCD_ShowString(30,50,200,16,16,"KEY0:automatic/manual mode");
	LCD_ShowString(30,70,200,16,16,"KEY1:adjust manually");
	LCD_ShowString(30,90,200,16,16,"2023/11/13 Han Yichen");	  
	POINT_COLOR=BLUE;
	LCD_ShowString(30,130,200,16,16,"LSENS_VAL:");	
  LCD_ShowString(30,150,200,16,16,"TEMPERATE: 00.00C");	 
	LCD_ShowString(30,210,200,16,16,"KEY INPUT    ");
	
	while(1)
	{
		//接受传感器的数值
		light=Lsens_Get_Val();
		temp=Get_Temprate();
		
		//显示光照强度、温度以及模式
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+10*8,150,16,16,16,"-");	//显示负号
		}else LCD_ShowString(30+10*8,150,16,16,16," ");	//无符号		
		LCD_ShowxNum(30+11*8,150,temp/100,2,16,0);		//显示整数部分
		LCD_ShowxNum(30+14*8,150,temp%100,2,16, 0X80);	//显示小数部分
		
		LCD_ShowxNum(30+10*8,130,light,3,16,0);    
		
		//识别模式
		if(autoFlag==1){
			LCD_ShowString(30,190,280,16,16,"Mode: Automatic");	
			LED0=0;
			if(light<=THRESHOLD){
				LED1=0;
			}
			else{
				LED1=1;
			}
		}
		else if(autoFlag==0){
			LCD_ShowString(30,190,280,16,16,"Mode: Manual    ");
			LED0=1;
		}
		
		 
		//温度报警
		 if (temp > WARN_TEMP) {       
            LED1 = 0;
            autoFlag = 0;
            LCD_ShowString(30, 190, 280, 16, 16, "Temperature: WARNING");
            BEEP = 1;
            delay_ms(300);
						BEEP = 0;
        } 
		 else{
			     BEEP=0;
		 }
		
		
		 if(inFlag==1){
 		key=KEY_Scan(0);	//得到键值
	   	if(key)
		{						   
			switch(key)
			{			
				case KEY2_PRES:
					inFlag=0; 
			    LCD_ShowString(30, 210, 300, 16, 16, "REMOTE INPUT");
				  delay_ms(20);
				  break;
				case KEY1_PRES:	
					 if(autoFlag==0){
            LED1 = !LED1;
            delay_ms(20);
            break;
						}
						else if(autoFlag==1)
						delay_ms(20);
   					break;
					
				case KEY0_PRES:	
					autoFlag = !autoFlag;
          LED0 = !LED0;
          delay_ms(20);
				  break;
					
			}
		}else delay_ms(10); 
	}	 
	
	else{
	reKey=Remote_Scan();        //用于接收遥控器信号	
		if(reKey)
		{	 		  
			switch(reKey)
			{	    
				case 152:	
					 if(autoFlag==0){
            LED1 = !LED1;
            delay_ms(200);
            break;
						}
						else if(autoFlag==1)
						delay_ms(200);
   					break;
					
				case 104:	
					autoFlag = !autoFlag;
          LED0 = !LED0;
          delay_ms(200);
				  break;
				
				case 176:
				  inFlag=1; 
			    LCD_ShowString(30, 210, 300, 16, 16, "KEY INPUT    ");
				  delay_ms(200);
			    break;
			}
     }
   }
	}

}
