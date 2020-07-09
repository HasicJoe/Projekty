/***********************************************************************
****        Operacne systemy:PROJEKT 2 -Synchronizacia procesov     ****
****        Autor: Samuel Valastin, xvalas10                        ****
****        Sk. rok: 2019/2020                                      ****
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h> 
#define TURN_ON 1
#define TURN_OFF 0

// struct for parsing input arguments
typedef struct{
    int ProcI; // PI
    int ImmG; // IG
    int JudgeG; // JG
    int ImmTime; // IT
    int JudgeTime; // JT
}InputArgs;

// global variables for shared memory
int *ActionCount = NULL;
int *ImmEnt = NULL;
int *ImmCon = NULL;
int *ImmBuild = NULL;
int *Judge = NULL;
int *Conleave = NULL;
int *TotalConfirmed = NULL;

//pointer to file
FILE *file;

// semaphores
sem_t *fileWriting = NULL;
sem_t *mutex = NULL;
sem_t *noJudge = NULL;
sem_t *confirm = NULL;
sem_t *leave = NULL;
sem_t *allSignedIn = NULL;

// functions declarations
void printErrors(int error);
int checkArgs(int argc,char **argv,InputArgs *args);
int checkInputValues(InputArgs *args);
int getTime(InputArgs *args,int set);
bool init();
void deInit();
void procImmigrant(InputArgs *args,int id);
void procJudge(InputArgs *args);

int main(int argc,char *argv[])
{
    // struct for parsing input args
    InputArgs args;
    int error = 0;
    // set for random time
    srand(time(NULL)); 
    if(checkArgs(argc,argv,&args) != 0 || checkInputValues(&args) != 0)
    {
        return 1;
    }
    file = fopen("proj2.out","w");
    if(file == NULL)
    {
        error = 4;
        printErrors(error);
        return 1;
    }
    if(!init())
    {
        error = 5;
        printErrors(error);
        deInit();
        return 1;
    }
    setbuf(file,NULL);
    // generate help proces for generating immigrants
    pid_t pid = fork();
    if(pid == 0)
    {
        // child process of pid - help process to handle immigrants
        pid_t immpid;
        for(int i = 0; i < args.ProcI ; i++)
        {
            immpid = fork();
            if(immpid == 0)
            {
                int time = getTime(&args,2);
                usleep(time);
                procImmigrant(&args,i);
            }
            else if (immpid < 0)
            {
                int error = 6;
                printErrors(error);
                deInit(); // if fork fails deInit shared memory and end with code 1
                return 1;
            }
        }
        waitpid(immpid,NULL,0);
    }
    else if(pid > 0)
    {
        int time = getTime(&args,3);
        usleep(time);
        procJudge(&args);
        waitpid(pid,NULL,0);
    }
    else if(pid < 0)
    {
        //fork failed == return negative value
        error = 6;
        printErrors(error);
        deInit();
        return 1;
    }
    deInit();
    return 0;
}

/**
 * @brief Function print errors if something went wrong!
 * @param int error - value of printing error
 **/
void printErrors(int error)
{
    if(error == 1)
    {
        fprintf(stderr,"Invalid input - incorrect number of input arguments! \n");
        return;
    }
    else if(error == 2 )
    {
        fprintf(stderr,"Invalid argument!\n");
        return;
    }
    else if (error == 3)
    {
         fprintf(stderr,"Invalid argument value!\n");
         return;
    }
    else if (error == 4)
    {
        fprintf(stderr,"Failed to open proj2.out!");
        return;
    }
    else if(error == 5)
    {
        fprintf(stderr, "Cannot init shared variables and semaphores!\n");
        return;
    }
    else if(error == 6)
    {
        fprintf(stderr, "Process forking failed!\n");
        return;
    }
}

/**
 * @brief Function check args and convert string to integers and also save to struct
 * @param int argc - argument counter
 * @param char **argv - ** to argument value
 * @param InputArgs *args -> * to struct where we save the args
 * Function returns 0 when everything go well and <>0 when something went wrong
 */
