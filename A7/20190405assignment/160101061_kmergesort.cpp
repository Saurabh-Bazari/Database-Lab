#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

int DISK_PAGE_SIZE;
int MEM_FRAME_SIZE;
int k;

class Page{
public:
	vector<int> arr;
	int validEntries;
	//initializes an empty page with invalid entries i.e. -1
	Page(){
		arr.resize(DISK_PAGE_SIZE, -1);
		validEntries = 0;
	}
	void writePage();
	void fillPage(vector<int> &v);
};

class DiskFile{
public:
	vector<Page> data;
	int totalPages;
	int size; //in terms of number of entries
	
	DiskFile(){
		totalPages = 0;
		size = 0;
	}
	
	DiskFile(int s){
		size = 0;
		totalPages = s;
		data.resize(totalPages);
	}
	void readDiskFile();
	void writeDiskFile();
	void DiskFileCopy( DiskFile &f, int startPage, int endPage);
};

vector < DiskFile > datasaved;

class Frame{
public:
	vector<int> arr;
	int validEntries;
	Frame()
	{
		arr.resize(MEM_FRAME_SIZE, -1);
		validEntries = 0;
	}
	
	//fill Frame with given vector
	void fillFrame(vector<int> &v);
	void printFrame();
};

class MainMemory{

public:
	vector<Frame> data;
	int totalFrames;
	vector<bool> valid;

	MainMemory(int s){
		totalFrames = s;
		data.resize(s);
		valid.resize(s);
		for(int i = 0; i < s; i++)
			valid[i] = false;
		cout << "Memory created" << endl;
	}

	int loadPage(DiskFile &f, int n);
	int getEmptyFrame();
	int getValidEntries(int f);
	int getVal(int f, int i);
	void setVal(int f, int i, int val);
	void writeFrame(DiskFile &inputFile, int f, int p);
	void freeFrame(int f);
};

int runSize=0; 
int totalPass=0; 
int totalRuns=-1;

//creates DiskFile by reading input file through cin
void DiskFile :: readDiskFile(){
	
	int i = 0;
	this->size  = 0;
	this->totalPages = 0; 
	Page t;
	vector<int> temp(DISK_PAGE_SIZE,-1);
	bool flag = true;
	int x;
	int p;
	cin >> p;
	while(p){
		p--;
		cin >> x;

		flag = false;
		temp[i++] = x;
		this->size++;
		
		if(i == DISK_PAGE_SIZE){
			t.fillPage(temp);
			this->data.push_back(t);
			flag = true;
			this->totalPages++;
			i = 0;
		}
	}

	if(!flag){
		
		while(i != DISK_PAGE_SIZE){
			temp[i++] = -1;	
		}
		t.fillPage(temp);
		this->data.push_back(t);
		this->totalPages++;
	}
}

//writes disk file to console
void DiskFile :: writeDiskFile(){
	cout << "Contents of Disk File: " <<endl;
	for(int  i = 0; i < totalPages; i++){
		cout << "Page: " << i+1 << ": ";
		this->data[i].writePage();
	}
}

//copies contents of disk file f into destination disk file from startPage to endPage
void DiskFile :: DiskFileCopy(DiskFile &f, int startPage, int endPage){
	int j = 0; //start of source file
	int i = startPage; //start of destination file

	while(i <= endPage && j < f.totalPages && i < this->totalPages){
		this->data[i].validEntries = f.data[j].validEntries;
		for(int k = 0; k < DISK_PAGE_SIZE; k++){
			this->data[i].arr[k] = f.data[j].arr[k];
		}
		i++;
		j++;
	}
}

//writes page to cout
void Page :: writePage(){

	for(int i = 0; i < DISK_PAGE_SIZE; i++){
		if(this->arr[i] == -1)
			break;
		cout << this->arr[i] << " ";
	}
	cout << endl;
}

//fills page with contents from vector v
void Page :: fillPage(vector<int> &v){
	
	if(v.size() < DISK_PAGE_SIZE){
		cout << "vector should contain atleast as many entries as a page" << endl;
		exit(1);
	}
	int i = 0;
	this->validEntries = 0;
	for(i=0; i < v.size() && i < DISK_PAGE_SIZE; i++){
		if(v[i] == -1){
			this->validEntries = i;
			this->arr[i] = v[i];
			break;
		}
		this->arr[i] = v[i];
	}
	if(this->validEntries==0){
		this->validEntries=DISK_PAGE_SIZE;
	}
}

