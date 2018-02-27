#include <cstdlib>
#include <cstring>

namespace llib{

	#pragma pack(1)
#ifndef S_COLOR
#define S_COLOR
    typedef struct Color{
        unsigned char r;
        unsigned char g;
        unsigned char b;
	} Color;
#endif
	unsigned int RGBtoInt(Color *color);
	int colorEqual(Color *c1, Color *c2);
	void colorCopy(Color *source, Color *destination);
	const Color* strToColor(char* str);
}
