#include "Device/DeviceMoto.h"
#include "Task/MotoUpDownControlTask.h"
#include <rtx_os.h>
#include "Task/StatusCheckTask.h"


static void AppMotoUpDownTask(void *arg);
static osThreadId_t MotoUpDownTaskId = NULL;

void vMotoUpDownTaskStop(void)
{
	if (MotoUpDownTaskId != NULL) 
	{
		osThreadTerminate(MotoUpDownTaskId);
		MotoUpDownTaskId = NULL;
	}
}

void vMotoUpDownTaskStart(uint8_t arg)
{
	if (MotoUpDownTaskId != NULL) 
	{
		osThreadTerminate(MotoUpDownTaskId);
		MotoUpDownTaskId = NULL;
	}
	MotoUpDownTaskId = osThreadNew(AppMotoUpDownTask, (void *) arg, NULL);
}

static void AppMotoUpDownTask(void *arg)
{
	uint32_t cmd = (uint32_t) arg;
	vMotoDisableAll();
	switch (cmd)
	{
		case 0x01:
			osDelay(500);
			vMotoSelect(SHIPMENT);
			vMotoSetDir(CW);
			vMotoSetSpeed(10);
			break;
		
		case 0x02:
			break;
		
		case 0x03:
			osDelay(500);
			vMotoSelect(SHIPMENT);
			vMotoSetDir(CCW);
			vMotoSetSpeed(10);
			break;
	}
	MotoUpDownTaskId = NULL;
	osThreadExit();
}












