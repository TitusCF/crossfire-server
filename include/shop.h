/* Copyright 2003 Tim Rightnour */
/* $Id */

#ifndef _SHOP_H_
#define _SHOP_H_

typedef struct _supplydemand {
	char *name;
	char *title;
	uint32 bought;
	uint32 sold;
	int value;
	struct _supplydemand *next;
} supplydemand_t;

#endif
