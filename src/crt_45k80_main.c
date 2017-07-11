




#include    <pic18.h>

#include        "you_can.h"
#include        "com.h"
#include        "crt_45k80.h"


unsigned    char  	MainTimer=0;
unsigned    char	msec100=0;


bit bVirt_RxGood = 0;
unsigned int Virt_RxGoodTimer = 0;




extern  void  Timer0Init(void);
extern  void  Initial(void);



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
		case	1:	// 대성 감시반용 
			Com1BaudRate=19200;
			TX_DATA_CNT = 40; 
			break;
		case	2:	// 정진 감시반용 
			Com1BaudRate=38400;
			TX_DATA_CNT = 88; 
			break;
		case	3:	// LCD JUTF용
			Com1BaudRate=115200;
			TX_DATA_CNT = 40; 
			break;
		default:
			Com1BaudRate=115200;
			break;		
	}			
}


void  Serial2Check(void)
{
	unsigned char i;

//    if(Com1RxStatus==RX_GOOD){ 
	if (bVirt_RxGood)
	{
		bVirt_RxGood = 0;
	
		for (i=0; i<(Virt_Com1RxBuffer[3]+4); i++)	Can1TxBuf[i] = Virt_Com1RxBuffer[i];
		Can1TxDataTxPointer=2;	
		Can1TxDataTotalCnt= (Virt_Com1RxBuffer[3]+4);
    }             

              
}





void LoadCom1buf_ImageCmd(const char *buf)
{
	char i, j;

	for (i=0; i<MAX_PACKET; i++)
	{
		com1_buf[i] = 0;
	}	

	j = 0;
	com1_buf[j++] = '$';
	com1_buf[j++] = 'i';
	com1_buf[j++] = ',';
	com1_buf[j++] = '2';
	com1_buf[j++] = ',';
	
	for (i=0; buf[i]!=0; i++)
	{
		com1_buf[i+j] = buf[i];
	}

	com1_buf[i+j] = '#';

	SetCom1TxEnable();
	LCD_DELAY();
}



	
void LoadCom1buf_Textout(const char *buf)
{
	char i, j;

	for (i=0; i<MAX_PACKET; i++)
	{
		com1_buf[i] = 0;
	}	

	j = 0;
	com1_buf[j++] = '$';
	com1_buf[j++] = 'f';
	com1_buf[j++] = ',';
	com1_buf[j++] = '0';
	com1_buf[j++] = ',';
	
	for (i=0; buf[i]!=0; i++)
	{
		com1_buf[i+j] = buf[i];
	}

	com1_buf[i+j] = '#';

	SetCom1TxEnable();
	LCD_DELAY();
}




void LoadCom1buf_StartLCDCmd(const char *buf)
{
	char i, j;

	for (i=0; i<MAX_PACKET; i++)
	{
		com1_buf[i] = 0;
	}	

	j = 0;	
	com1_buf[j++] = '$';
	
	for (i=0; buf[i]!=0; i++)
	{
		com1_buf[i+j] = buf[i];
	}

	com1_buf[i+j] = '#';

	SetCom1TxEnable();
	LCD_DELAY();
}



void LoadCom1buf_ImLoadCmd(const char *buf)
{
	char i, j;

	for (i=0; i<MAX_PACKET; i++)
	{
		com1_buf[i] = 0;
	}	

	j = 0;
	com1_buf[j++] = '$';
	com1_buf[j++] = 'i';
	com1_buf[j++] = ',';
	com1_buf[j++] = '1';
	com1_buf[j++] = ',';
	
	for (i=0; buf[i]!=0; i++)
	{
		com1_buf[i+j] = buf[i];
	}

	com1_buf[i+j] = '#';

	SetCom1TxEnable();
	LCD_DELAY();
}





void LoadCom1buf_ColorSetCmd(const char *buf)
{
	char i, j;

	for (i=0; i<MAX_PACKET; i++)
	{
		com1_buf[i] = 0;
	}	

	j = 0;
	com1_buf[j++] = '$';
	com1_buf[j++] = 'l';
	com1_buf[j++] = ',';
	com1_buf[j++] = '0';
	com1_buf[j++] = ',';
	
	for (i=0; buf[i]!=0; i++)
	{
		com1_buf[i+j] = buf[i];
	}

	com1_buf[i+j] = '#';

	SetCom1TxEnable();
	LCD_DELAY();	
}

