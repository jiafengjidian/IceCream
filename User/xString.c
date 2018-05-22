#include "xString.h"

static const char HexList[] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F',
};

uint8_t xGetNumberLength(uint32_t number, uint8_t base)
{
	uint8_t len = 1;
	while (number /= base) {
		len ++;
	}
	return len;
}

uint8_t xStrValueOfUInt(char *buffer, uint32_t number, uint8_t base)
{
	uint8_t i;
	uint8_t len = xGetNumberLength(number, base);
	for (i = 0; i < len; i ++) {
		buffer[len - i - 1] = HexList[number % base];
		number /= base;
	}
	return len;
}

uint8_t xStrCopy(char *buffer, const char *str) 
{
	uint8_t i = 0;
	while (*str != '\0') {
		*buffer = *str;
		buffer ++;
		str ++;
		i ++;
	}
	*buffer = '\0';
	return i;
}

void vStrFormat(char *buffer, const char *format, ...)
{
	char c;
	uint8_t len;
	va_list ap;
	
	va_start(ap, format);
	
	while ((c = *format) != '\0')
	{
		if (c != '%') {
			*buffer = c;
			buffer ++;
			format ++;
			continue;
		}
		c = * (format + 1);
		switch (c)
		{
			case 'd': 
				len = xStrValueOfUInt(buffer, va_arg(ap, uint32_t), 10); 
				buffer += len; 
				format += 2;
				break;
			
			case 'x': 
				len = xStrValueOfUInt(buffer, va_arg(ap, uint32_t), 16); 
				buffer += len; 
				format += 2;
				break;
			
			case 's': 
				len = xStrCopy(buffer, va_arg(ap, const char *)); 
				buffer += len; 
				format += 2;
				break;
			
			default: 
				*buffer = '%'; 
				format += 1;
				buffer += 1;
				break;
		}
	}
	*buffer = '\0';
}











