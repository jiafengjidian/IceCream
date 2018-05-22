#include "Task/StatusCheckTask.h"
#include <rtx_os.h>
#include "Device/Device74HC165.h"
#include "BluetoothProtocol.h"
#include "Device/DeviceBluetooth.h"
#include "Device/DeviceMoto.h"

static osThreadId_t StatusCheckTaskId = NULL;
static uint64_t StatusCheckTaskStack[512 / 8];
static const osThreadAttr_t StatusCheckTaskAttr = 
{
	"StatusCheckTask", // 任务名称
	osThreadDetached, // 
	NULL, // 内存控制块由系统内核管理
	0, // 内核控制块大小
	StatusCheckTaskStack, 
	sizeof(StatusCheckTaskStack),
	osPriorityLow7, // 优先级
	0,
	0,
};

void AppStatusCheckTask(void *arg);

void vStopStatusCheckTask(void)
{
	if (StatusCheckTaskId == NULL) 
	{
		return;
	}
	osThreadTerminate(StatusCheckTaskId);
	StatusCheckTaskId = NULL;
}

void vStartStatusCheckTask(void)
{
	vStopStatusCheckTask();
	StatusCheckTaskId = osThreadNew(AppStatusCheckTask, NULL, &StatusCheckTaskAttr);
}

void AppStatusCheckTask(void *arg)
{
	uint8_t byte;
	uint8_t buffer[7];
	uint8_t i;
	while (1)
	{
		for (i = 0; i < 5; i ++)
		{
			byte = x74HC165ReadByte();
			osDelay(100);
		}
		vCreateReportStatusDataProtocol(buffer, byte);
		vBluetoothWriteByteArray(buffer, 7);
	}
}




