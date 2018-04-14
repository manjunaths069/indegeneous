
#ifndef __c_LINKLIST__
#define __c_LINKLIST__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* List Parameters */
#define LISTADDCURR   0x300  /* Add New Node At Current Record In List */
#define LISTADDHEAD   0x100  /* Add New Nodes At Head Of List */
#define LISTADDTAIL   0x200  /* Add New Nodes At Tail Of List */
#define LISTADDSPLAY  0x400  /* Add New Nodes As A Splay Tree */
#define LISTDELCURR   0x030  /* Delete Nodes At Current Record */
#define LISTDELHEAD   0x010  /* Delete Nodes At Head Of List */
#define LISTDELTAIL   0x020  /* Delete Nodes At Tail Of List */
#define LISTDELSPLAY  0x040  /* Delete Nodes As A Splay Tree */
#define LISTREADCURR  0x003  /* Read List At Current Node */
#define LISTREADHEAD  0x001  /* Read Head Of List */
#define LISTREADTAIL  0x002  /* Read Tail Of List */

#define LISTDELREAD   0x1000 /* Delete Node On Reading */
#define LISTCIRCULAR  0x2000 /* Circular List - Head->Next=Tail, etc */
#define LISTBTREE     0x4000 /* List is actually a binary tree */

/* Masks of List Parameters */
#define LISTADDMASK  0xF00  /* Add New Node Method */
#define LISTDELMASK  0x0F0  /* Delete Node Method */
#define LISTREADMASK 0x00F  /* Read Node Method */
#define LISTFLAGMASK 0xF000 /* Operation Flags */

/* Common Data Structure Types */
#define LIST (LISTADDCURR | LISTREADCURR | LISTDELCURR)
#define FIFO (LISTADDTAIL | LISTREADHEAD | LISTDELHEAD)
#define LIFO (LISTADDHEAD | LISTREADHEAD | LISTDELHEAD)
#define QUEUE (FIFO | LISTDELREAD)
#define STACK (LIFO | LISTDELREAD)
#define CIRCULAR_QUEUE (QUEUE | LISTCIRCULAR)
#define STREE (LISTBTREE | LISTADDSPLAY | LISTDELSPLAY | LISTREADCURR)

/* --------
   Possible return values of functions in this library
-------- */
#define LLIST_NOERROR 0
#define LLIST_NULL    1    /* Bad value passed to function */
#define LLIST_ERROR  -1    /* Misc. program/library error.  Serious trouble! */

#define LLIST_OK LLIST_NOERROR   /* duplicate definitions for compatibility */
#define LLIST_BADVALUE LLIST_NULL

/* --------
   List data structures
-------- */


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ListFreeFunc)(void *);
typedef void *(* ListAlloc)(size_t size);

#ifdef __cplusplus
}
#endif


typedef int (* NodeCompareFunc)(void *, void *);


typedef struct ListNode* listnodePtr;
typedef struct ListNode
{
  void        *Data;  /* Data stored at this node (user-defined) */
  listnodePtr Next,   /* Next Node in List, Right Child in Binary Tree */
              Prev;   /* Previous Node in List, Left Child in Binary Tree */
} listnode;

typedef struct LList* listPtr;
typedef struct LList
{
  listnodePtr  Current,  /* Last Accessed Node */
               Head,     /* Head of List, Root of Binary Tree */
               Tail;     /* Tail of List */
  int          Size,     /* Number of nodes in List or Binary Tree */
               Flags;    /* Flags associated with List/Tree */
  ListAlloc    memalloc; /* malloc()-type procedure to use */
  ListFreeFunc memfree;  /* free()-type procedure to use */
  NodeCompareFunc compare; /* Function to use to compare nodes */
} llist;



#ifndef DMALLOC
#define DMALLOC malloc
#endif
#ifndef DFREE
#define DFREE dfree
#endif
#ifndef DCOUNT
#define DCOUNT dcount
#endif

/* --------
  Function Prototypes
-------- */

listPtr NewListAlloc(int ListType, ListAlloc Lalloc, ListFreeFunc Lfree,
		     NodeCompareFunc Cfunc);

#define NewList(Type) NewListAlloc(Type, NULL, NULL, NULL)


listnodePtr NewListNode(listPtr List, void *Data);


#define NewNode(Data) NewListNode(NULL, Data)


void *GetNode(listPtr List);


void *FindNode(listPtr List, void *Data);


void *BTFind(listPtr List, void *Data);


void *GetNodeData(listnodePtr Node);


int AddNode(listPtr List, listnodePtr Node);


int InsertList(listPtr List, listnodePtr Node);
int HeadList(listPtr List, listnodePtr Node);
int TailList(listPtr List, listnodePtr Node);
int SplayInsertList(listPtr List, listnodePtr Node);


int DelNode(listPtr List);
int SplayRemoveList(listPtr List);

int RemoveList(listPtr List);
int DelHeadList(listPtr List);
int DelTailList(listPtr List);

void *NextNode(listPtr List);

void *PrevNode(listPtr List);


void *IndexNode(listPtr List, int Index);


int FreeList(listPtr List, ListFreeFunc DataFree);


void SwapList(listPtr List);

void SortList(listPtr List);


void *SplayList(listPtr List, void *Data);


int IntCompare(int *First, int* Second);
int StringCompare(char *First, char* Second);
int DoubleCompare(double *First, double* Second);

#ifdef LINKLIST_TEST

int PrintList(listPtr List, char *DataFmt);


int PrintTree(listPtr List, char *DataFmt);

#endif

#endif  /* __c_LINKLIST__ */





