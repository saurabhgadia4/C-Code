#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<sys/types.h>
#include<stdbool.h>
#include"my402sort.h"
#include"my402list.h"
unsigned long long int lineNumber = 0;
unsigned long long int transactionCount = 0;
long long int balanceDollar = 0;
int balanceCent = 0;
int balanceStatus = 0;

char *TimestampBuf = NULL;
char *AmountBuf = NULL;

void initBuffer() //call at the end of reading of every line.
{
    TimestampBuf = NULL;
    AmountBuf = NULL;
}

void error(char *msg)
{
    fprintf(stderr, msg, lineNumber);
    exit(1);
}

char *truncateSpace(char *bufferCursor)
{
    while(*(bufferCursor) == ' ')
        bufferCursor++;
    return bufferCursor;
}

void validateTabCount(char *buffer)
{
    int tabcount = 0;
    while(*buffer!='\0' && tabcount<4){
        if(*buffer == '\t')
            tabcount++;
        buffer++;
    }
    if(tabcount>4)
        error("Tabcount limit(4) exceeded for transaction line %llu\n.Exiting sort operation.\n");
}

My402Object *allocateObject()
{
    return (My402Object *)malloc(sizeof(My402Object));
}

void allocate(int length, int type)
{
    switch(type){
        case TIMESTAMP:
        {
            free(TimestampBuf);
            TimestampBuf = (char *)malloc((length+1)*sizeof(char));
            break;
        }
        case AMOUNT:
        {
            free(AmountBuf);
            AmountBuf = (char *)malloc((length+1)*sizeof(char));
            break;
        }

    }
}

char *validateType(char *bufferCursor, My402Object *newObject)
{
    if ((*(bufferCursor)) == '+')
        newObject->type = 0;
    else if ((*(bufferCursor)) == '-')
        newObject->type = 1;
    else if ((*(bufferCursor))!='+' || (*(bufferCursor))!='-'){
        fprintf(stderr,"Invalid transaction type as '%c' encountered on line:%llu\nSo exiting sort operation.\n",*bufferCursor, lineNumber);
        exit(1);
    }
    bufferCursor = bufferCursor + 1;
    return (bufferCursor);
}

char *checkTab(char *bufferCursor)
{
    if((*(bufferCursor)) != '\t'){
        fprintf(stderr,"Invalid transaction. Expected [tab] but received '%c' on line:%llu\nSo exiting sort operation.\n",*bufferCursor, lineNumber);
        exit(1);
    }
    else{
        bufferCursor = bufferCursor + 1;
        return (bufferCursor);
    }

}

char *calLength(char *tempBuf, int *length)
{
    while((*(tempBuf)) != '\t'){
        tempBuf++;
        (*length)++;
    }
    return tempBuf;
}

char *getTimestamp(char *bufferCursor, My402Object *newObject)
{
    //bufferCursor points to character after 1st [tab] on line. 
    char *nextTab;
    int length = 0;
    nextTab = calLength(bufferCursor, &length);
    if(length < 1 || length >= 11)
        error("Invalid transaction. Did not received valid timestamp field on line %llu\nSo exiting sort operation.\n");
    
    allocate(length, TIMESTAMP);
    copyItem(bufferCursor, TimestampBuf);
    validateTimestamp(TimestampBuf, newObject);
    nextTab = nextTab +1;
    return (nextTab);
}

char *getAmount(char *bufferCursor, My402Object *newObject)
{
    char *nextTab;
    int length = 0;
    nextTab = calLength(bufferCursor, &length);
    if(length<4)   //X.xx
        error("Invalid transaction. Did not received valid amount field on line %llu\nSo exiting sort operation.\n");
    allocate(length, AMOUNT);
    copyItem(bufferCursor, AmountBuf);
    validateAmount(AmountBuf, newObject);
    nextTab = nextTab +1;
    return (nextTab);
}

void copyItem(char *bufferCursor, char *Buf) //[tab]item[tab]
{
    char *Temp = Buf;
    while(*bufferCursor!='\t'){
        *Temp++ = *bufferCursor++;
    }
    *Temp = '\0';
}

