#include "clcd.h"
#include <builtins.h>
#include <xc.h> 
#include "oven.h"
#include "timers.h"
#include "matrix_keypad.h"

/*
 * Application entry and screen-state machine.
 *
 * Initializes peripherals and drives the FSM across modes:
 * Power → Menu → (Micro/Grill/Convection/Start). Uses sparse LCD updates
 * and Timer2 ISR for real-time countdowns. Keypad STATE mode for controls.
 */

#pragma config WDTE = OFF    // Watchdog Timer Enable bit (WDT disabled)

// Global array to store time digits: [Min1, Min2, Sec1, Sec2]
unsigned char time_digits[4] = {0, 0, 0, 0}; 
unsigned char pos = 0; // Current cursor position for time (0-3)
unsigned char temp_digits[3] = {0, 0, 0};
unsigned char temp_pos = 0; // Current cursor position for temperature (0-2)
volatile unsigned int timer2_counter = 0; // Counts up to 1250 (updated in ISR)
volatile unsigned int total_seconds = 0; // The total seconds remaining (updated in ISR)
unsigned int last_total_seconds = 0; // For reducing LCD writes (non-ISR)

static void init_config(void) {
    init_timer2();
    init_clcd();
    init_matrix_keypad();
    FAN_DDR = 0; // Set FAN pin as output
    FAN = 0;     // Turn off FAN initially
}

void __interrupt() isr(void)
{   
    // Timer2 ISR: accumulate ticks and convert to ~1s events
    if (TMR2IF == 1)
    {
        // Count the interrupts (ticks)
        timer2_counter++;
        
        // Every 1250 ticks (~1s), decrement the countdown if running
        if (timer2_counter == 1250)
        {
            if (total_seconds > 0)
            {
                total_seconds--;
            }
            timer2_counter = 0; // reset for next second
        }
        
        // Clear the Interrupt Flag (Critical!)
        TMR2IF = 0;
    }
}

