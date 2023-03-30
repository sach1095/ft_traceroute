#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <netinet/ip.h>

#define MAX_TTL 30
#define RECV_TIMEOUT 1

char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con);
char *reverse_dns_lookup(char *ip_addr);

unsigned short	checksum(void *packet, int len)
{
	unsigned short	*buf;
	unsigned int	sum;
	unsigned short	result;

	buf = packet;
	sum = 0;
	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return (result);
}

char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con) {
    struct hostent *host_entity;
    char *ip = (char *)malloc(NI_MAXHOST * sizeof(char));

    if ((host_entity = gethostbyname(addr_host)) == NULL) {
        return NULL;
    }

    strcpy(ip, inet_ntoa(*(struct in_addr *)host_entity->h_addr));

    addr_con->sin_family = host_entity->h_addrtype;
    addr_con->sin_port = htons(0);
    addr_con->sin_addr.s_addr = *(long *)host_entity->h_addr;

    return ip;
}

char *reverse_dns_lookup(char *ip_addr) {
    struct sockaddr_in temp_addr;
    socklen_t len;
    char buf[NI_MAXHOST], *ret_buf;

    temp_addr.sin_family = AF_INET;
    temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
    len = sizeof(struct sockaddr_in);

    if (getnameinfo((struct sockaddr *)&temp_addr, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD)) {
        return NULL;
    }

    ret_buf = (char *)malloc((strlen(buf) + 1) * sizeof(char));
    strcpy(ret_buf, buf);
    return ret_buf;
}

void traceroute(int sockfd, struct sockaddr_in *addr_con, char *dst_ip, char *dst_hostname) {
    int ttl, recv_len, addr_len;
    struct sockaddr_in recv_addr;
    struct icmphdr icmp_hdr;
    struct iphdr ip_hdr;
    char recv_buf[1024];
    char *rev_host;

    printf("Traceroute to %s (%s), %d hops max\n\n", dst_hostname, dst_ip, MAX_TTL);

    for (ttl = 1; ttl <= MAX_TTL; ttl++) {
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
            printf("Error setting TTL value!\n");
            return;
        }

        icmp_hdr.type = ICMP_ECHO;
        icmp_hdr.code = 0;
        icmp_hdr.checksum = 0;
        icmp_hdr.un.echo.id = htons(getpid() & 0xFFFF);
        icmp_hdr.un.echo.sequence = htons(1);
        icmp_hdr.checksum = checksum((unsigned short *)&icmp_hdr, sizeof(icmp_hdr));

        if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *) addr_con, sizeof(*addr_con)) < 0) {
            printf("Error sending ICMP packet!\n");
            return;
        }

        struct timeval timeout;
        timeout.tv_sec = RECV_TIMEOUT;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

        addr_len = sizeof(recv_addr);
        recv_len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &recv_addr, &addr_len);

        if (recv_len <= 0) {
            printf("%d\t*\n", ttl);
        } else {
            rev_host = reverse_dns_lookup(inet_ntoa(recv_addr.sin_addr));
            if (rev_host == NULL) {
                printf("%d %s (%s)\n", ttl, inet_ntoa(recv_addr.sin_addr), inet_ntoa(recv_addr.sin_addr));
            } else {
                printf("%d %s (%s)\n", ttl, rev_host, inet_ntoa(recv_addr.sin_addr));
                free(rev_host);
            }

            if (recv_addr.sin_addr.s_addr == addr_con->sin_addr.s_addr) {
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int sockfd;
    char *dst_ip;
    struct sockaddr_in addr_con;

    if (argc != 2) {
        printf("Usage: %s <address>\n", argv[0]);
        return 0;
    }

    dst_ip = dns_lookup(argv[1], &addr_con);
    if (dst_ip == NULL) {
        printf("DNS lookup failed! Could not resolve hostname!\n");
        return 0;
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        printf("Error creating socket!\n");
        return 0;
    }

    traceroute(sockfd, &addr_con, dst_ip, argv[1]);
    close(sockfd);

    return 0;
}
