#include "Task/Ch451sTask.h"
#include "Device/DeviceCh451s.h"
#include "Task/TimerTask.h"
#include "Periph/PeriphTimer.h"


static uint64_t Ch451sTaskStack[1024 / 8];

static void AppCh451sTask(void *arg);

static osThreadAttr_t Ch451sAttr = {
	"Ch451sTask", // CH451S的任务名称
	osThreadDetached,
	NULL,
	0,
	Ch451sTaskStack,
	sizeof (Ch451sTaskStack),
	osPriorityNormal,
	0,
	0,
};

osThreadId_t Ch451sTaskId;

void vCh451sTaskStart(void)
{
	Ch451sTaskId = osThreadNew(AppCh451sTask, NULL, &Ch451sAttr);
}

static void AppCh451sTask(void *arg)
{
	uint8_t i;
	vCh451sInit();
	vTimerTaskStart();
	osDelay(200);
	vTimerRobotInit();
	while (1)
	{
		for (i = 0; i < 8; i ++) 
		{
			vCh451sDisplay(i);
			osDelay(200);
		}
	}
}








