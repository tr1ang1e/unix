#include <stdio.h>

// getppid()
#include <sys/types.h>
#include <unistd.h>

// open(), read()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// strtok()
#include <string.h>

#define fd int

/**********************************************************************************
 *                                     easy way                                   *
 **********************************************************************************/ 

int main()
{
  pid_t ppid = getppid();
  printf("%d\n", ppid);

  return 0;
}


/**********************************************************************************
 *                               read stat file way                               *
 **********************************************************************************/ 

// void get_pid_path (char* buffer, pid_t pid)
// {
//   const char* proc = "/proc/";
//   sprintf(buffer, "%s%d/", proc, pid);
// }

// int main()
// {
//   pid_t pid = getpid();
  
//   char pid_path[256] = { 0 };
//   get_pid_path(pid_path, pid);

//   chdir(pid_path);
//   fd stat = open("./stat", O_RDONLY);
  

//   /* read stat file */


//   /* variant 1 = read directly ppid number
//    *   1. stat file format = tokens splitted by spaces
//    *   2. PPID token is on 4th place (idx = 3)  
//    *   3. read bytes one by one to any buffer until number of read spaces != 3
//    *   4. read bytes ony by one to destination ppid buffer until next space
//    *   5. replace space in ppid buffer with '\0'
//    **/

// /*
//   char read_bytes[256] = { 0 };    // 256 is enough to find PPID field
//   char ppid[10] = { 0 };           // 10 is enough to read PPID field

//   int spaces = 0;                  // number of spaces
//   int idx = 0;                     // index of buffer to write to

//   while (spaces != 3)
//   {
//     read(stat, &read_bytes[idx], 1);
//     if (read_bytes[idx] == ' ')
//       ++spaces;
//     ++idx;
//   }

//   idx = 0;    // reset index to write to ppid buffer
//   while (1)
//   {
//     read(stat, &ppid[idx], 1);
//     if (ppid[idx] == ' ')
//     {
//       ppid[idx] = '\0';
//       break;
//     }
//     ++idx;
//   }

//   printf("%s\n", ppid);    // done
//  */
  

//   /* variant 2 = use strtok() function
//    *   1. read 255 bytes (enough)
//    *   2. use strtok function until token number != 3 
//    * 
//    **/

//   /* uncomment to use varinat 2 

//   char read_bytes[256] = { 0 };
//   read(stat, read_bytes, 100);

//   char* token = strtok(read_bytes, " ");
//   int token_number = 0;

//   while (token_number != 3)
//   {
//     token = strtok(NULL, " ");
//     ++token_number;
//   }
 
//   printf("%s\n", token);

//   */
// }
