#include <headers.h>
#include <stdint.h>
#include <stdio.h>

volatile int LED_PULS_DELAY = 1;					// Frecuencia de parpadeo de led de pulsadores
volatile int LED_ALARM_DELAY = 5;					// Frecuencia de parpadeo de led de alarma
volatile int PULS_STATUS = 0;						// Estado de pulsadores
volatile int ALARM_STATUS = 0;						// Estado de alarma
volatile int LED1_STATUS = 0; 						// Estado inicial del LED1
volatile int LED2_STATUS = 0;						// Estado inicial del LED2
volatile int LED3_STATUS = 0; 						// Estado inicial del LED3
volatile int LED1_Ticks = 0;						// Ticks del LED1
volatile int LED2_Ticks = 0;						// Ticks del LED2

unsigned int tmp;
static volatile uint8_t ADC_Interrupt_Done_Flag;

void SysTick_Handler(void){
	printf("\nLED1_Ticks = %d - LED2_Ticks = %d", LED1_Ticks, LED2_Ticks);

	// LED de pulsadores
	if (PULS_STATUS == 1){
		LED1_Ticks++;
		GPIO_PORT-> SET[0]= (1<<14);				// Enciende LED1
		LED1_STATUS = 1;							// Cambia el estado
		printf("\nLED1 Encendido");
		if (LED1_Ticks == LED_PULS_DELAY){
			LED1_Ticks = 0;
			GPIO_PORT-> CLR[0]= (1<<14);			// Apaga LED1
			LED1_STATUS = 0;						// Cambia el estado del LED1
			PULS_STATUS = 0;						// Cambia el estado del PULS
			printf("\nLED1 Apagado");
			}
		}

	// LED de alarma
	if (ALARM_STATUS == 1){
		LED2_Ticks++;
		if (LED2_Ticks == LED_ALARM_DELAY){
			LED2_Ticks = 0;
				if(LED2_STATUS == 0){
					GPIO_PORT-> SET[1]= (1<<11);	// Enciende LED2
					LED2_STATUS = 1;				// Cambia el estado del LED2
					printf("\nLED2 Encendido");
				}
				else{
					GPIO_PORT-> CLR[1]= (1<<11);	// Apaga LED2
					LED2_STATUS = 0;				// Cambia el estado del LED2
					printf("\nLED2 Apagado");
				}
			}
	}
}

void ADC0_IRQHandler(void){
	printf('ADC HANDLER');
	//NVIC -> ICER[0] = (1<<17);						// Deshabilitacion de la interrupcion del ADC
	NVIC_DesIRQ(17);
	tmp = ADC -> DR1; 								// DR = Registro en el que se almacena el valor adquirido
	tmp = (tmp >> 6) & 0x03FF;
	ADC_Interrupt_Done_Flag = 1;
	printf('Medición = %u\n', tmp);
	NVIC_EnaIRQ(17);
	//NVIC -> ISER[0] = (1<<17);						// Habilito la interrupcion del ADC
}

//-----PULS_0 Handler = UP-----//
void GPIO0_IRQHandler(void){
	// TODO: Subir valor alarma
	GPIO_PIN_INT -> IST = (1 << 0);					// Limpia la interrupcion anterior en la pos 0 del PINTSEL0
	PULS_STATUS = 1;
}

//-----PULS_1 Handler = DOWN-----//
void GPIO1_IRQHandler(void){
	// TODO: Bajar valor alarma
	GPIO_PIN_INT -> IST = (1 << 1); 				// Limpia la interrupcion anterior en la pos 1 del PINTSEL0
	PULS_STATUS = 1;
	ALARM_STATUS = 0; // TODO: SACAR ESTO, ES SOLO PARA SIMULAR ALARMA ADC
}

//-----PULS_2 Handler = DOWN-----//
//void GPIO2_IRQHandler(void){
	// TODO: Bajar valor alarma
//	GPIO_PIN_INT->IST = (1 << 2); 					// Limpia la interrupcion anterior en la pos 1 del PINTSEL0
//	PULS_STATUS = 1;
//	ALARM_STATUS = 0; // TODO: SACAR ESTO, ES SOLO PARA SIMULAR ALARMA ADC
//}

//-----PULS_3 Handler = ENTER-----//
void GPIO3_IRQHandler(void){
	// TODO: Seleccionar valor alarma
	GPIO_PIN_INT -> IST = (1 << 3); 				// Limpia la interrupcion anterior en la pos 2 del PINTSEL0
	PULS_STATUS = 1;
	ALARM_STATUS = 1; // TODO: SACAR ESTO, ES SOLO PARA SIMULAR ALARMA ADC
}

static void readADC(void){
	NVIC_EnaIRQ(17);
	//NVIC -> ISER[0] = (1<<17);								// Habilito la interrupcion del ADC
	ADC -> INTEN = (1 << 0);								// Habilitacion de la interrupcion generada cuando se termina una conversion

	ADC_Interrupt_Done_Flag = 1;
	while (1){
		if (ADC_Interrupt_Done_Flag) {
			ADC_Interrupt_Done_Flag = 0;
			ADC -> CR = (1 << 24);							// Inicio de una adquisicion
		}
	}
	NVIC_DesIRQ(17);
	//NVIC -> ICER[0] = (1<<17);								// Deshabilitacion de la interrupcion del ADC
}

int main(void){
	NVIC_init();
	systick_init();
	LEDs_init();
	ADC_init();
	pulsadores_init();
	display_init();

	/*while(1) {
			//-----OK Status-----//
		if (ALARM_STATUS == 1){
			GPIO_PORT-> CLR[1]= (1<<12);
		}
		else{
			//-----ALARM Status-----//
			GPIO_PORT-> CLR[1]= (1<<11);
			GPIO_PORT-> SET[1]= (1<<12);
		}
	}*/
	readADC();
	return 0;
}
