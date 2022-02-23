#include <headers.h>
#include <clocks.h>

/* //-----Lectura del ADC-----//
unsigned int readADC(void){
	ADC -> CR = (0 << 24);									// Start en 0 para activar el Burst
	ADC -> CR = (1 << 16);									// Burst en 1

	while(!(ADC -> STAT & (1 << 1))){						// Espera a que termine la conversion
		printf('\nWaiting for convertion');
	}

	ADC -> CR = (1 << 24);									// Reactivo el Start
	//temperature = ADC -> DR1;
	//printf("\nTemperature = %d", temperature);
	//return (unsigned int) temperature;
} */

//-----NVIC-----//
void NVIC_init(void){
	//NVIC -> ISER[0] = (1<<2);   							// Habilita las interrupciones del DMA
	NVIC -> ISER[1] = (1<<0) | (1<<1) | (1<<3);  			// Habilita las interrupciones de los GPIO (Pulsadores)
}

//-----SYSTICK-----//
void systick_init(void){
	_SysTick -> LOAD = (GPDMA_CLOCK / TICKRATE) - 1;		// Set reload register
	_SysTick -> VAL  = 0;									// Se limpia el valor
	_SysTick -> CTRL = _SysTick_CTRL_CLKSOURCE_PROC_Msk |	// Se usa el reloj del sistema (204 MHz)
					   _SysTick_CTRL_TICKINT_Msk   		|
					   _SysTick_CTRL_ENABLE_Msk;			// Habilitacion del contador
}

//-----LEDs-----//
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

/* //-----DMA-----//
void DMA_init(void){
	LLI0.source = (unsigned int) &(ADC -> GDR);				// Se le pasa solo la primer posicion del valor fuente (GDR del ADC, VREF ya esta en bit6, no hace falta correrlo luego)
	//LLI0.destination = (unsigned int) &temp[0];				// Se le pasa el mismo vector que usa el DAC para sacar las muestras.
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
} */

//-----ADC0_1-----//
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

//-----PULSADORES-----//
void puls_init(void) {
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

//-----DISPLAY-----//
void display_init(void){
	// TODO: Configurar pines del display
}

//-----Clocks-----//
uint32_t Chip_Clock_GetRate(CHIP_CCU_CLK_T clk)
{
	CHIP_CGU_BASE_CLK_T baseclk;
	uint32_t reg, div, rate;

	/* Get CCU config register for clock */
	if (clk >= CLK_CCU2_START) {
		reg = LPC_CCU2->CLKCCU[clk - CLK_CCU2_START].CFG;
	}
	else {
		reg = LPC_CCU1->CLKCCU[clk].CFG;
	}

	/* Is the clock enabled? */
	if (reg & 1) {
		/* Get base clock for this peripheral clock */
		baseclk = Chip_Clock_FindBaseClock(clk);

		/* Get base clock rate */
		rate = Chip_Clock_GetBaseClocktHz(baseclk);

		/* Get divider for this clock */
		if (((reg >> 5) & 0x7) == 0) {
			div = 1;
		}
		else {
			div = 2;/* No other dividers supported */

		}
		rate = rate / div;
	}
	else {
		rate = 0;
	}

	return rate;
}

static CHIP_CGU_BASE_CLK_T Chip_Clock_FindBaseClock(CHIP_CCU_CLK_T clk)
{
	CHIP_CGU_BASE_CLK_T baseclk = CLK_BASE_NONE;
	int i = 0;

	while ((baseclk == CLK_BASE_NONE) && (periph_to_base[i].clkbase != baseclk)) {
		if ((clk >= periph_to_base[i].clkstart) && (clk <= periph_to_base[i].clkend)) {
			baseclk = periph_to_base[i].clkbase;
		}
		else {
			i++;
		}
	}

	return baseclk;
}

uint32_t Chip_Clock_GetClockInputHz(CHIP_CGU_CLKIN_T input)
{
	uint32_t rate = 0;

	switch (input) {
	case CLKIN_32K:
		rate = CRYSTAL_32K_FREQ_IN;
		break;

	case CLKIN_IRC:
		rate = CGU_IRC_FREQ;
		break;

	case CLKIN_ENET_RX:
		if ((LPC_CREG->CREG6 & 0x07) != 0x4) {
			/* MII mode requires 25MHz clock */
			rate = 25000000;
		}
		break;

	case CLKIN_ENET_TX:
		if ((LPC_CREG->CREG6 & 0x07) != 0x4) {
			rate = 25000000; /* MII uses 25 MHz */
		} else {
			rate = 50000000; /* RMII uses 50 MHz */
		}
		break;

	case CLKIN_CLKIN:
		rate = ExtRateIn;
		break;

	case CLKIN_CRYSTAL:
		rate = OscRateIn;
		break;

	case CLKIN_USBPLL:
		rate = audio_usb_pll_freq[CGU_USB_PLL];
		break;

	case CLKIN_AUDIOPLL:
		rate = audio_usb_pll_freq[CGU_AUDIO_PLL];
		break;

	case CLKIN_MAINPLL:
		rate = Chip_Clock_GetMainPLLHz();
		break;

	case CLKIN_IDIVA:
		rate = Chip_Clock_GetDivRate(input, CLK_IDIV_A);
		break;

	case CLKIN_IDIVB:
		rate = Chip_Clock_GetDivRate(input, CLK_IDIV_B);
		break;

	case CLKIN_IDIVC:
		rate = Chip_Clock_GetDivRate(input, CLK_IDIV_C);
		break;

	case CLKIN_IDIVD:
		rate = Chip_Clock_GetDivRate(input, CLK_IDIV_D);
		break;

	case CLKIN_IDIVE:
		rate = Chip_Clock_GetDivRate(input, CLK_IDIV_E);
		break;

	case CLKINPUT_PD:
		rate = 0;
		break;

	default:
		break;
	}

	return rate;
}

uint32_t Chip_Clock_GetBaseClocktHz(CHIP_CGU_BASE_CLK_T clock)
{
	return Chip_Clock_GetClockInputHz(Chip_Clock_GetBaseClock(clock));
}
