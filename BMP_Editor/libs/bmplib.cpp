#include <cstdlib>
#include <cctype>
#include <climits>
#include <cstdio>
#include "bmplib.h"
#include "color.h"
#include "utilities.h"
#include "error_message.h"

#pragma pack(1)
namespace llib{
	
	void BMPLib::count(Node **head,Color *color){
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

	void BMPLib::drawRectangle(Bitmap bitmap, const Vector *top_left, const Vector *bot_right, Color *color, int thickness){

        for(int i=bot_right->y;i<top_left->y;i++){
                for(int j=top_left->x;j<bot_right->x;j++){
					if((i-bot_right->y<thickness)||(top_left->y-i<thickness+1)||(j-top_left->x<thickness)||(bot_right->x-j<thickness+1)){
						colorCopy(color,(*(bitmap+i)+j));
					}
				}
		}	
	}
	void BMPLib::swapRegions(Bitmap bitmap, const Vector *top_left, const Vector *bot_right){
		Vector *pivot = (Vector*)malloc(sizeof(Vector));
		pivot->x=(bot_right->x+top_left->x)/2;
		pivot->y=(bot_right->y+top_left->y)/2;
		
		int w_region = pivot->x-top_left->x;
        int h_region = top_left->y-pivot->y;

		Color *temp = (Color*) malloc(sizeof(Color));
        for(int i=pivot->y;i<top_left->y;i++){
			for(int j=top_left->x;j<pivot->x;j++){
				colorCopy((*(bitmap+i)+j),temp);
				colorCopy((*(bitmap+i)+j+w_region),(*(bitmap+i)+j));
				colorCopy((*(bitmap+i-h_region)+j+w_region),(*(bitmap+i)+j+w_region));
				colorCopy((*(bitmap+i-h_region)+j),(*(bitmap+i-h_region)+j+w_region));
				colorCopy(temp,(*(bitmap+i-h_region)+j));			
			}
		}
		free(temp);
	}
	void BMPLib::replaceMostCommon(Bitmap bitmap, const Vector *img_size, Color *replace_by){

		Node *head=NULL;

		for(int i=0;i<img_size->y;i++){
			for(int j=0;j<img_size->x;j++){
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
		for(int i=0;i<img_size->y;i++){
			for(int j=0;j<img_size->x;j++){
				if(colorEqual((*(bitmap+i)+j),most_common)){
					colorCopy(replace_by,(*(bitmap+i)+j));	
				}
				
			}
		}
	}

    void BMPLib::readBitmap(const char* fileName, Header **header, Bitmap *bitmap){
		FILE *file;
		file = fopen(fileName,"rb+");
        if(file==NULL) {err_message=ERR_FILE_NOT_FOUND; return;}
        if((*header)==NULL)
            *header = (Header*) malloc(sizeof(Header));
		fread((*header),sizeof(Header),1,file);
        if((*header)->header_size!=40) {
            err_message=ERR_FILE_FORMAT;
            return;
        }
		int row_size = ((*header)->bpp*((*header)->img_width)+31)/32*4;
		
		fseek(file,((*header)->file_header).bitmap_offset,SEEK_SET);
        char* data = NULL;
        if((*bitmap)==NULL){
            data = (char*) malloc(sizeof(char)*row_size*((*header)->img_height));
            *bitmap = (Bitmap) malloc(sizeof(Color*)*((*header)->img_height));
        }
        for(unsigned int i=0;i<((*header)->img_height);i++){
			*((*bitmap)+i)=(Color*) ((void*)((char*)data+i*row_size));
			fread(*((*bitmap)+i),row_size,1,file);
		}	
        for(unsigned int i=0;i<(*header)->img_height;i++){
            for(unsigned int j=0;j<(*header)->img_width;j++){
                unsigned char t = (*((*bitmap)+i)+j)->b;
                (*((*bitmap)+i)+j)->b = (*((*bitmap)+i)+j)->r;
                (*((*bitmap)+i)+j)->r=t;
            }
        }
        fclose(file);
	}

	void BMPLib::writeBitmap(const char* fileName, Header *header, Bitmap bitmap){
		FILE *file;
        file = fopen(fileName,"rb+");
        if(file==NULL) {err_message=ERR_FILE_NOT_FOUND; printf("%s",fileName); return;}

        //WRITE
        int row_size = (header->bpp*(header->img_width)+31)/32*4;
        fseek(file,(header->file_header).bitmap_offset,SEEK_SET);
        unsigned char t;
        for(unsigned int i=0;i<(header->img_height);i++){
            for(unsigned int j=0;j<header->img_width;j++){
                t = (*(bitmap+i)+j)->b;
                (*(bitmap+i)+j)->b = (*(bitmap+i)+j)->r;
                (*(bitmap+i)+j)->r=t;
            }
            fwrite(*(bitmap+i),row_size,1,file);
            for(unsigned int j=0;j<header->img_width;j++){
                t = (*(bitmap+i)+j)->b;
                (*(bitmap+i)+j)->b = (*(bitmap+i)+j)->r;
                (*(bitmap+i)+j)->r=t;
            }
        }
		fflush(file);
		fclose(file);	
	}
    void BMPLib::freeFile(Header **header, Bitmap *bitmap){
        free((char*)*((*bitmap)+0));
        free(*bitmap);
        free(*header);
        *header=NULL;
        bitmap=NULL;
	}
}
