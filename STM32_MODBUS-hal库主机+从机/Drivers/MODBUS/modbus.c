#include "modbus.h"
#include "usart.h"

MODBUS modbus;//½á¹¹Ìå±äÁ¿





//×÷Îª´Ó»ú²¿·ÖµÄ´úÂë
 
uint16_t Reg[] ={0x0001,
            0x0002,
            0x0003,
            0x0004,
	          0x0005,
            0x0006,
            0x0007,
			0X0008,
           };//regÊÇÌáÇ°¶¨ÒåºÃµÄ¼Ä´æÆ÷ºÍ¼Ä´æÆ÷Êı¾İ£¬Òª¶ÁÈ¡ºÍ¸ÄĞ´µÄ²¿·ÖÄÚÈİ


// Modbus³õÊ¼»¯º¯Êı
void Modbus_Init()
{
  modbus.myadd = 0x02; //´Ó»úÉè±¸µØÖ·Îª1
  modbus.timrun = 0;    //modbus¶¨Ê±Æ÷Í£Ö¹¼ÆËã
	modbus.slave_add=0x01;//Ö÷»úÒªÆ¥ÅäµÄ´Ó»úµØÖ·
}
// Modbus 3ºÅ¹¦ÄÜÂëº¯Êı
// Modbus Ö÷»ú¶ÁÈ¡¼Ä´æÆ÷Öµ
void Modbus_Func3()
{
  uint16_t Regadd,Reglen,crc;
	uint8_t i,j;	
	//µÃµ½Òª¶ÁÈ¡¼Ä´æÆ÷µÄÊ×µØÖ·
	Regadd = modbus.rcbuf[2]*256+modbus.rcbuf[3];//¶ÁÈ¡µÄÊ×µØÖ·
	//µÃµ½Òª¶ÁÈ¡¼Ä´æÆ÷µÄÊı¾İ³¤¶È
	Reglen = modbus.rcbuf[4]*256+modbus.rcbuf[5];//¶ÁÈ¡µÄ¼Ä´æÆ÷¸öÊı
	//·¢ËÍ»ØÓ¦Êı¾İ°ü
	i = 0;
	modbus.sendbuf[i++] = modbus.myadd;      //IDºÅ£º·¢ËÍ±¾»úÉè±¸µØÖ·
	modbus.sendbuf[i++] = 0x03;              //·¢ËÍ¹¦ÄÜÂë
  modbus.sendbuf[i++] = ((Reglen*2)%256);   //·µ»Ø×Ö½Ú¸öÊı
	for(j=0;j<Reglen;j++)                    //·µ»ØÊı¾İ
	{
		//regÊÇÌáÇ°¶¨ÒåºÃµÄ16Î»Êı×é£¨Ä£·Â¼Ä´æÆ÷£©
	  modbus.sendbuf[i++] = Reg[Regadd+j]/256;//¸ßÎ»Êı¾İ
		modbus.sendbuf[i++] = Reg[Regadd+j]%256;//µÍÎ»Êı¾İ
	}
	crc = Modbus_CRC16(modbus.sendbuf,i);    //¼ÆËãÒª·µ»ØÊı¾İµÄCRC
	modbus.sendbuf[i++] = crc/256;//Ğ£ÑéÎ»¸ßÎ»
	modbus.sendbuf[i++] = crc%256;//Ğ£ÑéÎ»µÍÎ»
	//Êı¾İ°ü´ò°üÍê³É
	// ¿ªÊ¼·µ»ØModbusÊı¾İ
	
	RS485_TX_ENABLE;//ÕâÊÇ¿ªÆô485·¢ËÍ
	
	for(j=0;j<i;j++)//·¢ËÍÊı¾İ
	{
		printf("%d",modbus.sendbuf[j]);
	  Modbus_Send_Byte(modbus.sendbuf[j]);	
	}
	RS485_RX_ENABLE;//ÕâÀïÊÇ¹Ø±Õ485·¢ËÍ
}