//fills frame with data from vector v
void Frame :: fillFrame(vector<int> &v){
	int i = 0;
	bool flag = false;
	while(i < v.size() && i < MEM_FRAME_SIZE){
		this->arr[i] = v[i];
		if(v[i] == -1){
			flag = true;
			this->validEntries = i;
			i++;
			break;
		}
		i++;
	}
	while(i < MEM_FRAME_SIZE){
		this->arr[i] = -1;
		i++;
	}
	if(!flag)
		this->validEntries = MEM_FRAME_SIZE;
}

//Prints all valid entries of a frame
void Frame :: printFrame()
{
	for(int i = 0; i < this->validEntries; i++)
		cout << this->arr[i] << endl;
}

//loads nth page of disk file f to an empty frame if available
int MainMemory :: loadPage(DiskFile &f, int n){

	if(n >= f.totalPages)
		return -1; // invalid page

	for(int i = 0; i < this->totalFrames; i++){
		if(!this->valid[i]){
			// cout<<f.data[n].arr.size()<<" check "<<endl;
			// cout<<this->data[i].arr.size()<<" dsfsd"<<endl;
			this->data[i].fillFrame(f.data[n].arr);
			this->valid[i] = true;
			return i;
		}
	}
	return -1; //no empty frame is available
}

//returns an empty frame if available
int MainMemory :: getEmptyFrame(){

	vector<int> v(MEM_FRAME_SIZE, -1);

	for(int i = 0; i < this->totalFrames; i++){
		if(!this->valid[i]){
			this->data[i].fillFrame(v);
			this->valid[i] = true;
			return i;
		}
	}
	return -1; //no empty frame is available

}

//returns number of valid entries in frame f
int MainMemory :: getValidEntries(int f){
	if(f >= this->totalFrames || !this->valid[f])
		return -1;
	return this->data[f].validEntries;
}


// returns value stored at location i in frame f
int MainMemory :: getVal(int f, int i){
	if(!this->valid[f] || i >= MEM_FRAME_SIZE)
		return -1;
	return this->data[f].arr[i];
}

//assigns value val to ith location of frame f
void MainMemory :: setVal(int f, int i, int val){
	if(!this->valid[f] || i >= MEM_FRAME_SIZE){
		cout << "accessing invalid address" << endl;
		exit(1);
	}

	this->data[f].arr[i] = val;
	if(this->data[f].validEntries == i)
		this->data[f].validEntries = i+1;

}

//write Frame f to file at page p
void MainMemory :: writeFrame(DiskFile &inputFile, int f, int p){
	inputFile.data[p].validEntries = getValidEntries(f);

	for(int i = 0; i <	MEM_FRAME_SIZE; i++)
		inputFile.data[p].arr[i] = this->data[f].arr[i];
}

//clears frame f
void MainMemory :: freeFrame(int f){
	if(f < totalFrames){
		this->valid[f] = false;
		this->data[f].validEntries = 0;
	}
}	

//creates initial runs of 1 page size
void firstPass(DiskFile &inputFile, MainMemory &memory){
	
	int frame = -1;
	int ct=0;
	vector<int> tp;
	int st=0;
	for(int i = 0; i < inputFile.totalPages; i++){
		if(ct<memory.totalFrames){
			for(int j=0;j<inputFile.data[i].validEntries;j++){
				tp.push_back(inputFile.data[i].arr[j]);
			}
			ct++;
		}
		if(ct==memory.totalFrames){
			sort(tp.begin(),tp.end());
			for(int j=0;j<memory.totalFrames && j+st<inputFile.totalPages;j++){
				vector<int> t(DISK_PAGE_SIZE,-1);				
				for(int k=0;k<DISK_PAGE_SIZE && tp.size()>0;k++){
					t[k]=tp[0];
					tp.erase(tp.begin());
				}
				inputFile.data[j+st].fillPage(t);
			}
			ct=0;
			st=i+1;
		}
	}
	if(ct>0){
		sort(tp.begin(),tp.end());
		for(int j=0;j<memory.totalFrames && j+st<inputFile.totalPages;j++){
			vector<int> t(DISK_PAGE_SIZE,-1);				
			for(int k=0;k<DISK_PAGE_SIZE && tp.size()>0;k++){
				t[k]=tp[0];
				tp.erase(tp.begin());
			}
			inputFile.data[j+st].fillPage(t);
		}
	}

	runSize = memory.totalFrames;
	totalPass = 1;
	totalRuns = ceil(inputFile.totalPages/runSize);
	cout << "First Pass Performed" << endl;
	// inputFile.writeDiskFile(); //print file to cout
}

//sorts each frame
void sortFrame(MainMemory &memory, int f){
	sort(memory.data[f].arr.begin(), memory.data[f].arr.begin()+memory.getValidEntries(f));
}

