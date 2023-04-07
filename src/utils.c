/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:46:02 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/07 15:37:20 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

double	get_time(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return ((t.tv_sec * 1000000.0) + t.tv_usec);
}

char	*ft_strcpy(char *dest, char *src)
{
	int	i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
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

void	print_recv_packet(t_args *args, t_addr_in	recv_addr, int loop)
{
	if (loop != 1)
		return ;
	if (args->hostname == NULL)
		printf("%d %s (%s)", args->ttl, inet_ntoa(recv_addr.sin_addr),
			inet_ntoa(recv_addr.sin_addr));
	else
	{
		printf("%d %s (%s)", args->ttl, args->hostname,
			inet_ntoa(recv_addr.sin_addr));
		free(args->hostname);
		args->hostname = NULL;
	}
}
