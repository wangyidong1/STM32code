#ifndef _modbus_h_
#define _modbus_h_
//#include "stm32f10x.h"
#include "modbus_crc.h"
#include "main.h"

 

typedef struct 
{
	//��Ϊ�ӻ�ʱʹ��
  uint8_t  myadd;        //���豸�ӻ���ַ
	uint8_t  rcbuf[100];   //modbus���ܻ�����
	uint8_t  timout;       //modbus���ݳ���ʱ��
	uint8_t  recount;      //modbus�˿ڽ��յ������ݸ���
	uint8_t  timrun;       //modbus��ʱ���Ƿ��ʱ��־
	uint8_t  reflag;       //modbusһ֡���ݽ�����ɱ�־λ
	uint8_t  sendbuf[100]; //modbus�ӷ��ͻ�����
	
	//��Ϊ������Ӳ���
	uint8_t Host_Txbuf[8];	//modbus��������	
	uint8_t slave_add;		//Ҫƥ��Ĵӻ��豸��ַ��������ʵ��ʱʹ�ã�
	uint8_t Host_send_flag;//�����豸����������ϱ�־λ
	int Host_Sendtime;//������һ֡���ݺ�ʱ�����
	uint8_t Host_time_flag;//����ʱ�䵽��־λ��=1��ʾ����������ʱ����
	uint8_t Host_End;//�������ݺ������
}MODBUS;




extern MODBUS modbus;
extern uint16_t Reg[];
void Modbus_Init(void);
void Modbus_Func3(void);//���Ĵ�������
void Modbus_Func6(void);//��1���Ĵ�����д������
void Modbus_Func16(void);//������Ĵ�����д������
void Modbus_Event(void);



//void Host_send03(void);
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num);
void Host_RX(void);
//�������մӻ�����Ϣ���д���
void HOST_ModbusRX(void);
void RS485_Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num);



//�ӻ���������
void Host_Func6(void);
//��һ���Ĵ�����д���ݵĲ�������
void Host_write06_slave(uint8_t slave,uint8_t fun,uint16_t StartAddr,uint16_t num);

#endif

