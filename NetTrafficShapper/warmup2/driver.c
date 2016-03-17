#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<ctype.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
#include<sys/time.h>
#include<errno.h>
#include<math.h>
#include<signal.h>
#include"driver.h"
#include"my402list.h"
#include"cs402.h"

double rateInv, lambdaInv, muInv;
const char *arg[] = {"-lambda", "-mu", "-r", "-B", "-P", "-n", "-t"};
FILE *fp = NULL;
option param;
pthread_mutex_t mutex;
pthread_cond_t cv1;
sigset_t set;
int sigTriggered = 0;
int mode = DETERMINISTIC;
unsigned int startSeconds, startuSeconds;
Packet lastServed;
double totalEmultationTime = 0.0, siat, totalServiceTime=0.0, sq1t =0.0, sq2t =0.0, sumSystemTime = 0.0, lastGenPact0, emuStartTime = 0.0, emuEndTime = 0.0;
double sumSqrTiSys=0;
unsigned long int droppedToken=0, totalGenToken=0, droppedPacket=0, totalGenPacket=0, activeTokens=0, servicedPacketCount=0;
my402queue q1, q2, bucket;
int terminateServer=0, removedQ1Packets =0, removedQ2Packets =0;
unsigned long long int lineNumber=0;
void startEmulator()
{
	struct timeval t;
	double startTime = 0.0;
	if(gettimeofday(&t,NULL))
	{
		printf("\ncant get time of day!!");
		exit(1);
	}
	else
	{
		startSeconds = (unsigned int)t.tv_sec;
		startuSeconds = (unsigned int)t.tv_usec;
	}
	fprintf(stdout,"%012.3lfms: emulation begins\n",startTime);
}

double computeElapsedTime()
{
	struct timeval t;
	double value;
	unsigned int sec, usec;
	if(gettimeofday(&t,NULL))
	{
		printf("\ncant get time of day!!");
		exit(1);
	}
	else
	{
		sec = (unsigned int)t.tv_sec;
		usec = (unsigned int)t.tv_usec;
	}
	if(usec > startuSeconds)
	{
		usec = usec - startuSeconds;
		sec = sec - startSeconds;
	}
	else if(usec < startuSeconds)
	{
		usec = usec + 1000000 - startuSeconds;
		sec = sec - startSeconds - 1;

	}
	value = sec + ((double)usec/1000000.0);
	return value;

}

void error(char *msg)
{
	fprintf(stderr,"%s", msg);
    exit(1);
}

void calculateSleep(sleepFormat *time, double value)
{
	unsigned int data;
	data=value * 1000000.0; //convert into miroseconds
	time->usecs = data % 1000000;
	time->seconds = data/1000000;
}

void parseOption(int argc, char **argv)
{
	int i,j, match =0;
	setDefaultOptions();
	if(argc>1)
	{
		for(i=1;i<argc;i++)
		{
			if(*(*(argv+i)) == '-')
			{
				match = 0;
				for(j=0;j<7;j++)
				{
					if(!strcmp(arg[j],argv[i]))
					{
						if(argv[i+1] == '\0')
						{
							printf("Malformed Command.\nNo value is passed for option '%s'.\n",argv[i]);
							exit(1);
						}
						setOptionValues(j, argv[i+1]);
						match = 1;
						i++;
						break;
					}
				}
				if(!match)
				{
					printf("\nInvalid option '%s' passed.\n",argv[i]);
					error("Program expects options as: warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");
				}
			}
			else
			{
				printf("\nInvalid option '%s' passed.\n",argv[i]);
				error("Program expects options as: warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");
			}

		}
		if(mode == TRACEFILE)
		{
			fp = fopen(param.fpath,"r");
			if(fp == NULL)
			{
			    printf("Malformed Command.\n%s\n",strerror(errno));
			    exit(1);
			}
		}
	}
}

