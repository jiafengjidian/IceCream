#ifndef		__DEVICE_MOTO_H
#define		__DEVICE_MOTO_H

#ifdef		__cplusplus
 extern "C" {
#endif
	
	typedef enum {
		ROBOT1 = 0x00, // ��е�۵��1
		ROBOT2 = 0x01, // ��е�۵��2
		REF = 0x02, // �����ŵ��
		SHIPMENT = 0x03, // ȡ�����
		AIR = 0x04, // ��ѹ��
		REV = 0x05, // йѹ��
	} MotoId;
	
	typedef enum {
		CW = 0x01, // ˳ʱ��
		CCW = 0x02, // ��ʱ��
		PAUSE = 0x04, // ֹͣ
	} MotoDir;
	
	#include "Periph/PeriphTimer.h"
	
	#define vMotoSetSpeed(x)	vTimer3SetCycle(x)
	void vMotoDisableAll(void); // ֹͣ���Ե��
	void vMotoInit(void);
	void vMotoSetDir(MotoDir dir);
	void vMotoSelect(MotoId id);

#ifdef		__cplusplus
 }
#endif

#endif
