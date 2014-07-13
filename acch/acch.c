#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#define CDepth 2
#define WSIZE 0x8000
#define LIT_BUFSIZE 0x8000
//compile:gcc acch.c `pkg-config --cflags --libs glib-2.0` -o acch -w

typedef unsigned char uch;
typedef unsigned short ush;

char window[WSIZE];
int st[WSIZE];//012  Match ,Check,Uncheck
uch l_buf[LIT_BUFSIZE];
ush d_buf[LIT_BUFSIZE];
int last_lit;


GSList* g_kw_list = NULL;
typedef struct linknode{
    char* data;
    struct linknode* next;
}ln;


ln* set_insert(ln* L,char* e)
{
    if(!L)
    {
        L=(ln*)malloc(sizeof(ln));
        L->next=NULL;
        L->data=e;
        return L;
    }

    ln* p=L;
    ln*q=NULL;
    while(p)
    {
        if(strcmp(p->data,e)==0)
        return L;
        else
        {
            q=p;
            p=p->next;
        }
    };
    if(!p)
    {
        p=(ln*)malloc(sizeof(ln));
        p->next=NULL;
        p->data=e;
        q->next=p;
    }
    return L;
}


#define ALPHABET_SIZE 26
#define MAXINUM_STATES 100

int _goto[MAXINUM_STATES][ALPHABET_SIZE];
int fail[MAXINUM_STATES];
ln* _out[MAXINUM_STATES];
int dept[MAXINUM_STATES];

void buildGoto()
{
    dept[0]=0;

    unsigned int used_states=0;
    unsigned int t;
    GSList* it=NULL;
    char* s;
    int l;
    int j;
    for(it = g_kw_list; it; it = it->next)
    {
         s=(char*)it->data;
         char* outs=s;
         t=0;
         l=strlen(s);
         for(j=0;j<l;j++,s++)
         {
             if(_goto[t][*s-'a']==-1)
             {
                 _goto[t][*s-'a']=++used_states;
                 dept[used_states]=dept[t]+1;
                 t=used_states;
             }
             else t=_goto[t][*s-'a'];
         }
         _out[t]=set_insert(_out[t],outs);


    }

}

void initial_goto()
{
    int i,j;
    for(i=0;i<MAXINUM_STATES;i++)
        for(j=0;j<ALPHABET_SIZE;j++)
         _goto[i][j]=-1;

    buildGoto();
    for(j=0;j<ALPHABET_SIZE;j++)
        if(_goto[0][j]==-1)
            _goto[0][j]=0;
}

void buildFail()
{
    int queue[100];
    int rear=0;
    int front=0;
    int r;//last state r
    int j;
    int s;//state s
    int state;
    for(j=0;j<ALPHABET_SIZE;j++)
        if((s=_goto[0][j])>0)
        {
            queue[rear++]=s;
            fail[s]=0;
        }
    while(front<rear)
    {
        r=queue[front++];
        for(j=0;j<ALPHABET_SIZE;j++)
        if((s=_goto[r][j])!=-1)
        {
            queue[rear++]=s;
            state=fail[r];
            while(_goto[state][j]==-1)
            state=fail[state];

            fail[s]=_goto[state][j];


            ln* node=_out[fail[s]];
            while(node)
            {
                _out[s]=set_insert(_out[s],node->data);
                node=node->next;
            }

        }
    }


}

void testcase()
{
    g_kw_list = g_list_append(g_kw_list, "he");
    g_kw_list = g_list_append(g_kw_list, "she");
    g_kw_list = g_list_append(g_kw_list, "eah");
    g_kw_list = g_list_append(g_kw_list, "rbc");
}

void print_set(ln* L)
{
    ln* p=L;
    while(p)
    {
        printf("%s:matched\n",p->data);
        p=p->next;
    }
}
/*
void ac(char* s)//input main string
{
    int state=0;
    int alph;
    while(*s)
    {
        alph=*s-'a';
        while(_goto[state][alph]==-1)
            state=fail[state];
        state=_goto[state][alph];

        if(_out[state])
        {
            print_set(_out[state]);
        }

        s++;

    }
}
*/
int fsm(int state,char e)//return next state
{
    int alph=e-'a';
        while(_goto[state][alph]==-1)
            state=fail[state];
        state=_goto[state][alph];

    if(_out[state])
        {
            print_set(_out[state]);
        }
    return state;
}
void ac(char* s)//input main string
{
    int state=0;
    while(*s)
    {
        state=fsm(state,*s);



        s++;

    }
}

void check_dept()
{
    int i;
    for(i=0;i<10;i++)
    printf("state%d:%d\n",i,dept[i]);
}


int scanAC(int state, char b, int* status)
{
    state=fsm(state,b);
    if(_out[state]!=NULL)
    *status=0;//Match
    else{
        if(dept[state]>=CDepth)
        *status=1;//Check
        else *status=2;//Uncheck
    }
    return state;
}
void acch()
{
    int n=last_lit;
    unsigned int w=0;
    int i;
    int status;
    int state=0;
    for(i=0;i<n;i++)
    {
        if(d_buf[i]==0)
        {
            state=scanAC(state,l_buf[i],&status);
            window[w]=l_buf[i];
            st[w++]=status;
        }
        else
        {
            unsigned int d=d_buf[i];
            int len=l_buf[i]+3;
            int j=0;
            while(dept[state]>j&&j<len)
            {
                state=scanAC(state,window[w-d],&status);
                window[w]=window[w-d];
                st[w++]=status;
                j++;
            }
            int k=j-1;
            int fk;//find k
            int fp;//find p
            int p;
            while(k<len-1)
            {
                k=len-1;
                for(fk=j;fk<len;fk++)
                {
                    if(st[w-d+fk]==0)//match
					{
					k=fk;
                    break;
					}
                }
                
                p=j;
                for(fp=k;fp>=j;fp--)
                {
                    if(st[w-d+fp]==2)
					{
					p=fp;
                    break;
					}
						
                }
                
                if(j<(p-CDepth+1))
                {
                while(j<(p-CDepth+1))
                {
                    window[w]=window[w-d];
					 w++;
                    j++;
                }
                state=0;
                for(j=(p-CDepth+1);j<p;j++)
                {
                    state=scanAC(state,window[w-d],&status);
                    window[w]=window[w-d];
                    st[w++]=status;

                }
                }
                int l;
                for(l=j;l<=k;l++)
                {
                    state=scanAC(state,window[w-d],&status);
                    window[w]=window[w-d];
                    st[w++]=status;
                }
                j=k+1;

            }


        }



    }

}


int main()
{
    int i;
    for(i=0;i<MAXINUM_STATES;i++)
    _out[i]=NULL;
    testcase();
    initial_goto();
    buildFail();
	char* str="aherbscdesheaherbc";
    ac("aherbscdesheaherbc");
    //check_dept();
	
	printf("acch:\n");
    last_lit=14;
	for(i=0;i<12;i++)
		l_buf[i]=str[i];
	l_buf[12]=2;
	l_buf[13]='c';
	for(i=0;i<14;i++)
		d_buf[i]=0;
	d_buf[12]=12;
	acch();
	printf("window:%s\n",window);
    return 0;
}


