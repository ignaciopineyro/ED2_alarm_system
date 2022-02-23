#include <stdbool.h>
#include <stdint.h>

#define SYSTEM_BAUD_RATE 115200
#define CIAA_BOARD_UART USART2

typedef unsigned short uint16_t;

//-----SCU-----//

#define SCU_BASE 0x40086000							// Direccion Base SCU
#define SCU ((SCU_T*) SCU_BASE)						// Puntero a la estructura SCU

#define SCU_MODE_FUNC6		0x6						// Seleccion de la funcion 6 del pin
#define SCU_MODE_EZI		(0x1 << 6)				// Habilita buffer de entrada (deshabilita con 0)
#define SCU_MODE_ZIF_DIS	(0x1 << 7)				// Deshabilita el filtro anti glitch de entrada (habilita con 1)
#define MD_PDN				(0x3 << 3)				/** Enable pull-down resistor at pad */
#define MD_PLN						(0x2 << 3)		/** Disable pull-down and pull-up resistor at resistor at pad */

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

//-----SCB-----//
#define SCS_BASE (0xE000E000UL)         			//*!< System Control Space Base Address  */
#define SCB_BASE (SCS_BASE +  0x0D00UL)             /*!< System Control Block Base Address  */

typedef struct {
       uint32_t RESERVED0[1];
       volatile const  uint32_t ICTR;               //*!< Offset: 0x004 (R/ )  Interrupt Controller Type Register      */
       volatile uint32_t ACTLR;                  	//*!< Offset: 0x008 (R/W)  Auxiliary Control Register              */
} SCnSCB_Type;

typedef struct
{
	volatile const  uint32_t CPUID;                 //*!< Offset: 0x000 (R/ )  CPUID Base Register                                   */
	volatile uint32_t ICSR;                    		//*!< Offset: 0x004 (R/W)  Interrupt Control and State Register                  */
	volatile uint32_t VTOR;                    		//*!< Offset: 0x008 (R/W)  Vector Table Offset Register                          */
	volatile uint32_t AIRCR;                   		/*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register      */
	volatile uint32_t SCR;                     		/*!< Offset: 0x010 (R/W)  System Control Register                               */
	volatile uint32_t CCR;                     		/*!< Offset: 0x014 (R/W)  Configuration Control Register                        */
	volatile uint8_t  SHP[12];                 		/*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
	volatile uint32_t SHCSR;                  		/*!< Offset: 0x024 (R/W)  System Handler Control and State Register             */
	volatile uint32_t CFSR;                   		/*!< Offset: 0x028 (R/W)  Configurable Fault Status Register                    */
	volatile uint32_t HFSR;                    		/*!< Offset: 0x02C (R/W)  HardFault Status Register                             */
	volatile uint32_t DFSR;                    		/*!< Offset: 0x030 (R/W)  Debug Fault Status Register                           */
	volatile uint32_t MMFAR;                   		/*!< Offset: 0x034 (R/W)  MemManage Fault Address Register                      */
	volatile uint32_t BFAR;                    		/*!< Offset: 0x038 (R/W)  BusFault Address Register                             */
	volatile uint32_t AFSR;                    		/*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register                       */
	volatile const  uint32_t PFR[2];                /*!< Offset: 0x040 (R/ )  Processor Feature Register                            */
	volatile const  uint32_t DFR;                   /*!< Offset: 0x048 (R/ )  Debug Feature Register                                */
	volatile const  uint32_t ADR;                   /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register                            */
	volatile const  uint32_t MMFR[4];               /*!< Offset: 0x050 (R/ )  Memory Model Feature Register                         */
	volatile const  uint32_t ISAR[5];               /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register                   */
    uint32_t RESERVED0[5];
    volatile uint32_t CPACR;                   		/*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register                   */
} SCB_Type;

#define SCnSCB ((SCnSCB_Type*) SCS_BASE)			//*!< System control Register not in SCB */
#define SCB ((SCB_Type*) SCB_BASE)   				//*!< SCB configuration struct */

//-----SYSTICK-----//

