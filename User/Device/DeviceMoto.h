#ifndef		__DEVICE_MOTO_H
#define		__DEVICE_MOTO_H

#ifdef		__cplusplus
 extern "C" {
#endif
	
	typedef enum {
		ROBOT1 = 0x00, // 机械臂电机1
		ROBOT2 = 0x01, // 机械臂电机2
		REF = 0x02, // 冰箱门电机
		SHIPMENT = 0x03, // 取货电机
		AIR = 0x04, // 空压机
		REV = 0x05, // 泄压阀
	} MotoId;
	
	typedef enum {
		CW = 0x01, // 顺时针
		CCW = 0x02, // 逆时针
		PAUSE = 0x04, // 停止
	} MotoDir;
	
	#include "Periph/PeriphTimer.h"
	
	#define vMotoSetSpeed(x)	vTimer3SetCycle(x)
	void vMotoDisableAll(void); // 停止所以电机
	void vMotoInit(void);
	void vMotoSetDir(MotoDir dir);
	void vMotoSelect(MotoId id);

#ifdef		__cplusplus
 }
#endif

#endif
