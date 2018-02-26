#include <cstdlib>
#include <cstring>
#include <cctype>
#include "utilities.h"

namespace llib{

	

	void mirrorYAxis(Vector *v,int height){
		v->y=height-v->y;
	};
	void clampCoordinate(Vector *v, const Vector* img_size){
		v->x=v->x<0?0:v->x;
		v->y=v->y<0?0:v->y;
		
		v->x=(v->x)>(img_size->x)?(img_size->x):(v->x);
		v->y=(v->y)>(img_size->y)?(img_size->y):(v->y);
	}

	int isNumber(const char* str){
		for(int i=0;i<strlen(str);i++){
			if(!isdigit(str[i])) return 0;
		}
		return 1;
	}
}