#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

// manage error output
// #define SUPRESS_GETOPT_ERROR
#ifdef SUPRESS_GETOPT_ERROR
  extern int opterr;
  int opterr = 0;
#endif          

/* Register options

    struct option
    {
      const char *name;     // option full name without -- (this identifier would be parsed automatically)
      int has_arg;          // in fact is bool = are there any option arguments
      int *flag;            // variable to keep value if given option is flag
      int val;              // value to return - or - to set to flag variable if given option is flag
    };

*/
struct option opts[] =  // array to register options
{
  // register options
  { "query"                  , required_argument , NULL , 'q' },            
  { "longinformationrequest" , no_argument       , NULL , 'i' },
  { "version"                , no_argument       , NULL , 'v' },

  // must ends with empty item
  {0, 0, 0, 0}                
};


int main(int argc, char** argv)
{

  /* getopt_long() description
   * 
   * Signature = getopt_long(argc, argv, shortopts, longopts, longindex)
   *   argc      :  int              :  number of arguments in given array 
   *   argv      :  char**           :  the array with arguments 
   *   shortopts :  const char*      :  string wich describes all available short ('-') options (should be "" if no short args)
   *   longopts  :  struct options*  :  array of registered long ('--') options
   *   longindex :  int*             :  NULL or variable to keep index of longindex which was just handled
   *  
   * General flow (abstract, for details see 'man 3 getopt') =
   *
   * 1. takes command line argument
   * 2. desides whether it short or long option
   *      (A) if short, try to fing in 'shortopts' argument and return this option character
   *      (B) if long, try to find in 'longopts' argument (full or partitial matching) and return:
   *            > if 'flag' is NULL, return 'val'
   *            > otherwise return 0 and set 'flag' variable to 'val'
   * 3. if doesn't find, returns error code corresponding to the error type (and print error message) 
   *       : = if required option parameter was not found && ':' symbol is on the [0] place in 'shortopts'
   *       ? = if ...
   *              required option parameter was not found
   *              option character is not in 'shortopts'
   *              long option is not in 'longopts'
   *              long option ambiguous match
   * 4. when parsing is ended (naturally or forced by found '--' without really option) returns -1
   * 
   * */

  int longOptIdx = 0;
  char isCorrect = '+'; 
  int result = 0;
  while(1)
  {
    result = getopt_long(argc, argv, "q:iv", opts, &longOptIdx);
    
    // end of parcing
    if (result == -1)
    {
      break;
    }

    // handle parced option
    switch(result)
    {
      // nothing to do if options are correct
      case  0 :   // not necessary in current task (just for example when 'flag' is not NULL in one of the long options)
      case 'q':
      case 'i':
      case 'v':
        break;
      
      // do this if error occurs
      default:
        isCorrect = '-';
        break;
    }
  }

  // answer
  printf("%c\n", isCorrect);

  return 0;
}