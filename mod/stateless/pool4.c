#include "nat64/mod/stateless/pool4.h"

#include <linux/rculist.h>
#include <linux/inet.h>

#include "nat64/common/str_utils.h"
#include "nat64/mod/stateless/pool.h"

static struct list_head pool;

int pool4_init(char *pref_strs[], int pref_count)
{
	return pool_init(pref_strs, pref_count, &pool);
}

void pool4_destroy(void)
{
	return pool_destroy(&pool);
}

int pool4_add(struct ipv4_prefix *prefix)
{
	return pool_add(&pool, prefix);
}

int pool4_remove(struct ipv4_prefix *prefix)
{
	return pool_remove(&pool, prefix);
}

int pool4_flush(void)
{
	return pool_flush(&pool);
}

bool pool4_contains(__be32 addr)
{
	struct pool_entry *entry;
	struct in_addr inaddr = { .s_addr = addr };

	list_for_each_entry_rcu(entry, &pool, list_hook) {
		if (ipv4_prefix_contains(&entry->prefix, &inaddr)) {
			return true;
		}
	}

	return false;
}

int pool4_for_each(int (*func)(struct ipv4_prefix *, void *), void *arg)
{
	return pool_for_each(&pool, func, arg);
}

int pool4_count(__u64 *result)
{
	return pool_count(&pool, result);
}

bool pool4_is_empty(void)
{
	return pool_is_empty(&pool);
}
