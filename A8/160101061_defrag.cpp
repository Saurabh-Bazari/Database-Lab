#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#define DIR_ENTRY_LENGTH 13
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <malloc.h>
using namespace std;
int DISK_PAGE_SIZE;
struct DirectoryEntry {
    int id;
    int length;
    int start;
    bool valid;
    DirectoryEntry() {
        this->id = 0;
        this->length = 0;
        this->start = 0;
        this->valid = false;
    }
    DirectoryEntry(int id, int length, int start, bool valid) {
        this->id = id;
        this->length = length;
        this->start = start;
        this->valid = valid;
    }
};

class Page{

    public:
        vector<DirectoryEntry> arr;
        int spaceLeft;

        Page(){
            this->arr.resize(1, DirectoryEntry());
            this->spaceLeft = DISK_PAGE_SIZE - (sizeof(this->spaceLeft)+this->arr.size()*DIR_ENTRY_LENGTH);
        }
};

struct Node {
    Page data;
    struct Node* next;
    struct Node* prev;
};

class DiskFile{
    public:
        struct Node * nodePointer = NULL;
        int totalPages;

        DiskFile(){ }

        DiskFile(int n, bool * create_diskFile){
            
            for(int i=0; i<n; i++)
                appendPages(&this->nodePointer, Page());
            printf("Created %d number of pages of size %d bytes each in DiskFile...\n", n, DISK_PAGE_SIZE);
            printf("Printing created initial DiskFile...\n");
            printPages(this->nodePointer);
            this->totalPages = n;
            *create_diskFile = true;
        }

        void appendPages(struct Node** head_ref, Page new_data);
        void printPages(struct Node* node);
        void insertRecord(struct Node** head_ref, int rec_id, int rec_length, bool * inserted_records);
        void deleteRecord(struct Node** head_ref, int rec_id);
        void all_Defragment(struct Node* node);
    	void Defragment(struct Node* node,int i);
    	void insertRecord1(struct Node* head_ref, int rec_id, int rec_length);

};

