#ifndef _JOOL_MOD_CORE_H
#define _JOOL_MOD_CORE_H

/**
 * @file
 * The core is the packet handling's entry point.
 */

#include <linux/skbuff.h>
#include "mod/common/types.h"
#include "mod/common/xlator.h"

/**
 * Assumes "skb" is a IPv6 packet, checks whether it should be NAT64'd and either translates and
 * sends it or does nothing.
 * Intended to be hooked to Netfilter.
 *
 * @return what should the caller do to the packet. see the NF_* constants.
 */
verdict core_6to4(struct sk_buff *skb, struct xlator *instance);
/**
 * Assumes "skb" is a IPv4 packet, checks whether it should be NAT64'd and either translates and
 * sends it or does nothing.
 * Intended to be hooked to Netfilter.
 *
 * @return what should the caller do to the packet. see the NF_* constants.
 */
verdict core_4to6(struct sk_buff *skb, struct xlator *instance);

#endif /* _JOOL_MOD_CORE_H */