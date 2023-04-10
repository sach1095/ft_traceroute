/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:49 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/09 18:00:28 by sbaranes         ###   ########.fr       */
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
	struct addrinfo hints = {0}, *result;
	hints.ai_family = addr_con->sa_family;

	if (getaddrinfo(addr_host, NULL, &hints, &result) != 0)
		return NULL;

	memcpy(&addr_con, result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);
	char *ip = (char *)malloc(NI_MAXHOST * sizeof(char));
	inet_ntop(AF_INET, addr_con, ip, INET_ADDRSTRLEN);
	return (ip);
}

int	main(int ac, char **av)
{
	t_args		args;
	t_addr_in	addr_con;
	struct timeval	timeout;


	if (ac != 2)
		return (printf("%s: Bad Args: usage: ./%s <address>\n", av[0], av[0]));
	init_args(&args);
	args.ip = dns_lookup(av[1], &addr_con);
	timeout.tv_usec = 0;
	timeout.tv_sec = RECV_TIMEOUT;
	if (args.ip == NULL)
		return (printf("Ft_traceroute : could not resolve hostname\n"));
	args.recv_sock = socket(addr_con.sa_family, SOCK_RAW, IPPROTO_ICMP);
	args.send_sock = socket(addr_con.sa_family, SOCK_DGRAM, IPPROTO_IP);
	// args.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	// if (setsockopt(args.sockfd, IPPROTO_IP, IP_TTL,
	// 		&args.ttl, sizeof(args.ttl)) != 0)
	// 	return (print_error("Error setting TTL value!\n"));
	// setsockopt(args.sockfd, SOL_SOCKET, SO_RCVTIMEO,
	// 	(const char *)&timeout, sizeof(timeout));
	if (args.send_sock < 0 || args.recv_sock < 0)
	{
		free(args.ip);
		return (printf("Ft_traceroute: Error creating socket!\n"));
	}
	if (setsockopt(args.recv_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
		return (printf("Ft_traceroute: Error setting socket!\n"));
	printf("Traceroute to %s (%s), %d hops max, 60 byte packets\n",
		av[1], args.ip, MAX_TTL);
	process_traceroute(&args, &addr_con);
	close_and_free(&args);
	return (0);
}