int checkArgs(int argc,char **argv,InputArgs *args)
{
    int errorCode = 0;
    char *ptr;
    if(argc != 6)
    {
        errorCode = 1;
        printErrors(errorCode);
        return 1;
    }
    args->ProcI = strtol(argv[1],&ptr,10);
    if(*ptr != '\0')
	{
		errorCode = 2;
		printErrors(errorCode);
		return 1;
	}
    args->ImmG = strtol(argv[2],&ptr,10);
    if(*ptr != '\0')
	{
		errorCode = 2;
		printErrors(errorCode);
		return 1;
	}
	args->JudgeG = strtol(argv[3],&ptr,10);
    if(*ptr != '\0')
	{
		errorCode = 2;
		printErrors(errorCode);
		return 1;
	}
    args->ImmTime = strtol(argv[4],&ptr,10);
    if(*ptr != '\0')
	{
		errorCode = 2;
		printErrors(errorCode);
		return 1;
	}
    args->JudgeTime = strtol(argv[5],&ptr,10);
    if(*ptr != '\0')
	{
		errorCode = 2;
		printErrors(errorCode);
		return 1;
	}
    return 0;
}

/**
 * @brief Function checks input values (PI>0) IG,JG,IT,JT -> <0,2000>
 * @param InputArgs *args -> pointer to struct of input args
 * Returns 0 when values are correct and also 1 when they are wrong
 **/
int checkInputValues(InputArgs *args)
{
    int errorCode = 0;
    if(args->ProcI <= 0 || args->ImmG < 0 || args->ImmG > 2000 ||args->JudgeG < 0 
        || args->JudgeG > 2000 || args->ImmTime < 0 || args->ImmTime > 2000
        || args->JudgeTime < 0 || args->JudgeTime > 2000)
    {
        errorCode = 3;
        printErrors(errorCode);
        return 1;
    }
    return 0;
}

/**
 * @brief Function generate random time from input arguments
 * @param InputArgs *args struct of input args
 * @param int set is value which differentiate input args
 * Function returns random time
 */
int getTime(InputArgs *args,int set)
{
    int time = 0;
    if(set == 0)
    {
        time = args->ImmTime;
    }
    else if(set == 1) 
    {
        time = args->JudgeTime;
    }
    else if (set == 2)
    {
        time = args ->ImmG;
    }
    else if(set == 3)
    {
        time = args->JudgeG;
    }
    int randomTime = rand() % (time+1)*1000;
    return randomTime;
}

/**
 * @brief Function init shared variables and open semaphores
 * Returns false if something went wrong or true if init succeed
 */
bool init()
{
    // init shared variables 
    ActionCount =  mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0);
    ImmEnt = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0);
    ImmCon = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0);
    ImmBuild = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0);
    Judge = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0);
    Conleave = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0);
    TotalConfirmed = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0);
    
    // check if we succeeded
    if((ActionCount == MAP_FAILED) || (ImmEnt == MAP_FAILED) || (ImmCon == MAP_FAILED) || 
       (ImmBuild == MAP_FAILED) || (Conleave==MAP_FAILED) || (TotalConfirmed == MAP_FAILED))
    {
        return false;
    }
    // open semaphores
    fileWriting = sem_open("/xvalas10_fileWriting",O_CREAT | O_EXCL, 0666,TURN_ON);
    mutex = sem_open("/xvalas10_mutex",O_CREAT | O_EXCL, 0666, TURN_ON);
    noJudge = sem_open("/xvalas10_noJudge",O_CREAT | O_EXCL, 0666,TURN_ON);
    confirm = sem_open("/xvalas10_confirmation",O_CREAT | O_EXCL, 0666, TURN_OFF);
    leave = sem_open("/xvalas10_leave",O_CREAT | O_EXCL, 0666, TURN_OFF);
    allSignedIn = sem_open("/xvalas10_allSignedIn",O_CREAT | O_EXCL, 0666, TURN_OFF);

    //check if we succeeded
    if((fileWriting == SEM_FAILED) || (mutex == SEM_FAILED) || (noJudge == SEM_FAILED) ||
       (confirm == SEM_FAILED) || (leave == SEM_FAILED) || (allSignedIn == SEM_FAILED))
    {
        return false;
    }
    return true;
}

