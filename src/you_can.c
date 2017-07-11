


#include    <pic18.h>


#include "you_can.h" 
#include "com.h" 
#include "crt_45k80.h" 
#include "MyUtil.h" 
#include "you_can_oldProtocol.h" 



#if defined(CPU65K80)  || defined(CPU45K80)

	#define	TXB0REQ		TXB0CONbits.TXREQ

	#define	RXB0FUL		RXB0CONbits.RXFUL
		// RXFUL: Receive Full Status bit (1)
		// 1 = Receive buffer contains a received message, 0 = Receive buffer is open to receive a new message
	#define	RXB0FILHIT0 RXB0CONbits.FILHIT0
	#define	RXB0FILHIT1 RXB0CONbits.FILHIT1
	#define	RXB0FILHIT2 RXB0CONbits.FILHIT2
	#define	RXB0RTRRO 	RXB0CONbits.RXRTRRO

	#define	TXB1REQ		TXB1CONbits.TXREQ
	#define	RXB1FUL		RXB1CONbits.RXFUL
	#define	RXB1FILHIT0 RXB1CONbits.FILHIT0
	#define	RXB1FILHIT1 RXB1CONbits.FILHIT1
	#define	RXB1FILHIT2 RXB1CONbits.FILHIT2
	#define	RXB1RTRRO 	RXB1CONbits.RXRTRRO


	#define	RXB0M0		RXB0CONbits.RXM0
	#define	RXB0M1		RXB0CONbits.RXM1
	#define	RXB0DBEN	RXB0CONbits.RB0DBEN
				


	#define	RXB1M0		RXB1CONbits.RXM0
	#define	RXB1M1		RXB1CONbits.RXM1
	#define	RXB1DBEN	RXB1CONbits.RB1DBEN


	#define	RXB0IF		PIR5bits.RXB0IF
	#define	RXB1IF		PIR5bits.RXB1IF


	#define	RXF0EXIDEN	RXF0SIDLbits.EXIDEN
	#define	RXF1EXIDEN	RXF1SIDLbits.EXIDEN
	#define	RXF2EXIDEN	RXF2SIDLbits.EXIDEN
	#define	RXF3EXIDEN	RXF3SIDLbits.EXIDEN
	#define	RXF4EXIDEN	RXF4SIDLbits.EXIDEN
	#define	RXF5EXIDEN	RXF5SIDLbits.EXIDEN



	#define	TXB0IE		TXBIEbits.TXB0IE
	#define	TXB1IE		TXBIEbits.TXB1IE
	#define	TXB2IE		TXBIEbits.TXB2IE

	#define	RXB0IE		BIE0bits.RXB0IE
	#define	RXB1IE		BIE0bits.RXB1IE


	#define	TXB2REQ		TXB2CONbits.TXREQ

	#define	TXB0RTR		TXB0DLCbits.TXRTR

#endif



unsigned    long    Can1TxEid=0;
unsigned    int     Can1TxSid=0;
unsigned    char    Can1TxDlc=0;
unsigned    char	Can1TxDataTotalCnt=0;
unsigned    char    Can1TxDataTxPointer=0;


unsigned    long    Can1RxEid=0;
unsigned    int     Can1RxSid=0;
unsigned    char    Can1RxDlc=0;
unsigned    char	Can1RxDataTotalCnt=0;
unsigned    char    Can1RxDataRxPointer=0;

unsigned    char    Can1TxBuf[CAN1_MAX_TX_BUF];           
unsigned    char    Can1RxBuf[CAN1_MAX_RX_BUF];           

unsigned    char    Can1ThisRxBuf[8];           


///////////////////////

void Can1PortSetting(void)//------------------------------------------------------------------------
{
   	TRISB3 = 1 ; // CAN RX
   	TRISB2 = 0 ; // CAN TX 


	//요청(Request) CAN 동작 모드 비트	
#ifdef		 U_COMPILER		1
	CANCONbits.REQOP = 4 ;// Entry Configration Mode
#else
	REQOP2 = 1 ;// Entry Configration Mode 구성모드 요청
	REQOP1 = 0 ;
	REQOP0 = 0 ;
#endif

}



