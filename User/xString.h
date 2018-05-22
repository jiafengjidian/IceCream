#ifndef		__XSTRING_H
#define		__XSTRING_H

#ifdef		__cplusplus
 extern "C" {
#endif
	
	#include <stdint.h>
	#include <stdarg.h>
	
	void vStrFormat(char *buffer, const char *format, ...);

	uint8_t xStrCopy(char *buffer, const char *str) ;
	uint8_t xStrValueOfUInt(char *buffer, uint32_t number, uint8_t base);
	
	 
#ifdef		__cplusplus
 }
#endif

#endif
