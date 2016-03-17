#define DEPOSIT 0
#define WITHDRAWL 1
#define TIMESTAMP 1
#define AMOUNT 2
#define DOLLAR 1
#define CENT 2
#define TCSTART 1
#define TCEND 2
#define POSITIVEBALANCE 0
#define NEGATIVEBALANCE 1

#include"my402list.h"
#include<stdbool.h>
typedef struct tagMy402Object {
    int type;
    unsigned long long int timestamp;
    long long int dollar;
    int cent;
    char description[24];

} My402Object;
extern void initBuffer();
extern void error(char *);
extern void validateTabCount(char *);
extern char *truncateSpace(char *);
extern My402Object *allocateObject();
extern void allocate(int, int);
extern char *validateType(char *, My402Object *);
extern char *checkTab(char *);
extern char *getTimestamp(char *, My402Object *); 
extern void copyItem(char *, char *);
extern bool disDigit(char *, long long int *);
extern bool tisDigit(char *, unsigned long long int *);
extern bool cisDigit(char *, int *);
/*--*/extern void validateTimestamp(char *, My402Object *);
/*--*/extern char *getAmount(char *, My402Object *);
/*--*/extern char *calLength(char *, int *);
/*--*/extern void validateAmount(char *, My402Object *);
/*--*/extern void calAmount(char *, long long int *, int *);
/*--*/extern char *getDescription(char *, My402Object *);
/*--*/extern void printObject(My402Object *);
/*--*/extern My402Object *getElemObj(My402ListElem *);
/*--*/extern int insertTransaction(My402List *, My402Object *);
/*--*/extern void printList(My402List *);
/*--*/extern void renderOutput(My402List *);
/*--*/extern char *placeTime(unsigned long long int, char *);
/*--*/extern char *formatStart(char *);
/*--*/extern char *placeDesc(char *, char *);
/*--*/extern char *placeAmount(long long int, int, int, char *);
/*--*/extern void calBalance(long long int, int, int);
/*--*/extern char *formatTcType(int, char *, int);
/*--*/extern char *fillSpace(char *, int, int);
/*--*/extern void ddtoa(long long int*, char *);
/*--*/extern void cdtoa(int*, char *);
/*--*/extern void reformat(char *);
/*--*/extern char *placeBalance(long long int, int, int, char *);