/**
 * @brief Function uninitialize shared variables and close semaphores & output file
 * @returns void
 */
void deInit()
{
    //close open semaphores
    sem_close(fileWriting);
    sem_close(mutex);
    sem_close(noJudge);
    sem_close(confirm);
    sem_close(leave);
    sem_close(allSignedIn);

    //remove closed semaphores
    sem_unlink("/xvalas10_fileWriting");
    sem_unlink("/xvalas10_mutex");
    sem_unlink("/xvalas10_noJudge");
    sem_unlink("/xvalas10_confirmation");
    sem_unlink("/xvalas10_leave");
    sem_unlink("/xvalas10_allSignedIn");
    
    //unmap shared variables
    munmap(ActionCount,sizeof(int));
    munmap(ImmCon,sizeof(int));
    munmap(ImmEnt,sizeof(int));
    munmap(ImmBuild,sizeof(int));
    munmap(Judge,sizeof(int));
    munmap(Conleave,sizeof(int));
    munmap(TotalConfirmed,sizeof(int));
    //close file if is opened
    if(file != NULL)
    {
        fclose(file);
    }
}

/**
 * @brief Function represents the process of immigrant
 * @param InputArgs *args represent struct of input arguments
 * @param int id each imm has his owm id
 */
void procImmigrant(InputArgs *args,int id)
{
    //starts, indexing from zero so we have to add 1
    id = id+1;
    sem_wait(fileWriting);
    fprintf(file,"%d\t : IMM %d	: starts\n",++(*ActionCount),id);
    sem_post(fileWriting);

    //try to enter = if no judge => enter else wait for judge leaving
    sem_wait(noJudge);
    sem_wait(fileWriting);
    fprintf(file,"%d\t : IMM %d	: enters	   \t\t\t: %d\t: %d\t: %d\n",++(*ActionCount),id,++(*ImmEnt),(*ImmCon),++(*ImmBuild));
    sem_post(fileWriting);
    sem_post(noJudge);

    //after enters imm has to checks
    sem_wait(mutex);
    sem_wait(fileWriting);
    fprintf(file,"%d\t : IMM %d	: checks	   \t\t\t: %d\t: %d\t: %d\n",++(*ActionCount),id,(*ImmEnt),++(*ImmCon),(*ImmBuild));
    sem_post(fileWriting);

    //if judge is inside and number of entries is equal to confirmed imms - send signal that imms are ready to confirmation
    if(((*Judge)==1) && ((*ImmEnt) == (*ImmCon)))
    {
        sem_post(allSignedIn);
    }
    // else send signal to judge that some imms/imm have to checks
    else
    {
        sem_post(mutex);
    }
    //waiting for confirmation
    sem_wait(confirm);
    sem_wait(fileWriting);
    fprintf(file,"%d\t : IMM %d	: wants certificate  \t\t\t: %d\t: %d\t: %d\n",++(*ActionCount),id,(*ImmEnt),(*ImmCon),(*ImmBuild));
    sem_post(fileWriting);
    
    //imm have to sleep from random time <0,JT> 
    int time = getTime(args,0);
    usleep(time);

    //after wake up
    sem_wait(fileWriting);
    fprintf(file,"%d\t : IMM %d	: got certificate    \t\t\t: %d\t: %d\t: %d\n",++(*ActionCount),id,(*ImmEnt),(*ImmCon),(*ImmBuild));
    sem_post(fileWriting);

    //wait for judge to leave and then leave & exit success
    sem_wait(noJudge);
    sem_wait(leave);
    sem_wait(fileWriting);
    fprintf(file,"%d\t : IMM %d	: leaves	           \t\t: %d\t: %d\t: %d\n",++(*ActionCount),id,(*ImmEnt),(*ImmCon),--(*ImmBuild));
    sem_post(fileWriting);
    sem_post(noJudge);
    exit(0);
}

