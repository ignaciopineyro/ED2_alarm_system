#include <headers.h>
#include <math.h>
#include <stdio.h>

#define GPDMA_CLOCK		204e6						// Frecuencia de CLK de operacion de la placa
#define TICKRATE		1000						// 1000 ticks por segundo

volatile int LED_PULS_DELAY = 1;					// Frecuencia de parpadeo de led de pulsadores
volatile int LED_ALARM_DELAY = 5;					// Frecuencia de parpadeo de led de alarma
volatile int PULS_STATUS = 0;						// Estado de pulsadores
volatile int ALARM_STATUS = 0;						// Estado de alarma
volatile int LED1_STATUS = 0; 						// Estado inicial del LED1
volatile int LED2_STATUS = 0;						// Estado inicial del LED2
volatile int LED3_STATUS = 0; 						// Estado inicial del LED3
volatile int LED1_Ticks = 0;						// Ticks del LED1
volatile int LED2_Ticks = 0;						// Ticks del LED2

unsigned int temperature=0, temperature2=0;

struct LLI_T LLI0, LLI1;							// Struct de tipo LLI_T

//-----Lectura del ADC-----//
unsigned int readADC(void){
	ADC -> CR = (0 << 24);							// Start en 0 para activar el Burst
	ADC -> CR = (1 << 16);							// Burst en 1

	while(!(ADC -> STAT & (1 << 1))){				// Espera a que termine la conversion
		printf('\nWaiting for convertion');
	}

	ADC -> CR = (1 << 24);							// Reactivo el Start
	temperature = ADC -> DR1;
	//printf("\nTemperature = %d", temperature);
	return (unsigned int) temperature;
}

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
	// TODO: Subir valor alarma
	GPIO_PIN_INT->IST = (1 << 0); 					// Limpia la interrupcion anterior en la pos 0 del PINTSEL0
	PULS_STATUS = 1;
	temperature = readADC();
	printf("\nTemperature = %d", temperature);
}

//-----PULS_1 Handler = DOWN-----//
void GPIO1_IRQHandler(void){
	// TODO: Bajar valor alarma
	GPIO_PIN_INT->IST = (1 << 1); 					// Limpia la interrupcion anterior en la pos 1 del PINTSEL0
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
	GPIO_PIN_INT->IST = (1 << 3); 					// Limpia la interrupcion anterior en la pos 2 del PINTSEL0
	PULS_STATUS = 1;
	ALARM_STATUS = 1; // TODO: SACAR ESTO, ES SOLO PARA SIMULAR ALARMA ADC
}

int main(void){
	static int i = 0;

	//-----NVIC-----//
	//NVIC -> ISER[0] = (1<<2);   							// Habilita las interrupciones del DMA
	NVIC -> ISER[1] = (1<<0) | (1<<1) | (1<<3);  			// Habilita las interrupciones de los GPIO (Pulsadores)

	//-----SYSTICK-----//
	_SysTick -> LOAD = (GPDMA_CLOCK / TICKRATE) - 1;		// Set reload register
	_SysTick -> VAL  = 0;									// Se limpia el valor
	_SysTick -> CTRL = _SysTick_CTRL_CLKSOURCE_PROC_Msk |	// Se usa el reloj del sistema (204 MHz)
					   _SysTick_CTRL_TICKINT_Msk   		|
					   _SysTick_CTRL_ENABLE_Msk;			// Habilitacion del contador

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

	/* //-----DMA-----//
	LLI0.source = (unsigned int) &(ADC -> GDR);				// Se le pasa solo la primer posicion del valor fuente (GDR del ADC, VREF ya esta en bit6, no hace falta correrlo luego)
	LLI0.destination = (unsigned int) &temp[0];				// Se le pasa el mismo vector que usa el DAC para sacar las muestras.
	LLI0.next = (unsigned int) &LLI0;						// Apunta hacia la misma lista

	LLI0.control = (128<<0) | (0 << 12)  					// TRANSFER SIZE = 128, cantidad de datos a enviar | source burst size (12 - 14) = 1
							| (0 << 15)  					// destination burst size (15 - 17) = 1
							| (2 << 18) 			 		// source width (18 - 20) = 32 bit
							| (2 << 21)  					// destination width (21 - 23) = 32 bit
							| (0 << 24)  					// source AHB select (24) = AHB 0
							| (1 << 25)  					// destination AHB select (25) = AHB 1
							| (1 << 26)  					// source increment (26) = increment
							| (0 << 27)  					// destination increment (27) = no increment
							| (0 << 28)  					// mode select (28) = access in user mode
							| (0 << 29)  					// (29) = access not bufferable
							| (0 << 30)  					// (30) = access not cacheable
							| (1 << 31);					// terminal count interrupt disabled (deshabilita (0) / habilita (1))
	DMA -> CH[0].LLI = LLI0;								// Asignar las propiedades de LLI0 (Struct tipo LLI_T) al channel 1 del DMA de 8 canales. No se pueden asignar datos directamente a LLI
	(DMA -> CH[0]).CONFIG =  1   							// Habilitacion del canal 1 (Dentro del canal)
							| (0xD << 1)  					// Source peripheral. En 0xD usa ADC0 como fuente
							| (0 << 6) 						// Destination peripheral. Ignorar si el destino es la memoria.
							| (0x2 << 11)  					// Flow control. En 2 es periferico a memoria
							| (0 << 14)  					// (14) = mask out error interrupt (0 = enmsacara)
							| (1 << 15)  					// (15) = mask out terminal count interrupt  (0 = enmsacara)
							| (0 << 16)  					// (16) = no locked transfers
							| (0 << 18); 					// (27) = no HALT

	DMA -> CONFIG = 1;										// Habilitacion de DMA
	DMA -> ENBLDCHNS = (1 << 0);							// Habilitacion del canal 0 (Por fuera)
	DMA -> ENBLDCHNS = (1 << 1);							// Habilitacion del canal 1 (Por fuera)
	*/

	//-----ADC0_1-----//
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

	//-----PULSADORES-----//
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

	//-----DISPLAY-----//
	//TODO: Configurar los pines del display

	while(1) {
		//-----OK Status-----//
		if (ALARM_STATUS == 1){
			GPIO_PORT-> CLR[1]= (1<<12);
		}
		else{
			GPIO_PORT-> CLR[1]= (1<<11);
			GPIO_PORT-> SET[1]= (1<<12);
		}

		//temperature2 = readADC();
		//printf("\nTemperature2 = %d", temperature);

	}

	return 0;
}
