#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void PoorStudent(int[]);
void DearOldDad(int[]);

int main(int argc, char *argv[])
{
     int ShmID;
     int *ShmPTR;
     pid_t pid;
     int status;

     srandom(getpid()); 

     if (argc != 3)
     {
          printf("Use: %s 0 0\n", argv[0]); 
     }

     ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0)
     {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }

     printf("Server has received a shared memory of two integers...\n");


     ShmPTR = (int *)shmat(ShmID, NULL, 0);

     if (*ShmPTR == -1)
     {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server attached shared memory...\n");

     ShmPTR[0] = atoi(argv[1]); 
     ShmPTR[1] = atoi(argv[2]);

     printf("Server has filled %d %d in shared memory...\n",
            ShmPTR[0], ShmPTR[1]); 

     printf("Server is about to fork a child process...\n");

     pid = fork(); 

     if (pid < 0)
     {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0)
     {                         
          PoorStudent(ShmPTR); 
          exit(0);
     }
     else
     {
          DearOldDad(ShmPTR);
     }

     wait(&status);
     printf("Server's child is finished...\n");
     shmdt((void *)ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}

void DearOldDad(int SharedMem[])
{
     int i = 0;
     int account = SharedMem[0];
     int amount;

     for (i = 0; i < 25; i++)
     {
          sleep(random() % 5); 
          while (SharedMem[1] != 0)
          {    
          }
          account = SharedMem[0];

          if (account <= 100)
          {
               amount = random() % 100; 
               if (amount % 2 == 0)
               {
                    account = account + amount;
                    printf("Dear old Dad - Deposits -> $%d / Balance -> $%d\n", amount, account);
               }
               else
               {
                    printf("Dear old Dad - No money to give\n");
               }
          }
          else
          {
               printf("Dear old Dad - Student has enough money ($%d)\n", account);
          }

          SharedMem[0] = account; 
          SharedMem[1] = 1;           
     }
}

void PoorStudent(int SharedMem[])
{
     int i = 0;
     int account = SharedMem[0];
     int amountNeed;

     for (i = 0; i < 25; i++)
     {
          sleep(random() % 5); 

          while (SharedMem[1] != 1)
          {
          }

          account = SharedMem[0];
          amountNeed = random() % 50;
          printf("Poor Student - Needs -> $%d\n", amountNeed);

          if (amountNeed <= account)
          {
               account = account - amountNeed; 
               printf("Poor Student - Withdraws -> $%d  Account Balance -> $%d\n", amountNeed, account);
          }
          else
          {
               printf("Poor Student - Not enough money ($%d)\n", account);
          }

          SharedMem[0] = account;
          SharedMem[1] = 0;
     }
}