/**
 * @file interface.h
 * @author m4ghaniofficial@gmail.com
 * @brief everything about interfaces.
 * @version 0.1
 * @date 2022-05-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <arpa/inet.h>

#include "functions.h"

extern struct data input;

struct ifaddrs *search_intf(char *name);
int new_intf(char *name);
int new_ip(struct data input);
int del_ip(char *name);

#endif