void Can1BaudSetting(void)
{
    SEG2PHTS = 1 ;      // Freely programmable
   	SAM      = 1 ;      // Bus Line is sampled three times prior to the sample point

	
	//비트타임 = (동기 + 전파 + 위상1 + 위상2) X TQ 즉, 8TQ = 0.00002
	//.......따라서, 보우율 = 1/0.00002 = 50KHz	
	// 동기세그먼트	1Tq
    SJW1    = 0 ;       
    SJW0    = 0 ;	

	// 전파세그먼트	3Tq
   	PRSEG2	 = 0 ;      
   	PRSEG1	 = 1 ;
   	PRSEG0	 = 0 ;

	// 위상버퍼세그먼트1	2Tq
   	SEG1PH2	 = 0 ;      
   	SEG1PH1	 = 0 ;
   	SEG1PH0	 = 1 ;

	//위상버퍼세그먼트2	2Tq
   	SEG2PH2	 = 0 ;      
   	SEG2PH1	 = 0 ;
   	SEG2PH0	 = 1 ;

 	// 타임세그먼트	TQ=(2x40)/Fosc(32 Mhz)=0.0000025 
    BRP5    = 1 ; // Tq = 0.00000125/2
	BRP4    = 0 ; 
	BRP3    = 0 ; 
	BRP2    = 1 ;  
	BRP1    = 1 ;  
	BRP0    = 1 ; 
}   





// sid, eid의 비트에 대하여 마스터 된 비트에 대하여 필터 비트 셋팅  
void Can1SetFilter(char filter_no, unsigned int sid, unsigned long eid)
{      
    unsigned int uppereid;

    sid     =(sid << 5) & 0xffe0;
    eid     =(eid & 0x0003ffff);
    uppereid=(unsigned int)(eid >> 16);
    sid     =(sid | uppereid);
 
    uppereid =(unsigned int)(eid); 

    switch(filter_no)
    {
    case 0:
      RXF0SIDH  = (unsigned char)(sid >> 8); 
      RXF0SIDL  = (unsigned char)(sid);
      RXF0SIDL  = (RXF0SIDL | 0x08);
      RXF0EIDH  = (unsigned char)(uppereid >> 8);
      RXF0EIDL  = (unsigned char)(uppereid);
      break;
    case 1:
      RXF1SIDH  = (unsigned char)(sid >> 8); 
      RXF1SIDL  = (unsigned char)(sid);
      RXF1SIDL  = (RXF1SIDL | 0x08);
      RXF1EIDH  = (unsigned char)(uppereid >> 8);
      RXF1EIDL  = (unsigned char)(uppereid);
      break;
    case 2:
      RXF2SIDH  = (unsigned char)(sid >> 8); 
      RXF2SIDL  = (unsigned char)(sid);
      RXF2SIDL  = (RXF2SIDL | 0x08);
      RXF2EIDH  = (unsigned char)(uppereid >> 8);
      RXF2EIDL  = (unsigned char)(uppereid);
      break;
    case 3:
      RXF3SIDH  = (unsigned char)(sid >> 8); 
      RXF3SIDL  = (unsigned char)(sid);
      RXF3SIDL  = (RXF3SIDL | 0x08);
      RXF3EIDH  = (unsigned char)(uppereid >> 8);
      RXF3EIDL  = (unsigned char)(uppereid);
      break;
    case 4:
      RXF4SIDH  = (unsigned char)(sid >> 8); 
      RXF4SIDL  = (unsigned char)(sid);
      RXF4SIDL  = (RXF4SIDL | 0x08);
      RXF4EIDH  = (unsigned char)(uppereid >> 8);
      RXF4EIDL  = (unsigned char)(uppereid);
      break;
    case 5:
      RXF5SIDH  = (unsigned char)(sid >> 8); 
      RXF5SIDL  = (unsigned char)(sid);
      RXF5SIDL  = (RXF5SIDL | 0x08);
      RXF5EIDH  = (unsigned char)(uppereid >> 8);
      RXF5EIDL  = (unsigned char)(uppereid);
      break;
    default:
      RXF0SIDH  = (unsigned char)(sid >> 8); 
      RXF0SIDL  = (unsigned char)(sid);
      RXF0SIDL  = (RXF0SIDL | 0x08);
      RXF0EIDH  = (unsigned char)(uppereid >> 8);
      RXF0EIDL  = (unsigned char)(uppereid);
      break;
    }
}


