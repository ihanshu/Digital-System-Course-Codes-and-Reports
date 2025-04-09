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

#define THRESHOLD 50     //�趨����ǿ�ȸ�Ӧ����ֵ
#define WARN_TEMP 5000   //�趨�¶ȱ����ĸ�Ӧ��ֵ
 int main(void)
 {
 	vu8 key=0;	         //���ܰ���
	u8 reKey;            //����ң��
	int autoFlag;        //ģʽ��־
	int inFlag;          //����ģʽ
	u8 light;            //��ȡ����ǿ��
	short temp;          //��ȡ�¶� 
	u16 brightness;
	
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	POINT_COLOR=RED;      //��������Ϊ��ɫ*/
	delay_init();	    	 //��ʱ������ʼ��	  
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
	BEEP_Init();         //��ʼ���������˿�
  LCD_Init();				    //��ʼ��LCD
	Lsens_Init(); 			  //��ʼ������������
	BEEP_Init();         	//��ʼ���������˿ڣ��¿ر�������
	T_Adc_Init();		  		//ADC��ʼ��	 	
	Remote_Init();			  //������ճ�ʼ��	
	
	 
  autoFlag=1;           //ģʽ�趨ָ�꣨��ʼΪ�Զ�ģʽ��
	inFlag=1;             //Ĭ��Ϊ����ģʽ����
	LED0=0;               //0��LED����Ϊ�Զ�ģʽ״̬��ʾ�ƣ���ʼ����
	LED1=1;               //1��LED����Ϊ�����ƣ���ʼ�رգ�
	BEEP=0;               //��������Ϊ�¿ر���������ʼΪ�����죩
	brightness=0;
	
	//��ʾ��Ļ��Ϣ											      
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
		//���ܴ���������ֵ
		light=Lsens_Get_Val();
		temp=Get_Temprate();
		
		//��ʾ����ǿ�ȡ��¶��Լ�ģʽ
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+10*8,150,16,16,16,"-");	//��ʾ����
		}else LCD_ShowString(30+10*8,150,16,16,16," ");	//�޷���		
		LCD_ShowxNum(30+11*8,150,temp/100,2,16,0);		//��ʾ��������
		LCD_ShowxNum(30+14*8,150,temp%100,2,16, 0X80);	//��ʾС������
		
		LCD_ShowxNum(30+10*8,130,light,3,16,0);    
		
		//ʶ��ģʽ
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
		
		 
		//�¶ȱ���
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
 		key=KEY_Scan(0);	//�õ���ֵ
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
	reKey=Remote_Scan();        //���ڽ���ң�����ź�	
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
