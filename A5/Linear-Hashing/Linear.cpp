#include<bits/stdc++.h>

using namespace std;

class Bucket{
public:
	list<int> bucket;
};

class LinearHashing{
	int level,bucketSize,next,noOfBuckets;
	vector<Bucket*> dir;
public:
	void createHashTable(int, int);
	void insertElement(int val);
	bool searchElement(int val);
	int getBucketIndex(int val);
	void splitBucket(int bucketIndex);
	int twoPower(int p);
	void printHashTable();
    void increaseBuckets();
};

int LinearHashing::twoPower(int p){
	return 1<<p;
}

void LinearHashing::createHashTable(int noOfBuckets, int bucketSize){
	this->noOfBuckets = noOfBuckets;
	this->dir.resize(noOfBuckets);
	this->bucketSize=bucketSize;
    this->level=0;
    this->next=0;
	for(int i=0;i<noOfBuckets;i++){
		this->dir[i] = new Bucket();
	}
}

int LinearHashing::getBucketIndex(int val){ 
    if(val%(this->twoPower(this->level)*this->noOfBuckets) >= this->next){
        return val%(this->twoPower(this->level)*this->noOfBuckets);
    }
    else{
        return val%(2*(this->noOfBuckets)*(this->twoPower(this->level)));
    }
}

void LinearHashing::insertElement(int val){
	int bucketIndex = this->getBucketIndex(val);
	if((this->dir[bucketIndex]->bucket.size())%(this->bucketSize) == 0 && this->dir[bucketIndex]->bucket.size()>0){
            this->increaseBuckets();
            this->splitBucket(this->next);
    }
    int newIndex = this->getBucketIndex(val);
    this->dir[newIndex]->bucket.push_back(val);
}

bool LinearHashing::searchElement(int val){
	int bucketIndex = this->getBucketIndex(val);
	cout<<bucketIndex<<endl;
	for(auto itr = this->dir[bucketIndex]->bucket.begin(); itr!=this->dir[bucketIndex]->bucket.end(); itr++)
		if(*itr==val)
			return true;
	return false;
}

void LinearHashing::increaseBuckets(){
    int size = this->dir.size();
    this->dir.resize(size+1);
    this->dir[size]=new Bucket();
}

void LinearHashing::splitBucket(int bucketIndex){

	list<int> &firstBucket = this->dir[bucketIndex]->bucket;
	list<int> &secondBucket = this->dir[this->dir.size()-1]->bucket;
	for(auto itr=firstBucket.begin();itr!=firstBucket.end();){
		int bIndex=(*itr)%(2*this->twoPower(this->level)*this->noOfBuckets);
		if(bIndex == this->dir.size()-1){
			secondBucket.push_back(*itr);
			itr = firstBucket.erase(itr);
		}
		else itr++;
	}
	this->next = (this->next+1)%((twoPower(level))*(this->noOfBuckets));
    if(this->next == 0){
        this->level++;
    }
}

void LinearHashing::printHashTable(){
	cout<<"\n ***** Hash Table ***** \n"<<endl;
	cout<<"noOfBuckets :: " << this->noOfBuckets<<endl;
	cout<<"pos :: " << this->next <<endl;
	unordered_set<Bucket*> uset;
	cout<<"| Elements |"<<endl;
	for(int i=0; i<this->dir.size(); i++){
		if(uset.find(this->dir[i])!=uset.end())
			continue;
		uset.insert(this->dir[i]);
		for(auto l:this->dir[i]->bucket)
			cout<<l<<" ";
		cout<<"|" <<endl;
	}
	cout<<endl;
}


int main(){
	
	int choice;
	LinearHashing *d=NULL;
	do{
		cout<<"1. Create new LinearHashTable"<<endl;
		cout<<"2. Add an element to the HashTable"<<endl;
		cout<<"3. Search an element to the HashTable"<<endl;
		cout<<"4. Print HashTable"<<endl;
		cout<<"Any other choice to exit"<<endl;
		cout<<"Enter your choice: ";
		cin>>choice;
		cout<<"You entered "<<choice<<endl;
		switch(choice){
			case 1:
				if(d!=NULL)
					delete d;
				d= new LinearHashing();
				int noOfBuckets;
				cout<<"Enter Numner of Buckets: ";
				cin>>noOfBuckets;
				int bucketSize;
				cout<<"Enter number of entries in bucket: ";
				cin>>bucketSize;
				d->createHashTable(noOfBuckets,bucketSize);
				break;
			case 2:
				int add;
				cout<<"Enter element to add: ";
				cin>>add;
				d->insertElement(add);
				break;
			case 3:
				int search;
				cout<<"Enter element to search: ";
				cin>>search;
				if(d->searchElement(search))
					cout<<"Element is found"<<endl;
				else
					cout<<"Element is not found"<<endl;
				break;
			case 4:
				d->printHashTable();
				break;
			default:
				return 0;
		}
	}while(true);
}