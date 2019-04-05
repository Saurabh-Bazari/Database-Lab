#include<bits/stdc++.h>
using namespace std; 

class Node{
	public:
		Node(int totalKeys);
    	int getNumKeys();
    	void setNumKeys(int val);
	    int getKeyAtIndex(int i);
    	void setKeyAtIndex(int key, int i);
	private:
		int numKeys;
		int *keys;
};

class IndexNode: public Node{
	public:
		IndexNode(int numIndexPointers);
		void *getIndexPointerAt(int i);
		void setIndexPointerAt(void *p,int i);
	private:
		void **indexPointers;
};

class DataNode: public Node{
	public:
		DataNode(int numDataPointers);
		void *getDataPointerAt(int i);
		void setDataPointerAt(void *p, int i);
		DataNode* getDataNodeLeft();
		void setDataNodeLeft(DataNode *p);
		DataNode* getDataNodeRight();
		void setDataNodeRight(DataNode *p);
	private:
		void **dataPointers;
		DataNode *left, *right;
};

int Node::getNumKeys(){
  return numKeys;
}

void Node::setNumKeys(int val){
	numKeys = val;
}

int Node::getKeyAtIndex(int i){
  return keys[i];
}

void Node::setKeyAtIndex(int key, int i){
	keys[i] = key;
}

Node::Node(int totalKeys){
	numKeys = 0;
	keys = new int[totalKeys+1];
}

IndexNode::IndexNode(int numIndexPointers)
	: Node(numIndexPointers)
{
	indexPointers = new void*[numIndexPointers+1];
}

void* IndexNode::getIndexPointerAt(int i){
	return indexPointers[i];
}
void IndexNode::setIndexPointerAt(void* p, int i){
	indexPointers[i] = p;
}

DataNode::DataNode(int numDataPointers)
	: Node(numDataPointers)
{
	dataPointers = new void*[numDataPointers];
	left = NULL;
	right = NULL;
}

void* DataNode::getDataPointerAt(int i){
	return dataPointers[i];
}
void DataNode::setDataPointerAt(void *p, int i){
	dataPointers[i] = p;
}
DataNode* DataNode::getDataNodeLeft(){
	return left;
}
void DataNode::setDataNodeLeft(DataNode *p){
	left = p;
}
DataNode* DataNode::getDataNodeRight(){
	return right;
}
void DataNode::setDataNodeRight(DataNode *p){
	right = p;
}

class BPlusTree{
  public:
    BPlusTree(int numIndexPointers,int numDataPointers);
    IndexNode* toIndexNode(void *p);
    DataNode* toDataNode(void *p);
    void insert(void *root, int key, void* &new_entry, int &val,int depth, void* &data2,int &val2);
    DataNode* search(void *node, int key, int depth);
    void printtree(void *node, int depth);
	IndexNode* splitIndexNode(IndexNode* y,int key,void* new_entry,int &val);
	void insertValAtLeafNode(DataNode *temp,int key);
	void insertValAtIndexNode(IndexNode *temp,int key,void* new_entry);
	DataNode* splitLeafNode(DataNode* y,int key,DataNode* &data2);
	void LevelOrder(void *root, int depth);

    void* root;
    int numIndexPointers;
    int numDataPointers; 
    int depth;
};

BPlusTree::BPlusTree(int numIndexPointers,int numDataPointers){
	root = new DataNode(numDataPointers);
	this->numIndexPointers = numIndexPointers;
	this->numDataPointers = numDataPointers;
	depth = 0;
}

IndexNode* BPlusTree::toIndexNode(void *p){
	return static_cast<IndexNode*>(p);
}

DataNode* BPlusTree::toDataNode(void *p){
	return static_cast<DataNode*>(p);
}

DataNode* BPlusTree::search(void *node, int key, int depth){
	
	if(depth==0){
		DataNode *x = toDataNode(node);	
		for(int i = 0; i < x->getNumKeys(); i++)
			if(x->getKeyAtIndex(i) == key)
				return x;
		return NULL;	
	}
	else{
		IndexNode *x = toIndexNode(node);
		int i = 0;
		while(i < x->getNumKeys() && key >= x->getKeyAtIndex(i))
			i = i + 1;
		return search(x->getIndexPointerAt(i), key, depth-1);
	}
}

