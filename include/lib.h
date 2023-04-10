/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:24 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/09 18:25:12 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIB_H
# define LIB_H

# include <stdbool.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <netinet/ip_icmp.h>
# include <time.h>
# include <fcntl.h>
# include <signal.h>
# include <time.h>
# include <netinet/ip.h>
# include <sys/time.h>
# include <errno.h>
# include <limits.h>

# define RECV_TIMEOUT 1
# define ERROR_ARG "Number of arguments incorrect.\n"

typedef struct sockaddr	t_addr_in;

typedef union
{
	struct sockaddr sa;
	struct sockaddr_in sin;
	struct sockaddr_in6 sin6;
	struct sockaddr_storage ss;
} t_addr;

/*
** Struct for the icmp packet
*/
typedef struct s_packet
{
	struct icmphdr	hdr;
	char			buffer[USHRT_MAX];
}t_packet;


/*
** Struct for main variables
*/
typedef struct s_args
{
	char			*ip;
	int				recv_sock;
	int				send_sock;
	int				ttl;
	bool			icmp;
	int				max_ttl;
	int				datalen;
	t_packet		pkt;
	t_addr			server_addr;
	struct timeval	time;
}t_args;

/*******************************
** Methods in different files **
*******************************/

/*
** utils.c
*/
double			get_time(void);
int				ft_strlen(char *str);
char			*ft_strcpy(char *dest, char *src);
unsigned short	calc_checksum(void *packet, int len);
void			print_recv_packet(t_args *args, t_addr_in recv_addr);

/*
** utils2.c
*/
void			init_args(t_args *args);
int				print_error(char *strError);
void			set_packet_header(t_args *args);
void			print_time_recv(t_args *args);
bool			check_if_print(t_args *args, t_addr_in *addr_prev,
					t_addr_in recv_addr);

/*
** process_traceroute.c
*/
void			process_traceroute(t_args *args);

#endif
