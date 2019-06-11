#include "tLib.h"

void tBitmapInit(tBitmap *bitmap){
	bitmap->bitmap = 0;
}
uint32_t tBitmapPosCount(void){
	return 32;
}   

void tBitmapSet(tBitmap *bitmap, uint32_t pos){
	bitmap->bitmap |= 1 << pos;
}

void tBitmapClear(tBitmap *bitmap, uint32_t pos){
	bitmap->bitmap &= ~(1 << pos);
}

uint32_t tBitmapGetFirstSet(tBitmap *bitmap){
	int i, mask;
	static const uint8_t quickFindTable[] = {
		/*00*/0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*10*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,   
		/*20*/5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*30*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*40*/6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*50*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*60*/5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*70*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*80*/7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*90*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*A0*/5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*B0*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*C0*/6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*D0*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*E0*/5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*F0*/4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};
	
	for(i = 0; i < 4; ++i){
		mask = 0xff << (i * 8);
		if(bitmap->bitmap & mask){
			return(quickFindTable[bitmap->bitmap >> (i * 8) & 0xff] + (i * 8));
		}
	}
	
	return quickFindTable[0];
}
