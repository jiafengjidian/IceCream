#include "BluetoothProtocol.h"

/**
 * @brief Bluetooth与手机App通用协议
 * @Header 0xB1
 * @Length 本条协议的数据长度(最多20)
 * @Type   协议类型, 最高位为1表示本条数据从主控板发送到App, 最高位为0表示本条数据从App发送到主控板, [6:0]:表示协议ID号
 * @Arg1   参数1(可选的) 参数部分的字节最高位不能为1, 如果参数大于127必须多传一个参数
 * @Arg2
 * @Argn   参数n
 * @XorCheck 异或校验(对前面所有的数据进行异或校验的结果)
 * @EnaFlag 0xED 
 */
 
 /**
  * @brief 创建通用协议
  */
void vCreateGPProtocol(uint8_t *protocolBuffer, const uint8_t *content, uint8_t size)
{
	uint8_t check = 0;
	uint8_t i;
	
	protocolBuffer[0] = 0xB1; // 消息头
	check ^= protocolBuffer[0];
	
	protocolBuffer[1] = size + 4; // 数据长度 + 头部字节 + 尾部字节 + 校验字节 + 长度字节
	check ^= protocolBuffer[1];
	
	// 拷贝协议类型 协议内容
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
 * @brief 主控板上传磁编码器角度数据协议
 * 0xB1 0x09 0x81 (磁编码器1角度数据高7位) (磁编码器1角度数据低7位) (磁编码器2角度数据高7位) (磁编码器2角度数据低7位) (异或校验) 0xED
 * protocolBuffer: 至少9个字节的缓冲区
 * sData1: 磁编码器1的数据值
 * sData2: 磁编码器2的数据值
 */
void vCreateUpdateSensOstepDataProtocol(uint8_t *protocolBuffer, uint16_t sData1, uint16_t sData2)
{
	uint8_t temp[] = {
		0x81, // 协议类型
		sData1 >> 7,
		sData1 & 0x7F,
		sData2 >> 7,
		sData2 & 0x7F,
	};
	vCreateGPProtocol(protocolBuffer, temp, 5);
}

/**
 * @brief 对蓝牙接收到的数据进行解析
 * @return 返回协议类型, 解析失败返回NONE
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



















