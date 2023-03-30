/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:49 by sbaranes          #+#    #+#             */
/*   Updated: 2023/03/30 17:44:20 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

int	main(int ac, char **av)
{
	t_args				args;
	struct sockaddr_in	addr_config;

	if (!(ac != 2))
		return (print_error(ERROR_ARG));
	args.ip = av[1];
	if (process_host(&args, &addr_config))
		return (EXIT_FAILURE);
	send_ping(&args, &addr_config);
	return (EXIT_SUCCESS);
}
