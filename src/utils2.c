#include "lib.h"

void	init_args(t_args *args)
{
	args->ttl = 1;
	args->ip = NULL;
	args->sockfd = 0;
	args->hostname = NULL;
}

void	set_packet_header(t_args *args)
{
	args->pkt.hdr.type = ICMP_ECHO;
	args->pkt.hdr.code = 0;
	args->pkt.hdr.checksum = 0;
	args->pkt.hdr.un.echo.id = htons(getpid() & 0xFFFF);
	args->pkt.hdr.un.echo.sequence = htons(1);
	args->pkt.hdr.checksum = calc_checksum((unsigned short *)&args->pkt.hdr, sizeof(args->pkt.hdr));
}

int	print_error(char *strError)
{
	return (write(2, strError, ft_strlen(strError)));
}