// Modbus 6ºÅ¹¦ÄÜÂëº¯Êı
// Modbus Ö÷»úĞ´Èë¼Ä´æÆ÷Öµ
void Modbus_Func6()  
{
  uint16_t Regadd;//µØÖ·16Î»
	uint16_t val;//Öµ
	uint16_t i,crc,j;
	i=0;
  Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //µÃµ½ÒªĞŞ¸ÄµÄµØÖ· 
	val=modbus.rcbuf[4]*256+modbus.rcbuf[5];     //ĞŞ¸ÄºóµÄÖµ£¨ÒªĞ´ÈëµÄÊı¾İ£©
	Reg[Regadd]=val;  //ĞŞ¸Ä±¾Éè±¸ÏàÓ¦µÄ¼Ä´æÆ÷
	
	//ÒÔÏÂÎª»ØÓ¦Ö÷»ú
	modbus.sendbuf[i++]=modbus.myadd;//±¾Éè±¸µØÖ·
  modbus.sendbuf[i++]=0x06;        //¹¦ÄÜÂë 
  modbus.sendbuf[i++]=Regadd/256;//Ğ´ÈëµÄµØÖ·
	modbus.sendbuf[i++]=Regadd%256;
	modbus.sendbuf[i++]=val/256;//Ğ´ÈëµÄÊıÖµ
	modbus.sendbuf[i++]=val%256;
	crc=Modbus_CRC16(modbus.sendbuf,i);//»ñÈ¡crcĞ£ÑéÎ»
	modbus.sendbuf[i++]=crc/256;  //crcĞ£ÑéÎ»¼ÓÈë°üÖĞ
	modbus.sendbuf[i++]=crc%256;
	//Êı¾İ·¢ËÍ°ü´ò°üÍê±Ï
	RS485_TX_ENABLE;;//Ê¹ÄÜ485¿ØÖÆ¶Ë(Æô¶¯·¢ËÍ)  
	for(j=0;j<i;j++)
	{
	 Modbus_Send_Byte(modbus.sendbuf[j]);
	}
	RS485_RX_ENABLE;//Ê§ÄÜ485¿ØÖÆ¶Ë£¨¸ÄÎª½ÓÊÕ£©

}

//ÕâÊÇÍù¶à¸ö¼Ä´æÆ÷Æ÷ÖĞĞ´ÈëÊı¾İ
//¹¦ÄÜÂë0x10Ö¸Áî¼´Ê®½øÖÆ16
void Modbus_Func16()
{
		uint16_t Regadd;//µØÖ·16Î»
		uint16_t Reglen;
		uint16_t i,crc,j;
		
		Regadd=modbus.rcbuf[2]*256+modbus.rcbuf[3];  //ÒªĞŞ¸ÄÄÚÈİµÄÆğÊ¼µØÖ·
		Reglen = modbus.rcbuf[4]*256+modbus.rcbuf[5];//¶ÁÈ¡µÄ¼Ä´æÆ÷¸öÊı
		for(i=0;i<Reglen;i++)//Íù¼Ä´æÆ÷ÖĞĞ´ÈëÊı¾İ
		{
			//½ÓÊÕÊı×éµÄµÚÆßÎ»¿ªÊ¼ÊÇÊı¾İ
			Reg[Regadd+i]=modbus.rcbuf[7+i*2]*256+modbus.rcbuf[8+i*2];//¶Ô¼Ä´æÆ÷Ò»´ÎĞ´ÈëÊı¾İ
		}
		//Ğ´ÈëÊı¾İÍê±Ï£¬½ÓÏÂÀ´ĞèÒª½øĞĞ´ò°ü»Ø¸´Êı¾İÁË
		
		//ÒÔÏÂÎª»ØÓ¦Ö÷»úÄÚÈİ
		//ÄÚÈİ=½ÓÊÕÊı×éµÄÇ°6Î»+Á½Î»µÄĞ£ÑéÎ»
		modbus.sendbuf[0]=modbus.rcbuf[0];//±¾Éè±¸µØÖ·
		modbus.sendbuf[1]=modbus.rcbuf[1];  //¹¦ÄÜÂë 
		modbus.sendbuf[2]=modbus.rcbuf[2];//Ğ´ÈëµÄµØÖ·
		modbus.sendbuf[3]=modbus.rcbuf[3];
		modbus.sendbuf[4]=modbus.rcbuf[4];
		modbus.sendbuf[5]=modbus.rcbuf[5];
		crc=Modbus_CRC16(modbus.sendbuf,6);//»ñÈ¡crcĞ£ÑéÎ»
		modbus.sendbuf[6]=crc/256;  //crcĞ£ÑéÎ»¼ÓÈë°üÖĞ 
		modbus.sendbuf[7]=crc%256;
		//Êı¾İ·¢ËÍ°ü´ò°üÍê±Ï
		
		RS485_TX_ENABLE;;//Ê¹ÄÜ485¿ØÖÆ¶Ë(Æô¶¯·¢ËÍ)  
		for(j=0;j<8;j++)
		{
			Modbus_Send_Byte(modbus.sendbuf[j]);
		}
		RS485_RX_ENABLE;//Ê§ÄÜ485¿ØÖÆ¶Ë£¨¸ÄÎª½ÓÊÕ£©	
}