void LoadCom1buf_RectCmd(const char *buf)
{
	char i, j;

	for (i=0; i<MAX_PACKET; i++)
	{
		com1_buf[i] = 0;
	}	

	j = 0;
	com1_buf[j++] = '$';
	com1_buf[j++] = 'l';
	com1_buf[j++] = ',';
	com1_buf[j++] = '2';
	com1_buf[j++] = ',';
	
	for (i=0; buf[i]!=0; i++)
	{
		com1_buf[i+j] = buf[i];
	}

	com1_buf[i+j] = '#';

	SetCom1TxEnable();
	LCD_DELAY();
}









bit TestLCD_ImgDraw()
{
	static char sel = 0;	
	
	if (LcdDelay > 200)
	{	
		LcdDelay = 0;		
			
		switch (sel)
		{						
		case 0:
			LoadCom1buf_ImageCmd("1,0,100");
			break;	
		case 1:
			LoadCom1buf_ImageCmd("2,0,100");
			break;	
		case 2:
			LoadCom1buf_ImageCmd("3,0,100");
			break;
		case 3:
			LoadCom1buf_ImageCmd("4,0,100");
			break;
		case 4:
			LoadCom1buf_ImageCmd("5,0,100");
			break;	
		case 5:
			LoadCom1buf_ImageCmd("6,0,100");
			break;	
		case 6:
			LoadCom1buf_ImageCmd("7,0,100");
			break;	
		case 7:
			LoadCom1buf_ImageCmd("8,0,100");
			break;		
		case 8:
			LoadCom1buf_ImageCmd("101,0,0");
			break;	
		case 9:
			LoadCom1buf_ImageCmd("102,0,0");
			break;	
		case 10:	
			LoadCom1buf_ImageCmd("103,0,0");			
			break;
		case 11:
			LoadCom1buf_ImageCmd("151,250,200");
			LoadCom1buf_ImageCmd("152,320,200");
			LoadCom1buf_ImageCmd("153,390,200");
			LoadCom1buf_ImageCmd("154,250,130");
			LoadCom1buf_ImageCmd("155,320,130");
			LoadCom1buf_ImageCmd("156,390,130");
			LoadCom1buf_ImageCmd("157,250,60");
			LoadCom1buf_ImageCmd("158,320,60");
			
			//Draw_ButtonOff_1F();	
			//Draw_ButtonOff_2F();	
			//Draw_ButtonOff_3F();
			break;						
		default:
			return TRUE;		
		}

		sel++;
	}

	return FALSE;
}







 
bit LoadImg_Flr(void)
{
	static bit bEnd;

	bEnd = FALSE;
	
	if (ELE_DSP1 == '0')
	{
		switch (ELE_DSP2)
		{	
		case '1': // 1층 		
			LoadCom1buf_ImageCmd("1,0,100"); 
			break;			
		case '2':  
			LoadCom1buf_ImageCmd("2,0,100");  
			break;			
		case '3': 
			LoadCom1buf_ImageCmd("3,0,100");  
			break;	
		case '4': 
			LoadCom1buf_ImageCmd("4,0,100");  
			break;
		case '5': 
			LoadCom1buf_ImageCmd("5,0,100");  
			break;
		case '6': 
			LoadCom1buf_ImageCmd("6,0,100");  
			break;
		case '7':  
			LoadCom1buf_ImageCmd("7,0,100");  
			break;
		case '8': 
			LoadCom1buf_ImageCmd("8,0,100");  
			break;	
		default:			
			break;
		}		
	}

	
	
	bEnd = TRUE;
	return bEnd;
}

bit LoadImg_Arrow(void)
{
	static bit bEnd;

	bEnd = FALSE;
	
	if (ELE_bUP) // up
	{
		LoadCom1buf_ImageCmd("101,0,0"); // Up 이미지 표시 
	}	
	else if (ELE_bDOWN) // down
	{
		LoadCom1buf_ImageCmd("102,0,0"); // Dn 이미지 표시 
	}
	else
	{
		LoadCom1buf_ImageCmd("103,0,0"); // 정지 이미지 표시 
	}	


	bEnd = TRUE;
	return bEnd;	
}