// sid, eid 에 대한 각 비트별 체크 여부 셋팅 
void Can1SetMask(char mask_no, unsigned int sid, unsigned long eid)
{ 
    unsigned int uppereid;

    sid     =(sid << 5) & 0xffe0;
    eid     =(eid & 0x0003ffff);
    uppereid=(unsigned int)(eid >> 16);
    sid     =(sid | uppereid);
    uppereid=(unsigned int)(eid);

    switch(mask_no)
    {
    case 0:
      RXM0SIDH = (unsigned char)(sid >> 8);
      RXM0SIDL = (unsigned char)(sid);
      RXM0EIDH = (unsigned char)(uppereid >> 8);
      RXM0EIDL = (unsigned char)(uppereid);
      break;
    case 1:
      RXM1SIDH = (unsigned char)(sid >> 8);
      RXM1SIDL = (unsigned char)(sid);
      RXM1EIDH = (unsigned char)(uppereid >> 8);
      RXM1EIDL = (unsigned char)(uppereid);
      break;
    default:
      RXM0SIDH = (unsigned char)(sid >> 8);
      RXM0SIDL = (unsigned char)(sid);
      RXM0EIDH = (unsigned char)(uppereid >> 8);
      RXM0EIDL = (unsigned char)(uppereid);
      break;
    }
}





void Can1TxSidEidSet(char MsgFlag,unsigned int sid, unsigned long eid)
{
    unsigned int uppereid;


    sid     =(sid << 5) & 0xffe0;
    eid     =(eid & 0x0003ffff);
    uppereid=(unsigned int)(eid >> 16);
    sid     =(sid | uppereid);
 
    uppereid =(unsigned int)(eid); 

    switch(MsgFlag)
    {
    case 0:
      TXB0SIDH  = (unsigned char)(sid >> 8); 
      TXB0SIDL  = (unsigned char)(sid);
      TXB0SIDL  = (TXB0SIDL | 0x08);
      TXB0EIDH  = (unsigned char)(uppereid >> 8);
      TXB0EIDL  = (unsigned char)(uppereid);
      break;
    case 1:
      TXB1SIDH  = (unsigned char)(sid >> 8); 
      TXB1SIDL  = (unsigned char)(sid);
      TXB1SIDL  = (TXB1SIDL | 0x08);
      TXB1EIDH  = (unsigned char)(uppereid >> 8);
      TXB1EIDL  = (unsigned char)(uppereid);
      break;
    case 2:
      TXB2SIDH  = (unsigned char)(sid >> 8); 
      TXB2SIDL  = (unsigned char)(sid);
      TXB2SIDL  = (TXB2SIDL | 0x08);
      TXB2EIDH  = (unsigned char)(uppereid >> 8);
      TXB2EIDL  = (unsigned char)(uppereid);
      break;
    default:
      TXB0SIDH  = (unsigned char)(sid >> 8); 
      TXB0SIDL  = (unsigned char)(sid);
      TXB0SIDL  = (TXB0SIDL | 0x08);
      TXB0EIDH  = (unsigned char)(uppereid >> 8);
      TXB0EIDL  = (unsigned char)(uppereid);
      break;
    }
}


void Can1DlcSet(char MsgFlag,unsigned char DataCnt)
{
    switch(MsgFlag)
    {
    case 0:      
      TXB0DLC  = DataCnt; 
      break;
    case 1:
      TXB1DLC  = DataCnt; 
      break;
    case 2:
      TXB2DLC  = DataCnt; 
      break;
    default:
      TXB0DLC  = DataCnt; 
      break;
    }
}




void    GetRxSidEidLocal(void)
{

    unsigned int  itmp1,itmp2;
    unsigned long ltmp1;
    unsigned char TmpRcvAdr;

    itmp1=(RXB0SIDH << 8);
    itmp2=RXB0SIDL;
    Can1RxSid=(unsigned int)(itmp1 | itmp2);
    Can1RxSid=(Can1RxSid >> 5);
 
   
    ltmp1=(unsigned long)(RXB0SIDL & 0x03);
    Can1RxEid=(ltmp1 << 16);

    ltmp1=(unsigned long)(RXB0EIDH);
    ltmp1=(ltmp1 << 8);
    Can1RxEid=(Can1RxEid | ltmp1);

    ltmp1=(unsigned long)(RXB0EIDL);
    Can1RxEid=(Can1RxEid | ltmp1);

	Can1RxDlc=(RXB0DLC & 0x0f);

	TmpRcvAdr = (unsigned char)((Can1RxEid & HOST_ADDRESS) >> 14);
    ReceiveAdr = (unsigned char)(TmpRcvAdr & 0x03);
    SrcAddress = (unsigned char)(TmpRcvAdr & 0x03);

}



