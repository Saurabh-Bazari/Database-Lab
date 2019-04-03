#include <bits/stdc++.h>
using namespace std;
#define ll long long int
int n,m;
class Bucket{
public:
	vector<int> data;
	vector<Bucket*> childptr;
	Bucket * right , *left;
	bool isLeaf;
};

class Bplus
{	
	public:
	Bucket * root = NULL;
	Bucket * head = NULL;

 	void createTree();
 	void printTree();
 	void pri(Bucket*);
 	void insert1(int);
 	bool search(int);
 	int insertElement(Bucket*,int);
};

void Bplus::createTree()
{
	Bucket * b = new Bucket();
	this->root = b;
	this->head = b;
	b-> isLeaf = true;
	b->right = NULL;
	b->left = NULL;
	return;
}

int Bplus::insertElement(Bucket * b , int val)
{
	if (b->isLeaf==true)
	{
		int p1=b->data.size();
		for (int i = 0; i < b->data.size(); ++i)
		{
			if (b->data[i]>val)
			{
				p1 = i;
				break;
			}
		}
		b->data.insert(b->data.begin()+p1,val);
		if (b->data.size()>n)
		{
			// cout << "split leaf with starting data " << b->data[0] << endl;
			Bucket * b1 = new Bucket();
			b1->right = b->right;
			b->right = b1;
			b1->left = b;
			Bucket * b4 = b1->right;
			if ( b4!=NULL ){
				b4->left = b1;
			}
			b1->isLeaf= true;

			for (int i = n/2+1; i <= n; ++i)
				b1->data.push_back(b->data[i]);
			for (int i = n/2+1; i <= n; ++i)
				b->data.pop_back();
			return 1;
		}
		return 0;
	}
	else
	{
		Bucket * b1 = b->childptr[0];
		int size1 = b->data.size();
		for (int i = 0; i < size1; ++i)
		{
			if (b->data[i]>val)
				break;
			b1 = b->childptr[i+1];
		}
		int c = insertElement(b1,val);

		if (c==1)
		{
			Bucket *b2  = b1->right;
			int d = b2->data[0];
			if (b2->isLeaf==false)
			{
				b2->data.erase(b2->data.begin());
			}
			int p=b->data.size();
			for (int i = 0; i < b->data.size(); ++i)
			{
				if (b->data[i]>d)
				{
					p = i;
					break;
				}
			}
			b->data.insert(b->data.begin()+p,d);
			b->childptr.insert(b->childptr.begin()+p+1,b2);

			if (b->data.size() > m)
			{
				cout << "split index node with starting index " << b->data[0] << endl;
				Bucket * b3 = new Bucket();
				b3->right = b->right;
				b->right = b3;
				b3->left =b;
				Bucket * b4 = b3->right;
				if ( b4!=NULL ){
					b4->left = b3;
				}

				b3->isLeaf = false;

				for (int i = m/2 + 1; i <= m; ++i)
				{
					b3->data.push_back(b->data[i]);
					b3->childptr.push_back(b->childptr[i+1]);
				}
				for (int i = m/2 + 1; i <= m; ++i)
				{
					b->data.pop_back();
					b->childptr.pop_back();
				}
				return 1;
			}
			return 0;
		}
	}
}

void Bplus::insert1(int val)
{
	int c = insertElement(this->root,val);
	if (c==1)
	{
		Bucket *r1 = new Bucket();
		r1->isLeaf = false;
		r1->right = NULL;
		r1->left = NULL;

		Bucket *b1 = this->root;
		Bucket *b2  = b1->right;
		r1->childptr.push_back(b1);
		r1->childptr.push_back(b2);
		this->root = r1;

		int d = b2->data[0];
		if (b2->isLeaf==false)
		{
			b2->data.erase(b2->data.begin());
		}
		r1->data.push_back(d);
	}
}

bool Bplus::search(int val)
{
	Bucket * r1 = this->root;

	while(!r1->isLeaf)
	{
		int d = r1->data.size();
		for (int i = 0; i < r1->data.size(); ++i)
		{
			// cout << r1->data[i] << " " ;
			if (r1->data[i] > val)
			{
				d = i;
				break;
			}
		}
		r1 = r1->childptr[d];
	}

	for (int i = 0; i < r1->data.size(); ++i)
	{
		// cout << r1->data[i] << " " ;
		if (r1->data[i] == val)
			return true;
		if (r1->data[i] > val)
			return false;
	}
	return false;
}

void Bplus::pri(Bucket * r1)
{
	cout << r1->childptr.size() << " , " ;
	for (int i = 0; i < r1->data.size(); ++i)
		cout << r1->data[i]  << " ";
	cout << "||";
}

void Bplus::printTree()
{
	cout << endl;
	cout << "Level : || Number of Childrern , Data ||" << endl;;
	Bucket * r = this->root;
	cout << endl;
	int i = 0;
	while(r)
	{
		cout << i;
		i++;
		if (r->isLeaf)
		{
			cout << " (leaf)" ;
		}
		Bucket *r1 = r;
		cout << " : || "; 
		while(r1)
		{
			pri(r1);
			r1 = r1->right;
		}

		if (r->childptr.size()==0)
			break;

		r = r->childptr[0];
		cout << endl;
	}
	cout << endl << endl;
}

int main(){

	Bplus * d = NULL;

	do{
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
				if(d!=NULL)
					delete d;
				d = new Bplus();
				cout << "Enter max index node size : " ;
				cin>>m;
				cout << "Enter max data node size : " ;
				cin>>n;
				d->createTree();
				break;
			case 2:
				if(d==NULL)
				{	
					cout << "First Create Tree" << endl;
					break;
				}
				int add;
				cout<<"Enter element to add: ";
				cin>>add;
				d->insert1(add);
								d->printTree();

				break;
			case 3:
				if(d==NULL)
				{	
					cout << "First Create Tree" << endl;
					break;
				}
				int search;
				cout<<"Enter element to search: ";
				cin>>search;
				if(d->search(search))
					cout<<"Element is found"<<endl;
				else
					cout<<"Element is not found"<<endl;
				break;
			case 4:
				if(d==NULL)
				{	
					cout << "First Create Tree" << endl;
					break;
				}
				d->printTree();
				break;
			default:
				return 0;
		}
	}while(true);

	return 0;
}

// 1 4 3  2 1 2 5 2 9 2 11 2 15 2 3 2 4 2 2 2 6 2 7 2 8 2 10   