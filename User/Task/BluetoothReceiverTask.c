#include "Task/BluetoothReceiverTask.h"
#include "Device/DeviceBluetooth.h"
#include <stdarg.h>
#include "xString.h"
#include "Task/MotoControlTask.h"
#include "BluetoothProtocol.h"
#include "Device/DeviceMoto.h"
#include "Task/MotoUpDownControlTask.h"

/**
 * @brief ����BLE���ݴ�������
 * 1. ��������ַ�
 * 2. �������ݽ���
 * 3.
 */

#define MQ_COUNT			100
#define MQ_SIZE				sizeof (BluetoothReceiverType)

static uint64_t BluetoothTaskStack[1024]; // 8k byte
static MotoRobotType MotoRobotData; // ��е�۵����������
static void AppBluetoothTask(void *arg);

osThreadId_t BluetoothTaskId; // ��������������ID
osMessageQueueId_t BluetoothReceiverMQId; // ����������Ϣ����

const uint8_t buff[] = {0x0a, 0x00};

static osThreadAttr_t BluetoothTaskAttr = { // BluetoothTaskId ����������
	"BluetoothTask", // ��������
	osThreadDetached, // 
	NULL, // �ڴ���ƿ���ϵͳ�ں˹���
	0, // �ں˿��ƿ��С
	BluetoothTaskStack, 
	sizeof(BluetoothTaskStack),
	osPriorityHigh7, // ���ȼ�
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





