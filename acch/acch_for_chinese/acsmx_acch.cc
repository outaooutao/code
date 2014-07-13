#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "acsm.h"

#define CDepth 2
#define WSIZE 0x8000
#define LIT_BUFSIZE 0x8000

typedef unsigned char uch;
typedef unsigned short ush;

char window[WSIZE];
int st[WSIZE];//012  Match ,Check,Uncheck
uch l_buf[LIT_BUFSIZE];
ush d_buf[LIT_BUFSIZE];
int last_lit;

/* Malloc the AC Memory
*/ 
static void *AC_MALLOC (int n) 
{
	void *p;
	p = malloc (n);

	return p;
}

/*
*Free the AC Memory
*/ 
static void AC_FREE (void *p) 
{
	if (p)
		free (p);
}


/*
*    Simple QUEUE NODE
*/ 
typedef struct _qnode
{
	int state;
	struct _qnode *next;
}QNODE;

/*
*    Simple QUEUE Structure
*/ 
typedef struct _queue
{
	QNODE * head, *tail;
	int count;
}QUEUE;

/*
*Init the Queue
*/ 
static void queue_init (QUEUE * s) 
{
	s->head = s->tail = 0;
	s->count = 0;
}


/*
*  Add Tail Item to queue
*/ 
static void queue_add (QUEUE * s, int state) 
{
	QNODE * q;
	/*Queue is empty*/
	if (!s->head)
	{
		q = s->tail = s->head = (QNODE *) AC_MALLOC (sizeof (QNODE));
		/*if malloc failed,exit the problom*/
		q->state = state;
		q->next = 0; /*Set the New Node's Next Null*/
	}
	else
	{
		q = (QNODE *) AC_MALLOC (sizeof (QNODE));
		q->state = state;
		q->next = 0;
		/*Add the new Node into the queue*/
		s->tail->next = q;
		/*set the new node is the Queue's Tail*/
		s->tail = q;
	}
	s->count++;
}


/*
*  Remove Head Item from queue
*/ 
static int queue_remove (QUEUE * s) 
{
	int state = 0;
	QNODE * q;
	/*Remove A QueueNode From the head of the Queue*/
	if (s->head)
	{
		q = s->head;
		state = q->state;
		s->head = s->head->next;
		s->count--;

		/*If Queue is Empty,After Remove A QueueNode*/
		if (!s->head)
		{
			s->tail = 0;
			s->count = 0;
		}
		/*Free the QueNode Memory*/
		AC_FREE (q);
	}
	return state;
}


/*
*Return The count of the Node in the Queue
*/ 
static int queue_count (QUEUE * s) 
{
	return s->count;
}


/*
*Free the Queue Memory
*/ 
static void queue_free (QUEUE * s) 
{
	while (queue_count (s))
	{
		queue_remove (s);
	}
}


/*
*  Add a pattern to the list of patterns terminated at this state.
*  Insert at front of list.
*/ 
static void AddMatchListEntry (ACSM_STRUCT * acsm, int state, ACSM_PATTERN * px) 
{
	ACSM_PATTERN * p;
	p = (ACSM_PATTERN *) AC_MALLOC (sizeof (ACSM_PATTERN));
	memcpy (p, px, sizeof (ACSM_PATTERN));

	/*Add the new pattern to the pattern  list*/
	p->next = acsm->acsmStateTable[state].MatchList;
	acsm->acsmStateTable[state].MatchList = p;
}

/* 
* Add Pattern States
*/ 
static void AddPatternStates (ACSM_STRUCT * acsm, ACSM_PATTERN * p) 
{
	unsigned char *pattern;
	int state=0, next, n;
	n = p->n; /*The number of alpha in the pattern string*/
	pattern = p->patrn;
    
	/* 
	*  Match up pattern with existing states
	*/ 
	for (; n > 0; pattern++, n--)
	{
		next = acsm->acsmStateTable[state].NextState[*pattern];
		if (next == ACSM_FAIL_STATE)
			break;
		state = next;
	}

	/*
	*   Add new states for the rest of the pattern bytes, 1 state per byte
	*/ 
	int i;
	for (; n > 0; pattern++, n--)
	{
		acsm->acsmNumStates++;
		ACSM_STATETABLE state_p;
		state_p.MatchList=0;
        acsm->acsmStateTable.push_back(state_p);
	    for (i = 0; i < ALPHABET_SIZE; i++)
	    {
		    acsm->acsmStateTable[acsm->acsmNumStates].NextState[i] = ACSM_FAIL_STATE;
	    }
		acsm->acsmStateTable[state].NextState[*pattern] = acsm->acsmNumStates;
		
		acsm->dept.push_back(acsm->dept[state]+1);
		
		state = acsm->acsmNumStates;
	}
	/*Here,An accept state,just add into the MatchListof the state*/
	AddMatchListEntry (acsm, state, p);
}