bit Draw_ButtonOff_1F(void)
{
	LoadCom1buf_ColorSetCmd("255,255,255");
	LoadCom1buf_RectCmd("250,200,40,40,1"); 
	LoadCom1buf_ColorSetCmd("128,128,128"); // 색갈 희색 설정 
	LoadCom1buf_RectCmd("250,200,40,40,0"); // 사각형 버튼 그리기 

	return TRUE;
}

bit Draw_ButtonOff_2F(void)
{
	LoadCom1buf_ColorSetCmd("255,255,255");
	LoadCom1buf_RectCmd("300,200,40,40,1"); 
	LoadCom1buf_ColorSetCmd("128,128,128"); // 색갈 희색 설정 
	LoadCom1buf_RectCmd("300,200,40,40,0"); // 사각형 버튼 그리기 

	return TRUE;
}

bit Draw_ButtonOff_3F(void)
{
	LoadCom1buf_ColorSetCmd("255,255,255");
	LoadCom1buf_RectCmd("350,200,40,40,1"); 
	LoadCom1buf_ColorSetCmd("128,128,128"); // 색갈 희색 설정 
	LoadCom1buf_RectCmd("350,200,40,40,0"); // 사각형 버튼 그리기 

	return TRUE;
}

bit Draw_ButtonOn_1F(void)
{
	LoadCom1buf_ColorSetCmd("128,128,128"); // 색갈 희색 설정 
	LoadCom1buf_RectCmd("250,200,40,40,1"); // 사각형 버튼 그리기 

	return TRUE;
}

bit Draw_ButtonOn_2F(void)
{
	LoadCom1buf_ColorSetCmd("128,128,128"); // 색갈 희색 설정 
	LoadCom1buf_RectCmd("300,200,40,40,1"); // 사각형 버튼 그리기 

	return TRUE;
}


bit Draw_ButtonOn_3F(void)
{
	LoadCom1buf_ColorSetCmd("128,128,128"); // 색갈 희색 설정 
	LoadCom1buf_RectCmd("350,200,40,40,1"); // 사각형 버튼 그리기 

	return TRUE;
}


void LCD_DELAY(void)
{
	unsigned int i;
	unsigned int val;
	
	val = 2000;
	for(i=0; i<val; i++);
}

bit IsLoadedCom1Buf_EleData()
{
	LoadImg_Flr();	
	LoadImg_Arrow();	

	return TRUE;
}







bit check_collision(int startx, int starty, int width, int heigh, int x, int y)
{
	if ((startx < x) && (startx + width> x))
	{
		if ((starty < y) && (starty + heigh> y))
		{
			return TRUE;
		}
	}
	return FALSE;
}


void GetXY_Atoi(int *x, int *y)
{
	char i,k,m,ind;	
	
	*x = 0;
	k = 0;
	ind = 5;
	for (i=0; Com1RxBuffer[ind+i]!=','; i++)
		k++;	

	
	if (k == 1)
	{
		*x = (Com1RxBuffer[ind] - '0'); 
	}
	else if (k == 2)
	{
		*x = (Com1RxBuffer[ind] - '0') * 10; 
		*x = *x + (Com1RxBuffer[ind+1] - '0');
	}
	else if (k == 3)
	{
		*x = (Com1RxBuffer[ind] - '0') * 100; 
		*x = *x + (Com1RxBuffer[ind+1] - '0') * 10;
		*x = *x + (Com1RxBuffer[ind+2] - '0');
	}	

	
	*y = 0;
	m = k;
	k = 0;
	ind = 5+m+1;
	for (i=0; Com1RxBuffer[ind+i]!='#'; i++)
		k++;	
	
	if (k == 1)
	{
		*y = (Com1RxBuffer[ind] - '0'); 
	}
	else if (k == 2)
	{
		*y = (Com1RxBuffer[ind] - '0') * 10; 
		*y = *y + (Com1RxBuffer[ind+1] - '0');
	}
	else if (k == 3)
	{
		*y = (Com1RxBuffer[ind] - '0') * 100; 
		*y = *y + (Com1RxBuffer[ind+1] - '0') * 10;
		*y = *y + (Com1RxBuffer[ind+2] - '0');
	}	
	
}

