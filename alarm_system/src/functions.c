#include <headers.h>

#define GPDMA_CLOCK		204e6						// Frecuencia de CLK de operacion de la placa
#define TICKRATE		1000						// 1000 ticks por segundo

void NVIC_init(void){
	NVIC -> ISER[1] = (1<<0) | (1<<1) | (1<<3);  			// Habilita las interrupciones de los GPIO (Pulsadores)
}

void systick_init(void){
	_SysTick -> LOAD = (GPDMA_CLOCK / TICKRATE) - 1;		// Set reload register
	_SysTick -> VAL  = 0;									// Se limpia el valor
	_SysTick -> CTRL = _SysTick_CTRL_CLKSOURCE_PROC_Msk |	// Se usa el reloj del sistema (204 MHz)
					   _SysTick_CTRL_TICKINT_Msk   		|
					   _SysTick_CTRL_ENABLE_Msk;			// Habilitacion del contador
}

void LEDs_init(void){
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
}

void ADC_init(void){
	//SCU -> SFSP[4][1] = (1 << 4) | (0x4);					// ADC0_1 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU -> ENAIO[0] = (1 << 1);								// Selecciona la entrada analogica del ADC0_1

	ADC -> CR = (1 << 1)  									// ADC Select = ADCn_1
			  | (0xB9 << 8)								    // Clock Divider. Fcpu / (Fm*Clks) = 204MHz / (100kHz*11) = 185.45
			  | (0 << 16)									// Burst mode. Es necesario para usar junto al DMA.
			  | (0x0 << 17)									// Numero de Clocks. Conversiones mas lentas (11 ticks) pero mas precisas.
			  | (1 << 21)									// Power mode normal
			  | (1 << 24)									// Start now. No usamos disparo por flanco.
			  | (0 << 27);									// Tipo de flanco (solo si se eligio flanco en el Start)

	ADC -> GDR = (0 << 6)									// VREF: Cuando Done = 1, aca se guarda una fraccion binaria que representa la V_ADC / VDD
			   | (0 << 24)									// CHN: Channel donde se hizo la conversion de LS bits DUDA: Que son LS bits
			   | (0 << 30)									// OVERRUN: Guarda un 1 si hubo errores en la conversion
			   | (0 << 31);									// DONE: Guarda un 1 cuando se realiza una conversion

	ADC -> INTEN = (0 << 0)									// PONER 1 EN 0 ADINTEN: Canal donde se genera la interrupcion al realizar una conversion
				 | (0 << 8);								// ADGINTEN: En 1 habilita interrupcion por flag global DONE y en 0 los channels de ADINTEN generan las interrupciones

	ADC -> DR1 = (0 << 0)									// VREF: Cuando Done = 1, aca se guarda una fraccion binaria que representa la V_ADC / VDD
			   | (0 << 30)									// OVERRUN: Guarda un 1 si hubo errores en la conversion
			   | (0 << 31);									// DONE: Guarda un 1 cuando se realiza una conversion

	ADC -> STAT = (0 << 0)									// DONE: Copia el DONE Status Flag de cada channel
				| (0 << 8)									// OVERRUN: Copia el OVERRUN Status Flag de cada channel
				| (0 << 16);								// ADINT: Se pone en 1 cuando la DONE Status Flag de algun Channel se pone en 1 y se realiza la interrupcion
}

void pulsadores_init(void){
	SCU -> SFSP[1][0] = (1 << 6) | (1 << 4) | (0x0);		// PULS_0 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU -> SFSP[1][6] = (1 << 6) | (1 << 4) | (0x0);		// PULS_1 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU -> SFSP[1][1] = (1 << 6) | (1 << 4) | (0x0); 		// PULS_2 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU -> SFSP[1][2] = (1 << 6) | (1 << 4) | (0x0); 		// PULS_3 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer

	SCU -> PINTSEL[0] = (1 << 2)  |							// PINTSEL 0 = Vector de 32bits, separado en 4 interrupciones de 8 bits c/u
						(1 << 11) |							// PULS_0 = GPIO0_4 => (1<<2) = 00000100 -> PORTBITS = 000 ^ PINBITS = 00100 = 4
						(1 << 16) | (1 << 19) |          	// PULS_1 = GPIO0_9 => (1<<11) = 00001000 ; PULS_2 = GPIO0_9 => (1<<16) | (1<<19) = 00001001
						(1 << 24) | (1 << 27) | (1 << 29);	// PULS_3 = GPIO1_9 => (1<<24) | (1<<27) | (1<<29) = 00101001

	GPIO_PIN_INT -> ISEL = (0 << 0) | (0 << 1) | (0 << 3); 	// Habilita la interrupcion por flanco para las 3 interruciones GPIO en PINTSEL
	GPIO_PIN_INT -> SIENF = (1 << 0) | (1 << 1) | (1 << 3);	// Interrupcion por flanco descendente
}

void display_init(void){
	//TODO: Configurar los pines del display
}