void BPlusTree::insertValAtLeafNode(DataNode *temp,int key)
{
	int i=temp->getNumKeys()-1;
	for(i=temp->getNumKeys()-1;i>=0;i--)
	{
		if(temp->getKeyAtIndex(i) > key)
		{
			temp->setKeyAtIndex(temp->getKeyAtIndex(i),i+1);
		}
		else
		{
			break;
		}
	}
	i++;
	temp->setKeyAtIndex(key,i);
	temp->setNumKeys(temp->getNumKeys()+1);
	return;
}

DataNode* BPlusTree::splitLeafNode(DataNode* y,int key, DataNode* &data2){
	DataNode *x = new DataNode(numDataPointers);
	DataNode *z = new DataNode(numDataPointers);
	int i=y->getNumKeys()-1;
	for(i=y->getNumKeys()-1;i>=0;i--)
	{
		if(y->getKeyAtIndex(i) > key)
		{
			y->setKeyAtIndex(y->getKeyAtIndex(i),i+1);
		}
		else{
			break;
		}
	}
	i++;
	y->setKeyAtIndex(key,i);
	int first1 = (numDataPointers+1)/3;
	int second1=(numDataPointers+1)/3;
	int third1=(numDataPointers+1)/3;
	int r=(numDataPointers+1)%3;
	if(r == 1)
	{
		second1++;
	}
	if(r == 2)
	{
		second1++;
		third1++;
	}
	z->setNumKeys(third1);
	for(int j = 0; j < third1; j++)
	{
		z->setKeyAtIndex(y->getKeyAtIndex(j + second1), j);
		z->setDataPointerAt(y->getDataPointerAt(j + second1), j);
	}
	x->setNumKeys(first1);
	for(int j=0;j<first1;j++)
	{
		x->setKeyAtIndex(y->getKeyAtIndex(j + second1+third1), j);
		x->setDataPointerAt(y->getDataPointerAt(j + second1+third1), j);
	}
	y->setNumKeys(second1);
	z->setDataNodeLeft(y);
	z->setDataNodeRight(y->getDataNodeRight());
	y->setDataNodeRight(z);
	x->setDataNodeLeft(z);
	z->setDataNodeRight(z->getDataNodeRight());
	z->setDataNodeRight(x);
	data2=x;
	return z;
}

void BPlusTree::insertValAtIndexNode(IndexNode *temp,int key,void* new_entry)
{
	int i=temp->getNumKeys()-1;
	for(i=temp->getNumKeys()-1;i>=0;i--)
	{
		if(temp->getKeyAtIndex(i) > key)
		{
			temp->setKeyAtIndex(temp->getKeyAtIndex(i),i+1);
			temp->setIndexPointerAt(temp->getIndexPointerAt(i+1),i+2);
		}
		else
		{
			break;
		}
	}
	i++;
	temp->setKeyAtIndex(key,i);
	temp->setIndexPointerAt(new_entry,i+1);
	temp->setNumKeys(temp->getNumKeys()+1);
	return;
}

IndexNode* BPlusTree::splitIndexNode(IndexNode* y,int key,void* new_entry,int &val){
	insertValAtIndexNode(y,key,new_entry);
	IndexNode* x=new IndexNode(numIndexPointers);
	int first1=(numIndexPointers+1)/2-1;
	x->setNumKeys(first1);
	int ct=0;
	int i=0;
	for(i=y->getNumKeys()-first1;i<y->getNumKeys();i++)
	{
		x->setKeyAtIndex(y->getKeyAtIndex(i),ct);
		x->setIndexPointerAt(y->getIndexPointerAt(i),ct);
		ct++;
	}
	val=y->getKeyAtIndex(y->getNumKeys()-first1-1);
	x->setIndexPointerAt(y->getIndexPointerAt(i),ct);
	y->setNumKeys(y->getNumKeys()-first1-1);
	return x;
}

