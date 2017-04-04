/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#ifndef _FR_CONTROL_H
#define _FR_CONTROL_H
/**
 * $Id$
 *
 * @file util/control.h
 * @brief control-plane signaling
 *
 * @copyright 2016 Alan DeKok <aland@freeradius.org>
 */
RCSIDH(control_h, "$Id$")

#include <freeradius-devel/util/atomic_queue.h>
#include <freeradius-devel/util/ring_buffer.h>
#include <freeradius-devel/util/time.h>

#include <sys/types.h>
#include <sys/event.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  A one-way control plane signal.
 *
 *  Multiple-producer, single consumer.
 */
typedef struct fr_control_t fr_control_t;
typedef	void (*fr_control_callback_t)(void *ctx, void const *data, size_t data_size, fr_time_t now);

/*
 *	A suggestion for max # of messages, and max message size.
 */
#define FR_CONTROL_MAX_MESSAGES (1024)
#define FR_CONTROL_MAX_SIZE	(64)

/*
 *	A list of pre-allocated IDs, so that the callers don't have to manage their own.
 */
#define FR_CONTROL_ID_CHANNEL (1)
#define FR_CONTROL_ID_SOCKET  (2)
#define FR_CONTROL_ID_WORKER  (3)

fr_control_t *fr_control_create(TALLOC_CTX *ctx, int kq, fr_atomic_queue_t *aq);
void fr_control_free(fr_control_t *c);

int fr_control_gc(fr_control_t *c, fr_ring_buffer_t *rb) CC_HINT(nonnull);

int fr_control_message_send(fr_control_t *c, fr_ring_buffer_t *rb, uint32_t id, void *data, size_t data_size) CC_HINT(nonnull);
int fr_control_message_service_kevent(fr_control_t *c, struct kevent const *kev) CC_HINT(nonnull);

int fr_control_message_push(fr_control_t *c, fr_ring_buffer_t *rb, uint32_t id, void *data, size_t data_size) CC_HINT(nonnull);
ssize_t fr_control_message_pop(fr_atomic_queue_t *aq, uint32_t *p_id, void *data, size_t data_size) CC_HINT(nonnull);

int fr_control_callback_add(fr_control_t *c, uint32_t id, void *ctx, fr_control_callback_t callback) CC_HINT(nonnull(1,4));
int fr_control_callback_delete(fr_control_t *c, uint32_t id) CC_HINT(nonnull);

void fr_control_service(fr_control_t *c, void *data, size_t data_size, fr_time_t now) CC_HINT(nonnull);

#ifdef __cplusplus
}
#endif

#endif /* _FR_CONTROL_H */
