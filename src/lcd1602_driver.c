#include <lcd1602_driver.h>

/* From Datasheet. */
#define CLEAR_DISPLAY 0x01U
#define CURSOR_RETURN 0x02U
#define ENTRY_MODE_NO_SHIFT 0x06U
#define DISPLAY_CURSOR_OFF 0x0CU
#define DISPLAY_CURSOR_ON 0x0EU
#define DISPLAY_CURSOR_BLINKING 0x0FU
#define EIGHT_BIT_TWO_LINES_MODE 0x38U
#define LCD_FIRST_ROW_BASE_ADDRESS 0x00U

#define LCD_MAX_COL 16
#define LCD_MAX_ROW 2

#define RS_PIN (1U << 8) /* PC8 */
#define E_PIN (1U << 9)  /* PC9 */
#define DATA_PIN_MASK 0xFFU
#define IS_COMMAND 1
#define CURSOR_MOVE 0x80U /* Command template. Or the proper address with this template. */

/* Helper function definition START.*/
static void lcd_send(uint8_t data, uint8_t isCommand) {

	/* Force E Pin to LOW. */
	GPIOC->BSRR = E_PIN << 16;

	/* Set RS Pin to LOW for sending commands. */
	GPIOC->BSRR = (isCommand == 1) ? RS_PIN << 16 : RS_PIN;

	/* Clear command bits. */
	GPIOC->BSRR = DATA_PIN_MASK << 16;

	/* Extract the 1-byte command. */
	GPIOC->BSRR = (data & DATA_PIN_MASK);

	/* Pulse E pin. */
	GPIOC->BSRR = E_PIN;

	/* Required delay by LCD manufacturer. */
	/******** Delay... *********/

	delay_ms(1);

	/* Force E Pin to LOW. */
	GPIOC->BSRR = E_PIN << 16;
}
/* Helper function definition END. */

/* API definitions START. */
void lcd_init(void) {
	timer_config();
	/* Enable clock access for Port C. */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	/* Reset mode for pin 0 to 9. */
	GPIOC->MODER &= (0xFFF00000U);
	/* Configure pin 0 to 9 as output. */
	GPIOC->MODER |= (0x00055555U);

	/* Wait 20 ms. */
	delay_ms(20);

	/* 1st 0x30 */
	send_command(0x30U);

	/* Wait 5 ms. */
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
/* API definitions END. */
