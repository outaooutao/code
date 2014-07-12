#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/time.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
//-std=c++11
vector<vector<string> > vec;

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

sqlite3 *db = NULL;
int init_sqlite()
{
    char* dbpath=":memory:";
    //char* dbpath="/opt/lampp/htdocs/FBControlUI/rulecontrol/fbac.db";
    int rc;
    //open the database file.If the file is not exist,it will create a file.
    rc = sqlite3_open(dbpath, &db);
    if(rc)
    {
        printf("error\n");
		sqlite3_close(db);
        return 0;
    }
	return 1;
}	

char** query(char *sql, int* nrow, int* ncolumn)
{
    char *zErrMsg;
    char **azResult;
    sqlite3_get_table(db, sql, &azResult, nrow, ncolumn, &zErrMsg);
    //printf("finddb_error:%s\n",zErrMsg);
    return azResult;
}




int exeNonQuery(char* sql)//return 0 if Ok
{
    char *zErrMsg;
    int r=sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	//printf("insert_rule_error:%s\n",zErrMsg);
    return r;
}
int create_table()
{
	string sql="CREATE TABLE 'rule_list' (\
    'res_type_num' TEXT,\
    'sub_group' TEXT,\
    'res_group' TEXT)";
	return exeNonQuery((char*)sql.c_str());
	
}
void insert_rule(const char* sg,const char* type,const char* rg)
{
    //insert into rule_list (res_type_num,sub_group,res_from_group,res_group) values (,'','','')
    int len=91;
   
    len=len+strlen(type)+strlen(sg)+strlen(rg);
    char sql[len];
    memset(sql,0,sizeof(sql));
    strcpy(sql,"insert into rule_list (res_type_num,sub_group,res_group) values (");
    strcat(sql,type);
    strcat(sql,",'");
    strcat(sql,sg);
    strcat(sql,"','");
    strcat(sql,rg);
    strcat(sql,"')");
    exeNonQuery(sql);
}


int find_db(const char* sid,const char* res_type_num,const char* rid)
{
    //select * from  rule_list where (res_type_num= or res_type_num=*) and (sub_group=sid or sub_group='*') and (res_group=rid or res_group='*')
	int len =300;
    len=len+strlen(res_type_num)+strlen(sid)+strlen(rid);
    char sql[len];
    memset(sql,0,sizeof(sql));
    strcpy(sql,"select * from rule_list where (res_type_num=");
    strcat(sql,res_type_num);
    strcat(sql," or res_type_num='*') and (sub_group ='");
    strcat(sql,sid);
    strcat(sql,"' or sub_group='*') and (res_group ='");
    strcat(sql,rid);
    strcat(sql,"' or res_group='*')");
    int row=0,column;
    //printf("%s len:%d\n",sql,strlen(sql));
    query(sql,&row,&column);
	//cout<<"row"<<row<<endl;
    return row;
}
void del_db(const char* sid,const char* res_type_num,const char* rid)
{
	//"delete  from rule_list where res_type_num='' and sub_group='' and res_group=''";
	int len=78+strlen(sid)+strlen(res_type_num)+strlen(rid);
	string sql="delete from rule_list where res_type_num='"+(string)res_type_num+"' and sub_group='"+(string)sid+"' and res_group='"+(string)rid+"'";
	exeNonQuery((char*)sql.c_str());
	
}

int main() {
	struct timeval start, end;
	int interval;
	gettimeofday(&start, NULL);
	readFile(vec, "5000rule.txt",5000);
	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"readfile_time"<<interval/1000.0<<endl;
	if(init_sqlite())
		cout<<"open"<<endl;
	create_table();
	
	
	gettimeofday(&start, NULL);
	for(auto &rule: vec) {
		insert_rule(rule[0].c_str(),rule[1].c_str(),rule[2].c_str());
	}
	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"insert_time"<<interval/1000.0<<endl;
	gettimeofday(&start, NULL);
	for(auto &rule: vec) {
		find_db(rule[0].c_str(),rule[1].c_str(),rule[2].c_str());
	}
	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"find_time"<<interval/1000.0<<endl;
	
	vector<string> rul=vec[5];
	gettimeofday(&start, NULL);
	del_db(rul[0].c_str(),rul[1].c_str(),rul[2].c_str());
	/*
	for(auto &rule: vec) {
		del_db(rule[0].c_str(),rule[1].c_str(),rule[2].c_str());
		insert_rule(rule[0].c_str(),rule[1].c_str(),rule[2].c_str());
	}
	*/
	gettimeofday(&end, NULL);
	interval=1000000*(end.tv_sec - start.tv_sec)+(end.tv_usec- start.tv_usec);
	cout<<"del_time"<<interval/1000.0<<endl;
	
	
	sqlite3_close(db);
	return 0;
}