void ChkArea(int x, int y)
{
	// 1층
	if (check_collision(250,200,60,60,x,y)) 
	{	
		newKeyScan[0] = 0x01;
	}
	// 2층 
	if (check_collision(320,200,60,60,x,y))
	{
		newKeyScan[0] = 0x02;
	}
	// 3층 
	if (check_collision(390,200,60,60,x,y))
	{
		newKeyScan[0] = 0x04;
	}
	// 4층 
	if (check_collision(250,130,60,60,x,y))
	{
		newKeyScan[0] = 0x08;
	}
	// 5층 
	if (check_collision(320,130,60,60,x,y))
	{
		newKeyScan[0] = 0x10;
	}
	// 6층 
	if (check_collision(390,130,60,60,x,y))
	{
		newKeyScan[0] = 0x20;
	}
	// 7층 
	if (check_collision(250,60,60,60,x,y))
	{
		newKeyScan[0] = 0x40;
	}
	// 8층 
	if (check_collision(320,60,60,60,x,y))
	{
		newKeyScan[0] = 0x80;
	}
}

void ChkRxTouch(void)
{
	char c;
	int x, y;

	GetXY_Atoi(&x, &y);	
	
	c = Com1RxBuffer[3];

	if (c == '1')
	{
		if (!bTouchPress)
		{
			bTouchPress = TRUE;
			ChkArea(x, y);
		}

	}
	else
	{
		bTouchPress = FALSE;
		newKeyScan[0] = 0x0;		
	}
}


void  ChkKeyButtonPush(void)
{
    if ((newKeyScan[0] == oldKeyScan[0]))
    {
        if (KeyScantimer[0] > 60) // 60미리 세크 만큼 키를 스캔한다. 
        {
			KeyScantimer[0] = 0;
            SetKey(0, 1, newKeyScan); 			
        }
    }
    else
    {
        KeyScantimer[0] = 0x0;
        oldKeyScan[0] = newKeyScan[0];
    }
}


void  SetKey(unsigned char pt, unsigned char tfloor, unsigned char* newKeyScan)
{
    unsigned char i, selbit;
	static unsigned char  bKeySeted[4] = {0, 0, 0, 0};

    tfloor = (tfloor + MyBaseFlr);

    selbit = 0x01;
    for (i = 0; i<8 ;i++)
    {
        if (newKeyScan[pt] & selbit)
        {
            if (!(bKeySeted[pt] & selbit))
            {
                if (CarKey[pt] & selbit) // 이미 등록 되어 있다면?
                {
                    CarKey[pt] = CarKey[pt] & ~selbit;
					if (i == 0)						
						LoadCom1buf_ImageCmd("151,250,200"); // off 1층 버튼 				
					if (i == 1)
						LoadCom1buf_ImageCmd("152,320,200"); // off 2층 버튼 
					if (i == 2)						
						LoadCom1buf_ImageCmd("153,390,200"); // off 3층 버튼 				
					if (i == 3)
						LoadCom1buf_ImageCmd("154,250,130"); // off 4층 버튼
					if (i == 4)						
						LoadCom1buf_ImageCmd("155,320,130"); // off 5층 버튼 				
					if (i == 5)
						LoadCom1buf_ImageCmd("156,390,130"); // off 6층 버튼
					if (i == 6)						
						LoadCom1buf_ImageCmd("157,250,60"); // off 7층 버튼 				
					if (i == 7)
						LoadCom1buf_ImageCmd("158,320,60"); // off 8층 버튼	
                }
                else if (ELE_FLOOR != (tfloor + i)) // 같은 층이 아니면 키 등록 
                {
                    CarKey[pt] = CarKey[pt] | selbit;
					if (i == 0)
						LoadCom1buf_ImageCmd("201,250,200"); // on 1층 버튼
					if (i == 1)
						LoadCom1buf_ImageCmd("202,320,200"); // on 2층 버튼 
					if (i == 2)
						LoadCom1buf_ImageCmd("203,390,200"); // on 3층 버튼
					if (i == 3)
						LoadCom1buf_ImageCmd("204,250,130"); // on 4층 버튼 
					if (i == 4)
						LoadCom1buf_ImageCmd("205,320,130"); // on 5층 버튼
					if (i == 5)
						LoadCom1buf_ImageCmd("206,390,130"); // on 6층 버튼 
					if (i == 6)
						LoadCom1buf_ImageCmd("207,250,60"); // on 7층 버튼
					if (i == 7)
						LoadCom1buf_ImageCmd("208,320,60"); // on 8층 버튼 	
                }				

				bKeySeted[pt] = bKeySeted[pt] | selbit;
            }
        }
        else
        {
            bKeySeted[pt] = bKeySeted[pt] & ~selbit;
        }
		
        selbit = selbit << 1;
    }
}


