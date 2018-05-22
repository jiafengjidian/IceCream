#include "Task/MotoControlTask.h"
#include "Device/DeviceMoto.h"
#include "Periph/PeriphTimer.h"
#include "BluetoothProtocol.h"
#include <math.h>
#include "Task/StatusCheckTask.h"
#include "Device/Device74HC165.h"

#define PI				3.1415926

static osThreadId_t MotoTaskId = NULL;

static void AppMotoControlTask(void *arg);
static void vControlRobot1(int32_t angle);
static void vControlRobot2(int32_t angle);

static uint64_t MotoTaskStack[1024/8];

static const osThreadAttr_t MotoControlAttr = 
{
	"MotoControlTask", // ��������
	osThreadDetached, // 
	NULL, // �ڴ���ƿ���ϵͳ�ں˹���
	0, // �ں˿��ƿ��С
	MotoTaskStack, 
	sizeof(MotoTaskStack),
	osPriorityISR, // ���ȼ�
	0,
	0,
};

void vMotoControlTaskStart(const MotoRobotType *robotType)
{
	MotoTaskId = osThreadNew(AppMotoControlTask, (void *) robotType, &MotoControlAttr);
}


void vMotoControlTaskStop(void)
{
	if (MotoTaskId == NULL) {
		return;
	}
	osThreadTerminate(MotoTaskId);
	MotoTaskId = NULL;
}

static uint32_t getAbs(int32_t a, int32_t b)
{
	if (a > b) {
		return a - b;
	} else {
		return b - a;
	}
}

static __inline uint32_t getSpeed1(int32_t abs)
{
	return 40 * sin(0.0015162167f * abs) + 38;
}

static __inline uint32_t getSpeed(int32_t abs) 
{	
	return 100 * sin(0.0015162167f * abs) + 35;
}

static void vControlRobot1(int32_t angle)
{
	int32_t value;
	uint32_t abs;
	
	vMotoDisableAll();
	osDelay(200);
	vMotoSelect(ROBOT1);
	
	while (1)
	{
		value = xGetRobot1Cycle();
		if (value < angle) {
			vMotoSetDir(CW);
		} else {
			vMotoSetDir(CCW);
		}
		abs = getAbs(value, angle);
		vMotoSetSpeed(getSpeed1(abs));
		if (abs == 0) 
		{
			vMotoSetDir(PAUSE);
			return;
		}
		osDelay(20);
	}
}

static void vControlRobot2(int32_t angle)
{
	int32_t value;
	uint32_t abs;
	
	vMotoDisableAll();
	osDelay(200);
	vMotoSelect(ROBOT2);
	
	while (1)
	{
		value = xGetRobot2Cycle();
		if (value < angle) {
			vMotoSetDir(CW);
		} else {
			vMotoSetDir(CCW);
		}
		abs = getAbs(value, angle);
		vMotoSetSpeed(getSpeed(abs));
		if (abs == 0) 
		{
			vMotoSetDir(PAUSE);
			return;
		}
		osDelay(20);
	}	
}

void vDoorMotoDown(void)
{
	uint8_t data;
	uint16_t count = 0;
	
	data = x74HC165ReadByte();
	if ((data & 0x02) == 0x00) {
		return;
	}
	
	vTimer4CntReset();
	vMotoDisableAll();
	osDelay(200);
	vMotoSelect(REF);
	vMotoSetDir(CCW);
	vMotoSetSpeed(50);
	
//	while (1)
//	{
//		osDelay(20);
//		count = xTimer4GetCnt();
//		if (count > 300) {
//			break;
//		}
//	}
//	osDelay(3000);
	//vMotoSetDir(PAUSE);
	//vMotoSetSpeed(5);
	while (1)
	{
		osDelay(20);
		count ++;
		data = x74HC165ReadByte();
		if ((data & 0x02) == 0x00) {
			vMotoDisableAll();
			osDelay(200);
			vMotoSetDir(PAUSE);
			return;
		}
	}	
}

/**
 * 门上开关: 1, 
 * 门下开关: 2, 未压住为1 压住为0
 */
