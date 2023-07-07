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
/*                        G L O B A L                        */
/* --------------------------------------------------------- */

const char* serversAvailable[SERVER_MAX] = 
{
    [SERVER_LOOPBACK] = "127.0.0.1",
    [SERVER_HOSTRY]   = "185.186.246.53",
    [SERVER_RPI]      = "rpi.local"
};

const char* serversStrings[SERVER_MAX] =
{
    #define X(server) [SERVER_##server] = #server,
        __SERVERS
    #undef X
};


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

SideType __whoami = SIDE_UNSPEC;        // should be cpecified

const char* __ip = "185.186.246.53";    // default
in_port_t __port = 1111;                // random


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

static void parse_cli_args(int argc, char** argv);
static void print_start_state();
static const char* resolve_server_ip(const char* optarg);

// signals handling
static void register_sighandlers(SideType side);
static void handle_sigchld(int signum);


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
                    { 
                        const char* temp = resolve_server_ip(optarg);
                        if (NULL == temp)   warning("Wrong --ip option value: %s", optarg);
                        else                __ip = temp;
                    }
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
        __console("  PID = %d              \n", getpid());
        __console("  Server to connect:    \n");
        __console("    address = %s:%d     \n", __ip, __port);          
    } break;

    case SIDE_SERVER:
    {
        __console("  SERVER ...........    \n");
        __console("  PID = %d              \n", getpid());
        __console("  Listen port = %d      \n", __port);
    } break;

    default: 
        break;
    }
}

const char* resolve_server_ip(const char* optarg)
{
    const char* string = optarg;

    /* handle predefined constants */

    for (int i = 0; i < SERVER_MAX; ++i)
    {
        int rc = strcmp(optarg, serversStrings[i]);
        if (0 == rc)
        {
            string = serversAvailable[i]; 
            break;
        }
    }

    /* resolve domain name if necessary */

    static char ipRepr[INET_COMMON_ADDRSTRLEN] = { 0 };
    
    bool result = Sock_getaddrinfo(AF_INET, string, ipRepr);
    if (result) 
        return ipRepr;
    return NULL;
}

/* signals handling */

void register_sighandlers(SideType side)
{
    switch (side)
    {
    case SIDE_CLIENT:
    {
	    __unused Sigaction(SIGPIPE, NULL);
    }
    break;

    case SIDE_SERVER:
    {
	    __unused Sigaction(SIGPIPE, NULL);
        __unused Sigaction(SIGCHLD, handle_sigchld);
    }
    break;

    default:
        break;
    }
}

void handle_sigchld(int signum)
{
    int waitStatus;
    pid_t pid; 
    
    while (true)
    {
        pid = waitpid(-1, &waitStatus, WNOHANG);
        if (0 >= pid)
            break;

        // handle child exit status
        int rc = -1;
        if (WIFEXITED(waitStatus))
            rc = WEXITSTATUS(waitStatus);
    }

    return;
}
