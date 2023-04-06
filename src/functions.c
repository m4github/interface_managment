/**
 * @file functions.c
 * @author m4ghaniofficial@gmail.com
 * @brief decisions and other stuff.
 * @version 0.1
 * @date 2022-05-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>

#include "functions.h"
#include "interface.h"

int validate_ip(struct in_addr ip)
{
	if (ip.s_addr == 0)
		return INVALID;
	return VALID;
}

int validate_mask(struct in_addr mask)
{
	if (mask.s_addr == 0)
		return INVALID;
	if (mask.s_addr & (~mask.s_addr >> 1))
		return VALID;
	else
		return INVALID;
}

int get_ip(char *name, char *ip)
{
	int fd;
	struct ifreq req;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return EXEC_FAIL;

	strncpy(req.ifr_name, name, sizeof (req.ifr_name));
	if (ioctl(fd, SIOCGIFADDR, &req) == -1) {
		close(fd);
		return EXEC_FAIL;
	}
	close(fd);
	strncpy(ip, inet_ntoa(((struct sockaddr_in *)&req.ifr_addr)->sin_addr), INET_ADDRSTRLEN);
	return SUCCESS;
}

int get_mask(char *name, char *mask)
{
	int fd;
	struct ifreq req;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return EXEC_FAIL;
	strncpy(req.ifr_name, name, sizeof (req.ifr_name));
	if (ioctl(fd, SIOCGIFNETMASK, &req) == -1) {
		close(fd);
		return EXEC_FAIL;
	}
	close(fd);
	strncpy(mask, inet_ntoa(((struct sockaddr_in *)&req.ifr_netmask)->sin_addr), INET_ADDRSTRLEN);
	return SUCCESS;
}

int get_mac(char *name, unsigned char *mac)
{
	int fd ;
	struct ifreq req;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return EXEC_FAIL;
	strncpy(req.ifr_name, name, sizeof (req.ifr_name));
	if (ioctl(fd, SIOCGIFHWADDR, &req) != -1) {
		memcpy(mac,req.ifr_hwaddr.sa_data,ETHER_ADDR_LEN);
		close(fd);
	}
	close(fd);
	return EXEC_FAIL;
}

char *get_status(struct ifreq req)
{
	int fd ;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return NULL;
	if (ioctl(fd, SIOCGIFFLAGS, &req) == -1){
		close(fd);
		return NULL;
	}
	close(fd);
	if (req.ifr_flags && IFF_UP)
		return "up";
	else
		return "down";
}

int add_interface(char *name)
{
	if (search_intf(name) == NULL) {
		if (new_intf(name) == EXEC_FAIL)
			return EXEC_FAIL;
	} else
		return ALREADY_EXIST;
}

int add_ip(struct data input)
{
	int retval;

	retval = validate_ip(input.ip);
	if (retval == INVALID)
		return INVALID_IP;
	else if (retval == EXEC_FAIL)
		return EXEC_FAIL;

	if (validate_mask(input.mask) == INVALID)
		return INVALID_MASK;

	if (search_intf(input.name) == NULL)
		return INTF_NOT_FOUND;
	if (new_ip(input) == EXEC_FAIL)
		return EXEC_FAIL;
	return SUCCESS;
}

int delete_ip(struct data input)
{
	int retval;
	struct ifaddrs *intf;
	struct sockaddr_in ip_addrs;

	retval = validate_ip(input.ip);
	if (retval == INVALID)
		return INVALID_IP;
	else if (retval == EXEC_FAIL)
		return EXEC_FAIL;

	if (validate_mask(input.mask) == INVALID_MASK)
		return INVALID_MASK;

	intf = search_intf(input.name);
	if (intf == NULL)
		return INTF_NOT_FOUND;

	memset(&ip_addrs, 0, sizeof (struct sockaddr));
	ip_addrs.sin_family = AF_INET;
	ip_addrs.sin_addr.s_addr = input.ip.s_addr;
	if (input.ip.s_addr != ip_addrs.sin_addr.s_addr)
		return INVALID_IP;
	if (del_ip(input.name) == EXEC_FAIL)
		return EXEC_FAIL;
	return SUCCESS;
}

int show_intf()
{
	struct ifaddrs *intf;
	struct ifreq ifr;

	char *status;
	char ip[INET_ADDRSTRLEN];
	char mask[INET_ADDRSTRLEN];
	unsigned char mac[ETHER_ADDR_LEN];

	if (getifaddrs(&intf) == -1)
		return EXEC_FAIL;

	fprintf(stdout, "name\t|\tip\t|\tmask\t|\tmac\t |\tstatus\n");
	fprintf(stdout, "----------------------------------------------------"\
	         "----------------------\n");
	while (intf) {
		if (intf->ifa_addr == NULL || intf->ifa_addr->sa_family != AF_INET) {
			intf = intf->ifa_next;
			continue;
		}
		strncpy(ifr.ifr_name, intf->ifa_name, sizeof (ifr.ifr_name));

		get_ip(ifr.ifr_name, ip);
		get_mask(ifr.ifr_name, mask);
		get_mac(ifr.ifr_name, mac);
		status = get_status(ifr);

		fprintf(stdout, "%s\t%s\t%s\t%02X:%02X:%02X:%02X:%02X:%02X\t%s\n",
		         ifr.ifr_name,
		         ip, mask, mac[0], mac[1], mac[2], mac[3],
		         mac[4], mac[5], status);

		intf = intf->ifa_next;
	}
	freeifaddrs(intf);
	return SUCCESS;
}


void print_err(int retval)
{
	switch (retval) {
	case INVALID_IP:
		fprintf(stderr, "\e[31mInvalid IP.\x1b[0m\n");
		break;
	case INVALID_MASK:
		fprintf(stderr, "\e[31mInvalid mask.\x1b[0m\n");
		break;
	case INTF_NOT_FOUND:
		fprintf(stderr, "\e[31mInterface not found.\x1b[0m\n");
		break;
	case IP_NOT_FOUND:
		fprintf(stderr, "\e[31mIP not found.\x1b[0m\n");
		break;
	case MEM_FAILED:
		fprintf(stderr, "\e[31mCouldn't allocate memory.\x1b[0m\n");
		break;
	case EXEC_FAIL:
		fprintf(stderr, "\e[31mFailed to run command.\x1b[0m\n");
		break;
	case ALREADY_EXIST:
		fprintf(stderr, "\e[31mData already exist. Try another value.\x1b[0m\n");
		break;
	default:
		break;
	}
}
