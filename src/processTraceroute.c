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

static char *reverse_dns_lookup(char *ip_addr)
{
	t_addr_in	temp_addr;
	socklen_t	len;
	char		buf[NI_MAXHOST];
	char		*ret_buf;

	ret_buf = NULL;
	temp_addr.sin_family = AF_INET;
	len = sizeof(t_addr_in);
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	if (getnameinfo((struct sockaddr *)&temp_addr, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD))
		return NULL;
	ret_buf = (char *)malloc(NI_MAXHOST * sizeof(char));
	ft_strcpy(ret_buf, buf);
	return ret_buf;
}

bool	same_recv(t_addr_in *addr_prev, t_addr_in *recv_addr)
{
	if (addr_prev->sin_addr.s_addr == recv_addr->sin_addr.s_addr)
	{
		printf("coucou diff\n");
		return true;
	}
	return false;
}

void	print_time_recv(t_args *args)
{
	if (args->t1 != -1)
		printf(" %.3f ms ", args->t1);
	if (args->t2 != -1)
		printf(" %.3f ms ", args->t2);
	if (args->t3 != -1)
		printf(" %.3f ms ", args->t3);
	args->t1 = -1;
	args->t2 = -1;
	args->t3 = -1;
}

void	set_time(t_args *args, int loop, double time)
{
	if (loop == 1)
		args->t1 = time;
	else if (loop == 2)
		args->t2 = time;
	else if (loop == 3)
		args->t3 = time;
}

static bool	check_recv(t_args *args, t_addr_in *addr_con, t_addr_in	*addr_prev)
{
	t_addr_in		recv_addr;
	unsigned int	addr_len;
	int				recv;
	char			recv_buf[1024];

	addr_len = sizeof(recv_addr);
	recv = recvfrom(args->sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &recv_addr, &addr_len);
	if (inet_ntoa(recv_addr.sin_addr) != inet_ntoa(addr_prev->sin_addr))
		args->recv_host = false;
	if (recv <= 0)
	{
		printf(" *");
		args->revc_error = true;
	}
	else if (args->recv_host == false)
	{
		addr_prev->sin_addr.s_addr = recv_addr.sin_addr.s_addr;
		args->hostname = reverse_dns_lookup(inet_ntoa(recv_addr.sin_addr));
		print_recv_packet(args, recv_addr);
		if (recv_addr.sin_addr.s_addr == addr_con->sin_addr.s_addr)
			args->recv_host = 2;
		else
			args->recv_host = 1;
		print_time_recv(args);
	}
	return false;
}

static int	process_packet(t_args *args, t_addr_in *addr_con, t_addr_in	*addr_prev, int loop)
{
	struct timeval	timeout;
	double			tstart;

	timeout.tv_usec = 0;
	timeout.tv_sec = RECV_TIMEOUT;
	if (setsockopt(args->sockfd, IPPROTO_IP, IP_TTL, &args->ttl, sizeof(args->ttl)) != 0)
		return (print_error("Error setting TTL value!\n"));
	set_packet_header(args);
	setsockopt(args->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
	if (sendto(args->sockfd, &args->pkt.hdr, sizeof(args->pkt.hdr), 0, (struct sockaddr *) addr_con, sizeof(*addr_con)) < 0)
		return (print_error("Error sending ICMP packet!\n"));
	tstart = get_time();
	check_recv(args, addr_con, addr_prev);
	set_time(args, loop, ((get_time() - tstart) / 1000.0));
	return (0);
}

void	process_traceroute(t_args *args, t_addr_in *addr_con)
{
	int			loop;
	t_addr_in	addr_prev;

	while (args->ttl <= MAX_TTL)
	{
		loop = 0;
		printf("%d  ", args->ttl);
		args->recv_host = false;
		args->t1 = -1;
		args->t2 = -1;
		args->t3 = -1;
		while (loop != 3)
		{
			args->revc_error = false;
			loop++;
			if (process_packet(args, addr_con, &addr_prev, loop))
				return ;
		}
		print_time_recv(args);
		printf("\n");
		if (args->recv_host == 2)
			break ;
		args->ttl++;
	}
}