// ModbusÊÂ¼ş´¦Àíº¯Êı
void Modbus_Event()
{
	uint16_t crc,rccrc;//crcºÍ½ÓÊÕµ½µÄcrc
	//Ã»ÓĞÊÕµ½Êı¾İ°ü
  if(modbus.reflag == 0)  //Èç¹û½ÓÊÕÎ´Íê³ÉÔò·µ»Ø¿Õ
	{
		//printf("111");
	   return;
	}
	printf("000");
	//ÊÕµ½Êı¾İ°ü(½ÓÊÕÍê³É)			
	//Í¨¹ı¶Áµ½µÄÊı¾İÖ¡¼ÆËãCRC
	//²ÎÊı1ÊÇÊı×éÊ×µØÖ·£¬²ÎÊı2ÊÇÒª¼ÆËãµÄ³¤¶È£¨³ıÁËCRCĞ£ÑéÎ»ÆäÓàÈ«Ëã£©
	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2); //»ñÈ¡CRCĞ£ÑéÎ»
	// ¶ÁÈ¡Êı¾İÖ¡µÄCRC
	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1];//¼ÆËã¶ÁÈ¡µÄCRCĞ£ÑéÎ»
	//µÈ¼ÛÓÚÏÂÃæÕâÌõÓï¾ä
	//rccrc=modbus.rcbuf[modbus.recount-1]|(((uint16_t)modbus.rcbuf[modbus.recount-2])<<8);//»ñÈ¡½ÓÊÕµ½µÄCRC
//	if(crc == rccrc) //CRC¼ìÑé³É¹¦ ¿ªÊ¼·ÖÎö°ü
//	{	
	   if(modbus.rcbuf[0] == modbus.myadd)  // ¼ì²éµØÖ·ÊÇ·ñÊ±×Ô¼ºµÄµØÖ·
		 {
		   switch(modbus.rcbuf[1])   //·ÖÎömodbus¹¦ÄÜÂë
			 {
			   case 0:             break;
				 case 1:             break;
				 case 2:             break;
				 case 3:      Modbus_Func3();      break;//ÕâÊÇ¶ÁÈ¡¼Ä´æÆ÷µÄÊı¾İ
				 case 4:             break;
				 case 5:             break;
         case 6:      Modbus_Func6();      break;//ÕâÊÇĞ´Èëµ¥¸ö¼Ä´æÆ÷Êı¾İ
				 case 7:             break;
				 case 8:             break;
				 case 9:             break;
				 case 16:     Modbus_Func16(); 			break;//Ğ´Èë¶à¸ö¼Ä´æÆ÷Êı¾İ
			 }
		 }
		 else if(modbus.rcbuf[0] == 0) //¹ã²¥µØÖ·²»Óè»ØÓ¦
		 {
		    
		 }	 
	//}	
	 modbus.recount = 0;//½ÓÊÕ¼ÆÊıÇåÁã
   modbus.reflag = 0; //½ÓÊÕ±êÖ¾ÇåÁã
}
//×÷Îª´Ó»ú²¿·ÖÄÚÈİ½áÊø









//ÏÂÃæÊ±×÷ÎªÖ÷»úÊ±ÄÚÈİ

//Ö÷»úÑ¡Ôñ´Ó»ú
//²ÎÊı1´Ó»ú£¬²ÎÊı2ÆğÊ¼µØÖ·£¬²ÎÊı3¼Ä´æÆ÷¸öÊı  slave 0x01
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num)
{
	int j;
	uint16_t crc;//¼ÆËãµÄCRCĞ£ÑéÎ»
	modbus.slave_add=slave;
	modbus.Host_Txbuf[0]=slave;//ÕâÊÇÒªÆ¥ÅäµÄ´Ó»úµØÖ·
	modbus.Host_Txbuf[1]=0x03;//¹¦ÄÜÂë
	modbus.Host_Txbuf[2]=StartAddr/256;//ÆğÊ¼µØÖ·¸ßÎ»
	modbus.Host_Txbuf[3]=StartAddr%256;//ÆğÊ¼µØÖ·µÍÎ»
	modbus.Host_Txbuf[4]=num/256;//¼Ä´æÆ÷¸öÊı¸ßÎ»
	modbus.Host_Txbuf[5]=num%256;//¼Ä´æÆ÷¸öÊıµÍÎ»
	crc=Modbus_CRC16(&modbus.Host_Txbuf[0],6); //»ñÈ¡CRCĞ£ÑéÎ»
	modbus.Host_Txbuf[6]=crc/256;//¼Ä´æÆ÷¸öÊı¸ßÎ»
	modbus.Host_Txbuf[7]=crc%256;//¼Ä´æÆ÷¸öÊıµÍÎ»
	
	//·¢ËÍÊı¾İ°ü×°Íê±Ï
	RS485_TX_ENABLE;//Ê¹Ä  Ü485¿ØÖÆ¶Ë(Æô¶¯·¢ËÍ)  
//	HAL_Delay(100);
	for(j=0;j<8;j++)
	{
		Modbus_Send_Byte(modbus.Host_Txbuf[j]);
	}
//	HAL_Delay(10);
	RS485_RX_ENABLE;//Ê§ÄÜ485¿ØÖÆ¶Ë£¨¸ÄÎª½ÓÊÕ£©
	modbus.Host_send_flag=1;//±íÊ¾·¢ËÍÊı¾İÍê±Ï
}


