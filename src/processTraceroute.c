/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processTraceroute.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:59 by sbaranes          #+#    #+#             */
/*   Updated: 2023/03/31 18:21:30 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

t_stats	g_stats;

// static void	save_stats(t_stats *stats)
// {
// 	float	old_time;

// 	old_time = 0;
// 	stats->time_recv = get_time();
// 	old_time = stats->time;
// 	stats->time = stats->time_recv - stats->time_start;
// 	if (stats->min > stats->time)
// 		stats->min = stats->time;
// 	if (stats->max < stats->time)
// 		stats->max = stats->time;
// 	stats->avg += stats->time;
// 	if (stats->seq > 1)
// 	{
// 		if (stats->time > old_time)
// 			stats->mdev += stats->time - old_time;
// 		else
// 			stats->mdev += old_time - stats->time;
// 	}
// }

static void	set_packet_header(t_args *args)
{
	args->pkt.hdr.type = ICMP_ECHO;
	args->pkt.hdr.code = 0;
	args->pkt.hdr.checksum = 0;
	args->pkt.hdr.un.echo.id = htons(getpid() & 0xFFFF);
	args->pkt.hdr.un.echo.sequence = htons(1);
	args->pkt.hdr.checksum = calc_checksum((unsigned short *)&args->pkt.hdr, sizeof(args->pkt.hdr));
}

static char *reverse_dns_lookup(char *ip_addr)
{
	struct sockaddr_in	temp_addr;
	socklen_t			len;
	char				buf[NI_MAXHOST];
	char				*ret_buf;

	ret_buf = NULL;
	temp_addr.sin_family = AF_INET;
	len = sizeof(struct sockaddr_in);
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	if (getnameinfo((struct sockaddr *)&temp_addr, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD))
		return NULL;
	ret_buf = (char *)malloc(NI_MAXHOST * sizeof(char));
	ft_strcpy(ret_buf, buf);
	return ret_buf;
}

static bool	check_recv(t_args *args, struct sockaddr_in *addr_con)
{
	struct sockaddr_in	recv_addr;
	unsigned int		addr_len;
	char				recv_buf[1024];
	char				*rev_host;

	addr_len = sizeof(recv_addr);
	if (recvfrom(args->sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &recv_addr, &addr_len) <= 0)
		printf("%d\t*\n", args->ttl);
	else
	{
		rev_host = reverse_dns_lookup(inet_ntoa(recv_addr.sin_addr));
		if (rev_host == NULL)
			printf("%d %s (%s)\n", args->ttl, inet_ntoa(recv_addr.sin_addr), inet_ntoa(recv_addr.sin_addr));
		else
		{
			printf("%d %s (%s)\n", args->ttl, rev_host, inet_ntoa(recv_addr.sin_addr));
			free(rev_host);
		}
		if (recv_addr.sin_addr.s_addr == addr_con->sin_addr.s_addr)
			return true;
	}
	return false;
}

void	process_traceroute(t_args *args, struct sockaddr_in *addr_con)
{
	while (args->ttl <= MAX_TTL)
	{
		if (setsockopt(args->sockfd, IPPROTO_IP, IP_TTL, &args->ttl, sizeof(args->ttl)) != 0) {
			printf("Error setting TTL value!\n");
			return;
		}
		set_packet_header(args);
		if (sendto(args->sockfd, &args->pkt.hdr, sizeof(args->pkt.hdr), 0, (struct sockaddr *) addr_con, sizeof(*addr_con)) < 0) {
			printf("Error sending ICMP packet!\n");
			return;
		}
		setsockopt(args->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&args->timeout, sizeof(args->timeout));
		if (check_recv(args, addr_con))
			break ;
		args->ttl++;
	}
}
