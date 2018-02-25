#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <limits.h>
#define ERR_UNDEFINED_COLORNAME "Undefined color name\n"
#define ERR_ARG_FORM "Argument's form not correct\n"
#define ERR_FILE_NOT_FOUND "File not found\n"

#define MODE_DRAW_REC 1
#define MODE_REPLACE 2
#define MODE_SWAP 3
#define MODE_PRINT_HELP 4
char *err_message=NULL;
#pragma pack(1)
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

typedef struct Color{
	unsigned char b;
	unsigned char g;
	unsigned char r;
} Color;
typedef struct Vector{
	unsigned int x;
	unsigned int y;
} Vector;

typedef struct Node{
	unsigned int count;
	Color *color;
	struct Node *next;
} Node;
typedef Color** Bitmap;
unsigned int RGBtoInt(Color *color){
	return ((color->r)<<16)+((color->g)<<8)+(color->b);
}
int colorEqual(Color *c1, Color *c2){
	return RGBtoInt(c1)==RGBtoInt(c2);
}
void count(Node **head,Color *color){
	if(*head==NULL){
		*head = (Node*) malloc(sizeof(Node));
		(*head)->color=color;
		(*head)->count=1;
		(*head)->next=NULL;
		return;
	}
	Node *p=*head;
	while(p->next!=NULL){
		if(colorEqual(p->color,color)){
			p->count++;
			return;
		}
		p=p->next;
	}
	if(colorEqual(p->color,color)){
		p->count++;
	}else{
		Node *ele = (Node*) malloc(sizeof(Node));
		ele->color=color;
		ele->count=1;
		ele->next=NULL;
		p->next=ele;
	}
}
void colorCopy(Color *source, Color *destination){
	destination->r=source->r;
	destination->g=source->g;
	destination->b=source->b;

}

void mirrorYAxis(Vector *v,int height){
	v->y=height-v->y;
}
void clampCoordinate(Vector *v, const Vector* img_size){
	v->x=v->x<0?0:v->x;
	v->y=v->y<0?0:v->y;
	
	v->x=(v->x)>(img_size->x)?(img_size->x):(v->x);
	v->y=(v->y)>(img_size->y)?(img_size->y):(v->y);
}
// DRAW RECTANGLE
// To draw filled rectangle, input thickness with MAX_INT
void drawRectangle(Bitmap bitmap, Vector top_left, Vector bot_right, Color color, int thickness){

	for(int i=bot_right.y;i<top_left.y;i++){
			for(int j=top_left.x;j<bot_right.x;j++){
				if((i-bot_right.y<thickness)||(top_left.y-i<thickness+1)||(j-top_left.x<thickness)||(bot_right.x-j<thickness+1)){
					colorCopy(&color,(*(bitmap+i)+j));
				}
			}
	}	
}

void swapRegions(Bitmap bitmap, Vector top_left, Vector bot_right){
	Vector pivot = {(bot_right.x+top_left.x)/2,(bot_right.y+top_left.y)/2};
	int w_region = pivot.x-top_left.x;
	int h_region = top_left.y-pivot.y;

	Color *temp = (Color*) malloc(sizeof(Color));
	for(int i=pivot.y;i<top_left.y;i++){
		for(int j=top_left.x;j<pivot.x;j++){
			colorCopy((*(bitmap+i)+j),temp);
			colorCopy((*(bitmap+i)+j+w_region),(*(bitmap+i)+j));
			colorCopy((*(bitmap+i-h_region)+j+w_region),(*(bitmap+i)+j+w_region));
			colorCopy((*(bitmap+i-h_region)+j),(*(bitmap+i-h_region)+j+w_region));
			colorCopy(temp,(*(bitmap+i-h_region)+j));			
		}
	}
	free(temp);
}

void replaceMostCommon(Bitmap bitmap, Vector img_size, Color replace_by){

	Node *head=NULL;

	for(int i=0;i<img_size.y;i++){
		for(int j=0;j<img_size.x;j++){
			count(&head,(*(bitmap+i)+j));
		}
	}
	Node *p=head;
	unsigned int max=0;
	Color *most_common = (Color*) malloc(sizeof(Color));
	while(p!=NULL){
		if((p->count)>max){
			max=p->count;
			colorCopy(p->color,most_common);
		}
		p=p->next;
	}
	for(int i=0;i<img_size.y;i++){
		for(int j=0;j<img_size.x;j++){
			if(colorEqual((*(bitmap+i)+j),most_common)){
				colorCopy(&replace_by,(*(bitmap+i)+j));	
			}
			
		}
	}
}
void readBitmap(const char* fileName, Header **header, Bitmap *bitmap){
	FILE *file;
	file = fopen(fileName,"rb+");
	if(file==NULL) err_message=ERR_FILE_NOT_FOUND;
	*header = (Header*) malloc(sizeof(Header));
	fread((*header),sizeof(Header),1,file);
	
	int row_size = ((*header)->bpp*((*header)->img_width)+31)/32*4;
	
	fseek(file,((*header)->file_header).bitmap_offset,SEEK_SET);
	char* data = (char*) malloc(sizeof(char)*row_size*((*header)->img_height));
	*bitmap = (Bitmap) malloc(sizeof(Color*)*((*header)->img_height));
	for(int i=0;i<((*header)->img_height);i++){
		*((*bitmap)+i)=(Color*) ((void*)((char*)data+i*row_size));
		fread(*((*bitmap)+i),row_size,1,file);
	}	
	fclose(file);
}

