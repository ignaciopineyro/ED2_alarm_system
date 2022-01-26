#include <stdbool.h>

#define SYSTEM_BAUD_RATE 115200
#define CIAA_BOARD_UART USART2

//-----SCU-----//

#define SCU_BASE			0x40086000								// Direccion Base SCU
#define DAC_BASE			0x400E1000								// Direccion Base DAC
#define ADC_BASE			0x400E3000								// Direccion Base ADC
#define NVIC_BASE			0xE000E100								// Direccion Base NVIC
#define GPDMA_BASE			0x40002000								// Direccion Base GPDMA
#define SCU				    ((SCU_T*) SCU_BASE)						// Puntero a la estructura SCU
#define DAC                 ((DAC_T*) DAC_BASE)						// Puntero a la estructura DAC
#define ADC                 ((ADC_T*) ADC_BASE)						// Puntero a la estructura ADC
#define NVIC				((NVIC_Type*) NVIC_BASE)				// Puntero a la estructura NVIC
#define DMA              	((GPDMA_T*) GPDMA_BASE)					// Puntero a la estructura GPDMA

//-----GPIO-----//

#define GPIO_PORT_BASE			0x400F4000							// Direccion Base GPIO
#define PIN_INT_BASE			0x40087000							// Direccion Base PIN_INT
#define GPIO_PORT				((GPIO_T *)	GPIO_PORT_BASE)			// Puntero a la estructura GPIO
#define GPIO_PIN_INT			((PIN_INT_T *) PIN_INT_BASE)		// Puntero a la estructura PIN_INT


//-----STRUCTS-----//

// SCU
typedef struct {
	int  SFSP[16][32];				// P0 a P9 y PA a PF
	int  RESERVED0[256];			// Reservado
	int  SFSCLK[4];					// Pin configuration register for pins CLK0-3
	int  RESERVED16[28];			// Reservado
	int  SFSUSB;					// Pin configuration register for USB
	int  SFSI2C0;					// Pin configuration register for I2C0-bus pins
	int  ENAIO[3];					// Analog function select registers
	int  RESERVED17[27];			// Reservado
	int  EMCDELAYCLK;				// EMC clock delay register
	int  RESERVED18[63];			// Reservado
	int  PINTSEL[2];				// Pin interrupt select register for pin int 0 to 3 index 0, 4 to 7 index 1
} SCU_T;

// GPIO
typedef struct {					// Estructura para GPIO
	unsigned char B[128][32];		// Offset 0x0000: Byte pin registers ports 0 to n; pins PIOn_0 to PIOn_31 */
	int W[32][32];					// Offset 0x1000: Word pin registers port 0 to n
	int DIR[32];					// Offset 0x2000: Direction registers port n
	int MASK[32];					// Offset 0x2080: Mask register port n
	int PIN[32];					// Offset 0x2100: Portpin register port n
	int MPIN[32];					// Offset 0x2180: Masked port register port n
	int SET[32];					// Offset 0x2200: Write: Set register for port n Read: output bits for port n
	int CLR[32];					// Offset 0x2280: Clear port n
	int NOT[32];					// Offset 0x2300: Toggle port n
} GPIO_T;

// DAC
typedef struct {
	int CR;							// Valores de output del DAC desde bit6. Vout= VALUE/1024 * VDD.
	int CTRL;						// Cantidad de datos a sacar por el DAC
	int CNTVAL;						// Valor calculado en main
} DAC_T;

// ADC
typedef struct {
	int CR;							// A/D Control Register (P. 1331, T.1113)
	int GDR;						// A/D Global Data Register (P. 1332, T.1114)
	int RESERVED;					// Reservado
	int INTEN;						// A/D Interrupt Enable Register (P. 1333)
	int DR0;  						// A/D Channel 0 Data Register (P. 1333, T.1116)
	int DR1;  						// A/D Channel 1 Data Register
	int DR2;  						// A/D Channel 2 Data Register
	int DR3;  						// A/D Channel 3 Data Register
	int DR4;  						// A/D Channel 4 Data Register
	int DR5;  						// A/D Channel 5 Data Register
	int DR6;  						// A/D Channel 6 Data Register
	int DR7;  						// A/D Channel 7 Data Register
	int STAT;						// A/D Status Register (P. 1334, T.1117)
} ADC_T;

