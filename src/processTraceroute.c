/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processTraceroute.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:59 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/09 18:24:31 by sbaranes         ###   ########.fr       */
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
	char			recv_buf[1024];

	addr_len = sizeof(recv_addr);
	if (recvfrom(args->sockfd, recv_buf, sizeof(recv_buf), 0,
			(struct sockaddr *) &recv_addr, &addr_len) <= 0)
	{
		printf(" *");
		args->revc_error = true;
	}
	else if (check_if_print(args, addr_prev, recv_addr))
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
	return (false);
}

static int	process_packet(t_args *args, t_addr_in *addr_con,
		t_addr_in	*addr_prev, int loop)
{
	struct timeval	timeout;
	double			tstart;

	timeout.tv_usec = 0;
	timeout.tv_sec = RECV_TIMEOUT;
	set_packet_header(args);
	if (setsockopt(args->sockfd, IPPROTO_IP, IP_TTL,
			&args->ttl, sizeof(args->ttl)) != 0)
		return (print_error("Error setting TTL value!\n"));
	setsockopt(args->sockfd, SOL_SOCKET, SO_RCVTIMEO,
		(const char *)&timeout, sizeof(timeout));
	tstart = get_time();
	if (sendto(args->sockfd, &args->pkt.hdr, sizeof(args->pkt.hdr),
			0, (struct sockaddr *) addr_con, sizeof(*addr_con)) < 0)
		return (print_error("Error sending ICMP packet!\n"));
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
		args->recv_host = 0;
		args->t1 = -1;
		args->t2 = -1;
		args->t3 = -1;
		while (loop != 3)
		{
			args->revc_error = false;
			loop++;
			if (process_packet(args, addr_con, &addr_prev, loop))
				return ;
			print_time_recv(args);
		}
		printf("\n");
		if (args->recv_host == 2)
			break ;
		args->ttl++;
	}
}
