#ifndef		__DEVICE_DEVICE_LCD_H
#define		__DEVICE_DEVICE_LCD_H

#ifdef		__cplusplus
 extern "C" {
#endif

	#include "Periph/PeriphFsmc.h"

	#define LcdWriteRegister(r, d)	FsmcWriteData(0, r); FsmcWriteData(0x20000, d)
	#define LcdWriteCommand(r)		FsmcWriteData(0, r)
	#define LcdWriteData(d)			FsmcWriteData(0x20000, d)
	#define LcdReadCommand()		FsmcReadData(0)
    #define LcdReadData()			FsmcReadData(0x20000)
	 
	void vLcdInit(void);
	void vLcdFullRect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color);
	void vLcdDisplayChar(uint16_t x0, uint16_t y0, uint16_t index);
	void vLcdDisplayAscii(uint16_t x0, uint16_t y0, char c);
	void vLcdDisplayAsciiString(uint16_t x0, uint16_t y0, const char *str);
	
#ifdef		__cplusplus
 }
#endif
 
#endif
