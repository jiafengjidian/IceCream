#ifndef		__PERIPH_GPIO_H
#define		__PERIPH_GPIO_H

#ifdef		__cplusplus
 extern "C" {
#endif

#define DIR_OUT			0x01
#define DIR_IN			0x00
#define DIR_AF			0x02	 

#define OTYPE_PP		0x00
#define OTYPE_OD		0x01
	 
#define PUPD_UP			0x01
#define PUPD_DOWN		0x02
#define	PUPD_NOPULL		0x00
	 

#define	GPIO_INIT(PORT, PIN, DIR, OTYPE, PUPD)		\
	{												\
		PORT->MODER		&= 	~ (3 << PIN * 2);		\
		PORT->MODER 	|=	  (DIR << PIN * 2);		\
		PORT->PUPDR 	&=	~ (3 << PIN * 2);		\
		PORT->PUPDR		|=	  (PUPD << PIN * 2);	\
		PORT->OSPEEDR 	|=	  (3 << PIN * 2);		\
		PORT->OTYPER 	&= 	~ (1 << PIN);			\
		PORT->OTYPER	|=	  (OTYPE << PIN);		\
	}
	
#define	GPIO_PP_OUT(PORT, PIN)						\
	{												\
		PORT->MODER		&=	~ (3 << PIN * 2);		\
		PORT->MODER		|=	  (1 << PIN * 2);		\
		PORT->PUPDR		&=	~ (3 << PIN * 2);		\
		PORT->OSPEEDR	|=	  (3 << PIN * 2);		\
		PORT->OTYPER	&=	~ (1 << PIN);			\
	}

	/**
	 * @brief 开漏输出 && 打开上拉电阻
	 */
#define GPIO_OD_OUT(PORT, PIN)						\
	{												\
		PORT->MODER		&=	~ (3 << PIN * 2);		\
		PORT->MODER		|=	  (1 << PIN * 2);		\
		PORT->PUPDR		&=	~ (3 << PIN * 2);		\
		PORT->PUPDR		|=	  (1 << PIN * 2);		\
		PORT->OSPEEDR	|=	  (3 << PIN * 2);		\
		PORT->OTYPER	|=	  (1 << PIN);			\
	}

	
	

#ifdef		__cplusplus
 }
#endif

#endif