int main(void) {
    init_config();
    unsigned char screen_flag = POWER_SCREEN;
    unsigned char key = ALL_RELEASED;  // Declare key here for use in all cases

while (1) 
    {
        switch (screen_flag) {

            case POWER_SCREEN:
                // Splash screen; blocking delay acceptable for startup UX
                display_power_screen();
                __delay_ms(1000);
                
                clear_screen();
                
                // Pre-draw the menu before switching to avoid flicker
                display_modes();
                
                screen_flag = MODES_SCREEN;
                break;

            case MODES_SCREEN:
                // Just scan the keypad
                key = read_matrix_keypad(STATE);

                FAN = 0; // Ensure FAN is off in menu
                
                if (key == 1) {

                    screen_flag =  MICRO_MODE;
                    
                }
                else if (key == 2) {
                    
                    screen_flag = GRILL_MODE;
                }
                else if (key == 3) {
                    screen_flag = CONVECTION_MODE;
                }
                else if (key == 4) {
                    screen_flag = QUICK_START_MODE;
                }
                break;

            case MICRO_MODE:
                    clear_screen();
                    display_micro_mode_power();
                    __delay_ms(1000);
                    clear_screen();
                    display_time_screen();
                    // Initialize time input
                    for (unsigned char i = 0; i < 4; i++) {
                        time_digits[i] = 0;
                    }
                    pos = 0;
                    screen_flag = TIME_INPUT;
                    break;

            case GRILL_MODE:
                    // Clear screen and display grill mode power
                    display_grill_mode_power();
                    __delay_ms(1000);
                    // After displaying, goto time input screen
                    clear_screen();
                    display_time_screen();
                    // Initialize time input
                    for (unsigned char i = 0; i < 4; i++) {
                        time_digits[i] = 0;
                    }
                    pos = 0;
                    screen_flag = TIME_INPUT;
                    break;

            case CONVECTION_MODE:
                    display_convection_mode_banner();
                    __delay_ms(500);
                    // Move to temperature input screen
                    clear_screen();
                    display_temp_screen();
                    for (unsigned char i = 0; i < 3; i++) {
                        temp_digits[i] = 0;
                    }
                    temp_pos = 0;
                    screen_flag = TEMP_INPUT;
                    break;

                case TIME_INPUT:
                    // Display current time input with cursor on active position
                    display_time(time_digits, pos);

                    FAN = 0; // Ensure FAN is off during time input
                    
                    key = read_matrix_keypad(STATE);
                    
                    if (key >= 1 && key <= 9 && pos < 4) {
                        // Valid digit key (1-9) and space available
                        time_digits[pos] = key;
                        pos++;
                    }
                    else if ((key == (unsigned char)0) && pos < 4) {
                        // Key 0 also valid for time input
                        time_digits[pos] = 0;
                        pos++;
                    }
                    else if (key == (unsigned char)'*') { 
                        // '*' key for CLEAR - reset all time digits and position
                        for (unsigned char i = 0; i < 4; i++) {
                            time_digits[i] = 0;
                        }
                        pos = 0;
                    }
                    else if (key == (unsigned char)'#') { 
                        // '#' key for CONFIRM - compute total seconds and start cooking
                        total_seconds = (unsigned int)(time_digits[0] * 600
                                        + time_digits[1] * 60
                                        + time_digits[2] * 10
                                        + time_digits[3]);
                        timer2_counter = 0;
                        TMR2IF = 0;
                        TMR2ON = (total_seconds > 0) ? 1 : 0;

                        // Pre-draw Cooking screen before switching state (reduce flicker)
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); // hide input cursor
                        clcd_print("Time Rem:", LINE1(1));
                        clcd_print("4. Start", LINE2(2));
                        clcd_print("5. Pause", LINE3(2));
                        clcd_print("6. Stop", LINE4(2));
                        display_time_remaining(total_seconds);
                        last_total_seconds = total_seconds;

                        screen_flag = CLOCK_COUNTDOWN;
                        break;
                    }
                    
                    break; 

            case TEMP_INPUT:
                    // Display temperature input
                    display_temp(temp_digits, temp_pos);

                    FAN = 0; // Ensure FAN is off during temperature input

                    key = read_matrix_keypad(STATE);

                    if (key >= 1 && key <= 9 && temp_pos < 3) {
                        temp_digits[temp_pos] = key;
                        temp_pos++;
                    }
                    else if ((key == (unsigned char)0) && temp_pos < 3) {
                        temp_digits[temp_pos] = 0;
                        temp_pos++;
                    }
                    else if (key == (unsigned char)'*') {
                        for (unsigned char i = 0; i < 3; i++) {
                            temp_digits[i] = 0;
                        }
                        temp_pos = 0;
                    }
                    else if (key == (unsigned char)'#') {
                        // Confirm temperature; begin preheating (placeholder 30s)
                        unsigned int temp_value = (unsigned int)(temp_digits[0] * 100
                                                + temp_digits[1] * 10
                                                + temp_digits[2]);
                        (void)temp_value; // currently unused placeholder

                        total_seconds = 30; // Preheat duration placeholder
                        timer2_counter = 0;
                        TMR2IF = 0;
                        TMR2ON = (total_seconds > 0) ? 1 : 0;

                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        display_preheat_banner();
                        display_preheat_time_remaining(total_seconds);
                        last_total_seconds = total_seconds;

                        screen_flag = PREHEAT_COUNTDOWN;
                    }
                    break;
                    
            case CLOCK_COUNTDOWN:
                // Only update the time field when it changes to reduce flicker
                if (total_seconds != last_total_seconds) {
                    display_time_remaining(total_seconds);
                    last_total_seconds = total_seconds;
                }

                // Read keys for controls
                key = read_matrix_keypad(STATE);
                
                // Control FAN based on timer state
                if (TMR2ON == 1) {
                    FAN = 1; // Turn on FAN only when timer is running
                } else {
                    FAN = 0; // Turn off FAN when paused or stopped
                }

                // Handle Keys (Start/Resume, Pause, Stop)
                if (key == 4) { // Start/Resume or +30s
                    if (total_seconds > 0 && TMR2ON == 0) {
                        TMR2ON = 1; // Resume if paused
                    } else if (TMR2ON == 1) {
                        total_seconds += 30; // Add 30 seconds when running
                    }
                } else if (key == 5) { // Pause
                    TMR2ON = 0; // Pause timer (FAN will turn off via TMR2ON check)
                } else if (key == 6) { // Stop
                    TMR2ON = 0;
                    FAN = 0;
                    clear_screen();
                    display_modes();
                    screen_flag = MODES_SCREEN;
                    break;
                }

                // If done, stop timer and return to menu
                if (total_seconds == 0) {
                    TMR2ON = 0;
                    FAN = 0;    // Turn off FAN when countdown completes
                    clear_screen();
                    display_modes();
                    screen_flag = MODES_SCREEN;
                }
                break;

            case PREHEAT_COUNTDOWN:
                // Update preheat countdown
                if (total_seconds != last_total_seconds) {
                    display_preheat_time_remaining(total_seconds);
                    last_total_seconds = total_seconds;
                }

                FAN = 1; // Turn on FAN during preheating

                key = read_matrix_keypad(STATE);

                if (key == 4) {
                    if (total_seconds > 0) TMR2ON = 1;
                } else if (key == 5) {
                    TMR2ON = 0;
                } else if (key == 6) {
                    TMR2ON = 0;
                    clear_screen();
                    display_modes();
                    screen_flag = MODES_SCREEN;
                    break;
                }

                if (total_seconds == 0) {
                    TMR2ON = 0;
                    // After preheating, go to time input for cooking
                    clear_screen();
                    display_time_screen();
                    for (unsigned char i = 0; i < 4; i++) {
                        time_digits[i] = 0;
                    }
                    pos = 0;
                    screen_flag = TIME_INPUT;
                }
                break;

            case QUICK_START_MODE:
                    // Quick start with 30 seconds, then proceed to countdown
                    total_seconds = 30;
                    timer2_counter = 0;
                    TMR2IF = 0;
                    TMR2ON = 1;
                    clear_screen();
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    clcd_print("Time Rem:", LINE1(1));
                    clcd_print("4. +30s", LINE2(2));
                    clcd_print("5. Pause", LINE3(2));
                    clcd_print("6. Stop", LINE4(2));
                    display_time_remaining(total_seconds);
                    last_total_seconds = total_seconds;
                    screen_flag = CLOCK_COUNTDOWN;
                    break;
        }
    }
    return 0;

}