/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:49 by sbaranes          #+#    #+#             */
/*   Updated: 2023/03/31 18:19:16 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

static void	init_args(t_args *args)
{
	args->ip = NULL;
	args->ttl = 1;
	args->sockfd = 0;
	args->timeout.tv_sec = 1;
	args->timeout.tv_usec = 0;
}

static int	print_error(char *strError)
{
	return (write(2, strError, ft_strlen(strError)));
}

static char	*dns_lookup(char *addr_host, struct sockaddr_in *addr_con)
{
	struct hostent	*host_entity;
	char			*ip;

	ip = NULL;
	host_entity = gethostbyname(addr_host);
	if (host_entity == NULL)
		return NULL;
	ip = (char *)malloc(NI_MAXHOST * sizeof(char));
	ft_strcpy(ip, inet_ntoa(*(struct in_addr *)host_entity->h_addr));
	addr_con->sin_family = host_entity->h_addrtype;
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = *(long *)host_entity->h_addr;
	return (ip);
}

static void	close_and_free(t_args *args)
{
	free(args->ip);
	close(args->sockfd);
	args->ip = NULL;
	args->sockfd = 0;
}

int	main(int ac, char **av)
{
	t_args				args;
	struct sockaddr_in	addr_con;

	if (ac != 2)
		print_error("Ft_traceroute: Bad Args: usage: ./ft_traceroute <address>\n");
	init_args(&args);
	args.ip = dns_lookup(av[1], &addr_con);
	if (args.ip == NULL)
		print_error("Ft_traceroute : could not resolve hostname\n");
	args.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (args.sockfd < 0)
	{
		free(args.ip);
		print_error("Ft_traceroute: Error creating socket!\n");
	}
	printf("Traceroute to %s (%s), %d hops max, 60 byte packets\n", av[1], args.ip, MAX_TTL);
	process_traceroute(&args, &addr_con);
	close_and_free(&args);
	return (0);
}
