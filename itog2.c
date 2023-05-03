#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ftw.h>  
#include<sys/stat.h>
		
struct s 
{ 
unsigned char a; 
}; 
struct td 
{ 
unsigned short a; 
}; 				
			  
typedef struct KYH{
	char ch[256];
	int h;
	int l;//высота и длина
} kyh;


void sizeguf(const char* name,int array[2]);
void sizeguf(const char* name,int array[2])
{
    struct s a;
    struct td b;
    struct td c;
    FILE *f = fopen(name,"rb");
    fread(&a, sizeof(struct s), 1, f); 
    fread(&a, sizeof(struct s), 1, f); 
    fread(&a, sizeof(struct s), 1, f); 
    fread(&a, sizeof(struct s), 1, f); 
    fread(&a, sizeof(struct s), 1, f); 
    fread(&a, sizeof(struct s), 1, f); 
    fread(&b, sizeof(struct td), 1, f); 
    fread(&c, sizeof(struct td), 1, f); 
    fclose(f);
    
    array[0] = b.a;
    array[1] = c.a; 

}


			 
void sizejpeg(const char* name, int array[2]);
void sizejpeg(const char* name, int  array[2])
{

    int iHeight=0, iWidth=0, iPos, i;
    long len;
    unsigned char *ucpImageBuffer ;
    FILE *file = fopen(name,"r");
    fseek(file,0,SEEK_END);
    len = ftell(file);
    fseek(file,0,SEEK_SET);

    ucpImageBuffer = (unsigned char*) malloc (len+1);
    fread(ucpImageBuffer,1,len,file);
    fclose(file);

    for(i=0;i<len;i++)
    {
        if((ucpImageBuffer[i]==0xFF) && (ucpImageBuffer[i+1]==0xC0) )
        {
            iPos=i;         
            break;
        }       
    }   

    iPos = iPos + 5;
    iHeight = ucpImageBuffer[iPos]<<8|ucpImageBuffer[iPos+1];
    iWidth = ucpImageBuffer[iPos+2]<<8|ucpImageBuffer[iPos+3];

    array[0] = iWidth;
    array[1] = iHeight ;

    free(ucpImageBuffer);
}
			 
		  
			  
int GUF(const char *fname);
int GUF(const char *fname){
	int i;
	FILE *f;
	char r[13], a[4]={'G', 'I','F', ' '};
	f=fopen(fname,"r");
	if(!f) return 0;
	if(fscanf(f,"%d",r[0])==0) return 0;
	for(i=1;i<13;i++){
		fscanf(f,"%d",r[i]);
	}
	if(a[0]==r[8] && a[1]==r[9] && a[2]==r[10] && a[3]==r[11] ) {
        fclose(f);
        return 1;
    } 
    fclose(f);
	return 0;
}		  
			  
int JPEG(const char *fname);
int JPEG(const char *fname){
	int i;
	FILE *f;
	char r[13], a[4]={'J','P','E','G'};
	f=fopen(fname,"r");
	if(!f) return 0;
	if(fscanf(f,"%d",r[0])==0) return 0;
	for(i=1;i<13;i++){
		fscanf(f,"%d",r[i]);
	}
	if(a[0]==r[8] && a[1]==r[9] && a[2]==r[10] && a[3]==r[11] ) {
        fclose(f);
        return 1;
    } 
    fclose(f);
	return 0;
}		  
				  
			  
int osnova(const char *v);	  
int osnova(const char *v){
	DIR *t;
	int b, count, size;
	count=1;
	kyh *arr;
	int m[2];
	arr=(kyh*)malloc(sizeof(kyh)*count);
	struct dirent *d;
	struct stat *r;
	t=opendir(v);
	if(t==NULL){
		printf("NO CATALOG\n");
		return -1;
	}
	stat(v,r);
	d=readdir(t);
	if(d==NULL){
		printf("EMPTY CATALOG\n");
		return -1;
	}
	while(d!=NULL){
		if(S_ISCHR(r->st_mode)) osnova(d->d_name);
		size=r->st_size;
		size/=3;
		if(JPEG(d->d_name)==1){
			count++;
			arr=(kyh*)realloc(arr,sizeof(kyh)*(count+1));
			sizejpeg(d->d_name, m);
			(arr+count-1)->l=m[0];
			(arr+count-1)->h=m[1];
		}
		if(GUF(d->d_name)==1){
			count++;
			arr=(kyh*)realloc(arr,sizeof(kyh)*(count+1));
			sizeguf(d->d_name, m);
			(arr+count-1)->l=m[0];
			(arr+count-1)->h=m[1];
			
		}
		d=readdir(t);
		}
	if(count==1){
		printf("THERE IS NO RIGHT FILES\n");
		return -1;
	}
	for(int j=0; j<count-1;j++){
		for(int k=0;k<count-1;k++){
			if((arr+j)->h==(arr+k)->h && (arr+j)->l==(arr+k)->l){
				
			}
		}	
		printf("Name: %c\n Height: %d\n Lenth: %d\n",(arr+count-1)->ch, (arr+count-1)->h,(arr+count-1)->l);
	}	
    closedir(t);	
	return b;
}			  
			  
			  
int main(){
	char r[256];
	printf("Enter the folder: \n");
	scanf("%c",r);
	osnova(r);
	return 0;
}			  
			  
			  
