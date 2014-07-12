 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/time.h>

#define MAXWORD 30
//gcc ac.c `pkg-config --cflags --libs glib-2.0` -o ac

void free_string(void* data)
{
	free((char*)data);
}


typedef struct linknode{
    char* data;
    struct linknode* next;
}ln;

static ln* set_insert(ln* L,char* e)
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
#define MAXINUM_STATES 300000

int _goto[MAXINUM_STATES][ALPHABET_SIZE];
int fail[MAXINUM_STATES];
ln* _out[MAXINUM_STATES];

static void buildGoto(GList* g_kw_list)
{
    unsigned int used_states=0;
    unsigned int t;
    GList* it=NULL;
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
                 t=used_states;
             }
             else t=_goto[t][*s-'a'];
         }
         _out[t]=set_insert(_out[t],outs);


    }

}


void initial_goto(GList* g_kw_list)
{
    int i,j;
	 for(i=0;i<MAXINUM_STATES;i++)
    _out[i]=NULL;
    for(i=0;i<MAXINUM_STATES;i++)
        for(j=0;j<ALPHABET_SIZE;j++)
         _goto[i][j]=-1;

    buildGoto(g_kw_list);
    for(j=0;j<ALPHABET_SIZE;j++)
        if(_goto[0][j]==-1)
            _goto[0][j]=0;
}

void buildFail()
{
    int queue[300000];
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

/*
void read_kw_file(char *filename)
{
	FILE *file = fopen(filename, "r");
	while(1)
	{
		char *word = (char *)malloc(MAXWORD);
		fgets(word,MAXWORD,file);
        word[strlen(word)-1]='\0';
        int n=strlen(word);
		if(n > 0)
		{
		    g_kw_list = g_list_append(g_kw_list, word);
		    //printf("%s\n",word);
		}

		else
			break;
	}
	fclose(file);
}
*/



static void print_set(ln* L)
{
    ln* p=L;
    while(p)
    {
        printf("%s:matched\n",p->data);
        p=p->next;
    }
}
static void widthfirst()
{
    int i;
    int t[100];
    memset(t,0,100);
    int tp=1;
    t[0]=0;
    int r;
    int k=0;
    while(k<tp)
    {
        for(i=0;i<26;i++)
        {
           if(_goto[t[k]][i]!=-1&&_goto[t[k]][i]!=t[k])
           {
               r=_goto[t[k]][i];
               t[tp++]=r;
               printf("%d-%c->%d\n",t[k],(char)(i+'a'),r);
               printf("fail[%d]:%d\n",r,fail[r]);
           }
        }
        k++;
    }


}

int ac(char* s)//input main string
{
	int match=0;
    int state=0;
    int alph;
    while(*s)
    {
		
        alph=*s-'a';
        if(alph>=0&&alph<26)
		{
        while(_goto[state][alph]==-1)
            state=fail[state];
        state=_goto[state][alph];

        if(_out[state])
        {
            //print_set(_out[state]);
			match++;
        }
        s++;
		}
        else 
		{
			state=0;
			s++;
		}
    }
    return match;
}

int main()
{
	
	struct timeval start, end;

    int interval;

 

    FILE* strf=fopen("/home/billowkiller/testAc/baidu","r");
	
	char str[40000];
	fread(str,40000,1,strf);
	fclose(strf);
	//testac
	GList *g_kw_list = NULL;
	FILE *file = fopen("/home/billowkiller/testAc/keywords", "r");
	while(TRUE)
	{
		char *word = (char *)malloc(MAXWORD);
		int n = fscanf(file, "%s", word);
		if(n > 0)
			g_kw_list = g_list_append(g_kw_list, word);
		else
			break;
	}
	fclose(file);
	
	
	gettimeofday(&start, NULL);
	initial_goto(g_kw_list);
    buildFail();
	gettimeofday(&end, NULL);
	 interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

    printf("ac_build fSM interval = %f\n", interval/1000.0);
	
	gettimeofday(&start, NULL);
	ac(str);
	 gettimeofday(&end, NULL);
	
	 interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

    printf("ac_interval = %f\n", interval/1000.0);
	
	/*
	gettimeofday(&start, NULL);
	GList *iterator = NULL;
	for (iterator = g_kw_list; iterator; iterator = iterator->next)
		 if(strstr(str, (char*)iterator->data))
		 {
			 //printf("data = %s matched\n", (char *)iterator->data);
		 }
	 gettimeofday(&end, NULL);
	interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

    printf("str_interval = %f\n", interval/1000.0);
    */
	g_list_free_full(g_kw_list,free_string);	 
	
	return 0;
}