void SetCom1TxEnable(void)
{
	ptStr = com1_buf; // Q1
	TXREG = *ptStr;
	ptStr++;
	TXIE = TRUE; 
}






unsigned char SetHighFloor(unsigned char carkey)
{

    unsigned char HighOnlyFlr, k;
	static bit l;

	l = FALSE;

    k = HighFlr;


        HighOnlyFlr = (HighFlr & ONLY_FLR);

        if (HighOnlyFlr == 0)
        {
			HighFlr = (carkey |  CAR_READY);
        }
        else if (HighOnlyFlr > carkey)
        {
			HighFlr = (carkey |  CAR_READY);
        }
        else if (HighOnlyFlr < carkey)
        {
            if ((HighFlr & UPDN_CAR_READY) == DN_READY)
            {
				HighFlr = (carkey |  CAR_READY);
            }
            else if (!(HighFlr & UPDN_CAR_READY))
			{
				HighFlr = (carkey |  CAR_READY);
            }
            else
            {
                l = TRUE;
            }
        }
        else if (HighOnlyFlr == carkey)
        {
			HighFlr = (HighFlr |  CAR_READY);
        }
 


    if (l == TRUE)                        
		return(FALSE);
	
    if (k != HighFlr)                
		return(TRUE);
    else                            
		return(FALSE);

}


unsigned char SetLowFloor(unsigned char i)
{

    unsigned char LowOnlyFlr, k;
	static bit l;
    unsigned char tmp_Max_Low = 0;

	l = FALSE;

    tmp_Max_Low = i;
    if (i == 0)	tmp_Max_Low = (i | UP_READY);


    k = LowFlr;


 
    LowOnlyFlr = (LowFlr & ONLY_FLR);
    if (LowOnlyFlr == 0)  LowFlr = (i |  CAR_READY);
    else if (LowOnlyFlr < i)  LowFlr = (i |  CAR_READY);
    else if (LowOnlyFlr > i)
    {
        if ((LowFlr & UPDN_CAR_READY) == UP_READY)   LowFlr = (i |  CAR_READY);
        else if (!(LowFlr & UPDN_CAR_READY))   LowFlr = (i |  CAR_READY);
        else
        {

            l = TRUE;
        }
    }
    else if (LowOnlyFlr == i) LowFlr = (LowFlr |  CAR_READY);
 


    if (l == TRUE)          return(FALSE);
    if (k != LowFlr)   return(TRUE);
    else              return(FALSE);

}


unsigned char  ChkCarKeyLoad(unsigned char id)
{

    unsigned char  i, j, k, Curflr;
	static bit bKeyLoaded;

    bKeyLoaded = FALSE;
    k = 0x01;
    j = 0;
    Curflr = (ELE_FLOOR - 1);

    for (i = (0 + MyBaseFlr); i < (32 + MyBaseFlr); i++)
    {
        if (CarKey[j] & k)
        {
            if (i > Curflr)
            {
                if (SetHighFloor(i))
                    bKeyLoaded = TRUE;
            }
            else if (i < Curflr)
            {
                if (SetLowFloor(i))
                    bKeyLoaded = TRUE;
            }
            else
            {
  
            }
        }

        k = (k << 1);
        if (k == 0)
        {
            k = 0x01;
            j++;
        }
    }

    return(bKeyLoaded);

}


