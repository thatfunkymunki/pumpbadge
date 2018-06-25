#include "deque.h"

Deque::Deque(){

  head = NULL;
  tail = NULL;
  for(int i=0; i < QUEUE_LENGTH; i++){
    node *temp = (node *) malloc(sizeof(node));
    temp->prev = NULL;
    temp->next = NULL;
    //initialize the list with just black blocks
    memset(temp->blocks, BG_COLOR, LINE_WIDTH);
    insertTail(temp);
  }
}
void Deque::insertHead(node *in){
  if(in != NULL){
    if(head==NULL){
      head = in;
      tail = in;
    }
    else{
      in->next = head;
      head->prev = in;
      head = in;
    }
  }
}
void Deque::insertTail(node *in){
  if(in != NULL){
    if(tail==NULL){
      head = in;
      tail = in;
    }
    else{
      in->prev = tail;
      tail->next = in;
      tail = in;
    }
  }
}
node *Deque::deleteHead(){
  if(head == NULL){
    return NULL;
  }
  else{
    node *temp = head;
    head = head->next;
    if(head==NULL){
      tail=NULL;
    }
    else{
      head->prev = NULL;
    }
    return temp;
  }
}
node *Deque::deleteTail(){
  if(tail==NULL){
    return NULL;
  }
  else{
    node *temp = tail;
    tail = tail->prev;
    if(tail==NULL){
      head=NULL;
    }
    else{
      tail->next = NULL;
    }
    return temp;
  }
}

bool Deque::isHead(node *in){
  return in != NULL && in->prev == NULL ;
}
bool Deque::isTail(node *in){
  return in != NULL && in->next == NULL;
}

void Deque::print(){
  node *temp = head;
  while(temp!=NULL){
    //print node contents
    for(int i=0;i<LINE_WIDTH;i++){
      Serial.print(temp->blocks[i],HEX);Serial.print(" ");
    }
    delay(5);
    Serial.println();
    temp = temp->next;
  }
  
}

