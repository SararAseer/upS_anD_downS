#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>

union semun{
	int val;
	struct semid_ds * buf;
	unsigned short * array;
};

char * remove_n(char * entry){
  for(int i=0; i < strlen(entry); i++){
    if(entry[i] == '\n'){
      entry[i]='\0';
    }
  }
  return entry;
}

void view(){
  int f = open("story", O_RDONLY, 0666);
  char * story = calloc(1, 10000);
  read(f, story, 10000);
  printf("The story: \n %s\n", story);
  close(f);
}


void create(){
  int f = open("story", O_CREAT | O_TRUNC, 0666);
  int key = ftok(".", 'b');
  int shmid = shmget(key, 1000, IPC_CREAT | IPC_EXCL | 0666);
  int sem = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
  union semun val;
  val.val = 1;
  semctl(sem, 0, SETVAL, val);
}

void rem(){
    int key = ftok(".", 'b');
    int sem = semget(key, 1, 0666);
    struct sembuf ss;
    ss.sem_op = -1;
    ss.sem_num = 0;
    ss.sem_flg = 0;
    semop(sem, &ss, 1);
    semctl(sem, 0, IPC_RMID, 0);
    view();
    remove("story");
    int shmid = shmget(key, 1000, 0666);
    shmctl(shmid, IPC_RMID, NULL);
}

void check(){
  char input[100];
    fgets(input, 500,stdin);
    remove_n(input);
    if(strcmp(input, "exit") == 0){
      exit(0);
    }
    if(strcmp(input, "-v") == 0){
      view();
    }
    if(strcmp(input, "-r") == 0){
      rem();
    }
    if(strcmp(input, "-c") == 0){
      create();
    }
  
}



int main(int argc, char * argv[]){
   if(argc == 1){
        printf("Pleae use mode -c,-r, or -v\n");
        exit(1);
    }
    if(strcmp(argv[1], "-c") == 0){
        create();
    }
    if(strcmp(argv[1], "-r") == 0){
        rem();
    }
    if(strcmp(argv[1], "-v") == 0){
        view();
    }
}