int IsKeyLoaded(unsigned char id)
{

    static bit bkeyloaded;
    int j = 0;
    unsigned char hi, lo, eq, dr, flrlong;
    unsigned int IdPt;

    CanCmd = CAN_NO_KEY_SET;

    IdPt = (id * HOST_DATA_RECOD) + RCV_DATA;

    hi = CANRxHOSTDataBuf[IdPt+mHighFloor];
    lo = CANRxHOSTDataBuf[IdPt+mLowFloor];


    HighFlr = hi;
    LowFlr = lo;
    

    if (ChkCarKeyLoad(id))
    {
        if (hi != HighFlr)
        {
            CanCmd = CAN_KEY_SET;
            CanKeyValue[1] = (HighFlr & ONLY_FLR) | CAR_READY;
            
        }
		else if (lo != LowFlr)
        {
            CanCmd = CAN_KEY_SET;
            CanKeyValue[1] = (LowFlr & ONLY_FLR) | CAR_READY;
        }

		bkeyloaded = TRUE;
    }
	else
	{
		bkeyloaded = FALSE;
	}

    return(bkeyloaded);
}


void ButtonOff(void)
{
	unsigned char i, selbit, curFlr;

	curFlr = ELE_FLOOR - 1;
	selbit= 0x01;
	for (i = 0; i<8 ;i++)
    {
    	if ((CarKey[0] & selbit) && (curFlr == i)) // 이미 등록 되어 있다면?
    	{
			CarKey[0] = CarKey[0] & ~selbit;
			
			if (i == 0)
				LoadCom1buf_ImageCmd("151,250,200"); // off 1층 버튼 				
			if (i == 1)
				LoadCom1buf_ImageCmd("152,320,200"); // off 2층 버튼 
			if (i == 2)						
				LoadCom1buf_ImageCmd("153,390,200"); // off 3층 버튼 				
			if (i == 3)
				LoadCom1buf_ImageCmd("154,250,130"); // off 4층 버튼
			if (i == 4)						
				LoadCom1buf_ImageCmd("155,320,130"); // off 5층 버튼 				
			if (i == 5)
				LoadCom1buf_ImageCmd("156,390,130"); // off 6층 버튼
			if (i == 6)						
				LoadCom1buf_ImageCmd("157,250,60"); // off 7층 버튼 				
			if (i == 7)
				LoadCom1buf_ImageCmd("158,320,60"); // off 8층 버튼	
    	}
		selbit = selbit << 1;
	}
}