void BPlusTree::insert(void *root, int key, void* &new_entry, int &val,int depth, void* &data2,int &val2){
	if(depth==0)
	{
		DataNode* droot = toDataNode(root);
		if(droot->getNumKeys() == numDataPointers)
		{
            DataNode* tp = NULL;
			DataNode* z = splitLeafNode(droot,key,tp);
			
			val=droot->getKeyAtIndex(droot->getNumKeys()-1) + 1; 
			val2=z->getKeyAtIndex(z->getNumKeys()-1) + 1;
            new_entry = (void*)z;
            data2=tp;
			cout<<val2<<" this"<<endl;
			if(this->root==droot)
			{
				this->depth++;
				IndexNode* newnode=new IndexNode(numIndexPointers);
				newnode->setNumKeys(2);
				newnode->setKeyAtIndex(val,0);
				newnode->setIndexPointerAt(root,0);
				newnode->setIndexPointerAt(new_entry,1);
                newnode->setKeyAtIndex(val2,1);
				newnode->setIndexPointerAt(data2,2);
				this->root=newnode;
				return ;
			}
			return;			
		}
		else
		{
			insertValAtLeafNode(droot,key);
			new_entry=NULL;
			return;
		}
	}
	else{
		IndexNode* temp=toIndexNode(root);
		int i=0;
		while(i<temp->getNumKeys() && temp->getKeyAtIndex(i)<=key)
		{
			i++;
		}
		insert(temp->getIndexPointerAt(i),key,new_entry,val,depth-1,data2,val2);
		if(new_entry==NULL)
			return;
		else{
            if(data2!=NULL)
            {
                if(temp->getNumKeys()==numIndexPointers-1)
                {
                    insertValAtIndexNode(temp,val,new_entry);
                    int va=0;
                    new_entry = splitIndexNode(temp,val2,data2,va);
                    val=va;
                    data2=NULL;
                    if(this->root==temp)
                    {
                        this->depth++;
                        IndexNode* newnode=new IndexNode(numIndexPointers);
                        newnode->setNumKeys(1);
                        newnode->setKeyAtIndex(val,0);
                        newnode->setIndexPointerAt(temp,0);
                        newnode->setIndexPointerAt(new_entry,1);
                        this->root=newnode;
                        return;
                    }

                }
                else if(temp->getNumKeys()==numIndexPointers)
                {
                    int va=0;
                    IndexNode* entry = splitIndexNode(temp,val,new_entry,va);
                    val=va;
                    if(val2 <= val)
                        insertValAtIndexNode(temp,val2,data2);
                    else 
                    	insertValAtIndexNode(entry,val2,data2);
                    new_entry=entry;
                    data2=NULL;
                    if(this->root==temp)
                    {
                        this->depth++;
                        IndexNode* newnode=new IndexNode(numIndexPointers);
                        newnode->setNumKeys(1);
                        newnode->setKeyAtIndex(val,0);
                        newnode->setIndexPointerAt(temp,0);
                        newnode->setIndexPointerAt(new_entry,1);
                        this->root=newnode;
                        return;
                    }
                }
                else{
                    insertValAtIndexNode(temp,val,new_entry);
                    insertValAtIndexNode(temp,val2,data2);
                    new_entry=NULL;
                    data2=NULL;
                    val=0;
                    return;
                }
            }
            else{
                if(temp->getNumKeys()==numIndexPointers){
                    int va=0;
                    new_entry = splitIndexNode(temp,val,new_entry,va);
                    val=va;
                    if(this->root==temp){
                        this->depth++;
                        IndexNode* newnode=new IndexNode(numIndexPointers);
                        newnode->setNumKeys(1);
                        newnode->setKeyAtIndex(val,0);
                        newnode->setIndexPointerAt(temp,0);
                        newnode->setIndexPointerAt(new_entry,1);
                        this->root=newnode;
                        return;
                    }
                }
                else{
                    insertValAtIndexNode(temp,val,new_entry);
                    new_entry=NULL;
                    val=0;
                    return;
                }
            }
			
		}
	}
}

