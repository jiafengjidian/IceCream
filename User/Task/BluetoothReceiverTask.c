#include "Task/BluetoothReceiverTask.h"
#include "Device/DeviceBluetooth.h"
#include <stdarg.h>
#include "xString.h"
#include "Task/MotoControlTask.h"
#include "BluetoothProtocol.h"
#include "Device/DeviceMoto.h"
#include "Task/MotoUpDownControlTask.h"

/**
 * @brief 蓝牙BLE数据处理任务
 * 1. 负责任务分发
 * 2. 命令数据解析
 * 3.
 */

#define MQ_COUNT			100
#define MQ_SIZE				sizeof (BluetoothReceiverType)

static uint64_t BluetoothTaskStack[1024]; // 8k byte
static MotoRobotType MotoRobotData; // 机械臂电机控制数据
static void AppBluetoothTask(void *arg);

osThreadId_t BluetoothTaskId; // 蓝牙接收者任务ID
osMessageQueueId_t BluetoothReceiverMQId; // 蓝牙数据消息队列

const uint8_t buff[] = {0x0a, 0x00};

static osThreadAttr_t BluetoothTaskAttr = { // BluetoothTaskId 的任务属性
	"BluetoothTask", // 任务名称
	osThreadDetached, // 
	NULL, // 内存控制块由系统内核管理
	0, // 内核控制块大小
	BluetoothTaskStack, 
	sizeof(BluetoothTaskStack),
	osPriorityHigh7, // 优先级
	0,
	0,
};
 
void vBluetoothTaskStart(void)
{
	BluetoothReceiverMQId = osMessageQueueNew(MQ_COUNT, MQ_SIZE, NULL);
	BluetoothTaskId = osThreadNew(AppBluetoothTask, NULL, &BluetoothTaskAttr);
}

static void onRobotAngle(const BluetoothReceiverType *recv)
{
	MotoRobotData.robot1Angle = xGetAngleData1(recv);
	MotoRobotData.robot2Angle = xGetAngleData2(recv);
	MotoRobotData.steps = xGetSteps(recv);
	vMotoControlTaskStop();
	vMotoControlTaskStart(&MotoRobotData);
}

static void onMotoUpDown(const BluetoothReceiverType *recv)
{
	vMotoUpDownTaskStart(xGetMotoCmd(recv));
}

static void AppBluetoothTask(void *arg)
{
	BluetoothReceiverType recv;
	BluetoothProtocolId protocolId;
	//vBluetoothInit();
	
	while (1)
	{
		osMessageQueueGet(BluetoothReceiverMQId, &recv, NULL, osWaitForever);
		protocolId = xParseBluetoothProtocol(&recv);
		
		switch (protocolId) 
		{
			case ROBOT_ANGLE: 
				onRobotAngle(&recv);
				break;
			
			case MOTO_UP_DOWN:
				onMotoUpDown(&recv);
				break;
		}
	}
}





