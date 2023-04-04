/*
    :: TCP
    :: Get timestamp in string format from server  
    :: 1.01
        - CLI options parcing
        - prepare environment, no server communication

    $ ./__c --ip=<ip> --port=<port>
*/

#include "lib.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void print_start_state()
{
    CONSOLE("\n # ----- CLIENT ----- #  \n");
    CONSOLE("                           \n");
    CONSOLE("   Server to connect:      \n");
    CONSOLE("     IP = %s               \n", ip);          
    CONSOLE("     port = %d             \n", port); 
    CONSOLE("                           \n");
}


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */

int main(int argc, char** argv)
{
    parse_cli_args(argc, argv);
    print_start_state();

    return 0;
}
