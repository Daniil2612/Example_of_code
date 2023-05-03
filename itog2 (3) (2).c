#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>  
#include<sys/stat.h>
#include<ftw.h>     
#include<errno.h> 
 
 
int i=0;
int j=0;
int  count1=0;
int  count2=0;  
        
struct s 
{ 
unsigned char a; 
}; 
struct td 
{ 
unsigned short a; 
};              
              
struct picture{
    char ch[100];
    int h;
    int l;//высота и длина
};
 
 
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
 
    ucpImageBuffer = (unsigned char*) malloc((size_t)(len+1));
    fread(ucpImageBuffer,1,(size_t)len,file);
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
    char *y;
    char r[10000];
    FILE* file = fopen(fname, "r");
        if (file==NULL)
    {
            printf("error\n");
        }
        fgets(r,10000, file);
        y=r;
        if((strstr(y,"GIF87a")-y==0) || (strstr(y,"GIF89a")-y==0)) return 1;        
        else return 0;
        
}         
              
int JPEG(const char *fname);
int JPEG(const char *fname){
     unsigned char bytes[3];
    FILE* file = fopen(fname, "r");
    if (file==NULL)
    {
        printf("error\n");
    }
    fread(bytes, 3, 1, file);
    if (bytes[0] == 0xff && bytes[1] == 0xd8 ) return 1;
    else return 0;
}         
    
    
void otbor(char *v, struct picture *arr1, struct picture *arr2);      
void otbor(char *v, struct picture *arr1, struct picture *arr2){
    DIR *t;
    char f[200];
    int h[2];
    struct dirent *r;
    t=opendir(v);
    while(r=readdir(t)){
    	printf("AAA\n");
    	 if((r->d_type==4) && ((strcmp(r->d_name,".")!=0) && (strcmp(r->d_name,"..")!=0))){
            v=strcat(v,"/");
            strcpy(f,v);
            strcat(f,r->d_name);
            strcat(f,"/");
            otbor(f, arr1, arr2);
        }
        if(r->d_type!=4){
            v=strcat(v,"/");
            strcpy(f,v);
            strcat(f,r->d_name);
            if(JPEG(f)==1){
                sizejpeg(f,h);
                strcpy(arr1[i].ch,r->d_name);
                printf("%s: ",arr1[i].ch);
                (arr1+i)->h=h[0];
                (arr1+i)->l=h[1];
                printf("%d multiply on %d\n", (arr1+i)->l,(arr1+i)->h);
                i++;
            }
            if(GUF(f)==1){
                sizeguf(f,h);
                strcpy(arr2[j].ch,r->d_name);
                printf("%s: ",arr2[j].ch);
                (arr2+j)->h=h[0];
                (arr2+j)->l=h[1];
                printf("%d multiply on %d\n", (arr2+j)->l,(arr2+j)->h);
                j++;
            }
        }
  //      r=readdir(t);
    }   
  for(int i1=0; i1<count1; i1++){
      for(int j1=0;j1<count2; j1++){
          if(((arr1+i1)->h==(arr2+j1)->h) && ((arr1+i1)->l==(arr1+j1)->l)){
              printf("%s and %s are equal by size. Height: %d\n Lenth: %d\n",(arr1+i1)->ch,(arr2+j1)->ch,(arr1+i1)->h,(arr1+i1)->l);
          }
      }
  }
    closedir(t);
}                      
              
void osnova(char *v);     
void osnova(char *v){
    DIR *t;
    char f[200];
    struct dirent *d;
    if(t=opendir(v)){
        printf("NO CATALOG\n");
    }
    while((d=readdir(t))!=NULL){
        if(d->d_type!=4){
            v=strcat(v,"/");
            strcpy(f,v);
            strcat(f,d->d_name);
            if(JPEG(f)==1){
                //printf("%s\n",f);
                count1++;
            }
            if(GUF(f)==1){
                //printf("%s\n",f);
                count2++;
            }
        }
        if((d->d_type==4) && ((strcmp(d->d_name,".")!=0) && (strcmp(d->d_name,"..")!=0))){
            v=strcat(v,"/");
            strcpy(f,v);
            strcat(f,d->d_name);
            strcat(f,"/");
            osnova(f);
        }
    }   
    closedir(t);
}             
              
 
int main(int argc, char **argv){
    char r[256];
    //int count1, count2;
    //count1=0;
    //count2=0;
    struct picture *arr1;
    struct picture *arr2;
    if (argc<2){
        strcpy(r,".");
    } 
    else{
        strcpy(r,argv[1]);
    }
    osnova(r);
    printf("%d %d \n", count1, count2);
    arr1=(struct picture*)malloc(count1*sizeof(struct picture));
    arr2=(struct picture*)malloc(count2*sizeof(struct picture));
    otbor(r, arr1, arr2);
    free(arr1);
    free(arr2);
    return 0;
}   			  			  
