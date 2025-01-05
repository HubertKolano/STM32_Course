#include <learninglib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int ledMode = 0;
static int line_length = 0;
static char line_buffer[LINE_MAX_LENGTH + 1];
static int speedChosen = 0;

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


/*LEDS*/
bool timePassed(int multiplier){
	static uint32_t lastTick = 0;
	uint32_t tick = HAL_GetTick();
	if(tick - lastTick > multiplier * SWTICH_SPEED_ARR[speedChosen] || ((lastTick > tick) && (UINT_MAX - lastTick +  tick > multiplier* SWTICH_SPEED_ARR[speedChosen]))){
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

	if(timePassed(3)){
		switch (colour){
			case (RED): for (int i = 0; i < TOTAL_LEDS; i=i+3){led_set(i, true);} for(int i = 2; i < TOTAL_LEDS; i=i+3){led_set(i, false);} colour = GREEN; break;
			case (GREEN): for (int i = 1; i < TOTAL_LEDS; i=i+3){led_set(i, true);} for(int i = 0; i < TOTAL_LEDS; i=i+3){led_set(i, false);} colour = BLUE; break;
			case (BLUE): for (int i = 2; i < TOTAL_LEDS; i=i+3){led_set(i, true);} for(int i = 1; i < TOTAL_LEDS; i=i+3){led_set(i, false);} colour = RED; break;
			}
		}
	}

void ledForthMode(){
	static int ledIterator = 0;

	if(timePassed(2)){
		led_set(ledIterator, false);
		ledIterator = (ledIterator < TOTAL_LEDS - 1) ? ledIterator+1 : 0;
		led_set(ledIterator, true);
	}
	}

void ledForthBackMode(){

	static int ledIterator = 0;
	static int direction = 1;

	if(timePassed(2)){
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
	if(timePassed(2)){
		led_set(rand() % TOTAL_LEDS, true);
		led_set(rand() % TOTAL_LEDS, false);
	}
}


/*
 * *********************************************** *
 * 					CZĘŚĆ UARTOWA
 * *********************************************** *
 */



void line_append(uint8_t value) //funkcja zczytuje uart az do znaku enter i potem wykonuje komende
{
	if (value == '\r' || value == '\n') {
		// odebraliśmy znak końca linii
		if (line_length > 0) {
			// dodajemy 0 na końcu linii
			line_buffer[line_length] = '\0';
			// przetwarzamy danetempo

			if (strcmp(line_buffer, "next") == 0) {
				changeMode();
			} else if (strcmp(line_buffer, "szybciej") == 0) {
				changePaceDown();
			} else if (strcmp(line_buffer, "wolniej") == 0) {
				changePaceUp();}
			else if (strncmp(line_buffer, "tempo", 4) == 0 && line_length >= 7) {
				int inputSpeed = 0;
				for (int i = 0; i < line_length - 6; i++){
					inputSpeed += (line_buffer[6+i] - 48) * (10 * (line_length - 7 - i));
				}
				inputSpeed += line_buffer[line_length - 1] - 48;
					if (inputSpeed >= 0 && inputSpeed < 30){
						speedChosen = inputSpeed;
					}
					else{
						printf("Wybrałeś złą prędkość: %i\n", inputSpeed);
					}
			}
			else if (strcmp(line_buffer, "show") == 0) {
				printf("Aktualne tempo: %i\n", speedChosen);
			}
			else {
				printf("Nieznane polecenie: %s\n", line_buffer);
			}
			// zaczynamy zbieranie danych od nowa
			line_length = 0;
		}
	}
	else {
		if (line_length >= LINE_MAX_LENGTH) {
			// za dużo danych, usuwamy wszystko co odebraliśmy dotychczas
			line_length = 0;
		}
		// dopisujemy wartość do bufora
		line_buffer[line_length++] = value;
	}
}
