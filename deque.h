#ifndef DEQUE_H
#define DEQUE_H

#include "pumpbadge.h"
#include "Arduino.h"

struct node {
  uint8_t blocks[LINE_WIDTH];
  struct node *next;
  struct node *prev;      
};

class Deque {
  public:
    Deque();
    void insertTail(node *in);
    void insertHead(node *in);
    node *deleteTail();
    node *deleteHead();
    node *head;
    node *tail;
    bool isHead(node *in);
    bool isTail(node *in);
    void print();    
};




#endif
