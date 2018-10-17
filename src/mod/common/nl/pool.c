#include "mod/common/nl/pool.h"

#include "common/types.h"
#include "mod/common/nl/nl_common.h"
#include "mod/common/nl/nl_core2.h"
#include "mod/siit/pool.h"

static int pool_to_usr(struct ipv4_prefix *prefix, void *arg)
{
	return nlbuffer_write(arg, prefix, sizeof(*prefix));
}

static int handle_addr4pool_display(struct addr4_pool *pool,
		struct genl_info *info, union request_pool *request)
{
	struct nlcore_buffer buffer;
	struct ipv4_prefix *offset;
	int error;

	log_debug("Sending IPv4 address pool to userspace.");

	error = nlbuffer_init_response(&buffer, info, nlbuffer_response_max_size());
	if (error)
		return nlcore_respond(info, error);

	offset = request->display.offset_set ? &request->display.offset : NULL;
	error = pool_foreach(pool, pool_to_usr, &buffer, offset);
	nlbuffer_set_pending_data(&buffer, error > 0);
	error = (error >= 0)
			? nlbuffer_send(info, &buffer)
			: nlcore_respond(info, error);

	nlbuffer_clean(&buffer);
	return error;
}

static int handle_addr4pool_add(struct addr4_pool *pool,
		union request_pool *request, bool force)
{
	log_debug("Adding an address to an IPv4 address pool.");
	return pool_add(pool, &request->add.addrs, force);
}

static int handle_addr4pool_rm(struct addr4_pool *pool,
		union request_pool *request)
{
	log_debug("Removing an address from an IPv4 address pool.");
	return pool_rm(pool, &request->rm.addrs);
}

static int handle_addr4pool_flush(struct addr4_pool *pool)
{
	log_debug("Flushing an IPv4 address pool...");
	return pool_flush(pool);
}

static int handle_addr4pool(struct addr4_pool *pool, struct genl_info *info)
{
	struct request_hdr *hdr = get_jool_hdr(info);
	union request_pool *request = (union request_pool *)(hdr + 1);
	int error;

	if (xlat_is_nat64()) {
		log_err("Stateful NAT64 doesn't have IPv4 address pools.");
		return nlcore_respond(info, -EINVAL);
	}

	error = validate_request_size(info, sizeof(*request));
	if (error)
		return nlcore_respond(info, error);

	switch (be16_to_cpu(hdr->operation)) {
	case OP_FOREACH:
		return handle_addr4pool_display(pool, info, request);
	case OP_ADD:
		error = handle_addr4pool_add(pool, request, hdr->force);
		break;
	case OP_REMOVE:
		error = handle_addr4pool_rm(pool, request);
		break;
	case OP_FLUSH:
		error = handle_addr4pool_flush(pool);
		break;
	default:
		log_err("Unknown operation: %u", be16_to_cpu(hdr->operation));
		error = -EINVAL;
	}

	return nlcore_respond(info, error);
}

int handle_blacklist_config(struct xlator *jool, struct genl_info *info)
{
	return handle_addr4pool(jool->siit.blacklist, info);
}

int handle_pool6791_config(struct xlator *jool, struct genl_info *info)
{
	return handle_addr4pool(jool->siit.pool6791, info);
}