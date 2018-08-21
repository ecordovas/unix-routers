#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int s;
	
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) > 0) {
		char buf[] = "M-SEARCH * HTTP/1.1\r\n"\
					"HOST: 239.255.255.250:1900\r\n"\
					"MAN: \"ssdp:discover\"\r\n"\
					"ST: urn:dial-multiscreen-org:service:dial:1\r\n"\
					"MX: 1\r\n"\
					"\r\n";
		struct sockaddr_in srv;
		
		bzero(&srv, sizeof(srv));
		srv.sin_family = AF_INET;
		srv.sin_port = htons(1900);
		srv.sin_addr.s_addr = inet_addr("239.255.255.250");
		
		if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &srv, sizeof(srv)) > 0) {
			fd_set readset;
			struct timeval tv;
			
			printf("%s\n", buf);
			
			tv.tv_sec = 3;
			tv.tv_usec = 0;
			FD_ZERO(&readset);
			FD_SET(s, &readset);
			
			while (select(s + 1, &readset, NULL, NULL, &tv) > 0) {
				char res[512+1];
				struct sockaddr_in from;
				socklen_t fromlen = sizeof(struct sockaddr_in);
				int recvLen = -1;
				
				if ((recvLen = recvfrom(s, res, sizeof(res)-1, 0, (struct sockaddr *) &from, &fromlen)) > 0) {
					res[recvLen] = '\0';
					printf("got response from (%s)\n", inet_ntoa(from.sin_addr));
					printf("%s", res);
				}
			}			
		}
		
		close(s);
	}
	
	return 0;
}