#define _SysTick_BASE 0xE000E010					// Direccion Base SYSTICK
#define _SysTick ((SysTick_T*) _SysTick_BASE)       // Puntero a la estructura SYSTICK
#define _SysTick_CTRL_CLKSOURCE_PROC_Msk (1 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_CLKSOURCE_EXT_Msk	(0 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_TICKINT_Msk (1 << 1)			// SysTick CTRL: TICKINT Mask
#define _SysTick_CTRL_ENABLE_Msk (1 << 0)			// SysTick CTRL: ENABLE Mask
#define TICKRATE 1000								// 1000 ticks por segundo

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


//-----DAC-----//

#define DAC_BASE 0x400E1000							// Direccion Base DAC
#define DAC ((DAC_T*) DAC_BASE)						// Puntero a la estructura DAC

typedef struct {
	int CR;											// Valores de output del DAC desde bit6. Vout= VALUE/1024 * VDD.
	int CTRL;										// Cantidad de datos a sacar por el DAC
	int CNTVAL;										// Valor calculado en main
} DAC_T;


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

static inline void NVIC_SetPriority(int IRQn, uint32_t priority)
{
  if(IRQn < 0) {
    SCB->SHP[((uint32_t)(IRQn) & 0xF)-4] = ((priority << (8 - 3)) & 0xff); } /* set Priority for Cortex-M  System Interrupts */
  else {
    NVIC->IP[(uint32_t)(IRQn)] = ((priority << (8 - 3)) & 0xff);    }        /* set Priority for device specific Interrupts  */
}

//-----DMA-----//

#define GPDMA_BASE 0x40002000						// Direccion Base GPDMA
#define DMA ((GPDMA_T*) GPDMA_BASE)					// Puntero a la estructura GPDMA
#define GPDMA_NUMBER_CHANNELS 8
#define GPDMA_CLOCK		204e6						// Frecuencia de CLK de operacion de la placa

struct LLI_T {
 unsigned int  source;								// Inicio del area donde se encuentran los datos a transferir
 unsigned int  destination; 						// Inicio del area destino de los datos transferidos
 unsigned int  next;        						// Address of next strLLI in chain
 unsigned int  control;     						// DMACCxControl register
};

typedef struct {
	struct LLI_T LLI ;								// DMA Channel Source Address Register
	unsigned int  CONFIG;							// DMA Channel Configuration Register
	unsigned int  RESERVED1[3];
} _GPDMA_CH_T;

//struct LLI_T LLI0, LLI1;							// Struct de tipo LLI_T

typedef struct {
	unsigned int  INTSTAT;							// DMA Interrupt Status Register
	unsigned int  INTTCSTAT;						// DMA Interrupt Terminal Count Request Status Register
	unsigned int  INTTCCLEAR;						// DMA Interrupt Terminal Count Request Clear Register. Limpia la interrupcion anterior.
	unsigned int  INTERRSTAT;						// DMA Interrupt Error Status Register
	unsigned int  INTERRCLR;						// DMA Interrupt Error Clear Register
	unsigned int  RAWINTTCSTAT;						// DMA Raw Interrupt Terminal Count Status Register
	unsigned int  RAWINTERRSTAT;					// DMA Raw Error Interrupt Status Register
	unsigned int  ENBLDCHNS;						// DMA Enabled Channel Register
	unsigned int  SOFTBREQ;							// DMA Software Burst Request Register
	unsigned int  SOFTSREQ;							// DMA Software Single Request Register
	unsigned int  SOFTLBREQ;						// DMA Software Last Burst Request Register
	unsigned int  SOFTLSREQ;						// DMA Software Last Single Request Register
	unsigned int  CONFIG;							// DMA Configuration Register
	unsigned int  SYNC;								// DMA Synchronization Register
	unsigned int  RESERVED0[50];
	_GPDMA_CH_T   CH[GPDMA_NUMBER_CHANNELS];
} GPDMA_T;

//-----UART/USART-----//
#define USART0_BASE		0x40081000
#define USART2_BASE		0x400C1000
#define USART3_BASE		0x400C2000
#define UART1_BASE		0x40082000

#define USART0			((USART_T	*) USART0_BASE)
#define USART2			((USART_T	*) USART2_BASE)
#define USART3			((USART_T	*) USART3_BASE)
#define UART1			((USART_T	*) UART1_BASE)

