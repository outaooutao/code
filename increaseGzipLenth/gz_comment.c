#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
int main()
{
    int nul=0;
    char zero=(char)nul;
    int co=24;
    char com=(char)co;//允许注释


    int fd=-1;
    char filename[]="a.gz";
    int adLength=100;
    char add[100];
    int j;
    for(j=0;j<100;j++)
    add[j]='a';
    fd=open(filename,O_RDWR);
    if(fd==-1)
    {
        printf("failed to open file");
        return 0;
    }
    struct stat st;
    if(stat(filename,&st)==-1)
    {
        printf("stat error");
        return 0;
    }
    int len=(int)st.st_size;//文件大小
    char buf[10000];
    read(fd,buf,len);
    int nowLen=len+1+adLength;
    buf[3]=com;
    int sInsert=12;
    int inN=adLength+1;
    int i;
    for(i=len-1;i>=sInsert;i--)
    {
        buf[i+inN]=buf[i];
    }
    for(i=0;i<adLength;i++)
    {
        buf[sInsert+i]=add[i];
    }
    buf[sInsert+adLength]=zero;

    lseek(fd,0,SEEK_SET);

    write(fd,buf,nowLen);
    close(fd);

}
