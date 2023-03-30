/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sendPing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:59 by sbaranes          #+#    #+#             */
/*   Updated: 2023/01/16 15:39:27 by sbaranes         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

t_stats	g_stats;

static void	catch_signal(int signal)
{
	(void)signal;
	print_stats(g_stats);
	exit(0);
}

static void	save_stats(t_stats *stats)
{
	float	old_time;

	old_time = 0;
	stats->time_recv = get_time();
	old_time = stats->time;
	stats->time = stats->time_recv - stats->time_start;
	if (stats->min > stats->time)
		stats->min = stats->time;
	if (stats->max < stats->time)
		stats->max = stats->time;
	stats->avg += stats->time;
	if (stats->seq > 1)
	{
		if (stats->time > old_time)
			stats->mdev += stats->time - old_time;
		else
			stats->mdev += old_time - stats->time;
	}
}

static t_packet	create_packet(t_stats *stats)
{
	unsigned long	i;
	unsigned long	len_msg;
	t_packet		packet;

	i = 0;
	len_msg = sizeof(packet.msg) - 1;
	ft_bzero(&packet, sizeof(packet));
	packet.hdr.type = ICMP_ECHO;
	packet.hdr.un.echo.id = getpid();
	while (i < len_msg)
	{
		packet.msg[i] = i + '0';
		i++;
	}
	packet.msg[i] = 0;
	packet.hdr.un.echo.sequence = stats->seq++;
	packet.hdr.checksum = calc_checksum(&packet, sizeof(packet));
	usleep(PING_SLEEP_RATE);
	return (packet);
}

void	procces_recv(t_args *args, t_packet packet, bool sucess)
{
	struct sockaddr_in	*addr_config_recev;
	unsigned int		size_config_recev;

	size_config_recev = sizeof(addr_config_recev);
	if (!(recvfrom(args->sock, &packet, sizeof(packet), 0,
				(struct sockaddr*)&addr_config_recev, &size_config_recev)
			<= 0 && g_stats.seq > 1))
	{
		save_stats(&g_stats);
		if (sucess)
		{
			if (packet.hdr.type == RESERVED && packet.hdr.code
				== ECHO_REPLY)
				print_receive_success(args, &g_stats);
			else if (packet.hdr.type == RESERVED && packet.hdr.code
				== TTL_EXCCEDED && args->flags[V] == true)
				print_receive_fail(args, &g_stats);
		}
	}
}

int	send_ping(t_args *args, struct sockaddr_in *addr_config)
{
	bool				sucess;
	t_packet			packet;

	printf("PING %s (%s) 56(84) bytes of data.\n", args->ip_brut, args->ip);
	init_stats_and_time(&g_stats, args);
	signal(SIGINT, catch_signal);
	chek_rev_dns(args->ip, args);
	if (setsockopt(args->sock, SOL_IP, IP_TTL, &args->ttl, sizeof(args->ttl)))
		return (print_error("Set option socket fail"));
	setsockopt(args->sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&args->tv_out,
		sizeof args->tv_out);
	g_stats.time_gstart = get_time();
	while (1)
	{
		sucess = true;
		packet = create_packet(&g_stats);
		g_stats.time_start = get_time();
		if (sendto(args->sock, &packet, sizeof(packet), 0,
				(struct sockaddr*) addr_config, sizeof(*addr_config)) <= 0)
			sucess = false;
		else
			g_stats.pkt_transmited++;
		procces_recv(args, packet, sucess);
	}
	return (0);
}
