/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processTraceroute.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:59 by sbaranes          #+#    #+#             */
/*   Updated: 2023/03/31 18:20:37 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

t_stats	g_stats;

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

static t_packet	create_packet()
{
	t_packet	packet;

	ft_bzero(&packet, sizeof(packet));
	packet.hdr.type = ICMP_ECHO;
	packet.hdr.code = 0;
	packet.hdr.un.echo.id = htons(getpid() & 0xFFFF);
	packet.hdr.un.echo.sequence = htons(1);
	packet.hdr.checksum = 0;
	packet.hdr.checksum = calc_checksum(&packet, sizeof(packet));
	return (packet);
}

void	procces_recv(t_args *args, t_packet packet, struct sockaddr_in *addr_config)
{
	struct sockaddr_in	*recv_addr;
	unsigned int		size_config_recev;

	size_config_recev = sizeof(recv_addr);
	if (recvfrom(args->sock, &packet, sizeof(packet), 0,
				(struct sockaddr*)&recv_addr, &size_config_recev)
			<= 0)
	{
		 printf("%d\t*\n", args->ttl);
		// save_stats(&g_stats);
		// if (sucess)
		// {
		// 	if (packet.hdr.type == RESERVED && packet.hdr.code
		// 		== ECHO_REPLY)
		// 		print_receive_success(args, &g_stats);
		// }
	}
	else
	{
		chek_rev_dns(args->ip, args);
		if (args->hostname == NULL)
			printf("%d %s (%s)\n", args->ttl, args->ip, args->ip);
		else
			printf("%d %s (%s)\n", args->ttl, args->hostname, args->ip);
		if (recv_addr->sin_addr.s_addr == addr_config->sin_addr.s_addr)
			exit(0);
	}

}

int	process_traceroute(t_args *args, struct sockaddr_in *addr_config)
{
	t_packet	packet;

	printf("Ft_Traceroute %s (%s) 56(84) bytes of data.\n", args->ip_brut, args->ip);
	while (args->ttl <= MAX_TTL)
	{
		if (setsockopt(args->sock, IPPROTO_IP, IP_TTL, &args->ttl, sizeof(args->ttl)))
			return (print_error("Set option socket fail"));
		packet = create_packet();
		if (sendto(args->sock, &packet, sizeof(packet), 0,
				(struct sockaddr*) addr_config, sizeof(*addr_config)) < 0)
			return print_error("Ft_traceroute: Error sending packet");
		procces_recv(args, packet, addr_config);
		args->ttl++;
	}

	// init_stats_and_time(&g_stats, args);

	// g_stats.time_gstart = get_time();
	// while (1)
	// {
	// 	sucess = true;
	// 	g_stats.time_start = get_time();
	// 		sucess = false;
	// 	else
	// 		g_stats.pkt_transmited++;
	// }
	return (0);
}