void    Can1FilterSet(void)
{    
    Can1SetFilter(0,Can1RxSid,Can1RxEid);
    Can1SetFilter(1,Can1RxSid,Can1RxEid);
    Can1SetFilter(2,Can1RxSid,Can1RxEid);
    Can1SetFilter(3,Can1RxSid,Can1RxEid);
    Can1SetFilter(4,Can1RxSid,Can1RxEid);
    Can1SetFilter(5,Can1RxSid,Can1RxEid);
}


void    Can1MaskSet(void)       //new
{    
    Can1SetMask(0,Can1RxSid,Can1RxEid);
    Can1SetMask(1,Can1RxSid,Can1RxEid);
}




void    Can1Interrupt(void)
{
    unsigned int i,j,hostnm;
    unsigned char *ptr;

    if(IRXIF){
        IRXIF=0;
    }
    else if(WAKIF){
        WAKIF=0;
    }
    else if(ERRIF){
        ERRIF=0;
        if(RXB0OVFL){
            RXB0OVFL=0;
        }  
        if(RXB1OVFL){
            RXB1OVFL=0;
        }  
        if(TXBO){
            TXBO=0;
        }  
        if(TXBP){
            TXBP=0;    
        }  
        if(RXBP){
            RXBP=0;
        }  
        if(TXWARN){
            TXWARN=0;
        }  
        if(RXWARN){
            RXWARN=0;
        }  
        if(EWARN){
            EWARN=0;
        }  
        COMSTAT=0;
    }
    else if(RXB1IF){
        RXB1IF=0;
    }

    else if(RXB0IF){

		GetRxSidEidLocal();

       if (Can1RxEid & HOST_BOARD)                                 //host board
       {
            if ((Can1RxEid & HOST_BLOCK_END)) 
            {
                j = 4;
                j = (j * 8);

                hostnm = (ReceiveAdr * HOST_DATA_RECOD) + j;

					

               	ptr = (unsigned char*) & RXB0D0; 
				if (ReceiveAdr < MAX_ELEV)
				{
               		for (i = 0;i < 8;i++)    CANRxHOSTDataBuf[hostnm +i+RCV_DATA] = ptr[i];    // Buff배열에 있는 데이터를 TXB0D0~D7까지 차례로 저장
				}


				msgCANRx_EleData_Updated = TRUE;

				CanTxAct = TRUE;

		
            }
            else
            {
                j = (unsigned char)(Can1RxEid & HOST_BLOCK_NUM);
                j = (j * 8);
                hostnm = (ReceiveAdr * HOST_DATA_RECOD) + j;

                ptr = (unsigned char*) &RXB0D0 ;                        // TXB0D0레지스터 주소를 구하여 ptr변수에 지정
                if (ReceiveAdr < MAX_ELEV)
                {
					for (i = 0;i < 8;i++)    CANRxHOSTDataBuf[hostnm + i + RCV_DATA] = ptr[i];    // Buff배열에 있는 데이터를 TXB0D0~D7까지 차례로 저장
                }	

				CanTxAct = FALSE;
			}
        }
	   
        ptr = (unsigned char*)&RXB0D0 ;                          			
        for(i=0;i<Can1RxDlc;i++)	Can1ThisRxBuf[i]=ptr[i];

        RXB0IF = FALSE;
        RXB0FUL = FALSE; // 0이면, 수신버퍼가 새 메시지를 받기위해 오픈되어 있음을 뜻한다.
    }   
}





