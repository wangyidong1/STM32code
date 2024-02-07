#include "modbus.h"
#include "usart.h"

MODBUS modbus;//�ṹ�����





//��Ϊ�ӻ����ֵĴ���
 
uint16_t Reg[] ={0x0001,
            0x0002,
            0x0003,
            0x0004,
	          0x0005,
            0x0006,
            0x0007,
			0X0008,
           };//reg����ǰ����õļĴ����ͼĴ������ݣ�Ҫ��ȡ�͸�д�Ĳ�������


// Modbus��ʼ������
void Modbus_Init()
{
  modbus.myadd = 0x02; //�ӻ��豸��ַΪ1
  modbus.timrun = 0;    //modbus��ʱ��ֹͣ����
	modbus.slave_add=0x01;//����Ҫƥ��Ĵӻ���ַ
}
// Modbus 3�Ź����뺯��
// Modbus ������ȡ�Ĵ���ֵ
void Modbus_Func3()
{
  uint16_t Regadd,Reglen,crc;
	uint8_t i,j;	
	//�õ�Ҫ��ȡ�Ĵ������׵�ַ
	Regadd = modbus.rcbuf[2]*256+modbus.rcbuf[3];//��ȡ���׵�ַ
	//�õ�Ҫ��ȡ�Ĵ��������ݳ���
	Reglen = modbus.rcbuf[4]*256+modbus.rcbuf[5];//��ȡ�ļĴ�������
	//���ͻ�Ӧ���ݰ�
	i = 0;
	modbus.sendbuf[i++] = modbus.myadd;      //ID�ţ����ͱ����豸��ַ
	modbus.sendbuf[i++] = 0x03;              //���͹�����
  modbus.sendbuf[i++] = ((Reglen*2)%256);   //�����ֽڸ���
	for(j=0;j<Reglen;j++)                    //��������
	{
		//reg����ǰ����õ�16λ���飨ģ�¼Ĵ�����
	  modbus.sendbuf[i++] = Reg[Regadd+j]/256;//��λ����
		modbus.sendbuf[i++] = Reg[Regadd+j]%256;//��λ����
	}
	crc = Modbus_CRC16(modbus.sendbuf,i);    //����Ҫ�������ݵ�CRC
	modbus.sendbuf[i++] = crc/256;//У��λ��λ
	modbus.sendbuf[i++] = crc%256;//У��λ��λ
	//���ݰ�������
	// ��ʼ����Modbus����
	
	RS485_TX_ENABLE;//���ǿ���485����
	
	for(j=0;j<i;j++)//��������
	{
		printf("%d",modbus.sendbuf[j]);
	  Modbus_Send_Byte(modbus.sendbuf[j]);	
	}
	RS485_RX_ENABLE;//�����ǹر�485����
}


// Modbus 6�Ź����뺯��
// Modbus ����д��Ĵ���ֵ
void Modbus_Func6()  
{
  uint16_t Regadd;//��ַ16λ
	uint16_t val;//ֵ
	uint16_t i,crc,j;
	i=0;
  Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //�õ�Ҫ�޸ĵĵ�ַ 
	val=modbus.rcbuf[4]*256+modbus.rcbuf[5];     //�޸ĺ��ֵ��Ҫд������ݣ�
	Reg[Regadd]=val;  //�޸ı��豸��Ӧ�ļĴ���
	
	//����Ϊ��Ӧ����
	modbus.sendbuf[i++]=modbus.myadd;//���豸��ַ
  modbus.sendbuf[i++]=0x06;        //������ 
  modbus.sendbuf[i++]=Regadd/256;//д��ĵ�ַ
	modbus.sendbuf[i++]=Regadd%256;
	modbus.sendbuf[i++]=val/256;//д�����ֵ
	modbus.sendbuf[i++]=val%256;
	crc=Modbus_CRC16(modbus.sendbuf,i);//��ȡcrcУ��λ
	modbus.sendbuf[i++]=crc/256;  //crcУ��λ�������
	modbus.sendbuf[i++]=crc%256;
	//���ݷ��Ͱ�������
	RS485_TX_ENABLE;;//ʹ��485���ƶ�(��������)  
	for(j=0;j<i;j++)
	{
	 Modbus_Send_Byte(modbus.sendbuf[j]);
	}
	RS485_RX_ENABLE;//ʧ��485���ƶˣ���Ϊ���գ�

}