/**
 * @brief Function represents process of judge, there is only one judge in this project
 * @param InputArgs pointer to structure args
 * @returns void
 **/ 
void procJudge(InputArgs *args)
{
    while(true)
    {
        sem_wait(noJudge);
        sem_wait(mutex);
        sem_wait(fileWriting);
        fprintf(file,"%d\t : JUDGE        : wants to enter\n",++(*ActionCount));
        fprintf(file,"%d\t : JUDGE        : enters	     \t\t\t: %d\t: %d\t: %d\n",++(*ActionCount),(*ImmEnt),(*ImmCon),(*ImmBuild));
        sem_post(fileWriting);
        ++(*Judge); // judge = 1
        // if number of imms which entry bigger then number of imms which check -> judge waits for 
        if((*ImmEnt) > (*ImmCon))
        {
            sem_post(mutex);
            sem_wait(fileWriting);
            fprintf(file,"%d\t : JUDGE        : waits for imm          \t\t: %d\t: %d\t: %d\n",++(*ActionCount),(*ImmEnt),(*ImmCon),(*ImmBuild));
            sem_post(fileWriting);
            sem_wait(allSignedIn);
        }
        // else judge starts confirmation
        sem_wait(fileWriting);
        fprintf(file,"%d\t : JUDGE        : starts confirmation    \t\t: %d\t: %d\t: %d\n",++(*ActionCount),(*ImmEnt),(*ImmCon),(*ImmBuild));
        sem_post(fileWriting);

        // sleep random time  <0,JT>
        int time = getTime(args,1);
        usleep(time);

        // if some imms are inside and also checked
        if(((*ImmEnt) > 0) && ((*ImmCon) > 0))
        {
            //shared variable to number of imms can leave after judgement
            (*Conleave) = (*ImmCon);

            // shared variable of total confirmed imms
            (*TotalConfirmed) += (*Conleave);

            // set new values of imms inside and checked == 0
            (*ImmEnt) = 0;
            (*ImmCon)= 0;
        }
        // end of confirmation
        sem_wait(fileWriting);
        fprintf(file,"%d\t : JUDGE        : ends confirmation      \t\t: %d\t: %d\t: %d\n",++(*ActionCount),(*ImmEnt),(*ImmCon),(*ImmBuild));
        sem_post(fileWriting);

        //sleep random <0,JT>
        int time2 = getTime(args,1);
        usleep(time2);

        sem_wait(fileWriting);
        fprintf(file,"%d\t : JUDGE        : leaves	\t\t\t: %d\t: %d\t: %d\n",++(*ActionCount),(*ImmEnt),(*ImmCon),(*ImmBuild));
        sem_post(fileWriting);

        // judge is not inside
        (*Judge)=0;
        sem_post(mutex);
        sem_post(noJudge);
        // while conleave !=0 cycle to send signals to imms that they are confirmed and can leave if there is no judge inside building
        while((*Conleave) > 0)
        {
            sem_post(confirm);
            sem_post(leave);
            (*Conleave)--;
        }
        // if judge confirmed all of imms
        if(((*TotalConfirmed) == args->ProcI))
        {
            sem_wait(fileWriting);
            fprintf(file,"%d\t : JUDGE        : finishes\n",++(*ActionCount));
            sem_post(fileWriting);
            return;
        }
        // else sleep random time <0,JG>
        else
        {
            int time3 = getTime(args,3);
            usleep(time3);
        }
    } 
}
