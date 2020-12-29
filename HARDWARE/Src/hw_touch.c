#include "hw_touch.h"

/*---------------------------------- IIC �������ú� ---------------------------------*/
 

#define Touch_IIC_SCL_CLK_ENABLE     __HAL_RCC_GPIOH_CLK_ENABLE()		// SCL ����ʱ��
#define Touch_IIC_SCL_PORT   			 GPIOH                 				// SCL ���Ŷ˿�
#define Touch_IIC_SCL_PIN     		 GPIO_PIN_4  							// SCL ����
         
#define Touch_IIC_SDA_CLK_ENABLE     __HAL_RCC_GPIOH_CLK_ENABLE() 	// SDA ����ʱ��
#define Touch_IIC_SDA_PORT   			 GPIOH                   			// SDA ���Ŷ˿�
#define Touch_IIC_SDA_PIN    			 GPIO_PIN_5              			// SDA ����

#define Touch_INT_CLK_ENABLE    		 __HAL_RCC_GPIOI_CLK_ENABLE()		// INT ����ʱ��
#define Touch_INT_PORT   				 GPIOI                   			// INT ���Ŷ˿�
#define Touch_INT_PIN    				 GPIO_PIN_11             			// INT ����

#define Touch_RST_CLK_ENABLE   		 __HAL_RCC_GPIOC_CLK_ENABLE()		// RST ����ʱ��
#define Touch_RST_PORT   				 GPIOC                   			// RST ���Ŷ˿�
#define Touch_RST_PIN    				 GPIO_PIN_13           				// RST ����

/*----------------------------------- IIC��ض��� ----------------------------------*/

#define ACK_OK  	1  			// ��Ӧ����
#define ACK_ERR 	0				// ��Ӧ����

// IICͨ����ʱ��Touch_IIC_Delay()����ʹ�ã�
//	ͨ���ٶ���100KHz���ң�����ʹ�ýϳ���FPC����������Ļ��������ʹ�ù��ߵ��ٶ�
#define IIC_DelayVaule  20  	

/*------------------------------------ IO�ڲ��� ------------------------------------*/   

	
#define Touch_IIC_SCL(a)	if (a)	\
										HAL_GPIO_WritePin(Touch_IIC_SCL_PORT, Touch_IIC_SCL_PIN, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(Touch_IIC_SCL_PORT, Touch_IIC_SCL_PIN, GPIO_PIN_RESET)	

#define Touch_IIC_SDA(a)	if (a)	\
										HAL_GPIO_WritePin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN, GPIO_PIN_RESET)		

/*------------------------------------ �Ĵ涨�� -----------------------------------*/  		

#define GT9XX_IIC_RADDR 0xBB			// IIC��ʼ����ַ
#define GT9XX_IIC_WADDR 0xBA

#define GT9XX_CFG_ADDR 	0x8047		// �̼�������Ϣ�Ĵ�����������ʼ��ַ
#define GT9XX_READ_ADDR 0x814E		// ������Ϣ�Ĵ���
#define GT9XX_ID_ADDR 	0x8140		// �������ID�Ĵ���

/*------------------------------------ �������� -----------------------------------*/  		
/*****************************************************************************************
*	�� �� ��: Touch_IIC_GPIO_Config
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ��ʼ��IIC��GPIO��,�������
*	˵    ��: ����IICͨ���ٶȲ��ߣ������IO���ٶ�����Ϊ2M����
******************************************************************************************/

