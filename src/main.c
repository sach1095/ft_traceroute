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


char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con) {
	struct hostent *host_entity;
	char *ip = (char *)malloc(NI_MAXHOST * sizeof(char));

	if ((host_entity = gethostbyname(addr_host)) == NULL) {
		return NULL;
	}

	ft_strcpy(ip, inet_ntoa(*(struct in_addr *)host_entity->h_addr));

	addr_con->sin_family = host_entity->h_addrtype;
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = *(long *)host_entity->h_addr;

	return ip;
}

char *reverse_dns_lookup(char *ip_addr) {
	struct sockaddr_in temp_addr;
	socklen_t len;
	char buf[NI_MAXHOST], *ret_buf;

	temp_addr.sin_family = AF_INET;
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	len = sizeof(struct sockaddr_in);

	if (getnameinfo((struct sockaddr *)&temp_addr, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD)) {
		return NULL;
	}

	ret_buf = (char *)malloc((length(buf) + 1) * sizeof(char));
	ft_strcpy(ret_buf, buf);
	return ret_buf;
}

int	main(int ac, char **av)
{
	t_args				args;
	struct sockaddr_in	addr_config;

	if (!(ac != 2))
		return (print_error(ERROR_ARG));
	args.ip = av[1];
	if (process_host(&args, &addr_config))
		return (EXIT_FAILURE);
	process_traceroute(&args, &addr_config);
	return (EXIT_SUCCESS);
}
