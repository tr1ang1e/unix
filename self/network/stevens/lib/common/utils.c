/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "utils.h"


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

const char* Getenv(const char* name, const char* value)
{
    const char* env = getenv(name);
    return env ? env : value;
}

void wait_for_enter()
{
    __console("> ");
    __unused getchar();
}

ByteOrder get_endiannes()
{
    uint16_t var = 0x0001;
    uint8_t byte = ((uint8_t*)&var)[0];

    ByteOrder endiannes = byte ? L_END : B_END;
    return endiannes;
}
