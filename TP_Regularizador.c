#include <ED2_Regularizador.h>
#include <math.h>
#include <stdio.h>

#define PI				(float)(3.141592654)		// Defino numero PI
#define WAVE_SAMPLE_NUM	128							// Cantidad de muestras para que se vea bien la señal
#define AMP				512							// Amplitud de la señal (pico a pico 1024 y no puede tener valores negativos)
#define FRECUENCIA		440							// Frecuencia de la señal en Hz
#define GPDMA_CLOCK		204e6						// Frecuencia de CLK de operacion de la placa

unsigned int sinWave[WAVE_SAMPLE_NUM];				// Funcion senoidal a muestrear
unsigned int sinAux_1[WAVE_SAMPLE_NUM];				// Funcion senoidal aux1 (usada en la suma de varias funciones)
unsigned int sinAux_2[WAVE_SAMPLE_NUM];				// Funcion senoidal aux2 (usada en la suma de varias funciones)
unsigned int sinAux_3[WAVE_SAMPLE_NUM];				// Funcion senoidal aux3 (usada en la suma de varias funciones)
unsigned int sinAux_4[WAVE_SAMPLE_NUM];				// Funcion senoidal aux4 (usada en la suma de varias funciones)

int state0 = 0, state1 = 0, state2 = 0, state3 = 0; // Estado de los PULS 0 ~ 3

struct LLI_T LLI0;									// Struct de tipo LLI_T

void DMA_IRQHandler (void){
	DMA->INTTCCLEAR = (0x1);			// Limpia la interrupcion anterior
}

void GPIO0_IRQHandler(void){
	GPIO_PIN_INT->IST = (1 << 0); 		// Limpia la interrupcion anterior en la pos 0 del PINTSEL0
	if (state0 == 0){					// Toogle
		state0 = 1;
		GPIO_PORT-> SET[5]= (1<<2);		// Prende LED_B (GPIO5_2)
	}
	else{
		state0 = 0;						// Toogle
		GPIO_PORT-> CLR[5]= (1<<2); 	// Apaga LED_B
	}
}

