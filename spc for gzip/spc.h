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

char utrf[SIZE];//非压缩的字符串缓存
uch partialMatch[BITSIZE];//指示是否发生前m 字节部分匹配 ,每一比特对应于utrf中的一个字节 
int m=5;
int pattersNum;

uch* l_buf;//store length-3 0-255
ush* d_buf;
