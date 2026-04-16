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
  while(1){
    pthread_mutex_lock(&list->mutex);
    LinkList pnode=list->head;
    while(pnode->next!=NULL){
      pnode=pnode->next;
    }
    LinkList headd=(LinkList)malloc(sizeof(LinkList));
    pnode->next=headd;
    pnode->next->value=value;
    pthread_mutex_unlock(&list->mutex);    
    pthread_cond_signal(&list->cond);
  }

}

void consumer(list_lock_t* list){

    LinkList pnode=list->head;
    LinkList savenode;
    while(pnode->next!=NULL){
      savenode=pnode;
      pnode=pnode->next;
    }
    while(1){
    pthread_mutex_lock(&list->mutex);

    while(getListSize((list))==0){
      pthread_cond_wait(&list->cond,&list->mutex);
    }

    savenode->next=NULL;
    free(pnode);
    pthread_mutex_unlock(&list->mutex);
    }
    return;

}

int getListSize(list_lock_t* list) {
  pthread_mutex_lock(&list->mutex);
  long long int listsize=0;
    if(list->head==NULL){
      return 0;
    }
  LinkList pnode=list->head;
  while(1){
   if(pnode->next==NULL){
    pthread_mutex_unlock(&list->mutex);
    return listsize;
   }
   pnode=pnode->next;
   listsize++;

  }

}