long long int myatoi(char *data, int option)
{
	long long int value=0;
	char *value1 = data;
	while(*data != '\0')
	{
		if(!isdigit(*data))
		{
			if(mode==TRACEFILE)
				printf("\nInput file is not in right format.\nInvalid value '%s' encountered on line:%llu\n", value1, lineNumber);
			else
				printf("\nInvalid value passed for option '%s'.\nExpected number but received '%s' as value.\n",arg[option], value1);
			exit(1);
		}
		value = value*10 + (*data - '0');
		data++;
	}
	return value;
}

double myatof(char *value, int option)
{
	int i=0;
	char *data = value;
	double digit=0.0, power;
	if(!isdigit(*value))
	{
		if(mode==TRACEFILE)
			printf("\nInput file is not in right format.\nInvalid value '%s' encountered on line:%llu\n", data, lineNumber);
		else
			printf("\nInvalid value passed for option '%s'.\nExpected number but received '%s' as value.\n",arg[option], data);
			exit(1);
	}
	for(i=0;isdigit(*(value+i));i++)
		digit = digit*10 + (*(value +i) - '0');
	if(*(value+i)=='\0')
		return digit;
	else if(*(value+i) == '.')
		power = 1.0;
	else
	{
		if(mode==TRACEFILE)
			printf("\nInput file is not in right format.\nInvalid value '%s' encountered on line:%llu\n", data, lineNumber);
		else
			printf("\nInvalid value passed for option '%s'.\nExpected number but received '%s' as value.\n",arg[option], data);
		exit(1);
	}
	i++;
	while(*(value +i)!='\0')
	{
		if(!isdigit(*(value +i)))
		{
			if(mode==TRACEFILE)
				printf("\nInput file is not in right format.\nInvalid value '%s' encountered on line:%llu\n", data, lineNumber);
			else
				printf("\nInvalid value passed for option '%s'.\nExpected number but received '%s' as value.\n",arg[option], data);
			exit(1);
		}
		power*=10;
		digit = digit*10 + (*(value +i) - '0');
		i++;
	}
	return (digit/power);
}		

void setOptionValues(int type, char *data)
{
	switch(type){
		case PARAM_LAMBDA:
		{
			param.lambda = myatof(data, PARAM_LAMBDA);
			break;
		}
		case PARAM_MU:
		{
			param.mu = myatof(data, PARAM_MU);
			break;
		}
		case PARAM_RATE:
		{
			param.rate = myatof(data, PARAM_RATE); 
			break;
		}
		case PARAM_B:
		{
			param.bucketSize = myatoi(data, PARAM_B);
			break;
		}
		case PARAM_P:
		{
			param.tokenReq = myatoi(data, PARAM_P);
			break;
		}
		case PARAM_N:
		{
			param.totalPackets = myatoi(data, PARAM_N);
			break;
		}
		case PARAM_T:
		{
			param.fpath = (char *)malloc(sizeof(char)*(strlen(data)+1));
			param.fpath = strcpy(param.fpath, data);
			*(param.fpath+strlen(data)) = '\0';
			mode = TRACEFILE;
			break;
		}

	}
}

void setDefaultOptions()
{
	param.lambda = 0.5;
	param.mu = 0.35;
	param.rate = 1.5;
	param.totalPackets = 20;
	param.bucketSize = 10;
	param.tokenReq = 3;
	param.fpath = NULL;
}

void printParams()
{
	char *token, *fname;
	printf("\nEmulation Parameters:\n");
	if(mode == TRACEFILE){
		
		token = strtok(param.fpath, "//");
		while(token!=NULL)
		{
			fname = token;
			token = strtok(NULL, "//");
		}
		printf("\tnumber to arrive = %lu\n\tr = %f\n\tB = %lu\n\ttsfile = %s\n\n", param.totalPackets, param.rate, param.bucketSize, fname);
	}
	else
		printf("\tnumber to arrive = %lu\n\tlambda = %f\n\tmu = %f\n\tr = %f\n\tB = %lu\n\tP = %lu\n\n",param.totalPackets, param.lambda, param.mu, param.rate, param.bucketSize, param.tokenReq);
}

