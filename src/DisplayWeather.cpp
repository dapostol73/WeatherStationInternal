#include "DisplayWeather.h"

// 'snowflake_mask_sml', 9x10px
const uint8_t snowflake_mask_sml [] PROGMEM = {
	0x00, 0x00, 0x1c, 0x49, 0x7b, 0x54, 0x20, 0x00, 0x00, 
	0x00, 0x48, 0x71, 0xc7, 0xde, 0xcf, 0x76, 0x4c, 0x00, 
	0x5c, 0xd8, 0x72, 0x0b, 0xe0, 0x14, 0x65, 0xdd, 0x65, 
	0x95, 0xff, 0xe8, 0x47, 0xce, 0x48, 0xd7, 0xff, 0xa3, 
	0x8c, 0x55, 0x5e, 0xdc, 0x62, 0xe1, 0x6c, 0x4f, 0x8d, 
	0x8a, 0x47, 0x54, 0xdf, 0x5e, 0xe3, 0x64, 0x41, 0x8a, 
	0x92, 0xff, 0xea, 0x52, 0xd0, 0x54, 0xdb, 0xff, 0x9f, 
	0x57, 0xde, 0x76, 0x01, 0xda, 0x0c, 0x67, 0xe1, 0x5f, 
	0x00, 0x4e, 0x76, 0xc7, 0xe2, 0xcc, 0x7a, 0x4d, 0x00, 
	0x00, 0x00, 0x24, 0x56, 0x7e, 0x5d, 0x28, 0x00, 0x00
};