//µÚÈı¸öÊÇ×Ö½Ú¸öÊı
//Ö÷»ú´¦Àí½ÓÊÜµ½µÄÊı¾İ
void Host_Func3()
{
	int i;
	int count=(int)modbus.rcbuf[2];//ÕâÊÇÊı¾İ¸öÊı
	
	printf("´Ó»ú·µ»Ø %d ¸ö¼Ä´æÆ÷Êı¾İ£º\r\n",count/2);
	for(i=0;i<count;i=i+2)
	{
		printf("Temp_Hbit= %d Temp_Lbit= %d temp= %d\r\n",(int)modbus.rcbuf[3+i],(int)modbus.rcbuf[4+i],(int)modbus.rcbuf[4+i]+((int)modbus.rcbuf[3+i])*256);
	}
	modbus.Host_End=1;//½ÓÊÕµÄÊı¾İ´¦ÀíÍê±Ï
}

//Ö÷»ú½ÓÊÕ´Ó»úµÄÏûÏ¢½øĞĞ´¦Àí
void HOST_ModbusRX()
{
	uint16_t crc,rccrc;//¼ÆËãcrcºÍ½ÓÊÕµ½µÄcrc

	printf("0000");
  if(modbus.reflag == 0)  //Èç¹û½ÓÊÕÎ´Íê³ÉÔò·µ»Ø¿Õ
	{
		printf("55555");
	   return;
	}
	//½ÓÊÕÊı¾İ½áÊø

	//£¨Êı×éÖĞ³ıÁË×îºóÁ½Î»CRCĞ£ÑéÎ»ÆäÓàÈ«Ëã£©
	crc = Modbus_CRC16(&modbus.rcbuf[0],modbus.recount-2); //»ñÈ¡CRCĞ£ÑéÎ»
	rccrc = modbus.rcbuf[modbus.recount-2]*256+modbus.rcbuf[modbus.recount-1];//¼ÆËã¶ÁÈ¡µÄCRCĞ£ÑéÎ»
	printf("111");
//	if(crc == rccrc) //CRC¼ìÑé³É¹¦ ¿ªÊ¼·ÖÎö°ü
//	{	
//	   if(modbus.rcbuf[0] == modbus.slave_add)  // ¼ì²éµØÖ·ÊÇÊÇ¶ÔÓ¦´Ó»ú·¢¹ıÀ´µÄ
//		 {
//			 if(modbus.rcbuf[1]==3)//¹¦ÄÜÂëÊ±03
//				 printf("222");
//		      Host_Func3();//ÕâÊÇ¶ÁÈ¡¼Ä´æÆ÷µÄÓĞĞ§Êı¾İÎ»½øĞĞ¼ÆËã
//		 }
//		 
//	}	
		if(modbus.rcbuf[0] == modbus.slave_add)  // ¼ì²éµØÖ·ÊÇÊÇ¶ÔÓ¦´Ó»ú·¢¹ıÀ´µÄ
		 {
			 if(modbus.rcbuf[1]==3)//¹¦ÄÜÂëÊ±03
				 printf("222");
		      Host_Func3();//ÕâÊÇ¶ÁÈ¡¼Ä´æÆ÷µÄÓĞĞ§Êı¾İÎ»½øĞĞ¼ÆËã
		 }
	 modbus.recount = 0;//½ÓÊÕ¼ÆÊıÇåÁã
   modbus.reflag = 0; //½ÓÊÕ±êÖ¾ÇåÁã
	
}





