#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* urldecode(char *cp)
{
    char *p=(char*)malloc((strlen(cp)+1));
    memcpy(p,cp,strlen(cp)+1);
    char *pr=p;
    register i=0;
    while(*(p+i))
    {
       if ((*p=*(p+i)) == '%')
       {
        *p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');//0xDF按位与是将小写字母转成大写大写
        *p=(*p) * 16;
        *p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');
        i+=2;
       }
       else if (*(p+i)=='+')
       {
        *p=' ';
       }
       p++;

    }
    *p='\0';
    p++;

    return pr;
}
int main()
{
    char p[100];
    scanf("%s",p);
    char *cp=urldecode(p);
    printf("%s\n",cp);

}


