#include<iostream>
#include<vector>
#include<list>
#include<unordered_set>

using namespace std;

class Bucket{
public:
	int localDepth;
	list<int> bucket;
};
class ExtendibleHashing{
	int globalDepth,bucketSize;
	vector<Bucket*> dir;
public:
	void createHashTable(int globalDepth,int bucketSize);
	void insertElement(int val);
	bool searchElement(int val);
	bool deleteElement(int val);
	int getBucketIndex(int val);
	void doubleHashTable();
	void splitBucket(int bucketIndex);
	int twoPower(int p);
	void printHashTable();
};
void ExtendibleHashing::createHashTable(int globalDepth,int bucketSize){
	int buckets = twoPower(globalDepth);
	this->globalDepth = globalDepth;
	this->dir.resize(buckets);
	this->bucketSize=bucketSize;
	for(int i=0;i<buckets;i++){
		this->dir[i] = new Bucket();
		this->dir[i]->localDepth = globalDepth;
	}
}

int ExtendibleHashing::getBucketIndex(int val){
	int lsbOfOnes = twoPower(this->globalDepth)-1;
	return val & lsbOfOnes;  
}

void ExtendibleHashing::insertElement(int val){
	
	int bucketIndex = this->getBucketIndex(val);

	if (this->dir[bucketIndex]->bucket.size() != this->bucketSize )
	{
		this->dir[bucketIndex]->bucket.push_back(val);
	}
	else
	{
		int count = 0;
		for(int i=0; i<this->dir.size(); i++){
			if (this->dir[i]->bucket.size() <= this->bucketSize)
				count++;
		}
		if (count==1){
			this->doubleHashTable();
			int newindex = this->getBucketIndex(val);
			this->dir[newindex]->bucket.push_back(val);	
		}
		else{
			this->dir[bucketIndex]->bucket.push_back(val);
		}
	}
}

bool ExtendibleHashing::searchElement(int val){
	int bucketIndex = this->getBucketIndex(val);
	for(auto itr = this->dir[bucketIndex]->bucket.begin(); itr!=this->dir[bucketIndex]->bucket.end(); itr++)
		if(*itr==val)
			return true;
	return false;
}

void ExtendibleHashing::doubleHashTable(){
	int size = this->dir.size();
	this->globalDepth++;
	this->dir.resize(2*size);
	for(int i=size;i<2*size;i++){
		this->splitBucket(i-size);
	}
}

void ExtendibleHashing::splitBucket(int bucketIndex){
	
	int localDepthPower = twoPower(this->dir[bucketIndex]->localDepth);
	int firstBucketIndex = bucketIndex & (localDepthPower-1);
	int secondBucketIndex = firstBucketIndex + localDepthPower;

	this->dir[secondBucketIndex] = new  Bucket();
	this->dir[secondBucketIndex]->localDepth = ++this->dir[firstBucketIndex]->localDepth;
	
	list<int> &firstBucket = this->dir[firstBucketIndex]->bucket;
	list<int> &secondBucket = this->dir[secondBucketIndex]->bucket;

	for(auto itr=firstBucket.begin();itr!=firstBucket.end();){
		if(*itr & localDepthPower){
			secondBucket.push_back(*itr);
			itr = firstBucket.erase(itr);
		}
		else itr++;
	}
}

int ExtendibleHashing::twoPower(int p){
	return 1<<p;
}

void ExtendibleHashing::printHashTable(){
	cout<<"\n ***** Hash Table ***** \n"<<endl;
	cout<<"globalDepth :: " << this->globalDepth<<endl;
	cout<<"Bucket Number :: | Elements |"<<endl;
	for(int i=0; i<this->dir.size(); i++){
		cout<<"Bucket " << i <<" :: | ";
		for(auto l:this->dir[i]->bucket)
			cout<<l<<" ";
		cout<<"|" <<endl;
	}
	cout<<endl;
}

bool ExtendibleHashing::deleteElement(int val){
	int bucketIndex = this->getBucketIndex(val);
	for(auto itr = this->dir[bucketIndex]->bucket.begin(); itr!=this->dir[bucketIndex]->bucket.end(); itr++){
		if(*itr==val){
			this->dir[bucketIndex]->bucket.erase(itr);
			return true;
		}
	}
	return false;
}

int main(){
	
	int choice;
	ExtendibleHashing *d=NULL;
	do{
		cout<<"1. Create new ExtendibleHashTable"<<endl;
		cout<<"2. Add an element to the HashTable"<<endl;
		cout<<"3. Search an element to the HashTable"<<endl;
		cout<<"4. Delete an element from the HashTable"<<endl;
		cout<<"5. Print HashTable"<<endl;

		cout<<"Any other choice to exit"<<endl;
		cout<<"Enter your choice: ";
		cin>>choice;
		cout<<"You entered "<<choice<<endl;
		switch(choice){
			case 1:
				if(d!=NULL)
					delete d;
				d= new ExtendibleHashing();
				int globalDepth;
				cout<<"Enter global depth: ";
				cin>>globalDepth;
				int bucketSize;
				cout<<"Enter number of entries in bucket: ";
				cin>>bucketSize;
				d->createHashTable(globalDepth,bucketSize);
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
				int del;
				cout<<"Enter element to delete: ";
				cin>>del;
				if(d->deleteElement(del))
					cout<<"Element is deleted"<<endl;
				else
					cout<<"Element is not found"<<endl;
				break;
			case 5:
				d->printHashTable();
				break;
			default:
				return 0;
		}
	}while(true);
}