/*
*   Build Non-Deterministic Finite Automata
*/ 
static void Build_DFA (ACSM_STRUCT * acsm) 
{
	int r, s;
	int i;
	QUEUE q, *queue = &q;
	ACSM_PATTERN * mlist=0;
	ACSM_PATTERN * px=0;

	/* Init a Queue */ 
	queue_init (queue);

	/* Add the state 0 transitions 1st */
	/*1st depth Node's FailState is 0, fail(x)=0 */
	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		s = acsm->acsmStateTable[0].NextState[i];
		if (s)
		{
			queue_add (queue, s);
			acsm->acsmStateTable[s].FailState = 0;
		}
	}

	/* Build the fail state transitions for each valid state */ 
	while (queue_count (queue) > 0)
	{
		r = queue_remove (queue);

		/* Find Final States for any Failure */ 
		for (i = 0; i < ALPHABET_SIZE; i++)
		{
			int fs, next;
			/*** Note NextState[i] is a const variable in this block ***/
			if ((s = acsm->acsmStateTable[r].NextState[i]) != ACSM_FAIL_STATE)
			{
				queue_add (queue, s);
				fs = acsm->acsmStateTable[r].FailState;

				/* 
				*  Locate the next valid state for 'i' starting at s 
				*/ 
				/**** Note the  variable "next" ****/
				/*** Note "NextState[i]" is a const variable in this block ***/
				while ((next=acsm->acsmStateTable[fs].NextState[i]) ==
					ACSM_FAIL_STATE)
				{
					fs = acsm->acsmStateTable[fs].FailState;
				}

				/*
				*  Update 's' state failure state to point to the next valid state
				*/ 
				acsm->acsmStateTable[s].FailState = next;

                                
                                ACSM_PATTERN* pat = acsm->acsmStateTable[next].MatchList;
                                for (; pat != NULL; pat = pat->next)
                                {
                                    AddMatchListEntry(acsm, s, pat);    
                                }
			}
			else
			{
				acsm->acsmStateTable[r].NextState[i] =
					acsm->acsmStateTable[acsm->acsmStateTable[r].FailState].NextState[i];
			}
		}
	}

	/* Clean up the queue */ 
	queue_free (queue);
}


/*
* Init the acsm DataStruct
*/ 
ACSM_STRUCT * acsmNew () 
{
	ACSM_STRUCT * p;
	p = (ACSM_STRUCT *) AC_MALLOC (sizeof (ACSM_STRUCT));
	if (p)
		memset (p, 0, sizeof (ACSM_STRUCT));
	return p;
}


/*
*   Add a pattern to the list of patterns for this state machine
*/ 
int acsmAddPattern (ACSM_STRUCT * p, unsigned char *pat, int n) 
{
	ACSM_PATTERN * plist;
	plist = (ACSM_PATTERN *) AC_MALLOC (sizeof (ACSM_PATTERN));
	plist->patrn = (unsigned char *) AC_MALLOC (n+1);
	memset(plist->patrn+n,0,1);
	memcpy (plist->patrn, pat, n);
	plist->n = n;
	//plist->nmatch=0;
	/*Add the pattern into the pattern list*/
	plist->next = p->acsmPatterns;
	p->acsmPatterns = plist;

	return 0;
}

/*
*   Compile State Machine
*/ 
int acsmCompile (ACSM_STRUCT * acsm) 
{
	
	acsm->dept.push_back(0);
	int i, k;
	ACSM_PATTERN * plist;

	/* Count number of states */ 
	acsm->acsmMaxStates = 1; /*State 0*/
	for (plist = acsm->acsmPatterns; plist != NULL; plist = plist->next)
	{
		acsm->acsmMaxStates += plist->n;
	}
/*
	acsm->acsmStateTable = (ACSM_STATETABLE *) AC_MALLOC (sizeof (ACSM_STATETABLE) * acsm->acsmMaxStates);
	memset (acsm->acsmStateTable, 0,sizeof (ACSM_STATETABLE) * acsm->acsmMaxStates);
*/
	/* Initialize state zero as a branch */ 
	acsm->acsmNumStates = 0;
	
	ACSM_STATETABLE state_p;
	state_p.MatchList=0;
    acsm->acsmStateTable.push_back(state_p);
	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		acsm->acsmStateTable[0].NextState[i] = ACSM_FAIL_STATE;
	}
	
	/* Initialize all States NextStates to FAILED */ 
	/*
	for (k = 0; k < acsm->acsmMaxStates; k++)
	{
		for (i = 0; i < ALPHABET_SIZE; i++)
		{
			acsm->acsmStateTable[k].NextState[i] = ACSM_FAIL_STATE;
		}
	}
    */
	/* This is very import */
	/* Add each Pattern to the State Table */ 
	for (plist = acsm->acsmPatterns; plist != NULL; plist = plist->next)
	{
		AddPatternStates (acsm, plist);
	}

	/* Set all failed state transitions which from state 0 to return to the 0'th state */ 
	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		if (acsm->acsmStateTable[0].NextState[i] == ACSM_FAIL_STATE)
		{
			acsm->acsmStateTable[0].NextState[i] = 0;
		}
	}

	/* Build the NFA  */ 
	Build_DFA (acsm);
	{
    vector<ACSM_STATETABLE>(acsm->acsmStateTable).swap(acsm->acsmStateTable);
	}
	return 0;
}