bool disDigit(char *p, long long int*value)
{
    long long int data = 0;
    while(*p!='\0')
    {
        if((((*p) - '0') >=0) || (((*p) - '0') <=9))
        {
            data = data*10 + ((*p) - '0');
        }
        else
        {
            return FALSE;
        }
        p++;
    }
    *value = data;
    return TRUE;
}

bool tisDigit(char *p, unsigned long long int *value)
{
    unsigned long long int data = 0;
    while(*p!='\0')
    {
        if((((*p) - '0') >=0) || (((*p) - '0') <=9))
        {
            data = data*10 + ((*p) - '0');
        }
        else
        {
            return FALSE;
        }
        p++;
    }
    *value = data;
    return TRUE;
}

bool cisDigit(char *p, int *value)
{
    int data = 0;
    while(*p!='\0')
    {
        if((((*p) - '0') >=0) || (((*p) - '0') <=9))
        {
            data = data*10 + ((*p) - '0');
        }
        else
        {
            return FALSE;
        }
        p++;
    }
    *value = data;
    return TRUE;


}

void validateTimestamp(char *timestamp, My402Object *newObject)
{
    bool success = FALSE;
    unsigned long long int ts;
    success = tisDigit(TimestampBuf, &ts);
    if(success == FALSE)
        error("Timestamp format is invalid on line %llu.\n");

    newObject->timestamp = ts;
}

void validateAmount(char *Amount, My402Object *newObject)
{
    long long int Dollar = 0;
    int cent = 0;
    calAmount(Amount, &Dollar, &cent);
    newObject->dollar = Dollar;
    newObject->cent = cent;
}

void calAmount(char *Amount, long long int *Dollar, int *Cent)
{
    char *decimalPos,*temp;
    int DollarLen, CentLen, i=0;
    bool dollarSuccess = FALSE, centSuccess = FALSE;
    temp = Amount;
    decimalPos = strchr(Amount, '.');
    if(decimalPos == NULL)
        error("Amount format is invalid on line %llu.\nExiting sort operation.\n");
   
    //cent figure validation.
    temp = Amount;
    DollarLen = decimalPos - Amount;
    CentLen = strlen(Amount) - DollarLen-1;
    if(CentLen != 2)
        error("Amount field don't have 2-digit value cent on line %llu.\nExiting sort operation");
        
    if(DollarLen>7)
        error("In amount field Dollar value is greater than 10M on line %llu.\nExiting sort operation.\n");
    
    char DollarHolder[DollarLen+1], CentHolder[3];
    while(*temp!='.')
    {
        DollarHolder[i] = *temp;
        i++;
        temp++;
    }
    DollarHolder[i] = '\0';
    temp = decimalPos;
    temp +=1;
    i = 0;
    while(*temp!='\0'){
        CentHolder[i] = *temp;
        i++;
        temp++;
    }
    CentHolder[i] = '\0';
    dollarSuccess = disDigit(DollarHolder, Dollar);
    if(dollarSuccess == FALSE)
        error("Dollar field of amount on line %llu is invalid.\nExiting sort operation");
      
    centSuccess = cisDigit(CentHolder, Cent);
    if(centSuccess == FALSE)
        error("Cent field of amount on line %llu is invalid.\nExiting sort operation");
}

char *getDescription(char *bufferCursor, My402Object *newObject)
{
    int i = 0;
    while(i < 24 && *(bufferCursor) != '\n' && *(bufferCursor) != '\0')
    {
        newObject->description[i] = *bufferCursor;
        i++;
        bufferCursor++;
    }
    newObject->description[i] = '\0';
    return bufferCursor;
}

void printObject(My402Object *newObject)
{
    printf("type:%d\ttimestamp:%lld\tdollar:%lld\tcent:%d\tdesc:%s\n",newObject->type, newObject->timestamp, newObject->dollar, newObject->cent, newObject->description);
}

void printList(My402List *Anchor_holder)
{
    My402ListElem *element;
    element = My402ListFirst(Anchor_holder);
    printf("\nnumber of transaction = %d\n",Anchor_holder->num_members);
    while(element->obj != NULL)
    {
        printObject((My402Object*)element->obj);
        element = element->next;
    }

}

