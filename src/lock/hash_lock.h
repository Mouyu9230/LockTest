#include <pthread.h>

#define HASHNUM 13
#define HASH(key) key % HASHNUM

typedef struct HashNode {
  int value;
  int key;
  struct HashNode* next;
}Hnode,*Hlist;//成员节点结构

struct HashTable {
  pthread_mutex_t mutex;
  Hlist head;
};//成员

typedef struct{
  struct HashTable table[HASHNUM];
}hash_lock_t;//总

void hashInit(hash_lock_t* bucket);
int getValue(hash_lock_t* bucket, int key);
void insert(hash_lock_t* bucket, int key,int value);
int setKey(hash_lock_t* bucket,int key, int new_key);