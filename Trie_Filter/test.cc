/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/17/2014 09:51:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "trie_tree.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/time.h>

using namespace std;

vector<vector<string> > vec;

int test_trie_tree()
{
	TrieTree tt;
/*
	tt.insertRule("1234 * *");
	cout << tt.match_rule("1234 4 1234") << endl;
*/
	struct timeval start, end;
	int interval;
	gettimeofday(&start, NULL);
	for(auto &rule: vec) {
		if(tt.insertRule(rule)==false)
			cout << "insert false " << endl;
	}

	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"rule_insert_time"<<interval/1000.0<<endl;
	
	gettimeofday(&start, NULL);
	for(auto &rule: vec) {
		if(false == tt.match_tuple(rule))
			cout << "match false" << endl;
	}

	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"rule_match_time"<<interval/1000.0<<endl;


	gettimeofday(&start, NULL);
	tt.deleteRule(vec[5], true);
	
    
	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"rule_delete_time"<<interval/1000.0<<endl;

//	tt.insertRule("13");
//	tt.insertRule("12");
//	tt.destroyTree();

	return 0;
}

void readFile(vector<vector<string> > &vec, string s,int num) {
	ifstream ifs(s);
	string sid, type, rid;
	int i=0;
	while(ifs >> sid >> type >> rid) {
		if(i>=num)
			break;
		vector<string> v = {sid, type, rid};
		vec.push_back(v);
		i++;
	}
}

int main() {
	struct timeval start, end;
	int interval;
	gettimeofday(&start, NULL);
	readFile(vec, "5000rule.txt",5000);
	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"readfile_time"<<interval/1000.0<<endl;
	test_trie_tree();
	return 0;
}