void GPIO1_IRQHandler(void){
	GPIO_PIN_INT->IST = (1 << 1); 		// Limpia la interrupcion anterior en la pos 1 del PINTSEL0
	if (state1 == 0) {
		state1 = 1;						// Toogle
		GPIO_PORT-> SET[0]= (1<<14);	// Prende LED1
	}
	else{
		state1 = 0;						// Toogle
		GPIO_PORT-> CLR[0]= (1<<14);	// Apaga LED1
	}
}

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

	NVIC->ISER[0]=(1<<2);   								// Habilita las interrupciones del DMA
	NVIC->ISER[1]=(1<<0)|(1<<1)|(1<<2)|(1<<3);  			// Habilita las interrupciones de los GPIO (Pulsadores)

	SCU->SFSP[2][10]= (0x2 << 3) | (0x0);					// LED1 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU->SFSP[2][11]= (0x2 << 3) | (0x0);					// LED2 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU->SFSP[2][12]= (0x2 << 3) | (0x0);					// LED3 = Func 0 - Desactiva PullUp - Desactiva PullDown
	SCU->SFSP[2][0]= (0x2 << 3) | (0x4);					// LED_R = Func 4 - Desactiva PullUp - Desactiva PullDown
	SCU->SFSP[2][1]= (0x2 << 3) | (0x4);					// LED_G = Func 4 - Desactiva PullUp - Desactiva PullDown
	SCU->SFSP[2][2]= (0x2 << 3) | (0x4);					// LED_B = Func 4 - Desactiva PullUp - Desactiva PullDown
	SCU->SFSP[4][4]=(1<<4)|(0<<3); 							// Puerto P4_4 (DAC) como GPIO - Puldown, etc
	SCU->ENAIO[2]= (1<<0);     								// Selecciona funcion analogica para el P4_4 (DAC)

	GPIO_PORT->DIR[0]=(1 << 14);							// LED1 = Output
	GPIO_PORT->DIR[1]=(1 << 11) | (1 << 12) ;				// LED2 = Output | LED3 = Output
	GPIO_PORT->DIR[5]=((1 << 2) | (1 << 1) | (1 << 0));		// LED_R = Output | LED_G = Output | LED_B = Output

	//pulsinit - Configuro botones
	SCU->SFSP[1][0] = (1<<6)|(1<<4)|(0x0); 					// PULS_0 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU->SFSP[1][6] = (1<<6)|(1<<4)|(0x0);					// PULS_1 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU->SFSP[1][1] = (1<<6)|(1<<4)|(0x0); 					// PULS_2 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer
	SCU->SFSP[1][2] = (1<<6)|(1<<4)|(0x0); 					// PULS_3 = Func 0 - Desactiva PullUp - Desactiva PullDown - Activa Buffer

	SCU->PINTSEL[0] = (1 << 2)  |							// PINTSEL 0 = Vector de 32bits, separado en 4 interrupciones de 8 bits c/u
					  (1 << 11) |							// PULS_0 = GPIO0_4 => (1<<2) = 00000100 -> PORTBITS = 000 ^ PINBITS = 00100 = 4
					  (1 << 16) | (1 << 19) |          		// PULS_1 = GPIO0_9 => (1<<11) = 00001000 ; PULS_2 = GPIO0_9 => (1<<16) | (1<<19) = 00001001
				      (1 << 24) | (1 << 27) | (1 << 29);	// PULS_3 = GPIO1_9 => (1<<24) | (1<<27) | (1<<29) = 00101001


	GPIO_PIN_INT->ISEL = (0<<0)|(0<<1)|(0<<2)|(0<<3);    	// Habilita la interrupcion por flanco para las 4 interruciones GPIO en PINTSEL
	GPIO_PIN_INT->SIENF = (1<<0)|(1<<1)|(1<<2)|(1<<3);  	// Interrupcion por flanco descendente

	LLI0.source= (unsigned int) &sinWave[0];				// Se le pasa solo la primer posicion del valor fuente (Vector con sinWave)
	LLI0.destination= (unsigned int) &(DAC->CR);			// Se le pasa los datos a partir del bit6 del registro CR del DAC (Ver tabla 1153). Las 6 posiciones se corren luego en la sinWave.
	LLI0.next= (unsigned int) &LLI0;						// Apunta hacia la misma lista
	LLI0.control= (128<<0)  | (0 << 12)  					// TRANSFER SIZE = 128, cantidad de datos a enviar | source burst size (12 - 14) = 1
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
	DMA-> CH[0].LLI=LLI0;									// Asignar las propiedades de LLI0 (Struct tipo LLI_T) al channel 0 del DMA de 8 canales. No se pueden asignar datos directamente a LLI
    (DMA-> CH[0]).CONFIG=  1   								// Habilitacion del canal 0 (Dentro del canal)
            				| (0 << 1)  					// Source peripheral. En 0 usa la memoria como fuente
							| (0x0F << 6) 					// Destination peripheral. En 0x0F es el DAC
							| (1 << 11)  					// Flow control. En 1 es memoria a periferico
							| (0 << 14)  					// (14) = mask out error interrupt (0 = enmsacara)
							| (1 << 15)  					// (15) = mask out terminal count interrupt  (0 = enmsacara)
							| (0 << 16)  					// (16) = no locked transfers
							| (0 << 18); 					// (27) = no HALT
	DMA-> CONFIG= 1;										// Habilitacion de DMA
	DMA-> ENBLDCHNS= (1<<0);								// Habilitacion del canal 0 (Por fuera)

    DAC-> CNTVAL= GPDMA_CLOCK / (FRECUENCIA * WAVE_SAMPLE_NUM);         // Razon de periodos para que se muestree bien la señal en 440Hz. T1 = Tm/muestras ^ CNTVL = T1/Ttot = T1*fclock
    DAC-> CTRL= (1<<0) | (1<<2) | (1<<3); 					    			// DMA Request | DMA Time-out | DMA Enable (Activa DAC para trabajar con DMA)

	while (1){
		if(state0==0 && state1==0 && state2==0 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinWave[i] = ((((unsigned int)(AMP *0* sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16));
			}
		}

		if (state0 == 1  && state1==0 && state2==0 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinWave[i] = ((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16));
			}
		}

		if(state0==0 && state1==1 && state2==0 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++) {
				sinWave[i] = ((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16));
			}
		}

		if(state0==0 && state1==0 && state2==1 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinWave[i] = ((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16));
			}
		}

		if(state0==0 && state1==0 && state2==0 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinWave[i] = ((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16));
			}
		}

		//SUMA DE FUNCIONES SENOIDALES
		if (state0 == 1  && state1==1 && state2==0 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512; //Se normaliza la amplitud: (n*A)/A
				sinAux_2[i] = (((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i];
			}
		}

		if (state0 == 1  && state1==0 && state2==1 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i];
			}
		}

		if (state0 == 1  && state1==0 && state2==0 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i];
			}
		}

		if (state0 == 0  && state1==1 && state2==1 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i];
			}
		}

		if (state0 == 0  && state1==1 && state2==0 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i];
			}
		}

		if (state0 == 0  && state1==0 && state2==1 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1024)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i];
			}
		}

		if (state0 == 1  && state1==1 && state2==1 && state3==0){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_3[i] = (((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i]+sinAux_3[i];
			}
		}

		if (state0 == 0  && state1==1 && state2==1 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_3[i] = (((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i]+sinAux_3[i];
			}
		}

		if (state0 == 1  && state1==0 && state2==1 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_3[i] = (((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i]+sinAux_3[i];
			}
		}

		if (state0 == 1  && state1==1 && state2==0 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinAux_3[i] = (((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/1536)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i]+sinAux_3[i];
			}
		}

		if (state0 == 1  && state1==1 && state2==1 && state3==1){
			for(i=0; i < WAVE_SAMPLE_NUM; i++){
				sinAux_1[i] = (((((unsigned int)(AMP * sin(2*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/2048)*512;
				sinAux_2[i] = (((((unsigned int)(AMP * sin(4*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/2048)*512;
				sinAux_3[i] = (((((unsigned int)(AMP * sin(8*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/2048)*512;
				sinAux_4[i] = (((((unsigned int)(AMP * sin(16*PI*i/WAVE_SAMPLE_NUM) + AMP) & 0x3FF) << 6) | (1 << 16))/2048)*512;
				sinWave[i]= sinAux_1[i]+sinAux_2[i]+sinAux_3[i]+sinAux_4[i];
			}
		}
	}
	return 0;
}
