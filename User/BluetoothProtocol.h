#ifndef		__BLUETOOTH_PROTOCOL_H
#define		__BLUETOOTH_PROTOCOL_H

#ifdef		__cplusplus 
 extern "C" {
#endif

	#include <stdint.h>
	#include "Device/DeviceBluetooth.h"
	 
	typedef enum {
		ROBOT_ANGLE = 0x02, // �Ƕȿ�������
		MOTO_UP_DOWN = 0x03, // ���������������
		NONE,
	} BluetoothProtocolId;
	 
	void vCreateUpdateSensOstepDataProtocol(uint8_t *protocolBuffer, uint16_t sData1, uint16_t sData2);
	BluetoothProtocolId xParseBluetoothProtocol(const BluetoothReceiverType* recv);
	void vCreateReportSensOstepDataProtocol(uint8_t *protocolBuffer, uint8_t isSuccess);
	void vCreateReportStatusDataProtocol(uint8_t *protocolBuffer, uint8_t status);
	
	#define xGetAngleData1(recv)					((recv->byteArray[3] << 7) + recv->byteArray[4]) // ��ȡ��е�۵��1�ĽǶ�
	#define xGetAngleData2(recv)					((recv->byteArray[5] << 7) + recv->byteArray[6]) // ��ȡ��е�۵��2�ĽǶ�
	#define xGetSteps(recv)							((recv->byteArray[7] << 7) + recv->byteArray[8])
	#define xGetMotoCmd(recv)						((recv->byteArray[3]))
	
#ifdef		__cplusplus 
 }
#endif

#endif