//ÏòÒ»¸ö¼Ä´æÆ÷ÖĞĞ´Êı¾İµÄ²ÎÊıÉèÖÃ
void Host_write06_slave(uint8_t slave,uint8_t fun,uint16_t StartAddr,uint16_t num)
{
	uint16_t crc,j;//¼ÆËãµÄCRCĞ£ÑéÎ»
	modbus.slave_add=slave;//´Ó»úµØÖ·¸³ÖµÒ»ÏÂ£¬ºóÆÚÓĞÓÃ
	modbus.Host_Txbuf[0]=slave;//ÕâÊÇÒªÆ¥ÅäµÄ´Ó»úµØÖ·
	modbus.Host_Txbuf[1]=fun;//¹¦ÄÜÂë
	modbus.Host_Txbuf[2]=StartAddr/256;//ÆğÊ¼µØÖ·¸ßÎ»
	modbus.Host_Txbuf[3]=StartAddr%256;//ÆğÊ¼µØÖ·µÍÎ»
	modbus.Host_Txbuf[4]=num/256;
	modbus.Host_Txbuf[5]=num%256;
	crc=Modbus_CRC16(&modbus.Host_Txbuf[0],6); //»ñÈ¡CRCĞ£ÑéÎ»
	modbus.Host_Txbuf[6]=crc/256;//¼Ä´æÆ÷¸öÊı¸ßÎ»
	modbus.Host_Txbuf[7]=crc%256;//¼Ä´æÆ÷¸öÊıµÍÎ»
	
		//·¢ËÍÊı¾İ°ü×°Íê±Ï
	RS485_TX_ENABLE;;//Ê¹ÄÜ485¿ØÖÆ¶Ë(Æô¶¯·¢ËÍ)  
//	HAL_Delay(100);
	for(j=0;j<8;j++)
	{
		Modbus_Send_Byte(modbus.Host_Txbuf[j]);
	}
	HAL_Delay(10);
	RS485_RX_ENABLE;//Ê§ÄÜ485¿ØÖÆ¶Ë£¨¸ÄÎª½ÓÊÕ£©
	modbus.Host_send_flag=1;//±íÊ¾·¢ËÍÊı¾İÍê±Ï

}

//´Ó»ú·µ»ØÊı¾İ
void Host_Func6()
{
	int crc,rccrc;
	crc = Modbus_CRC16(&modbus.rcbuf[0],6); //»ñÈ¡CRCĞ£ÑéÎ»
	rccrc = modbus.rcbuf[6]*256+modbus.rcbuf[7];//¼ÆËã¶ÁÈ¡µÄCRCĞ£ÑéÎ»
	printf("222");
//	if(crc == rccrc) //CRC¼ìÑé³É¹¦ ¿ªÊ¼·ÖÎö°ü
//	{	
//	   if(modbus.rcbuf[0] == modbus.slave_add)  // ¼ì²éµØÖ·ÊÇÊÇ¶ÔÓ¦´Ó»ú·¢¹ıÀ´µÄ
//		 {
//			 if(modbus.rcbuf[1]==6)//¹¦ÄÜÂëÊ±06
//			 {
//		
//						printf("µØÖ·Îª %d µÄ´Ó»ú¼Ä´æÆ÷ %d ÖĞĞ´ÈëÊı¾İ %d \r\n ",(int)modbus.rcbuf[0],(int)modbus.rcbuf[3]+((int)modbus.rcbuf[2])*256,(int)modbus.rcbuf[5]+((int)modbus.rcbuf[4])*256);
//						printf("Host_06 write data right!\r\n");
//				
//			 }
//		 }
//		 
//	}	
		  if(modbus.rcbuf[0] == modbus.slave_add)  // ¼ì²éµØÖ·ÊÇÊÇ¶ÔÓ¦´Ó»ú·¢¹ıÀ´µÄ
		 {
			 if(modbus.rcbuf[1]==6)//¹¦ÄÜÂëÊ±06
			 {
						printf("555");
						printf("µØÖ·Îª %d µÄ´Ó»ú¼Ä´æÆ÷ %d ÖĞĞ´ÈëÊı¾İ %d \r\n ",(int)modbus.rcbuf[0],(int)modbus.rcbuf[3]+((int)modbus.rcbuf[2])*256,(int)modbus.rcbuf[5]+((int)modbus.rcbuf[4])*256);
						printf("Host_06 write data right!\r\n");
				
			 }
		 }
		
		modbus.Host_End=1;//½ÓÊÕµÄÊı¾İ´¦ÀíÍê±Ï
}





