

#include "linklist.h"

/*Linked List Libarary*/

/* NewListAlloc() */
listPtr NewListAlloc(int ListType, ListAlloc Lalloc, ListFreeFunc Lfree,
		     NodeCompareFunc Cfunc)
{
  listPtr List;

  if (Lalloc == NULL) Lalloc = malloc;
  if (Lfree == NULL)  Lfree = free;

  if ((List = (listPtr)((Lalloc)(sizeof(struct LList)))) != NULL)
    {
      List->Current  = List->Head = List->Tail = NULL;
      List->memalloc = Lalloc;
      List->memfree  = Lfree;
      List->compare  = Cfunc;
      List->Size     = 0;
      List->Flags    = ListType;
    }

  return List;
}

/* NewListNode() */
listnodePtr NewListNode(listPtr List, void *Data)
{
  listnodePtr Node;
  ListAlloc  Alloc;

  if (List == NULL)
    Alloc = malloc;
  else
    Alloc = List->memalloc;

  if ((Node = (listnodePtr)((Alloc(sizeof(struct ListNode))))) != NULL)
    {
       Node->Data = Data;
       Node->Next = Node->Prev = NULL;
    }

  return Node;
}

/* GetNode() */
void *GetNode(listPtr List)
{
  void *Data;
  int  Delete;

  if (List == NULL) return NULL;

  Data   = NULL;
  Delete = ((List->Flags & LISTFLAGMASK) & LISTDELREAD);

  switch (List->Flags & LISTREADMASK)
    {
       case LISTREADHEAD: if (List->Head != NULL) {
	                    Data = List->Head->Data;
			    if (Delete) DelHeadList(List);
                          }
                          break;
       case LISTREADTAIL: if (List->Tail != NULL) {
                            Data = List->Tail->Data;
                            if (Delete) DelTailList(List);
                          }
                          break;
       default:
       case LISTREADCURR: if (List->Current != NULL) {
                            Data = List->Current->Data;
	                    if (Delete) RemoveList(List);
                          }
                          break;
    }

  return Data;
}

/* GetNodeData() */
void *GetNodeData(listnodePtr Node)
{
  if (Node != NULL)
    return Node->Data;
   else
    return NULL;
}

/* FindNode() */
void *FindNode(listPtr List, void *Data)
{
  int Compare;

  if ((List == NULL) || (List->compare == NULL))
    return NULL;

  //printf("\nlist and compare not null...");
  if ((List->Flags & LISTADDMASK) & LISTADDSPLAY)
    { /* List is a splay tree; do weirdness... */
      if (SplayList(List, Data) != NULL)
	Compare = 0;
    }
  if ((List->Flags & LISTFLAGMASK) & LISTBTREE)
    { /* List is some other binary tree, do a binary tree search... */
      if (BTFind(List, Data) != NULL)
	Compare = 0;
    }
  else
    { /* List is a normal list, not a tree, step through it... */
      List->Current = List->Head;
      //printf("\nSetting current to head...");
      if (List->Current == NULL)
	return NULL;
      //For debugging...
      //printf("\nStart compare...");
      //if(List->Current->Data == NULL) printf("\ndata null...");
      while ((Compare = (List->compare)(List->Current->Data, Data)) != 0){
          //printf("\ninside while...");
          if(List->Current->Next != NULL){
              List->Current = List->Current->Next;
          }else{
              printf("\nCurrent = NULL.");
              return NULL;
          }
      }
    }

  if (Compare != 0)
    return NULL;
  else
    return List->Current->Data;

}

/* BTFind() */
void *BTFind(listPtr List, void *Data)
{
  //listnodePtr Par, Grand;
  int Compare;

  if ((List == NULL) || (List->compare == NULL))
    return NULL;

  Compare = 1;
  while ((Compare != 0) && (List->Current != NULL))
    {
      if ((Compare = (List->compare)(List->Current->Data, Data) != 0))
	{
	  if (Compare < 0)
	    List->Current = List->Current->Next;
	  else
	    List->Current = List->Current->Prev;
	}
    }

  if (List->Current != NULL)
    return List->Current->Data;
  else
    return NULL;
}


