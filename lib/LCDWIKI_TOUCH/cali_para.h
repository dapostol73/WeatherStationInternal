#ifndef _CALI_PARA_
#define _CALI_PARA_

//for resolution ILI9341 240x320,the calibration parameter is 663,-13,894,-30
//for resolution ILI9488, ST7796S 320x480,the calibration parameter is 852,-14,1284,-30
//for resolution NT35510 480x800,the calibration parameter is 1344,-33,2156,-45

#ifdef DISPLAY_ILI9341
	#define XFAC      663
	#define XOFFSET   -13
	#define YFAC      894
	#define YOFFSET   -30
#elif DISPLAY_ILI9488
	#define XFAC      852
	#define XOFFSET   -14
	#define YFAC      1284
	#define YOFFSET   -30
#else
	#define XFAC      1307
	#define XOFFSET   -30
	#define YFAC      2062
	#define YOFFSET   -17
#endif

#endif
