
#include    <pic18.h>

#include        "you_can.h"
#include        "com.h"
#include        "cpu18f4480.h"

unsigned    char  	MainTimer=0;
unsigned    char	msec100=0;

bit bVirt_RxGood = 0;
unsigned int Virt_RxGoodTimer = 0;


void    PortInit(void)
{
	TRISA=0;
	TRISB=0;
	TRISC=0;
	TRISD=0;
	TRISE=0;

  	LATA=0xff;	
  	LATB=0xff;	
  	LATC=0xff;	
  	LATD=0xff;	
  	LATE=0xff;	
}


void    UserBaudRate(void)
{
	unsigned char val;
	val=0;

	TRISE0=1;
	TRISE1=1;
	TRISE2=1;
	if(!RE0)	val= (val | 0x01);		
	if(!RE1)	val= (val | 0x02);		
	if(!RE2)	val= (val | 0x04);
	switch(val){
		case	0:	
			Com1BaudRate=9600;
			break;
		case	1:	// �뼺 ���ùݿ� 
			Com1BaudRate=19200;
			TX_DATA_CNT = 40; 
			break;
		case	2:	// ���� ���ùݿ� 
			Com1BaudRate=38400;
			TX_DATA_CNT = 88; 
			break;
		case	3:	// LCD JUTF��
			Com1BaudRate=115200;
			TX_DATA_CNT = 40; 
			break;
		default:
			Com1BaudRate=115200;
			break;		
	}			
}


void LoadCom1buf_StartLCDCmd(unsigned char *buf)
{
	char i, j;

	// 초기화 
	for (i=0; i<MAX_PACKET; i++)
	{
		com1_buf[i] = 0;
	}	
	
	for (i=0; buf[i]!=0; i++)
	{
		com1_buf[i] = buf[i];
	}

	SetCom1TxEnable();
}


void SetCom1TxEnable(void)
{
	ptStr = com1_buf; // Q1
	TXREG = *ptStr;
	ptStr++;
	TXIE = TRUE; 
	LED_TX = !LED_TX;
}

/*
// Rx 데이타를 받으면 받은 데이타를 Tx로 다시 돌려 보내는 프로그램이다. 	 
*/
void main(void)
{
	unsigned char i;	
    
    di();
    Initial();
    PortInit();
    Timer0Init(); 
	UserBaudRate();
    init_comms();
    ei();

	MainTimer=0;
	msec100=0;
	Com1SerialTime=0;		

	TXIE=0;
	RCIE=1;

	TRISD5=0;
	TX485_EN=0;

	Com1RxStatus=RTX_CHK; 

	TXIE = 0;
	strPt = 0;

	LED_TX 	= 0;
	LED_RX 	= 1;
	LED_RUN = 0;

    while (1)
	{ 		
        CLRWDT();

		
		if(Com1RxStatus == RX_GOOD){
			Com1RxStatus = RTX_CHK;
			LED_RX = !LED_RX; 	
			LoadCom1buf_StartLCDCmd(Com1RxBuffer);
		}
		
	 
		if(Com1RxStatus == TX_SET){
			if(Com1SerialTime > 3){
				Com1SerialTime=0;
				Com1RxStatus=RTX_CHK; 
				Com1TxThisPt=0;
				Com1TxCnt=0;
				TXIE=0;
				TX485_EN=0;
			}
		}
    }
}

	

void interrupt isr(void)
{
   
    if( TMR0IF )		
    {    
        TMR0IF = 0 ;
        TMR0L=MSEC_L;
        TMR0H=MSEC_H;    

        Com1SerialTime++;
        msec100++;

        if(msec100>100){
            msec100=0;
			MainTimer++;
			if(MainTimer > 5){
				LED_RUN=!LED_RUN;
			}				
        } 
    }

	serial_interrupt();	
}




