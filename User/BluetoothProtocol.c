#include "BluetoothProtocol.h"

/**
 * @brief Bluetooth���ֻ�Appͨ��Э��
 * @Header 0xB1
 * @Length ����Э������ݳ���(���20)
 * @Type   Э������, ���λΪ1��ʾ�������ݴ����ذ巢�͵�App, ���λΪ0��ʾ�������ݴ�App���͵����ذ�, [6:0]:��ʾЭ��ID��
 * @Arg1   ����1(��ѡ��) �������ֵ��ֽ����λ����Ϊ1, �����������127����ഫһ������
 * @Arg2
 * @Argn   ����n
 * @XorCheck ���У��(��ǰ�����е����ݽ������У��Ľ��)
 * @EnaFlag 0xED 
 */
 
 /**
  * @brief ����ͨ��Э��
  */
void vCreateGPProtocol(uint8_t *protocolBuffer, const uint8_t *content, uint8_t size)
{
	uint8_t check = 0;
	uint8_t i;
	
	protocolBuffer[0] = 0xB1; // ��Ϣͷ
	check ^= protocolBuffer[0];
	
	protocolBuffer[1] = size + 4; // ���ݳ��� + ͷ���ֽ� + β���ֽ� + У���ֽ� + �����ֽ�
	check ^= protocolBuffer[1];
	
	// ����Э������ Э������
	for (i = 0; i < size; i ++) 
	{
		protocolBuffer[i + 2] = content[i];
		check ^= content[i];
	}
	
	protocolBuffer[size + 2] = check;
	
	protocolBuffer[size + 3] = 0xED;
}

void vCreateReportStatusDataProtocol(uint8_t *protocolBuffer, uint8_t status)
{
	uint8_t temp[] = 
	{
		0x84, status & 0x80, status & 0x7F,
	};
	vCreateGPProtocol(protocolBuffer, temp, 3);
}

void vCreateReportSensOstepDataProtocol(uint8_t *protocolBuffer, uint8_t isSuccess)
{
	uint8_t temp[] = {
		0x82,
		isSuccess,
	};
	vCreateGPProtocol(protocolBuffer, temp, 2);
}

/**
 * @brief ���ذ��ϴ��ű������Ƕ�����Э��
 * 0xB1 0x09 0x81 (�ű�����1�Ƕ����ݸ�7λ) (�ű�����1�Ƕ����ݵ�7λ) (�ű�����2�Ƕ����ݸ�7λ) (�ű�����2�Ƕ����ݵ�7λ) (���У��) 0xED
 * protocolBuffer: ����9���ֽڵĻ�����
 * sData1: �ű�����1������ֵ
 * sData2: �ű�����2������ֵ
 */
void vCreateUpdateSensOstepDataProtocol(uint8_t *protocolBuffer, uint16_t sData1, uint16_t sData2)
{
	uint8_t temp[] = {
		0x81, // Э������
		sData1 >> 7,
		sData1 & 0x7F,
		sData2 >> 7,
		sData2 & 0x7F,
	};
	vCreateGPProtocol(protocolBuffer, temp, 5);
}

/**
 * @brief ���������յ������ݽ��н���
 * @return ����Э������, ����ʧ�ܷ���NONE
 */
BluetoothProtocolId xParseBluetoothProtocol(const BluetoothReceiverType* recv)
{
	const uint8_t *buffer = recv->byteArray;
	uint32_t len = recv->length;
	uint8_t i;
	uint8_t check = 0;
	
	if (len < 5) {
		return NONE;
	}
	
	if (buffer[1] != len) {
		return NONE;
	}
	
	if ((buffer[0] != 0xB1) || (buffer[len - 1] != 0xED)) {
		return NONE;
	}
	
	for (i = 0; i < len - 2; i ++) {
		check ^= buffer[i];
	}
	
	if (check != buffer[len - 2]) {
		return NONE;
	}
	
	return (BluetoothProtocolId) buffer[2];
}



