int getGap(struct Node* last, int i){
  int gap;
  if(i == last->data.arr.size()-1){
    gap = (DISK_PAGE_SIZE - last->data.arr[i].start - last->data.arr[i].length - last->data.arr.size()*DIR_ENTRY_LENGTH - sizeof(int) - last->data.spaceLeft);
  }else{
    gap = last->data.arr[i+1].start - (last->data.arr[i].start + last->data.arr[i].length);
  }
  return gap;
}
    void DiskFile :: appendPages(struct Node** head_ref, Page new_data)
    {
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        struct Node *last = *head_ref;
        new_node->data  = new_data;
        new_node->next = NULL;
        if (*head_ref == NULL)
        {
           new_node->prev = NULL;
           *head_ref = new_node;
           return;
        }
        while (last->next != NULL)
            last = last->next;
        last->next = new_node;
        new_node->prev = last;
        return;
    }

    void DiskFile :: insertRecord(struct Node** head_ref, int rec_id, int rec_length, bool * inserted_records)
    {
        if(rec_length > (DISK_PAGE_SIZE - (sizeof(int)+DIR_ENTRY_LENGTH))){
            printf("Record length should be less than effective allowable Page size %d, where the maximum Page size is %d\n", (int)(DISK_PAGE_SIZE - (sizeof(int)+DIR_ENTRY_LENGTH)), DISK_PAGE_SIZE);
            *inserted_records = false;
            return;
        }
        struct Node *last = *head_ref;
        int count = 0;

        while(last != NULL) {
            count = count+1;
            /* Case-1: When DataPages are empty at the beginning */
            if( last->data.arr.size() == 1 && last->data.arr[0].valid == false && last->data.arr[0].length == 0 ){ 
                last->data.arr[0].id = rec_id;
                last->data.arr[0].length = rec_length;
                last->data.arr[0].start = 0;
                last->data.arr[0].valid = true;
                last->data.spaceLeft = last->data.spaceLeft - (rec_length);
                printf("Record inserted in Page : %d \n", count);
                *inserted_records = true;
                return;
            }
            else{
                for(int i=0; i<last->data.arr.size(); i++){
                    if(last->data.arr[i].id == 0 && last->data.arr[i].valid == false){
                        bool space_available = (rec_length <= (last->data.arr[i].length));
                        if(space_available){

                        	if (i==last->data.arr.size()-1)
                        	{
                        		last->data.spaceLeft += ( last->data.arr[i].length - rec_length);
                        	}

                            last->data.arr[i].id = rec_id;
                            last->data.arr[i].length = rec_length;
                            last->data.arr[i].valid = true;
                            printf("Record inserted in an empty slot of Page : %d \n", count);
                            return;
                        }
                    }
                }
                if (last->data.spaceLeft - (rec_length + DIR_ENTRY_LENGTH) >= 0){
                    int next_start_idx = last->data.arr[last->data.arr.size()-1].start + last->data.arr[last->data.arr.size()-1].length + getGap(last, last->data.arr.size()-1);
                    last->data.arr.push_back(DirectoryEntry(rec_id, rec_length, next_start_idx, true));
                    last->data.spaceLeft = last->data.spaceLeft - (rec_length + DIR_ENTRY_LENGTH);
                    printf("Record inserted in Page : %d \n", count);
                    return;
                }
                else{
                    if(count<this->totalPages){
                        printf("Exhausted space in Page %d. Moving on to Page %d to insert Record...\n", count, count+1);
                    }
                    else{
                        printf("Exhausted space in Page %d. Page %d is created to insert this new Record!\n", count, count+1);
                        appendPages(&this->nodePointer, Page());
                        this->totalPages = this->totalPages + 1;
                    }
                }
              last = last->next;
            }
        }
      return;
    }


     void DiskFile :: insertRecord1(struct Node* head_ref, int rec_id, int rec_length)
    {
   
        struct Node *last = head_ref;
        if( last->data.arr.size() == 1 && last->data.arr[0].valid == false && last->data.arr[0].length == 0 ){ 
            last->data.arr[0].id = rec_id;
            last->data.arr[0].length = rec_length;
            last->data.arr[0].start = 0;
            last->data.arr[0].valid = true;
            last->data.spaceLeft = last->data.spaceLeft - (rec_length);
            return;
        }
        else{
            if (last->data.spaceLeft - (rec_length + DIR_ENTRY_LENGTH) >= 0){
                int next_start_idx = last->data.arr[last->data.arr.size()-1].start + last->data.arr[last->data.arr.size()-1].length + getGap(last, last->data.arr.size()-1);
                last->data.arr.push_back(DirectoryEntry(rec_id, rec_length, next_start_idx, true));
                last->data.spaceLeft = last->data.spaceLeft - (rec_length + DIR_ENTRY_LENGTH);
                return;
            }
        }
    }

   /* Given a reference pointer to the head
   of a list and an Record ID, it deletes the corresponding record in a Page  */
    void DiskFile :: deleteRecord(struct Node** head_ref, int rec_id)
    {
        struct Node *last = *head_ref;
        bool flag = false; /* To check for duplicate entries. Currently deletes all
        duplicate entries by traversing all the pages. */
        int count = 0;
        while(last != NULL) {
            count = count + 1;
            for(int i=0; i<last->data.arr.size(); i++){
                if(last->data.arr[i].id == rec_id && last->data.arr[i].valid == true){
                    last->data.arr[i].id = 0;
                    //fix gap
                    int gap = getGap(last, i);
                    // int gap = (i == last->data.arr.size()-1) ? 0 : last->data.arr[i+1].start - (last->data.arr[i].start + last->data.arr[i].length);
                    /* Claim the left-over space through gap, restores original slot's length but not defined for last Record of a Page */
                    last->data.arr[i].length = last->data.arr[i].length + gap;
                    last->data.arr[i].valid = false;
                    flag = true;
                    printf("Record deleted from Page : %d \n", count);
                    /* No return after one deletion. After one entry deletion it checks
                    for all the duplicate entries matching IDs and deletes them. */
                }
            }
          last = last->next;
        }

      if(flag==false) // If not a single deletion has happened
        printf("Sorry the entered Record ID does not exist in any of the Pages...\n");

      return;
    }



