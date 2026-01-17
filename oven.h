/*
 * Oven UI/State definitions and display helpers.
 *
 * Declares screen-state constants and UI rendering functions for power banners,
 * time/temperature input, preheat and countdown displays.
 */
#ifndef OVEN_H
#define	OVEN_H

// Fan Control Pin
#define FAN                 RC2
#define FAN_DDR             TRISC2

// Buzzer Control Pin
#define BUZZER              RC1
#define BUZZER_DDR          TRISC1

// Screen States
#define POWER_SCREEN        0x01
#define MODES_SCREEN        0x02
#define MICRO_MODE          0x03
#define TIME_INPUT          0x04
#define CLOCK_COUNTDOWN     0x05
#define GRILL_MODE          0x06
#define CONVECTION_MODE     0x07
#define QUICK_START_MODE    0x08
#define TEMP_INPUT          0x09
#define PREHEAT_COUNTDOWN   0x0A


extern volatile unsigned int total_seconds;

void display_power_screen(void);
void clear_screen(void);
void display_modes(void);
void display_micro_mode_power(void);
void display_time_screen(void);
void display_time(unsigned char *time_digits, unsigned char cursor_pos);
void display_time_remaining(unsigned int total_seconds);
void display_grill_mode_power(void);
void display_convection_mode_banner(void);
void display_temp_screen(void);
void display_temp(unsigned char *temp_digits, unsigned char cursor_pos);
void display_preheat_banner(void);
void display_preheat_time_remaining(unsigned int total_seconds);
void beep(unsigned char count);
void display_cooking_complete_screen(void);

#endif    /* OVEN_H */