// PIN INT
typedef struct {
	int ISEL;						// Pin Interrupt Mode register. Elegir el tipo de disparo (flanco o nivel)
	int IENR;						// Pin Interrupt Enable (Rising) register
	int SIENR;						// Set Pin Interrupt Enable (Rising) register
	int CIENR;						// Clear Pin Interrupt Enable (Rising) register
	int IENF;						// Pin Interrupt Enable Falling Edge / Active Level register
	int SIENF;						// Set Pin Interrupt Enable Falling Edge / Active Level register. Flanco de bajada (depende de la placa, cuando soltas/apretas el boton se activa)
	int CIENF;						// Clear Pin Interrupt Enable Falling Edge / Active Level address
	int RISE;						// Pin Interrupt Rising Edge register
	int FALL;						// Pin Interrupt Falling Edge register
	int IST;						// Pin Interrupt Status register
} PIN_INT_T;

// NVIC
typedef struct {
	int ISER[8];					// Offset: 0x000 (R/W)  Interrupt Set Enable Register (Elegir vector de interrupciones de 32b)
	int RESERVED0[24];				// Reservado
	int ICER[8];					// Offset: 0x080 (R/W)  Interrupt Clear Enable Register
	int RSERVED1[24];				// Reservado
	int ISPR[8];					// Offset: 0x100 (R/W)  Interrupt Set Pending Register
	int RESERVED2[24];				// Reservado
	int ICPR[8];					// Offset: 0x180 (R/W)  Interrupt Clear Pending Register
	int RESERVED3[24];				// Reservado
	int IABR[8];					// Offset: 0x200 (R/W)  Interrupt Active bit Register
	int RESERVED4[56];				// Reservado
	unsigned char IP[240];			// Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide)
    int RESERVED5[644];				// Reservado
	int STIR;						// Offset: 0xE00 ( /W)  Software Trigger Interrupt Register
} NVIC_Type;

//-----DMA-----//

#define GPDMA_NUMBER_CHANNELS 8

// DMA LLI
struct LLI_T {
 unsigned int  source;				// Inicio del area donde se encuentran los datos a transferir
 unsigned int  destination; 		// Inicio del area destino de los datos transferidos
 unsigned int  next;        		// Address of next strLLI in chain
 unsigned int  control;     		// DMACCxControl register
};

// DMA CHANNELS
typedef struct {
	struct LLI_T LLI ;				// DMA Channel Source Address Register
	unsigned int  CONFIG;			// DMA Channel Configuration Register
	unsigned int  RESERVED1[3];
} _GPDMA_CH_T;

// GPDMA
typedef struct {
	unsigned int  INTSTAT;			// DMA Interrupt Status Register
	unsigned int  INTTCSTAT;		// DMA Interrupt Terminal Count Request Status Register
	unsigned int  INTTCCLEAR;		// DMA Interrupt Terminal Count Request Clear Register. Limpia la interrupcion anterior.
	unsigned int  INTERRSTAT;		// DMA Interrupt Error Status Register
	unsigned int  INTERRCLR;		// DMA Interrupt Error Clear Register
	unsigned int  RAWINTTCSTAT;		// DMA Raw Interrupt Terminal Count Status Register
	unsigned int  RAWINTERRSTAT;	// DMA Raw Error Interrupt Status Register
	unsigned int  ENBLDCHNS;		// DMA Enabled Channel Register
	unsigned int  SOFTBREQ;			// DMA Software Burst Request Register
	unsigned int  SOFTSREQ;			// DMA Software Single Request Register
	unsigned int  SOFTLBREQ;		// DMA Software Last Burst Request Register
	unsigned int  SOFTLSREQ;		// DMA Software Last Single Request Register
	unsigned int  CONFIG;			// DMA Configuration Register
	unsigned int  SYNC;				// DMA Synchronization Register
	unsigned int  RESERVED0[50];
	_GPDMA_CH_T   CH[GPDMA_NUMBER_CHANNELS];
} GPDMA_T;