void Touch_IIC_GPIO_Config (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	Touch_IIC_SCL_CLK_ENABLE;	//��ʼ��IO��ʱ��
	Touch_IIC_SDA_CLK_ENABLE;
	Touch_INT_CLK_ENABLE;	
	Touch_RST_CLK_ENABLE;	
	
	GPIO_InitStruct.Pin 			= Touch_IIC_SCL_PIN;				// SCL����
	GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_OD;			// ��©���
	GPIO_InitStruct.Pull 		= GPIO_NOPULL;						// ����������
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;			// �ٶȵȼ� 
	HAL_GPIO_Init(Touch_IIC_SCL_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin 			= Touch_IIC_SDA_PIN;				// SDA����
	HAL_GPIO_Init(Touch_IIC_SDA_PORT, &GPIO_InitStruct);		

	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      			// �������
	GPIO_InitStruct.Pull  = GPIO_PULLUP;		 					// ����	
	
	GPIO_InitStruct.Pin = Touch_INT_PIN; 							//	INT
	HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);				

	GPIO_InitStruct.Pin = Touch_RST_PIN; 							//	RST
	HAL_GPIO_Init(Touch_RST_PORT, &GPIO_InitStruct);					   

	HAL_GPIO_WritePin(Touch_IIC_SCL_PORT, Touch_IIC_SCL_PIN, GPIO_PIN_SET);		// SCL����ߵ�ƽ
	HAL_GPIO_WritePin(Touch_IIC_SDA_PORT, Touch_IIC_SDA_PIN, GPIO_PIN_SET);    // SDA����ߵ�ƽ
	HAL_GPIO_WritePin(Touch_INT_PORT, 	  Touch_INT_PIN,     GPIO_PIN_RESET);  // INT����͵�ƽ
	HAL_GPIO_WritePin(Touch_RST_PORT,     Touch_RST_PIN,     GPIO_PIN_SET);    // RST�����	��ƽ

}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_Delay
*	��ڲ���: a - ��ʱʱ��
*	�� �� ֵ: ��
*	��������: ����ʱ����
*	˵    ��: Ϊ����ֲ�ļ�����Ҷ���ʱ����Ҫ�󲻸ߣ����Բ���Ҫʹ�ö�ʱ������ʱ
******************************************************************************************/

void Touch_IIC_Delay(uint32_t a)
{
	volatile  int i; //����volatile���ᱻ�Ż���
	while (a --)				
	{
		for (i = 0; i < 10; i++);
	}
}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_INT_Out
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ����IIC��INT��Ϊ���ģʽ
*	˵    ��: ��
******************************************************************************************/

void Touch_INT_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      	// ���ģʽ
	GPIO_InitStruct.Pull  = GPIO_PULLUP;		 			// ����	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    	// �ٶȵȼ�
	GPIO_InitStruct.Pin   = Touch_INT_PIN ;  				// ��ʼ�� INT ����
	
	HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);		
}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_INT_In
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ����IIC��INT��Ϊ����ģʽ
*	˵    ��: ��
******************************************************************************************/

void Touch_INT_In(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;      		// ����ģʽ
	GPIO_InitStruct.Pull  = GPIO_NOPULL;		 			// ����	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    	// �ٶȵȼ�
	GPIO_InitStruct.Pin   = Touch_INT_PIN ;  				// ��ʼ�� INT ����
	
	HAL_GPIO_Init(Touch_INT_PORT, &GPIO_InitStruct);		

}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_Start
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: IIC��ʼ�ź�
*	˵    ��: ��SCL���ڸߵ�ƽ�ڼ䣬SDA�ɸߵ�������Ϊ��ʼ�ź�
******************************************************************************************/

