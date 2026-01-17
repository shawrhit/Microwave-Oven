#include <xc.h>
#include "oven.h"
#include "clcd.h"
#include "builtins.h"

/*
 * Oven UI rendering helpers.
 *
 * Contains functions to draw power screens, time/temperature input,
 * and countdown/preheat displays. Uses CLCD HAL for placement.
 */



void display_power_screen(void)
{
    int i;
    
    /* Line 1: Display 16 solid blocks */
    /* 0xFF is the standard hex code for a solid block character */
    for (i = 0; i < 16; i++)
    {
        clcd_putch(0xFF, LINE1((unsigned char)i));
        __delay_ms(50);  // Small delay for visual effect
    }
    
    /* Line 2: Display "Powering ON" centered */
    /* "Powering ON" is 11 chars. (16-11)/2 = 2.5 -> Start at index 2 */
    clcd_print("  Powering ON   ", LINE2(0));

    __delay_ms(500); // Brief pause before next line

    /* Line 3: Display "Microwave Oven" centered */
    /* "Microwave Oven" is 14 chars. (16-14)/2 = 1 -> Start at index 1 */
    clcd_print(" Microwave Oven ", LINE3(0));
    
    /* Line 4: Display 16 solid blocks */
    for (i = 0; i < 16; i++)
    {
        clcd_putch(0xFF, LINE4((unsigned char)i));
        __delay_ms(50);  // Small delay for visual effect
    }
}

void clear_screen(void)
{
    // Send the Clear command (0x01) in INSTRUCTION Mode
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    
    // LCD needs a bit of time to physically clear pixels
    __delay_us(500);
}

void display_modes(){
    clear_screen();
    clcd_print("1. Micro", LINE1(0));
    clcd_print("2. Grill", LINE2(0));
    clcd_print("3. Convection", LINE3(0));
    clcd_print("4. Start", LINE4(0));
}

void display_micro_mode_power(void){
    clear_screen();
    clcd_print("Microwave Mode", LINE2(1));
    clcd_print("Power = 900W", LINE3(2));
}

void display_grill_mode_power(void){
    clear_screen();
    clcd_print("Grill Mode", LINE2(3));
    clcd_print("Power = 1200W", LINE3(1));
}

void display_time_screen(void){
    clcd_print("Set Time (MM:SS)", LINE1(0));
    clcd_print("Time - 00:00", LINE2(2));
    clcd_print("*:CLEAR", LINE4(0));
    clcd_print("#:SET", LINE4(11));
}

// Helper to display time input in MM:SS format with optional cursor
// cursor_pos: 0-3 for which digit to highlight, 0xFF to disable cursor, cursor blinks
void display_time(unsigned char *time_digits, unsigned char cursor_pos)
{
    // Line 2 format: "Time - MM:SS"
    // Positions: MM at indices 9-10, colon at 11, SS at indices 12-13
    
    // Display MM (minutes)
    if (cursor_pos == 0) {
               clcd_putch('_', LINE2(9)); 
               clcd_putch(('0' + time_digits[0]), LINE2(9)); // Min Tens
    } else {
        clcd_putch(('0' + time_digits[0]), LINE2(9)); // Min Tens
    }

    if (cursor_pos == 1) {
               clcd_putch('_', LINE2(10)); 
               clcd_putch(('0' + time_digits[1]), LINE2(10)); // Min Ones
    } else {
        clcd_putch(('0' + time_digits[1]), LINE2(10)); // Min Ones
    }

     // Display colon separator
    clcd_putch(':', LINE2(11));

    // Display SS (seconds)
    if (cursor_pos == 2) {
               clcd_putch('_', LINE2(12)); 
               clcd_putch(('0' + time_digits[2]), LINE2(12)); // Sec Tens
    } else {
        clcd_putch(('0' + time_digits[2]), LINE2(12)); // Sec Tens
    }

    if (cursor_pos == 3) {
               clcd_putch('_', LINE2(13));
               clcd_putch(('0' + time_digits[3]), LINE2(13)); // Sec Ones
    } else {
        clcd_putch(('0' + time_digits[3]), LINE2(13)); // Sec Ones
    }
   
}

