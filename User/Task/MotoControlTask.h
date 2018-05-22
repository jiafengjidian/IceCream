#ifndef		__MOTO_CONTROL_TASK_H
#define		__MOTO_CONTROL_TASK_H

#ifdef		__cplusplus
 extern "C" {
#endif

	#include "rtx_os.h"
	#include "BluetoothReceiverTask.h"
	
	typedef struct {
		float kp;
		float ki;
		float kd;
		int32_t target; // 目标值
		float integralError; // 积分环节
		float lastError; // 上一次的误差
	} PIDObject; 
	
	void vMotoControlTaskStart(const MotoRobotType *robotType);
	void vMotoControlTaskStop(void);

#ifdef		__cplusplus
 }
#endif

#endif
