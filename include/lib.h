/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:24 by sbaranes          #+#    #+#             */
/*   Updated: 2023/03/31 14:11:37 by sbaranes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIB_H
# define LIB_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <netinet/ip.h>
#include <sys/time.h>

#define MAX_TTL 30
#define RECV_TIMEOUT 1
# define PKT_SIZE 64
# define RESERVED 69
# define TTL_EXCCEDED 192
# define ECHO_REPLY 0
# define PING_SLEEP_RATE 1000000
# define ERROR_ARG "Number of arguments incorrect.\n"

/*
** Struct for the icmp packet
*/
typedef struct s_packet
{
	struct icmphdr	hdr;
}t_packet;

/*
** Struct for main variables
*/
typedef struct s_args
{
	char				*ip;
	char				*ip_brut;
	int					sockfd;
	bool				diff;
	int					ttl;
	struct timeval		timeout;
	t_packet			pkt;
}t_args;

/*
** Strcut for save the times of icmp packet like :
** min 41.445ms : best low time (minimum)
** avg 50.697 : average times (moyenne)
** max 110.247ms : best high time (maximum)
** mdev : 15.931 ms : moyenne des écarts de temps de chaque ping
*/
typedef struct s_stats
{
	float	min;
	float	avg;
	float	max;
	float	mdev;
	float	time;
	int		seq;
	int		pkt_lost;
	int		pkt_error;
	int		pkt_received;
	int		pkt_transmited;
	double	time_gstart;
	double	time_start;
	double	time_recv;
	double	time_btw_packet;
	char	*ip;
	bool	verbos;
	bool	error;
}t_stats;

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

/*
** process_traceroute.c
*/
void			process_traceroute(t_args *args, struct sockaddr_in *addr_con);

#endif
