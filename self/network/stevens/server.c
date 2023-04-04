/*
    :: TCP
    :: Provide timestamp in string format
    :: 1.01
        - CLI options parsing
        - prepare environment, no client communication

    $ ./__s --port=<port>
*/

#include "lib.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void print_start_state()
{
    CONSOLE("\n # ----- SERVER ----- #  \n");
}


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */

int main(int argc, char** argv)
{
    print_start_state();

    return 0;
}