void vDoorMotoUp(void)
{
	uint8_t data;
	uint16_t speed = 100;
	uint16_t count = 0;
	
	data = x74HC165ReadByte();
	if ((data & 0x04) == 0x00) {
		return;
	}	
	
	vTimer4CntReset();
	vMotoDisableAll();
	osDelay(200);
	vMotoSelect(REF);
	vMotoSetDir(CW);
	vMotoSetSpeed(speed);
	 // 539

	while (1)
	{
		osDelay(20);
		count = xTimer4GetCnt();
		if (count > 470) {
			break;
		}
	}
	vMotoSetSpeed(35);
	while (1)
	{
		data = x74HC165ReadByte();
		if ((data & 0x04) == 0x00) {
			vMotoDisableAll();
			osDelay(200);
			vMotoSetDir(PAUSE);
			return;
		}
		osDelay(20);
	}
}

void vShipmentMotoDown(uint32_t steps)
{
	uint16_t data;
	
	vTimer1CntReset();
	vMotoDisableAll();
	osDelay(200);
	vMotoSelect(SHIPMENT);
	vMotoSetDir(CCW);
	vMotoSetSpeed(10);
	
	while (1)
	{
		data = xTimer1GetCnt();
		if (data > steps) {
			return;
		}
		osDelay(10);
	}
}

/**
 * @brief ������ 2313
 * ����������Ϸ� 1100
 * 
 */
void vShipmentMotoUp(void)
{
	uint16_t data;
	vMotoDisableAll();
	osDelay(200);
	vMotoSelect(SHIPMENT);
	vMotoSetDir(CW);
	vMotoSetSpeed(10);
	while (1)
	{
		data = x74HC165ReadByte();
		if (0 == (data & 0x30)) {
			return;
		}
		osDelay(20);
	}
}

float xPIDControlExec(PIDObject *pid, int32_t current)
{
	float pValue, iValue, dValue;
	float err = pid->target - current; // 获取误差
	pid->integralError += err; // 积分累加
	pValue = err * pid->kp; // 计算比例环节
	iValue = pid->integralError * pid->ki; // 计算积分环节
	dValue = (err - pid->lastError) * pid->kd; // 计算微分环节
	pid->lastError = err; // 保存本次误差
	return pValue + iValue + dValue;
}

/**
 * @brief arg 角度
 */
static void AppMotoControlTask(void *arg)
{
	const MotoRobotType *robotType = (MotoRobotType *) arg;
	const int32_t r1 = robotType->robot1Angle;
	const int32_t r2 = robotType->robot2Angle;
	const int32_t l1 = robotType->steps;
//	PIDObject pid = {
//		.kp = 1.0f,
//		.ki = 0.0f,
//		.kd = 0.0f,
//		.target = 0,
//		.integralError = 0,
//		.lastError = 0,
//	};
//	float u;
//	int32_t v;
//	vMotoDisableAll();
//	osDelay(200);
//	vMotoSelect(ROBOT1);
//	while (1)
//	{
//		v = xGetRobot1Cycle();
//		v = r1 - v;
//		if (v == 0) {
//			break;
//		}
//		u = xPIDControlExec(&pid, v);
//		if (u > 0) {
//			vMotoSetDir(CCW);
//			v = (int32_t) u;
//		} else {
//			vMotoSetDir(CW);
//			v = (int32_t) (-u);
//		}
//		if (v > 100) {
//			v = 100;
//		}
//		if (v < 30) {
//			v = 30;
//		}
//		vMotoSetSpeed(v);
//		osDelay(20);
//	}
	
//	vControlRobot1(518);
//	vControlRobot2(518);
//	vControlRobot1(518);
//	
//	if ((r1 == 518) && (r2 == 518)) {
//		MotoTaskId = NULL;
//		osThreadExit();
//	}
//	
//	vShipmentMotoUp();
//	vDoorMotoUp();
//	
//	if ((r1 > 400) && (r1 < 600))
//	{
//		vControlRobot1(r1);
//		vControlRobot2(r2);
//		vControlRobot1(r1);
//	}
//	else
//	{
//		vControlRobot2(r2);
//		vControlRobot1(r1);
//		vControlRobot1(r1);
//	}
//	
//	if (r1 != 518 || r2 != 518)
//	{
//		vMotoDisableAll();
//		osDelay(500);
//		vMotoSelect(SHIPMENT);
//		vMotoSetDir(CCW);
//		vMotoSetSpeed(10);
//	}
	
//	vShipmentMotoDown(l1);
//	vShipmentMotoUp();
	
//	vControlRobot1(518);
//	vControlRobot2(518);
//	vControlRobot1(518);
	
	vDoorMotoDown();
	
	MotoTaskId = NULL;
	osThreadExit();
}







