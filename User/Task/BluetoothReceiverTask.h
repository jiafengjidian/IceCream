#ifndef		__BLUETOOTH_RECEIVER_TASK_H
#define		__BLUETOOTH_RECEIVER_TASK_H

#ifdef		__cplusplus
 extern "C" {
#endif

	#include "rtx_os.h"

	void vBluetoothTaskStart(void);

	extern osThreadId_t BluetoothTaskId; // ��������������ID
	extern osMessageQueueId_t BluetoothReceiverMQId; // ����������Ϣ����
	
	typedef struct {
	
		uint8_t type; // Э������
		const uint8_t *byteArrayArg; // ����
	} BluetoothProtocol; 
	
	typedef struct {
		int32_t robot1Angle; // ��е��1�Ƕ����� -1��ʾ����Ҫ����
		int32_t robot2Angle; // ��е��2�Ƕ����� -1��ʾ����Ҫ����
		int32_t steps; // �½���λ��
	} MotoRobotType;

#ifdef		__cplusplus
 }
#endif

#endif