void setInvRate()
{
	lambdaInv = 1.0/(param.lambda);
	rateInv = 1.0/(param.rate);
	muInv = 1.0/(param.mu);
	if(mode == DETERMINISTIC)
	{
		if(lambdaInv > 10.0)
			lambdaInv = 10.0;
		if(muInv > 10.0)
			muInv = 10.0;
	}
	if(rateInv>10.0)
		rateInv = 10.0;
}

void checkSpaces(char *ptr)
{
	int count=0;
	while(*ptr==' ' || *ptr=='\t')
	{
		ptr++;
		count++;
	}
	if(count>0)
	{
		printf("Invalid format occured on line %llu of tracefile",lineNumber);
		exit(1);
	}
}

char *calLength(char *tempBuf, int *length)
{
    while((*(tempBuf)) != '\t' && (*(tempBuf)) != ' ' &&(*(tempBuf))!= '\n'){
        tempBuf++;
        (*length)++;
    }
    return tempBuf;
}

int main(int argc, char *argv[])
{
	pthread_t *packetThread, *bucketThread, *serverThread, *signalThread;
	char buffer[1027];
	char *bufferCursor;
	char *tTotalPacket;
	unsigned long int tPacketCount = 0;
	int length=0;
	sleepFormat *packetTime, *tokenTime, *serverTime;
	packetTime = (sleepFormat *)malloc(sizeof(sleepFormat));
	tokenTime = (sleepFormat *)malloc(sizeof(sleepFormat));
	serverTime = (sleepFormat *)malloc(sizeof(sleepFormat));
	
	packetThread = (pthread_t *)allocate(PTHREAD);
	bucketThread = (pthread_t *)allocate(PTHREAD);
	serverThread = (pthread_t *)allocate(PTHREAD);
	signalThread = (pthread_t *)allocate(PTHREAD);
	//initialize mutex

	if(pthread_mutex_init(&mutex, NULL))
	{
		printf("\nCannot initialize mutex.");
		exit(1);
	}
	//initialize condition variable
	if(pthread_cond_init(&cv1, NULL))
	{
		printf("\nCannot initialize condition variable cv1\n");
		exit(1);
	}

	parseOption(argc, argv);
	setInvRate();

	//calculate 1/l, 1/r, 1/mu
	calculateSleep(packetTime, lambdaInv);
	calculateSleep(tokenTime, rateInv);
	calculateSleep(serverTime, muInv);
	
	// initialize queue q1,q2,server.

	if(!initQueue(&q1, INFINITE_SIZE))
		error("Cannot initialize packet queue Q1.\n");
	if(!initQueue(&q2, INFINITE_SIZE))
		error("Cannot initialize server queue Q2.\n");
	if(!initQueue(&bucket, param.bucketSize))
		error("Cannot initialize bucket queue.\n");
//...................initialize signal set..............................//

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, 0);

// now all the threads created by main thread will have these signal added and masked in them.
//so no thread will terminate or respond to SIGINT signal.
//...........................end .....................................
	if(mode == TRACEFILE)
	{
		memset(buffer, 0, sizeof(buffer));
		bufferCursor = fgets(buffer, 1026,fp);
		if(bufferCursor==NULL)
			error("Invalid format occured in tracefile.\n");
		lineNumber++;
		checkSpaces(bufferCursor);
		while(*bufferCursor!='\n')
		{
			bufferCursor++;
			length++;
		}
		tTotalPacket = (char *)malloc((length+1)*sizeof(char));
		tTotalPacket = strncpy(tTotalPacket, buffer, length);
		*(tTotalPacket+length) = '\0';
		tPacketCount = myatoi(tTotalPacket, PARAM_N);
		param.totalPackets = tPacketCount;
	}

