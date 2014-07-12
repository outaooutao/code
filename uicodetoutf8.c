#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void convert(char** pstrutf,char** pstruni)//0x652f->支
{
    unsigned short n=0;
    char* struni=*pstruni;
    char* strutf=*pstrutf;
    int i;
    for(i=0;i<4;i++)
    {
        n=n*16+((*struni)>'A'?((*struni & 0xDF) - 'A')+10:(*struni-'0'));
        struni++;
    }
    //printf("%d\n",n);
    if(n<0x80)
    {
        (*strutf++)=(char)(n&0x7F);

    }
    else if(n<0x800)
    {
        (*strutf++)=0xC0|(n>>6);
        (*strutf++)=0x80|(n&0x3F);
    }
    else
    {
        (*strutf++)=0xE0|(n>>12);
        (*strutf++)=(0x80|((n>>6)&0x3F));
        (*strutf++)=0x80|(n&0x3F);
    }
    *pstrutf=strutf;//pointer change!
    *pstruni=struni;
}

int main()
{
    /*
    char* struni="652F";
    char utf[10];
    memset(utf,0,sizeof(utf));
    char* strutf=utf;
    convert(&strutf,&struni);
    printf("%s,len:%d\n",utf,strlen(utf));
    */
    int fd=-1;
    ssize_t size=-1;
    char filename[50]="test";
    fd=open(filename,O_RDONLY);
    if(fd==-1)
    {
        printf("failed to open file %s,fd:%d\n",filename,fd);
        return -1;
    }
    struct stat st;
    if(stat(filename,&st)==-1)
    {
        printf("error\n");
        return -1;
    }
    unsigned int m_size=st.st_size+1;
    char buf[m_size];
    memset(buf,0,sizeof(buf));
    size=read(fd,buf,m_size);
    if(size==-1)
    {
        close(fd);
        printf("read file error occurs\n");

        return -1;

    }
    close(fd);
    //printf("%s\n",buf);
    char utf[m_size];
    memset(utf,0,sizeof(utf));
    char* strutf=utf;
    char* struni=buf;

    while(*struni) //while(m_size) no process'\0'
    {
        if(*struni=='\\')
        {
            if(*(struni+1)=='"'||*(struni+1)=='/')
            {
                *strutf++=*(++struni);
                struni++;
                //m_size-=2;//
            }
            else if(*(struni+1)=='u')
            {
                struni+=2;
                convert(&strutf,&struni);
                //m_size-=6;//
            }
            else //'/'后不为u"..
            {
                *strutf++=*struni++;
            }
        }
        else
        {
            *strutf++=*struni++;
            //m_size--;//
        }
    }
    printf("%s",utf);
    strcat(filename,"_parsed");
    fd=-1;
    fd=open(filename,O_RDWR|O_CREAT);
    if(fd==-1)
    {
        printf("failed to open file %s,fd:%d\n",filename,fd);
        return -1;
    }
    write(fd,utf,strlen(utf));
    close(fd);

    return 0;


}
