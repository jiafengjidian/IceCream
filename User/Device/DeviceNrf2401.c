#include "Device/DeviceNrf2401.h"

const static uint8_t NrfTxAddress[] = {0xE5, 0xE5, 0xE5, 0xE5, 0xE5};
const static uint8_t NrfRxAddress[] = {0xE5, 0xE5, 0xE5, 0xE5, 0xE5};
const static uint8_t NrfRfChannel = 50;

static void vNrfGpioInit(void);
uint8_t xNrfReadRegister(uint8_t reg);
uint8_t vNrfSendPacket(const uint8_t *packet, uint8_t len);
uint8_t xNrfWriteRegister(uint8_t reg, uint8_t value);
uint8_t xNrfReadRegisterOfBuffer(uint8_t reg, uint8_t *buffer, uint8_t len);
uint8_t xNrfWriteRegisterOfBuffer(uint8_t reg, const uint8_t *buffer, uint8_t len);
static uint8_t xSpiTranslateByte(uint8_t byte);

#define CSLow()				GPIOD->BSRRH = 0x4000
#define CSHigh()			GPIOD->BSRRL = 0x4000

#define CELow()				GPIOD->BSRRH = 0x8000
#define CEHigh()			GPIOD->BSRRL = 0x8000

#define IrqRead()			(GPIOD->IDR & 0x200)

#define MisoRead()			(GPIOD->IDR & 0x400)

#define MosiLow()			GPIOD->BSRRH = 0x800
#define MosiHigh()			GPIOD->BSRRL = 0x800

#define SckLow()			GPIOD->BSRRH = 0x2000
#define SckHigh()			GPIOD->BSRRL = 0x2000

//NRF24L01�Ĵ�����������
#define SPI_READ_REG    0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define SPI_WRITE_REG   0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���	 
//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG          0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
                              //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define EN_AA           0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW        0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR      0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS          0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
                              //bit5:���ݷ�������ж�;bit6:���������ж�;
#define MAX_TX  	    0x10    //�ﵽ����ʹ����ж�
#define TX_OK       	0x20    //TX��������ж�
#define RX_OK   	    0x40    //���յ������ж�

#define OBSERVE_TX      0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD              0x09  //�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0      0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define FIFO_STATUS     0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
                              //bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;
#define DYNPD           0x1C  //ʹ��pipe��̬�غɳ��� 
#define FEATURE         0x1D  

void vNrf2401Init(void)
{
	vNrfGpioInit();
}

uint8_t xNrfSendPacket(const uint8_t *packet, uint8_t len)
{
	uint8_t status;
	CELow();
	xNrfWriteRegisterOfBuffer(WR_TX_PLOAD, packet, len);
	CEHigh();
	while (IrqRead() != 0); 
	status = xNrfReadRegister(STATUS);
	xNrfWriteRegister(SPI_WRITE_REG + STATUS, status);
	
	if (status & MAX_TX) {
		xNrfWriteRegister(FLUSH_TX, NOP);
		return MAX_TX;
	}
	
	if (status & TX_OK) {
		return TX_OK;
	}

	return status;
}