void display_time_remaining(unsigned int total_seconds)
{
    unsigned char min, sec;
    
    // Calculate Minutes and Seconds
    min = (unsigned char)(total_seconds / 60);
    sec = (unsigned char)(total_seconds % 60);
    
    // Print Minutes (Tens and Ones)
    clcd_putch((min / 10) + '0', LINE1(10)); // Index 7 (matches "TIME = ")
    clcd_putch((min % 10) + '0', LINE1(11));
    
    // Print Colon
    clcd_putch(':', LINE1(12));

    // Print Seconds (Tens and Ones)
    clcd_putch((sec / 10) + '0', LINE1(13));
    clcd_putch((sec % 10) + '0', LINE1(14));
}

void display_convection_mode_banner(void){
    clear_screen();
    clcd_print("Convection", LINE2(3));
    clcd_print("Mode", LINE3(6));
}

void display_temp_screen(void){
    clcd_print("Set Temp (C)", LINE1(2));
    clcd_print("Temp - 000C", LINE2(2));
    clcd_print("*:CLEAR", LINE4(0));
    clcd_print("#:SET", LINE4(11));
}

// Display temperature input as 3 digits (e.g., 180C)
void display_temp(unsigned char *temp_digits, unsigned char cursor_pos)
{
    // Positions: digits at LINE2(9), LINE2(10), LINE2(11); 'C' at LINE2(12)
    if (cursor_pos == 0) {
        clcd_putch('_', LINE2(9));
        clcd_putch(('0' + temp_digits[0]), LINE2(9));
    } else {
        clcd_putch(('0' + temp_digits[0]), LINE2(9));
    }

    if (cursor_pos == 1) {
        clcd_putch('_', LINE2(10));
        clcd_putch(('0' + temp_digits[1]), LINE2(10));
    } else {
        clcd_putch(('0' + temp_digits[1]), LINE2(10));
    }

    if (cursor_pos == 2) {
        clcd_putch('_', LINE2(11));
        clcd_putch(('0' + temp_digits[2]), LINE2(11));
    } else {
        clcd_putch(('0' + temp_digits[2]), LINE2(11));
    }

    clcd_putch('C', LINE2(12));
}

void display_preheat_banner(void){
    clear_screen();
    clcd_print("Preheating", LINE1(3));
    clcd_print("Time Rem.:", LINE2(3));
}

void display_preheat_time_remaining(unsigned int total_seconds)
{
    unsigned char min, sec;
    
    // Calculate Minutes and Seconds
    min = (unsigned char)(total_seconds / 60);
    sec = (unsigned char)(total_seconds % 60);
    
    // Print Minutes (Tens and Ones)
    clcd_putch((min / 10) + '0', LINE3(5)); 
    clcd_putch((min % 10) + '0', LINE3(6));
    
    // Print Colon
    clcd_putch(':', LINE3(7));
    // Print Seconds (Tens and Ones)
    clcd_putch((sec / 10) + '0', LINE3(8));
    clcd_putch((sec % 10) + '0', LINE3(9));
}

void display_cooking_complete_screen(void)
{
    int i;
    int j;
    
    // Draw top border: Line 1 filled with solid block character (0xFF)
    // Creates a decorative frame that provides visual feedback for cooking completion
    for (i = 0; i < 16; i++)
    {
        clcd_putch(0xFF, LINE1((unsigned char)i));
    }

    // Draw bottom border: Line 4 filled with solid block character (0xFF)
    // Completes the decorative frame around the completion message
    for (i = 0; i < 16; i++)
    {
        clcd_putch(0xFF, LINE4((unsigned char)i));
    }

    // Flash the completion message twice (visual feedback to user that cooking is done)
    // The loop creates a flashing effect: show text for 2 seconds, hide for 1 second, repeat
    for(j = 0; j < 2; j++){

        // Display "Time" on line 2, centered at position 6
        clcd_print("Time", LINE2(6));
        
        // Display "Complete" on line 3, centered at position 4
        clcd_print("Complete", LINE3(4));

        // Keep message visible for 2 seconds
        __delay_ms(2000);

        // Clear both lines 2 and 3 to create the flashing effect
        clcd_print("                ", LINE2(0));
        clcd_print("                ", LINE3(0));
        // Pause 1 second before showing message again (or ending if last loop iteration)
        __delay_ms(1000);

    }

}