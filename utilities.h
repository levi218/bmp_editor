#include <cstdlib>

namespace llib{
#ifndef S_VECTOR
#define S_VECTOR
	typedef struct Vector{
		unsigned int x;
		unsigned int y;
	}Vector;
#endif

		
	void mirrorYAxis(Vector *v,int height);
	void clampCoordinate(Vector *v, const Vector* img_size);

	int isNumber(const char* str);
}