#define _UART_IER_RBRINT	(1 << 0)	// RBR Interrupt enable

#define UART_FCR_FIFO_EN        (1 << 0)	/*!< UART FIFO enable */
#define UART_FCR_RX_RS          (1 << 1)	/*!< UART RX FIFO reset */
#define UART_FCR_TX_RS          (1 << 2)	/*!< UART TX FIFO reset */
#define UART_LCR_WLEN8          (3 << 0)	/*!< UART word length select: 8 bit data mode */
#define UART_LCR_SBS_1BIT       (0 << 2)	/*!< UART stop bit select: 1 stop bit */
#define UART_LCR_PARITY_DIS     (0 << 3)	/*!< UART Parity Disable */
#define UART_LCR_DLAB_EN        (1 << 7)		/*!< UART Divisor Latches Access bit enable */
#define UART_FCR_TRG_LEV0       (0)			/*!< UART FIFO trigger level 0: 1 character */
#define UART_TER2_TXEN      (1 << 0)		/*!< Transmit enable bit  - valid for 18xx/43xx only */


typedef struct {

	union {
		unsigned int  DLL; // Divisor Latch LSB. Least significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1).
		unsigned int  THR; // Transmit Holding Register. The next character to be transmitted is written here (DLAB = 0).
		unsigned int  RBR; // Receiver Buffer Register. Contains the next received character to be read (DLAB = 0).
	};

	union {
		unsigned int IER;	// Interrupt Enable Register. Contains individual interrupt enable bits for the 7 potential UART interrupts (DLAB = 0).
		unsigned int DLM;	// Divisor Latch MSB. Most significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1).
	};

	union {
		unsigned int FCR;	// FIFO Control Register. Controls UART FIFO usage and modes.
		unsigned int IIR;	// Interrupt ID Register. Identifies which interrupt(s) are pending.
	};

	unsigned int LCR;		// Line Control Register. Contains controls for frame formatting and break generation.
	unsigned int MCR;		// Modem Control Register. Only present on USART ports with full modem support.
	unsigned int LSR;		// Line Status Register. Contains flags for transmit and receive status, including line errors.
	unsigned int MSR;		// Modem Status Register. Only present on USART ports with full modem support.
	unsigned int SCR;		// Scratch Pad Register. Eight-bit temporary storage for software.
	unsigned int ACR;		// Auto-baud Control Register. Contains controls for the auto-baud feature.
	unsigned int ICR;		// IrDA control register (not all UARTS)
	unsigned int FDR;		// Fractional Divider Register. Generates a clock input for the baud rate divider.
	unsigned int OSR;		// Oversampling Register. Controls the degree of oversampling during each bit time. Only on some UARTS.
	unsigned int TER1;		// Transmit Enable Register. Turns off USART transmitter for use with software flow control.
	unsigned int RESERVED0[3];
    unsigned int HDEN;		// Half-duplex enable Register- only on some UARTs
	unsigned int RESERVED1[1];
	unsigned int SCICTRL;	// Smart card interface control register- only on some UARTs

	unsigned int RS485CTRL;	// RS-485/EIA-485 Control. Contains controls to configure various aspects of RS-485/EIA-485 modes.
	unsigned int RS485ADRMATCH;	// RS-485/EIA-485 address match. Contains the address match value for RS-485/EIA-485 mode.
	unsigned int RS485DLY;		// RS-485/EIA-485 direction control delay.

	union {
		unsigned int SYNCCTRL;	// Synchronous mode control register. Only on USARTs.
		unsigned int FIFOLVL;	// FIFO Level register. Provides the current fill levels of the transmit and receive FIFOs.
	};

	unsigned int TER2;			// Transmit Enable Register. Only on LPC177X_8X UART4 and LPC18XX/43XX USART0/2/3.
} USART_T;

static inline void UART_SendByte(USART_T *pUART, unsigned char data){
	pUART->THR = (unsigned int) data;
}

//-----Encabezados-----//
void NVIC_init(void);
void systick_init(void);
void LEDs_init(void);
void DMA_init(void);
void ADC_init(void);
void puls_init(void);
void display_init(void);
void UART_init(void);

int sprintf_mio(char *, const char *, ...);
unsigned int readADC(void);
