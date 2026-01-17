#include <xc.h>
#include "oven.h"
#include "builtins.h"

#define _XTAL_FREQ 20000000UL

// Simple beep function for basic buzzer (count = number of beeps)
void beep(unsigned char count) {
    unsigned char i;
    for (i = 0; i < count; i++) {
        BUZZER = 1;
        __delay_ms(100);
        BUZZER = 0;
        if (i < count - 1) {
            __delay_ms(100);  // Gap between beeps
        }
    }
}