//.........................................................................................
	
	printParams();
	startEmulator();
	//starting packet thread
	if(pthread_create(packetThread, NULL, processPacket, packetTime))
		error("cannot create packet thread.\n");
	if(pthread_create(bucketThread, NULL, processToken, tokenTime))
		error("cannot create token depositing thread.\n");
	if(pthread_create(serverThread, NULL, processServer, serverTime))
		error("cannot create server thread.\n");
	if(pthread_create(signalThread, NULL, monitorSignal, NULL))
		error("cannot create signal thread.\n");

	pthread_join(*packetThread, NULL);
	pthread_join(*bucketThread, NULL);
	pthread_join(*serverThread, NULL);
	if(!sigTriggered)
		pthread_cancel(*signalThread);
	else
		pthread_join(*signalThread, NULL);
	if(sigTriggered)
		recalculate();
	emuEndTime = computeElapsedTime();   //returned in seconds. 2.4343secs
	fprintf(stdout,"%012.3lfms: emulation ends\n",(emuEndTime * 1000.0));
	printStats();
	pthread_mutex_destroy(&mutex);
	free(packetTime);
	free(serverTime);
	free(tokenTime);
    if(mode==TRACEFILE)
		free(tTotalPacket);
	return 0;
	
}

void recalculate()
{
	my402queueElem *head;
	Packet *curPacket;
	double t1 = 0.0;
	removeElements(&bucket, bucket.num_members);
	removedQ1Packets = q1.num_members;
	//removeElements(&q1, removedQ1Packets);
	while(!isEmpty(&q1))
	{
		head = getFrontElem(&q1);
		curPacket = (Packet *)head->data;
		fprintf(stdout,"%012.3lfms: p%lu removed from Q1\n",(computeElapsedTime() *1000.0), curPacket->pNumber);
		free((Packet *)head->data);
		dequeue(&q1);
	}
	removedQ2Packets = q2.num_members;
	while(!isEmpty(&q2))
	{
		head = getFrontElem(&q2);
		curPacket = (Packet *)head->data;
		t1 = (curPacket->t2 - curPacket->t1);
		sq1t = sq1t - t1;
		fprintf(stdout,"%012.3lfms: p%lu removed from Q2\n",(computeElapsedTime() *1000.0), curPacket->pNumber);
		free((Packet *)head->data);
		dequeue(&q2);
	}
}

void *monitorSignal(void *arg)
{
	//just handling SIGINT for this assignment.
	int sig;
	while(1)
	{
		sigwait(&set, &sig);	
		sigTriggered = 1;
		printf("\n");
		break;
		
	}
	return NULL;
}

