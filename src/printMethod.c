/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printMethod.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:53 by sbaranes          #+#    #+#             */
/*   Updated: 2023/01/16 16:48:11 by sbaranes         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

int	print_error(char *strError)
{
	return (write(2, strError, length(strError)));
}

void	print_stats(t_stats stats)
{
	printf("\n--- %s ping statistics ---\n", stats.ip);
	if (stats.pkt_transmited != 0 && stats.pkt_received != 0)
	{
		stats.pkt_lost = ((stats.pkt_transmited - stats.pkt_received)
				/ stats.pkt_transmited) * 100;
		stats.error = true;
	}
	stats.time = (get_time() - stats.time_gstart);
	printf("%d packets transmitted, %d received, ", stats.pkt_transmited,
		stats.pkt_received);
	if (stats.verbos == true && stats.pkt_error != 0)
		printf("+%d errors, %d%c packet loss, time %.0fms\n\n",
			stats.pkt_error, stats.pkt_lost, '%', stats.time);
	else
	{
		printf("%d%c packet loss, time %.0fms\n", stats.pkt_lost, '%',
			stats.time);
		stats.avg /= stats.seq;
		stats.mdev /= stats.seq;
		if (stats.error == true)
			printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", stats.min,
				stats.avg, stats.max, stats.mdev);
		else
			printf("\n");
	}
}

void	print_receive_success(t_args *args, t_stats *stats)
{
	printf("%d bytes from ", PKT_SIZE);
	if (args->diff)
		printf("%s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n",
			args->hostname, args->ip, stats->seq, args->ttl, stats->time);
	else
		printf("%s: icmp_seq=%d ttl=%d time=%.1f ms\n",
			args->ip, stats->seq, args->ttl, stats->time);
	stats->pkt_received++;
}

void	print_receive_fail(t_args *args, t_stats *stats)
{
	if (args->diff)
		printf("%d bytes from %s (%s): icmp_seq=%d Time to live exceeded\n",
			PKT_SIZE, args->ip, args->ip, stats->seq);
	else
		printf("%d bytes from %s: icmp_seq=%d Time to live exceeded\n",
			PKT_SIZE, args->ip, stats->seq);
	stats->pkt_error++;
}
