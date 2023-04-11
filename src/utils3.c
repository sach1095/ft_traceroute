/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 18:16:22 by root              #+#    #+#             */
/*   Updated: 2023/04/11 18:18:23 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

void	print_help(void)
{
	printf("\nUsage\n  ping [options] <destination>\n\nOptions:\n");
	printf("  <destination>      dns name or ip address\n");
	printf("  -h                 print help and exit\n");
	printf("  -v                 verbose output\n\n");
	printf("For more details see ping(8).\n");
	exit(0);
}

void	close_and_free(t_args *args)
{
	free(args->ip);
	close(args->sockfd);
	args->ip = NULL;
	args->sockfd = 0;
}

int	procces_ac_three(char **av, t_args *args, int *old_i)
{
	int	i;
	int	y;

	i = 1;
	if (av[1][0] == '-' && (ft_strlen(av[1]) >= 2 && ft_strlen(av[1]) <= 3))
	{
		y = 0;
		while (av[1][++y] != '\0')
		{
			if (av[1][y] == 'h' || av[1][y] == 'I')
			{
				if (av[1][y] == 'h')
					args->flags[H] = true;
				else
					args->flags[I] = true;
			}
			else
				return (print_error("Ft_traceroute: invalid option -- 'x'.\n"));
		}
	}
	else
		return (print_error(ERROR_FLAG));
	i++;
	*old_i = i;
	return (EXIT_SUCCESS);
}