/* AddNode() */
int AddNode(listPtr List, listnodePtr Node)
{
  if (List == NULL) return LLIST_NULL;

  switch (List->Flags & LISTADDMASK)
    {
       case LISTADDCURR: return InsertList(List, Node); break;
       case LISTADDHEAD: return HeadList(List, Node); break;
       case LISTADDTAIL: return TailList(List, Node); break;
       case LISTADDSPLAY: return SplayInsertList(List, Node); break;
     default: return InsertList(List, Node); break;
    }

  return LLIST_ERROR;
}

/* SplayInsertList() */
int SplayInsertList(listPtr List, listnodePtr Node)
{
  if (List == NULL) return LLIST_NULL;
  if (Node == NULL) return LLIST_NULL;

  if (List->Head != NULL) /* empty tree? */
    {
      if (SplayList(List, Node->Data) != NULL)
	/* We found this element in the splay tree; this is bad.  Say so... */
	return LLIST_BADVALUE;

      if ((List->compare)(Node->Data, List->Head->Data) > 0)
	{
	  Node->Prev = List->Head;
	  if ((List->compare)(Node->Data, GetNodeData(List->Head->Next)) < 0)
	    {
	      Node->Next = List->Head->Next;
	      List->Head->Next = NULL;
	    }
	}
      else
	{
	  Node->Next = List->Head;
	  if ((List->compare)(Node->Data, GetNodeData(List->Head->Prev)) > 0)
	    {
	      Node->Prev = List->Head->Prev;
	      List->Head->Prev = NULL;
	    }
	}
    }

  List->Head = List->Current = Node;
  List->Size++;

  return LLIST_NOERROR;
}

/* InsertList() */
int InsertList(listPtr List, listnodePtr Node)
{
  if (List == NULL) return LLIST_NULL;
  if (Node == NULL) return LLIST_NULL;

  if (List->Current != NULL)
    {
      Node->Prev = List->Current;
      Node->Next = List->Current->Next;
      List->Current->Next = Node;

      if (Node->Next == NULL)
        List->Tail = Node;
      else
	Node->Next->Prev = Node;
    }
  List->Current = Node;

  if (++List->Size == 1) /* If List Was Empty... */
    {
      List->Head = Node;
      List->Tail = Node;
    }

  return LLIST_NOERROR;
}

/* HeadList() */
int HeadList(listPtr List, listnodePtr Node)
{
  if (List == NULL) return LLIST_NULL;
  if (Node == NULL) return LLIST_NULL;

  if (List->Head != NULL)
    {
      Node->Next = List->Head;
      List->Head->Prev = Node;
    }

  List->Head = Node;
  List->Current = Node;

  if (++List->Size == 1) /* If List Was Empty... */
    List->Tail = Node;

  return LLIST_NOERROR;
}

/* TailList() */
int TailList(listPtr List, listnodePtr Node)
{
  if (List == NULL) return LLIST_NULL;
  if (Node == NULL) return LLIST_NULL;

  if (List->Tail != NULL)
    {
      Node->Prev = List->Tail;
      List->Tail->Next = Node;
    }

  List->Tail = Node;
  List->Current = Node;

  if (++List->Size == 1) /* If List Was Empty... */
    List->Head = Node;

  return LLIST_NOERROR;
}


/* DelNode() */
int DelNode(listPtr List)
{
  if (List == NULL) return LLIST_NULL;

  switch (List->Flags & LISTDELMASK)
    {
       case LISTDELCURR: return RemoveList(List); break;
       case LISTDELHEAD: return DelHeadList(List); break;
       case LISTDELTAIL: return DelTailList(List); break;
       case LISTDELSPLAY: return SplayRemoveList(List); break;
      default: return RemoveList(List); break;
    }

  return LLIST_ERROR;
}

/* SplayRemoveList() */
int SplayRemoveList(listPtr List)
{
  listnodePtr DelNode;

  if (List == NULL) return LLIST_NULL;
  if (List->Head == NULL) return LLIST_NOERROR;

  /* Assumes List->Head (root of splay tree) is already selected to
     be deleted node */

  DelNode = List->Head;
  if (List->Head->Next == NULL)
    List->Head = List->Head->Prev;
  else
    {
      if (List->Head->Prev != NULL)
	{
	  SplayList(List, List->Head->Prev->Data);   /* Must succeed */
	  List->Head->Next = DelNode->Next;
	}
      else
	List->Head = List->Head->Next;
    }

  List->memfree(DelNode);
  List->Current = List->Head;

  List->Size--;
  return LLIST_NOERROR;
}

