#ifndef LEARNINGLIB
#define LEARNINGLIB
//#include "stm32f3xx_hal.h"
#include "main.h"
#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>

/* RTC functionality */
bool hasTimedOut(uint32_t interval_ms, uint32_t *lastExecutionTime);
const char* getWeekdayName(uint8_t weekday);



/* UART FUNCTIONALITY */
void line_append(uint8_t value);
void UART_commands();

/*	BUTTON FUNCTIONALITY	*/
#define BOUNCE_TIME 40
#define UART_BUFFER_SIZE 80

typedef void(*buttonFunction)();

void changeMode();
void changePaceUp();
void changePaceDown();
void buttonPressDetect();



typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_PinState lastState;
	buttonFunction function;
	uint32_t bounceTimeTick;
	uint32_t longpressTimeTick;
	const uint32_t longpressTime;
}button_t;


static button_t BUTTON[] = {
	{USER_BUTTON_GPIO_Port, USER_BUTTON_Pin,  GPIO_PIN_SET, changeMode, 0, 0, 1000},
	{USER_BUTTON2_GPIO_Port, USER_BUTTON2_Pin, GPIO_PIN_SET, changePaceUp, 0, 0, 100},
	{USER_BUTTON3_GPIO_Port, USER_BUTTON3_Pin, GPIO_PIN_SET, changePaceDown, 0, 0, 100}
};




/* LED functionality */
bool LED_timePassed(int multiplier);
void ledLightMode();
void led_set(int led, bool turn_on);
void ledOneColourMode();
void ledForthMode();
void ledForthBackMode();
void ledRandomMode();

typedef struct {
GPIO_TypeDef* port;
uint16_t pin;
}led_t;


//static int led;

enum colour {
		RED,
		GREEN,
		BLUE};


static const led_t LED[] = {
		{ LED1_GPIO_Port, LED1_Pin },
		{ LED2_GPIO_Port, LED2_Pin },
		{ LED3_GPIO_Port, LED3_Pin },
		{ LED4_GPIO_Port, LED4_Pin },
		{ LED5_GPIO_Port, LED5_Pin },
		{ LED6_GPIO_Port, LED6_Pin },
		{ LED7_GPIO_Port, LED7_Pin },
		{ LED8_GPIO_Port, LED8_Pin },
		{ LED9_GPIO_Port, LED9_Pin }
};



static const int TOTAL_LEDS = sizeof(LED)/sizeof(LED[0]);

static const int TOTAL_BUTTONS = sizeof(BUTTON)/sizeof(BUTTON[0]);

static const int SWTICH_SPEED_ARR[] = {			//30 consecutive logarithmic values, multiplied by 10, integer precision
        500, 437, 382, 335, 294, 257,
		225, 197, 172, 151, 131, 114,
		100, 87, 75, 66, 57, 50,
		43, 38, 33, 29, 25, 22,
		19, 17, 15, 13, 11, 10

    };

static const int SWITCH_SPEED_SIZE = sizeof(SWTICH_SPEED_ARR)/sizeof(SWTICH_SPEED_ARR[0]);



#ifdef __cplusplus
}
#endif

#endif
