#ifndef		__BLUETOOTH_RECEIVER_TASK_H
#define		__BLUETOOTH_RECEIVER_TASK_H

#ifdef		__cplusplus
 extern "C" {
#endif

	#include "rtx_os.h"

	void vBluetoothTaskStart(void);

	extern osThreadId_t BluetoothTaskId; // 蓝牙接收者任务ID
	extern osMessageQueueId_t BluetoothReceiverMQId; // 蓝牙数据消息队列
	
	typedef struct {
	
		uint8_t type; // 协议类型
		const uint8_t *byteArrayArg; // 参数
	} BluetoothProtocol; 
	
	typedef struct {
		int32_t robot1Angle; // 机械臂1角度数据 -1表示不需要动作
		int32_t robot2Angle; // 机械臂2角度数据 -1表示不需要动作
		int32_t steps; // 下降的位置
	} MotoRobotType;

#ifdef		__cplusplus
 }
#endif

#endif
