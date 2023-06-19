/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "cli.h"


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
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

SideType __whoami = SIDE_UNSPEC;        // should be cpecified

// const char* __ip = "127.0.0.1";      // loopback
const char* __ip = "185.186.246.53";    // hostry.com
in_port_t __port = 1111;                // random


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

static void parse_cli_args(int argc, char** argv);
static void print_start_state();
static void register_sighandlers(SideType side);


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void init(SideType side, int argc, char** argv)
{
    __whoami = side;
    parse_cli_args(argc, argv);
    print_start_state(side);
    register_sighandlers(side);
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
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
                switch (optIndex | __whoami)
                {
                case OPT_IP | SIDE_CLIENT:
                    __ip = optarg;
                    break;

                case OPT_PORT | SIDE_CLIENT:
                case OPT_PORT | SIDE_SERVER:
                    __port = atoi(optarg);
                    break;

                default: 
                    error("Wrong CLI option '%s'", cliOpts[optIndex].name);
                    break;
                }
            }
            
        } break;

        default: break;
        }
    }
}

void print_start_state()
{
    switch (__whoami)
    {
    case SIDE_CLIENT:
    {
        __console("  CLIENT ...........    \n");
        __console("  Server to connect:    \n");
        __console("    address = %s:%d     \n", __ip, __port);          
        
    } break;

    case SIDE_SERVER:
    {
        __console("  SERVER ...........    \n");
        __console("  Listen port = %d      \n", __port);

    } break;

    default: 
        break;
    }
}

void register_sighandlers(SideType side)
{
    switch (side)
    {
    case SIDE_CLIENT:
    case SIDE_SERVER:
		__unused sigaction(SIGPIPE, &__sigIgnorance, NULL);
        break;

    default:
        break;
    }
}