/*
*   Search Text or Binary Data for Pattern matches
*/ 
int acsmSearch (ACSM_STRUCT * acsm, unsigned char *Tx, int n) 
{
	
	
	int state;
	ACSM_PATTERN * mlist;
	unsigned char *Tend;
	
	vector<ACSM_STATETABLE>  &StateTable = (acsm->acsmStateTable);
	

	int nfound = 0; /*Number of the found(matched) patten string*/
	unsigned char *T;
	int index;

	
	T = Tx;
	Tend = T + n;

	
	for (state = 0; T < Tend; T++)
	{
		
		state = StateTable[state].NextState[*T];

		// State is a accept state? 
		if( StateTable[state].MatchList != NULL )
		{
			for( mlist=StateTable[state].MatchList; mlist!=NULL;mlist=mlist->next )
			{
				//Get the index  of the Match Pattern String in  the Text
				index = T - mlist->n + 1 - Tx;

				//mlist->nmatch++;
				nfound++;
				printf("Match KeyWord %s at %d char\n",mlist->patrn,index);
				
				
			}
		}
		
	}

    
	return nfound;
}


int fsm(ACSM_STRUCT * acsm,int state,char e,int* out)//return next state
{
	vector<ACSM_STATETABLE>  &StateTable = (acsm->acsmStateTable);
	ACSM_PATTERN * mlist;
    state = StateTable[state].NextState[(unsigned char)e];
	if( StateTable[state].MatchList != NULL )
		{
			*out=1;
			for( mlist=StateTable[state].MatchList; mlist!=NULL;mlist=mlist->next )
			{
				printf("Match KeyWord %s\n",mlist->patrn);
			}
		}
	
    return state;
}
/*
*   Free all memory
*/ 
void acsmFree (ACSM_STRUCT * acsm) 
{
	
	int i;
	ACSM_PATTERN * mlist, *ilist;
	for (i = 0; i <acsm->acsmNumStates; i++)

	{
		if (acsm->acsmStateTable[i].MatchList != NULL)

		{
			mlist = acsm->acsmStateTable[i].MatchList;
			while (mlist)
			{
				ilist = mlist;
				mlist = mlist->next;
				AC_FREE (ilist);
			}
		}
	}
	
	acsm->acsmStateTable.clear();
	vector<ACSM_STATETABLE>().swap(acsm->acsmStateTable);
}

int scanAC(ACSM_STRUCT * acsm, int state, char b, int* status)
{
	int isMatch=0;
    state=fsm(acsm,state,b,&isMatch);
    if(isMatch)
    *status=0;//Match
    else{
        if(acsm->dept[state]>=CDepth)
        *status=1;//Check
        else *status=2;//Uncheck
    }
    return state;
}

void acch(ACSM_STRUCT * acsm)
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
            state=scanAC(acsm,state,l_buf[i],&status);
            window[w]=l_buf[i];
            st[w++]=status;
        }
        else
        {
            unsigned int d=d_buf[i];
            int len=l_buf[i]+3;
            int j=0;
            while(acsm->dept[state]>j&&j<len)
            {
                state=scanAC(acsm,state,window[w-d],&status);
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
                    state=scanAC(acsm,state,window[w-d],&status);
                    window[w]=window[w-d];
                    st[w++]=status;

                }
                }
                int l;
                for(l=j;l<=k;l++)
                {
                    state=scanAC(acsm,state,window[w-d],&status);
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
	
    ACSM_STRUCT * acsm;
    acsm = acsmNew ();

    char* str="aherbscdesheaherbc";//aherbscdeshe(2,12)c//aherb

	char* word[]={"he","she","eah","rbc"};
	int i;
	
	for(i=0;i<4;i++)
	acsmAddPattern (acsm, (unsigned char*)word[i], strlen(word[i]));
	
	

	acsmCompile(acsm);
	
	
	last_lit=14;
	for(i=0;i<12;i++)
		l_buf[i]=str[i];
	l_buf[12]=2;
	l_buf[13]='c';
	for(i=0;i<14;i++)
		d_buf[i]=0;
	d_buf[12]=12;
	acch(acsm);
	
	
	
	acsmSearch(acsm, (unsigned char*)str, strlen (str));
	 
	acsmFree (acsm);
	
	return 0;
}
