
/*
void printQueue(my402queue *queue, int type)
{
	void *data;
	my402queueElem *element = queue->front;
	data = element->data;
	if(element == NULL)
	{
		printf("\nQueue is empty!!\n");
		exit(1);
	}
	else
	{
		while(element->next!=NULL)
		{
			switch(type)
			{
				case 1:  //packet queue
				{
					printf("pno.:%lu\t",(Packet *)data->pNumber);
					break;
				}
				case 2:  //Token queue
				{
					printf("tno.:%lu\t",(Token *)data->tnumber);
					break;
				}
			}
		}
	}
}

*/
