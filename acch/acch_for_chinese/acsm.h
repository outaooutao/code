 
#ifndef ACSM_H
#define ACSM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
/*
*   Prototypes
*/
#define ALPHABET_SIZE    256     
#define MAXLEN 256

#define ACSM_FAIL_STATE   -1     
#define MAXWORD 30

using namespace std;
typedef struct _acsm_pattern {      

	struct  _acsm_pattern *next;
	unsigned char         *patrn;
	int      n;
	//int		 nmatch;

} ACSM_PATTERN;


typedef struct  {    

	/* Next state - based on input character */
	int      NextState[ ALPHABET_SIZE ];  

	/* Failure state - used while building NFA & DFA  */
	int      FailState;   

	/* List of patterns that end here, if any */
	ACSM_PATTERN *MatchList;   

}ACSM_STATETABLE; 


/*
* State machine Struct
*/
typedef struct {

	int acsmMaxStates;  
	int acsmNumStates;  

	ACSM_PATTERN    * acsmPatterns;
	//ACSM_STATETABLE * acsmStateTable;
	
	vector<ACSM_STATETABLE> acsmStateTable;
	vector<int> dept;

}ACSM_STRUCT;

/*
*   Prototypes
*/
ACSM_STRUCT * acsmNew ();

int acsmDelPattern( ACSM_STRUCT * p, char * pat, int n);
int acsmAddPattern( ACSM_STRUCT * p, unsigned char * pat, int n);
int acsmCompile ( ACSM_STRUCT * acsm );
//int acsmSearch ( ACSM_STRUCT * acsm,unsigned char * T, int n, int (*Match) (ACSM_PATTERN * pattern,ACSM_PATTERN * mlist, int nline,int index));
int acsmSearch (ACSM_STRUCT * acsm, unsigned char *Tx, int n);
void acsmFree ( ACSM_STRUCT * acsm );
int fsm(ACSM_STRUCT * acsm,int state,char e,int* out);

#endif
 
