/*
 * Mini_Project_2-Interfacing_1_Project.c
 *
 *  Created on: Sep 11, 2024
 *      Author: Yousef
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Global variables for time keeping
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;

// Flags for handling button press debounce
unsigned char flag_HOURS_INC_Handle = 0;
unsigned char flag_HOURS_DEC_Handle = 0;
unsigned char flag_MINUTES_INC_Handle = 0;
unsigned char flag_MINUTES_DEC_Handle = 0;
unsigned char flag_SECONDS_INC_Handle = 0;
unsigned char flag_SECONDS_DEC_Handle = 0;

unsigned char Toggle_Mode_Flag = 0;  // Flag for toggling between count modes
unsigned char Count_Flag = 0;        // Flag to indicate count up or down mode

// Initialize Timer1 in CTC mode for 1-second interrupts
void Timer1_CTC_Init(void)
{
	TCNT1 = 0;                        // Initialize Timer1 counter
	OCR1A = 15624;                    // Compare value for 1 second (assuming 16 MHz clock and 1024 prescaler)
	TIMSK |= (1<<OCIE1A);             // Enable output compare match interrupt for Timer1
	TCCR1A = (1<<FOC1A);              // Force output compare for Channel A
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);  // CTC mode, prescaler 1024
	sei();                            // Enable Global Interrupts
}

// Initialize external interrupt INT0 to reset the timer
void Reset_Timer_INT0_Init(void)
{
	DDRD &= ~(1<<2);                  // Set PD2 as input for INT0
	PORTD |= (1<<2);                  // Enable internal pull-up resistor for PD2
	GICR |= (1<<6);                   // Enable INT0
	MCUCR |=(1<<ISC01);               // Trigger INT0 on the falling edge
	sei();                            // Enable Global Interrupts
}

// Initialize external interrupt INT1 to pause the timer
void Pause_Timer_INT1_Init(void)
{
	DDRD &= ~(1<<3);                  // Set PD3 as input for INT1
	GICR |= (1<<INT1);                // Enable INT1
	MCUCR |=(1<<ISC10) | (1<<ISC11);  // Trigger INT1 on the rising edge
	sei();                            // Enable Global Interrupts
}

// Initialize external interrupt INT2 to resume the timer
void Resume_Timer_INT2_Init(void)
{
	GICR |= (1<<INT2);                // Enable INT2
	MCUCSR &= (1<<ISC2);              // Trigger INT2 on the falling edge
	sei();                            // Enable Global Interrupts
}

// Function to display seconds on 7-segment display
void display_seconds(void)
{
	unsigned char first_digit = seconds % 10;      // Calculate unit digit of seconds
	unsigned char second_digit = seconds / 10;     // Calculate tens digit of seconds

	PORTA = (PORTA & 0xC0) | 0x20;                 // Enable second digit display
	PORTC = (PORTC & 0xF0) | (first_digit & 0x0F); // Show the first digit
	_delay_ms(2);                                  // Small delay for display refresh

	PORTA = (PORTA & 0xC0) | 0x10;                 // Enable first digit display
	PORTC = (PORTC & 0xF0) | (second_digit & 0x0F);// Show the second digit
	_delay_ms(2);                                  // Small delay for display refresh
}

// Function to display minutes on 7-segment display
void display_minutes(void)
{
	unsigned char first_digit = minutes % 10;      // Calculate unit digit of minutes
	unsigned char second_digit = minutes / 10;     // Calculate tens digit of minutes

	PORTA = (PORTA & 0xC0) | 0x08;                 // Enable second digit display
	PORTC = (PORTC & 0xF0) | (first_digit & 0x0F); // Show the first digit
	_delay_ms(2);                                  // Small delay for display refresh

	PORTA = (PORTA & 0xC0) | 0x04;                 // Enable first digit display
	PORTC = (PORTC & 0xF0) | (second_digit & 0x0F);// Show the second digit
	_delay_ms(2);                                  // Small delay for display refresh
}

// Function to display hours on 7-segment display
void display_hours(void)
{
	unsigned char first_digit = hours % 10;        // Calculate unit digit of hours
	unsigned char second_digit = hours / 10;       // Calculate tens digit of hours

	PORTA = (PORTA & 0xC0) | 0x02;                 // Enable second digit display
	PORTC = (PORTC & 0xF0) | (first_digit & 0x0F); // Show the first digit
	_delay_ms(2);                                  // Small delay for display refresh

	PORTA = (PORTA & 0xC0) | 0x01;                 // Enable first digit display
	PORTC = (PORTC & 0xF0) | (second_digit & 0x0F);// Show the second digit
	_delay_ms(2);                                  // Small delay for display refresh
}

// Handle time adjustments using buttons (increment/decrement hours, minutes, and seconds)
void Handle_time_with_buttons(void)
{
	// Increment hours with button on PB1
	if (!(PINB & (1<<PB1)))
	{
		_delay_ms(30);  // Debounce delay
		if (!(PINB & (1<<PB1)))
		{
			if (flag_HOURS_INC_Handle == 0)
			{
				hours++;
				flag_HOURS_INC_Handle = 1;   // Set flag to avoid multiple increments
			}
		}
	}
	else
		flag_HOURS_INC_Handle = 0;   // Reset flag when button is released

	// Decrement hours with button on PB0
	if (!(PINB & (1<<PB0)))
	{
		_delay_ms(30);  // Debounce delay
		if (!(PINB & (1<<PB0)))
		{
			if (flag_HOURS_DEC_Handle == 0)
			{
				if (hours == 0)
					hours = 0;  // Prevent negative hours
				else
					hours--;
				flag_HOURS_DEC_Handle = 1;   // Set flag to avoid multiple decrements
			}
		}
	}
	else
		flag_HOURS_DEC_Handle = 0;   // Reset flag when button is released

	// Increment minutes with button on PB4
	if (!(PINB & (1<<PB4)))
	{
		_delay_ms(30);  // Debounce delay
		if (!(PINB & (1<<PB4)))
		{
			if (flag_MINUTES_INC_Handle == 0)
			{
				if (minutes == 59)
				{
					minutes = 0;
					hours++;  // Roll over to next hour if minutes reach 59
				}
				else
					minutes++;
				flag_MINUTES_INC_Handle = 1;   // Set flag to avoid multiple increments
			}
		}
	}
	else
		flag_MINUTES_INC_Handle = 0;   // Reset flag when button is released

	// Decrement minutes with button on PB3
	if (!(PINB & (1<<PB3)))
	{
		_delay_ms(30);  // Debounce delay
		if (!(PINB & (1<<PB3)))
		{
			if (flag_MINUTES_DEC_Handle == 0)
			{
				if (minutes == 0 && seconds == 0)
				{
					minutes = 59;
					hours--;  // Roll back to previous hour if minutes reach 0
				}
				else
					minutes--;
				flag_MINUTES_DEC_Handle = 1;   // Set flag to avoid multiple decrements
			}
		}
	}
	else
		flag_MINUTES_DEC_Handle = 0;   // Reset flag when button is released

	// Increment seconds with button on PB6
	if (!(PINB & (1<<PB6)))
	{
		_delay_ms(30);  // Debounce delay
		if (!(PINB & (1<<PB6)))
		{
			if (flag_SECONDS_INC_Handle == 0)
			{
				seconds++;
				if (seconds == 60)
				{
					seconds = 0;
					minutes++;  // Roll over to next minute if seconds reach 60
					if(minutes == 60)
					{
						minutes = 0;
						hours ++ ;
					}
				}
				flag_SECONDS_INC_Handle = 1;   // Set flag to avoid multiple increments
			}
		}
	}
	else
		flag_SECONDS_INC_Handle = 0;   // Reset flag when button is released

	// Decrement seconds with button on PB5
	if (!(PINB & (1<<PB5)))
	{
		_delay_ms(30);  // Debounce delay
		if (!(PINB & (1<<PB5)))
		{
			if (flag_SECONDS_DEC_Handle == 0)
			{
				if (seconds == 0)
				{
					seconds = 59;
					if (minutes > 0)
						minutes--;
					else if (hours > 0)
					{
						hours--;
						minutes = 59;
					}
				}
				else
					seconds--;
				flag_SECONDS_DEC_Handle = 1;   // Set flag to avoid multiple decrements
			}
		}
	}
	else
		flag_SECONDS_DEC_Handle = 0;   // Reset flag when button is released
}

// Handle switching between count up and count down modes using button on PB7
void Timer_Mode_Handle(void)
{
	if (!(PINB & (1<<PB7)))
	{
		_delay_ms(30);  // Debounce delay
		if (!(PINB & (1<<PB7)))
		{
			if (Toggle_Mode_Flag == 0)
			{
				Count_Flag ^= 1;  // Toggle count mode (up/down)
				Toggle_Mode_Flag = 1;  // Set flag to avoid multiple toggles
			}
		}
	}
	else
		Toggle_Mode_Flag = 0;  // Reset flag when button is released
}

int main(void)
{
	// Initialize ports and peripherals
	DDRA |= 0x3F;        // Set PORTA as output for 7-segment control
	DDRB = 0x00;         // Set PORTB as input for buttons
	DDRC |= 0x0F;        // Set lower nibble of PORTC as output for 7-segment control
	DDRD |= (1<<PD4) | (1<<PD5);  // Set PD4 and PD5 as output for LEDs
	PORTD &= ~((1<<PD4) | (1<<PD5));  // Turn off both LEDs
	DDRD |= (1<<PD0);    // Set PD0 as output for buzzer
	PORTD &= ~(1<<PD0);  // Turn off buzzer
	PORTB = 0xFF;        // Enable internal pull-up resistors for all PORTB pins

	// Initialize external interrupts and timer
	Pause_Timer_INT1_Init();
	Resume_Timer_INT2_Init();
	Reset_Timer_INT0_Init();
	Timer1_CTC_Init();

	// Main loop
	while (1)
	{
		display_seconds();     // Display seconds on 7-segment
		display_minutes();     // Display minutes on 7-segment
		display_hours();       // Display hours on 7-segment
		Handle_time_with_buttons();  // Handle time adjustments with buttons
		Timer_Mode_Handle();   // Handle mode toggling (count up/down)
	}
}

// Interrupt Service Routine for Timer1 Compare Match (CTC mode)
ISR(TIMER1_COMPA_vect)
{
	// Count up mode
	if (Count_Flag == 0)
	{
		PORTD |= (1<<PD4);   // Turn on Red LED (indicating count up mode)
		PORTD &= ~(1<<PD5);  // Turn off Yellow LED
		PORTD &= ~(1<<PD0);  // Turn off buzzer

		seconds++;  // Increment seconds

		if (seconds == 60)
		{
			seconds = 0;
			minutes++;  // Increment minutes if seconds reach 60
		}

		if (minutes == 60)
		{
			minutes = 0;
			hours++;  // Increment hours if minutes reach 60
		}

		if (hours >= 24)  // Reset time after 23 hours (24-hour format)
		{
			hours = 0;
		}
	}
	// Count down mode
	else if (Count_Flag == 1)
	{
		PORTD |= (1<<PD5);   // Turn on Yellow LED (indicating count down mode)
		PORTD &= ~(1<<PD4);  // Turn off Red LED

		if (hours == 0 && minutes == 0 && seconds == 0)
		{
			// Time has reached 00:00:00, trigger buzzer
			PORTD |= (1<<PD0);  // Turn on buzzer
		}
		else
		{
			// Decrement seconds
			if (seconds == 0)
			{
				if (minutes == 0 && hours > 0)
				{
					minutes = 59;  // Decrement hours if minutes and seconds are both zero
					hours--;
				}
				else if (minutes > 0)
				{
					minutes--;  // Decrement minutes if greater than zero
				}
				seconds = 59;  // Reset seconds to 59
			}
			else
			{
				seconds--;  // Decrement seconds
			}
		}
	}
}

// ISR for INT0 (Reset Timer)
ISR(INT0_vect)
{
	seconds = 0;
	minutes = 0;
	hours = 0;
	TCNT1 = 0;  // Reset Timer1 counter
}

// ISR for INT1 (Pause Timer)
ISR(INT1_vect)
{
	TCCR1B &= ~( (1<<CS10) | (1<<CS11) | (1<<CS12) );  // Stop Timer1
}

// ISR for INT2 (Resume Timer)
ISR(INT2_vect)
{
	TCCR1B |= (1<<CS10) | (1<<CS12);  // Resume Timer1 with prescaler 1024
}
