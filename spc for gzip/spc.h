#ifndef SPC_H
#define SPC_H

#define SIZE 0x8000
#define BITSIZE 0x1000
#define LIT_BUFSIZE 0x8000
#define DIST_BUFSIZE LIT_BUFSIZE

#define B 2
typedef unsigned char uch;
typedef unsigned short ush;
#endif

char utrf[SIZE];//��ѹ�����ַ�������
uch partialMatch[BITSIZE];//ָʾ�Ƿ���ǰm �ֽڲ���ƥ�� ,ÿһ���ض�Ӧ��utrf�е�һ���ֽ� 
int m=5;
int pattersNum;

uch* l_buf;//store length-3 0-255
ush* d_buf;
