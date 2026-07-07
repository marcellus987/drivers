#include "lcd.h"




#define RS_PIN (1U << 8) /* PC8 */
#define E_PIN (1U << 9)  /* PC9 */
#define DATA_PIN_MASK 0xFFU
#define IS_COMMAND 1
#define CURSOR_MOVE 0x80U /* Command template. Or the proper address with this template. */



/* C0-C9. */
void lcd_init(void) {
	/* Enable clock access for Port C. */
	RCC->AHB1ENR |= (1U << 2);

	/* Configure pins as output. */
	GPIOC->MODER &= ~(0x000FFFFFU);
	GPIOC->MODER |= 0x00055555U;

	/* Wait 15 ms. */
	delay_ms(20);

	/* 1st 0x30 */
	send_command(0x30U);

	/* Wait 4 ms. */
	delay_ms(5);

	/* 2nd 0x30. */
	send_command(0x30U);

	/* Wait 100 us. */
	timDelay_us(150);

	/* 3rd 0x30. */
	send_command(0x30U);
	timDelay_us(150);

	/* Send initial commands. */
	send_command(EIGHT_BIT_TWO_LINES_MODE);
	send_command(0x08U);
	send_command(CLEAR_DISPLAY);
	send_command(ENTRY_MODE_NO_SHIFT);
	send_command(DISPLAY_CURSOR_OFF);
}

void cls(void) {
	send_command(CLEAR_DISPLAY);
}

void lcd_send(uint8_t data, uint8_t isCommand) {

	/* Force E Pin to LOW. */
	GPIOC->BSRR = E_PIN << 16;

	/* Set RS Pin to LOW for sending commands. */
	GPIOC->BSRR = isCommand == 1 ? RS_PIN << 16 : RS_PIN;

	/* Clear command bits. */
	GPIOC->BSRR = DATA_PIN_MASK << 16;

	/* Extract the 1-byte command. */
	GPIOC->BSRR = (data & DATA_PIN_MASK);

	/* Pulse E pin. */
	GPIOC->BSRR = E_PIN;

	/* Required delay by LCD manufacturer. */
	/******** Delay... *********/

	/*Note that this is not 1 ns, clocks's minimum is ~62.5ns,
	*  so this is ~62.5 * 4 = ~250*/
	delay_ms(1);

	/* Force E Pin to LOW. */
	GPIOC->BSRR = E_PIN << 16;
}

void send_command(uint8_t command) {
	lcd_send(command, IS_COMMAND);
	if(command == CLEAR_DISPLAY || command == CURSOR_RETURN) {
		delay_ms(2);
	}

	delay_ms(1);

}

void send_data(uint8_t data) {
	lcd_send(data, ~IS_COMMAND);
	delay_ms(1); /* Similar to pulsing E. */
}

void move_cursor(uint8_t location) {
	uint8_t command = CURSOR_MOVE | location;
	send_command(command);
}
