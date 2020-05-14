#include <stdio.h>
#include <stdlib.h>
#include "hashset.h"

 /*This is the prehash function from Lab 6. Modify it to work with a void* instead of a char array
unsigned long prehash(unsigned char *str) {
    unsigned long h = 5381;
    int c;

    while (c = *str++) { 
			h = ((h << 5) + h) + c;
    }

    return h;
}
*/

unsigned long prehash(void* item, unsigned int item_size) { // preahash to unsigned long
    unsigned long h = 5381;
    int c;

    for(int i=0; i<item_size; i++) { 
        c = item++;
		h = ((h << 5) + h) + c;
    }
    return h;
}


unsigned long hash(unsigned long prehash, unsigned long buckets){ // hash prehash value to a bucket
    unsigned long bucket = (17*prehash/10)%buckets;
    return bucket;
}

void initHashSet(HashSet* hashset_pointer, unsigned int size){ // initializes hashset
    List** array = malloc(sizeof(List*)*size); // mallocs a list of list pointers
    
    for(int i=0; i<size; i++){
        List* temp = malloc(sizeof(List*));
        initList(temp);
        *(array+i) = temp;
    }
    hashset_pointer->table = array;
    hashset_pointer->size = size;
    int x = 0;
    hashset_pointer->load_factor = (char)x;
}
bool insertItem(HashSet* hashset_pointer, void* item, unsigned int item_size){
    unsigned long pre_hash = prehash(item, item_size);
    unsigned long index = hash(pre_hash, hashset_pointer->size);
    //printf("Bucket: %ld\n",index);
    if(findItem(hashset_pointer, item, item_size)){// check if the item is already in the hashtable
        return false;
    }
    insertAtTail(hashset_pointer->table[index],item); // inserts item at tail of linked list
    //printf("Inserted\n");
    float percent = 100 * 1/(hashset_pointer->size);
    //printf("percent: %f\n", percent);
    hashset_pointer->load_factor += (char)percent; // updates the load factor
    //printf("Load factor: %d\n", (int)hashset_pointer->load_factor);
    if(hashset_pointer->load_factor > (char)70){ // if load factor exceeds 70%, call resize 
        printf("Resizing called\n");
        resizeTable(hashset_pointer, (hashset_pointer->size)*2);
        printf("Resize successful\n");
        printf("New size: %d\n", hashset_pointer->size);
    }

    return true;
}

bool removeItem(HashSet* hashset_pointer, void* item, unsigned int item_size){ // removes and item from hashtable
    unsigned long pre_hash = prehash(item, item_size);
    unsigned long index = hash(pre_hash, hashset_pointer->size);

    int list_index = findIndexOfItem(hashset_pointer->table[index], item); // finds where in the linked list the item is
    if(list_index == -1){
        //printf("Not found\n");
        return false;
    }
    //printf("Found\n");
    void* removed = removeAtIndex(hashset_pointer->table[index], list_index); // removes that item
    //printf("Removed\n");
    float percent = 100 * 1/(hashset_pointer->size);
    //printf("percent: %f\n", percent);
    hashset_pointer->load_factor -= (char)percent; // updates the load factor
    //printf("Load factor: %d\n", (int)hashset_pointer->load_factor);
    return true;
}

bool findItem(HashSet* hashset_pointer, void* item, unsigned int item_size){ // finds the item in the hash table
    unsigned long pre_hash = prehash(item, item_size);
    unsigned long index = hash(pre_hash, hashset_pointer->size);
    int list_index = findIndexOfItem(hashset_pointer->table[index], item); // finds index of item in linked list
    if(list_index == -1){ // if the iem was not found, return false
        return false;
    }
    //printf("Item found is: %p\n ", hashset_pointer->table[index]->tail->item);
    return true;
}

void resizeTable(HashSet* hashset_pointer, unsigned int new_size){   // resizes the table when load factor is too large
    HashSet *new_hs;
	new_hs = (HashSet*) malloc(sizeof(HashSet)); 
	initHashSet(new_hs, new_size); // initialized new larger hashset
    for(int i=0; i< hashset_pointer->size; i++){
        Node* current = hashset_pointer->table[i]->head;
        int count =0;
        while(current != NULL){
            insertItem(new_hs,current->item,sizeof(int)); // inserts item into new hashset
            if(current == NULL || current == current->next) break;
		    current = current->next;
            count++;
            //if(count == 30)break;
        }
	}
    hashset_pointer->size = new_size; // sets attributes to the original hashset
    hashset_pointer->table = new_hs->table;
    hashset_pointer->load_factor = new_hs->load_factor;
    free(new_hs);
    //printf("New size: %d\n", hashset_pointer->size);
}

void printHashSet(HashSet* hashset_pointer){ // prints out the complete hashtable
    for(int i=0; i< hashset_pointer->size; i++){
        Node* current = hashset_pointer->table[i]->head;
        printf("Bucket %d ",i);
        while(current != NULL){
            printf("--> %p ", current->item);
            if(current == NULL || current == current->next) break;
		    current = current->next;
        }
        printf("\n");
	}
}

int findIndexOfItem(List* list_pointer, void* item){ // finds index of item in the linked list
    Node* current = list_pointer->head;
    int count = 0;
    
    while(current != NULL){
        //printf("Index %d\n", count);
        if(current->item == item){
            return count;
        }
        //if(count == 20) break;
        if(current == NULL || current == current->next) return -1; // for some reason, it would set current = current->next and this catches that error
		current = current->next;
        count++;
	}
    return -1;
}