//����������Ĵ�������д������
//������0x10ָ�ʮ����16
void Modbus_Func16()
{
		uint16_t Regadd;//��ַ16λ
		uint16_t Reglen;
		uint16_t i,crc,j;
		
		Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //Ҫ�޸����ݵ���ʼ��ַ
		Reglen = modbus.rcbuf[4]*256+modbus.rcbuf[5];//��ȡ�ļĴ�������
		for(i=0;i<Reglen;i++)//���Ĵ�����д������
		{
			//��������ĵ���λ��ʼ������
			Reg[Regadd+i]=modbus.rcbuf[7+i*2]*256+modbus.rcbuf[8+i*2];//�ԼĴ���һ��д������
		}
		//д��������ϣ���������Ҫ���д���ظ�������
		
		//����Ϊ��Ӧ��������
		//����=���������ǰ6λ+��λ��У��λ
		modbus.sendbuf[0]=modbus.rcbuf[0];//���豸��ַ
		modbus.sendbuf[1]=modbus.rcbuf[1];  //������ 
		modbus.sendbuf[2]=modbus.rcbuf[2];//д��ĵ�ַ
		modbus.sendbuf[3]=modbus.rcbuf[3];
		modbus.sendbuf[4]=modbus.rcbuf[4];
		modbus.sendbuf[5]=modbus.rcbuf[5];
		crc=Modbus_CRC16(modbus.sendbuf,6);//��ȡcrcУ��λ
		modbus.sendbuf[6]=crc/256;  //crcУ��λ������� 
		modbus.sendbuf[7]=crc%256;
		//���ݷ��Ͱ�������
		
		RS485_TX_ENABLE;;//ʹ��485���ƶ�(��������)  
		for(j=0;j<8;j++)
		{
			Modbus_Send_Byte(modbus.sendbuf[j]);
		}
		RS485_RX_ENABLE;//ʧ��485���ƶˣ���Ϊ���գ�	
}


// Modbus�¼�������
void Modbus_Event()
{
	uint16_t crc,rccrc;//crc�ͽ��յ���crc
	//û���յ����ݰ�
  if(modbus.reflag == 0)  //�������δ����򷵻ؿ�
	{
		//printf("111");
	   return;
	}
	printf("000");
	//�յ����ݰ�(�������)			
	//ͨ������������֡����CRC
	//����1�������׵�ַ������2��Ҫ����ĳ��ȣ�����CRCУ��λ����ȫ�㣩
	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2); //��ȡCRCУ��λ
	// ��ȡ����֡��CRC
	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1];//�����ȡ��CRCУ��λ
	//�ȼ��������������
	//rccrc=modbus.rcbuf[modbus.recount-1]|(((uint16_t)modbus.rcbuf[modbus.recount-2])<<8);//��ȡ���յ���CRC
//	if(crc == rccrc) //CRC����ɹ� ��ʼ������
//	{	
	   if(modbus.rcbuf[0] == modbus.myadd)  // ����ַ�Ƿ�ʱ�Լ��ĵ�ַ
		 {
		   switch(modbus.rcbuf[1])   //����modbus������
			 {
			   case 0:             break;
				 case 1:             break;
				 case 2:             break;
				 case 3:      Modbus_Func3();      break;//���Ƕ�ȡ�Ĵ���������
				 case 4:             break;
				 case 5:             break;
         case 6:      Modbus_Func6();      break;//����д�뵥���Ĵ�������
				 case 7:             break;
				 case 8:             break;
				 case 9:             break;
				 case 16:     Modbus_Func16(); 			break;//д�����Ĵ�������
			 }
		 }
		 else if(modbus.rcbuf[0] == 0) //�㲥��ַ�����Ӧ
		 {
		    
		 }	 
	//}	
	 modbus.recount = 0;//���ռ�������
   modbus.reflag = 0; //���ձ�־����
}
//��Ϊ�ӻ��������ݽ���









//����ʱ��Ϊ����ʱ����

//����ѡ��ӻ�
//����1�ӻ�������2��ʼ��ַ������3�Ĵ�������  slave 0x01
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num)
{
	int j;
	uint16_t crc;//�����CRCУ��λ
	modbus.slave_add=slave;
	modbus.Host_Txbuf[0]=slave;//����Ҫƥ��Ĵӻ���ַ
	modbus.Host_Txbuf[1]=0x03;//������
	modbus.Host_Txbuf[2]=StartAddr/256;//��ʼ��ַ��λ
	modbus.Host_Txbuf[3]=StartAddr%256;//��ʼ��ַ��λ
	modbus.Host_Txbuf[4]=num/256;//�Ĵ���������λ
	modbus.Host_Txbuf[5]=num%256;//�Ĵ���������λ
	crc=Modbus_CRC16(&modbus.Host_Txbuf[0],6); //��ȡCRCУ��λ
	modbus.Host_Txbuf[6]=crc/256;//�Ĵ���������λ
	modbus.Host_Txbuf[7]=crc%256;//�Ĵ���������λ
	
	//�������ݰ�װ���
	RS485_TX_ENABLE;//ʹ�  �485���ƶ�(��������)  
//	HAL_Delay(100);
	for(j=0;j<8;j++)
	{
		Modbus_Send_Byte(modbus.Host_Txbuf[j]);
	}
//	HAL_Delay(10);
	RS485_RX_ENABLE;//ʧ��485���ƶˣ���Ϊ���գ�
	modbus.Host_send_flag=1;//��ʾ�����������
}


//���������ֽڸ���
//����������ܵ�������
void Host_Func3()
{
	int i;
	int count=(int)modbus.rcbuf[2];//�������ݸ���
	
	printf("�ӻ����� %d ���Ĵ������ݣ�\r\n",count/2);
	for(i=0;i<count;i=i+2)
	{
		printf("Temp_Hbit= %d Temp_Lbit= %d temp= %d\r\n",(int)modbus.rcbuf[3+i],(int)modbus.rcbuf[4+i],(int)modbus.rcbuf[4+i]+((int)modbus.rcbuf[3+i])*256);
	}
	modbus.Host_End=1;//���յ����ݴ������
}

