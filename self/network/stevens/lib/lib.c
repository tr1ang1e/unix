#include "lib.h"


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

const char* ip = "127.0.0.1";       // loopback
int port = 1111;                    // random


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef enum CliOptionState
{
    OPT_DEFAULT = 100,
    OPT_SPECIFIED = 101
} CliOptionState;

typedef enum CliOptions
{
    OPT_IP = 0,
    OPT_PORT,

    OPT_MAX
} CliOptions;


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void parse_cli_args(int argc, char** argv)
{
    /*
        External variables:
            char* optarg  :  opt value if presents, NULL otherwise 
            int   optind  :  ...
            int   opterr  :  ...
            int   optopt  :  ...
    */
    
    int flag;    
    struct option cliOpts[] = 
    {
        [ OPT_IP   ] = { "ip"   , required_argument , &flag , OPT_SPECIFIED },
        [ OPT_PORT ] = { "port" , required_argument , &flag , OPT_SPECIFIED },
        [ OPT_MAX  ] = {      0 ,                 0 ,     0 ,             0 }    // indicates end
    };

    int rc = 0;
    int optIndex = 0;   // for long opts only = opt index in cliOpts[]

    while (true)
    {
        flag = OPT_DEFAULT;
        
        rc = getopt_long(argc, argv, "", cliOpts, &optIndex);
        if (-1 == rc)
        {
            // end of parcing
            break;
        } 

        switch (rc)
        {
        case 0:  
        {
            /*
                Processing long options with 'flag' param is not NULL
                Flag value:
                    - is set to 'value' if option is presented = OPT_SPECIFIED
                    - keep the current value if option is absent = OPT_DEFAULT
            */
            
            if (OPT_SPECIFIED == flag)
            {
                switch (optIndex)
                {
                case OPT_IP:
                    ip = optarg;
                    break;

                case OPT_PORT:
                    port = atoi(optarg);
                    break;

                default: 
                    break;
                }
            }
            
        } break;

        default: break;
        }
    }

}