#include<stdio.h>
#include<stdlib.h>	
#include<pthread.h>
#include"my402queue.h"

int initQueue(my402queue *anchor, int size)
{
	if(anchor == NULL)
		return FAILURE;
	anchor->num_members = 0;
	anchor->size = size;
	anchor->front = NULL;
	anchor->rear = NULL;
	return SUCCESS;
}

void *allocate(int type)
{
	switch(type)
	{
		case 1:
		{
			return (malloc(sizeof(my402queueElem)));
			break;
		}
		case 2:
		{
			return (malloc(sizeof(my402queue)));
			break;
		}
		case 3:
		{
			return (malloc(sizeof(pthread_t)));
			break;
		}
	}
	return NULL;
}

int isEmpty(my402queue *anchor)
{
	if(anchor->num_members == 0)
		return TRUE;
	else
		return FALSE;
}

int isFull(my402queue *anchor)
{
	if(anchor->num_members == anchor->size)
		return TRUE;
	else 
		return FALSE;
}

my402queueElem *enqueue(my402queue *anchor, void *data) //push new Element to rear
{
	my402queueElem *element;
	my402queueElem *newElement;
	if(isFull(anchor))
		return NULL;
	newElement = (my402queueElem *)allocate(ELEMENT);
	if(newElement == NULL)
		return NULL;
	newElement->data = data;
	if(anchor->rear == NULL)
		insertFirstElem(anchor, newElement);
	else
	{
		element = anchor->rear;
		element->next = newElement;
		newElement->next = NULL;
		anchor->rear = newElement;
		anchor->num_members++;
	}
	return newElement;

}

int insertElment(my402queue *anchor, my402queueElem *newElement)
{
	my402queueElem *element;
	if(isFull(anchor))
		return FAILURE;
	if(isEmpty(anchor))
		insertFirstElem(anchor, newElement);
	else
	{
		element = anchor->rear;
		element->next = newElement;
		newElement->next = NULL;
		anchor->rear = newElement;
		anchor->num_members++;
	}
	return SUCCESS;
}

int dequeue(my402queue *anchor)
{
	my402queueElem *element = anchor->front;
	if(isEmpty(anchor))
		return EMPTYQUEUE;
	if(anchor->num_members==1)
		removeLastElem(anchor);
	else
	{
		anchor->front = element->next;
		free(element);
		anchor->num_members--;
	}
	return SUCCESS;

}

void removeElements(my402queue *anchor, int number)
{
	int i=0;
	my402queueElem *element;
	for(i=0;i<number;i++)
	{
		element = anchor->front;
		if(isEmpty(anchor))
			break;
		if(anchor->num_members==1)
		{
			free(element->data);
			element->data = NULL;
			free(anchor->front);
			anchor->front = anchor->rear = NULL;
			anchor->num_members--;
		}
		else
		{
			anchor->front = element->next;
			free(element->data);
			element->data = NULL;
			free(element);
			anchor->num_members--;
		}
	}

}

void insertFirstElem(my402queue *anchor, my402queueElem *newElement)
{
	anchor->rear = anchor->front = newElement;
	newElement->next = NULL;
	anchor->num_members++;
}

void removeLastElem(my402queue *anchor)
{
	free(anchor->front);
	anchor->front = anchor->rear = NULL;
	anchor->num_members--;
}

my402queueElem *getFrontElem(my402queue *anchor)
{
	if(isEmpty(anchor))
		return NULL;
	else
		return anchor->front;
}

my402queueElem *getRearElem(my402queue *anchor)
{
	if(isEmpty(anchor))
		return NULL;
	else
		return anchor->rear;
}