// 'snowflake_mask_med', 19x21px
const uint8_t snowflake_mask_med [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b, 0xda, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x89, 0x04, 0x38, 0xf7, 0x5e, 0x00, 0x87, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x17, 0x0d, 0x42, 0xf7, 0xc7, 0x70, 0xf5, 0x80, 0xaa, 0xf7, 0x5e, 0x04, 0x1a, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xe2, 0xdf, 0x00, 0x7b, 0xf7, 0xf7, 0xf5, 0xf7, 0xf7, 0x96, 0x00, 0xc1, 0xf7, 0x02, 0x00, 0x00, 
	0x06, 0x1e, 0x00, 0xb6, 0xf7, 0x10, 0x00, 0x8a, 0xf7, 0xf7, 0xf7, 0xa2, 0x00, 0x00, 0xf7, 0xd8, 0x00, 0x1c, 0x0b, 
	0x7f, 0xf7, 0x7e, 0x7e, 0xf7, 0x63, 0x00, 0x00, 0x91, 0xf7, 0xa8, 0x00, 0x00, 0x41, 0xf7, 0x90, 0x71, 0xf7, 0xa3, 
	0x3a, 0xd1, 0xf7, 0xf7, 0xf7, 0xab, 0x00, 0x00, 0x26, 0xf7, 0x4d, 0x00, 0x00, 0x8c, 0xf7, 0xf5, 0xf7, 0xdc, 0x4a, 
	0x00, 0x10, 0xbc, 0xf7, 0xf7, 0xf7, 0x41, 0x0b, 0x99, 0xf7, 0xb0, 0x0c, 0x30, 0xed, 0xf7, 0xf7, 0xcc, 0x20, 0x00, 
	0xa8, 0xec, 0xf7, 0xf7, 0xf5, 0xf7, 0xf7, 0xe4, 0xf7, 0xf7, 0xf7, 0xe5, 0xf7, 0xf7, 0xf4, 0xf7, 0xf7, 0xef, 0xb4, 
	0xd0, 0xec, 0x9a, 0x50, 0x1b, 0x36, 0xda, 0xf7, 0x9a, 0x27, 0x83, 0xf7, 0xed, 0x43, 0x1a, 0x49, 0x91, 0xe5, 0xdd, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xf7, 0x08, 0x00, 0x00, 0xf7, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xb2, 0xc4, 0x79, 0x39, 0x0a, 0x24, 0xc8, 0xf7, 0x88, 0x19, 0x70, 0xf7, 0xe1, 0x30, 0x09, 0x33, 0x70, 0xbe, 0xbb, 
	0xc1, 0xf7, 0xf7, 0xf7, 0xe3, 0xf7, 0xf7, 0xf4, 0xf7, 0xf3, 0xf7, 0xf5, 0xf7, 0xf7, 0xe3, 0xf7, 0xf7, 0xf7, 0xce, 
	0x00, 0x13, 0xc1, 0xf7, 0xf7, 0xf7, 0x55, 0x1b, 0xa7, 0xf7, 0xbc, 0x1d, 0x43, 0xf7, 0xf7, 0xf7, 0xd0, 0x24, 0x00, 
	0x27, 0xa9, 0xf7, 0xf7, 0xf7, 0xb3, 0x00, 0x00, 0x28, 0xf7, 0x4e, 0x00, 0x00, 0x95, 0xf7, 0xf7, 0xf7, 0xb6, 0x34, 
	0x85, 0xf7, 0x97, 0x88, 0xf7, 0x6d, 0x00, 0x00, 0x81, 0xf7, 0x9c, 0x00, 0x00, 0x4a, 0xf7, 0x97, 0x89, 0xf7, 0xab, 
	0x11, 0x37, 0x00, 0xa0, 0xf7, 0x19, 0x00, 0x72, 0xf7, 0xf7, 0xf7, 0x8a, 0x00, 0x05, 0xf7, 0xc3, 0x00, 0x34, 0x19, 
	0x00, 0x00, 0x00, 0xf4, 0xf6, 0x00, 0x62, 0xf7, 0xf7, 0xf5, 0xf7, 0xf7, 0x7d, 0x00, 0xd7, 0xf7, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x2d, 0x24, 0x3c, 0xf7, 0xdd, 0x83, 0xf5, 0x8f, 0xc1, 0xf7, 0x56, 0x16, 0x32, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x2e, 0xa9, 0x13, 0x34, 0xf7, 0x59, 0x00, 0xa6, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xdf, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'snowflake_mask_lrg', 38x41px
const uint8_t snowflake_mask_lrg [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0xde, 0xf6, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0xf6, 0xf6, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x2f, 0x00, 0x00, 0x00, 0xad, 0xf6, 0xf6, 0xe2, 0x08, 0x00, 0x00, 0x22, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5d, 0xf6, 0xf6, 0x9c, 0x00, 0x00, 0xa6, 0xf6, 0xf6, 0xdb, 0x00, 0x00, 0x73, 0xf6, 0xf6, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0xf6, 0xf6, 0xf6, 0x8f, 0x00, 0x9b, 0xf6, 0xf6, 0xd6, 0x00, 0x54, 0xf6, 0xf6, 0xf6, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x42, 0x00, 0x00, 0x5d, 0xf6, 0xf6, 0xf6, 0xf6, 0x7c, 0x8b, 0xf6, 0xf6, 0xb8, 0x74, 0xf6, 0xf6, 0xf6, 0xf6, 0x87, 0x00, 0x00, 0x31, 0x56, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0xf6, 0xf6, 0x8d, 0x00, 0x00, 0x63, 0xf6, 0xf6, 0xf6, 0xf2, 0xef, 0xf6, 0xf6, 0xf0, 0xf0, 0xf6, 0xf6, 0xf6, 0x94, 0x00, 0x00, 0x57, 0xf6, 0xf6, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xf6, 0xf6, 0xd1, 0x00, 0x00, 0x00, 0x74, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x9d, 0x00, 0x00, 0x00, 0xb3, 0xf6, 0xf6, 0xd9, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0xf6, 0xf6, 0xf6, 0x20, 0x00, 0x00, 0x00, 0x62, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x90, 0x00, 0x00, 0x00, 0x00, 0xd8, 0xf6, 0xf6, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x21, 0x9e, 0x59, 0x00, 0x00, 0x4e, 0xf6, 0xf6, 0xf6, 0x64, 0x00, 0x00, 0x00, 0x00, 0x60, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x91, 0x00, 0x00, 0x00, 0x00, 0x2d, 0xf6, 0xf6, 0xf6, 0x6b, 0x00, 0x00, 0x3d, 0xa6, 0x38, 0x00, 
	0x00, 0xc2, 0xf6, 0xf6, 0xba, 0x19, 0x04, 0xe4, 0xf6, 0xf6, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5d, 0xf6, 0xf6, 0xf6, 0xf6, 0x8b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0xf6, 0xf6, 0xf6, 0x2f, 0x00, 0x9c, 0xf6, 0xf6, 0xef, 0x20, 
	0x14, 0xee, 0xf6, 0xf6, 0xf6, 0xf6, 0x80, 0xab, 0xf6, 0xf6, 0xed, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xf6, 0xf6, 0xdf, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0xf6, 0xf6, 0xd4, 0x81, 0xf0, 0xf6, 0xf6, 0xf6, 0xf6, 0x48, 
	0x00, 0x70, 0xf6, 0xf6, 0xf6, 0xf6, 0xf4, 0xf2, 0xf6, 0xf6, 0xf6, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0xf6, 0xf6, 0xd9, 0x02, 0x00, 0x00, 0x00, 0x00, 0x24, 0xf6, 0xf6, 0xf6, 0xf3, 0xf3, 0xf6, 0xf6, 0xf6, 0xf6, 0x9e, 0x00, 
	0x00, 0x00, 0x33, 0xba, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x8a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xf6, 0xf6, 0xd9, 0x01, 0x00, 0x00, 0x00, 0x00, 0x60, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xd1, 0x51, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x39, 0xf3, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xdd, 0x26, 0x00, 0x00, 0x00, 0x0d, 0xd4, 0xf6, 0xf6, 0xf6, 0x34, 0x00, 0x00, 0x00, 0x08, 0xc3, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x62, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x05, 0x44, 0x8a, 0xa3, 0xe4, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xa4, 0x1f, 0x80, 0xf5, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xa7, 0x1a, 0x7b, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf3, 0xac, 0x90, 0x57, 0x0d, 0x00, 
	0x48, 0xee, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf5, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf5, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x6e, 
	0xcc, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xd6, 0x7d, 0xb3, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xbc, 0xab, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xce, 0x79, 0xc3, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xda, 
	0x9f, 0xf6, 0xf6, 0xf6, 0xd8, 0x88, 0x46, 0x1c, 0x02, 0x00, 0x00, 0x38, 0xd1, 0xf6, 0xf6, 0xf6, 0xea, 0x58, 0x00, 0x00, 0x38, 0xd3, 0xf6, 0xf6, 0xf6, 0xed, 0x56, 0x00, 0x00, 0x00, 0x15, 0x3b, 0x78, 0xc9, 0xf6, 0xf6, 0xf6, 0xc1, 
	0x07, 0x78, 0x8a, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0xf6, 0xf6, 0xf6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x44, 0xf6, 0xf6, 0xf6, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x79, 0x8d, 0x1f, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xf6, 0xf6, 0xf6, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x40, 0xf6, 0xf6, 0xf6, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3e, 0x4e, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xf0, 0xf6, 0xf6, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x40, 0xf6, 0xf6, 0xf6, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x4e, 0x05, 
	0x77, 0xf6, 0xf6, 0xe5, 0x99, 0x4d, 0x23, 0x03, 0x00, 0x00, 0x00, 0x12, 0xa5, 0xf6, 0xf6, 0xf6, 0xcf, 0x2b, 0x00, 0x00, 0x13, 0xb0, 0xf6, 0xf6, 0xf6, 0xcc, 0x29, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x41, 0x87, 0xd8, 0xf6, 0xf6, 0xa2, 
	0xcf, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xe8, 0x9c, 0x3e, 0x82, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x95, 0x7c, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xa6, 0x3a, 0x86, 0xdb, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xdb, 
	0x76, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x9f, 
	0x00, 0x2d, 0x87, 0xc1, 0xc7, 0xe6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xc8, 0x57, 0xa7, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xc7, 0x51, 0xa6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf0, 0xca, 0xc6, 0x98, 0x40, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0xe4, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf5, 0x5c, 0x00, 0x00, 0x00, 0x32, 0xeb, 0xf6, 0xf6, 0xf6, 0x5a, 0x00, 0x00, 0x00, 0x36, 0xe1, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x44, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x09, 0x7f, 0xe8, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0xf6, 0xf6, 0xda, 0x02, 0x00, 0x00, 0x00, 0x00, 0x6e, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x9e, 0x1e, 0x00, 0x00, 
	0x00, 0x41, 0xf4, 0xf6, 0xf6, 0xf6, 0xf6, 0xf5, 0xf6, 0xf6, 0xf6, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0xf6, 0xf6, 0xda, 0x02, 0x00, 0x00, 0x00, 0x00, 0x2d, 0xf6, 0xf6, 0xf6, 0xf5, 0xf5, 0xf6, 0xf6, 0xf6, 0xf6, 0x6a, 0x00, 
	0x10, 0xeb, 0xf6, 0xf6, 0xf6, 0xf6, 0xb0, 0xb8, 0xf6, 0xf6, 0xf6, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0xf6, 0xf6, 0xdb, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0xf6, 0xf6, 0xd3, 0xae, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x45, 
	0x06, 0xdd, 0xf6, 0xf6, 0xe0, 0x4e, 0x02, 0xcc, 0xf6, 0xf6, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0xf6, 0xf6, 0xf6, 0xf6, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0xf6, 0xf6, 0xf6, 0x25, 0x20, 0xcc, 0xf6, 0xf6, 0xf6, 0x37, 
	0x00, 0x41, 0xdd, 0x96, 0x07, 0x00, 0x47, 0xf6, 0xf6, 0xf6, 0x71, 0x00, 0x00, 0x00, 0x00, 0x31, 0xeb, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x60, 0x00, 0x00, 0x00, 0x00, 0x39, 0xf6, 0xf6, 0xf6, 0x65, 0x00, 0x00, 0x74, 0xe6, 0x63, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0xf6, 0xf6, 0xf6, 0x2b, 0x00, 0x00, 0x00, 0x38, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x62, 0x00, 0x00, 0x00, 0x05, 0xde, 0xf6, 0xf6, 0x97, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xf6, 0xf6, 0xd9, 0x01, 0x00, 0x00, 0x49, 0xf5, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0x74, 0x00, 0x00, 0x00, 0xbe, 0xf6, 0xf6, 0xd6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xf6, 0xf6, 0xa7, 0x00, 0x00, 0x37, 0xf5, 0xf6, 0xf6, 0xf6, 0xf5, 0xf6, 0xf6, 0xf5, 0xf6, 0xf6, 0xf6, 0xf6, 0x63, 0x00, 0x00, 0x75, 0xf6, 0xf6, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x86, 0x81, 0x07, 0x00, 0x40, 0xf6, 0xf6, 0xf6, 0xf6, 0xa3, 0xa0, 0xf6, 0xf6, 0xc0, 0x9f, 0xf6, 0xf6, 0xf6, 0xf6, 0x67, 0x00, 0x00, 0x64, 0x9e, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0xf6, 0xf6, 0xf6, 0xbd, 0x00, 0x93, 0xf6, 0xf6, 0xd0, 0x00, 0x83, 0xf6, 0xf6, 0xf6, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0xf6, 0xf6, 0xc7, 0x00, 0x00, 0xa6, 0xf6, 0xf6, 0xdb, 0x00, 0x00, 0x9f, 0xf6, 0xf6, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x6d, 0x05, 0x00, 0x00, 0xab, 0xf6, 0xf6, 0xe1, 0x07, 0x00, 0x00, 0x53, 0x7a, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0xf6, 0xf6, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0xf4, 0xf6, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


DisplayWeather::DisplayWeather()
{

}

void DisplayWeather::init()
{
#ifdef DISPLAY_ILI9488
	DisplayControl::init(1, &CalibriRegular8pt7b);// Default Font print line
	setFont(&CalibriBold16pt7b);// Initial Font for text	
#else
	DisplayControl::init(3, &CalibriRegular8pt7b);// Default Font print line
	setFont(&CalibriBold24pt7b);// Initial Font for text	
#endif
}

/// @brief Sun 24*24
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawSun(int16_t x, int16_t y, int16_t size, int16_t radius, uint16_t color)
{
	size = max(size, 1);
	radius *= size;
	DisplayGFX->fillCircle(x+radius, y+radius, radius, color);
}

/// @brief Moon 24*24
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawMoon(int16_t x, int16_t y, int16_t size, int16_t radius, uint16_t color)
{
	size = max(size, 1);
	radius *= size;
	DisplayGFX->fillCircle(x+radius, y+radius, radius, color);
	RGBColor rgbColor = colorRGB(color);
	rgbColor.R *= 0.75;
	rgbColor.G *= 0.75;
	rgbColor.B *= 0.75;
	uint16_t craterColor = color565(rgbColor);
	DisplayGFX->fillCircle(x+(radius*0.5), y+(radius*1.2), radius*0.2, craterColor);
	DisplayGFX->fillCircle(x+(radius*1.3), y+(radius*1.4), radius*0.18, craterColor);
	DisplayGFX->fillCircle(x+(radius*0.6), y+(radius*0.7), radius*0.1, craterColor);
	DisplayGFX->fillCircle(x+(radius*1.2), y+(radius*0.7), radius*0.3, craterColor);
}

/// @brief Asterial icon depending on day or night 24*24
/// @param x 
/// @param y 
/// @param radius 
/// @param size 
/// @param day 
void DisplayWeather::drawAsterial(int16_t x, int16_t y, int16_t radius, int16_t size, bool day)
{
	if (day)
	{
		drawSun(x, y, radius, size);
	}
	else
	{
		drawMoon(x, y, radius, size);
	}
}

/// @brief Cloud is 58*36 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawCloud(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	DisplayGFX->fillCircle(10*size+x, 25*size+y, 11*size, color);
	DisplayGFX->fillCircle(24*size+x, 12*size+y, 12*size, color);
	DisplayGFX->fillCircle(41*size+x, 16*size+y, 7*size,  color);
	DisplayGFX->fillCircle(50*size+x, 28*size+y, 8*size,  color);
	DisplayGFX->fillRoundRect(30*size+x, 10*size+y, 19*size, 16*size, 7*size, color);
	DisplayGFX->fillRoundRect(10*size+x, 26*size+y, 40*size, 11*size, 2*size, color);
	DisplayGFX->fillRect(20*size+x, 20*size+y, 20*size, 6*size, color);
}

/// @brief Thunder is 12*22 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawThunder(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	fillPolygon(4 * size + x, 0 * size + y,
				0 * size + x, 12 * size + y,
				4 * size + x, 12 * size + y,
				11 * size + x, 0 * size + y,
				color);
	fillPolygon(6 * size + x, 8 * size + y,
				12 * size + x, 8 * size + y,
				0 * size + x, 22 * size + y,
				1 * size + x, 18 * size + y,
				color);
}