void Touch_IIC_Start(void)
{
	Touch_IIC_SDA(1);		
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SDA(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_Stop
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: IICֹͣ�ź�
*	˵    ��: ��SCL���ڸߵ�ƽ�ڼ䣬SDA�ɵ͵�������Ϊ��ʼ�ź�
******************************************************************************************/

void Touch_IIC_Stop(void)
{
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(1);
	Touch_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_ACK
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: IICӦ���ź�
*	˵    ��: ��SCLΪ�ߵ�ƽ�ڼ䣬SDA�������Ϊ�͵�ƽ������Ӧ���ź�
******************************************************************************************/

void Touch_IIC_ACK(void)
{
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(0);
	Touch_IIC_Delay(IIC_DelayVaule);	
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SCL(0);		// SCL�����ʱ��SDAӦ�������ߣ��ͷ�����
	Touch_IIC_SDA(1);		
	
	Touch_IIC_Delay(IIC_DelayVaule);

}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_NoACK
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: IIC��Ӧ���ź�
*	˵    ��: ��SCLΪ�ߵ�ƽ�ڼ䣬��SDA����Ϊ�ߵ�ƽ��������Ӧ���ź�
******************************************************************************************/

void Touch_IIC_NoACK(void)
{
	Touch_IIC_SCL(0);	
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SDA(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	
	Touch_IIC_SCL(0);
	Touch_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
*	�� �� ��: Touch_IIC_WaitACK
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: �ȴ������豸����Ӧ���ź�
*	˵    ��: ��SCLΪ�ߵ�ƽ�ڼ䣬����⵽SDA����Ϊ�͵�ƽ��������豸��Ӧ����
******************************************************************************************/

uint8_t Touch_IIC_WaitACK(void)
{
	Touch_IIC_SDA(1);
	Touch_IIC_Delay(IIC_DelayVaule);
	Touch_IIC_SCL(1);
	Touch_IIC_Delay(IIC_DelayVaule);	
	
	if( HAL_GPIO_ReadPin(Touch_IIC_SDA_PORT,Touch_IIC_SDA_PIN) != 0) //�ж��豸�Ƿ���������Ӧ		
	{
		Touch_IIC_SCL(0);	
		Touch_IIC_Delay( IIC_DelayVaule );		
		return ACK_ERR;	//��Ӧ��
	}
	else
	{
		Touch_IIC_SCL(0);	
		Touch_IIC_Delay( IIC_DelayVaule );		
		return ACK_OK;	//Ӧ������
	}
}

/*****************************************************************************************
*	�� �� ��:	Touch_IIC_WriteByte
*	��ڲ���:	IIC_Data - Ҫд���8λ����
*	�� �� ֵ:	ACK_OK  - �豸��Ӧ����
*          	ACK_ERR - �豸��Ӧ����
*	��������:	дһ�ֽ�����
*	˵    ��:��λ��ǰ
******************************************************************************************/

uint8_t Touch_IIC_WriteByte(uint8_t IIC_Data)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		Touch_IIC_SDA(IIC_Data & 0x80);
		
		Touch_IIC_Delay( IIC_DelayVaule );
		Touch_IIC_SCL(1);
		Touch_IIC_Delay( IIC_DelayVaule );
		Touch_IIC_SCL(0);		
		if(i == 7)
		{
			Touch_IIC_SDA(1);			
		}
		IIC_Data <<= 1;
	}

	return Touch_IIC_WaitACK(); //�ȴ��豸��Ӧ
}

/*****************************************************************************************
*	�� �� ��:	Touch_IIC_ReadByte
*	��ڲ���:	ACK_Mode - ��Ӧģʽ������1�򷢳�Ӧ���źţ�����0������Ӧ���ź�
*	�� �� ֵ:	ACK_OK  - �豸��Ӧ����
*          	ACK_ERR - �豸��Ӧ����
*	��������:��һ�ֽ�����
*	˵    ��:1.��λ��ǰ
*				2.Ӧ�������������һ�ֽ�����ʱ���ͷ�Ӧ���ź�
******************************************************************************************/

uint8_t Touch_IIC_ReadByte(uint8_t ACK_Mode)
{
	uint8_t IIC_Data = 0;
	uint8_t i = 0;
	
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		
		Touch_IIC_SCL(1);
		Touch_IIC_Delay( IIC_DelayVaule );
		IIC_Data |= (HAL_GPIO_ReadPin(Touch_IIC_SDA_PORT,Touch_IIC_SDA_PIN) & 0x01);	
		Touch_IIC_SCL(0);
		Touch_IIC_Delay( IIC_DelayVaule );
	}
	
	if ( ACK_Mode == 1 )				//	Ӧ���ź�
		Touch_IIC_ACK();
	else
		Touch_IIC_NoACK();		 	// ��Ӧ���ź�
	
	return IIC_Data; 
}

/********************************************************************************************/
TouchStructure touchInfo; 			//	������Ϣ�ṹ�壬�ں��� Touch_Scan() �ﱻ���ã��洢��������

	
// ���������������飬�ں��� GT9XX_SendCfg() ����ã��������ô���IC����ز���
//	����GT911���Թ̻�������Щ�����������û�һ������������ٽ�������
//	��ϸ�ļĴ���������ο� GT911 �����ֲ�
//
const uint8_t GT9XX_CFG_DATA[] = 	
{                              	 
	0XFF,			// �Ĵ�����ַ��0x8047�����ܣ����ð汾��
	
	0XE0,0X01,	// �Ĵ�����ַ��0x8048~0x8049�����ܣ�X�������ֵ����λ��ǰ
	0X10,0X01,	// �Ĵ�����ַ��0x804A~0x804B�����ܣ�Y�������ֵ����λ��ǰ
	
	0X05,			// �Ĵ�����ַ��0x804C�����ܣ����������������1~5��
	0X0d,			// �Ĵ�����ַ��0x804D�����ܣ�����INT������ʽ��XY���꽻��
	0X00,			// �üĴ�����������
	0X01,			// �Ĵ�����ַ��0x804F�����ܣ����»��ɿ�ȥ������
	0X0a,			// �Ĵ�����ַ��0x8050�����ܣ�ԭʼ���괰���˲�ֵ
	
	0X28,0X0f,0X50,0X32,0X03,0X05,0X00,0X00,0X00,0X00, 	// 0X8051 ~ 0X805A
	0X00,0X00,0X08,0X17,0X19,0X1c,0X14,0X87,0X29,0X0a, 	// 0X805B ~ 0X8064
	0X2a,0X2c,0X31,0X0d,0X00,0X00,0X01,0X9a,0X02,0X2d, 	// 0X8065 ~ 0X806E
	0X00,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 	// 0X806F ~ 0X8078
	0X00,0X1e,0X3c,0X94,0Xc5,0X02,0X07,0X00,0X00,0X04, 	// 0X8079 ~ 0X8082
	0X9e,0X20,0X00,0X8d,0X25,0X00,0X7f,0X2a,0X00,0X73, 	// 0X8083 ~ 0X808C
	0X30,0X00,0X67,0X38,0X00,0X67,0X00,0X00,0X00,0X00, 	// 0X808D ~ 0X8096
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 	// 0X8097 ~ 0X80A0
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 	// 0X80A1 ~ 0X80AA
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 	// 0X80AB ~ 0X80B4
	0X00,0X00,

/******************************************************************************************
*	�Ĵ�����ַ:	0x80B7~0X80C4
*	����˵��  :	�޸ĸ�Ӧͨ����Ӧ��оƬͨ���ţ����Ըı䴥�����Ĵ�ֱɨ�跽��
*******************************************************************************************/

	0X02,0X04,0X06,0X08,0X0a,0X0c,0X0e,0X10,0X12,0X14,
	0Xff,0Xff,0Xff,0Xff,
	
/******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// �����üĴ�������������
	0X00,0X00,0X00,0X00,0x00,0x00, 								// �����üĴ�������������
	
/*******************************************************************************************
*	�Ĵ�����ַ:	0x80D5~0X80EE
*	����˵��  :	�޸�����ͨ����Ӧ��оƬͨ���ţ����Ըı䴥������ˮƽɨ�跽��
********************************************************************************************/

	0X00,0X02,0X04,0X06,0X08,0X0a,0X0c,0X1d,0X1e,0X1f,
	0X20,0X21,0X22,0X24,0X26,0X28,0Xff,0Xff,0Xff,0Xff,
	0Xff,0Xff,0Xff,0Xff,0Xff,0Xff,
	
/*******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// ͨ������ϵ���Ĵ����������޸�
	0X00,0X00,0X00,0X00,0X00,0X00,								// ͨ������ϵ���Ĵ����������޸�

};


/*****************************************************************************************
*	�� �� ��:	GT9XX_Reset
*	��ڲ���:	��
*	�� �� ֵ:	��
*	��������:	��λGT911
*	˵    ��:��λGT911������оƬ��IIC��ַ����Ϊ0xBA/0xBB
******************************************************************************************/

void GT9XX_Reset(void)
{
	Touch_INT_Out();	//	��INT��������Ϊ���
	
	// ��ʼ������״̬
	HAL_GPIO_WritePin(Touch_INT_PORT,Touch_INT_PIN,GPIO_PIN_RESET);  // INT����͵�ƽ
	HAL_GPIO_WritePin(Touch_RST_PORT,Touch_RST_PIN,GPIO_PIN_SET);    // RST�����	��ƽ
	Touch_IIC_Delay(40000);
	
	// ��ʼִ�и�λ
	//	INT���ű��ֵ͵�ƽ���䣬��������ַ����Ϊ0XBA/0XBB
	HAL_GPIO_WritePin(Touch_RST_PORT,Touch_RST_PIN,GPIO_PIN_RESET); // ���͸�λ���ţ���ʱоƬִ�и�λ
	Touch_IIC_Delay(1000000);										// ��ʱ
	HAL_GPIO_WritePin(Touch_RST_PORT,Touch_RST_PIN,GPIO_PIN_SET);			// ���߸�λ���ţ���λ����
	Touch_IIC_Delay(350000);										// ��ʱ
	Touch_INT_In();													// INT����תΪ��������
	Touch_IIC_Delay(350000);										// ��ʱ
									
}


/*****************************************************************************************
*	�� �� ��:	GT9XX_WriteHandle
*	��ڲ���:	addr - Ҫ�����ļĴ���
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX д����
*	˵    ��:��ָ���ļĴ���ִ��д����
******************************************************************************************/

uint8_t GT9XX_WriteHandle (uint16_t addr)
{
	uint8_t status;				// ״̬��־λ

	Touch_IIC_Start();	// ����IICͨ��
	if( Touch_IIC_WriteByte(GT9XX_IIC_WADDR) == ACK_OK ) //д����ָ��
	{
		if( Touch_IIC_WriteByte((uint8_t)(addr >> 8)) == ACK_OK ) //д��16λ��ַ
		{
			if( Touch_IIC_WriteByte((uint8_t)(addr)) != ACK_OK )
			{
				status = ERROR;	// ����ʧ��
			}			
		}
	}
	status = SUCCESS;	// �����ɹ�
	return status;	
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_WriteData
*	��ڲ���:	addr - Ҫд��ļĴ���
*				value - Ҫд�������
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX дһ�ֽ�����
*	˵    ��:��ָ���ļĴ���д��һ�ֽ�����
******************************************************************************************/

uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value)
{
	uint8_t status;
	
	Touch_IIC_Start(); //����IICͨѶ

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//д��Ҫ�����ļĴ���
	{
		if (Touch_IIC_WriteByte(value) != ACK_OK) //д����
		{
			status = ERROR;						
		}
	}	
	Touch_IIC_Stop(); // ֹͣͨѶ
	
	status = SUCCESS;	// д��ɹ�
	return status;
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_WriteReg
*	��ڲ���:	addr - Ҫд��ļĴ��������׵�ַ
*				cnt  - ���ݳ���
*				value - Ҫд���������
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX д�Ĵ���
*	˵    ��:��оƬ�ļĴ�����д��ָ�����ȵ�����
******************************************************************************************/

uint8_t GT9XX_WriteReg (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	Touch_IIC_Start();

	if( GT9XX_WriteHandle(addr) == SUCCESS) 	// д��Ҫ�����ļĴ���
	{
		for(i = 0 ; i < cnt; i++)			 	// ����
		{
			Touch_IIC_WriteByte(value[i]);	// д������
		}					
		Touch_IIC_Stop();		// ֹͣIICͨ��
		status = SUCCESS;		// д��ɹ�
	}
	else
	{
		Touch_IIC_Stop();		// ֹͣIICͨ��
		status = ERROR;		// д��ʧ��
	}
	return status;	
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_ReadReg
*	��ڲ���:	addr - Ҫ��ȡ�ļĴ��������׵�ַ
*				cnt  - ���ݳ���
*				value - Ҫ��ȡ��������
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX ���Ĵ���
*	˵    ��:��оƬ�ļĴ�������ȡָ�����ȵ�����
******************************************************************************************/

uint8_t GT9XX_ReadReg (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	status = ERROR;
	Touch_IIC_Start();		// ����IICͨ��

	if( GT9XX_WriteHandle(addr) == SUCCESS) //д��Ҫ�����ļĴ���
	{
		Touch_IIC_Start(); //��������IICͨѶ

		if (Touch_IIC_WriteByte(GT9XX_IIC_RADDR) == ACK_OK)	// ���Ͷ�����
		{	
			for(i = 0 ; i < cnt; i++)	// ����
			{
				if (i == (cnt - 1))
				{
					value[i] = Touch_IIC_ReadByte(0);	// �������һ������ʱ���� ��Ӧ���ź�
				}
				else
				{
					value[i] = Touch_IIC_ReadByte(1);	// ����Ӧ���ź�
				}
			}					
			Touch_IIC_Stop();	// ֹͣIICͨ��
			status = SUCCESS;
		}
	}
	Touch_IIC_Stop();	// ֹͣIICͨ��
	return (status);	
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_SendCfg
*	��ڲ���:	��
*	�� �� ֵ:��
*	��������:	����GT911���ò���
*	˵    ��:����GT911���Ե��籣�����ò��������ҳ���ʱ�����úã�һ��������û�������ģ�
*				�û��޸Ĳ���֮����Ҫ���ú������ε�����ȻƵ����д��Ὣ����оƬ��Flashд��
******************************************************************************************/

void GT9XX_SendCfg(void)
{
	uint8_t GT9XX_Check[2]; // 
	uint8_t i=0;
	
	GT9XX_Check[1] = 1;		// ���ø��±�־
	
	for(i=0;i<sizeof(GT9XX_CFG_DATA);i++)
	{
		GT9XX_Check[0] += GT9XX_CFG_DATA[i];	// ����У���
	}
   GT9XX_Check [0] = (~GT9XX_Check[0])+1;		
	
	GT9XX_WriteReg(0X8047,sizeof(GT9XX_CFG_DATA),(uint8_t*)GT9XX_CFG_DATA);	//	���ͼĴ�������
	GT9XX_WriteReg(0X80FF,2,GT9XX_Check); 		// д��У��ͣ�����������
} 

/*****************************************************************************************
*	�� �� ��:	GT9XX_ReadCfg
*	��ڲ���:	��
*	�� �� ֵ:��
*	��������:	��ȡGT911���ò���
*	˵    ��:ͨ�����ڴ�ӡ���ݣ����޸�����֮ǰ����Ҫ��ԭ���Ĳ�����ȡ�����ñ��ݣ�����Ķ��ؼ��Ĳ���		
******************************************************************************************/

void GT9XX_ReadCfg(void)
{
	uint8_t  GT9XX_Cfg[184];	// ���鳤��ȡ����ʵ�ʵ�оƬ�ļĴ�������
	uint16_t i = 0;

	printf("-----------------------------------------\r\n");	
	printf("��ȡоƬ������Ϣ�����ڴ�ӡ���\r\n");
	
	GT9XX_ReadReg (GT9XX_CFG_ADDR,184,GT9XX_Cfg);	// ��������Ϣ
	for(i=0;i<184;i++)
	{	
		if( (i%10 == 0) && (i>0) )			
		{
			printf("\r\n");
		}
		printf("0X%.2x,",GT9XX_Cfg[i]);
	}
	printf("\r\n-----------------------------------------\r\n");	
}


/*****************************************************************************************
*	�� �� ��: Touch_Init
*	��ڲ���: ��
*	�� �� ֵ:	 SUCCESS  - ��ʼ���ɹ�
*            ERROR 	 - ����δ��⵽������	
*	��������: ����IC��ʼ��������ȡ��Ӧ��Ϣ���͵�����
*	˵    ��: ��ʼ���������
******************************************************************************************/

uint8_t HW_Touch_Init(void)
{
	uint8_t GT9XX_Info[11];	// ������IC��Ϣ
	uint8_t cfgVersion = 0;	// �������ð汾
	
	Touch_IIC_GPIO_Config(); 	// ��ʼ��IIC����
	GT9XX_Reset();					// GT9147 ��λ
	
	// ��ȡGT911���ò�����ͨ�����ڴ�ӡ���	
	//	ͨ�����ڴ�ӡ���ݣ����޸�����֮ǰ����Ҫ��ԭ���Ĳ�����ȡ�����ñ��ݣ�����Ķ��ؼ��Ĳ���
//	GT9XX_ReadCfg();		
	
	// д��GT911���ã�һ��������û��������	
	//�û��޸Ĳ���֮����Ҫ���ú������ε�����ȻƵ����д��Ὣ����оƬ��Flashд��
//	GT9XX_SendCfg();				
	
	GT9XX_ReadReg (GT9XX_ID_ADDR,11,GT9XX_Info);		// ��������IC��Ϣ
	GT9XX_ReadReg (GT9XX_CFG_ADDR,1,&cfgVersion);	// ���������ð汾
	
	if( GT9XX_Info[0] == '9' )	//�жϵ�һ���ַ��Ƿ�Ϊ 9
	{
		printf("Touch ID: GT%.4s \r\n",GT9XX_Info);	//��ӡ����оƬ��ID
		printf("�̼��汾�� 0X%.4x\r\n",(GT9XX_Info[5]<<8) + GT9XX_Info[4]);	// оƬ�̼��汾
		printf("�����ֱ��ʣ�%d * %d\r\n",(GT9XX_Info[7]<<8) + GT9XX_Info[6],(GT9XX_Info[9]<<8) +GT9XX_Info[8]);	// ��ǰ�����ֱ���		
		printf("�����������ð汾�� 0X%.2x \r\n",cfgVersion);	// �������ð汾	
		
		return SUCCESS;
	}
	else
	{
		printf("Touch Error\r\n");	//����δ��⵽������
		return ERROR;
	}

}

/*****************************************************************************************
*	�� �� ��: Touch_Scan
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ����ɨ��
*	˵    ��: �ڳ����������Եĵ��øú��������Լ�ⴥ��������������Ϣ�洢�� touchInfo �ṹ��
******************************************************************************************/

void Touch_Scan(void)
{
 	uint8_t  touchData[2 + 8 * TOUCH_MAX ]; 		// ���ڴ洢��������
	uint8_t  i = 0;	
	
	GT9XX_ReadReg (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	// ������
	GT9XX_WriteData (GT9XX_READ_ADDR,0);									//	�������оƬ�ļĴ�����־λ
	touchInfo.num = touchData[0] & 0x0f;									// ȡ��ǰ�Ĵ�������
	
	if ( (touchInfo.num >= 1) && (touchInfo.num <=5) ) 	//	���������� 1-5 ֮��ʱ
	{
		for(i=0;i<touchInfo.num;i++)		// ȡ��Ӧ�Ĵ�������
		{
			touchInfo.y[i] = (touchData[5+8*i]<<8) | touchData[4+8*i];	// ��ȡY����
			touchInfo.x[i] = (touchData[3+8*i]<<8) | touchData[2+8*i];	//	��ȡX����			
		}
		touchInfo.flag = 1;	// ������־λ��1�������д�����������
	}
	else                       
	{
		touchInfo.flag = 0;	// ������־λ��0���޴�������
	}
}

/******************************************************************************************/

	
									