/* RemoveList() */
int RemoveList(listPtr List)
{
  listnodePtr Temp;

  if (List == NULL) return LLIST_NULL;

  if (List->Current == NULL)
    return LLIST_NOERROR;

  if (List->Current == List->Head)
    return DelHeadList(List);

  if (List->Current == List->Tail)
    return DelTailList(List);

  Temp = List->Current;

  Temp->Prev->Next = List->Current->Next;
  Temp->Next->Prev = List->Current->Prev;

  List->Current = List->Current->Next;

  List->memfree(Temp);

  List->Size--;

  return LLIST_NOERROR;
}

/* DelHeadList() */
int DelHeadList(listPtr List)
{
  listnodePtr Temp;

  if (List == NULL) return LLIST_NULL;

  if (List->Head == NULL)
    return LLIST_NOERROR;

  Temp = List->Head;

  if (Temp->Next != NULL)
    Temp->Next->Prev = NULL;
  List->Head = Temp->Next;
  List->Current = List->Head;

  List->memfree(Temp);

  List->Size--;

  return LLIST_NOERROR;
}

/* DelTailList() */
int DelTailList(listPtr List)
{
  listnodePtr Temp;

  if (List == NULL) return LLIST_NULL;

  if (List->Tail == NULL)
    return LLIST_NOERROR;

  Temp = List->Tail;

  if (Temp->Prev != NULL)
    Temp->Prev->Next = NULL;
  List->Tail = Temp->Prev;
  List->Current = List->Tail;

  List->memfree(Temp);

  List->Size--;

  return LLIST_NOERROR;
}

/* NextNode() */
void *NextNode(listPtr List)
{
  if (List == NULL)
    return NULL;

  if (List->Current != NULL)
    List->Current = List->Current->Next;

  if ((List->Flags & LISTFLAGMASK) & LISTCIRCULAR)
    if (List->Current == NULL)
      List->Current = List->Head;

  return GetNodeData(List->Current);
}

/* PrevNode() */
void *PrevNode(listPtr List)
{
  if (List == NULL)
    return NULL;

  if (List->Current != NULL)
    List->Current = List->Current->Prev;

  if ((List->Flags & LISTFLAGMASK) & LISTCIRCULAR)
    if (List->Current == NULL)
      List->Current = List->Tail;

  return GetNodeData(List->Current);
}

/* IndexNode() */
void *IndexNode(listPtr List, int Index)
{
  int Count;
  //void *Data;

  if ((List == NULL) ||
      ((List->Flags & LISTFLAGMASK) & LISTBTREE))
    return NULL;

  if (List->Size < Index)
    return NULL;

  List->Current = List->Head;
  for (Count = 1; Count < Index; Count++)
    List->Current = List->Current->Next;

  return GetNodeData(List->Current);
}

/* FreeList() */
int FreeList(listPtr List, ListFreeFunc DataFree)
{
  int Delete;
  int Count = 0;

  if (List == NULL) return -1;

  List->Current = List->Head;
  Delete = LLIST_NOERROR;
  while ((Delete == LLIST_NOERROR) && (List->Size > 0))
    {
      if ((DataFree != NULL)  && (List->Head->Data != NULL))
	DataFree(List->Head->Data);

      if ((List->Flags & LISTDELMASK) & LISTDELSPLAY)
	Delete = SplayRemoveList(List);
      else
	Delete = DelHeadList(List);
    }

  if (Delete == LLIST_NOERROR)
    List->memfree(List);

  return Delete;
}


/* SwapList() */
void SwapList(listPtr List)
{
  listnodePtr Temp1, Temp2;

  if ((List == NULL) || (List->Current == NULL) ||
      (List->Current->Next == NULL))
    return;

  Temp1 = List->Current;
  Temp2 = List->Current->Next;

  Temp1->Next = Temp2->Next;
  if (Temp2->Next != NULL)
    Temp2->Next->Prev = Temp1;
  else
    List->Tail = Temp1;

  Temp2->Prev = Temp1->Prev;
  if (Temp1->Prev != NULL)
    Temp1->Prev->Next = Temp2;
  else
    List->Head = Temp2;

  Temp1->Prev = Temp2;
  Temp2->Next = Temp1;

  return;
}


