/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2008-2015 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef __MALI_SESSION_H__
#define __MALI_SESSION_H__

#include "mali_mmu_page_directory.h"
#include "mali_osk.h"
#include "mali_osk_list.h"
#include "mali_memory_types.h"
#include "mali_memory_manager.h"

struct mali_timeline_system;
struct mali_soft_system;

/* Number of frame builder job lists per session. */
#define MALI_PP_JOB_FB_LOOKUP_LIST_SIZE 16
#define MALI_PP_JOB_FB_LOOKUP_LIST_MASK (MALI_PP_JOB_FB_LOOKUP_LIST_SIZE - 1)

struct mali_session_data {
	_mali_osk_notification_queue_t *ioctl_queue;

	_mali_osk_mutex_t *memory_lock; /**< Lock protecting the vm manipulation */
    _mali_osk_mutex_t *cow_lock; /** < Lock protecting the cow memory free manipulation */
#if 0
	_mali_osk_list_t memory_head; /**< Track all the memory allocated in this session, for freeing on abnormal termination */
#endif
	struct mali_page_directory *page_directory; /**< MMU page directory for this session */

	_MALI_OSK_LIST_HEAD(link); /**< Link for list of all sessions */
	_MALI_OSK_LIST_HEAD(pp_job_list); /**< List of all PP jobs on this session */

#if defined(CONFIG_MALI_DVFS)
	_mali_osk_atomic_t number_of_window_jobs; /**< Record the window jobs completed on this session in a period */
#endif

	_mali_osk_list_t pp_job_fb_lookup_list[MALI_PP_JOB_FB_LOOKUP_LIST_SIZE]; /**< List of PP job lists per frame builder id.  Used to link jobs from same frame builder. */

	struct mali_soft_job_system *soft_job_system; /**< Soft job system for this session. */
	struct mali_timeline_system *timeline_system; /**< Timeline system for this session. */

	mali_bool is_aborting; /**< MALI_TRUE if the session is aborting, MALI_FALSE if not. */
	mali_bool use_high_priority_job_queue; /**< If MALI_TRUE, jobs added from this session will use the high priority job queues. */
	u32 pid;
	char *comm;
	atomic_t mali_mem_array[MALI_MEM_TYPE_MAX]; /**< The array to record mem types' usage for this session. */
	atomic_t mali_mem_allocated_pages; /** The current allocated mali memory pages, which include mali os memory and mali dedicated memory.*/
	size_t max_mali_mem_allocated_size; /**< The past max mali memory allocated size, which include mali os memory and mali dedicated memory. */
	/* Added for new memroy system */
	struct mali_allocation_manager allocation_mgr;
};

_mali_osk_errcode_t mali_session_initialize(void);
void mali_session_terminate(void);

/* List of all sessions. Actual list head in mali_kernel_core.c */
extern _mali_osk_list_t mali_sessions;
/* Lock to protect modification and access to the mali_sessions list */
extern _mali_osk_spinlock_irq_t *mali_sessions_lock;

MALI_STATIC_INLINE void mali_session_lock(void)
{
	_mali_osk_spinlock_irq_lock(mali_sessions_lock);
}

MALI_STATIC_INLINE void mali_session_unlock(void)
{
	_mali_osk_spinlock_irq_unlock(mali_sessions_lock);
}

void mali_session_add(struct mali_session_data *session);
void mali_session_remove(struct mali_session_data *session);
u32 mali_session_get_count(void);

#define MALI_SESSION_FOREACH(session, tmp, link) \
	_MALI_OSK_LIST_FOREACHENTRY(session, tmp, &mali_sessions, struct mali_session_data, link)

MALI_STATIC_INLINE struct mali_page_directory *mali_session_get_page_directory(struct mali_session_data *session)
{
	return session->page_directory;
}

MALI_STATIC_INLINE void mali_session_memory_lock(struct mali_session_data *session)
{
	MALI_DEBUG_ASSERT_POINTER(session);
	_mali_osk_mutex_wait(session->memory_lock);
}

MALI_STATIC_INLINE void mali_session_memory_unlock(struct mali_session_data *session)
{
	MALI_DEBUG_ASSERT_POINTER(session);
	_mali_osk_mutex_signal(session->memory_lock);
}

MALI_STATIC_INLINE void mali_session_send_notification(struct mali_session_data *session, _mali_osk_notification_t *object)
{
	_mali_osk_notification_queue_send(session->ioctl_queue, object);
}

#if defined(CONFIG_MALI_DVFS)

MALI_STATIC_INLINE void mali_session_inc_num_window_jobs(struct mali_session_data *session)
{
	MALI_DEBUG_ASSERT_POINTER(session);
	_mali_osk_atomic_inc(&session->number_of_window_jobs);
}

/*
 * Get the max completed window jobs from all active session,
 * which will be used in  window render frame per sec calculate
 */
u32 mali_session_max_window_num(void);

#endif

void mali_session_memory_tracking(_mali_osk_print_ctx *print_ctx);

#endif /* __MALI_SESSION_H__ */