void vNrfEnterTxMode(void)
{
	CELow();
	
	/** ����Tx����ַ **/
	xNrfWriteRegisterOfBuffer(SPI_WRITE_REG + TX_ADDR, NrfTxAddress, sizeof(NrfTxAddress));
	xNrfWriteRegisterOfBuffer(SPI_WRITE_REG + RX_ADDR_P0, NrfRxAddress, sizeof(NrfRxAddress));
	
	// ʹ��ͨ��0�Զ�Ӧ��
	xNrfWriteRegister(SPI_WRITE_REG + EN_AA, 0x00);
	
	// ʹ��ͨ��0�Ľ��յ�ַ
	xNrfWriteRegister(SPI_WRITE_REG + EN_RXADDR, 0x01);
	
	// �����Զ��ط�ʱ���� 500us + 86us �Զ��ط����� 10
	xNrfWriteRegister(SPI_WRITE_REG + SETUP_RETR, 0x21);
	
	// ����RFͨ��
	xNrfWriteRegister(SPI_WRITE_REG + RF_CH, NrfRfChannel);
	
	// ʹ��ͨ��0��̬�غɳ���
	xNrfWriteRegister(SPI_WRITE_REG + DYNPD, 0x01);
	
	// ʹ�ܶ�̬�غɳ���
	xNrfWriteRegister(SPI_WRITE_REG + FEATURE, 0x06);
	
	// ����TX������� 0db 2Mbps ���������濪��
	xNrfWriteRegister(SPI_WRITE_REG + RF_SETUP, 0x26);
	
	// ���û�������ģʽ�Ĳ��� PWR_UP, EN_CRC, 16BIT_CRC. PRIM_RX ���������ж�
	xNrfWriteRegister(SPI_WRITE_REG + CONFIG, 0x0E);
	
	CEHigh();
}

uint8_t xNrfCheck(void)
{
	uint8_t temp[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	uint8_t ret[sizeof(temp)];
	uint8_t i;
	xNrfWriteRegisterOfBuffer(SPI_WRITE_REG + TX_ADDR, temp, sizeof(temp));
	xNrfReadRegisterOfBuffer(TX_ADDR, ret, sizeof(temp));
	for (i = 0; i < sizeof(temp); i ++) {
		if (temp[i] != ret[i]) {
			return 1;
		}
	}
	return 0;
}

uint8_t xNrfWriteRegisterOfBuffer(uint8_t reg, const uint8_t *buffer, uint8_t len)
{
	uint8_t status, i;
	CSLow();
	status = xSpiTranslateByte(reg);
	for (i = 0; i < len; i ++) {
		xSpiTranslateByte(buffer[i]);
	}
	CSHigh();
	return status;
}

uint8_t xNrfReadRegisterOfBuffer(uint8_t reg, uint8_t *buffer, uint8_t len)
{
	uint8_t status, i;
	CSLow();
	status = xSpiTranslateByte(reg);
	for (i = 0; i < len; i ++) {
		buffer[i] = xSpiTranslateByte(NOP);
	}
	CSHigh();
	return status;
}

uint8_t xNrfReadRegister(uint8_t reg)
{
	uint8_t status;
	CSLow();
	xSpiTranslateByte(reg);
	status = xSpiTranslateByte(NOP);
	CSHigh();
	return status;
}

uint8_t xNrfWriteRegister(uint8_t reg, uint8_t value)
{
	uint8_t status;
	CSLow();
	status = xSpiTranslateByte(reg);
	xSpiTranslateByte(value);
	CSHigh();
	return status;
}

static uint8_t xSpiTranslateByte(uint8_t byte)
{
	uint8_t result = 0;
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		if (byte & 0x80) {
			MosiHigh();
		} else {
			MosiLow();
		}
		SckHigh();
		byte <<= 1;
		result <<= 1;
		if (MisoRead()) {
			result ++;
		}
		SckLow();
	}
	return result;
}

/**
 * @brief NRF2401L��������
 * PD9 <-> IRQ // ��������
 * PD10 <-> MISO // ��������
 * PD11 <-> MOSI // �������
 * PD13 <-> SCK // �������
 * PD14 <-> CS // �������
 * PD15 <-> CE // �������
 */
static void vNrfGpioInit(void)
{
	RCC->AHB1ENR |= 0x08;
	GPIOD->MODER &= ~0xFCFC0000;
	GPIOD->MODER |= 0x54400000;
	GPIOD->OTYPER &= ~0xE800;
	GPIOD->PUPDR &= ~0xFCFC0000;
	GPIOD->OSPEEDR |= 0xFCFC0000;
	SckLow();
	CELow();
	CSHigh();
}
















