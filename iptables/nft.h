#ifndef _NFT_H_
#define _NFT_H_

#include "xshared.h"
#include "nft-shared.h"

struct nft_handle {
	int			family;
	struct mnl_socket	*nl;
	uint32_t		portid;
	uint32_t		seq;
	bool			commit;
	struct nft_family_ops	*ops;
};

int nft_init(struct nft_handle *h);
void nft_fini(struct nft_handle *h);

/*
 * Operations with tables.
 */
struct nft_table;
struct nft_chain_list;

int nft_table_add(struct nft_handle *h, const struct nft_table *t);
int nft_for_each_table(struct nft_handle *h, int (*func)(struct nft_handle *h, const char *tablename, bool counters), bool counters);
bool nft_table_find(struct nft_handle *h, const char *tablename);
int nft_table_set_dormant(struct nft_handle *h, const char *table);
int nft_table_wake_dormant(struct nft_handle *h, const char *table);
int nft_table_purge_chains(struct nft_handle *h, const char *table, struct nft_chain_list *list);

/*
 * Operations with chains.
 */
struct nft_chain;

int nft_chain_add(struct nft_handle *h, const struct nft_chain *c);
int nft_chain_set(struct nft_handle *h, const char *table, const char *chain, const char *policy, const struct xt_counters *counters);
struct nft_chain_list *nft_chain_dump(struct nft_handle *h);
struct nft_chain *nft_chain_list_find(struct nft_handle *h, struct nft_chain_list *list, const char *table, const char *chain);
int nft_chain_save(struct nft_handle *h, struct nft_chain_list *list, const char *table);
int nft_chain_user_add(struct nft_handle *h, const char *chain, const char *table);
int nft_chain_user_del(struct nft_handle *h, const char *chain, const char *table);
int nft_chain_user_rename(struct nft_handle *h, const char *chain, const char *table, const char *newname);

/*
 * Operations with rule-set.
 */
struct nft_rule;

int nft_rule_add(struct nft_handle *h, const char *chain, const char *table, struct iptables_command_state *cmd, bool append, uint64_t handle, bool verbose);
int nft_rule_check(struct nft_handle *h, const char *chain, const char *table, struct iptables_command_state *cmd, bool verbose);
int nft_rule_delete(struct nft_handle *h, const char *chain, const char *table, struct iptables_command_state *cmd, bool verbose);
int nft_rule_delete_num(struct nft_handle *h, const char *chain, const char *table, int rulenum, bool verbose);
int nft_rule_replace(struct nft_handle *h, const char *chain, const char *table, struct iptables_command_state *cmd, int rulenum, bool verbose);
int nft_rule_list(struct nft_handle *h, const char *chain, const char *table, int rulenum, unsigned int format);
int nft_rule_list_save(struct nft_handle *h, const char *chain, const char *table, int rulenum, int counters);
int nft_rule_save(struct nft_handle *h, const char *table, bool counters);
int nft_rule_flush(struct nft_handle *h, const char *chain, const char *table);

enum nft_rule_print {
	NFT_RULE_APPEND,
	NFT_RULE_DEL,
};

void nft_rule_print_save(struct nft_rule *r, enum nft_rule_print type, bool counters);

/*
 * global commit and abort
 */
int nft_commit(struct nft_handle *h);
int nft_abort(struct nft_handle *h);

/*
 * revision compatibility.
 */
int nft_compatible_revision(const char *name, uint8_t rev, int opt);

/*
 * Error reporting.
 */
const char *nft_strerror(int err);

/* For xtables.c */
int do_commandx(struct nft_handle *h, int argc, char *argv[], char **table);

/*
 * Parse config for tables and chain helper functions
 */
#define XTABLES_CONFIG_DEFAULT  "/etc/xtables.conf"

struct nft_table_list;
struct nft_chain_list;

extern int xtables_config_parse(const char *filename, struct nft_table_list *table_list, struct nft_chain_list *chain_list);

enum {
	NFT_LOAD_VERBOSE = (1 << 0),
};

int nft_xtables_config_load(struct nft_handle *h, const char *filename, uint32_t flags);

#endif