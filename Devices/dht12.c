#include "dht12.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "delay.h"
#include "USART.h"

float Temprature, Humi;		//������ʪ�ȱ��� ���˱���Ϊȫ�ֱ���
u8 Sensor_AnswerFlag = 0;	//���崫������Ӧ��־
u8 Sensor_ErrorFlag = 0;  	//�����ȡ�����������־

//��ʾ����
unsigned char dht_tempr_arrary[10] = {"+00.0 `C"};
unsigned char dht_humi_arrary[10] = {"+00.0 %Rh"};


void change_value()
{
	if (Temprature < 0)
	{
		dht_tempr_arrary[0] = '-';
		Temprature = -Temprature;
	}
	
	dht_tempr_arrary[1] = ((unsigned char)Temprature / 10 % 10) + '0';
	dht_tempr_arrary[2] = ((unsigned char)Temprature % 10) + '0';
	dht_tempr_arrary[4] = ((unsigned char)(Temprature * 10) % 10) + '0';
	
	dht_humi_arrary[1] = ((unsigned char)Humi / 10 % 10) + '0';
	dht_humi_arrary[2] = ((unsigned char)Humi % 10) + '0';
	dht_humi_arrary[4] = ((unsigned char)(Humi * 10) % 10) + '0';	
}

void change_error()
{
	unsigned int i = 0;
	
	for (i = 0; dht_tempr_arrary[i] != '\0'; i++)
	{
		dht_tempr_arrary[i] = '%';
	}
	for (i = 0; dht_humi_arrary[i] != '\0'; i++)
	{
		dht_humi_arrary[i] = '%';
	}
}


//DHT12��ʼ��
void dht12_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	//PA11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;	//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}


/********************************************\
|* ���ܣ� �����������͵ĵ����ֽ�	        *|
\********************************************/
unsigned char DHT12_Rdata(void)
{
	u8 i;
	u16 j;
	u8 data=0,bit=0;
	
	for(i=0;i<8;i++)
	{
		while(!READ_SDA)//����ϴε͵�ƽ�Ƿ����
		{
			if(++j>=50000) //��ֹ������ѭ��
			{
				break;
			}
		}
		//��ʱMin=26us Max70us ��������"0" �ĸߵ�ƽ		 
		DelayUs(30);

		//�жϴ�������������λ
		bit=0;
		if(READ_SDA)
		{
			bit=1;
		}
		j=0;
		while(READ_SDA)	//�ȴ��ߵ�ƽ����
		{
			if(++j>=50000) //��ֹ������ѭ��
			{
				break;
			}		
		}
		data<<=1;
		data|=bit;
	}
	return data;
}


/********************************************\
|* ���ܣ�DHT12��ȡ��ʪ�Ⱥ���       *|
\********************************************/
//������Humi_H��ʪ�ȸ�λ��Humi_L��ʪ�ȵ�λ��Temp_H���¶ȸ�λ��Temp_L���¶ȵ�λ��Temp_CAL��У��λ
//���ݸ�ʽΪ��ʪ�ȸ�λ��ʪ��������+ʪ�ȵ�λ��ʪ��С����+�¶ȸ�λ���¶�������+�¶ȵ�λ���¶�С����+ У��λ
//У�飺У��λ=ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ
unsigned char DHT12_READ(void)
{
	u32 j;
	u8 Humi_H,Humi_L,Temp_H,Temp_L,Temp_CAL,temp;

	//����������ʼ�ź�
	SDA_OUT() //��Ϊ���ģʽ
	SEND_SDA=0;	//�������������ߣ�SDA������
	DelayMs(20);//����һ��ʱ�䣨����18ms���� ֪ͨ������׼������
	SEND_SDA=1;	 //�ͷ�����
	SDA_IN();	//��Ϊ����ģʽ���жϴ�������Ӧ�ź�
	DelayUs(30);//��ʱ30us

	Sensor_AnswerFlag=0;	//��������Ӧ��־
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(READ_SDA==0)
	{
		Sensor_AnswerFlag=1;	//�յ���ʼ�ź�

		j=0;
		while((!READ_SDA)) //�жϴӻ����� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	
		{
			if(++j>=500) //��ֹ������ѭ��
			{
				Sensor_ErrorFlag=1;
				break;
			}
		}

		j=0;
		while(READ_SDA)//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
		{
			if(++j>=800) //��ֹ������ѭ��
			{
				Sensor_ErrorFlag=1;
				break;
			}		
		}
		//��������
		Humi_H=DHT12_Rdata();
		Humi_L=DHT12_Rdata();
		Temp_H=DHT12_Rdata();	
		Temp_L=DHT12_Rdata();
		Temp_CAL=DHT12_Rdata();

		temp=(u8)(Humi_H+Humi_L+Temp_H+Temp_L);//ֻȡ��8λ

		if(Temp_CAL==temp)//���У��ɹ�����������
		{
			Humi=Humi_H*10+Humi_L; //ʪ��
	
			if(Temp_L&0X80)	//Ϊ���¶�
			{
				Temprature =0-(Temp_H*10+((Temp_L&0x7F)));
			}
			else   //Ϊ���¶�
			{
				Temprature=Temp_H*10+Temp_L;//Ϊ���¶�
			}
			//�ж������Ƿ񳬹����̣��¶ȣ�-20��~60�棬ʪ��20��RH~95��RH��
			if(Humi>950) 
			{
			  Humi=950;
			}
			if(Humi<200)
			{
				Humi =200;
			}
			if(Temprature>600)
			{
			  Temprature=600;
			}
			if(Temprature<-200)
			{
				Temprature = -200;
			}
			Temprature=Temprature/10;//����Ϊ�¶�ֵ
			Humi=Humi/10; //����Ϊʪ��ֵ
			
			change_value();
			
//			printf("\r\n�¶�Ϊ:  %.1f  ��\r\n", Temprature); //��ʾ�¶�
//			printf("ʪ��Ϊ:  %.1f  %%RH\r\n", Humi);//��ʾʪ��	
		}
		
		else
		{
			change_error();
//		 	printf("CAL Error!!\r\n");
//			printf("%d \r%d \r%d \r%d \r%d \r%d \r\n",Humi_H,Humi_L,Temp_H,Temp_L,Temp_CAL,temp);
		}
	}
	else
	{
//		change_error();
//		Sensor_ErrorFlag=0;  //δ�յ���������Ӧ
//		printf("Sensor Error!!\r\n");
	}

	return 1;
}
