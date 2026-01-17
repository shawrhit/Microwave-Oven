#include <xc.h>
#include "matrix_keypad.h"

/*
 * Keypad scanning implementation.
 *
 * Initializes RB0-2 as inputs with pullups and RD3-0 as outputs.
 * Scans rows by pulling one LOW at a time and reading columns.
 * `read_matrix_keypad(STATE)` returns a key once per press.
 */

void init_matrix_keypad(void)
{
    /* Setting the Columns as Inputs (RB2 - RB0)*/
    //TRISB = TRISB | 0x07; xxxx xxxx | 0000 0111 =  xxxx x111
    MATRIX_KEYPAD_COLUMN_PORT_DDR = MATRIX_KEYPAD_COLUMN_PORT_DDR | 0x07;  
   
    /* Setting the Rows as Output (RD3 - RD0)*/
    MATRIX_KEYPAD_ROW_PORT_DDR = MATRIX_KEYPAD_ROW_PORT_DDR & 0xF0; // TRISD = TRISD & 0xF0 -> (TRISD0 to TRISD3)
    
    /* Enabling PORTB Pullups for columns */
    nRBPU = 0;
    
    /* Initialize all rows HIGH; scanning will pull each LOW in turn */
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = HI;
    ROW4 = HI;
}

static unsigned char scan_keypad(void)
{
    int i;
    
    // Scan Row1 by pulling LOW and reading columns
    ROW1 = LOW;
    ROW2 = HI;
    ROW3 = HI;
    ROW4 = HI;   
    
    for (i = 100; i--; );
    
    if (COL1 == LOW) { return 1; }
    else if (COL2 == LOW) { return 2; }
    else if (COL3 == LOW) { return 3; }
    
    // Scan Row2
    ROW1 = HI;
    ROW2 = LOW;
    ROW3 = HI;
    ROW4 = HI;   
    
    for (i = 100; i > 0; i--) { ; }
    
    if (COL1 == LOW) { return 4; }
    else if (COL2 == LOW) { return 5; }
    else if (COL3 == LOW) { return 6; }
    
    // Scan Row3
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = LOW;
    ROW4 = HI;   

    for (i = 100; i > 0; i--);
    
    if (COL1 == LOW) { return 7; }
    else if (COL2 == LOW) { return 8; }
    else if (COL3 == LOW) { return 9; }
    
    // Scan Row4
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = HI;
    ROW4 = LOW;   

    for (i = 100; i > 0; i--);
    
    if (COL1 == LOW) { return '*'; }
    else if (COL2 == LOW) { return 0; }
    else if (COL3 == LOW) { return '#'; }
    
    return ALL_RELEASED;
}

unsigned char read_matrix_keypad(unsigned char mode) // LEVEL STATE 
{
    static unsigned char once = 1; // Debounce gate for STATE mode
    unsigned char key;
    
    key = scan_keypad(); // 1..9, 0, '*', '#', or ALL_RELEASED
    
    if (mode == LEVEL)
    {
        return key; // raw, continuous state
    }
    else // STATE: return one event per press
    {
        if ((key != ALL_RELEASED) && once)
        {
            once = 0;
            return key;
        }
        else if (key == ALL_RELEASED)
        {
            once = 1; // arm for next press
        }
    }
    
    return ALL_RELEASED;
}