/*
 //-----SCU-----//

#define SCU_BASE				0x40086000	// Direccion del System Control Unit

#define	SFSP1_0					0x080		// Offset del registro de configurcion PULS_0
#define	SFSP1_1					0x084		// Offset del registro de configurcion PULS_1
#define	SFSP1_2					0x088		// Offset del registro de configurcion PULS_2
#define	SFSP1_6					0x098		// Offset del registro de configurcion PULS_3

#define	SFSP2_0					0x100		// Offset del registro de configurcion LED0_R
#define	SFSP2_1					0x104		// Offset del registro de configurcion LED0_G
#define	SFSP2_2					0x108		// Offset del registro de configurcion LED0_B
#define	SFSP2_10				0x128		// Offset del registro de configurcion LED1
#define	SFSP2_11				0x12C		// Offset del registro de configurcion LED2
#define	SFSP2_12				0x130		// Offset del registro de configurcion LED3

//-----GPIO-----//

#define GPIO_BASE						0x400F4000	// Direccion del GPIO

#define	GPIO_PORT0_DIR_OFFSET			0x2000		// Offset del registro de direccion (DIR) del puerto 0
#define GPIO_PORT0_CLR_OFFSET			0x2280  	// Offset del registro clear (CLR) del puerto 0
#define GPIO_PORT0_SET_OFFSET			0x2200		// Offset del registro set (SET) del puerto 0

#define	GPIO_PORT1_DIR_OFFSET			0x2004		// Offset del registro de direccion (DIR) del puerto 1
#define GPIO_PORT1_CLR_OFFSET			0x2284		// Offset del registro clear (CLR) del puerto 1
#define GPIO_PORT1_SET_OFFSET			0x2204		// Offset del registro set (SET) del puerto 1

#define	GPIO_PORT5_DIR_OFFSET			0x2014		// Offset del registro de direccion (DIR) del puerto 5
#define GPIO_PORT5_CLR_OFFSET			0x2294		// Offset del registro clear (CLR) del puerto 5
#define GPIO_PORT5_SET_OFFSET			0x2214		// Offset del registro set (SET) del puerto 5

#define GPIO_PORT0_PIN4_B_OFFSET		0x0004		// Offset del registro byte (B) del puerto 0 PULS0
#define GPIO_PORT0_PIN8_B_OFFSET		0x0008		// Offset del registro byte (B) del puerto 0 PULS1
#define GPIO_PORT0_PIN9_B_OFFSET		0x0009		// Offset del registro byte (B) del puerto 0 PULS2
#define GPIO_PORT1_PIN9_B_OFFSET		0x0020		// Offset del registro byte (B) del puerto 1 PULS3


//-----SysTick-----//

#define _SysTick_BASE		0xE000E010							// Systick Base Address

#define _SysTick_CTRL_CLKSOURCE_PROC_Msk	(1 << 2)	// Mask para CLK Source interna (Bit2 en 1 para usar interna)
#define _SysTick_CTRL_CLKSOURCE_EXT_Msk		(0 << 2)	// Mask para CLK Source externa (Bit2 en 0 para usar interna)
#define _SysTick_CTRL_TICKINT_Msk	(1 << 1)	        // Mask para TICKINT: Habilita la interrupcion cuando el timer llega a 0
#define _SysTick_CTRL_ENABLE_Msk	(1 << 0)	        // Mask para ENABLE: Habilita el timer

#define _SysTick	      ((SysTick_T *) _SysTick_BASE) //Puntero para usar una struct _SysTick_T


//-----NVIC-----//

#define NVIC_BASE		0xE000E000
#define ISER0_B			0x100
#define ISER1_B			0x104
#define ICER0_B			0x180
#define ICER1_B			0x184

//-----STRUCTS-----//

typedef struct LED{
	int *conf;
	int *dir;
	int *clr;
	int *set;
	int pin;
}LED;

typedef struct PULS{
	int *conf;
	int *dir;
	int *clr;
	int *set;
	int pin;
	int *reg;
}PULS;

typedef struct SysTick{
  int CTRL;					// Offset: 0x000 (R/W)  SysTick Control and Status Register
  int LOAD;					// Offset: 0x004 (R/W)  SysTick Reload Value Register
  int VAL;					// Offset: 0x008 (R/W)  SysTick Current Value Register
  int CALIB;				// Offset: 0x00C (R/ )  SysTick Calibration Register
} SysTick_T;

typedef struct NVIC{
	int *ISER0;
	int *ISER1;
	int *ICER0;
	int *ICER1;
}NVIC_s;

typedef struct GPIO_Pint{
	unsigned int ISEL;
	unsigned int IENR;
	unsigned int SIENR;
	unsigned int CIENR;
	unsigned int IENF;
	unsigned int SIENF;
	unsigned int CIENF;
	unsigned int RISE;
	unsigned int FALL;
	unsigned int IST;
}GPIO_pint_s;


//-----Prototipos-----//

void led_status(char led, int state);  //Función para prender (1) y apagar (0) LED1/LED2/LED3/LEDR/LEDG/LEDB

*/
