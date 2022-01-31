#define OUT_PORT PORT0
//#define OUT_BIT  Bit17
#define OUT_BIT  Bit20

//#define IN_PORT  P0_16
#define IN_PORT  P0_18

#define PWM_FREQ 100       // in cycles/second
#define PWM_RES  100       // in counts/cycle
#define WKT_FREQ 1000000   // Use if the WKT is clocked by the LPOSC
//#define WKT_FREQ 937500  // Use if the WKT is clocked by the 15 MHz FRO, via the div-by-16 WKT divider
#define WKT_RELOAD 10      // Reload value for the WKT down counter
#define WKT_INT_FREQ (WKT_FREQ/WKT_RELOAD)



void setup_debug_uart(void);
void WKT_Config(void);