void DisplayWeather::drawRainDrop(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	DisplayGFX->fillTriangle(0*size+x, 2*size+y, 2*size+x, 0*size+y, 3*size+x, 3*size+y, color);
	DisplayGFX->fillCircle(1*size+x,  4*size+y, size*2, color);
}

/// @brief Rain Heavy is 26*18 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawRainLight(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	drawRainDrop(0*size+x,  4*size+y,  size, color);//r0.d0
	
	drawRainDrop(8*size+x,  0*size+y,  size, color);//r1.d0
	drawRainDrop(5*size+x,  11*size+y, size, color);//r1.d1
	
	drawRainDrop(14*size+x, 4*size+y, size, color);//r2.d0
	drawRainDrop(12*size+x, 15*size+y, size, color);//r2.d1
	
	drawRainDrop(22*size+x, 0*size+y, size, color);//r3.d0
	drawRainDrop(20*size+x, 11*size+y, size, color);//r3.d1
	
	drawRainDrop(28*size+x, 4*size+y,  size, color);//r4.d0
}

/// @brief Rain Heavy is 34*20 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawRainHeavy(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	drawRainDrop(2*size+x, 4*size+y, size, color);//r0.d0
	drawRainDrop(0*size+x, 12*size+y, size, color);//r0.d1

	drawRainDrop(10*size+x, 0*size+y, size, color);//r1.d0
	drawRainDrop(8*size+x,  8*size+y, size, color);//r1.d1
	drawRainDrop(6*size+x,  16*size+y, size, color);//r1.d2

	drawRainDrop(16*size+x, 4*size+y, size, color);//r2.d0
	drawRainDrop(14*size+x, 12*size+y, size, color);//r2.d1
	//drawRainDrop(12*size+x, 20*size+y, size, color);//r2.d2
	
	drawRainDrop(24*size+x, 0*size+y, size, color);//r3.d0
	drawRainDrop(22*size+x, 8*size+y, size, color);//r3.d1
	drawRainDrop(20*size+x, 16*size+y, size, color);//r3.d2

	drawRainDrop(30*size+x, 4*size+y, size, color);//r4.d0
	drawRainDrop(28*size+x, 12*size+y, size, color);//r4.d1
}

