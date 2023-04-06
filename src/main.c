/**
* @file main.c
* @author m4ghaniofficial@gmail.com
* @brief main file.
* @version 0.1
* @date 2022-05-21
*
* @copyright Copyright (c) 2022
*
*/

#include <string.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <stdio.h>

#include "functions.h"

#define ARGV_COMPARE(argv, command) !strncmp(argv, command, sizeof (argv))
#define INPUT_TOKENS_WITH_BITMASK 2
#define SHOW_COMMAND_LEN 2
#define CREATE_COMMAND_LEN 5
#define ADD_DEL_COMMAND_LEN 6

enum ACT {
	SHOW,
	NEW_INTF,
	ADD_IP,
	DEL_IP,
	INVALID_COMMAND
};

struct data input;

int get_bitmask(char *argv, char *ip)
{
	int counter = 0;
	int decimal_mask;
	char *mask_temp;
	char *token;

	token = strtok(argv, "/");
	while (token != NULL) {
		if(counter == 0)
			ip = token;
		if(counter == 1)
			mask_temp = token;
		counter++;
		token = strtok(NULL, "/");
	}
	if (counter == INPUT_TOKENS_WITH_BITMASK) {
		inet_aton(ip, &(input.ip));
		sscanf(mask_temp, "%i", &decimal_mask);
		return decimal_mask;
	} else{
		return -1;
	}
}

char *convert_bitmask(int decimal_mask, char *mask)
{
	long bits = 0;
	if(decimal_mask < 0)
		return NULL;
	if (decimal_mask == 0 || decimal_mask > 32)
		return NULL;

	bits = 0xffffffff ^ (1 << 32 - decimal_mask) - 1;
	sprintf(mask, "%ld.%ld.%ld.%ld",
	         (bits & 0x0000000000ff000000L) >> 24, (bits & 0x0000000000ff0000) >> 16,
	         (bits & 0x0000000000ff00) >> 8, bits & 0xff);
}

enum ACT parse_arg(int argc, char *argv[])
{
	int bitmask;
	char ip[INET_ADDRSTRLEN];
	char mask[INET_ADDRSTRLEN];

	if (argc != SHOW_COMMAND_LEN && argc != CREATE_COMMAND_LEN
	                && argc != ADD_DEL_COMMAND_LEN){
		return INVALID_COMMAND;
	}	

	if (ARGV_COMPARE (argv[1], "show")){
		return SHOW;
	}	

	if (ARGV_COMPARE (argv[1], "interface")) {

		if (ARGV_COMPARE (argv[2], "create") && ARGV_COMPARE (argv[3], "loopback")) {
			strncpy(input.name, argv[4], sizeof (input.name));
			return NEW_INTF;
		}

		if (ARGV_COMPARE (argv[3], "add")) {
			strncpy(input.name, argv[2], sizeof (input.name));
		
			bitmask = get_bitmask(argv[4], ip);
			if (bitmask == -1) {
				inet_aton(argv[4], &(input.ip));
				inet_aton(argv[5], &(input.mask));
			} else {
				
				if (convert_bitmask(bitmask, mask) == NULL){
					print_err(INVALID_MASK);
					return INVALID_COMMAND;
				}
				inet_aton(mask, &(input.mask));
			}
			return ADD_IP;
		}

		if (ARGV_COMPARE (argv[3], "del")) {
			strncpy(input.name, argv[2], sizeof (input.name));

			bitmask = get_bitmask(argv[4], ip);
			if (bitmask == -1) {
				inet_aton(argv[4], &(input.ip));
				inet_aton(argv[5], &(input.mask));
			} else {
				if (convert_bitmask(bitmask, mask) == NULL){
					print_err(INVALID_MASK);
					return INVALID_COMMAND;
				}
				inet_aton(mask, &(input.mask));
			}
			return DEL_IP;
		}
	} else{
		return INVALID_COMMAND;
	}
}

int main(int argc, char *argv[])
{
	int retval = 0;
	int action;

	action = parse_arg(argc, argv);
	if (action == INVALID_COMMAND) {
		fprintf(stderr, "\e[31mInvalid command.\x1b[0m\n");
		return 0;
	}

	switch (action) {
	case SHOW:
		retval = show_intf();
		break;
	case NEW_INTF:
		retval = add_interface(input.name);
		break;
	case ADD_IP:
		retval = add_ip(input);
		break;
	case DEL_IP:
		retval = delete_ip(input);
		break;
	default:
		break;
	}

	print_err(retval);
	return 0;
}
