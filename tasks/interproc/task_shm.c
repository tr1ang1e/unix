// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
  int flags = IPC_CREAT;
  int permissions = 384; 

  int key1 = atoi(argv[1]);
  int shm1 = shmget(key1, 1000, flags | permissions);
  int* nums1 = (int*)shmat(shm1, NULL, 0);
  
  int key2 = atoi(argv[2]);
  int shm2 = shmget(key2, 1000, flags | permissions);
  int* nums2 = (int*)shmat(shm2, NULL, 0);
  
  int key = ftok("sharedmemory", 1);
  int shm = shmget(key, 1000, flags | permissions);
  int* nums = (int*)shmat(shm, NULL, 0); 

  int i = 0, count = 100;
  for ( ; i < count; ++i)
  {
    *nums = *nums1 + *nums2;
    ++nums;
    ++nums1;
    ++nums2;
  }

  printf("%d\n", key);
  // shmctl(shm, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}