#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "spc.h"
#define zero -1//when hashtable do not find a value it returns null or 0,shift m-B+1
//gcc spc.c 'pkg-config --cflags --libs glib-2.0' -o  spc -w
typedef struct linknode
{
	int data;
	struct linknode* next;
}linkNode;

uch mask[8]={128,64,32,16,8,4,2,1};

GSList* g_kw_list=NULL;
GHashTable* shiftTable;
GHashTable* ptrns;
char** pList;//array of paterns

static int getShift(char* s)//���ݣ��ֽڵ��ַ���s�õ���Ӧ��ȫ��ת�ֽ��� 
{
	int shift;
	if(shift=g_hash_table_lookup(shiftTable,s))
	{
		if(shift==zero)
			return 0;
		else return shift;
	}
	else
	return m-B+1;
}

static void setPMatch(int i)//��־�����ֽ��±�pos=i��������m�ֽڵ�ƥ�� 
{
     int q=i/8;
     int r=i%8;
     partialMatch[q]=(partialMatch[q])|(mask[r]);
}
static int getPMatch(int i)//��ѯ�����ֽ��±�pos=i��������m�ֽڵ�ƥ��
{
     int q=i/8;
     int r=i%8;
     if(partialMatch[q]&mask[r])
     return 1;
     else return 0;
     
}

static void prePatterns()//����ptrns hash��ͳ��ģʽ������pattersNum
{
	ptrns=g_hash_table_new(g_str_hash,g_str_equal);
	pattersNum=0;
	m=100;//max m
    GSList* it=NULL;
	
	for(it=g_kw_list;it;it=it->next)
	{
		
		pattersNum++;
		if(m>strlen((char*)it->data))
		{
			m=strlen((char*)it->data);
		}
		
	}
	//build pList
	int i=0;
	pList=(char**)malloc(pattersNum*sizeof(char*));
	for(it=g_kw_list;it;it=it->next)
	{
		char* m_pattern=(char*)malloc(m+1);
		//char m_pattern[m+1];
		m_pattern[m]='\0';
		pList[i]=it->data;
		strncpy(m_pattern,it->data,m);
		linkNode* ln= (linkNode*)malloc(sizeof(linkNode));
		ln->data=i;
		linkNode* ln_it;
		if(ln_it=(linkNode*)g_hash_table_lookup(ptrns,m_pattern))
		{
			ln->next=ln_it->next;
			ln_it->next=ln;
		}
		else
		{
			ln->next=NULL;
			g_hash_table_insert(ptrns,m_pattern,ln);
		}
		
		i++;
	}
	
}

static void build_shiftTable()//����shift hash �� 
{

	shiftTable=g_hash_table_new(g_str_hash,g_str_equal);
	GSList* it=NULL;
	int i;
	char* s;
	int shift;
	for(it=g_kw_list;it;it=it->next)
	{
		s=it->data;
		for(i=0;i<m-B+1;i++)
		{   
		 
			char* b_shift=(char*)malloc(B+1);//can not char b_shift[B+1];
			b_shift[B]=0;
			strncpy(b_shift,s+i,B);
			shift=m-B-i;
			if(shift<getShift(b_shift))
			{
				if(shift==0)
				{
				g_hash_table_insert(shiftTable,b_shift,zero);
			
				
				}
				else
				{
				g_hash_table_insert(shiftTable,b_shift,shift);
			
				}
				
			}
			
		}
		
	}
	
}

static void testcase()//����ģʽ�� 
{
	g_kw_list=g_list_append(g_kw_list,"rainbow");
	g_kw_list=g_list_append(g_kw_list,"shine");
	g_kw_list=g_list_append(g_kw_list,"river");
	g_kw_list=g_list_append(g_kw_list,"version");
	g_kw_list=g_list_append(g_kw_list,"brush");
}



