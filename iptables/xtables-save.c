/* Code to save the xtables state, in human readable-form. */
/* (C) 1999 by Paul 'Rusty' Russell <rusty@rustcorp.com.au> and
 * (C) 2000-2002 by Harald Welte <laforge@gnumonks.org>
 * (C) 2012 by Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This code is distributed under the terms of GNU GPL v2
 *
 */
#include <getopt.h>
#include <sys/errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include "libiptc/libiptc.h"
#include "iptables.h"
#include "xtables-multi.h"
#include "nft.h"

#include <libnftables/chain.h>

#ifndef NO_SHARED_LIBS
#include <dlfcn.h>
#endif

static bool show_counters = false;

static const struct option options[] = {
	{.name = "counters", .has_arg = false, .val = 'c'},
	{.name = "dump",     .has_arg = false, .val = 'd'},
	{.name = "table",    .has_arg = true,  .val = 't'},
	{.name = "modprobe", .has_arg = true,  .val = 'M'},
	{.name = "ipv4",     .has_arg = false, .val = '4'},
	{.name = "ipv6",     .has_arg = false, .val = '6'},
	{NULL},
};

static int
do_output(struct nft_handle *h, const char *tablename, bool counters)
{
	struct nft_chain_list *chain_list;

	if (!tablename)
		return nft_for_each_table(h, do_output, counters);

	if (!nft_table_find(h, tablename)) {
		printf("Table `%s' does not exist\n", tablename);
		return 0;
	}

	chain_list = nft_chain_dump(h);

	time_t now = time(NULL);

	printf("# Generated by xtables-save v%s on %s",
	       IPTABLES_VERSION, ctime(&now));
	printf("*%s\n", tablename);

	/* Dump out chain names first,
	 * thereby preventing dependency conflicts */
	nft_chain_save(h, chain_list, tablename);
	nft_rule_save(h, tablename, counters);

	now = time(NULL);
	printf("COMMIT\n");
	printf("# Completed on %s", ctime(&now));

	return 1;
}

/* Format:
 * :Chain name POLICY packets bytes
 * rule
 */
int
xtables_save_main(int argc, char *argv[])
{
	const char *tablename = NULL;
	struct nft_handle h = {
		.family	= AF_INET,	/* default to AF_INET */
	};
	int c;

	xtables_globals.program_name = "xtables-save";
	/* XXX xtables_init_all does several things we don't want */
	c = xtables_init_all(&xtables_globals, NFPROTO_IPV4);
	if (c < 0) {
		fprintf(stderr, "%s/%s Failed to initialize xtables\n",
				xtables_globals.program_name,
				xtables_globals.program_version);
		exit(1);
	}
#if defined(ALL_INCLUSIVE) || defined(NO_SHARED_LIBS)
	init_extensions();
	init_extensions4();
#endif
	nft_init(&h);

	while ((c = getopt_long(argc, argv, "bcdt:46", options, NULL)) != -1) {
		switch (c) {
		case 'c':
			show_counters = true;
			break;

		case 't':
			/* Select specific table. */
			tablename = optarg;
			break;
		case 'M':
			xtables_modprobe_program = optarg;
			break;
		case 'd':
			do_output(&h, tablename, show_counters);
			exit(0);
		case '4':
			h.family = AF_INET;
			break;
		case '6':
			h.family = AF_INET6;
			break;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Unknown arguments found on commandline\n");
		exit(1);
	}

	return !do_output(&h, tablename, show_counters);
}