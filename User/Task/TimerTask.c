#include "Task/TimerTask.h"
#include "BluetoothProtocol.h"
#include "Device/DeviceBluetooth.h"
#include "Periph/PeriphTimer.h"

#define SENS_OSTEP_TICK			500 // SensOstep定时器周期

static uint8_t SendOstepProtocolBuffer[9];

static void AppSensOstepDataUpdateTimer(void *args);
static osTimerId_t SendOstepTimer;

void vTimerTaskStart(void)
{
	SendOstepTimer = osTimerNew(AppSensOstepDataUpdateTimer, osTimerPeriodic, NULL, NULL);
	if (SendOstepTimer == NULL) {
		return;
	}
	osTimerStart(SendOstepTimer, SENS_OSTEP_TICK);
}

static void AppSensOstepDataUpdateTimer(void *args)
{
	uint16_t d1 = xGetRobot1Cycle();
	uint16_t d2 = xGetRobot2Cycle();
	vCreateUpdateSensOstepDataProtocol(SendOstepProtocolBuffer, d1, d2);
	vBluetoothWriteByteArray(SendOstepProtocolBuffer, sizeof(SendOstepProtocolBuffer));
}