/* SortList() */
void SortList(listPtr List)
{
  //listnodePtr Temp1, Temp2;
  int Move;

  if ((List == NULL) || (List->compare == NULL) ||
      ((List->Flags & LISTFLAGMASK) & LISTBTREE))
    return;

  /* This is a DA-sort!! */

  if (List->Head == NULL)
    return;

  Move = 1;
  while (Move != 0)
    {
      Move = 0;
      List->Current = List->Head;
      while (List->Current->Next != NULL)
	{
	  if ((List->compare)(List->Current->Data,
			      List->Current->Next->Data) > 0)
	    {
	      Move++;
	      SwapList(List);
	    };

	  if (List->Current->Next != NULL)
	    List->Current = List->Current->Next;
	}
    }

  List->Current = List->Head;
  return;
}

/* SplayList() */
void *SplayList(listPtr List, void *Data)
{
  listnodePtr Temp, Left, Right, LeftRoot, RightRoot;
  int Test;

  if ((List == NULL) || (List->compare == NULL))
    return NULL;

  /* Implements a top-down splay on the list; that is, it splays as it
     searches down the list for Data. (Basic algorithm from Sleator and
     Tarjan's "Self-Adjusting Binary Search Trees" paper of 1985) */

  List->Current = List->Head;
  if (List->Current == NULL)
    return NULL;
  Left = LeftRoot = Right = RightRoot = NULL;
  for (;;) {
    if ((Test = (List->compare)(Data, List->Current->Data)) < 0)
      {
	if (List->Current->Prev == NULL)
	  break;
	if ((List->compare)(Data, List->Current->Prev->Data) < 0)
	  {
	    Temp = List->Current->Prev;
	    List->Current->Prev = Temp->Next;
	    Temp->Next = List->Current;
	    List->Current = Temp;
	    if (List->Current->Prev == NULL)
	      break;
	  }
	if (Right == NULL)
	  Right = RightRoot = List->Current;
	else
	  {
	    Right->Prev = List->Current;
	    Right = List->Current;
	  }
	List->Current = List->Current->Prev;
      }
    else if (Test > 0)
      {
	if (List->Current->Next == NULL)
	  break;
	if ((List->compare)(Data, List->Current->Next->Data) > 0)
	  {
	    Temp = List->Current->Next;
	    List->Current->Next = Temp->Prev;
	    Temp->Prev = List->Current;
	    List->Current = Temp;
	    if (List->Current->Next == NULL)
	      break;
	  }
	if (Left == NULL)
	  Left = LeftRoot = List->Current;
	else
	  {
	    Left->Next = List->Current;
	    Left = List->Current;
	  }
	List->Current = List->Current->Next;
      }
    else
      break;
  }

  if (List->Current == NULL)
    return NULL;

  if (Left != NULL)
    Left->Next = List->Current->Prev;
  else
    LeftRoot = List->Current->Prev;

  if (Right != NULL)
    Right->Prev = List->Current->Next;
  else
    RightRoot = List->Current->Next;

  /* This assignment order is backwards from example code, but seems
     to work... (and makes more intuitive sense) */
  List->Current->Next = RightRoot;
  List->Current->Prev = LeftRoot;

  List->Head = List->Current;
  if ((List->compare)(Data, List->Head->Data) == 0)
    return List->Head;
  else
    return NULL;
}


/* StringCompare() */
int StringCompare(char *First, char* Second)
{
  /* printf("comparing \"%s\" to \"%s\"\n", First, Second); */
  if ((First != NULL) && (Second != NULL))
    return strcmp(First, Second);
  else
    return 0;
}


/* IntCompare() */
int IntCompare(int *First, int* Second)
{
  if ((First == NULL) || (Second == NULL))
    return 0;

  if (*First > *Second)
    return 1;

  if (*First < *Second)
    return -1;

  return 0;
}


/* DoubleCompare() */
int DoubleCompare(double *First, double* Second)
{
  if ((First == NULL) || (Second == NULL))
    return 0;

  if (*First > *Second)
    return 1;

  if (*First < *Second)
    return -1;

  return 0;
}

/*
    End of LinkList Library
*/












