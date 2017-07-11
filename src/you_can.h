
#include "memory_map.h"

#ifndef 	_YOU_CAN_H_
#define 	_YOU_CAN_H_



#define     HOST_BOARD          0x2000
#define     HOST_ADDRESS        0x0000c000

#define     HOST_DATA_RECOD     42
#define     HOST_BLOCK_END      0x08
#define     HOST_BLOCK_NUM    	0x07

#define     MAX_ELEV            4
#define     RCV_DATA            0
#define     MAX_SAVE_BUF        HOST_DATA_RECOD * MAX_ELEV




#define  	H_INTV_TIME1  50   //msec
#define  	H_INTV_TIME2  55   //msec
#define  	H_INTV_TIME3  60   //msec
#define  	H_INTV_TIME4  65   //msec
#define  	H_INTV_TIME5  70   //msec



#define		CAN1_MAX_TX_BUF		100
#define		CAN1_MAX_RX_BUF		100










unsigned    char    ReceiveAdr;
unsigned    char    SrcAddress;
unsigned    char    CANRxHOSTDataBuf[MAX_SAVE_BUF]; // HOST Data 저장 공간 , 로컬 0~3(총4대)까지 저장 가능하다. 
unsigned    char    CanTxAct;
unsigned    char    CanLiveTimeAdr[MAX_ELEV];





extern  void    Can1DlcSet(char MsgFlag,unsigned char DataCnt);
extern  void    Can1Init(void);
extern  void    Can1Interrupt(void);
extern  void    Can1SetFilter(char filter_no, unsigned int sid, unsigned long eid);
extern  void    Can1SetMask(char mask_no, unsigned int sid, unsigned long eid);
extern	unsigned char	Can1Check(void);




extern	unsigned    long    Can1TxEid;
extern	unsigned    int     Can1TxSid;
extern	unsigned    char    Can1TxDlc;
extern	unsigned    char	Can1TxDataTotalCnt;
extern	unsigned    char    Can1TxDataTxPointer;


extern	unsigned    long    Can1RxEid;
extern	unsigned    int     Can1RxSid;
extern	unsigned    char    Can1RxDlc;
extern	unsigned    char	Can1RxDataTotalCnt;
extern	unsigned    char    Can1RxDataRxPointer;

extern	unsigned    char    Can1TxBuf[CAN1_MAX_TX_BUF];           
extern	unsigned    char    Can1RxBuf[CAN1_MAX_RX_BUF];           

extern	unsigned    char    Can1ThisRxBuf[8];           
extern	unsigned    int     Can1RxTimer;   


volatile unsigned int    IdPt = 0;

#define ELE_FLOOR         CANRxHOSTDataBuf[IdPt + S0_FLOOR]


#define ELE_DSP1         CANRxHOSTDataBuf[IdPt + DSP1]
#define ELE_DSP2         CANRxHOSTDataBuf[IdPt + DSP2]

#define ELE_bUP             (((CANRxHOSTDataBuf[IdPt + S1_STATE] & S1_UP) != 0x00)?		1:0)
#define ELE_bDOWN           (((CANRxHOSTDataBuf[IdPt + S2_STATE] & S2_DN) != 0x00)?		1:0)

#define     GROUP_NUMBER        0


unsigned    char    SelHostAdr = 0;
unsigned    char  CallMeAdr = 0;



unsigned    char    CanCmd;
unsigned    char  DoorKey=0;


extern unsigned char	Can1TxData(void);





#endif

