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

// void traceroute(int sockfd, struct sockaddr_in *addr_con, char *dst_ip, char *dst_hostname) {
//     int ttl, recv_len, addr_len;
//     struct sockaddr_in recv_addr;
//     struct icmphdr icmp_hdr;
//     struct iphdr ip_hdr;
//     char recv_buf[1024];
//     char *rev_host;

//     printf("Traceroute to %s (%s), %d hops max\n\n", dst_hostname, dst_ip, MAX_TTL);

//     for (ttl = 1; ttl <= MAX_TTL; ttl++) {
//         if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
//             printf("Error setting TTL value!\n");
//             return;
//         }

//         if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *) addr_con, sizeof(*addr_con)) < 0) {
//             printf("Error sending ICMP packet!\n");
//             return;
//         }

//         addr_len = sizeof(recv_addr);
//         recv_len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &recv_addr, &addr_len);

//         if (recv_len <= 0) {
//             printf("%d * * *\n", ttl);
//         } else {
//             rev_host = reverse_dns_lookup(inet_ntoa(recv_addr.sin_addr));
//             if (rev_host == NULL) {
//                 printf("%d %s (%s)\n", ttl, inet_ntoa(recv_addr.sin_addr), inet_ntoa(recv_addr.sin_addr));
//             } else {
//                 printf("%d %s (%s)\n", ttl, rev_host, inet_ntoa(recv_addr.sin_addr));
//                 free(rev_host);
//             }

//             if (recv_addr.sin_addr.s_addr == addr_con->sin_addr.s_addr) {
//                 break;
//             }
//         }
//     }
// }

void send_traceroute(int sockfd, struct sockaddr_in *dest_addr, char *dest_ip)
{
    int ttl = 1;
    int finished = 0;

    while (ttl <= MAX_HOPS && !finished)
    {
        // Set the TTL on the socket
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

        // Send the ICMP packet and receive the response
        int received = 0;
        struct sockaddr_in response_addr;
        socklen_t response_addr_len = sizeof(response_addr);
        struct icmp_packet packet;
        create_icmp_packet(&packet);

        struct timeval timeout;
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));

        if (recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&response_addr, &response_addr_len) > 0)
        {
            received = 1;
        }

        if (received)
        {
            char addr_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(response_addr.sin_addr), addr_str, INET_ADDRSTRLEN);

            printf("%d\t%s (%s)\n", ttl, addr_str, addr_str);

            if (strcmp(addr_str, dest_ip) == 0)
            {
                finished = 1;
            }
        }
        else
        {
            printf("%d\t*\n", ttl);
        }

        ttl++;
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

    send_traceroute(sockfd, &addr_con, dst_ip, argv[1]);
    close(sockfd);

    return 0;
}
