#ifndef _NFT_SHARED_H_
#define _NFT_SHARED_H_

#include <stdbool.h>

#include <libnftables/rule.h>
#include <libnftables/expr.h>

#include "xshared.h"

#if 0
#define DEBUGP(x, args...) fprintf(stdout, x, ## args)
#define NLDEBUG
#define DEBUG_DEL
#else
#define DEBUGP(x, args...)
#endif

/*
 * iptables print output emulation
 */

#define FMT_NUMERIC	0x0001
#define FMT_NOCOUNTS	0x0002
#define FMT_KILOMEGAGIGA 0x0004
#define FMT_OPTIONS	0x0008
#define FMT_NOTABLE	0x0010
#define FMT_NOTARGET	0x0020
#define FMT_VIA		0x0040
#define FMT_NONEWLINE	0x0080
#define FMT_LINENUMBERS 0x0100

#define FMT_PRINT_RULE (FMT_NOCOUNTS | FMT_OPTIONS | FMT_VIA \
			| FMT_NUMERIC | FMT_NOTABLE)
#define FMT(tab,notab) ((format) & FMT_NOTABLE ? (notab) : (tab))

struct nft_family_ops {
	int (*add)(struct nft_rule *r, struct iptables_command_state *cs);
	bool (*is_same)(const struct iptables_command_state *a,
			const struct iptables_command_state *b);
	void (*print_payload)(struct nft_rule_expr *e,
			      struct nft_rule_expr_iter *iter);
	void (*parse_meta)(struct nft_rule_expr *e, uint8_t key,
			   struct iptables_command_state *cs);
	void (*parse_payload)(struct nft_rule_expr_iter *iter,
			      struct iptables_command_state *cs,
			      uint32_t offset);
	void (*parse_immediate)(struct iptables_command_state *cs);
	uint8_t (*print_firewall)(const struct iptables_command_state *cs,
				  const char *targname, unsigned int num,
				  unsigned int format);
};

void add_meta(struct nft_rule *r, uint32_t key);
void add_payload(struct nft_rule *r, int offset, int len);
void add_bitwise_u16(struct nft_rule *r, int mask, int xor);
void add_cmp_ptr(struct nft_rule *r, uint32_t op, void *data, size_t len);
void add_cmp_u16(struct nft_rule *r, uint16_t val, uint32_t op);
void add_cmp_u32(struct nft_rule *r, uint32_t val, uint32_t op);
void add_iniface(struct nft_rule *r, char *iface, int invflags);
void add_outiface(struct nft_rule *r, char *iface, int invflags);
void add_addr(struct nft_rule *r, int offset,
	      void *data, size_t len, int invflags);
void add_proto(struct nft_rule *r, int offset, size_t len,
	       uint32_t proto, int invflags);

bool is_same_interfaces(const char *a_iniface, const char *a_outiface,
			unsigned const char *a_iniface_mask,
			unsigned const char *a_outiface_mask,
			const char *b_iniface, const char *b_outiface,
			unsigned const char *b_iniface_mask,
			unsigned const char *b_outiface_mask);

void parse_meta(struct nft_rule_expr *e, uint8_t key, char *iniface,
		unsigned char *iniface_mask, char *outiface,
		unsigned char *outiface_mask, uint8_t *invflags);

void print_proto(uint16_t proto, int invert);
void get_cmp_data(struct nft_rule_expr_iter *iter,
		  void *data, size_t dlen, bool *inv);
void print_num(uint64_t number, unsigned int format);
void print_firewall_details(const struct iptables_command_state *cs,
			    const char *targname, uint8_t flags,
			    uint8_t invflags, uint8_t proto,
			    const char *iniface, const char *outiface,
			    unsigned int num, unsigned int format);

struct nft_family_ops *nft_family_ops_lookup(int family);

#endif