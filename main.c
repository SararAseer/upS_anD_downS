#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>


int main(){
    int key = ftok(".", 'b');
    int sem = semget(key, 1, 0666);	
    int shmid = shmget(key, 1000, 0666);
    char * line = shmat(shmid, (void *) 0, 0);
    int f = open("story", O_WRONLY | O_APPEND, 0666);	
    struct sembuf fl;
    fl.sem_op = -1;
    fl.sem_num = 0;
    fl.sem_flg = SEM_UNDO;
    semop(sem,&fl,1);

    int size = line[0];
    if (!size)
      printf("Start  story \n");
    else{
      char * last = &line[1];
      printf("last line: %s\n",last);
      printf("Continue the story \n");
    }

    char input[200];
    fgets(input,200,stdin);

    int i = 0;
    while(input[i]){
      i++;
      size = i;
      line[0] = size; 
      strcpy(&line[1],input);
      write(f, line, 1024);
      shmdt(line);  
      fl.sem_op = 1;
      semop(sem, &fl, 1);
      close(f);
      return 0;
    }
}
