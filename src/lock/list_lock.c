#include "list_lock.h"

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>




void listInit(list_lock_t* list)
{

    list->head=NULL;

    pthread_mutex_init(&list->mutex, NULL);
    pthread_cond_init(&list->cond, NULL);
}

void producer(list_lock_t* list, DataType value) {
  
    LinkList node=malloc(sizeof(*node));
    node->value=value;
    node->next=NULL;

    pthread_mutex_lock(&list->mutex);

    if(list->head==NULL){
      list->head=node;
    }else{
      LinkList pnode=list->head;
      while(pnode->next!=NULL){
        pnode=pnode->next;
      }
      pnode->next=node;
    }

    pthread_mutex_unlock(&list->mutex);
    pthread_cond_signal(&list->cond);
  
}


void consumer(list_lock_t* list){

    
    pthread_mutex_lock(&list->mutex);

    while(list->head==NULL){
      pthread_cond_wait(&list->cond,&list->mutex);
    }

    LinkList pnode=list->head;

    if(pnode->next==NULL){
      list->head=NULL;
    }else{    
      LinkList savenode;
    while(pnode!=NULL&&pnode->next!=NULL){
      savenode=pnode;
      pnode=pnode->next;
    }

    savenode->next=NULL;

    }
    free(pnode);
    pthread_mutex_unlock(&list->mutex);
    

}

int getListSize(list_lock_t* list) {
  pthread_mutex_lock(&list->mutex);

  int size=0;
  LinkList pnode=list->head;

  while(pnode!=NULL){
    size++;
    pnode=pnode->next;
  }

  pthread_mutex_unlock(&list->mutex);
  return size;
}