/// @brief Snowflake is 9*10 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawSnowflake(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	if (size > 2)
	{
		drawMaskBitmap(x, y, 38, 41, snowflake_mask_lrg, color, BACKGROUND_COLOR, false, size-2);
	}
	else if (size == 2)
	{
		drawMaskBitmap(x, y, 19, 21, snowflake_mask_med, color, BACKGROUND_COLOR, false);
	}
	else
	{
		drawMaskBitmap(x, y, 9, 10, snowflake_mask_sml, color, BACKGROUND_COLOR, false);
	}	
}

/// @brief Snow is 32*18 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawSnow(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	drawSnowflake(x+0*size, y+2*size, size, color);
	drawSnowflake(x+12*size, y+8*size, size, color);
	drawSnowflake(x+22*size, y+0*size, size, color);
	//drawSnowflake(x+30*size, y+10*size, size, color);
}

/// @brief Fog is 52*33 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawFog(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	int16_t height = (4*size)-1;
	DisplayGFX->fillRoundRect(x+12*size, y+0*size,  16*size,  height, 2*size, color);
	DisplayGFX->fillRoundRect(x+4*size,  y+6*size,  32*size, height, 2*size, color);
	DisplayGFX->fillRoundRect(x+12*size, y+12*size, 40*size, height, 2*size, color);
	DisplayGFX->fillRoundRect(x+0*size,  y+18*size, 40*size, height, 2*size, color);
	DisplayGFX->fillRoundRect(x+8*size,  y+24*size, 40*size, height, 2*size, color);
	DisplayGFX->fillRoundRect(x+12*size, y+30*size, 24*size, height, 2*size, color);
}

/// @brief Wind is 32*22 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawWind(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	DisplayGFX->fillRoundRect(x+3*size, y+6*size, 22*size, 2*size, 1*size, color);
	fillArc(x+24*size, y+4*size, 315, 75, 4*size, 2*size, color);
	DisplayGFX->fillRoundRect(x, y+10*size, 32*size, 2*size, 1*size, color);
	DisplayGFX->fillRoundRect(x+6*size, y+14*size, 17*size, 2*size, 1*size, color);
	fillArc(x+22*size, y+18*size, 0, 75, 4*size, 2*size, color);
}

/// @brief Visibility 24*16 at size 1
/// @param x 
/// @param y 
/// @param size 
void DisplayWeather::drawVisibility(int16_t x, int16_t y, int16_t size)
{
	fillArc(x+(12*size), y+(16*size), 300, 40, 16*size, 8*size, WHITE);
	fillArc(x+(12*size), y,           120, 40, 16*size, 8*size, WHITE);
	DisplayGFX->fillCircle(x+(12*size), y+(8*size), 7*size, EYE_COLOR);
	DisplayGFX->fillCircle(x+(12*size), y+(8*size), 3*size,  BLACK);
}

/// @brief Assume 10*10 at size 1
/// @param x 
/// @param y 
/// @param direction = 0 - 359 
/// @param size 
void DisplayWeather::drawCompassArrow(int16_t x, int16_t y, int16_t direction, int16_t size)
{
	float angle = direction * DEG_TO_RAD;
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	float cx = 0.5;
	float cy = 0.5;
	float x0 = 0.5;
	float y0 = 0.1;
	float x1 = 0.5;
	float y1 = 0.7;
	float x2 = 0.3;
	float y2 = 0.85;
	float x3 = 0.7;
	float y3 = 0.85;
	uint16_t radius = 5*size;
	uint16_t diam = 2*radius;
	x -= diam*0.5;
	y -= diam*0.5;
	uint16_t cx0 = diam*(cx + (x0 - cx) * cosAngle - (y0 - cy) * sinAngle);
	uint16_t cy0 = diam*(cy + (x0 - cx) * sinAngle + (y0 - cy) * cosAngle);
	uint16_t cx1 = diam*(cx + (x1 - cx) * cosAngle - (y1 - cy) * sinAngle);
	uint16_t cy1 = diam*(cy + (x1 - cx) * sinAngle + (y1 - cy) * cosAngle);
	uint16_t cx2 = diam*(cx + (x2 - cx) * cosAngle - (y2 - cy) * sinAngle);
	uint16_t cy2 = diam*(cy + (x2 - cx) * sinAngle + (y2 - cy) * cosAngle);
	uint16_t cx3 = diam*(cx + (x3 - cx) * cosAngle - (y3 - cy) * sinAngle);
	uint16_t cy3 = diam*(cy + (x3 - cx) * sinAngle + (y3 - cy) * cosAngle);

	for (int i = 0; i < min(size, 2); i++)
	{
		DisplayGFX->drawCircle(x+radius, y+radius, radius-i, FOREGROUND_COLOR);
	}
	DisplayGFX->fillTriangle(x+cx0, y+cy0, x+cx1, y+cy1, x+cx2, y+cy2, ERROR_COLOR);
	DisplayGFX->fillTriangle(x+cx0, y+cy0, x+cx1, y+cy1, x+cx3, y+cy3, ERROR_COLOR);
}

void DisplayWeather::draw00Unknown(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 29*size;
	y -= 18*size;
	drawCloud(x, y, size);
	DisplayGFX->fillRoundRect(x+26*size, y+6*size, 6*size, 20*size, 3*size, ERROR_COLOR);
	DisplayGFX->fillCircle(x+29*size, y+30*size, 3*size, ERROR_COLOR);
}

void DisplayWeather::draw01Clear(int16_t x, int16_t y, int16_t size, bool day)
{
	int16_t radius = 24;
	drawAsterial(x-radius*size, y-radius*size, radius, size, day);
}

void DisplayWeather::draw02FewClouds(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 31*size;
	y -= 21*size;
	drawAsterial(30*size+x, y, 16, size, day);
	drawCloud(x, 6*size+y, size);
}

void DisplayWeather::draw03ScatteredClouds(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 29*size;
	y -= 18*size;
	drawCloud(x, y, size);
}

void DisplayWeather::draw04BrokenClouds(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 30*size;
	y -= 21*size;
	drawCloud(12*size+x, y, size, CLOUD_DARK_COLOR);
	drawCloud(x, 6*size+y, size);
}

void DisplayWeather::draw09ShowerRain(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 30*size;
	y -= 34*size;
	drawCloud(12*size+x, y, size, CLOUD_DARK_COLOR);
	drawCloud(x, 6*size+y, size);
	drawRainHeavy(12*size+x, 40*size+y, size);
}

void DisplayWeather::draw10Rain(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 30*size;
	y -= 30*size;
	drawAsterial(30*size+x, y, 16, size, day);
	drawCloud(x, 6*size+y, size);
	drawRainLight(12*size+x, 42*size+y, size);
}

