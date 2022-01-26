#include <definitions.h>
#include <math.h>
#include <stdio.h>

#define GPDMA_CLOCK		204e6						// Frecuencia de CLK de operacion de la placa

int state2 = 0, state3 = 0; 			// Estado de los LEDS 2 y 3

void DMA_IRQHandler (void){
	DMA->INTTCCLEAR = (0x1);			// Limpia la interrupcion anterior
}

//-----LED2 Handler = Alarma-----//
void GPIO2_IRQHandler(void){
	GPIO_PIN_INT->IST = (1 << 2); 		// Limpia la interrupcion anterior en la pos 2 del PINTSEL0
	if (state2 == 0) {
		state2 = 1;						// Toogle
		GPIO_PORT-> SET[1]= (1<<11);	// Prende LED2
	}
	else{
		state2 = 0;						// Toogle
		GPIO_PORT-> CLR[1]= (1<<11);	// Apaga LED2
	}
}

//-----LED3 Handler = OK Status-----//
void GPIO3_IRQHandler(void){
	GPIO_PIN_INT->IST = (1 << 3);		// Limpia la interrupcion anterior en la pos 3 del PINTSEL0
	if (state3 == 0){
		state3 = 1;						// Toogle
		GPIO_PORT-> SET[1]= (1<<12);	// Prende LED3
	}
	else{
		state3 = 0;						// Toogle
		GPIO_PORT-> CLR[1]= (1<<12);	// Apaga LED3
	}
}

int main(void){
	static int i = 0;

	//-----NVIC-----//
	NVIC->ISER[0]=(1<<2);   								// Habilita las interrupciones del DMA
	NVIC->ISER[1]=(1<<0)|(1<<1)|(1<<2)|(1<<3);  			// Habilita las interrupciones de los GPIO (Pulsadores)

	//-----LEDs-----//
	SCU->SFSP[2][11]= (0x2 << 3) | (0x0);					// LED2 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU->SFSP[2][12]= (0x2 << 3) | (0x0);					// LED3 = Func 0 - Desactiva PullUp - Desactiva PullDown
	GPIO_PORT -> DIR[1] = (1 << 11) | (1 << 12) ;			// LED2 = Output | LED3 = Output

	//-----ADC0_1-----//
	SCU -> SFSP[4][1] = (1 << 4) | (0x4);					// ADC0_1 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU -> ENAIO[0] = (1 << 1);								// Selecciona la entrada analogica del ADC0_1

	//-----PULSADORES-----//
	SCU->SFSP[1][0] = (1 << 6) | (1 << 4) | (0x0); 			// PULS_0 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU->SFSP[1][6] = (1 << 6) | (1 << 4) | (0x0);			// PULS_1 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU->SFSP[1][1] = (1 << 6) | (1 << 4) | (0x0); 			// PULS_2 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer

	SCU->PINTSEL[0] = (1 << 2)  |							// PINTSEL 0 = Vector de 32bits, separado en 4 interrupciones de 8 bits c/u
					  (1 << 11) |							// PULS_0 = GPIO0_4 => (1<<2) = 00000100 -> PORTBITS = 000 ^ PINBITS = 00100 = 4
					  (1 << 16) | (1 << 19);          		// PULS_1 = GPIO0_9 => (1<<11) = 00001000 ; PULS_2 = GPIO0_9 => (1<<16) | (1<<19) = 00001001

	GPIO_PIN_INT->ISEL = (0<<0)|(0<<1)|(0<<2)|(0<<3);    	// Habilita la interrupcion por flanco para las 4 interruciones GPIO en PINTSEL
	GPIO_PIN_INT->SIENF = (1<<0)|(1<<1)|(1<<2)|(1<<3);  	// Interrupcion por flanco descendente


	while(1) {
		continue;
	}

	return 0;
}
