#include <learninglib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int ledMode = 0;
static int line_length = 0;
static char line_buffer[UART_BUFFER_SIZE + 1];
static int speedChosen = 0;


//RTC functions
bool hasTimedOut(uint32_t interval_ms, uint32_t *lastExecutionTime) {
    uint32_t currentTime = HAL_GetTick();
    if ((currentTime - *lastExecutionTime) >= interval_ms) {
        *lastExecutionTime = currentTime;
        return true; // Time interval has passed
    }
    return false; // Time interval has not yet passed
}
const char* WeekdayNames[] = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
	"Sunday"
};

const char* getWeekdayName(uint8_t weekday){
	return  WeekdayNames[weekday - 1];
}


void buttonPressDetect(){
	uint16_t tick = HAL_GetTick();

	for (int i = 0; i < TOTAL_BUTTONS; i++){

	if(tick - BUTTON[i].bounceTimeTick < BOUNCE_TIME){
			continue;
		}
		BUTTON[i].bounceTimeTick = tick;
		GPIO_PinState currentState = HAL_GPIO_ReadPin(BUTTON[i].port, BUTTON[i].pin);

		if(currentState == GPIO_PIN_RESET && BUTTON[i].lastState == GPIO_PIN_SET){
			BUTTON[i].longpressTimeTick = tick;
			BUTTON[i].lastState = GPIO_PIN_RESET;
			//button press action below
			BUTTON[i].function();
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		}
		else if(currentState == GPIO_PIN_SET && BUTTON[i].lastState == GPIO_PIN_RESET){
			BUTTON[i].bounceTimeTick = tick;
			BUTTON[i].lastState = GPIO_PIN_SET;
			//button release action below
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		}
		else if(currentState == GPIO_PIN_RESET && BUTTON[i].lastState == GPIO_PIN_RESET && tick - BUTTON[i].longpressTimeTick > BUTTON[i].longpressTime){
			BUTTON[i].longpressTimeTick = tick;
			//button hold action below
			BUTTON[i].function();

		}

	}
}

//LED functions
void changeMode(){
	for (int i = 0; i < TOTAL_LEDS; i++){
		led_set(i, false);
	}
	ledMode++;
}

void changePaceUp(){

		if (speedChosen < SWITCH_SPEED_SIZE - 1){
			speedChosen++;
		}

}

void changePaceDown(){

	if (speedChosen > 0){
		speedChosen--;
	}

}


/*LEDS*/
bool LED_timePassed(int multiplier){
	static uint32_t lastTick = 0;
	uint32_t tick = HAL_GetTick();
	if(tick - lastTick > multiplier * SWTICH_SPEED_ARR[speedChosen]){
		lastTick = tick;
		return true;
	}
	else
		return false;
}

void led_set(int led, bool turn_on)
{
	GPIO_PinState state = (turn_on) ? GPIO_PIN_SET : GPIO_PIN_RESET;

	if (led >= 0 && led < TOTAL_LEDS) {
		HAL_GPIO_WritePin(LED[led].port, LED[led].pin, state);
        }
}

void ledLightMode(){
	switch (ledMode) {
	case 0: ledOneColourMode(); 	break;
	case 1: ledForthMode(); 		break;
	case 2: ledForthBackMode();		break;
	case 3: ledRandomMode();		break;
	default: ledMode = 0; 			break;
	}
}

void ledOneColourMode(){
	static int colour = RED;

	if(LED_timePassed(3)){
		switch (colour){
			case (RED): for (int i = 0; i < TOTAL_LEDS; i=i+3){led_set(i, true);} for(int i = 2; i < TOTAL_LEDS; i=i+3){led_set(i, false);} colour = GREEN; break;
			case (GREEN): for (int i = 1; i < TOTAL_LEDS; i=i+3){led_set(i, true);} for(int i = 0; i < TOTAL_LEDS; i=i+3){led_set(i, false);} colour = BLUE; break;
			case (BLUE): for (int i = 2; i < TOTAL_LEDS; i=i+3){led_set(i, true);} for(int i = 1; i < TOTAL_LEDS; i=i+3){led_set(i, false);} colour = RED; break;
			}
		}
	}

void ledForthMode(){
	static int ledIterator = 0;

	if(LED_timePassed(2)){
		led_set(ledIterator, false);
		ledIterator = (ledIterator < TOTAL_LEDS - 1) ? ledIterator+1 : 0;
		led_set(ledIterator, true);
	}
	}

void ledForthBackMode(){

	static int ledIterator = 0;
	static int direction = 1;

	if(LED_timePassed(2)){
		led_set(ledIterator, false);
		ledIterator += direction;
		if(ledIterator >= TOTAL_LEDS){
			direction = -1;
			ledIterator -= 2;
		}
		else if(ledIterator < 0){
			direction = 1;
			ledIterator += 2;
		}
		led_set(ledIterator, true);
	}

}
void ledRandomMode(){
	if(LED_timePassed(2)){
		led_set(rand() % TOTAL_LEDS, true);
		led_set(rand() % TOTAL_LEDS, false);
	}
}


/*
 * *********************************************** *
 * 					 UART PART
 * *********************************************** *
 */



void line_append(uint8_t value) //read the line till the ENTER key
{
	if (value == '\r' || value == '\n') {
			if (line_length > 0) {
				UART_commands();
			}
	}
	else {
		if (line_length >= UART_BUFFER_SIZE) {
			// bufffer overflow, start over
			line_length = 0;
		}
		line_buffer[line_length++] = value;
	}
}

void UART_commands(){

			line_buffer[line_length] = '\0';

			if (strcmp(line_buffer, "next") == 0) {
				changeMode();
			} else if (strcmp(line_buffer, "slower") == 0) {
				changePaceDown();
			} else if (strcmp(line_buffer, "faster") == 0) {
				changePaceUp();}
				//read the speed and integer that was given after the space
			else if (strncmp(line_buffer, "speed", 4) == 0 && line_length >= 7) {
				int inputSpeed = 0;
				for (int i = 0; i < line_length - 6; i++){
					inputSpeed += (line_buffer[6+i] - 48) * (10 * (line_length - 7 - i));
				}
				inputSpeed += line_buffer[line_length - 1] - 48;
					if (inputSpeed >= 0 && inputSpeed < 30){
						speedChosen = inputSpeed;
					}
					else{
						printf("You've chosen bad speed: %i\n", inputSpeed);
					}
			}
			else if (strcmp(line_buffer, "show") == 0) {
				printf("Current speed level: %i\n", speedChosen);
			}
			else {
				printf("Unknown command: %s\n", line_buffer);
			}
			// start over
			line_length = 0;
}