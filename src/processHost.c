/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processHost.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:56 by sbaranes          #+#    #+#             */
/*   Updated: 2023/01/16 15:43:16 by sbaranes         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

void	chek_rev_dns(char *ip_addr, t_args *args)
{
	struct sockaddr_in	temp_addr;
	socklen_t			len;
	char				buf[NI_MAXHOST];

	ft_bzero(buf, NI_MAXHOST);
	args->hostname = NULL;
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	len = sizeof(struct sockaddr_in);
	if (getnameinfo((struct sockaddr *) &temp_addr, len, buf, sizeof(buf), NULL,
			0, NI_NAMEREQD))
		return ;
	else
		args->hostname = ft_strcpy(args->hostname, buf);
}

int	process_host(t_args *args, struct sockaddr_in *addr_config)
{
	struct hostent	*host_entity;

	host_entity = gethostbyname(args->ip);
	if (host_entity == NULL)
	{
		printf("ft_ping: %s: Name or service not known\n", args->ip);
		return (1);
	}
	args->ip_brut = args->ip;
	args->ip = inet_ntoa(*(struct in_addr *)host_entity->h_addr);
	if (ft_strncmp(args->ip_brut, args->ip, length(args->ip_brut)))
		args->diff = true;
	(*addr_config).sin_family = host_entity->h_addrtype;
	(*addr_config).sin_port = htons(0);
	(*addr_config).sin_addr.s_addr = *(long *)host_entity->h_addr;
	args->sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	return (EXIT_SUCCESS);
}

unsigned short	calc_checksum(void *packet, int len)
{
	unsigned short	*buf;
	unsigned int	sum;
	unsigned short	result;

	buf = packet;
	sum = 0;
	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return (result);
}