void *processToken(void * arg)
{
	sleepFormat *tokenTime = (sleepFormat *)arg;
	Token *newToken;
	my402queueElem *head;
	int q2state=0;
	while(!sigTriggered)
	{
		sleep(tokenTime->seconds);
		usleep(tokenTime->usecs);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		pthread_mutex_lock(&mutex);
		if((!isEmpty(&q1) || totalGenPacket < param.totalPackets) && (!sigTriggered))
		{
			newToken = generateToken();
			if(activeTokens >= param.bucketSize)
			{
				fprintf(stdout,"%012.3lfms: token t%lu arrives, dropped\n",(newToken->genTime *1000.0), newToken->tnumber);
				droppedToken++;
			}
			else
			{
				activeTokens++;
				fprintf(stdout,"%012.3lfms: token t%lu arrives, token bucket now has %lu tokens\n",(newToken->genTime * 1000), newToken->tnumber, activeTokens);	
				enqueue(&bucket, newToken);
			}
			//check if we can now move head element at q1 to q2;
			if(!sigTriggered)
			{
				head = getFrontElem(&q1);
				q2state = isEmpty(&q2);
				if(head!=NULL && canTransfer2q2(head))  // & sigTriggered =0
				{	
					if(q2state)
					{
						//signal or broadcast server a noti saying that q2 is no more  empty.
						pthread_cond_signal(&cv1);
					}

				}
			}
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
	}
	return NULL;
}

char *tCalRate(char *cursor, sleepFormat *time1)
{
	double sleepValue=0;
	char * token, *bufferCursor, *nextToken;
	int length=0;
	bufferCursor = cursor;
	nextToken = calLength(bufferCursor, &length);
	token = (char *)malloc((length+1)* sizeof(char));
	token = strncpy(token, bufferCursor, length);
	*(token + length)  = '\0';
	sleepValue = myatof(token, PARAM_RATE);
	calculateSleep(time1, (sleepValue/1000.0));
	free(token);
	return nextToken;
}

char *skipSpaces(char *bufferCursor)
{
	while(*bufferCursor==' ' || *bufferCursor=='\t')
	{
		bufferCursor++;
	}
	return bufferCursor;
}

char *tTokenReqf(char *bufferCursor, int *tokenReq)
{
	int length=0;
	char *token, *nextToken;
	nextToken = calLength(bufferCursor, &length);
	token = (char *)malloc((length+1)* sizeof(char));
	token = strncpy(token, bufferCursor, length);
	*(token + length) = '\0';
	*tokenReq = myatoi(token, PARAM_P);
	free(token);
	return nextToken;
}

void checkEndofLine(char *bufferCursor)
{
	if(*bufferCursor!='\n' && *bufferCursor!='\0')
	{
		printf("Line:%llu has wrong format",lineNumber);
		exit(1);
	}

}

void *processPacket(void *arg)
{
	sleepFormat *packetTime = (sleepFormat *)arg;
	sleepFormat *serverTime;
	int tTokenReq;
	Packet *newPacket;
	char buffer[1027];
	char *bufferCursor, *nextToken;
	my402queueElem *newElement, *head;
	int q2state=0;
	//testing
	serverTime = (sleepFormat *)malloc(sizeof(sleepFormat));
	memset(buffer, 0, sizeof(buffer));
	while((totalGenPacket < param.totalPackets) && (!sigTriggered))
	{
		if(mode==TRACEFILE)
		{
			bufferCursor = fgets(buffer, 1026,fp);
			if(bufferCursor==NULL)
				error("Invalid format occured in tracefile.\n");
			lineNumber++;
			checkSpaces(bufferCursor);
			nextToken = tCalRate(bufferCursor, packetTime);
			nextToken = skipSpaces(nextToken);
			nextToken = tTokenReqf(nextToken, &tTokenReq);
			nextToken = skipSpaces(nextToken);
			nextToken = tCalRate(nextToken, serverTime);
			checkEndofLine(nextToken);
			memset(buffer, 0, sizeof(buffer));
		}
		sleep(packetTime->seconds);
		usleep(packetTime->usecs);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		pthread_mutex_lock(&mutex);
		newPacket = generatePacket();
		if(mode==TRACEFILE)
		{
			newPacket->sSeconds = serverTime->seconds;
			newPacket->sUsecs = serverTime->usecs;
			newPacket->tokenReq = tTokenReq;
		}
		else
		{
			newPacket->sSeconds = 0;
			newPacket->sUsecs = 0;
			newPacket->tokenReq = param.tokenReq;
		}
		
		if(newPacket->tokenReq > param.bucketSize){
			if(!sigTriggered)
			{
				fprintf(stdout,"%012.3lfms: p%lu arrives, needs %d tokens, inter-arrival time = %.3lfms, dropped\n", (newPacket->t0 * 1000.0), newPacket->pNumber, newPacket->tokenReq, interArrivalTime(newPacket->t0));	
				lastGenPact0 = newPacket->t0;
				droppedPacket++;
			}
			else
				{
				totalGenPacket--;
				}
			}
		else{
			if(sigTriggered){
				totalGenPacket--;
			}
			else 
			{
				fprintf(stdout,"%012.3lfms: p%lu arrives, needs %d tokens, inter-arrival time = %.3lfms\n", (newPacket->t0 * 1000.0), newPacket->pNumber, newPacket->tokenReq, interArrivalTime(newPacket->t0));
				lastGenPact0 = newPacket->t0;
				newElement = enqueue(&q1, newPacket);
				newPacket->t1 = computeElapsedTime();
				fprintf(stdout,"%012.3lfms: p%lu enters Q1\n", (newPacket->t1 * 1000), newPacket->pNumber);
				head = getFrontElem(&q1);
				if((head == newElement) && (!sigTriggered))
					{
						q2state = isEmpty(&q2);
						if(canTransfer2q2(head))
						{	
							if(q2state)
							{
								//signal or broadcast server a noti saying that q2 is no more  empty.
								pthread_cond_signal(&cv1);
							}

						}
					}
			}
		}
		pthread_mutex_unlock(&mutex);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
	}
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cv1);
	terminateServer = 1;
	pthread_mutex_unlock(&mutex);
	free(serverTime);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
	return NULL;
}