void DisplayWeather::draw11ThunderStorm(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 30*size;
	y -= 29*size;
	drawCloud(12*size+x, y, size, CLOUD_DARK_COLOR);
	drawCloud(x, 6*size+y, size);
	drawThunder(18*size+x, 34*size+y, size);
}

void DisplayWeather::draw13Snow(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 30*size;
	y -= 31*size;
	drawCloud(12*size+x, y, size, CLOUD_DARK_COLOR);
	drawCloud(x, 6*size+y, size);
	drawSnow(12*size+x, 44*size+y, size);
}

void DisplayWeather::draw50Mist(int16_t x, int16_t y, int16_t size, bool day)
{
	x -= 26*size;
	y -= 16*size;
	drawFog(x, y, size);
}

void DisplayWeather::drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center, int16_t size)
{
	bool day = iconName[2] != 'n';
	if (iconName == "01d" || iconName == "01n") draw01Clear(x, y, size, day);
	else if (iconName == "02d" || iconName == "02n") draw02FewClouds(x, y, size, day);
	else if (iconName == "03d" || iconName == "03n") draw03ScatteredClouds(x, y, size, day);
	else if (iconName == "04d" || iconName == "04n") draw04BrokenClouds(x, y, size, day);
	else if (iconName == "09d" || iconName == "09n") draw09ShowerRain(x, y, size, day);
	else if (iconName == "10d" || iconName == "10n") draw10Rain(x, y, size, day);
	else if (iconName == "11d" || iconName == "11n") draw11ThunderStorm(x, y, size, day);
	else if (iconName == "13d" || iconName == "13n") draw13Snow(x, y, size, day);
	else if (iconName == "50d" || iconName == "50n") draw50Mist(x, y, size, day);
	else draw00Unknown(x, y, size);
}

void DisplayWeather::drawTemperatureGauge(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size)
{
}

/// @brief Temperature 12*22 at size 1
/// @param temperature 
/// @param isMetric 
/// @param x 
/// @param y 
/// @param size 
void DisplayWeather::drawTemperatureIcon(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size)
{
	uint16_t color0 = TEMP_MILD_COLOR;
	float p = 1.0;//percentage
	uint8_t o = 14;//offset
	if (isMetric)
	{
		if (temperature < -2.0)
		{
			color0 = TEMP_COLD_COLOR;
		}
		else if (temperature < 6.0)
		{
			color0 = TEMP_COOL_COLOR;
		}
		else if (temperature > 16.0)
		{
			color0 = TEMP_WARM_COLOR;
		}
		else if (temperature > 26.0)
		{
			color0 = TEMP_HOT_COLOR;
		}

		// percentage mapped -10 to 30 celsius
		p = min((temperature + 10.0) / 40.0, 1.0);		
	}
	else if (temperature > 65)
	{
		if (temperature < 32.0)
		{
			color0 = TEMP_COLD_COLOR;
		}
		else if (temperature < 52.0)
		{
			color0 = TEMP_COOL_COLOR;
		}
		else if (temperature > 84.0)
		{
			color0 = TEMP_HOT_COLOR;
		}

		// percentage mapped 14 to 86 fahrenheit
		p = min((temperature + 18.0) / 104.0, 1.0);
	}

	o = max(floor(14.0*(1.0-p)), 2);// calc offset, but no less than 2
	DisplayGFX->fillRoundRect(x+6*size, y+3*size, 6*size, size, 0.5*size, FOREGROUND_COLOR);
	DisplayGFX->fillRoundRect(x+6*size, y+6*size, 5*size, size, 0.5*size, FOREGROUND_COLOR);
	DisplayGFX->fillRoundRect(x+6*size, y+9*size, 6*size, size, 0.5*size, FOREGROUND_COLOR);
	DisplayGFX->fillRoundRect(x+3*size, y, 6*size, 16*size, 3*size, FOREGROUND_COLOR);
	DisplayGFX->fillCircle(x+6*size, y+17*size, 5*size, FOREGROUND_COLOR);
	DisplayGFX->fillRoundRect(x+4*size, y+1*size, 4*size, 14*size, 2*size, BACKGROUND_COLOR);
	DisplayGFX->fillRoundRect(x+4*size, y+(1+o)*size, 4*size, (14-o)*size, 2*size, color0);
	DisplayGFX->fillCircle(x+6*size, y+17*size, 4*size, color0);
}

void DisplayWeather::drawTemperature(float temperature, bool isMetric, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor)
{
	int16_t x1, y1 = 0;
    uint16_t w, h = 0;
    DisplayGFX->getTextBounds("0", 0, 0, &x1, &y1, &w, &h);
	char temp[6];
	dtostrf(temperature, 2, 1, temp);
	int16_t sw = w * (strlen(temp) + 2);
    switch (align)
    {
        case TEXT_LEFT_TOP:
            break;
        case TEXT_LEFT_MIDDLE:
            y -= h * 0.5;
            break;
        case TEXT_LEFT_BOTTOM:
            y -= h;
            break;
        case TEXT_CENTER_TOP:
            x -= sw * 0.5;
            break;
        case TEXT_CENTER_MIDDLE:
            x -= sw * 0.5;
            y -= h * 0.5;
            break;
        case TEXT_CENTER_BOTTOM:
            x -= sw * 0.5;
            y -= h;
            break;
        case TEXT_RIGHT_TOP:
            x -= sw + 1;
            break;
        case TEXT_RIGHT_MIDDLE:
            x -= sw + 1;
            y -= h * 0.5;
            break;
        case TEXT_RIGHT_BOTTOM:
            x -= sw + 1;
            y -= h;
            break;
        default:
            break;
    }

	drawString(temp, x, y, TEXT_LEFT_TOP, foregroundColor);
	int16_t radius = w * 0.5;
	x = DisplayGFX->getCursorX() + radius;
	DisplayGFX->drawCircle(x, y + radius-1, radius-1, foregroundColor);
	DisplayGFX->drawCircle(x, y + radius-1, radius-2, foregroundColor);
	x += radius;
	if (isMetric)
	{
		drawString("C", x, y, TEXT_LEFT_TOP, foregroundColor);
	}
	else
	{
		drawString("F", x, y, TEXT_LEFT_TOP, foregroundColor);
	}
}

void DisplayWeather::drawHumidityGauge(float humidity, int16_t x, int16_t y, int16_t size)
{

}

/// @brief Humidity 16*22 at size 1
/// @param humidity 
/// @param x 
/// @param y 
/// @param size 
void DisplayWeather::drawHumidityIcon(float humidity, int16_t x, int16_t y, int16_t size)
{
	uint16_t color = HMD_AVG_COLOR;
	if (humidity < 40)
	{
		color = HMD_DRY_COLOR;
	}
	else if (humidity > 65)
	{
		color = HMD_WET_COLOR;
	}

	DisplayGFX->fillTriangle(x, y+12*size, x+8*size, y, x+16*size, y+12*size, color);
	DisplayGFX->fillCircle(x+8*size, y+14*size, 8*size, color);
	DisplayGFX->fillCircle(x+1*size, y+13*size, size, color);
	DisplayGFX->fillCircle(x+15*size, y+13*size, size, color);

	m_gfxFontTemp = m_gfxFont;
	switch (size)
	{
		case 1:
			setFont(&CalibriBold8pt7b);
			break;
		case 2:
			setFont(&CalibriBold16pt7b);
			break;
		case 3:
			setFont(&CalibriBold24pt7b);
			break;
		default:
			setFont(&CalibriBold32pt7b);
			break;
	}
	drawString("%", x+8*size, y+8*size, TEXT_CENTER_TOP, BACKGROUND_COLOR);
	setFont(m_gfxFontTemp);
}
    
