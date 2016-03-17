#include<stdio.h>
#include<stdlib.h>
#include"cs402.h"
#include"my402list.h"
#include"my402sort.h"
#include<stdbool.h>
#define ALLOCATE_ELEMENT 1
#define ALLOCATE_OBJECT 2

void InsertFirstElement(My402List *, My402ListElem *, void *);
void FreeElement(My402ListElem *);
void *Allocate();
void My402ElmentInit(My402ListElem *);
void UnlinkElem(My402List *, My402ListElem *);
My402ListElem *getNewElement();

int My402ListInit(My402List *Anchor_holder)
{
    if(Anchor_holder == NULL)
        return 0;
    Anchor_holder->num_members = 0;
    Anchor_holder->anchor.next = &(Anchor_holder->anchor);
    Anchor_holder->anchor.prev = &(Anchor_holder->anchor);
    Anchor_holder->anchor.obj = NULL;
    Anchor_holder->Length = My402ListLength;
    Anchor_holder->Empty = My402ListEmpty;
    Anchor_holder->Append = My402ListAppend;
    Anchor_holder->Prepend = My402ListPrepend;
    Anchor_holder->Unlink = My402ListUnlink;
    Anchor_holder->UnlinkAll = My402ListUnlinkAll;
    Anchor_holder->InsertBefore = My402ListInsertBefore;
    Anchor_holder->InsertAfter = My402ListInsertAfter;
    Anchor_holder->First = My402ListFirst;
    Anchor_holder->Last = My402ListLast;
    Anchor_holder->Next = My402ListNext;
    Anchor_holder->Prev = My402ListPrev;
    Anchor_holder->Find = My402ListFind;
    
    return 1;
}


void *Allocate(int type)
{
    //type 1-List Element, 2- Element Object
    switch(type)
    {
        case ALLOCATE_ELEMENT:
            return(malloc(sizeof(My402ListElem)));
        case ALLOCATE_OBJECT:
            return(malloc(sizeof(My402Object)));
        default: 
            return NULL;
    }
    
}

void FreeElement(My402ListElem *Element)
{
    //first free the object
    free(Element);
}

int My402ListLength(My402List *Anchor_holder)
{
    return Anchor_holder->num_members;
}

int My402ListEmpty(My402List *Anchor_holder)
{
    bool condition = ((Anchor_holder->anchor.next == (&(Anchor_holder->anchor))) && ((Anchor_holder->anchor.prev) == (&(Anchor_holder->anchor))));
    if(Anchor_holder->num_members == 0 && condition)
        return TRUE;
    else 
        return FALSE;
}

void InsertFirstElement(My402List *Anchor_holder, My402ListElem *Element, void *data)
{
    Element->next = Element->prev = &(Anchor_holder->anchor);
    Anchor_holder->anchor.next = Anchor_holder->anchor.prev = Element;
    Element->obj = data;
}

void My402ElmentInit(My402ListElem *newElement)
{
    newElement->next = NULL;
    newElement->prev = NULL;
    newElement->obj = NULL;
}

My402ListElem *getNewElement()
{
    My402ListElem *newElement = (My402ListElem *)Allocate(ALLOCATE_ELEMENT);
    My402ElmentInit(newElement);
    return newElement;
}

int My402ListAppend(My402List *Anchor_holder, void *data)
{
    //If list is empty, just add obj to the list. Otherwise, add obj after Last(). 
    //This function returns TRUE if the operation is performed successfully and returns FALSE otherwise.
    My402ListElem *newElement = getNewElement();
    if(newElement == NULL)
    {
        printf("\nCannot allocate memory of requested element object");
        printf("--->>Add Element Details later on while confirming object structure");
        return FALSE;
    }
    
    if(My402ListEmpty(Anchor_holder)){
        //create first element in list.
        InsertFirstElement(Anchor_holder, newElement, data);
        Anchor_holder->num_members +=1;
    }
    else
    {
        My402ListElem *Last =  My402ListLast(Anchor_holder);
        if(Last == NULL)
        {
            FreeElement(newElement);
            return FALSE;
        }
        newElement->next = &(Anchor_holder->anchor);
        Anchor_holder->anchor.prev = newElement;
        Last->next = newElement;
        newElement->prev = Last;
        newElement->obj = data;
        Anchor_holder->num_members +=1;
    }
    return TRUE;
}

