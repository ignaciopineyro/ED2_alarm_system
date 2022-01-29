#include <definitions.h>
#include <math.h>
#include <stdio.h>

#define GPDMA_CLOCK		204e6						// Frecuencia de CLK de operacion de la placa
#define TICKRATE		1000						// 1000 ticks por segundo

volatile int LED_PULS_DELAY = 1;					// Frecuencia de parpadeo de led de pulsadores
volatile int LED_ALARM_DELAY = 5;					// Frecuencia de parpadeo de led de alarma
volatile int PULS_STATUS = 0;						// Frecuencia de parpadeo de led de alarma
volatile int ALARM_STATUS = 0;						// Frecuencia de parpadeo de led de alarma
volatile int LED1_STATUS = 0; 						// Estado inicial del LED1
volatile int LED2_STATUS = 0;						// Estado inicial del LED2
volatile int LED3_STATUS = 0; 						// Estado inicial del LED3
volatile int LED1_Ticks = 0;						// Ticks del LED1
volatile int LED2_Ticks = 0;						// Ticks del LED2

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

void DMA_IRQHandler (void){
	DMA-> INTTCCLEAR = (0x1);			  			// Limpia la interrupcion anterior
}

//-----PULS_0 Handler = UP-----//
void GPIO0_IRQHandler(void){
	// TODO: Subir valor alarma y habilitar systick para LED1
	GPIO_PIN_INT->IST = (1 << 0); 					// Limpia la interrupcion anterior en la pos 0 del PINTSEL0
	PULS_STATUS = 1;
}

//-----PULS_1 Handler = DOWN-----//
void GPIO1_IRQHandler(void){
	// TODO: Bajar valor alarma y habilitar systick para LED1
	GPIO_PIN_INT->IST = (1 << 1); 					// Limpia la interrupcion anterior en la pos 1 del PINTSEL0
	PULS_STATUS = 1;
	ALARM_STATUS = 0; // TODO: SACAR ESTO, ES SOLO PARA SIMULAR ALARMA ADC
}

//-----PULS_3 Handler = ENTER-----//
void GPIO3_IRQHandler(void){
	// TODO: Seleccionar valor alarma y habilitar systick para LED1
	GPIO_PIN_INT->IST = (1 << 3); 					// Limpia la interrupcion anterior en la pos 2 del PINTSEL0
	PULS_STATUS = 1;
	ALARM_STATUS = 1; // TODO: SACAR ESTO, ES SOLO PARA SIMULAR ALARMA ADC
}

int main(void){
	static int i = 0;

	//-----NVIC-----//
	NVIC -> ISER[0] = (1<<2);   							// Habilita las interrupciones del DMA
	NVIC -> ISER[1] = (1<<0) | (1<<1) | (1<<3);  			// Habilita las interrupciones de los GPIO (Pulsadores)

	//-----SYSTICK-----//
	_SysTick->LOAD = (GPDMA_CLOCK / TICKRATE) - 1;			// Set reload register
	_SysTick->VAL  = 0;										// Se limpia el valor
	_SysTick->CTRL = _SysTick_CTRL_CLKSOURCE_PROC_Msk 	|	// Se usa el reloj del sistema (204 MHz)
					 _SysTick_CTRL_TICKINT_Msk   		|
					 _SysTick_CTRL_ENABLE_Msk;				// Habilitacion del contador

	//-----LEDs-----//
	SCU -> SFSP[2][10] = (0x2 << 3) | (0x0);				// LED1 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU -> SFSP[2][11] = (0x2 << 3) | (0x0);				// LED2 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU -> SFSP[2][12] = (0x2 << 3) | (0x0);				// LED3 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU -> SFSP[2][0] = (0x2 << 3) | (0x4);					// LED_R = Func 4 - Desactiva PullUp - Desactiva PullDown
	SCU -> SFSP[2][1] = (0x2 << 3) | (0x4);					// LED_G = Func 4 - Desactiva PullUp - Desactiva PullDown
	SCU -> SFSP[2][2] = (0x2 << 3) | (0x4);					// LED_B = Func 4 - Desactiva PullUp - Desactiva PullDown
	GPIO_PORT -> DIR[0] = (1 << 14);						// LED1 = Output
	GPIO_PORT -> DIR[1] = (1 << 11) | (1 << 12) ;			// LED2 = Output | LED3 = Output
	GPIO_PORT -> DIR[5] = ((1 << 2) | (1 << 1) | (1 << 0));	// LED_R = Output | LED_G = Output | LED_B = Output
	GPIO_PORT -> CLR[0] = (1<<14);							// Apaga LED1
	GPIO_PORT -> CLR[5] = (1<<0) | (1<<1) | (1<<2);			// Apaga LED R, G y B

	//-----ADC0_1-----//
	SCU -> SFSP[4][1] = (1 << 4) | (0x4);					// ADC0_1 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU -> ENAIO[0] = (1 << 1);								// Selecciona la entrada analogica del ADC0_1

	//-----PULSADORES-----//
	SCU -> SFSP[1][0] = (1 << 6) | (1 << 4) | (0x0);		// PULS_0 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU -> SFSP[1][6] = (1 << 6) | (1 << 4) | (0x0);		// PULS_1 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU->SFSP[1][2] = (1 << 6) | (1 << 4) | (0x0); 			// PULS_3 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer

	SCU -> PINTSEL[0] = (1 << 2)  |							// PINTSEL 0 = Vector de 32bits, separado en 4 interrupciones de 8 bits c/u
					  	(1 << 11) |							// PULS_0 = GPIO0_4 => (1<<2) = 00000100 -> PORTBITS = 000 ^ PINBITS = 00100 = 4
    					(1 << 24) | (1 << 27) | (1 << 29);	// PULS_3 = GPIO1_9 => (1<<24) | (1<<27) | (1<<29) = 00101001

	GPIO_PIN_INT -> ISEL = (0<<0) | (0<<1) | (0<<3); 		// Habilita la interrupcion por flanco para las 3 interruciones GPIO en PINTSEL
	GPIO_PIN_INT -> SIENF = (1<<0) | (1<<1) | (1<<3);		// Interrupcion por flanco descendente

	//-----DISPLAY-----//
	//TODO: Configurar los pines del display

	while(1) {
		//-----OK Status-----//
		if (ALARM_STATUS == 1){
			GPIO_PORT-> CLR[1]= (1<<12);
		}
		else{
			GPIO_PORT-> SET[1]= (1<<12);
		}
	}

	return 0;
}
