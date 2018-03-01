#include <cstdlib>
#include <cctype>
#include <climits>
#include "color.h"
#include "utilities.h"

#pragma pack(1)
namespace llib{
        #ifndef S_HEADER
        #define S_HEADER
        typedef struct FileHeader{
                unsigned short header_field;
                unsigned int file_size;
                unsigned short reserved1;
                unsigned short reserved2;
                unsigned int bitmap_offset;
        } FileHeader;

        typedef struct Header{
                FileHeader file_header;
                unsigned int header_size;
                unsigned int img_width;
                unsigned int img_height;
                unsigned short planes;
                unsigned short bpp;
                unsigned short compression;
                unsigned int img_size;
                int res_horizontal;
                int res_vertical;
                unsigned int color_palette;
                unsigned int important_colors;
        } Header;


        typedef struct Node{
                unsigned int count;
                Color *color;
                struct Node *next;
        } Node;

        typedef Color** Bitmap;
        #endif
	namespace BMPLib {
                void count(Node **head,Color *color);

                void drawRectangle(Bitmap bitmap, const Vector *top_left, const Vector *bot_right, Color *color, int thickness);
                void swapRegions(Bitmap bitmap, const Vector *top_left, const Vector *bot_right);
                void replaceMostCommon(Bitmap bitmap, const Vector *img_size, Color *replace_by);

                void readBitmap(const char* fileName, Header **header, Bitmap *bitmap);
                void writeBitmap(const char* fileName, Header *header, Bitmap bitmap);
                void freeFile(Header **header, Bitmap *bitmap);
	};
}