Packet *generatePacket()
{
	Packet *newPacket =(Packet *)malloc(sizeof(Packet));
	totalGenPacket++;
	newPacket->t0 = computeElapsedTime();
	newPacket->pNumber = totalGenPacket;
	return newPacket;
}

Token *generateToken()
{
	Token *newToken =(Token *)malloc(sizeof(Token));
	totalGenToken++;
	newToken->genTime = computeElapsedTime();
	newToken->tnumber = totalGenToken;
	return newToken;
}

double interArrivalTime(double time)
{
	siat += (time-lastGenPact0);
	return ((time - lastGenPact0) * 1000);
}

int canTransfer2q2(my402queueElem *element)
{
	Packet *elemPacket = (Packet *)element->data;
	if(elemPacket->tokenReq <= activeTokens)
	{
		dequeue(&q1);
		elemPacket->t2 = computeElapsedTime();
		removeTokens(elemPacket->tokenReq);
		fprintf(stdout,"%012.3lfms: p%lu leaves Q1, time in Q1 = %.3lfms, token bucket now has %lu tokens\n",(elemPacket->t2 * 1000), elemPacket->pNumber,timeInQ1(elemPacket), activeTokens);
		enqueue(&q2, elemPacket);
		elemPacket->t3 = computeElapsedTime();
		fprintf(stdout,"%012.3lfms: p%lu enters Q2\n", (elemPacket->t3 * 1000.0), elemPacket->pNumber);
		return SUCCESS;
	}
	return FAILURE;

}

Packet *startService()
{
	my402queueElem *head;
	double requestTime=0;
	head = getFrontElem(&q2);
	Packet *elemPacket = (Packet *)head->data;
	dequeue(&q2);
	elemPacket->t4 = computeElapsedTime();
	fprintf(stdout,"%012.3lfms: p%lu leaves Q2, time in Q2 = %.3lfms\n", (elemPacket->t4 * 1000.0),elemPacket->pNumber,timeInQ2(elemPacket));
	elemPacket->t5 = computeElapsedTime();	
	if(mode==TRACEFILE)
	{
		requestTime = ((elemPacket->sSeconds)*1000.0) + ((double)(elemPacket->sUsecs)/1000.0);
		fprintf(stdout,"%012.3lfms: p%lu begins service at S, requesting %.3lfms of service\n",(elemPacket->t5 * 1000.0), elemPacket->pNumber, requestTime);
	}
	else
		fprintf(stdout,"%012.3lfms: p%lu begins service at S, requesting %.3lfms of service\n",(elemPacket->t5 * 1000.0), elemPacket->pNumber, muInv*1000);
	return elemPacket;
}

void removeTokens(int tokenReq)
{	
	removeElements(&bucket, tokenReq);
	activeTokens-=tokenReq;
}

double timeInQ1(Packet *curPacket)
{
	sq1t += (curPacket->t2 - curPacket->t1);
	return ((curPacket->t2 - curPacket->t1) *1000);
}

double timeInQ2(Packet *curPacket)
{
	sq2t += (curPacket->t4 - curPacket->t3);
	return ((curPacket->t4 - curPacket->t3)*1000);
}

double timeAtServer(Packet *curPacket)
{
	servicedPacketCount++;
	totalServiceTime += (curPacket->t6 - curPacket->t5);
	return ((curPacket->t6 - curPacket->t5)*1000);
}

double timeInSystem(Packet *curPacket)
{
	double timeSpent = (curPacket->t6 - curPacket->t0);
	sumSqrTiSys += (timeSpent * timeSpent);
	sumSystemTime += timeSpent;
	return ((curPacket->t6 - curPacket->t0)*1000);
}

