#ifndef DEQUE
#include "Arduino.h"
#include "Print.h"
#include "Gfx4d.h"
#include "pumpbadge.h"

#define DEQUE


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
