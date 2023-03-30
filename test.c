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

        if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *) addr_con, sizeof(*addr_con)) < 0) {
            printf("Error sending ICMP packet!\n");
            return;
        }

        addr_len = sizeof(recv_addr);
        recv_len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &recv_addr, &addr_len);

        if (recv_len <= 0) {
            printf("%d * * *\n", ttl);
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
