#pragma once

typedef struct tagCOMMDATA {
	int     Port;
	int     BaudRate, Parity, ByteSize, StopBits;
	int		ibaudrate, iparity, ibytesize, istopbits;
	BOOL    Hw;		/* RTS/CTS hardware flow control */
	BOOL	Sw;		/* XON/XOFF software flow control */
	BOOL    Dtr, Rts;
	int		MAXCOM;
}COMMDATA, *LPCOMMDATA;

extern COMMDATA GCommData;

extern int GBaudTable[];
extern int GParityTable[];
extern int GStopBitsTable[];
extern int GDataBitsTable[];

extern CString GBaudTableStr[];
extern CString GParityTableStr[];
extern CString GStopBitsTableStr[];
extern CString GDataBitsTableStr[];

extern BOOL GbOpen;

extern TCHAR GszAppName[];

#define COMWIDTH 190
#define MYMAXCOM 4