void DisplayWeather::drawHumidity(float humidity, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor)
{
	char humi[8];
	dtostrf(humidity, 2, 1, humi);
	strcat(humi, "%");
	//String humi = String(humidity, 1) + "%";
	drawString(humi, x, y, align, foregroundColor);
}

void DisplayWeather::drawSensorData(int16_t x, int16_t y, SensorData *internalSensorData, SensorData *externalSensorData)
{
	fillScreen(BACKGROUND_COLOR);
#ifdef DISPLAY_ILI9488
    y = 20;
	x = 120;
	setFont(&CalibriBold16pt7b);
	drawString("Local", 240, y + 20, TEXT_CENTER_TOP, TEXT_TITLE_COLOR);

	drawString("Inside", x, y + 60, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);
	
	drawTemperatureIcon(internalSensorData->Temp, internalSensorData->IsMetric, x - 80 - 6, y + 120 - 22, 2);
	drawTemperature(internalSensorData->Temp, internalSensorData->IsMetric, x - 40, y + 120, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
	
	drawHumidityIcon(internalSensorData->Hmd, x - 80 - 8, y + 200 - 22, 2);
	drawHumidity(internalSensorData->Hmd, x - 40, y + 200, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	x = 360;
	drawString("Outside", x, y + 60, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);

	drawTemperatureIcon(externalSensorData->Temp, externalSensorData->IsMetric, x - 80 - 6, y + 120 - 22, 2);
	drawTemperature(externalSensorData->Temp, externalSensorData->IsMetric, x - 40, y + 120, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	drawHumidityIcon(externalSensorData->Hmd, x - 80 - 8, y + 200 - 22, 2);
	drawHumidity(externalSensorData->Hmd, x - 40, y + 200, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
#else
    y = 20;
	x = 200;
	setFont(&CalibriBold24pt7b);
	drawString("Local", 400, y + 20, TEXT_CENTER_TOP, TEXT_TITLE_COLOR);

	drawString("Inside", x, y + 100, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);
	
	drawTemperatureIcon(internalSensorData->Temp, internalSensorData->IsMetric, x - 100 - 18, y + 200 - 33, 3);
	drawTemperature(internalSensorData->Temp, internalSensorData->IsMetric, x - 40, y + 200, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
	
	drawHumidityIcon(internalSensorData->Hmd, x - 100 - 24, y + 300 - 33, 3);
	drawHumidity(internalSensorData->Hmd, x - 40, y + 300, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	x = 600;
	drawString("Outside", x, y + 100, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);

	drawTemperatureIcon(externalSensorData->Temp, externalSensorData->IsMetric, x - 100 - 18, y + 200 - 33, 3);
	drawTemperature(externalSensorData->Temp, externalSensorData->IsMetric, x - 40, y + 200, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	drawHumidityIcon(externalSensorData->Hmd, x - 100 - 24, y + 300 - 33, 3);
	drawHumidity(externalSensorData->Hmd, x - 40, y + 300, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
#endif
}

/// @brief Draw current forecast, which is 20 and the bottom most is 420 which means we have and area to 
///        draw 800 wide by 400 high
/// @param currentWeather 
/// @param x 
/// @param y 
void DisplayWeather::drawCurrentWeather(OpenWeatherMapCurrentData *currentWeather, int16_t x, int16_t y)
{
	char num[8] = "";
	char vis[20] = "";
	char wspd[20] = "";

	if (currentWeather->visibility < 1000)
	{
		sprintf(vis, "%d m", currentWeather->visibility);
	}
	else
	{
		dtostrf(0.001*currentWeather->visibility, 5, 1, num);
		sprintf(vis, "%s km", num);
	}

	dtostrf(currentWeather->windSpeed*3.6, 5, 1, num);
	sprintf(wspd, "%s km/h", num);

	uint16_t windDir = roundf(currentWeather->windDeg/22.5)%16;
	char windStr[4];
	readWindDirectionString(windStr, windDir);

	fillScreen(BACKGROUND_COLOR);
#ifdef DISPLAY_ILI9488
	int16_t xIcon = 260;
	int16_t xText = 320;
	y = 20;
	drawWeatherIcon(100, y + 120, currentWeather->icon, true, 2);
	setFont(&CalibriBold16pt7b);
	drawString(currentWeather->cityName, 240, y + 20, TEXT_CENTER_TOP, TEXT_TITLE_COLOR);
	drawString(currentWeather->description, 100, y + 240, TEXT_CENTER_MIDDLE, TEXT_MAIN_COLOR);

	setFont(&CalibriBold12pt7b);
	drawTemperatureIcon(currentWeather->temp, currentWeather->isMetric, xIcon - 5, y + 80 - 11, 1);
	drawTemperature(currentWeather->temp, currentWeather->isMetric, xText, y + 80, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	drawHumidityIcon(currentWeather->humidity, xIcon - 8, y + 120 - 11, 1);
	drawHumidity(currentWeather->humidity, xText, y + 120, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
	
	// Visibility
	drawVisibility(xIcon - 12, y + 160 - 8, 1);
	drawString(vis, xText, y + 160, TEXT_LEFT_MIDDLE, TEXT_MAIN_COLOR);

	// Wind speed convert m/s to km/h by multiplying by 3.6 (60 * 60 / 1000)
	drawWind(xIcon - 16, y + 200 - 11, 1);
	drawString(wspd, xText, y + 200, TEXT_LEFT_MIDDLE, TEXT_MAIN_COLOR);

	// Wind direction
	drawCompassArrow(xIcon, y + 240, currentWeather->windDeg, 3);
	drawString(windStr, xText, y + 240, TEXT_LEFT_MIDDLE, TEXT_MAIN_COLOR);
#else
	int16_t xIcon = 480;
	int16_t xText = 560;
	y = 20;
	drawWeatherIcon(200, y + 200, currentWeather->icon, true, 3);
	setFont(&CalibriBold24pt7b);
	drawString(currentWeather->cityName, 400, y + 20, TEXT_CENTER_TOP, TEXT_TITLE_COLOR);
	drawString(currentWeather->description, 200, y + 360, TEXT_CENTER_MIDDLE, TEXT_MAIN_COLOR);
	
	drawTemperatureIcon(currentWeather->temp, currentWeather->isMetric, xIcon - 10, y + 120 - 22, 2);
	drawTemperature(currentWeather->temp, currentWeather->isMetric, xText, y + 120, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	drawHumidityIcon(currentWeather->humidity, xIcon - 16, y + 180 - 22, 2);
	drawHumidity(currentWeather->humidity, xText, y + 180, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
	
	// Visibility
	drawVisibility(xIcon - 24, y + 240 - 16, 2);
	drawString(vis, xText, y + 240, TEXT_LEFT_MIDDLE, TEXT_MAIN_COLOR);

	// Wind speed convert m/s to km/h by multiplying by 3.6 (60 * 60 / 1000)
	drawWind(xIcon - 32, y + 300 - 22, 2);
	drawString(wspd, xText, y + 300, TEXT_LEFT_MIDDLE, TEXT_MAIN_COLOR);

	// Wind direction
	drawCompassArrow(xIcon, y + 360, currentWeather->windDeg, 5);
	drawString(windStr, xText, y + 360, TEXT_LEFT_MIDDLE, TEXT_MAIN_COLOR);
#endif
}

/// @brief Draw the 1 of the 3 details forecast, we pass in the top\center of the up most
///        up most which is 20, then bottom most is 420 which means we have and area to 
///        draw 260 wide by 400 high
/// @param forecastWeather 
/// @param x 
/// @param y 
/// @param dayIndex 
void DisplayWeather::drawForecastDetails(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y, int16_t dayIndex, bool showTime = false) 
{
	time_t observationTimestamp = forecastWeather[dayIndex].observationTime;
	int16_t day = weekday(observationTimestamp)-1;
	char header[10];
	readDaysString(header, day);
	if (showTime)
	{
		header[3] = '\0';
		sprintf_P(header,
			PSTR("%s %d %s"),
			header,
			hourFormat12(observationTimestamp),
			(isAM(observationTimestamp) ? "AM" : "PM"));
	}

#ifdef DISPLAY_ILI9488
	setFont(&CalibriBold12pt7b);
	drawString(header, x, y + 20, TEXT_CENTER_TOP, TEXT_TITLE_COLOR);
	drawWeatherIcon(x, y + 120, forecastWeather[dayIndex].icon, true, 2);

	setFont(&CalibriBold8pt7b);
	
	drawTemperatureIcon(forecastWeather[dayIndex].temp, forecastWeather[dayIndex].isMetric, x - 60 - 10, y + 220 - 11, 1);
	drawTemperature(forecastWeather[dayIndex].temp, forecastWeather[dayIndex].isMetric, x - 50, y + 220, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
	
	drawHumidityIcon(forecastWeather[dayIndex].humidity, x + 8 - 8, y + 220 - 11, 1);
	drawHumidity(forecastWeather[dayIndex].humidity, x + 20, y + 220, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	drawString(forecastWeather[dayIndex].description, x, y + 240, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);
#else
	setFont(&CalibriBold24pt7b);
	drawString(header, x, y + 20, TEXT_CENTER_TOP, TEXT_TITLE_COLOR);
	drawWeatherIcon(x, y + 180, forecastWeather[dayIndex].icon, true, 3);

	setFont(&CalibriBold16pt7b);
	
	drawTemperatureIcon(forecastWeather[dayIndex].temp, forecastWeather[dayIndex].isMetric, x - 114 - 10, y + 330 - 22, 2);
	drawTemperature(forecastWeather[dayIndex].temp, forecastWeather[dayIndex].isMetric, x - 90, y + 330, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);
	
	drawHumidityIcon(forecastWeather[dayIndex].humidity, x + 16 - 16, y + 330 - 22, 2);
	drawHumidity(forecastWeather[dayIndex].humidity, x + 40, y + 330, TEXT_LEFT_MIDDLE, TEXT_ALT_COLOR);

	drawString(forecastWeather[dayIndex].description, x, y + 360, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);
#endif
}

void DisplayWeather::drawForecastHourly(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y) 
{
	fillScreen(BACKGROUND_COLOR);
#ifdef DISPLAY_ILI9488
	drawForecastDetails(forecastWeather, 70, 20, 0, true);
	drawForecastDetails(forecastWeather, 240, 20, 1, true);
	drawForecastDetails(forecastWeather, 410, 20, 2, true);	
#else
	drawForecastDetails(forecastWeather, 130, 20, 0, true);
	drawForecastDetails(forecastWeather, 400, 20, 1, true);
	drawForecastDetails(forecastWeather, 670, 20, 2, true);	
#endif
}

void DisplayWeather::drawForecastDaily(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y) 
{
	fillScreen(BACKGROUND_COLOR);
#ifdef DISPLAY_ILI9488
	drawForecastDetails(forecastWeather, 70, 20, 0);
	drawForecastDetails(forecastWeather, 240, 20, 1);
	drawForecastDetails(forecastWeather, 410, 20, 2);
#else
	drawForecastDetails(forecastWeather, 130, 20, 0);
	drawForecastDetails(forecastWeather, 400, 20, 1);
	drawForecastDetails(forecastWeather, 670, 20, 2);
#endif	
}

void DisplayWeather::drawMemoryBar(int16_t x, int16_t y, int16_t size)
{
	int fRam = mu_freeRam();
	int pRam = ((8192.0-fRam)/8192.0)*100.0;
	char ramUsed[4];
	itoa(pRam, ramUsed, 10);
	strcat(ramUsed, "%");

	switch (size)
	{
		case 1:
		case 2:
			setFont(&CalibriBold8pt7b);
			break;
		case 3:
			setFont(&CalibriBold16pt7b);
			break;
		case 4:
			setFont(&CalibriBold24pt7b);			
			break;	
		default:
			break;
	}

	// draw ram free bar
	DisplayGFX->fillRoundRect(x, y, 100*size, 6*size, 3*size, BACKGROUND_COLOR);
	DisplayGFX->fillRoundRect(x, y, pRam*size, 6*size, 3*size, pRam < 85 ? SUCCESS_COLOR : ERROR_COLOR);
	drawString(ramUsed, x+(104*size), y+(3*size), TEXT_LEFT_MIDDLE);
}

/// @brief Default size when set to 1 is 12x12
/// @param x 
/// @param y 
/// @param size 
void DisplayWeather::drawWiFiSignal(int16_t x, int16_t y, int16_t size, uint16_t backgroundColor)
{
	size = max(size, 1);
	uint16_t gray = colorLerp(FOREGROUND_COLOR, BACKGROUND_COLOR, 128);

	if (backgroundColor != UINT16_MAX)
	{
		DisplayGFX->fillRect(x, y, size*12, size*12, backgroundColor);
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		int32_t strength = WiFi.RSSI();
		DisplayGFX->fillRect(x,        y+size*9, size*2, size*3,  strength > -80 ? FOREGROUND_COLOR : gray);
		DisplayGFX->fillRect(x+size*3, y+size*6, size*2, size*6,  strength > -70 ? FOREGROUND_COLOR : gray);
		DisplayGFX->fillRect(x+size*6, y+size*3, size*2, size*9,  strength > -60 ? FOREGROUND_COLOR : gray);
		DisplayGFX->fillRect(x+size*9, y,        size*2, size*12, strength > -50 ? FOREGROUND_COLOR : gray);
	}
	else
	{
		int16_t ly = y + size * 5;
		int16_t lw = size*10;
		for (int16_t t=0; t < size * 2; t++)
		{
			DisplayGFX->drawFastHLine(x+2, ly+t, lw, ERROR_COLOR);
		}
		int16_t r = 6*size;
		int16_t cx = x+r;
		int16_t cy = y+r;
		for (int16_t t=0; t < size * 2; t++)
		{
			DisplayGFX->drawCircle(cx, cy, r-t, ERROR_COLOR);
		}		
	}
}

void DisplayWeather::drawHeader(bool externalUpdated, bool currentUpdated, bool forecastHourlyUpdated, bool forecastDailyUpdated, time_t timeUpdated)
{
	char m_lastTimeUpdated[24] = "?Unkown";
    sprintf(m_lastTimeUpdated, "%02d/%02d/%04d -- %02d:%02d:%02d", 
            month(timeUpdated), day(timeUpdated), year(timeUpdated), 
            hour(timeUpdated), minute(timeUpdated), second(timeUpdated));
	setFont(&CalibriBold8pt7b);
#ifdef DISPLAY_ILI9488
	DisplayGFX->fillRect(0, 0, 480, 18, OVERLAY_COLOR);
	DisplayGFX->drawFastHLine(0, 19, 480, TEXT_ALT_COLOR);
	DisplayGFX->drawFastHLine(0, 20, 480, TEXT_ALT_COLOR);

	drawChar(10, 10, 'E', TEXT_CENTER_MIDDLE, externalUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawChar(25, 10, 'C', TEXT_CENTER_MIDDLE, currentUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawChar(40, 10, 'H', TEXT_CENTER_MIDDLE, forecastHourlyUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawChar(55, 10, 'F', TEXT_CENTER_MIDDLE, forecastDailyUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawString(WiFi.SSID(), 240, 10, TEXT_CENTER_MIDDLE);
	drawString(m_lastTimeUpdated, 475, 10, TEXT_RIGHT_MIDDLE);

	drawMemoryBar(70, 6, 1);
	drawWiFiSignal(454, 26, 2, BACKGROUND_COLOR);
#else
	DisplayGFX->fillRect(0, 0, 800, 18, OVERLAY_COLOR);
	DisplayGFX->drawFastHLine(0, 19, 800, TEXT_ALT_COLOR);
	DisplayGFX->drawFastHLine(0, 20, 800, TEXT_ALT_COLOR);

	drawChar(10, 10, 'E', TEXT_CENTER_MIDDLE, externalUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawChar(25, 10, 'C', TEXT_CENTER_MIDDLE, currentUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawChar(40, 10, 'H', TEXT_CENTER_MIDDLE, forecastHourlyUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawChar(55, 10, 'F', TEXT_CENTER_MIDDLE, forecastDailyUpdated ? SUCCESS_COLOR : ERROR_COLOR);
	drawString(WiFi.SSID(), 400, 10, TEXT_CENTER_MIDDLE);
	drawString(m_lastTimeUpdated, 796, 10, TEXT_RIGHT_MIDDLE);

	drawMemoryBar(80, 3, 2);
	drawWiFiSignal(764, 28, 3, BACKGROUND_COLOR);
#endif
}

void DisplayWeather::drawFooter(SensorData *sensorData, OpenWeatherMapCurrentData *currentWeather)
{
	char datetime[24];
	char monthStr[4];
	readMonthString(monthStr, month()-1);
	sprintf_P(datetime,
			  PSTR("%s %d, %d  %d:%02d %s"),
			  monthStr,
			  day(),
			  year(),
			  hourFormat12(),
			  minute(),
			  (isAM() ? "AM" : "PM"));
#ifdef DISPLAY_ILI9488
	DisplayGFX->fillRect(0, 300, 480, 320, OVERLAY_COLOR); 
	DisplayGFX->drawFastHLine(0, 298, 480, TEXT_ALT_COLOR);
	DisplayGFX->drawFastHLine(0, 299, 480, TEXT_ALT_COLOR);
	setFont(&CalibriBold12pt7b);
	
	drawString(datetime, 240, 300, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);
	if (sensorData->IsUpdated)
	{
		drawTemperature(sensorData->Temp, sensorData->IsMetric, 475, 300, TEXT_RIGHT_TOP, TEXT_ALT_COLOR);
		drawHumidity(sensorData->Hmd, 5, 300, TEXT_LEFT_TOP, TEXT_ALT_COLOR);
	}
	else
	{
		drawTemperature(currentWeather->temp, currentWeather->isMetric, 475, 300, TEXT_RIGHT_TOP, TEXT_ALT_COLOR);
		drawHumidity(currentWeather->humidity, 5, 300, TEXT_LEFT_TOP, TEXT_ALT_COLOR);
	}
#else
	DisplayGFX->fillRect(0, 422, 800, 480, OVERLAY_COLOR); 
	DisplayGFX->drawFastHLine(0, 420, 800, TEXT_ALT_COLOR);
	DisplayGFX->drawFastHLine(0, 421, 800, TEXT_ALT_COLOR);
	setFont(&CalibriBold24pt7b);
	
	drawString(datetime, 400, 434, TEXT_CENTER_TOP, TEXT_MAIN_COLOR);
	if (sensorData->IsUpdated)
	{
		drawTemperature(sensorData->Temp, sensorData->IsMetric, 790, 434, TEXT_RIGHT_TOP, TEXT_ALT_COLOR);
		drawHumidity(sensorData->Hmd, 10, 434, TEXT_LEFT_TOP, TEXT_ALT_COLOR);
	}
	else
	{
		drawTemperature(currentWeather->temp, currentWeather->isMetric, 790, 434, TEXT_RIGHT_TOP, TEXT_ALT_COLOR);
		drawHumidity(currentWeather->humidity, 10, 434, TEXT_LEFT_TOP, TEXT_ALT_COLOR);
	}
#endif	
}

void DisplayWeather::printWiFiInfo()
{
	// get your MAC address
	byte mac[6];
	WiFi.macAddress(mac);
	IPAddress ip = WiFi.localIP();
	
	// SSID
	char ssidInfo[34] = "";
	sprintf(ssidInfo, "WiFi SSID: %s", WiFi.SSID());

	// MAC address
	char macInfo[34] = "";
	sprintf(macInfo, "MAC address: %02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

	// IP address
	char ipInfo[34] = "";
	sprintf(ipInfo, "IP address: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    //fillScreen(BACKGROUND_COLOR);
    printLine(ssidInfo, SUCCESS_COLOR);
    printLine(ipInfo, TEXT_MAIN_COLOR);
    printLine(macInfo, TEXT_MAIN_COLOR);
    
	#ifdef SERIAL_LOGGING
	Serial.println(ssidInfo);
	Serial.println(ipInfo);	
	Serial.println(macInfo);
	#endif
}