My402Object *getElemObj(My402ListElem *element)
{
    if(element == NULL)
        return NULL;
    else
    {
        return ((My402Object *)element->obj);
    }
}

int insertTransaction(My402List *Anchor_holder, My402Object *newObject)
{
    My402ListElem *element;
    My402Object *elemObj;
    int i, success=0;
    element = My402ListFirst(Anchor_holder);
    elemObj = getElemObj(element);
    for(i=1;i<transactionCount;i++)
    {
        if(newObject->timestamp > elemObj->timestamp)
        {
            if(i == (transactionCount - 1))
            {
                success = My402ListInsertAfter(Anchor_holder, newObject, element);
                return success;
            }
            element = My402ListNext(Anchor_holder, element);
            elemObj = getElemObj(element);
            continue;
        }
        else if (newObject->timestamp < elemObj->timestamp)
            break;
        else
            error("Duplicate timestamp value encountered on line %llu.\n");
    }
    success = My402ListInsertBefore(Anchor_holder, newObject, element);
    return success;
}


char *formatStart(char *ptr)
{
    *ptr = '|';
    ptr++;
    *ptr = ' ';
    ptr++;
    return ptr;
}

char *placeTime(unsigned long long int timestamp, char *outCursor)
{
    char *htime, *temp;
    int i;
    temp = outCursor;
    htime = ctime((time_t *)&timestamp);
    temp = formatStart(temp);
    for(i=0;i<=10;i++)
        *temp++ = *htime++;
    for(i=11;i<=19;i++)
        htime++;
    for(i=20;i<24;i++)
        *temp++ = *htime++;
    *temp = ' ';
    temp++;
    return temp;
}

char *placeDesc(char *desc, char *outCursor)
{
    char *temp = outCursor;
    temp = formatStart(temp);
    while(*desc!='\0')
        *temp++ = *desc++;
    *temp = ' ';
    temp++;
    return temp;
}

void calBalance(long long int dollar, int cent, int type)
{
    /* BD->+ve and DEPO*/
    if((balanceStatus == POSITIVEBALANCE) && (type == DEPOSIT))
    {
        balanceCent += cent;
        if(balanceCent >= 100)
        {
            balanceCent -= 100;
            balanceDollar += 1; 
        }
        balanceDollar += dollar; 
        balanceStatus = POSITIVEBALANCE;
    }
    
    /* BD->-ve and WD*/


    if((balanceStatus == NEGATIVEBALANCE) &&(type == WITHDRAWL))
    {
        balanceCent += cent;
        if(balanceCent>=100)
        {
            balanceCent -=100;
            balanceDollar = balanceDollar -1; 
        }
        balanceDollar -= dollar; 
        balanceStatus = NEGATIVEBALANCE;
    }

    /* bd->+ve type->WD*/

    if((balanceStatus == POSITIVEBALANCE) && (type == WITHDRAWL))
    {
        if(dollar > balanceDollar)
        {
            if((cent - balanceCent)<0)
            {
                cent +=100;
                balanceCent = cent -balanceCent;
                dollar -=1;
                balanceDollar = (dollar - balanceDollar) * (-1);
            }
            else if(cent == balanceCent)
            {
                balanceCent = 0;
                balanceDollar = (dollar - balanceDollar) * (-1);
            }
            else if((cent - balanceCent)>0)
            {
                balanceCent = cent - balanceCent;
                balanceDollar = (dollar - balanceDollar) *(-1);
            }
            balanceStatus = NEGATIVEBALANCE;
        }
        else if(dollar == balanceDollar)
        {
            if((cent - balanceCent)<0)
            {
                balanceCent = balanceCent - cent;
                balanceDollar = 0;
                balanceStatus = POSITIVEBALANCE;
            }
            else if(cent == balanceCent)
            {
                balanceCent = 0;
                balanceDollar = 0;
                balanceStatus = POSITIVEBALANCE;
            }
            else if((cent - balanceCent)>0)
            {
                balanceCent = cent - balanceCent;
                balanceDollar = 0;
                balanceStatus = NEGATIVEBALANCE;
            }


        }
        else if(dollar < balanceDollar)
        {
            if(balanceStatus == POSITIVEBALANCE)
            {
                if((balanceCent-cent)>0 || (balanceCent == cent))
                {
                    balanceCent = balanceCent - cent;
                    balanceDollar -= dollar;
                }
                else if((balanceCent-cent)<0)
                {
                    balanceCent = 100 + balanceCent - cent;
                    balanceDollar = balanceDollar -1 -dollar;
                }
            }
        }

    }

    if((balanceStatus == NEGATIVEBALANCE) && (type == DEPOSIT))
    {
        if(dollar > (-1*balanceDollar))
        {
            if((cent - balanceCent) < 0)
            {
                balanceCent = cent + 100 - balanceCent;
                balanceDollar = dollar -1 + balanceDollar;
                
            }
            else if((cent - balanceCent) > 0)
            {
                balanceCent = cent - balanceCent;
                balanceDollar = dollar - balanceDollar;
                
            }
            else
            {
                balanceCent = 0;
                balanceDollar = dollar - balanceDollar;

            }
            balanceStatus = POSITIVEBALANCE;

        }
        else if(dollar == (-1*balanceDollar))
        {
            if((cent-balanceCent)>0)
            {
                balanceCent = cent - balanceCent;
                balanceDollar = 0;
                balanceStatus = POSITIVEBALANCE;
            }
            else if((cent-balanceCent)<0)
            {
                balanceCent -=cent;
                balanceDollar =0;
                balanceStatus = NEGATIVEBALANCE;
            }
            else if((cent-balanceCent) ==0)
            {
                balanceCent = 0;
                balanceDollar = 0;
                balanceStatus = POSITIVEBALANCE;
            }
        }

        else if(dollar < (-1*balanceDollar))
        {
            if((cent - balanceCent) < 0)
            {
                balanceCent -= cent;
                balanceDollar += dollar; 
            }
            else if((cent - balanceCent) > 0)
            {
                balanceCent = balanceCent + 100 -cent;
                balanceDollar = balanceDollar + dollar + 1; 
            }
            else
            {
                balanceCent = 0;
                balanceDollar = balanceDollar + dollar;
            }
            balanceStatus = NEGATIVEBALANCE;

        }
    }

}

