/**
 * @file functions.h
 * @author m4ghaniofficial@gmail.com
 * @brief decisions and other stuff.
 * @version 0.1
 * @date 2022-05-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "interface.h"

enum ERRORS {
	SUCCESS,
	INVALID_IP,
	INVALID_MASK,
	INTF_NOT_FOUND,
	IP_NOT_FOUND,
	MEM_FAILED,
        ALREADY_EXIST,
	EXEC_FAIL
};

enum VALIDATION {
	VALID,
	INVALID
};

struct data {
	char name[IFNAMSIZ];
	struct in_addr ip;
	struct in_addr mask;
};

int validate_ip(struct in_addr ip);
int validate_mask(struct in_addr mask);

int get_ip(char *name, char *ip);
int get_mask(char *name, char *mask);
int get_mac(char *name, unsigned char *mac);
char *get_status(struct ifreq req);

int add_interface(char *name);
int add_ip(struct data input);
int delete_ip(struct data input);

int show_intf();
void print_err(int retval);

#endif
