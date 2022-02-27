#include <stdbool.h>


//-----SCU-----//

#define SCU_BASE 0x40086000							// Direccion Base SCU
#define SCU ((SCU_T*) SCU_BASE)						// Puntero a la estructura SCU

typedef struct {
	int  SFSP[16][32];								// P0 a P9 y PA a PF
	int  RESERVED0[256];							// Reservado
	int  SFSCLK[4];									// Pin configuration register for pins CLK0-3
	int  RESERVED16[28];							// Reservado
	int  SFSUSB;									// Pin configuration register for USB
	int  SFSI2C0;									// Pin configuration register for I2C0-bus pins
	int  ENAIO[3];									// Analog function select registers
	int  RESERVED17[27];							// Reservado
	int  EMCDELAYCLK;								// EMC clock delay register
	int  RESERVED18[63];							// Reservado
	int  PINTSEL[2];								// Pin interrupt select register for pin int 0 to 3 index 0, 4 to 7 index 1
} SCU_T;


//-----SYSTICK-----//

#define _SysTick_BASE 0xE000E010					// Direccion Base SYSTICK
#define _SysTick ((SysTick_T*) _SysTick_BASE)       // Puntero a la estructura SYSTICK
#define _SysTick_CTRL_CLKSOURCE_PROC_Msk (1 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_CLKSOURCE_EXT_Msk	(0 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_TICKINT_Msk (1 << 1)			// SysTick CTRL: TICKINT Mask
#define _SysTick_CTRL_ENABLE_Msk (1 << 0)			// SysTick CTRL: ENABLE Mask

typedef struct {
  int CTRL;											// Offset: 0x000 (R/W)  SysTick Control and Status Register
  int LOAD;											// Offset: 0x004 (R/W)  SysTick Reload Value Register
  int VAL;											// Offset: 0x008 (R/W)  SysTick Current Value Register
  int CALIB;										// Offset: 0x00C (R/ )  SysTick Calibration Register
} SysTick_T;


//-----GPIO-----//

#define GPIO_PORT_BASE 0x400F4000					// Direccion Base GPIO
#define PIN_INT_BASE 0x40087000						// Direccion Base PIN_INT
#define GPIO_PORT ((GPIO_T*) GPIO_PORT_BASE)		// Puntero a la estructura GPIO
#define GPIO_PIN_INT ((PIN_INT_T *) PIN_INT_BASE)	// Puntero a la estructura PIN_INT

typedef struct {									// Estructura para GPIO
	unsigned char B[128][32];						// Offset 0x0000: Byte pin registers ports 0 to n; pins PIOn_0 to PIOn_31 */
	int W[32][32];									// Offset 0x1000: Word pin registers port 0 to n
	int DIR[32];									// Offset 0x2000: Direction registers port n
	int MASK[32];									// Offset 0x2080: Mask register port n
	int PIN[32];									// Offset 0x2100: Portpin register port n
	int MPIN[32];									// Offset 0x2180: Masked port register port n
	int SET[32];									// Offset 0x2200: Write: Set register for port n Read: output bits for port n
	int CLR[32];									// Offset 0x2280: Clear port n
	int NOT[32];									// Offset 0x2300: Toggle port n
} GPIO_T;

typedef struct {
	int ISEL;										// Pin Interrupt Mode register. Elegir el tipo de disparo (flanco o nivel)
	int IENR;										// Pin Interrupt Enable (Rising) register
	int SIENR;										// Set Pin Interrupt Enable (Rising) register
	int CIENR;										// Clear Pin Interrupt Enable (Rising) register
	int IENF;										// Pin Interrupt Enable Falling Edge / Active Level register
	int SIENF;										// Set Pin Interrupt Enable Falling Edge / Active Level register. Flanco de bajada (depende de la placa, cuando soltas/apretas el boton se activa)
	int CIENF;										// Clear Pin Interrupt Enable Falling Edge / Active Level address
	int RISE;										// Pin Interrupt Rising Edge register
	int FALL;										// Pin Interrupt Falling Edge register
	int IST;										// Pin Interrupt Status register
} PIN_INT_T;


//-----ADC-----//

#define ADC_BASE 0x400E3000							// Direccion Base ADC
#define ADC ((ADC_T*) ADC_BASE)						// Puntero a la estructura ADC

typedef struct {
	int CR;											// A/D Control Register (P. 1331, T.1113)
	int GDR;										// A/D Global Data Register (P. 1332, T.1114)
	int RESERVED;									// Reservado
	int INTEN;										// A/D Interrupt Enable Register (P. 1333)
	int DR0;  										// A/D Channel 0 Data Register (P. 1333, T.1116)
	int DR1;  										// A/D Channel 1 Data Register
	int DR2;  										// A/D Channel 2 Data Register
	int DR3;  										// A/D Channel 3 Data Register
	int DR4;  										// A/D Channel 4 Data Register
	int DR5;  										// A/D Channel 5 Data Register
	int DR6;  										// A/D Channel 6 Data Register
	int DR7;  										// A/D Channel 7 Data Register
	int STAT;										// A/D Status Register (P. 1334, T.1117)
} ADC_T;


//-----NVIC-----//

#define NVIC_BASE 0xE000E100						// Direccion Base NVIC
#define NVIC ((NVIC_Type*) NVIC_BASE)				// Puntero a la estructura NVIC

typedef struct {
	int ISER[8];									// Offset: 0x000 (R/W)  Interrupt Set Enable Register (Elegir vector de interrupciones de 32b)
	int RESERVED0[24];								// Reservado
	int ICER[8];									// Offset: 0x080 (R/W)  Interrupt Clear Enable Register
	int RSERVED1[24];								// Reservado
	int ISPR[8];									// Offset: 0x100 (R/W)  Interrupt Set Pending Register
	int RESERVED2[24];								// Reservado
	int ICPR[8];									// Offset: 0x180 (R/W)  Interrupt Clear Pending Register
	int RESERVED3[24];								// Reservado
	int IABR[8];									// Offset: 0x200 (R/W)  Interrupt Active bit Register
	int RESERVED4[56];								// Reservado
	unsigned char IP[240];							// Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide)
    int RESERVED5[644];								// Reservado
	int STIR;										// Offset: 0xE00 ( /W)  Software Trigger Interrupt Register
} NVIC_Type;


//-----Funciones-----//

void NVIC_init(void);
void systick_init(void);
void LEDs_init(void);
void ADC_init(void);
void pulsadores_init(void);
void display_init(void);
