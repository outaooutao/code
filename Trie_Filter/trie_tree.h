#include <string>
#include <vector>
#include <cstdio>
#include <assert.h>

using namespace std;
class TrieTree {

	typedef vector<string> rule_type;

	public:
	class Node {
		public:
			Node(char ch=0):
				c(ch), 
				isvalid(false),
				isend(false)
			{ 
				for(int i=0; i<CHARSIZE; i++) 
					children[i] = NULL;
			}
			Node* getchild(char c) {
				if(c=='*') 
					return children[CHARSIZE-1];
				else 
					return children[c-'0'];
			}

			Node* getchild(int index) {
				assert(index < CHARSIZE);
				return children[index];
			}

			Node* haschild() {
				for(int i=0; i<CHARSIZE; i++) {
					if(children[i])	
						return children[i];
				}
				return NULL;
			}


			void delchild(char c) {
				if(c=='*') 
				{
					delete(children[CHARSIZE-1]);
					children[CHARSIZE-1] = NULL;	
				}
				else 
				{
					delete(children[c-'0']);
					children['c'-0] = NULL;	
				}
			}
			
			void delchild(int index) {
				delete(children[index]);
				children[index] = NULL;	
			}

			//set right c to child Node
			//p.s. origin is 0;
			void addchild(char c) {
				if(c=='*') 
					children[CHARSIZE-1]=new Node(c);
				else 
					children[c-'0']=new Node(c);
			}

		public:
			static const int CHARSIZE= 11; //0-9*
			bool isvalid;
			bool isend;
			char c;

		private:
			Node* children[CHARSIZE];
	};	

	private:
		string vec2str(const rule_type &vec);
		void minimizeRule(string &s);
		void minimizeRule(rule_type &s);
		bool deleteHelper(Node* node, const string &key, int level);
		bool hasChildren(Node *node);
		void killDescendants(Node* node, bool self=false);

	public:
		TrieTree();
		/*
		 * Maybe we need to store the rule into other place, i.e. database
		 * therefor we use pass by value instead of pass by reference.
		 */
		bool insertRule(string s, bool check_mini=true);
		bool insertRule(rule_type rule);		
		bool match_tuple(const rule_type &rule);
		bool match_rule(const string &rule);
		/*
		 * This function is tricky.
		 * every part of the rule can be tested here.
		 * 
		 * If you use this function to test the final rule,
		 * which means the last key in a rule, you should check
		 * the isend flag of the return node by checkFinalNode.
		 */
		Node *match(Node *t, const string &str);
		bool checkFinalNode(Node *t);

		//if real_delete is true, then return means nothing.
		bool deleteRule(string &key, bool real_delete=false);
		bool deleteRule(rule_type &rule, bool real_delete=false);

		void destroyTree();
		
		Node* root;
	private:
};
