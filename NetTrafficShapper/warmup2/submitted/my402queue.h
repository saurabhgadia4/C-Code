#define INFINITE_SIZE -1
#define SUCCESS 1
#define FAILURE 0
#define ELEMENT 1
#define QUEUEANCHOR 2
#define PTHREAD 3
#define TRUE 1
#define FALSE 0
#define EMPTYQUEUE 1
#define FULLQUEUE 2

typedef struct tagMy402queueElem{
	void *data;
	struct tagMy402queueElem *next;
}my402queueElem;


typedef struct tagMy402queue
{
	int num_members;
	int size;
	my402queueElem *front;
	my402queueElem *rear;
}my402queue;

/*--*/extern void *allocate(int);
/*--*/extern int initQueue(my402queue *, int);
/*--*/extern int isEmpty(my402queue *);
/*--*/extern int isFull(my402queue *);
/*--*/extern my402queueElem *enqueue(my402queue *, void *);
/*--*/extern int dequeue(my402queue *);
/*--*/extern my402queueElem *getFrontElem(my402queue *);
/*--*/extern my402queueElem *getRearElem(my402queue *);
/*--*/extern void insertFirstElem(my402queue *, my402queueElem *);
/*--*/extern void removeLastElem(my402queue *);
/*--*/extern int insertElment(my402queue *, my402queueElem *);
/*--*/extern void removeElements(my402queue *, int);