static void pos_shitf(int* point_start, int* point_end, int* l_pos, int *flag, int* pos, int shift_value)//pos��l_pos λ��ʱͬ�� 
{
       int left_shift=shift_value;
       int p=*pos;
       if(*flag==0)//pos��point֮�� 
       {
          if(*pos+shift_value<= *point_end)//δ�Ƴ�point
          {
             //nothing 
          }
          else if(*pos+shift_value > *point_end)//pos�Ƴ�point,Ӧ����flag�ȱ��� 
          {
               *flag=1;
               
               left_shift=shift_value+*pos-*point_end-1;//�Ƶ�point��βλ�õ���һ��λ�� ���ʣ��shitf_value 
               p=p+*point_end+1-*pos;
               *l_pos=*l_pos+1;
               
          }
          
       }
       
       while(left_shift>0)
       {
               if(d_buf[*l_pos]==0)
               {
                  p++;
                  *l_pos=*l_pos+1;
                  left_shift--;
               }
               else//����point 
               {
                   if(l_buf[*l_pos]+3<=left_shift)//ֱ���ƹ�point 
                   {
                        left_shift=left_shift-(l_buf[*l_pos]+3);
                        *l_pos=*l_pos+1;
                        p=p+(l_buf[*l_pos]+3);
                   }
                   else//����point 
                   {
                       *flag=0;
                       *point_start=p;
                       *point_end=p+l_buf[*l_pos]+2;//p+(l_buf[*l_pos]+3)-1
                       p=p+left_shift;
                       left_shift=0;
                   }
                   
               }
       }
       *pos=*pos+shift_value;
}


void scanText(int n)//nΪutrf�����з�ѹ���ַ�����ʵ�ʳ��� 
{
	 prePatterns();
	 build_shiftTable();
     int pos=0;
     int l_pos=0;//l_buf,d_buf�±� 
     int point_end;
     int point_start;
     int start;//point�����õĴ����������ֽ� 
     int end;
     memset(partialMatch,0,n);
     int flag=1;//��־pos�Ƿ�δ��point֮�� 
     
     
     
     while(pos+m<=n)
     {
		
        if(flag&&d_buf[l_pos]!=0)//����point 
        {
			
           point_start=pos;
           point_end=pos+l_buf[l_pos]+2;//pos+(l_buf[l_pos]+3)-1;position of the pointer rightmost byte int the uncompressed data   
           flag=0;
        }
        if(d_buf[l_pos]!=0&&flag==0&&pos>=point_start&&pos+m-1<=point_end)//m byte window internal to pointer
        {
           start=pos-d_buf[l_pos];
           end=point_end-(m-1)-d_buf[l_pos];
           
           //findPartialMatches(start,end)
           int i;
           for(i=start;i<=end;i++)
           {
               if(getPMatch(i))//m byte matched
               {
				  
                   pos=i + (int)d_buf[l_pos];
                   setPMatch(pos);
                   //hash
				   char m_str[m+1];
				   strncpy(m_str,utrf+pos,m);
				   m_str[m]='\0';
                   linkNode* ln_it;
				   for(ln_it=(linkNode*)g_hash_table_lookup(ptrns,m_str);ln_it;ln_it=ln_it->next)
				   {
					  
					   if(strncmp(utrf+pos,pList[ln_it->data],strlen(pList[ln_it->data]))==0)   
						   printf("%s :matched at position %d\n",pList[ln_it->data],pos);
				   }
                   //
               }
           }
           pos=point_end-(m-1)+1;
          
           
        }
        else //m byte window not internal to pointer 
        {
			
			  char b_str[B+1];
			  b_str[B]=0;
			  strncpy(b_str,utrf+pos+m-B,B);
             int shift_value=getShift(b_str);
			
			 if(shift_value==0)
			 {
				
				 char m_str[m+1];
				 m_str[m]='\0';
				 strncpy(m_str,utrf+pos,m);
				 if(g_hash_table_lookup(ptrns,m_str))
				 {
				
					 setPMatch(pos);
					 linkNode* ln_it;
					 for(ln_it=g_hash_table_lookup(ptrns,m_str);ln_it;ln_it=ln_it->next)
					 {
						 if(strncmp(utrf+pos,pList[ln_it->data],strlen(pList[ln_it->data]))==0)   
						   printf("%s :matched at position %d\n",pList[ln_it->data],pos);
					}
				 }
				 	 
				 pos_shitf(&point_start, &point_end, &l_pos, &flag, &pos,1);
			 }
			 else pos_shitf(&point_start, &point_end, &l_pos, &flag, &pos,shift_value);
			
					 
        }
     
     }
     
      
}



int main(int argc, char *argv[])//test
{
	
  testcase();
  char sTemplate[]="This river isn't bright and shiny";
  char Template[]="This river isn't bright and shinyrivert";
  strcpy(utrf,Template);
 
  l_buf=(char*)malloc(sizeof(char)*100);
  int lpos=strlen(sTemplate);
  strcpy(l_buf,sTemplate);
  l_buf[lpos]=2;
  l_buf[lpos+1]='t';
  d_buf=(char*)malloc(sizeof(char)*100);
  int k;
  
  memset(d_buf,0,100);//sizeof(d_buf)==4,,32bit pointer
  d_buf[lpos]=28;

  scanText(strlen(Template));
  
 
  
  
  system("PAUSE");	
  return 0;
}
