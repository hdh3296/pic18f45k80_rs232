
#include    <pic18.h>

#include    "com.h"
#include    "serial.h"
#include    "MyUtil.h"
#include  	"cpu18f4480.h"

///////////////////////
#define     ASCTOHEX(x) ((x <= '9') ? (x - '0') : (x - '7')) 
#define     L_SHIFT(x)  (x << 4)

unsigned char 	Virt_Com1RxBuffer[6] = {0x00, 0xFE, 0x23, 0x02, 0x00, 0x00};	

unsigned char	Com1TxCnt=0;
unsigned char	Com1TxThisPt=0;

unsigned char   Com1RxStatus=0;
unsigned char	Com1RxCurCnt=0;
unsigned char	Com1SerialTime=0x0;
unsigned long	Com1BaudRate=115200;


void    init_comms(void)
{
	unsigned int tmpbaudrate;

	SPBRG = DIVIDER;     	

	tmpbaudrate=((int)(FOSC/(16UL * Com1BaudRate) -1));
	SPBRG = tmpbaudrate;     	

	TXSTA = (SPEED|NINE_BITS|0x20);
	RCSTA = (NINE_BITS|0x90);
	TRISC6=OUTPUT;
	TRISC7=INPUT;

    RCIE=1;	    // USART RX interrupt enable
    TXIE=1;	    // USART TX interrupt enable

    RCIF=0;	    // USART RX interrupt enable
    TXIF=0;	    // USART TX interrupt enable

	UART1MD=0;
	UART2MD=1;
}


void Interrupt_COM1Tx(void)
{
	Com1SerialTime = 0;
	Com1RxStatus = TX_SET;

	TXREG = *ptStr;
	ptStr++;		

	
	if(TXREG == 0x00)
	{
		TXIE = FALSE;	
		Com1RxStatus = RTX_CHK; 	

		bTxFinsh = TRUE;	 // Q1
	}
}


void Interrupt_COM1Rx(void)
{

   	unsigned char   buf;
	
	if (RCREG == 0x01)	cntRx = 0;


   	buf = RCREG;		
	Com1RxBuffer[cntRx] = buf;
	if (cntRx < COM1_MAX_RX_BUF - 1) {
		cntRx++;		
	}
	else{
		Com1RxStatus = RX_GOOD;
	}
}

void serial_interrupt()
{
	if((TXIE)&&(TXIF))										/*transmit interrupt routine*/
	{
        TXIF=0;
        Interrupt_COM1Tx();
	}	

	if( (RCIE)&&(RCIF) )										/*receive interrupt routine*/
	{
        RCIF = 0;
		if(Com1RxStatus != TX_SET)
		{
			Com1SerialTime = 0;
        	Interrupt_COM1Rx();
		}
	}	


	if(OERR) {
      	TXEN=0;
      	TXEN=1;
      	SPEN=0;
      	SPEN=1;
		CREN=0;
    }

	if( !CREN)	CREN=1;

}




