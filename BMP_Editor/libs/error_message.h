#include <cstdlib>
#define ERR_UNDEFINED_COLORNAME "Undefined color name\n"
#define ERR_ARG_FORM "Argument's form not correct\n"
#define ERR_FILE_NOT_FOUND "File not found\n"
#define ERR_FILE_FORMAT "File format not supported\n"

#define MODE_DRAW_REC 1
#define MODE_REPLACE 2
#define MODE_SWAP 3
#define MODE_PRINT_HELP 4
static const char *err_message=NULL;
