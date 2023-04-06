/**
 * @file interface.c
 * @author m4ghaniofficial@gmail.com
 * @brief everything about interfaces.
 * @version 0.1
 * @date 2022-05-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#include "interface.h"
#include "functions.h"

#define IP_ZERO_VALUE "0.0.0.0"

struct ifaddrs *search_intf(char *name)
{
	struct ifaddrs *intf, *intf_temp;
	if (getifaddrs(&intf) == -1)
		return NULL;
	intf_temp = intf;
	while (intf_temp) {
		if (!strncmp(intf_temp->ifa_name, name, sizeof (intf_temp->ifa_name))) {
			freeifaddrs(intf);
			return intf_temp;
		}
		intf_temp = intf_temp->ifa_next;
	}
	freeifaddrs(intf);
	return NULL;
}

int new_intf(char *name)
{
	char *args[] = {"ip", "link", "add", "name", name, "type", "dummy", NULL};
	if (execvp(args[0], args) == -1)
		return EXEC_FAIL;
	return SUCCESS;
}

int new_ip(struct data input)
{
	int fd ;
	struct ifreq req;
	struct sockaddr_in ip_addrs;
	struct sockaddr_in mask_addrs;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return EXEC_FAIL;

	strcpy(req.ifr_name, input.name);

	memset(&ip_addrs, 0, sizeof (struct sockaddr));
	ip_addrs.sin_family = AF_INET;
	ip_addrs.sin_addr.s_addr = input.ip.s_addr;
	memcpy(&req.ifr_addr, &ip_addrs, sizeof (struct sockaddr));
	if (ioctl(fd, SIOCSIFADDR, &req) < 0) {
		close(fd);
		return EXEC_FAIL;
	}

	memset(&mask_addrs, 0, sizeof (struct sockaddr));
	mask_addrs.sin_family = AF_INET;
	mask_addrs.sin_addr.s_addr = input.mask.s_addr;
	memcpy(&req.ifr_netmask, &mask_addrs, sizeof (struct sockaddr));
	if (ioctl(fd, SIOCSIFNETMASK, &req) < 0) {
		close(fd);
		return EXEC_FAIL;
	}
	close(fd);
	return SUCCESS;
}

int del_ip(char *name)
{
	int fd ;
	struct ifreq req;
	struct in_addr ipaddr;
	struct sockaddr_in ip_addrs;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return EXEC_FAIL;

	strcpy(req.ifr_name, name);
	inet_aton(IP_ZERO_VALUE, &ipaddr);

	memset(&ip_addrs, 0, sizeof (struct sockaddr));
	ip_addrs.sin_family = AF_INET;
	ip_addrs.sin_addr.s_addr = ipaddr.s_addr;
	memcpy(&req.ifr_addr, &ip_addrs, sizeof (struct sockaddr));
	if (ioctl(fd, SIOCSIFADDR, &req) < 0) {
		close(fd);
		return EXEC_FAIL;
	}
	return SUCCESS;
}