struct store{
    int nodeID;
    void* ptr;
    int parent;
};

void BPlusTree::printtree(void *root, int depth){
    queue<store> q;
	queue<int> level;
	int nodeID = 1;
    store curNode;
	int curLevel=0,prevLevel=-1;
	depth = depth + 1;
    curNode.nodeID=nodeID;
    curNode.ptr=root;
    curNode.parent=-1;
    q.push(curNode);
	level.push(0);
	cout<<"\n Type at depth , Parent node value -> NodeID || Values || \n"<<endl;
	while(!q.empty()){
		curNode = q.front(); q.pop();
		if(prevLevel!=level.front()){
			depth = depth-1;
			prevLevel = curLevel++;
		}
		level.pop();

		if (depth == 0)
			cout << " leaf ";
		else
			cout << "Index ";

		cout << "at " << depth;

		cout << " ";
        if(curNode.parent==-1){
            cout<<"NULL";
        }
        else{
            cout<<" "<<curNode.parent;
        }
        
        cout<< "->" << curNode.nodeID<< " || ";


		if(depth==0){
			DataNode *node = toDataNode(curNode.ptr);
			for(int i=0; i<node->getNumKeys(); i++){
				cout<<node->getKeyAtIndex(i)<<", ";
			}
			cout << "||" <<endl;
		}
		else{
			IndexNode *node = toIndexNode(curNode.ptr);
            int x=curNode.nodeID;
			for(int i=0; i <= node->getNumKeys(); i++){
                store temp;
                temp.parent=x;
                temp.ptr=node->getIndexPointerAt(i);
                temp.nodeID=++nodeID;
                q.push(temp);
				level.push(curLevel);
			}
			for(int i=0; i<node->getNumKeys(); i++){
				cout<<node->getKeyAtIndex(i)<<", ";
			}
			cout << " || " << endl;
		}
	}
	cout<<endl<<endl;
}

int main(){
	BPlusTree* tree = NULL;
 	int key,choice;
 	int numIndexPointers, numDataPointers;
  	int searchResultIndex;
  do {
    cout << endl;
	cout<<"1. Create new B+Tree"<<endl;
	cout<<"2. Add an element to the B+Tree"<<endl;
	cout<<"3. Search an element to the B+Tree"<<endl;
	cout<<"4. Print B+Tree"<<endl;
	cout<<"Any other choice to exit"<<endl;
	int choice;
	cin>>choice;
	cout<<"You entered "<<choice<<endl;

    switch(choice){
      case 1:
        if(tree != NULL){
          delete tree;
        }
        cout<<"Enter max size of Index node:(odd) ";
        cin>>numIndexPointers;
        cout<<"Enter max size of Data node: ()even";
        cin>>numDataPointers;
        if(numIndexPointers%2==0 || numDataPointers%2){
        	
        	break;
        }
        tree = new BPlusTree(numIndexPointers, numDataPointers);
        break;
      
      case 2:
        if(tree == NULL){
          cout << "Create tree first" << endl;
          break;
        }
        cout << "Enter key to add ";
        cin >> key;
		int a,b;
		void* t;
        void* q;
        tree->insert(tree->root, key,t,a,tree->depth,q,b);
        break;
      case 3:
        if(tree == NULL){
          cout << "Create tree first" << endl;
          break;
        }
        cout << "Enter key to search: ";
        cin >> key;
        {
        	DataNode *searchResult = tree->search(tree->root, key, tree->depth);
	        if(searchResult == NULL){
	           cout << "Key not found." << endl;
	        }
	        else{
	          cout << "Key found" << endl;
	        }
	    }
        break;
      case 4:
        if(tree == NULL){
          cout << "Create tree first" << endl;
          break;
        }
        tree->printtree(tree->root, tree->depth);
        break;
      default:
        return 0;
    }
  }
  while(true);
	return 0;
}