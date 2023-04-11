/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbaranes <sbaranes@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 14:45:24 by sbaranes          #+#    #+#             */
/*   Updated: 2023/04/11 18:21:16 by sbaranes         ###   ########.fr       */
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

# define H 0
# define I 1
# define MAX_TTL 30
# define RECV_TIMEOUT 1
# define PKT_SIZE 64
# define RESERVED 69
# define TTL_EXCCEDED 192
# define ECHO_REPLY 0
# define PING_SLEEP_RATE 1000000
# define ERROR_FLAG "Flags format invalide, you can use -[h, I]only.\n"
# define ERROR_ARG "Number of arguments incorrect.\n"

typedef struct sockaddr_in	t_addr_in;

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
	char		*ip;
	char		*hostname;
	int			sockfd;
	int			ttl;
	int			loop;
	double		t1;
	double		t2;
	double		t3;
	t_packet	pkt;
	bool		revc_error;
	int			recv_host;
	bool		flags[2];
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
** utils2.c
*/
int				procces_ac_three(char **av, t_args *args, int *old_i);
void			close_and_free(t_args *args);
void			print_help(void);

/*
** process_traceroute.c
*/
void			process_traceroute(t_args *args, t_addr_in *addr_con);

#endif