void writeBitmap(const char* fileName, Header *header, Bitmap bitmap){
	FILE *file;
	file = fopen("test1.bmp","rb+");
	//WRITE
	int row_size = (header->bpp*(header->img_width)+31)/32*4;
	fseek(file,(header->file_header).bitmap_offset,SEEK_SET);
	for(int i=0;i<(header->img_height);i++){
		fwrite(*(bitmap+i),row_size,1,file);
	}
	fflush(file);
	fclose(file);	
}
void freeFile(Header *header, Bitmap bitmap){
	free((char*)*(bitmap+0));
	free(bitmap);
	free(header);
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
int isNumber(const char* str){
	for(int i=0;i<strlen(str);i++){
		if(!isdigit(str[i])) return 0;
	}
	return 1;
}

void printHelp(){
	printf("BMP Editor\n");
	printf("Usage: bmp_editor.out -r [-f -a -b -c -t] fileName\n");
	printf("   or: bmp_editor.out -p [-c] fileName\n");
	printf("   or: bmp_editor.out -s [-a -b] fileName\n");

	printf("\nArguments:\n");
	printf("%-4s %20s %40s","-r","--rectangle","Set mode to drawing rectangle");
	printf("%-4s %20s %40s","-p","--replace","Set mode to replacing the most common color to the chosen color, if -c not listed then the default color (black) is used");
	printf("%-4s %20s %40s","-s","--swap","Swap 4 pieces of area. The rectangular area, selected by the user through the arguments -a and -b, is divided into 4 parts and these parts are interchanged.");
	printf("%-4s %20s %40s","-h","--help","Print help (this message) then exit");
	printf("\n");
	printf("%-4s %20s %40s","-a","--start x y","Coordinate of the upper left corner of the selected area.");
	printf("%-4s %20s %40s","-b","--end x y","Coordinate of the bottom right corner of the selected area");
	printf("%-4s %20s %40s","-f","--filled","Put this option to draw a filled rectangle in mode -r");
	printf("%-4s %20s %40s","-c","--color color-name","The specified color to use in mode -r and -p. Supported colors (red, blue, green)");
	printf("%-4s %20s %40s","-c","--color r g b","The specified color to use in mode -r and -p. r,g,b in the order are color code for red, green, blue channels. 0<=r,g,b<=255");
	printf("%-4s %20s %40s","-t","--thickness","The thickness of the line used to draw the rectangle in mode -r");

}

int main(int argc, char **argv){

    int c;
    int filled=0;
    int thickness=1;
    Vector top_left={0,0};
	Vector bot_right={0,0};
	Color color = {0,0,0};
    short mode=4;
    char *fileName;
    while (1)
    {
        static struct option long_options[] =
        {
            //{"verbose", no_argument,       &verbose_flag, 1},
            {"filled",   no_argument, NULL, 'f'},
        
            {"rectangle",     no_argument,       NULL, 'r'},
            {"replace",  no_argument, NULL, 'p'},
            {"swap",    no_argument, NULL, 's'},
            {"help",    no_argument, NULL, 'h'},
            
            {"start",  required_argument, NULL, 'a'},
            {"end",  required_argument, NULL, 'b'},
            {"color",    required_argument, NULL, 'c'},
            {"thickness",    required_argument, NULL, 't'},
            
            {0, 0, 0, 0}
        };
        int option_index = 0;

	    c = getopt_long (argc, argv, "rpsha:b:c:t:",
	                       long_options, &option_index);

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
		        	if(isNumber(optarg)&&isNumber(argv[optind++])){
		        		 x = atoi(optarg);
		        		 y = atoi(argv[optind]);
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
		        	if(isNumber(optarg)&&isNumber(argv[optind++])){
		        		 x = atoi(optarg);
		        		 y = atoi(argv[optind]);
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
	        			if((optind+2<argc)&&isNumber(argv[optind+1])&&isNumber(argv[optind+2])){
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
	          	//if (long_options[option_index].flag != 0)
	          	//	break;
	          	//c=long_options[option_index].val;
				//printf ("option %s", long_options[option_index].name);
	          //if (optarg)
	          //  printf (" with arg %s", optarg);
	          //printf ("\n");
	          //break;

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
	        	mode = 4;
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

    Header *header;
	Bitmap bitmap;
	readBitmap("test1.bmp",&header,&bitmap);
	
	if(err_message!=NULL) {
    	printf("%s",err_message);
    	return 0;
    }


    Vector img_size = {header->img_width,header->img_height};
	clampCoordinate(&top_left,img_size);
	clampCoordinate(&bot_right,img_size);
	
	mirrorYAxis(&top_left,header->img_height);
	mirrorYAxis(&bot_right,header->img_height);

	switch(mode){
    	case MODE_DRAW_REC:
    		if(filled)
    		{
    			drawRectangle(bitmap,top_left,bot_right,color,INT_MAX);
    		}else{
    			drawRectangle(bitmap,top_left,bot_right,color,thickness);	
    		}
			break;
		case MODE_SWAP:
			swapRegions(bitmap,top_left,bot_right);
			break;
		case MODE_REPLACE:
			replaceMostCommon(bitmap,img_size,color);
			break;
		case MODE_PRINT_HELP:
			printHelp();
			break;
		default:
			printf("Command not recognized");
			break;
    }

	writeBitmap("test1.bmp",header,bitmap);
	freeFile(header,bitmap);
	return 0;
}