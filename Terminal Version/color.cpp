#include <cstdlib>
#include <cstring>
#include "color.h"
#include "error_message.h"

namespace llib{
	

	int colorEqual(Color *c1, Color *c2){
		return RGBtoInt(c1)==RGBtoInt(c2);
	}
	unsigned int RGBtoInt(Color *color){
		return ((color->r)<<16)+((color->g)<<8)+(color->b);
	}
	void colorCopy(Color *source, Color *destination){
		destination->r=source->r;
		destination->g=source->g;
		destination->b=source->b;

	}
    const char * const colorsName[] = {"red","green","blue"};
	const Color colorsVal[] = {{255,0,0},{0,255,0},{0,0,255}};
	const Color* strToColor(char* str){
		for(int i=0;i<3;i++){
			if(strcmp(colorsName[i],str)==0){
				return &colorsVal[i];
			}
		}
		err_message=ERR_UNDEFINED_COLORNAME;
		return &colorsVal[0];
	}
}