char *fillSpace(char *ptr, int startIndex, int endIndex)
{
    int i=startIndex;
    while(i<=endIndex)
    {
        *ptr = ' ';
        ptr++;
        i++;
    }
    return ptr;
}

char *placeAmount(long long int dollar, int cent, int transType, char *outCursor)
{
    char *temp = outCursor, *decimalPos;
    char *fill;
    calBalance(dollar, cent, transType);
    temp = formatStart(temp);
    temp = formatTcType(transType, temp, TCSTART);
    /*filling amount part with space and endpart of amount*/
    fill = temp;
    fill = fillSpace(fill, 0, 11);
    decimalPos = fill -3;
    *decimalPos = '.';
    fill = formatTcType(transType, fill, TCEND);
    *fill = ' ';
    fill = fill + 1;
    /*I need to return fill which points to start of balance field*/
    ddtoa(&dollar, decimalPos);
    cdtoa(&cent, decimalPos);
    temp = fill;
    return fill;  ///points to start of balance i.e 62nd character
}

char *placeBalance(long long int dollar, int cent, int transType, char *outCursor)
{
    char *temp = outCursor, *decimalPos;
    char *fill;
    temp = formatStart(temp);
    temp = formatTcType(transType, temp, TCSTART);
    /*filling amount part with space and endpart of amount*/
    fill = temp;
    fill = fillSpace(fill, 0, 11);
    decimalPos = fill -3;
    *decimalPos = '.';
    fill = formatTcType(transType, fill, TCEND);
    *fill = ' ';
    fill = fill + 1;
    /*I need to return fill which points to start of balance field*/
    if((balanceStatus == NEGATIVEBALANCE) && (dollar<0))
        dollar = dollar *(-1);
    ddtoa(&dollar, decimalPos);
    cdtoa(&cent, decimalPos);
    temp = fill;
    
    if(balanceDollar > 9999999 || balanceDollar < (-9999999))
        reformat(decimalPos);
    return fill;  ///points to start of balance i.e 62nd character
}