//�������մӻ�����Ϣ���д���
void HOST_ModbusRX()
{
	uint16_t crc,rccrc;//����crc�ͽ��յ���crc

	printf("0000");
  if(modbus.reflag == 0)  //�������δ����򷵻ؿ�
	{
		printf("55555");
	   return;
	}
	//�������ݽ���

	//�������г��������λCRCУ��λ����ȫ�㣩
	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2); //��ȡCRCУ��λ
	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1];//�����ȡ��CRCУ��λ
	printf("111");
//	if(crc == rccrc) //CRC����ɹ� ��ʼ������
//	{	
//	   if(modbus.rcbuf[0] == modbus.slave_add)  // ����ַ���Ƕ�Ӧ�ӻ���������
//		 {
//			 if(modbus.rcbuf[1]==3)//������ʱ03
//				 printf("222");
//		      Host_Func3();//���Ƕ�ȡ�Ĵ�������Ч����λ���м���
//		 }
//		 
//	}	
		if(modbus.rcbuf[0] == modbus.slave_add)  // ����ַ���Ƕ�Ӧ�ӻ���������
		 {
			 if(modbus.rcbuf[1]==3)//������ʱ03
				 printf("222");
		      Host_Func3();//���Ƕ�ȡ�Ĵ�������Ч����λ���м���
		 }
	 modbus.recount = 0;//���ռ�������
   modbus.reflag = 0; //���ձ�־����
	
}





//��һ���Ĵ�����д���ݵĲ�������
void Host_write06_slave(uint8_t slave,uint8_t fun,uint16_t StartAddr,uint16_t num)
{
	uint16_t crc,j;//�����CRCУ��λ
	modbus.slave_add=slave;//�ӻ���ַ��ֵһ�£���������
	modbus.Host_Txbuf[0]=slave;//����Ҫƥ��Ĵӻ���ַ
	modbus.Host_Txbuf[1]=fun;//������
	modbus.Host_Txbuf[2]=StartAddr/256;//��ʼ��ַ��λ
	modbus.Host_Txbuf[3]=StartAddr%256;//��ʼ��ַ��λ
	modbus.Host_Txbuf[4]=num/256;
	modbus.Host_Txbuf[5]=num%256;
	crc=Modbus_CRC16(&modbus.Host_Txbuf[0],6); //��ȡCRCУ��λ
	modbus.Host_Txbuf[6]=crc/256;//�Ĵ���������λ
	modbus.Host_Txbuf[7]=crc%256;//�Ĵ���������λ
	
		//�������ݰ�װ���
	RS485_TX_ENABLE;;//ʹ��485���ƶ�(��������)  
//	HAL_Delay(100);
	for(j=0;j<8;j++)
	{
		Modbus_Send_Byte(modbus.Host_Txbuf[j]);
	}
	HAL_Delay(10);
	RS485_RX_ENABLE;//ʧ��485���ƶˣ���Ϊ���գ�
	modbus.Host_send_flag=1;//��ʾ�����������

}

//�ӻ���������
void Host_Func6()
{
	int crc,rccrc;
	crc = Modbus_CRC16(&modbus.rcbuf[0],6); //��ȡCRCУ��λ
	rccrc = modbus.rcbuf[6]*256+modbus.rcbuf[7];//�����ȡ��CRCУ��λ
	printf("222");
//	if(crc == rccrc) //CRC����ɹ� ��ʼ������
//	{	
//	   if(modbus.rcbuf[0] == modbus.slave_add)  // ����ַ���Ƕ�Ӧ�ӻ���������
//		 {
//			 if(modbus.rcbuf[1]==6)//������ʱ06
//			 {
//		
//						printf("��ַΪ %d �Ĵӻ��Ĵ��� %d ��д������ %d \r\n ",(int)modbus.rcbuf[0],(int)modbus.rcbuf[3]+((int)modbus.rcbuf[2])*256,(int)modbus.rcbuf[5]+((int)modbus.rcbuf[4])*256);
//						printf("Host_06 write data right!\r\n");
//				
//			 }
//		 }
//		 
//	}	
		  if(modbus.rcbuf[0] == modbus.slave_add)  // ����ַ���Ƕ�Ӧ�ӻ���������
		 {
			 if(modbus.rcbuf[1]==6)//������ʱ06
			 {
						printf("555");
						printf("��ַΪ %d �Ĵӻ��Ĵ��� %d ��д������ %d \r\n ",(int)modbus.rcbuf[0],(int)modbus.rcbuf[3]+((int)modbus.rcbuf[2])*256,(int)modbus.rcbuf[5]+((int)modbus.rcbuf[4])*256);
						printf("Host_06 write data right!\r\n");
				
			 }
		 }
		
		modbus.Host_End=1;//���յ����ݴ������
}