//Performs merging of 2 runs
void merge(DiskFile &inputFile, MainMemory &memory, vector<int> &indicies, vector<int> &ends){
	int leftStart=indicies[0];
	int rightEnd=ends[ends.size()-1];
	int finalRunSize = rightEnd - leftStart + 1;
	DiskFile tempFile(finalRunSize);

	bool flag = false;
	int currPage = 0;
	vector<int> frameNo;
	for(auto itr=indicies.begin();itr!=indicies.end();itr++){
		int f=memory.loadPage(inputFile, *itr);
		frameNo.push_back(f);
		if(f==-1){
			cout << "Can't proceed further due to unavailability of memory or invalid Page access" << endl;
			exit(1);
		}
	}
	int resFrame = memory.getEmptyFrame();	
	if(resFrame == -1){
		cout << "Can't proceed further due to unavailability of memory or invalid Page access" << endl;
		exit(1); 
	}

	int leftIndex = 0;
	int rightIndex = 0;
	int resIndex = 0;
	vector<int> pointers(frameNo.size(),0);
	while(indicies.size()>0){
		int mini=2e9;
		int minindex=-1;
		for(int i=0;i<frameNo.size();i++){
			if(pointers[i] == memory.getValidEntries(frameNo[i])){
				memory.freeFrame(frameNo[i]);
				indicies[i]++;
				if(indicies[i]<=ends[i]){
					pointers[i]=0;
					frameNo[i]=memory.loadPage(inputFile, indicies[i]);
				}
				else{
					indicies.erase(indicies.begin()+i);
					ends.erase(ends.begin()+i);
					pointers.erase(pointers.begin()+i);
					frameNo.erase(frameNo.begin()+i);
					i--;
					continue;
				}
			}
			int x= memory.getVal(frameNo[i], pointers[i]);
			if(x < mini){
				mini=x;
				minindex=i;
			}
		}
		if(minindex!=-1){
			pointers[minindex]++;
			memory.setVal(resFrame, resIndex, mini);
			resIndex++;
		}
		
		
		if(resIndex == MEM_FRAME_SIZE){
			memory.writeFrame(tempFile, resFrame, currPage);
			memory.freeFrame(resFrame);
			resFrame = memory.getEmptyFrame();
			currPage++; 
			resIndex = 0;
		}
	}

	if(resIndex>0){
		memory.writeFrame(tempFile, resFrame, currPage);
	}
	memory.freeFrame(resFrame);
	inputFile.DiskFileCopy(tempFile, leftStart, rightEnd);

}

void twoWaySort(DiskFile &inputFile, MainMemory &memory){
	if(memory.totalFrames < 3)
		cout << "Error: Two way merge sort requires atleast 3 frames" << endl; 
	
	datasaved.push_back(inputFile);
	firstPass(inputFile, memory);
	datasaved.push_back(inputFile);

	int leftStart;
	int t=(memory.totalFrames-k)/k;
	
	for(;runSize<inputFile.totalPages;runSize*=t){
		cout << "Pass: " << totalPass+1 << "\trunSize: " << runSize << endl;
		vector<int> indices;
		vector<int> ends;
		leftStart=0;
		int ct=0;
		int rightEnd=0;
		while(leftStart<inputFile.totalPages){
			ct=0;
			while(ct<t && leftStart<inputFile.totalPages){
				indices.push_back(leftStart);
				leftStart+=runSize;
				ends.push_back(min(leftStart-1, inputFile.totalPages-1));
				ct++;
			}
			
			merge(inputFile, memory, indices, ends);
		}
		datasaved.push_back(inputFile);
		totalPass++;
	}
	cout << "Total Passes required: " << totalPass << endl;
}

int main()
{	
	int x;
	cin>>DISK_PAGE_SIZE;
	cin >> x;
	cin >> k;
	MEM_FRAME_SIZE=DISK_PAGE_SIZE;
	MainMemory mm(x);
	
	DiskFile f;
	f.readDiskFile();
	// f.writeDiskFile();
	
	twoWaySort(f,mm);

	// f.writeDiskFile(); 

	int n1,n2;
	cin >> n1 ;

	while(n1!=-1)
	{
		cin >> n2;
		cout <<  "Pass: " << n1 << " Page: " << n2 << " :- ";
		if (datasaved.size() <= n1)
		{
			cout << "Invalid pass number" << endl;
			cin >> n1;
			continue;
		}

		if (datasaved[n1].data.size() < n2)
		{
			cout << "Invalid page number" << endl;
			cin >> n1;
			continue;
		}

		vector <int> v1 = datasaved[n1].data[n2-1].arr;
		int size1 = datasaved[n1].data[n2-1].validEntries;

		for (int i1 = 0; i1 < size1; ++i1)
			cout << v1[i1] << " ";
		cout << endl;
		cin >> n1;
	}
	return 0;
}