void *processServer(void *arg)
{
	sleepFormat *serviceTime = (sleepFormat *)arg;
	Packet *elemPacket;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if((!isEmpty(&q1) || !isEmpty(&q2) || totalGenPacket < param.totalPackets) && (!sigTriggered))
		{
			while(isEmpty(&q2) && (!terminateServer))
			{
				pthread_cond_wait(&cv1, &mutex);
			}
			if((!isEmpty(&q2)) && (!sigTriggered))
			{
				elemPacket = startService();
				pthread_mutex_unlock(&mutex);
				if(mode==TRACEFILE)
				{
					serviceTime->seconds = elemPacket->sSeconds;
					serviceTime->usecs = elemPacket->sUsecs;
				}

				//mark a variable which says that server is busy and wait for printing statistic until sleep is over.
				sleep(serviceTime->seconds);
				usleep(serviceTime->usecs);
				//ejectPacket()
				elemPacket->t6 = computeElapsedTime();
				fprintf(stdout,"%012.3lfms: p%lu departs from S, service time = %.3lfms, time in system = %.3lfms\n",(elemPacket->t6 * 1000.0), elemPacket->pNumber,timeAtServer(elemPacket),timeInSystem(elemPacket));
				lastServed = *elemPacket;
				free(elemPacket);
			}
			else
				pthread_mutex_unlock(&mutex);
		}
		else
		{
			pthread_mutex_unlock(&mutex);
			break;
		}
	}
	return NULL;
}

//All statistic functions
void printStats()
{
	printf("\nStatistics:\n");
	apiat();   //avg packet inter arrival time
	apst();    //avg packet service time
	aPinQ1();  //avg packet in q1;  
	aPinQ2();  //avg packet in q2;
	aPatS();   //avg packet at S;
	aSysTime(); //avg time in system
	calDeviation(); //calculate deviation for system time spent by packet.
	tokenDropProb(); //calculate token drop probability.
	packetDropProb();  //calculate packet drop probability.
}

void aPinQ1()
{
	double value;
	value = sq1t/emuEndTime;
	printf("\n\taverage number of packets in Q1 = %.6g\n",value);
}

void aPinQ2()
{
	double value=0.0;
	value = sq2t/emuEndTime;
	printf("\taverage number of packets in Q2 = %.6g\n",value);
}

void aPatS()
{

	double value;
	value = (double)totalServiceTime/emuEndTime;
	printf("\taverage number of packets at S = %.6g\n",value);
}

void aSysTime()
{
	double value =0.0;
	if(servicedPacketCount)
		value = sumSystemTime/servicedPacketCount;
	printf("\n\taverage time a packet spent in system = %.6g\n",value);
}

double getAvgSysTime()
{
	double value=0.0;
	if(servicedPacketCount)
		value = sumSystemTime/servicedPacketCount;
	return value;
}

void calDeviation()
{
	double term1, term2, variance=0.0, deviation=0.0;
	double avgSysTime = getAvgSysTime();
	if(servicedPacketCount)
	{
		term1 = (double)sumSqrTiSys/servicedPacketCount;
		term2 = avgSysTime * avgSysTime;
		variance = term1 - term2;
	}
	if(variance != 0.0)
		deviation = (double)sqrt(variance);
	printf("\tdeviation for time spent in system = %.6g\n",deviation);
}

void tokenDropProb()
{
	double value=0.0;
	if(totalGenToken !=0)
		value = ((double)droppedToken)/((double)totalGenToken);
	printf("\n\ttoken drop probability = %.6g\n", value);
}

void packetDropProb()
{
	double value=0.0;
	if(totalGenPacket!=0)
		value = ((double) droppedPacket)/((double) totalGenPacket);
	printf("\tpacket drop probability = %.6g\n", value);
}

void apiat()
{
	double value=0.0;
	if(totalGenPacket!=0)
		value = siat/(double)totalGenPacket;
	printf("\n\taverage packet inter-arrival time = %.6gs\n",value);
}

void apst()
{
	double value=0.0;
	if(servicedPacketCount)
		value = (double)totalServiceTime/servicedPacketCount;
	fprintf(stdout,"\taverage packet service time = %.6gs\n",value);
}




