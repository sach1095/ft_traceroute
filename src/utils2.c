/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/09 18:01:25 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/09 18:24:39 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

void	init_args(t_args *args)
{
	args->ttl = 1;
	args->ip = NULL;

}

void	set_packet_header(t_args *args)
{
	int	i;

	i = 0;
	if (args->icmp)
	{
	args->pkt.hdr.type = ICMP_ECHO;
	args->pkt.hdr.code = 0;
	args->pkt.hdr.checksum = 0;
	args->pkt.hdr.un.echo.id = htons(getpid() & 0xFFFF);
	args->pkt.hdr.un.echo.sequence = htons(1);
	args->pkt.hdr.checksum = calc_checksum((unsigned short *)&args->pkt.hdr,
			sizeof(args->pkt.hdr));
	}
	else
	{
		while (i < USHRT_MAX)
			args->pkt.buffer[i++] = 0;
	}
}

int	print_error(char *strError)
{
	return (write(2, strError, ft_strlen(strError)));
}

bool	check_if_print(t_args *args, t_addr_in *addr_prev, t_addr_in recv_addr)
{
	if (args->recv_host == 1 || args->recv_host == 2)
	{
		if (inet_ntoa(recv_addr.sin_addr)
			!= inet_ntoa(addr_prev->sin_addr))
		{
			args->recv_host = 0;
			return (true);
		}
		return (false);
	}
	return (true);
}

void	print_time_recv(t_args *args)
{
	if (args->revc_error == false)
	{
		if (args->t1 != -1)
			printf(" %.3f ms ", args->t1);
		if (args->t2 != -1)
			printf(" %.3f ms ", args->t2);
		if (args->t3 != -1)
			printf(" %.3f ms ", args->t3);
	}
	args->t1 = -1;
	args->t2 = -1;
	args->t3 = -1;
}