int  My402ListPrepend(My402List *Anchor_holder, void *data)
{
    //If list is empty, just add obj to the list. Otherwise, add obj before First(). 
    //This function returns TRUE if the operation is performed successfully and returns FALSE otherwise
    My402ListElem *newElement = getNewElement();;
    if(newElement == NULL)
    {
        printf("\nCannot allocate memory of requested element object");
        printf("--->>Add Element Details later on while confirming object structure");
        return FALSE;
    }
    if(My402ListEmpty(Anchor_holder)){
        //create first element in list.
        InsertFirstElement(Anchor_holder, newElement, data);
        Anchor_holder->num_members +=1;
    }
    else
    {
        My402ListElem *First = My402ListFirst(Anchor_holder);
        if(First == NULL)
        {
            FreeElement(newElement);
            return FALSE;
        }
        newElement->next = First;
        newElement->prev = &(Anchor_holder->anchor);
        newElement->obj = data;
        First->prev = newElement;
        Anchor_holder->anchor.next = newElement;
        Anchor_holder->num_members +=1;
    }


    return TRUE;
}
My402ListElem *My402ListLast(My402List *Anchor_holder)
{
    if(My402ListEmpty(Anchor_holder))
    {
        return NULL;
    }
    else
        return Anchor_holder->anchor.prev;
}

My402ListElem *My402ListFirst(My402List *Anchor_holder)
{
    if(My402ListEmpty(Anchor_holder))
    {
        return NULL;
    }
    else
        return Anchor_holder->anchor.next;
}

My402ListElem *My402ListNext(My402List *Anchor_holder, My402ListElem *Element)
{
    //Returns elem->next or NULL if elem is the last item on the list. 
    //Please do not check if elem is on the list.
    if(Element->next == (&(Anchor_holder->anchor)))
        return NULL;
    else
        return Element->next;
}

My402ListElem *My402ListPrev(My402List *Anchor_holder, My402ListElem *Element)
{
    //Returns elem->prev or NULL if elem is the first item on the list.
    //Please do not check if elem is on the list. 
    if(Element->prev == (&(Anchor_holder->anchor)))
        return NULL;
    else
        return Element->prev;
}

//check one more time
My402ListElem *My402ListFind(My402List *Anchor_holder, void *obj)
{
    //Returns the list element elem such that elem->obj == obj. 
    //Returns NULL if no such element can be found
    My402ListElem *Current = My402ListFirst(Anchor_holder);
    if(Current == NULL)
        return NULL;
    else
    {
        while((Current->obj != obj) && (Current!=(&(Anchor_holder->anchor))))
            Current = Current->next;
        if(Current->obj == obj)
            return Current;
        else 
            return NULL;
    }
}

void My402ListUnlink(My402List *Anchor_holder, My402ListElem *Element)
{
    //Unlink and delete elem from the list. 
    //Please do not delete the object pointed to by elem and do not check if elem is on the list. 
    /*My402ListElem *Current = My402ListFirst(Anchor_holder);
    while(Current->next != (&(Anchor_holder->anchor)) && Current != Element)
        Current = Current->next;*/
    
    UnlinkElem(Anchor_holder, Element);
    
}

void My402ListUnlinkAll(My402List *Anchor_holder)
{
    My402ListElem *Current, *Next;
    Current = Anchor_holder->anchor.next;
    if(Anchor_holder->num_members != 0){
        while(Current != &(Anchor_holder->anchor))
        {
            Next = Current->next;
            UnlinkElem(Anchor_holder, Current);
            Current = Next;
        }

    }
    
}

void UnlinkElem(My402List *Anchor_holder, My402ListElem *Element)
{
    (Element->next)->prev = Element->prev;
    (Element->prev)->next = Element->next;
    FreeElement(Element);
    Anchor_holder->num_members -=1;
}

int My402ListInsertAfter(My402List *Anchor_holder, void *data, My402ListElem *Element)
{
    int retcode = FALSE;
    if(My402ListEmpty(Anchor_holder) || (Element == NULL))
        retcode = My402ListAppend(Anchor_holder, data);
    else
    {
        My402ListElem *newElement = getNewElement();
        newElement->next = Element->next;
        newElement->prev = Element;
        newElement->obj = data;
        (Element->next)->prev = newElement;
        Element->next = newElement;
        Anchor_holder->num_members +=1;
        retcode = TRUE;
    }
    return retcode;
}

int  My402ListInsertBefore(My402List *Anchor_holder, void *data, My402ListElem *Element)
{
    int retcode = FALSE;
    if(My402ListEmpty(Anchor_holder) || (Element == NULL))
        retcode = My402ListPrepend(Anchor_holder, data);
    else
    {
        My402ListElem *newElement = getNewElement();
        newElement->next = Element;
        newElement->prev = Element->prev;
        newElement->obj = data;
        (Element->prev)->next = newElement;
        Element->prev = newElement;
        Anchor_holder->num_members +=1;
        retcode = TRUE;
    }
    return retcode;
}
