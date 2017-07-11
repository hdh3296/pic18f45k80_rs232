

#include    <pic18.h>

#include    "com.h"
#include    "serial.h"
#include    "MyUtil.h"

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






//unsigned    int  Crc;  


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


/*
void    Crc_Calulate1(unsigned int crcdata)
{
	register unsigned int    i;

   Crc=Crc ^ (crcdata & 0x00ff);
   for(i=0;i<=7;i++){
      if( (Crc & 0x0001) == 0x0001)    Crc=(Crc >> 1) ^ 0xA001;
      else Crc=Crc >> 1;
   }
}


void    Crc_Calulate(unsigned int crcdata)
{
	register unsigned int    i;

   Crc=Crc ^ (crcdata & 0x00ff);
   for(i=0;i<=7;i++){
      if( (Crc & 0x0001) == 0x0001)    Crc=(Crc >> 1) ^ 0xA001;
      else Crc=Crc >> 1;
   }
}
*/




void Interrupt_COM1Tx(void)
{
	Com1SerialTime = 0;
	Com1RxStatus = TX_SET;

	TXREG = *ptStr;
	ptStr++;		

	
	if(TXREG == '#')
	{
		TXIE = FALSE;	
		Com1RxStatus = RTX_CHK; 	

		bTxFinsh = TRUE;	 // Q1
	}
}


void Init_Com1RxBuffer(void)
{
	char i;
	
	for (i=0; i<COM1_MAX_RX_BUF; i++)	
		Com1RxBuffer[i] = 0;	
}

//
void Interrupt_COM1Rx(void)
{

   	unsigned char   buf;
	static unsigned char cntRx = 0;

   	buf = RCREG;		

	if (cntRx < COM1_MAX_RX_BUF - 1)
	cntRx++;	
	Com1RxBuffer[cntRx] = buf;			


	if (buf == '$')
	{
		Init_Com1RxBuffer();
		
		cntRx = 0;
		Com1RxBuffer[cntRx] = '$';			
	}
	else if (buf == '#')
	{
		Com1RxStatus = RX_GOOD;
	}		
 

}

/*
unsigned int   __attribute__((section(".usercode"))) Com1Crt_Protocol(unsigned char rdata)
{
   	unsigned char   buf;
   	unsigned char   temp;

	Com1RxBuffer[Com1RxCnt]=rdata;


   	buf=rdata;

    if(Com1RxStatus != TX_SET){   

		if(Com1SerialTime > 4){
            Com1RxCnt=0;
		}

        Com1SerialTime=0;
        Com1RxBuffer[Com1RxCnt]=buf;

        if(Com1RxCnt < (MAX_COM1_RXBUF-1)){
            Com1RxCnt++;
        }
        else{
            Com1RxCnt=0;
            Com1RxBuffer[Com1RxCnt]=buf;
        }

        switch(Com1RxStatus){
        	case    RTX_CHK:
                if(Com1RxCnt == 1){
                    Com1RxBuffer[0]=buf;				
                    Crc=0xffff;
                    Crc_Calulate((unsigned int)buf);
                }
                else if(Com1RxCnt == 2){
                	Crc_Calulate((unsigned int)buf);
                }
                else if(Com1RxCnt == 3){
						Com1RxStatus=DATA_CHK;
                        Crc_Calulate((unsigned int)buf);
                }                
                else{
	                Com1RxCnt=1;    
	                Com1RxBuffer[0]=buf;
	                Crc=0xffff;
	                Crc_Calulate((unsigned int)buf);
                }
        		break;
        	case    DATA_CHK:                                    
	            if(Com1RxCnt >= (Com1RxBuffer[3] + 4) ){
                    Com1RxStatus=CHKSUM_LOW_CHK;
                }
                Crc_Calulate((unsigned int)buf);
                break;
        	case    CHKSUM_LOW_CHK:
        		temp=(unsigned char)(Crc & 0x00ff);
        		if(temp==buf){
        			Com1RxStatus=CHKSUM_HIGH_CHK;
        		}
        		else    Com1RxStatus=RX_ERROR;
        		break;
        	case    CHKSUM_HIGH_CHK:
        		Crc=((Crc >> 8 ) & 0x00ff);
        		temp=(unsigned char)Crc;
        		if(buf==temp){
                    Com1RxStatus=RX_GOOD;
        		}
        		else    Com1RxStatus=RX_ERROR;
        		break;
        	case    RX_GOOD:
				break;
        	case    RX_ERROR:
        		Com1RxStatus=RTX_CHK;
        		Com1RxCnt=0;
        		break;
        	default:
        		Com1RxStatus=RTX_CHK;
        		Com1RxCnt=0;
        		break;
        }
	}	

	return(0);
}

*/







