#include<bits/stdc++.h>
using namespace std;
#define MAXL 5
struct Item{
	string value;
	string type;
	bool compare(Item I){
		if(type=="String"||I.type=="String"){
			return value==I.value;
		}else{
			return stoi(value)>stoi(I.value);
		}
	}
	void input(string type){
		cin>>value;
		this->type = type;
	}
	void print(){
		cout<<value<<" "<<type<<endl;
	}
	Item(string value,string type){
		this->value=value;
		this->type=type;
	}
	Item(){value="",type="";};
};
vector<vector<Item> > state;
vector<Item> input(MAXL);
vector<Item> attribute_row;
map<string, vector<Item> > mape;
Item yes("Play","String");
Item no("Don't_Play","String");
struct Node{
	Item attribute;
	Item value;
	vector<Node*> childs;
	Node(){}
}*root;
bool compare(Node* a,Node *b){
	return a->value.compare(b->value);
}
void Input(){
	attribute_row.push_back(Item("OUTLOOK","String"));
	attribute_row.push_back(Item("TEMPERATURE","Int"));
	attribute_row.push_back(Item("HUMIDITY","Int"));
	attribute_row.push_back(Item("WINDY","String"));
	attribute_row.push_back(Item("PLAY","String"));
	string s;
	while(cin>>s){
		input[0]=Item(s,attribute_row[0].type);
		for(int i=1;i<MAXL;i++){
			cin>>s;
			input[i]=Item(s,attribute_row[i].type);
		}
		state.push_back(input);
	}
}
void ToMap(){
	bool exited = false;
	vector<Item> values;
	for(int i = 0; i < MAXL-1; i++){
		for (int j = 0; j < state.size(); j++){
			for (int k = 0; k < values.size(); k++){
				//cout<<values[k].value<<" "<<state[j][i].value<<endl;
				if(values[k].value==state[j][i].value) exited = true;
			}
			if(!exited){
				values.push_back(state[j][i]);
			}
			exited = false;
		}
		//cout<<attribute_row[i].value<<" "<<values.size()<<endl;
		mape[attribute_row[i].value] = values;
		values.clear();
	}
}
bool AllSame(vector<vector<Item> > remain_state, Item label){
	for(int i=0;i<remain_state.size();i++){
		//cout<<remain_state[i][MAXL-1].value<<" "<<label.value<<endl; 
		if(!(remain_state[i][MAXL-1].value==label.value)){
			return false;
		}
	}
	return true;
}
Item MostLabel(vector<vector<Item> > remain_state){
	int y=0,n=0;
	for(int i=0;i<remain_state.size();i++){
		if(remain_state[i][MAXL-1].compare(yes)){
			y++;
		}else{
			n++;
		}
	}
	if(y>=n)return yes;
	else return no;
}
double ComputeEntropy(vector<vector<Item> > remain_state, Item attribute, Item value, bool isParent){
	vector<int> cnt(2,0);
	//cout<<attribute.value<<" "<<value.value;
	for(int j=0;j<MAXL-1;j++){
		if(attribute_row[j].compare(attribute)){
			for(int i=0;i<remain_state.size();i++){
				if(isParent||(!isParent&&remain_state[i][j].compare(value))){
					if(remain_state[i][MAXL-1].compare(yes))
						cnt[0]++;
					else
						cnt[1]++;
				}
			}
		}
	}
	if(cnt[0]==0||cnt[1]==0){
		//cout<<" "<<0<<endl;
		return 0;
	}
	double sum=cnt[0]+cnt[1];
	double entropy = -cnt[0]/sum*log(cnt[0]/sum)/log(2.0)-cnt[1]/sum*log(cnt[1]/sum)/log(2.0);
	//cout<<" "<<entropy<<endl;
	return entropy;
}
double ComputeGain(vector<vector<Item> > remain_state, Item attribute){
	double parent_entropy = ComputeEntropy(remain_state, attribute, Item(), true);
	double children_entropy = 0;
	
	vector<Item> values = mape[attribute.value];
	vector<double> ratio;
	vector<int> count_values;
	int tmp;
	int total=0;
	for(int m=0;m<values.size();m++){
		tmp=0;
		for(int k=0;k<MAXL-1;k++){
			if(attribute_row[k].compare(attribute)){
				for(int j=0;j<remain_state.size();j++){
					if(remain_state[j][k].compare(values[m])){
						tmp++;
						total++;
					}
				}
			}
		}
		count_values.push_back(tmp);
	} 
	for(int j=0;j<values.size();j++){
		ratio.push_back((double)count_values[j]/(double)total);
	}
	double temp_entropy;
	for(int j=0;j<values.size();j++){
		temp_entropy = ComputeEntropy(remain_state, attribute, values[j], false);
		//cout<<"entropy: "<<temp_entropy<<" ratio: "<<ratio[j]<<endl; 
		children_entropy += ratio[j]*temp_entropy;
	}
	//cout<<"parent: "<<parent_entropy<<" child: "<<children_entropy<<endl; 
	return parent_entropy-children_entropy;
}
int CountLabel(vector<vector<Item> > remain_state, Item label){
	int y=0;
	for(int i=0;i<remain_state.size();i++){
		//cout<<remain_state[i][MAXL-1].value<<" "<<label.value<<endl;
		if(remain_state[i][MAXL-1].value==label.value){
			y++;
		}
	}
	return y;
}
Node* DFS(Node *p, vector<vector<Item> > remain_state, vector<Item> remain_attribute){
	if(p==NULL){
		p=new Node();
	}
	if(AllSame(remain_state,yes)){
		p->attribute=yes;
		return p;
	}
	if(AllSame(remain_state,no)){
		p->attribute=no;
		return p;
	}
	if(remain_attribute.size()==0){
		p->attribute=MostLabel(remain_state);
		return p; 
	}
	double max_gain=-1, temp_gain;
	Item max_attr;
	for(auto it:remain_attribute){
		temp_gain = ComputeGain(remain_state, it);
		//cout<<"TempGain: "<<temp_gain<<endl;
		if(temp_gain>max_gain){
			max_gain=temp_gain;
			max_attr=it;
		}
	}
	vector<Item> new_attribute;
	vector<vector<Item> >new_state;
	for(auto it:remain_attribute){
		if(!it.compare(max_attr))
			new_attribute.push_back(it);
	}
	
	p->attribute=max_attr;
	vector<Item> values = mape[max_attr.value];
	int attrNum=0;
	for(int i=0;i<MAXL;i++){
		//cout<<max_attr.value<<" "<<attribute_row[i].value<<" "<<i<<endl;
		if(attribute_row[i].compare(max_attr)){
			//cout<<"HERE "<<max_attr.value<<" "<<attribute_row[i].value<<endl;
			attrNum=i;
			break;
		}
	}
	for(auto it:values){
		for(int i=0;i<remain_state.size();i++){
			if(remain_state[i][attrNum].compare(it)){
				new_state.push_back(remain_state[i]);
			}
		}
		Node* new_node = new Node();
		new_node->value = it;
		if(new_state.size()==0){
			new_node->attribute=MostLabel(remain_state);
		}else
			DFS(new_node,new_state,new_attribute);
		p->childs.push_back(new_node);
		new_state.clear();
	}
}
bool TreeISO(Node* a,Node *b){
	if(a==NULL&&b==NULL)return true;
	if(a==NULL||b==NULL)return false;
	if(a->attribute.value!=b->attribute.value)return false;
	if(a->childs.size()!=b->childs.size())return false;
	sort(a->childs.begin(),a->childs.end(),compare);
	sort(b->childs.begin(),b->childs.end(),compare);
	bool ok=true;
	for(int i=0;i<a->childs.size();i++){
		ok&=TreeISO(a->childs[i],b->childs[i]);
	}
	return ok;
} 
void PrintTree(Node *p, int depth){
	for (int i = 0; i < depth; i++) cout << '\t';
	if(p->value.value!=""){
		cout<<p->value.value<<endl;
		for (int i = 0; i < depth+1; i++) cout << '\t';
	}
	cout<<p->attribute.value<<endl;
	
	sort(p->childs.begin(),p->childs.end(),compare);
	for(int i=0;i<p->childs.size()-1;i++){
		//cout<<i<<" "<<p->childs.size()<<endl;
		if(i+1>p->childs.size())break;
		if(TreeISO(p->childs[i],p->childs[i+1])){
			continue;
		}
		PrintTree(p->childs[i], depth + 1);
	}
	if(p->childs.size()>=1)
		PrintTree(p->childs[p->childs.size()-1], depth + 1);
}

int main(){
	freopen("input.txt","r",stdin);
	Input();
	vector<Item> remain_attribute;
	
	remain_attribute.push_back(Item("OUTLOOK","String"));
	remain_attribute.push_back(Item("TEMPERATURE","String"));
	remain_attribute.push_back(Item("HUMIDITY","String"));
	remain_attribute.push_back(Item("WINDY","String"));
	vector<vector<Item> > remain_state;
	for(int i=0;i<state.size();i++){
		remain_state.push_back(state[i]);
	}
	ToMap();
	root = DFS(root,remain_state,remain_attribute);
	//cout<<root->childs.size()<<endl;
	sort(root->childs.begin(),root->childs.end(),compare);
	PrintTree(root,0); 
} 
