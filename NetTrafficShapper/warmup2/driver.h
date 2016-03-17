#include"my402queue.h"
#define DETERMINISTIC 1
#define TRACEFILE 2
#define PTIMESTAMP double
#define TTIMESTAMP double
#define PACKETQUEUE 1
#define TOKENQUEUE 2
enum parameters {PARAM_LAMBDA, PARAM_MU, PARAM_RATE, PARAM_B, PARAM_P, PARAM_N, PARAM_T};

typedef struct
{
	unsigned int seconds;
	unsigned int usecs;
}sleepFormat;

typedef struct tagMyPacket
{
	unsigned long int pNumber;
	PTIMESTAMP t0,t1,t2,t3,t4,t5,t6;
	int tokenReq;
	unsigned int sSeconds, sUsecs;

}Packet;

typedef struct tagCommandLine{
	float lambda, mu, rate;
	unsigned long int totalPackets, bucketSize, tokenReq;
	char *fpath;

} option;
typedef struct tagMyToken
{
	unsigned long int tnumber;
	TTIMESTAMP genTime;
	int status; //doubtful of this variable;
}Token;

extern double computeElapsedTime();
extern void parseOption(int, char **);
extern double myatof(char *, int);
extern long long int myatoi(char *, int);
extern void setDefaultOptions();
extern void setOptionValues(int, char *);
extern void printParams();
extern void error(char *);
extern void setInvRate();
extern void *processPacket(void *);
extern void calculateSleep(sleepFormat *, double);
extern void startEmulator();
extern Packet *generatePacket();
extern double interArrivalTime(double);
extern int canTransfer2q2(my402queueElem *);
extern void removeTokens(int);
extern double timeInQ1(Packet *);
extern double timeInQ2(Packet *);
extern void *processToken(void *);
extern Token *generateToken();
extern void *processServer(void *);
extern double timeInSystem(Packet *);
extern double timeAtServer(Packet *);
extern Packet *startService(); 
extern void printStats();
extern void apiat();
extern void apst();
extern void aPinQ1();
extern void aPinQ2();
extern void aPatS();
extern void aSysTime();
extern double getAvgSysTime();
extern void calDeviation();
extern void tokenDropProb();
extern void packetDropProb();
extern void checkEndofLine();
extern char *tTokenReqf(char *, int *);
extern char *skipSpaces(char *);
extern char *tCalRate(char *, sleepFormat *);
extern char *calLength(char *, int *);
extern void checkSpaces(char *);
extern void *monitorSignal(void *);
extern void recalculate();
