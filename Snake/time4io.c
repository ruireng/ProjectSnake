#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void)
{
    return((PORTD >> 8) & 0x0000f);     // shift PORTD 8 bits right, then mask the 4 LSB
}

int getbtns(void)
{
    return((PORTD >> 5) & 0x00007);     // shift PORTD 5 bits right, then mask the 3 LSB (button2-4)
}

int getbtnone(void)
{
    return(PORTF & 0x02);               // mask bit index 1 (button1)
}