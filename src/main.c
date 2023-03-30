/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:49 by sbaranes          #+#    #+#             */
/*   Updated: 2023/01/16 15:22:36 by sbaranes         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

static void	print_help(void)
{
	printf("\nUsage\n  ping [options] <destination>\n\nOptions:\n");
	printf("  <destination>      dns name or ip address\n");
	printf("  -h                 print help and exit\n");
	printf("  -v                 verbose output\n\n");
	printf("For more details see ping(8).\n");
	exit(0);
}

static int	procces_ac_three(char **av, t_args *args, int *old_i)
{
	int	i;
	int	y;

	i = 1;
	if (av[1][0] == '-' && (length(av[1]) >= 2 && length(av[1]) <= 3))
	{
		y = 0;
		while (av[1][++y] != '\0')
		{
			if (av[1][y] == 'h' || av[1][y] == 'v')
			{
				if (av[1][y] == 'h')
					args->flags[H] = true;
				else
					args->flags[V] = true;
			}
			else
				return (print_error("Ft_Ping: invalid option -- 'x'.\n"));
		}
	}
	else
		return (print_error(ERROR_FLAG));
	i++;
	*old_i = i;
	return (EXIT_SUCCESS);
}

static bool	pars_args(char **av, int ac, t_args *args)
{
	int	i;

	i = 1;
	init_args(args);
	if (ac == 3)
		if (procces_ac_three(av, args, &i))
			return (EXIT_FAILURE);
	args->ip = av[i];
	return (EXIT_SUCCESS);
}

int	main(int ac, char **av)
{
	t_args				args;
	struct sockaddr_in	addr_config;

	if (!(ac >= 2 && ac <= 3))
		return (print_error(ERROR_ARG));
	if (pars_args(av, ac, &args))
		return (EXIT_FAILURE);
	if (args.flags[H] == true)
		print_help();
	if (process_host(&args, &addr_config))
		return (EXIT_FAILURE);
	send_ping(&args, &addr_config);
	return (EXIT_SUCCESS);
}
