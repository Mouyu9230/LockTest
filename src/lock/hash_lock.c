#include "hash_lock.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void hashInit(hash_lock_t* bucket) {

  for(int i=0;i<HASHNUM;i++) {
        bucket->table[i].head=NULL;
        pthread_mutex_init(&bucket->table[i].mutex, NULL);
    }
}

int getValue(hash_lock_t* bucket, int key) {

  int posi=HASH(key);
  pthread_mutex_lock(&bucket->table[posi].mutex);
  Hlist pnode=bucket->table[posi].head;
  
  while(pnode!=NULL){
    if(pnode->key==key){
  pthread_mutex_unlock(&bucket->table[posi].mutex);
      return pnode->value;
    }else{
      pnode=pnode->next;
    }
  }
  pthread_mutex_unlock(&bucket->table[posi].mutex);
  return -1;
}

void insert(hash_lock_t* bucket, int key, int value) {
    int posi=HASH(key);

   pthread_mutex_lock(&bucket->table[posi].mutex);
    Hlist pnode=bucket->table[posi].head;
    Hlist prev=NULL;
  while(pnode!=NULL){
    if(pnode->key==key){
      pnode->value=value;
   pthread_mutex_unlock(&bucket->table[posi].mutex);   
      return;
    }else{
      prev=pnode;
      pnode=pnode->next;
      
    }
  }
  Hlist newnode=(Hlist)malloc(sizeof(*newnode));
  newnode->value=value;
  newnode->key=key;
  newnode->next=NULL;
  if(prev==NULL){
    bucket->table[posi].head = newnode;
}else{
    prev->next=newnode;
}
  pthread_mutex_unlock(&bucket->table[posi].mutex);   
}

int setKey(hash_lock_t* bucket, int key, int new_key) {

    int find=0;
    int posi=HASH(key);
    int newposi=HASH(new_key);

    int first_lock = (posi < newposi) ? posi : newposi;
    int second_lock = (posi < newposi) ? newposi : posi;

    pthread_mutex_lock(&bucket->table[first_lock].mutex);
    if (first_lock != second_lock) {
        pthread_mutex_lock(&bucket->table[second_lock].mutex);
    }

    //临界区------------
    Hlist pnode=bucket->table[posi].head;
    Hlist prev=NULL;

    while (pnode != NULL) {
        if (pnode->key == key) {
            if (prev == NULL) {
                bucket->table[posi].head = pnode->next;
            } else {
                prev->next = pnode->next;
            }
            find = 1;
            break;
        }
        prev = pnode;
        pnode = pnode->next;
    }

    if(find){
        pnode->key=new_key;
        pnode->next=bucket->table[newposi].head;
        bucket->table[newposi].head=pnode;
    }
    //------------

    if (first_lock!=second_lock) {
        pthread_mutex_unlock(&bucket->table[second_lock].mutex);
    }
    pthread_mutex_unlock(&bucket->table[first_lock].mutex);

    return find ? 0 : -1;
}