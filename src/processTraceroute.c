/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processTraceroute.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:59 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/07 15:57:10 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

static char	*reverse_dns_lookup(char *ip_addr)
{
	t_addr_in	temp_addr;
	socklen_t	len;
	char		buf[NI_MAXHOST];
	char		*ret_buf;

	ret_buf = NULL;
	temp_addr.sin_family = AF_INET;
	len = sizeof(t_addr_in);
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	if (getnameinfo((struct sockaddr *)&temp_addr, len, buf,
			sizeof(buf), NULL, 0, NI_NAMEREQD))
		return (NULL);
	ret_buf = (char *)malloc(NI_MAXHOST * sizeof(char));
	ft_strcpy(ret_buf, buf);
	return (ret_buf);
}

static void	check_recv(t_args *args, t_addr_in *addr_con, int loop)
{
	t_addr_in		recv_addr;
	unsigned int	addr_len;
	char			recv_buf[1024];
	int				recv;

	addr_len = sizeof(recv_addr);
	recv = recvfrom(args->sockfd, recv_buf, sizeof(recv_buf), 0,
			(struct sockaddr *) &recv_addr, &addr_len);
	if (recv_addr.sin_addr.s_addr == addr_con->sin_addr.s_addr)
		args->final = true;
	if (recv <= 0 || args->final)
	{
		if (loop == 1)
			printf("%d ", args->ttl);
		printf(" *");
		args->revc_error = true;
	}
	else
	{
		if (loop == 1)
		{
			args->hostname = reverse_dns_lookup(inet_ntoa(recv_addr.sin_addr));
			print_recv_packet(args, recv_addr, loop);
			if (args->final && args->ttl == 1)
				exit(0);
		}
	}
}

static int	process_packet(t_args *args, t_addr_in *addr_con, int loop)
{
	struct timeval	timeout;
	double	tstart;

	timeout.tv_sec = RECV_TIMEOUT;
	timeout.tv_usec = 0;
	if (setsockopt(args->sockfd, IPPROTO_IP, IP_TTL,
			&args->ttl, sizeof(args->ttl)) != 0)
		return (print_error("Error setting TTL value!\n"));
	set_packet_header(args);
	if (sendto(args->sockfd, &args->pkt.hdr, sizeof(args->pkt.hdr), 0,
			(struct sockaddr *) addr_con, sizeof(*addr_con)) < 0)
		return (printf("connect: %s\n", strerror(errno)));
	tstart = get_time();
	setsockopt(args->sockfd, SOL_SOCKET, SO_RCVTIMEO,
		(const char *)&timeout, sizeof(timeout));
	check_recv(args, addr_con, loop);
	if (args->revc_error == false)
		printf("  %.3f ms", ((get_time() - tstart) / 1000.0));
	return (false);
}

void	process_traceroute(t_args *args, t_addr_in *addr_con)
{
	int		loop;
	int		process_send;

	process_send = true;
	while (args->ttl <= MAX_TTL && process_send)
	{
		loop = 0;
		while (loop != 3 && process_send)
		{
			args->revc_error = false;
			loop++;
			if (process_packet(args, addr_con, loop))
				return ;
		}
		printf("\n");
		args->ttl++;
	}
}