void main(void)
{
	unsigned char i;	
    
    di();
    Initial();
    PortInit();
    Timer0Init(); 
	UserBaudRate();
    init_comms();
	Can1RxSid=0;
	Can1RxEid=0;

    Can1Init();

    ei();


	MainTimer=0;
	msec100=0;
	Com1SerialTime=0;		

	TXIE=0;
	RCIE=1;

	TRISD5=0;
	TX_EN=0;

	Com1RxStatus=RTX_CHK; 


	next = 0;
	bEndTestLCDMode = FALSE;
    do
	{
		switch (next)
		{
		case 0:
			LoadCom1buf_StartLCDCmd("s,4");
			LoadCom1buf_ColorSetCmd("255,255,255");
			//LoadCom1buf_RectCmd("0,0,480,272,1"); // JUTF43
			LoadCom1buf_RectCmd("0,0,800,480,1"); // JUTF70
			LoadCom1buf_ImLoadCmd("1,Floor_1.bmp");
			LoadCom1buf_ImLoadCmd("2,Floor_2.bmp");
			LoadCom1buf_ImLoadCmd("3,Floor_3.bmp");
			LoadCom1buf_ImLoadCmd("4,Floor_4.bmp");
			LoadCom1buf_ImLoadCmd("5,Floor_5.bmp");
			LoadCom1buf_ImLoadCmd("6,Floor_6.bmp");
			LoadCom1buf_ImLoadCmd("7,Floor_7.bmp");
			LoadCom1buf_ImLoadCmd("8,Floor_8.bmp");			
			LoadCom1buf_ImLoadCmd("101,up.bmp");
			LoadCom1buf_ImLoadCmd("102,down.bmp");
			LoadCom1buf_ImLoadCmd("103,stop.bmp");
			
			LoadCom1buf_ImLoadCmd("151,touch_off_1.bmp");			
			LoadCom1buf_ImLoadCmd("152,touch_off_2.bmp");
			LoadCom1buf_ImLoadCmd("153,touch_off_3.bmp");			
			LoadCom1buf_ImLoadCmd("154,touch_off_4.bmp");
			LoadCom1buf_ImLoadCmd("155,touch_off_5.bmp");			
			LoadCom1buf_ImLoadCmd("156,touch_off_6.bmp");
			LoadCom1buf_ImLoadCmd("157,touch_off_7.bmp");			
			LoadCom1buf_ImLoadCmd("158,touch_off_8.bmp");
			
			LoadCom1buf_ImLoadCmd("201,touch_on_1.bmp");
			LoadCom1buf_ImLoadCmd("202,touch_on_2.bmp");
			LoadCom1buf_ImLoadCmd("203,touch_on_3.bmp");
			LoadCom1buf_ImLoadCmd("204,touch_on_4.bmp");
			LoadCom1buf_ImLoadCmd("205,touch_on_5.bmp");
			LoadCom1buf_ImLoadCmd("206,touch_on_6.bmp");
			LoadCom1buf_ImLoadCmd("207,touch_on_7.bmp");
			LoadCom1buf_ImLoadCmd("208,touch_on_8.bmp");

			next = 1;
			break;

		case 1:
			if (TestLCD_ImgDraw())
				bEndTestLCDMode = TRUE;
			
			break;
		} 
		
        CLRWDT();
    }while(bEndTestLCDMode == FALSE);

	TXIE = 0;
	strPt = 0;

	IdPt = (LocalNumber * HOST_DATA_RECOD) + RCV_DATA;

	bTouchPress = FALSE;

    while (1)
	{    
		
        CLRWDT();

		
	
		// LCD 다시 그리기 
		if (msgCANRx_EleData_Updated)
		{
			msgCANRx_EleData_Updated = FALSE;
			IsLoadedCom1Buf_EleData();		
			TXLED = !TXLED; 			
		}	


		// Com1 수신 
		if(Com1RxStatus == RX_GOOD)
		{
			Com1RxStatus = RTX_CHK;
			RXLED = !RXLED; 	
			
			if(Com1RxBuffer[1] == 't')
			{
				ChkRxTouch();
			}
		}

        if (CanTxAct)
        {
            if (IsKeyLoaded(SelHostAdr))
            {
                Can1TxData(); 
            }

            CanTxAct = 0;
        }
		
		IdPt = (SelHostAdr * HOST_DATA_RECOD) + RCV_DATA;
		ChkKeyButtonPush();		
		
		ButtonOff();		
		
	 
		if(Com1RxStatus == TX_SET){
			if(Com1SerialTime > 3){
				Com1SerialTime=0;
				Com1RxStatus=RTX_CHK; 
				Com1TxThisPt=0;
				Com1TxCnt=0;
				TXIE=0;
				TX_EN=0;
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
				RUNLED=!RUNLED;
			}				
        } 
		
		Virt_RxGoodTimer++;
		if (Virt_RxGoodTimer > 100)
		{
			Virt_RxGoodTimer = 0;
			bVirt_RxGood = TRUE;
		}

		LcdDelay++;

		KeyScantimer[0]++;
        KeyScantimer[1]++;
        KeyScantimer[2]++;
        KeyScantimer[3]++;
    }

    if(PIR5 > 0)
	{
        Can1Interrupt();
    }	

	if((TXIE)&&(TXIF))										/*transmit interrupt routine*/
	{
        TXIF=0;
        Interrupt_COM1Tx();
	}	

	if((RCIE)&&(RCIF))										/*receive interrupt routine*/
	{
        RCIF=0;
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
		CREN1=0;
    }

	if( !CREN1)	CREN1=1;
	
}




