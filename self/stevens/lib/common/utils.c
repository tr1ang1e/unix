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

int wait_for_enter(const char* prompt)
{
    __console("%s", prompt ? prompt : "");
    static char input[2] = { 0 };
    __unused fgets(input, sizeof(input), stdin);
    __unused fflush(stdout);

    return input[0];
}

void clear_n_chars(size_t n)
{
    if (0 == n)
    {
        putchar('\r');
        printf("%-" VSTR(CONSOLE_LINE) "s", "");
        putchar('\r');
    }
    else
    {
        for (int i = 0; i < n; ++i)
            putchar('\b');       
        for (int i = 0; i < n; ++i)
            putchar(' ');
        for (int i = 0; i < n; ++i)
            putchar('\b');
    }
}

ByteOrder get_endiannes()
{
    uint16_t var = 0x0001;
    uint8_t byte = ((uint8_t*)&var)[0];

    ByteOrder endiannes = byte ? L_END : B_END;
    return endiannes;
}

uint64_t get_time_ms()
{
    struct timespec tms;
	__unused clock_gettime(CLOCK_MONOTONIC, &tms);

	uint64_t ms = 0;
    ms += ((uint64_t)tms.tv_sec) * MS_PER_SEC;
	ms += tms.tv_nsec / NS_PER_MS;

    return ms;
}

int get_proc_value(const char* path, handle_proc_value callback, void* args)
{
    int result = -1;
    FILE* file = NULL;
    static char value[MAXLINE];
    
    do
    {
        file = fopen(path, "r");
        if (NULL == file) 
            break;
             
        char* rc = fgets(value, sizeof(value), file);
        if (NULL == rc) 
            break;

        callback(value, args);
        result = 0;
    } while (0);
    
    if (file)
        fclose(file);

    return result;
}

void single_token_to_num(const char* token, void* result)
{
    *((int*)result) = atoi(token);
}

void serialize_coordinates(void* dest, const Coordinates* src)
{
    size_t memberLen = sizeof(int32_t);
    int32_t temp;

    size_t offset = 0;      temp = htonl(src->x);   memcpy(dest + offset, &temp, memberLen);
    offset += memberLen;    temp = htonl(src->y);   memcpy(dest + offset, &temp, memberLen);
    offset += memberLen;    temp = htonl(src->z);   memcpy(dest + offset, &temp, memberLen);
    offset += memberLen;    ((char*)dest)[offset] = '\n';    // protocol implementation
}

void deserialize_coordinates(Coordinates* dest, const void* src)
{
    size_t offset = 0;      dest->x = ntohl(*(((uint32_t*)src) + offset));
    offset += 1;            dest->y = ntohl(*(((uint32_t*)src) + offset));
    offset += 1;            dest->z = ntohl(*(((uint32_t*)src) + offset));
}