void DiskFile :: Defragment(struct Node* node,int i)
{
    struct Node* last;

    vector<pair<int,int>> a1;
    int check = 0;
    int count = 0;

    while (node != NULL) {
        count = count+1;

        if (count==i)
        {
        	check = 1;
        	for(int i=0; i<node->data.arr.size(); i++){

        		if (node->data.arr[i].valid)
        		{
        			int p1 = node->data.arr[i].length ;
        			int r1 = node->data.arr[i].id;
        			a1.push_back({p1,r1});
        		}
       		}
       		sort(a1.begin(), a1.end());
       		node->data.arr.resize(0);
			node->data.arr.push_back(DirectoryEntry());
            node->data.spaceLeft = DISK_PAGE_SIZE - (sizeof(node->data.spaceLeft)+node->data.arr.size()*DIR_ENTRY_LENGTH);

       		for (int i = 0; i < a1.size(); ++i)
       		{
       			insertRecord1(node,a1[i].second,a1[i].first);
       		}

       		break;
        }
       
        last = node;
        node = node->next;
    }

    if (check==0)
    {
    	cout << "Enter Valid Page Number" << endl;
    }
}

void DiskFile :: all_Defragment(struct Node* node)
{
    struct Node* last=node;
  
    int count = 0;
    while (node != NULL) {
        count = count+1;
        Defragment(last,count);       
        node = node->next;
    }
}

void DiskFile :: printPages(struct Node* node)
{
    struct Node* last;
    cout << "Number of pages : " <<  this->totalPages << endl;
    printf("DiskFile traversal, displaying Pages with Records <ID,Start,Length-Gap,Valid> :\n");
    int count = 0;
    while (node != NULL) {
        count = count+1;
        printf("Page:%d  Records{", count);
        for(int i=0; i<node->data.arr.size(); i++){
            int gap = getGap(node, i);
            
            if(i!=node->data.arr.size()-1)
                printf("<%d,%d,%d-%d,%s> ", node->data.arr[i].id, node->data.arr[i].start+1, node->data.arr[i].length, gap, node->data.arr[i].valid ? "true" : "false");
            else
                printf("<%d,%d,%d-%d,%s>", node->data.arr[i].id, node->data.arr[i].start+1, node->data.arr[i].length, gap, node->data.arr[i].valid ? "true" : "false");
        }
        printf("}\n");


        last = node;
        node = node->next;
    }
}

int main()
{
    DiskFile d;
    bool create_diskFile = false;
    bool inserted_records = false;
    while(true){
        int x;
        printf("\nEnter :- 1:Create New DiskFile, 2:Insert Record, 3:Delete Record, 4:Display DiskFile Structure, -1:Exit\n");
        printf("Choice : ");
        cin >> x;
        switch (x)
        {
           case 1: {
                       if(create_diskFile==false){
                       	    printf("Enter number of DataPages Size to create : ");
                       	    cin >> DISK_PAGE_SIZE;
                           printf("Enter number of DataPages to create : ");
                           int n;
                           cin >> n;
                           d = DiskFile(n, &create_diskFile);
                       }
                       else
                           printf("DiskFile has already been created.\n");
                   }
                   break;

           case 2: {
                       if(create_diskFile){
                           printf("Enter Record identifier in int to insert into Pages : ");
                           int id;
                           cin >> id;
                           printf("Enter Record length in bytes : ");
                           int l;
                           cin >> l;
                           d.insertRecord(&d.nodePointer,id,l,&inserted_records);
                       }
                       else
                           printf("You can not enter Records into Pages without creating the initial DiskFile.\n");
                   }
                   break;

           case 3: {
                       if(create_diskFile && inserted_records){
                           printf("Enter Record identifier in int to delete from Pages : ");
                           int id;
                           cin >> id;
                           d.deleteRecord(&d.nodePointer,id);
                       }
                       else
                           printf("You can not delete Records without creating the initial DiskFile or without inserting few Records into it.\n");
                   }
                   break;

           case 4: {
                       if(create_diskFile){
                           d.printPages(d.nodePointer);
                       }
                       else
                           printf("You can not view Pages & Records without creating the initial DiskFile.\n");
                   }
                   break;
           case 5: {
                       if(create_diskFile){
                       	int n1;
                       		cin >> n1;
                           d.Defragment(d.nodePointer,n1);
                       }
                       else
                           printf("You can not Defragment without creating the initial DiskFile.\n");
                   }
                   break;
           case 6: {
                       if(create_diskFile){
                           d.all_Defragment(d.nodePointer);
                       }
                       else
                           printf("You can not Defragment without creating the initial DiskFile.\n");
                   }
                   break;

           case -1: exit(0);

           default: printf("Choice other than 1, 2, 3, 4 and -1\n");
                    continue;
        }
    }
}
