#ifndef LCD_H__
#define LCD_H__
#include "stm32f4xx.h"
#include "timer.h"
#include "systick.h"
#include <stdint.h>


#define CLEAR_DISPLAY 0x01U
#define CURSOR_RETURN 0x02U
#define ENTRY_MODE_NO_SHIFT 0x06U
#define DISPLAY_CURSOR_OFF 0x0CU
#define DISPLAY_CURSOR_ON 0x0EU
#define DISPLAY_CURSOR_BLINKING 0x0FU
#define EIGHT_BIT_TWO_LINES_MODE 0x38U
#define LETTER_H 0x48U
#define LCD_FIRST_ROW_BASE_ADDRESS 0x00U
#define LCD_MAX_COL 16
#define LCD_MAX_ROW 2


void lcd_send(uint8_t data, uint8_t isCommand);
void send_command(uint8_t command);
void send_data(uint8_t data);
void lcd_init(void);

void move_cursor(uint8_t location);


void cls(void);




#endif /* LCD_H__ */
