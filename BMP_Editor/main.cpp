#include <cstdio>
#include <getopt.h>
#include "mainwindow.h"
#include <QApplication>
#include <cstdlib>
#include <cctype>
#include <climits>
#include "libs/bmplib.h"
#include "libs/color.h"
#include "libs/utilities.h"
#include "libs/error_message.h"

using namespace llib;

void printHelp();
int useGUI(int argc, char **argv){
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
int main(int argc, char **argv){
    if(argc<2){
        return useGUI(argc,argv);
    }

    int c;
    int filled=0;
    int thickness=1;
    Vector top_left={0,0};
    Vector bot_right={0,0};
    Color color = {0,0,0};
    short mode=0;
    const char *fileName=NULL;
    while (1)
    {
        static struct option long_options[10] =
        {
            {"filled",   no_argument, NULL, 'f'},

            {"rectangle",     no_argument,       NULL, 'r'},
            {"replace",  no_argument, NULL, 'p'},
            {"swap",    no_argument, NULL, 's'},
            {"help",    no_argument, NULL, 'h'},

            {"start",  required_argument, NULL, 'a'},
            {"end",  required_argument, NULL, 'b'},
            {"color",    required_argument, NULL, 'c'},
            {"thickness",    required_argument, NULL, 't'},

            {NULL, 0, NULL, 0}
        };
        int option_index = 0;

        c = getopt_long (argc, argv, "rpsfha:b:c:t:",
                           long_options, NULL);
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                break;
            case 'a':
                {
                    int x=0;
                    int y=0;
                    //printf("%s\n",argv[optind]);
                    if(isNumber(optarg)&&isNumber(argv[optind])){
                         x = atoi(optarg);
                         y = atoi(argv[optind++]);
                    }else{
                        err_message=ERR_ARG_FORM;
                    }
                    printf("Top_left: %d %d\n",x,y);
                    top_left.x=x;
                    top_left.y=y;
                }
                break;
            case 'b':
                {
                    int x=0;
                    int y=0;
                    if(isNumber(optarg)&&isNumber(argv[optind])){
                         x = atoi(optarg);
                         y = atoi(argv[optind++]);
                    }else{
                        err_message=ERR_ARG_FORM;
                    }
                    printf("Bottom_right: %d %d\n",x,y);
                    bot_right.x=x;
                    bot_right.y=y;
                }
                break;
            case 'c':
                {
                    if(!isNumber(optarg)){
                        color=(*strToColor(optarg));
                    }else{
                        if((optind+1<argc)&&isNumber(argv[optind])&&isNumber(argv[optind+1])){
                            color.r=atoi(optarg);
                            color.g=atoi(argv[optind++]);
                            color.b=atoi(argv[optind++]);
                        }else {
                            err_message=ERR_ARG_FORM;
                        }
                    }
                }
                break;
            case 't':
                {
                    if(isNumber(optarg)){
                         thickness=atoi(optarg);
                    }else{
                        err_message=ERR_ARG_FORM;
                    }
                }
                break;
            case 'f':
                thickness=INT_MAX;
                break;
            case 'r':
                mode = 1;
                break;
            case 'p':
                mode = 2;
                break;
            case 's':
                mode = 3;
                break;
            case 'h':
                {
                    printHelp();
                    return 0;
                }
                break;

            case '?':
                err_message=ERR_ARG_FORM;
                        break;

            default:
                        break;
            }
    }

        if (optind < argc)
        {
            fileName=argv[optind++];
        }else{
            err_message=ERR_FILE_NOT_FOUND;
        }
        if(err_message!=NULL) {
            printf("%s",err_message);
            return 0;
        }
        Header *header=NULL;
        Bitmap bitmap=NULL;
        BMPLib::readBitmap(fileName,&header,&bitmap);

        if(err_message!=NULL) {
        printf("%s",err_message);
        return 0;
    }


    Vector img_size = {(int)header->img_width,(int)header->img_height};
    clampCoordinate(&top_left,&img_size);
    clampCoordinate(&bot_right,&img_size);

    mirrorYAxis(&top_left,header->img_height);
    mirrorYAxis(&bot_right,header->img_height);

    switch(mode){
    case MODE_DRAW_REC:
            if(filled)
            {
                    BMPLib::drawRectangle(bitmap,&top_left,&bot_right,&color,INT_MAX);
            }else{
                    BMPLib::drawRectangle(bitmap,&top_left,&bot_right,&color,thickness);
            }
                    break;
            case MODE_SWAP:
                    BMPLib::swapRegions(bitmap,&top_left,&bot_right);
                    break;
            case MODE_REPLACE:
                    BMPLib::replaceMostCommon(bitmap,&img_size,&color);
                    break;
            case MODE_PRINT_HELP:
                    printHelp();
                    break;
            default:
                    printf("Command not recognized");
                    break;
    }

    BMPLib::writeBitmap(fileName,header,bitmap);
    BMPLib::freeFile(&header,&bitmap);
    return 0;
}

void printHelp(){
    printf("BMP Editor\n");
    printf("Usage: bmp_editor.out -r [-f -a -b -c -t] fileName\n");
    printf("   or: bmp_editor.out -p [-c] fileName\n");
    printf("   or: bmp_editor.out -s [-a -b] fileName\n");

    printf("\nArguments:\n");
    printf("%4s %-20s %-40s\n","-r","--rectangle","Set mode to drawing rectangle");
    printf("%4s %-20s %-40s\n","-p","--replace","Set mode to replacing the most common color to the chosen color, if -c not listed then the default color (black) is used");
    printf("%4s %-20s %-40s\n","-s","--swap","Swap 4 pieces of area. The rectangular area, selected by the user through the arguments -a and -b, is divided into 4 parts and these parts are interchanged.");
    printf("%4s %-20s %-40s\n","-h","--help","Print help (this message) then exit");
    printf("\n");
    printf("%4s %-20s %-40s\n","-a","--start x y","Coordinate of the upper left corner of the selected area.");
    printf("%4s %-20s %-40s\n","-b","--end x y","Coordinate of the bottom right corner of the selected area");
    printf("%4s %-20s %-40s\n","-f","--filled","Put this option to draw a filled rectangle in mode -r");
    printf("%4s %-20s %-40s\n","-c","--color color-name","The specified color to use in mode -r and -p. Supported colors (red, blue, green)");
    printf("%4s %-20s %-40s\n","-c","--color r g b","The specified color to use in mode -r and -p. r,g,b in the order are color code for red, green, blue channels. 0<=r,g,b<=255");
    printf("%4s %-20s %-40s\n","-t","--thickness","The thickness of the line used to draw the rectangle in mode -r");

}
