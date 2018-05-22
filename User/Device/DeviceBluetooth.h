#ifndef		__DEVICE_BLUETOOTH_H
#define		__DEVICE_BLUETOOTH_H

#ifdef		__cplusplus
 extern "C" {
#endif
	 
	#include "SysTimer.h"
	
	typedef struct {
		const uint8_t *byteArray;
		uint32_t length;
	} BluetoothReceiverType;
	 
	void vBluetoothInit(void);
	 
	void vBluetoothWriteByte(uint8_t byte);

	void vBluetoothWriteByteArray(const uint8_t *byteArray, uint16_t length);

	void vBluetoothWriteString(const char *string);
	
	void vBluetoothWrite(const BluetoothReceiverType *type);

#ifdef		__cplusplus
 }
#endif

#endif