void reformat(char *origdecimalPos)
{
    char *temp = origdecimalPos;
    int count = 0, i=0;
    //set cent placeholders to ?
    temp++;
    *temp++ = '?';
    *temp = '?';
    temp = origdecimalPos-1;
    for(i=0;i<7;i++)
    {
        *temp-- = '?';
        count++;
        if(count%3 == 0)
            temp--;
    }
}

void ddtoa(long long int *dataHolder, char *origdecimalPos)
{
    char num, *origptr;
    int count =0;
    origptr = origdecimalPos -1;
    *origptr = '0';
    while((*dataHolder) > 0)
        {
            if((((count) % 3) == 0) && count !=0)
            {
                *origptr = ',';
                origptr--;
            }
            num = ((*dataHolder) % 10) + '0';
            (*dataHolder) = (*dataHolder)/10;
            *origptr = num;
            origptr--; 
            count++;
        }
}

void cdtoa(int *dataHolder, char *origdecimalPos)
{
    char num, *origptr;
    origptr = origdecimalPos+1;
    *origptr++ = '0';
    *origptr = '0';
    origptr = origdecimalPos + 2;
    while((*dataHolder) > 0)
    {
        num = ((*dataHolder) % 10) + '0';
        (*dataHolder) = (*dataHolder)/10;
        *origptr = num;
        origptr--; 
    }

}

char *formatTcType(int transType, char *outCursor, int position)
{
    if(transType == DEPOSIT)
        *outCursor = ' ';
    else if(transType == WITHDRAWL){
        if(position==TCSTART)
            *outCursor = '(';
        else
            *outCursor = ')';
    }
    outCursor++;
    return outCursor;
}

void renderOutput(My402List *Anchor_holder)
{
    char output[81], *outCursor;
    My402ListElem *element;
    My402Object *object;
    element = My402ListFirst(Anchor_holder);
    while(element->obj != NULL)
    {
        memset(output, ' ', sizeof(output));
        output[81] = '\0';
        object = (My402Object *)element->obj;
        outCursor = placeTime(object->timestamp, output);
        outCursor = placeDesc(object->description, outCursor);
        outCursor = placeAmount(object->dollar, object->cent, object->type, (output+45));
        outCursor = placeBalance(balanceDollar, balanceCent, balanceStatus, (output+62));
        *outCursor++ = '|';
        *outCursor = '\n';
        fprintf(stdout,"%s\n",output);
        element = element->next;
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char buffer[1027]; // in the end check for bufferCursor-buffer<1024
    char *cursor;
    char *bufferCursor;
    int bufferLength = 0;
    int anchorInit = 0, insertSuccess =0;
    My402List Anchor_holder;
    anchorInit = My402ListInit(&Anchor_holder);
    if(!anchorInit)
        error("Unable to initialize the DCLL on line %llu.\n");
    
    My402Object *newObject;
    fp = fopen(argv[1], "r");
    if(fp ==NULL){
    printf("%s", strerror(errno));
    exit(1);
    }
    memset(buffer, 0, sizeof(buffer));
    while((cursor = fgets(buffer, 1026,fp))!=NULL)
    {
        lineNumber++;
        bufferLength = 0;
        bufferLength = strlen(buffer);
        if(bufferLength>1023)
            error("Transaction line length exceeded 1024 characters limit for line %llu.\nSo exiting sort operation.\n");
        validateTabCount(buffer);
        newObject = allocateObject();
        bufferCursor = truncateSpace(buffer);
        bufferCursor = validateType(bufferCursor, newObject);
        bufferCursor = checkTab(bufferCursor); //checks tab after type of operation
        bufferCursor = getTimestamp(bufferCursor, newObject);
        bufferCursor = getAmount(bufferCursor, newObject);
        bufferCursor = getDescription(bufferCursor, newObject);
        transactionCount++;
        insertSuccess = insertTransaction(&Anchor_holder, newObject);
        if(!insertSuccess)
            error("Unable to insert transaction on line %llu");
        memset(buffer, 0, sizeof(buffer));

    }
    printList(&Anchor_holder);
    renderOutput(&Anchor_holder);
    return 0;
}
