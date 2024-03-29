/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:49 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/07 15:58:02 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

static void	close_and_free(t_args *args)
{
	free(args->ip);
	close(args->sockfd);
	args->ip = NULL;
	args->sockfd = 0;
}

static char	*dns_lookup(char *addr_host, t_addr_in *addr_con)
{
	struct hostent	*host_entity;
	char			*ip;

	ip = NULL;
	host_entity = gethostbyname(addr_host);
	if (host_entity == NULL)
		return (NULL);
	ip = (char *)malloc(NI_MAXHOST * sizeof(char));
	ft_strcpy(ip, inet_ntoa(*(struct in_addr *)host_entity->h_addr));
	addr_con->sin_family = host_entity->h_addrtype;
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = *(long *)host_entity->h_addr;
	return (ip);
}

int	main(int ac, char **av)
{
	t_args		args;
	t_addr_in	addr_con;

	if (ac != 2)
		return printf("%s: Bad Args: usage: ./%s <address>\n", av[0], av[0]);
	init_args(&args);
	args.ip = dns_lookup(av[1], &addr_con);
	if (args.ip == NULL)
		return (printf("%s: could not resolve hostname\n", av[0]));
	args.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (args.sockfd < 0)
	{
		free(args.ip);
		return (printf("%s: Error creating socket!\n", av[0]));
	}
	printf("Traceroute to %s (%s), %d hops max, 60 byte packets\n",
		av[1], args.ip, MAX_TTL);
	process_traceroute(&args, &addr_con);
	close_and_free(&args);
	return (0);
}