void Can1Init(void)
{
    unsigned int i;
     
    Can1PortSetting();   
    Can1BaudSetting();
    
    
	ENDRHI = 1 ; // CANTX pin will drive when recessive
    CANCAP = 0 ; // enable CAN Capture mode
    WAKFIL = 0 ; // CAN bus line filter is not used for wake-up


/***********************************************/ 	
	RXB0FUL     = 0 ;   // clear receive buffer0 full flag
	RXB0M1      = 1 ;   // Receive extended identifier valid messages
	RXB0M0      = 0 ;   // Receive extended identifier valid messages
	RXB0DBEN    = 0 ;   // no data write Receive buffer0 to receive buffer1 
	RXB0FILHIT0 = 0;    // acceptance filter0
	RXB0RTRRO   = 0 ;   // No remote transfer request(read only) 
    JTOFF       = 0 ;   // (read only)
/***********************************************/ 	


/***********************************************/ 	
	RXB1FUL     = 0 ;
	RXB1M1      = 1 ; // Receive extended identifier valid messages 
	RXB1M0      = 0 ; 
	RXB1FILHIT2 = 0 ; //filter2
	RXB1FILHIT1 = 1 ;
	RXB1FILHIT0 = 0 ;
	RXB1RTRRO   = 0 ; // No remote transfer request(read Only) 
/***********************************************/ 	

    RXF0EXIDEN=1;
    RXF1EXIDEN=1;
    RXF2EXIDEN=1;
    RXF3EXIDEN=1;
    RXF4EXIDEN=1;
    RXF5EXIDEN=1;


    Can1FilterSet();
    Can1MaskSet();

    TXB0RTR  =0;

    TXB0REQ = 0 ;   //
   	TXB1REQ = 0 ;
    TXB2REQ = 0 ;


    RXB1IP=0;

#if  defined(CPU65K80)   ||  defined(CPU45K80)
IPR5=0;
#else        
IPR3=0;
#endif


    IRXIE       =0;
    WAKIE       =0;
    ERRIE       =1;
    TXB2IE      =0;
    TXB1IE      =0;
    TXB0IE      =0;
    RXB1IE      =1;
    RXB0IE      =1;


	//요청(Request) CAN 동작 모드 비트
#ifdef		 U_COMPILER		1
	CANCONbits.REQOP = 0 ;// Entry Configration Mode
#else
	REQOP2 = 0 ;// Entry Configration Mode , Normal Operation 일반모드 요청
	REQOP1 = 0 ;
	REQOP0 = 0 ;
#endif


}//--------------------------------------------------------------------------------------------------




void  Can1TxDataLoad(unsigned char pt)
{		
	TXB0D0=Can1TxBuf[pt+0];           
	TXB0D1=Can1TxBuf[pt+1];
	TXB0D2=Can1TxBuf[pt+2];
	TXB0D3=Can1TxBuf[pt+3];
	TXB0D4=Can1TxBuf[pt+4];
	TXB0D5=Can1TxBuf[pt+5];
	TXB0D6=Can1TxBuf[pt+6];
	TXB0D7=Can1TxBuf[pt+7];
}





void    TxSidEidArrary(void)
{
    unsigned int  itmp1,itmp2;
    unsigned long ltmp1;

    itmp1=HOST_COMPANY;
    itmp2=(unsigned int)GROUP_NUMBER;
    itmp2=(itmp2 << 7);
    Can1TxSid=(unsigned int)(itmp1 | itmp2);

	ltmp1= 0;
    ltmp1= (ltmp1 | (unsigned long)SelHostAdr);
    ltmp1= (ltmp1 << 14);
    ltmp1= (ltmp1 | (unsigned long)CallMeAdr);
	
    Can1TxEid=ltmp1;
}

void    LoadCanTxBuf(unsigned char *pt)
{
	unsigned char	i;
	for(i=0;i<8;i++)	*(pt+i)=CanKeyValue[i];           
}


unsigned char	Can1TxData(void)
{

	unsigned char ret;
	ret=1;

    if(!TXB0REQ)
	{

		TxSidEidArrary();
		Can1DlcSet(0, 8);
		Can1TxSidEidSet(0,Can1TxSid,Can1TxEid);
		
		CanKeyValue[0] = CanCmd;
		
		LoadCanTxBuf((unsigned char *)&TXB0D0);


        IPR5=0;
        TXB0IP=1;

        TXB0REQ = 1 ; // 전송 요구 비트 셋

//		bCan1TxStart=0;                    
//		bCan1RxAll=0;                    

		ret=0;       

    }

	return(ret);
}


////////////////////////////////






