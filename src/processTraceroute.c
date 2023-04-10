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

void	process_traceroute(t_args *args)
{
	int	loop;
	int	seq;

	seq = 0;
	while (args->ttl <= args->max_ttl)
	{
		if (setsockopt(args->send_sock, SOL_IP, IP_TTL, args->ttl, sizeof(args->ttl)) < 0)
			error("setsockopt", strerror(errno));
		printf("%2d ", args->ttl);
		loop = 0;
		while (loop < 3)
		{
			args->server_addr.sin.sin_port = htons(++seq);
			sendto(args->send_sock, args->pkt.buffer, sizeof(args->pkt.hdr), 0, &args->server_addr.sa, sizeof(struct sockaddr_in));
			gettimeofday(&args->time, NULL);
		}

	}
}
