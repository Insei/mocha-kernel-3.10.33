/*
<<<<<<< HEAD
 * Copyright (c) 2012-2015 NVIDIA Corporation. All rights reserved.
=======
 * Copyright (c) 2012-2016 NVIDIA Corporation. All rights reserved.
>>>>>>> update/master
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/atomic.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/printk.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
<<<<<<< HEAD
#include <linux/syscalls.h>
=======
>>>>>>> update/master
#include <asm/smp_plat.h>

#include "ote_protocol.h"

bool verbose_smc;
core_param(verbose_smc, verbose_smc, bool, 0644);

#define VR_AUTH_UUID	{0x0179ED96, 0x45A81ADB, 0x089DC68D, 0xBB520279}
#define SET_RESULT(req, r, ro)	{ req->result = r; req->result_origin = ro; }

<<<<<<< HEAD
static int te_pin_user_pages(void *buffer, size_t size,
		unsigned long *pages_ptr, uint32_t buf_type, bool *is_locked)
{
	int ret = 0;
	unsigned int nr_pages;
	struct page **pages = NULL;
	bool writable;
	struct vm_area_struct *vma = NULL;
	unsigned int flags;
	int i;
	bool is_locked_prev;

	nr_pages = (((uintptr_t)buffer & (PAGE_SIZE - 1)) +
			(size + PAGE_SIZE - 1)) >> PAGE_SHIFT;

	pages = kzalloc(nr_pages * sizeof(struct page *), GFP_KERNEL);
	if (!pages)
		return -ENOMEM;

	writable = (buf_type == TE_PARAM_TYPE_MEM_RW ||
		buf_type == TE_PARAM_TYPE_PERSIST_MEM_RW);

	down_read(&current->mm->mmap_sem);
	ret = get_user_pages(current, current->mm, (unsigned long)buffer,
			nr_pages, writable,
			0, pages, NULL);

	up_read(&current->mm->mmap_sem);

	if (ret <= 0) {
		pr_err("%s: Error %d in get_user_pages\n", __func__, ret);
		return ret;
	}

	*pages_ptr = (unsigned long) pages;
	nr_pages = ret;

	down_read(&current->mm->mmap_sem);

	is_locked_prev = false;
	vma = find_extend_vma(current->mm, (unsigned long)buffer);
	if (vma && (vma->vm_flags & VM_LOCKED))
		is_locked_prev = true;

	up_read(&current->mm->mmap_sem);

	/*
	 * Lock the pages if they are not already locked to ensure that
	 * AF bit is not set to zero.
	 */
	*is_locked = false;
	if (!is_locked_prev) {
		ret = sys_mlock((unsigned long)buffer, size);
		if (!ret)
			*is_locked = true;
		else
			/*
			 * Follow through even if mlock failed as it can be
			 * failed due to memory restrictions or invalid
			 * capabilities
			 */
			pr_warn("%s: Error %d in mlock, continuing session\n",
								__func__, ret);
	}

	down_read(&current->mm->mmap_sem);

	/* Fault pages to set the AF bit in PTE */
	flags = FAULT_FLAG_USER;
	if (writable)
		flags |= FAULT_FLAG_WRITE;
	for (i = 0; i < nr_pages; i++) {
		ret = fixup_user_fault(current, current->mm,
			(unsigned long)(buffer + (i * PAGE_SIZE)), flags);
		if (ret) {
			pr_err("%s: Error %d in fixup_user_fault\n",
							__func__, ret);
			break;
		}
	}

	up_read(&current->mm->mmap_sem);

	if (ret) {
		if (*is_locked)
			sys_munlock((unsigned long)buffer, size);
		return ret;
	}

	/* Return the number of pages pinned */
	return nr_pages;
}

static int te_prep_mem_buffer(uint32_t session_id,
		void *buffer, size_t size, uint32_t buf_type,
		struct tlk_context *context)
{
	unsigned long pages = 0;
	struct te_shmem_desc *shmem_desc = NULL;
	int ret = 0, nr_pages = 0;
	bool is_locked = false;

	/* allocate new shmem descriptor */
	shmem_desc = kzalloc(sizeof(struct te_shmem_desc), GFP_KERNEL);
	if (!shmem_desc) {
		pr_err("%s: te_add_shmem_desc failed\n", __func__);
		ret = OTE_ERROR_OUT_OF_MEMORY;
		goto error;
	}

	/* pin pages */
	nr_pages = te_pin_user_pages(buffer, size, &pages,
					buf_type, &is_locked);
	if (nr_pages <= 0) {
		pr_err("%s: te_pin_user_pages failed (%d)\n", __func__,
			nr_pages);
		ret = OTE_ERROR_OUT_OF_MEMORY;
		kfree(shmem_desc);
		goto error;
	}

	/* initialize shmem descriptor */
	INIT_LIST_HEAD(&(shmem_desc->list));
	shmem_desc->active = false;
	shmem_desc->buffer = buffer;
	shmem_desc->size = size;
	shmem_desc->nr_pages = nr_pages;
	shmem_desc->pages = (struct page **)(uintptr_t)pages;
	shmem_desc->is_locked = is_locked;

	/* add shmem descriptor to proper list */
	if ((buf_type == TE_PARAM_TYPE_MEM_RO) ||
		(buf_type == TE_PARAM_TYPE_MEM_RW))
		list_add_tail(&shmem_desc->list, &context->temp_shmem_list);
	else {
		list_add_tail(&shmem_desc->list, &context->persist_shmem_list);
	}

	return OTE_SUCCESS;
error:
	return ret;
}

static int te_prep_mem_buffers(struct te_request *request,
			struct tlk_context *context)
{
	uint32_t i;
	int ret = OTE_SUCCESS;
	struct te_oper_param *params = request->params;

	for (i = 0; i < request->params_size; i++) {
		switch (params[i].type) {
		case TE_PARAM_TYPE_NONE:
		case TE_PARAM_TYPE_INT_RO:
		case TE_PARAM_TYPE_INT_RW:
			break;
		case TE_PARAM_TYPE_MEM_RO:
		case TE_PARAM_TYPE_MEM_RW:
		case TE_PARAM_TYPE_PERSIST_MEM_RO:
		case TE_PARAM_TYPE_PERSIST_MEM_RW:
			ret = te_prep_mem_buffer(request->session_id,
				params[i].u.Mem.base,
				params[i].u.Mem.len,
				params[i].type,
				context);
			if (ret < 0) {
				pr_err("%s failed with err (%d)\n",
					__func__, ret);
				ret = OTE_ERROR_BAD_PARAMETERS;
				break;
			}
			break;
		default:
			pr_err("%s: OTE_ERROR_BAD_PARAMETERS\n", __func__);
			ret = OTE_ERROR_BAD_PARAMETERS;
			break;
		}
	}
	return ret;
}

static int te_prep_mem_buffers_compat(struct te_request_compat *request,
			struct tlk_context *context)
{
	uint32_t i;
	int ret = OTE_SUCCESS;
	struct te_oper_param_compat *params;

	params = (struct te_oper_param_compat *)(uintptr_t)request->params;
	for (i = 0; i < request->params_size; i++) {
		switch (params[i].type) {
		case TE_PARAM_TYPE_NONE:
		case TE_PARAM_TYPE_INT_RO:
		case TE_PARAM_TYPE_INT_RW:
			break;
		case TE_PARAM_TYPE_MEM_RO:
		case TE_PARAM_TYPE_MEM_RW:
		case TE_PARAM_TYPE_PERSIST_MEM_RO:
		case TE_PARAM_TYPE_PERSIST_MEM_RW:
			ret = te_prep_mem_buffer(request->session_id,
				(void *)(uintptr_t)params[i].u.Mem.base,
				params[i].u.Mem.len,
				params[i].type,
				context);
			if (ret < 0) {
				pr_err("%s failed with err (%d)\n",
					__func__, ret);
				ret = OTE_ERROR_BAD_PARAMETERS;
				break;
			}
			break;
		default:
			pr_err("%s: OTE_ERROR_BAD_PARAMETERS\n", __func__);
			ret = OTE_ERROR_BAD_PARAMETERS;
			break;
		}
=======
static struct te_session *te_get_session(struct tlk_context *context,
					uint32_t session_id)
{
	struct te_session *session, *tmp_session;

	list_for_each_entry_safe(session, tmp_session,
		&context->session_list, list) {
		if (session->session_id == session_id)
			return session;
>>>>>>> update/master
	}

<<<<<<< HEAD
static void te_release_mem_buffer(struct te_shmem_desc *shmem_desc)
{
	uint32_t i;
	int status;

	list_del(&shmem_desc->list);
	for (i = 0; i < shmem_desc->nr_pages; i++) {
		if ((shmem_desc->type == TE_PARAM_TYPE_MEM_RW) ||
			(shmem_desc->type == TE_PARAM_TYPE_PERSIST_MEM_RW))
			set_page_dirty_lock(shmem_desc->pages[i]);
		page_cache_release(shmem_desc->pages[i]);
	}
	kfree(shmem_desc->pages);

	if (shmem_desc->is_locked) {
		status = sys_munlock((unsigned long)shmem_desc->buffer,
							shmem_desc->size);
		if (status)
			pr_err("%s:Error %d in munlock\n", __func__, status);
	}

	kfree(shmem_desc);
}

static void te_release_temp_mem_buffers(struct tlk_context *context)
{
	struct te_shmem_desc *shmem_desc, *tmp_shmem_desc;

	if (list_empty(&context->temp_shmem_list))
		return;

	list_for_each_entry_safe(shmem_desc, tmp_shmem_desc,
		&context->temp_shmem_list, list) {
		te_release_mem_buffer(shmem_desc);
	}
}

static void te_release_persist_mem_buffers(uint32_t session_id,
	struct tlk_context *context)
{
	struct te_shmem_desc *shmem_desc, *tmp_shmem_desc;

	if (list_empty(&context->persist_shmem_list))
		return;

	/*
	 * Release any persistent mem buffers that either belong to
	 * the specified session_id or are not currently marked active
	 * (i.e. because the associated open_session or launch_operation
	 * failed).
	 */
	list_for_each_entry_safe(shmem_desc, tmp_shmem_desc,
		&context->persist_shmem_list, list) {
		if ((shmem_desc->session_id == session_id) ||
			(!shmem_desc->active))
			te_release_mem_buffer(shmem_desc);
	}
}

static void te_update_persist_mem_buffers(uint32_t session_id,
	struct tlk_context *context)
{
	struct te_shmem_desc *shmem_desc, *tmp_shmem_desc;

	/*
	 * Assumes any entries that have yet to be marked active belong
	 * to the session associated with the session_id that has been
	 * passed in.
	 */
	list_for_each_entry_safe(shmem_desc, tmp_shmem_desc,
		&context->persist_shmem_list, list) {

		if (!shmem_desc->active) {
			shmem_desc->session_id = session_id;
			shmem_desc->active = true;
		}
	}
=======
	return NULL;
>>>>>>> update/master
}

#ifdef CONFIG_SMP
cpumask_t saved_cpu_mask;
static long switch_cpumask_to_cpu0(void)
{
	long ret;
	cpumask_t local_cpu_mask = CPU_MASK_NONE;

	cpu_set(0, local_cpu_mask);
	cpumask_copy(&saved_cpu_mask, tsk_cpus_allowed(current));
	ret = sched_setaffinity(0, &local_cpu_mask);
	if (ret)
		pr_err("%s: sched_setaffinity #1 -> 0x%lX", __func__, ret);
<<<<<<< HEAD
=======

	return ret;
>>>>>>> update/master
}

static void restore_cpumask(void)
{
	long ret = sched_setaffinity(0, &saved_cpu_mask);
	if (ret)
		pr_err("%s: sched_setaffinity #2 -> 0x%lX", __func__, ret);
}
#else
static inline long switch_cpumask_to_cpu0(void) { return 0; };
static inline void restore_cpumask(void) {};
#endif

<<<<<<< HEAD
uint32_t tlk_generic_smc(uint32_t arg0, uintptr_t arg1, uintptr_t arg2)
{
	uint32_t retval;

	switch_cpumask_to_cpu0();

	retval = _tlk_generic_smc(arg0, arg1, arg2);
	while (retval == TE_ERROR_PREEMPT_BY_IRQ ||
	       retval == TE_ERROR_PREEMPT_BY_FS) {
		if (retval == TE_ERROR_PREEMPT_BY_FS)
			tlk_ss_op();
		retval = _tlk_generic_smc(TE_SMC_RESTART, 0, 0);
	}
=======
struct tlk_smc_work_args {
	uint32_t arg0;
	uintptr_t arg1;
	uint32_t arg2;
};
>>>>>>> update/master

static long tlk_generic_smc_on_cpu0(void *args)
{
	struct tlk_smc_work_args *work;
	int callback_status = 0;
	uint32_t retval;

	work = (struct tlk_smc_work_args *)args;
	retval = _tlk_generic_smc(work->arg0, work->arg1, work->arg2);

<<<<<<< HEAD
	retval = _tlk_extended_smc(regs);
	while (retval == TE_ERROR_PREEMPT_BY_IRQ)
		retval = _tlk_generic_smc(TE_SMC_RESTART, 0, 0);
=======
	while (retval == TE_ERROR_PREEMPT_BY_IRQ ||
	       retval == TE_ERROR_PREEMPT_BY_FS) {
		if (retval == TE_ERROR_PREEMPT_BY_FS)
			callback_status = tlk_ss_op();
		retval = _tlk_generic_smc(TE_SMC_RESTART, callback_status, 0);
	}
>>>>>>> update/master

	/* Print TLK logs if any */
	ote_print_logs();

	return retval;
}

/*
 * This routine is called both from normal threads and worker threads.
 * The worker threads are per-cpu and have PF_NO_SETAFFINITY set, so
 * any calls to sched_setaffinity will fail.
 *
 * If it's a worker thread on CPU0, just invoke the SMC directly. If
 * it's running on a non-CPU0, use work_on_cpu() to schedule the SMC
 * on CPU0.
 */
uint32_t send_smc(uint32_t arg0, uintptr_t arg1, uintptr_t arg2)
{
	long ret;
	struct tlk_smc_work_args work_args;

	work_args.arg0 = arg0;
	work_args.arg1 = arg1;
	work_args.arg2 = arg2;

	if (current->flags &
	    (PF_WQ_WORKER | PF_NO_SETAFFINITY | PF_KTHREAD)) {
		int cpu = cpu_logical_map(get_cpu());
		put_cpu();

		/* workers don't change CPU. depending on the CPU, execute
		 * directly or sched work */
		if (cpu == 0 && (current->flags & PF_WQ_WORKER))
			return tlk_generic_smc_on_cpu0(&work_args);
		else
			return work_on_cpu(0,
					tlk_generic_smc_on_cpu0, &work_args);
	}

	/* switch to CPU0 */
	ret = switch_cpumask_to_cpu0();
	if (ret) {
		/* not able to switch, schedule work on CPU0 */
		ret = work_on_cpu(0, tlk_generic_smc_on_cpu0, &work_args);
	} else {
		/* switched to CPU0 */
		ret = tlk_generic_smc_on_cpu0(&work_args);
		restore_cpumask();
	}

	return ret;
}

struct tlk_smc_work_args {
	uint32_t arg0;
	uintptr_t arg1;
	uint32_t arg2;
};

static long tlk_generic_smc_on_cpu0(void *args)
{
	struct tlk_smc_work_args *work;
	int cpu = cpu_logical_map(smp_processor_id());
	uint32_t retval;

	BUG_ON(cpu != 0);

	work = (struct tlk_smc_work_args *)args;
	retval = _tlk_generic_smc(work->arg0, work->arg1, work->arg2);
	while (retval == TE_ERROR_PREEMPT_BY_IRQ)
		retval = _tlk_generic_smc(TE_SMC_RESTART, 0, 0);
	return retval;
}

/*
 * Do an SMC call
 */
static void do_smc(struct te_request *request, struct tlk_device *dev)
{
	uint32_t smc_args;
	uint32_t smc_params = 0;

	smc_args = (uintptr_t)request - (uintptr_t)dev->req_addr;
	if (request->params) {
		smc_params =
			(uintptr_t)request->params - (uintptr_t)dev->req_addr;
	}

	(void)send_smc(request->type, smc_args, smc_params);
}

/*
 * VPR programming SMC
 *
 * This routine is called both from normal threads and worker threads.
 * The worker threads are per-cpu and have PF_NO_SETAFFINITY set, so
 * any calls to sched_setaffinity will fail.
 *
 * If it's a worker thread on CPU0, just invoke the SMC directly. If
 * it's running on a non-CPU0, use work_on_cpu() to schedule the SMC
 * on CPU0.
 */
int te_set_vpr_params(void *vpr_base, size_t vpr_size)
{
	uint32_t retval;

	/* Share the same lock used when request is send from user side */
	mutex_lock(&smc_lock);

<<<<<<< HEAD
	if (current->flags &
	    (PF_WQ_WORKER | PF_NO_SETAFFINITY | PF_KTHREAD)) {
		struct tlk_smc_work_args work_args;
		int cpu = cpu_logical_map(smp_processor_id());

		work_args.arg0 = TE_SMC_PROGRAM_VPR;
		work_args.arg1 = (uintptr_t)vpr_base;
		work_args.arg2 = vpr_size;

		/* workers don't change CPU. depending on the CPU, execute
		 * directly or sched work */
		if (cpu == 0 && (current->flags & PF_WQ_WORKER))
			retval = tlk_generic_smc_on_cpu0(&work_args);
		else
			retval = work_on_cpu(0,
					tlk_generic_smc_on_cpu0, &work_args);
	} else {
		retval = tlk_generic_smc(TE_SMC_PROGRAM_VPR,
					(uintptr_t)vpr_base, vpr_size);
	}
=======
	retval = send_smc(TE_SMC_PROGRAM_VPR, (uintptr_t)vpr_base, vpr_size);
>>>>>>> update/master

	mutex_unlock(&smc_lock);

	if (retval != OTE_SUCCESS) {
		pr_err("%s: smc failed err (0x%x)\n", __func__, retval);
		return -EINVAL;
	}
	return 0;
}
EXPORT_SYMBOL(te_set_vpr_params);

void te_restore_keyslots(void)
{
	uint32_t retval;

<<<<<<< HEAD
	mutex_lock(&smc_lock);

	if (current->flags &
			(PF_WQ_WORKER | PF_NO_SETAFFINITY | PF_KTHREAD)) {
		struct tlk_smc_work_args work_args;
		int cpu = cpu_logical_map(smp_processor_id());

		work_args.arg0 = TE_SMC_TA_EVENT;
		work_args.arg1 = TA_EVENT_RESTORE_KEYS;
		work_args.arg2 = 0;

		/* workers don't change CPU. depending on the CPU, execute
		 * directly or sched work */
		if (cpu == 0 && (current->flags & PF_WQ_WORKER)) {
			retval = tlk_generic_smc_on_cpu0(&work_args);
		} else {
			retval = work_on_cpu(0,
					tlk_generic_smc_on_cpu0, &work_args);
		}
	} else {
		retval = tlk_generic_smc(TE_SMC_TA_EVENT,
				TA_EVENT_RESTORE_KEYS, 0);
	}

	mutex_unlock(&smc_lock);
}
EXPORT_SYMBOL(te_restore_keyslots);

=======
	/* Share the same lock used when request is send from user side */
	mutex_lock(&smc_lock);

	retval = send_smc(TE_SMC_TA_EVENT, TA_EVENT_RESTORE_KEYS, 0);

	mutex_unlock(&smc_lock);

	if (retval != OTE_SUCCESS) {
		pr_err("%s: smc failed err (0x%x)\n", __func__, retval);
	}
}
EXPORT_SYMBOL(te_restore_keyslots);

/*
 * VRR Set Buffer
 *
 * Called from the DC driver and implemented as a monitor fastcall
 * to avoid taking the smc_lock. This call passes in the physical
 * address for the shared memory buffer.
 */
int te_vrr_set_buf(phys_addr_t addr)
{
	return _tlk_generic_smc(TE_SMC_VRR_SET_BUF, addr, 0);
}
EXPORT_SYMBOL(te_vrr_set_buf);

/*
 * VRR Sec
 *
 * Called from the DC driver and implemented as a monitor fastcall
 * to avoid taking the smc_lock.
 */
void te_vrr_sec(void)
{
	_tlk_generic_smc(TE_SMC_VRR_SEC, 0, 0);
}
EXPORT_SYMBOL(te_vrr_sec);

>>>>>>> update/master
/*
 * Open session SMC (supporting client-based te_open_session() calls)
 */
void te_open_session(struct te_opensession *cmd,
		    struct te_request *request,
		    struct tlk_context *context)
{
	struct te_session *session;
	int ret;

<<<<<<< HEAD
	request->type = TE_SMC_OPEN_SESSION;

	ret = te_prep_mem_buffers(request, context);
=======
	session = kzalloc(sizeof(struct te_session), GFP_KERNEL);
	if (!session) {
		SET_RESULT(request, OTE_ERROR_OUT_OF_MEMORY,
			OTE_RESULT_ORIGIN_API);
		return;
	}

	INIT_LIST_HEAD(&session->list);
	INIT_LIST_HEAD(&session->temp_shmem_list);
	INIT_LIST_HEAD(&session->inactive_persist_shmem_list);
	INIT_LIST_HEAD(&session->persist_shmem_list);

	request->type = TE_SMC_OPEN_SESSION;

	ret = te_prep_mem_buffers(request, session);
>>>>>>> update/master
	if (ret != OTE_SUCCESS) {
		pr_err("%s: te_prep_mem_buffers failed err (0x%x)\n",
			__func__, ret);
		SET_RESULT(request, ret, OTE_RESULT_ORIGIN_API);
		kfree(session);
		return;
	}

	memcpy(&request->dest_uuid,
	       &cmd->dest_uuid,
	       sizeof(struct te_service_id));

	pr_debug("OPEN_CLIENT_SESSION: 0x%x 0x%x 0x%x 0x%x\n",
		request->dest_uuid[0],
		request->dest_uuid[1],
		request->dest_uuid[2],
		request->dest_uuid[3]);

	do_smc(request, context->dev);

<<<<<<< HEAD
	if (request->result) {
		/* release any persistent mem buffers if we failed */
		te_release_persist_mem_buffers(request->session_id, context);
	} else {
		/* mark active any persistent mem buffers */
		te_update_persist_mem_buffers(request->session_id, context);
	}

	te_release_temp_mem_buffers(context);
=======
	if (request->result)
		goto error;

	/* release temporary mem buffers */
	te_release_mem_buffers(&session->temp_shmem_list);

	/* move persistent mem buffers to active list */
	te_activate_persist_mem_buffers(session);

	/* save off session_id and add to list */
	session->session_id = request->session_id;
	list_add_tail(&session->list, &context->session_list);

	return;
error:
	/* release buffers allocated in te_prep_mem_buffers */
	te_release_mem_buffers(&session->temp_shmem_list);
	te_release_mem_buffers(&session->inactive_persist_shmem_list);

	kfree(session);
>>>>>>> update/master
}

/*
 * Close session SMC (supporting client-based te_close_session() calls)
 */
void te_close_session(struct te_closesession *cmd,
		     struct te_request *request,
		     struct tlk_context *context)
{
	struct te_session *session;

	request->session_id = cmd->session_id;
	request->type = TE_SMC_CLOSE_SESSION;

	do_smc(request, context->dev);
	if (request->result)
<<<<<<< HEAD
		pr_info("%s: error closing session: %08x\n",
			__func__, request->result);

	/* release any peristent mem buffers */
	te_release_persist_mem_buffers(request->session_id, context);
=======
		pr_info("%s: error closing session: 0x%08x\n",
			__func__, request->result);

	session = te_get_session(context, cmd->session_id);
	if (!session) {
		pr_info("%s: session_id not found: 0x%x\n",
			__func__, cmd->session_id);
		return;
	}

	/* free session state */
	te_release_mem_buffers(&session->persist_shmem_list);
	list_del(&session->list);
	kfree(session);
>>>>>>> update/master
}

/*
 * Launch operation SMC (supporting client-based te_launch_operation() calls)
 */
void te_launch_operation(struct te_launchop *cmd,
			struct te_request *request,
			struct tlk_context *context)
{
	struct te_session *session;
	int ret;

<<<<<<< HEAD
	request->session_id = cmd->session_id;
	request->command_id = cmd->operation.command;
	request->type = TE_SMC_LAUNCH_OPERATION;

	ret = te_prep_mem_buffers(request, context);
	if (ret != OTE_SUCCESS) {
		pr_err("%s: te_prep_mem_buffers failed err (0x%x)\n",
			__func__, ret);
		SET_RESULT(request, ret, OTE_RESULT_ORIGIN_API);
		return;
	}

	do_smc(request, context->dev);

	if (request->result) {
		/* release any persistent mem buffers if we failed */
		te_release_persist_mem_buffers(request->session_id, context);
	} else {
		/* mark active any persistent mem buffers */
		te_update_persist_mem_buffers(request->session_id, context);
	}

	te_release_temp_mem_buffers(context);
=======
	session = te_get_session(context, cmd->session_id);
	if (!session) {
		pr_info("%s: session_id not found: 0x%x\n",
			__func__, cmd->session_id);
		SET_RESULT(request, OTE_ERROR_BAD_PARAMETERS,
				OTE_RESULT_ORIGIN_API);
		return;
	}

	request->session_id = cmd->session_id;
	request->command_id = cmd->operation.command;
	request->type = TE_SMC_LAUNCH_OPERATION;

	ret = te_prep_mem_buffers(request, session);
	if (ret != OTE_SUCCESS) {
		pr_err("%s: te_prep_mem_buffers failed err (0x%x)\n",
			__func__, ret);
		SET_RESULT(request, ret, OTE_RESULT_ORIGIN_API);
		return;
	}

	do_smc(request, context->dev);

	if (request->result)
		goto error;

	/* move persistent mem buffers to active list */
	te_activate_persist_mem_buffers(session);

	/* release temporary mem buffers */
	te_release_mem_buffers(&session->temp_shmem_list);

	return;

error:
	/* release buffers allocated in te_prep_mem_buffers */
	te_release_mem_buffers(&session->temp_shmem_list);
	te_release_mem_buffers(&session->inactive_persist_shmem_list);
>>>>>>> update/master
}

void te_authenticate_vrr(u8 *buf_ptr, u32 buflen)
{
	u32 i, no_of_params = 1;
	struct te_request *request;
	struct te_oper_param user_param;
	struct te_oper_param *param_array;
	struct te_oper_param *params = NULL;
	struct te_cmd_req_desc *cmd_desc = NULL;
	u32 session_id, vrr_auth_uuid[4] = VR_AUTH_UUID;

	mutex_lock(&smc_lock);

	/* Open & submit the work to SMC */
	cmd_desc = NULL;
	params = NULL;
	no_of_params =  1;

	cmd_desc = te_get_free_cmd_desc(&tlk_dev);
	params = te_get_free_params(&tlk_dev, no_of_params);

	if (!cmd_desc || !params) {
		pr_err("failed to get cmd_desc/params\n");
		goto error;
	}

	/* Request and parameter are prepared for VRR authenticaiton */
	request = cmd_desc->req_addr;
	memset(request, 0, sizeof(struct te_request));
	request->params = (uintptr_t)params;
	request->params_size = no_of_params;
	request->type = TE_SMC_OPEN_SESSION;

	user_param.index = 0;
	user_param.u.Mem.len = buflen;
	user_param.type = TE_PARAM_TYPE_MEM_RW;
	user_param.u.Mem.type = TE_MEM_TYPE_NS_KERNEL;
	user_param.u.Mem.base = (uint64_t)(uintptr_t)buf_ptr;
	memcpy(request->dest_uuid, vrr_auth_uuid, sizeof(vrr_auth_uuid));

	param_array = (struct te_oper_param *)(uintptr_t)request->params;

	for (i = 0; i < no_of_params; i++)
		memcpy(param_array + i, &user_param, sizeof(struct te_oper_param));

	do_smc(request, &tlk_dev);
	session_id = request->session_id;

	if (request->result) {
		pr_err("%s: error opening session: 0x%08x\n",
			__func__, request->result);
		goto error;
	}

	/* Close the session */
	request = cmd_desc->req_addr;
	memset(request, 0, sizeof(struct te_request));

	request->type = TE_SMC_CLOSE_SESSION;
	request->session_id = session_id;
	memcpy(request->dest_uuid, vrr_auth_uuid, sizeof(vrr_auth_uuid));

	do_smc(request, &tlk_dev);

	if (request->result) {
		pr_err("%s: error closing session: 0x%08x\n",
			__func__, request->result);
	}

error:
	if (cmd_desc)
		te_put_used_cmd_desc(&tlk_dev, cmd_desc);

	if (params)
		te_put_free_params(&tlk_dev, params, no_of_params);

	mutex_unlock(&smc_lock);
}
EXPORT_SYMBOL(te_authenticate_vrr);

/*
 * Command to open a session with the trusted app.
 * This API should only be called from the kernel space.
 * Takes UUID of the TA and size as argument
 * Returns Session ID if success or ERR when failure
 */
int te_open_trusted_session(u32 *ta_uuid, u32 uuid_size,
				u32 *session_id)
{
	struct te_request *request;
	struct te_cmd_req_desc *cmd_desc = NULL;

<<<<<<< HEAD
	request->type = TE_SMC_OPEN_SESSION;

	ret = te_prep_mem_buffers_compat(request, context);
	if (ret != OTE_SUCCESS) {
		pr_err("%s: te_prep_mem_buffers failed err (0x%x)\n",
			__func__, ret);
		SET_RESULT(request, ret, OTE_RESULT_ORIGIN_API);
		return;
	}
=======
	mutex_lock(&smc_lock);
>>>>>>> update/master

	/* Open & submit the work to SMC */
	cmd_desc = te_get_free_cmd_desc(&tlk_dev);

	if (!cmd_desc) {
		pr_err("%s: failed to get cmd_desc\n", __func__);
		goto error;
	}

<<<<<<< HEAD
	do_smc_compat(request, context->dev);

	if (request->result) {
		/* release any persistent mem buffers if we failed */
		te_release_persist_mem_buffers(request->session_id, context);
	} else {
		/* mark active any persistent mem buffers */
		te_update_persist_mem_buffers(request->session_id, context);
	}

	te_release_temp_mem_buffers(context);
=======
	request = cmd_desc->req_addr;
	memset(request, 0, sizeof(struct te_request));
	request->type = TE_SMC_OPEN_SESSION;

	if (uuid_size != sizeof(request->dest_uuid)) {
		pr_err("%s: Invalid size sent!\n", __func__);
		goto error;
	}

	if (ta_uuid == NULL || session_id == NULL) {
		pr_err("%s: Null parameters sent!\n", __func__);
		goto error;
	}

	memcpy(request->dest_uuid, ta_uuid, sizeof(*ta_uuid)*4);

	do_smc(request, &tlk_dev);

	if (request->result) {
		pr_err("%s: error opening session: 0x%08x\n",
		__func__, request->result);
		goto error;
	}

	*session_id = request->session_id;
	if (cmd_desc)
		te_put_used_cmd_desc(&tlk_dev, cmd_desc);

	mutex_unlock(&smc_lock);
	return OTE_SUCCESS;

error:
	if (cmd_desc)
		te_put_used_cmd_desc(&tlk_dev, cmd_desc);

	mutex_unlock(&smc_lock);
	return OTE_ERROR_GENERIC;
>>>>>>> update/master
}
EXPORT_SYMBOL(te_open_trusted_session);

/*
 * Command to close session opened with the trusted app.
 * This API should only be called from the kernel space.
 * Takes session Id and UUID of the TA as arguments
 */
void te_close_trusted_session(u32 session_id, u32 *ta_uuid,
				u32 uuid_size)
{
	struct te_request *request;
	struct te_cmd_req_desc *cmd_desc = NULL;

	mutex_lock(&smc_lock);

	/* Open & submit the work to SMC */
	cmd_desc = te_get_free_cmd_desc(&tlk_dev);

	if (!cmd_desc) {
		pr_err("%s: failed to get cmd_desc\n", __func__);
		goto error;
	}

	/* Close the session */
	request = cmd_desc->req_addr;
	memset(request, 0, sizeof(struct te_request));
	request->type = TE_SMC_CLOSE_SESSION;
	request->session_id = session_id;

<<<<<<< HEAD
	do_smc_compat(request, context->dev);
	if (request->result)
		pr_info("%s: error closing session: %08x\n",
			__func__, request->result);

	/* release any peristent mem buffers */
	te_release_persist_mem_buffers(request->session_id, context);
=======
	if (uuid_size != sizeof(request->dest_uuid)) {
		pr_err("%s: Invalid size sent!\n", __func__);
		goto error;
	}

	memcpy(request->dest_uuid, ta_uuid, sizeof(*ta_uuid)*4);

	do_smc(request, &tlk_dev);

	if (request->result) {
		pr_err("%s: error closing session: 0x%08x\n",
		__func__, request->result);
		goto error;
	}

error:
	if (cmd_desc)
		te_put_used_cmd_desc(&tlk_dev, cmd_desc);
	mutex_unlock(&smc_lock);
>>>>>>> update/master
}
EXPORT_SYMBOL(te_close_trusted_session);

/*
 * Command to launch operations from the linux kernel to
 * the trusted app.
 * This API should be called only from the kernel space.
 * The pointer to the buffer from the kernel, length of thus buffer,
 * session Id, UUID of the TA and the command requested from the
 * TA should be passed as arguments
 * Returns SUCCESS or FAILURE
 */
int te_launch_trusted_oper(u8 *buf_ptr, u32 buf_len, u32 session_id,
			u32 *ta_uuid, u32 ta_cmd, u32 uuid_size)
{
	u32 i;
	struct te_request *request;
	struct te_oper_param user_param;
	struct te_oper_param *param_array;
	struct te_oper_param *params = NULL;
	struct te_cmd_req_desc *cmd_desc = NULL;
	u32 no_of_params = 1;

<<<<<<< HEAD
	request->session_id = cmd->session_id;
	request->command_id = cmd->operation.command;
	request->type = TE_SMC_LAUNCH_OPERATION;

	ret = te_prep_mem_buffers_compat(request, context);
	if (ret != OTE_SUCCESS) {
		pr_err("%s: te_prep_mem_buffers failed err (0x%x)\n",
			__func__, ret);
		SET_RESULT(request, ret, OTE_RESULT_ORIGIN_API);
		return;
	}

	do_smc_compat(request, context->dev);

	if (request->result) {
		/* release any persistent mem buffers if we failed */
		te_release_persist_mem_buffers(request->session_id, context);
	} else {
		/* mark active any persistent mem buffers */
		te_update_persist_mem_buffers(request->session_id, context);
	}

	te_release_temp_mem_buffers(context);
}
=======
	mutex_lock(&smc_lock);

	/* Open & submit the work to SMC */
	cmd_desc = te_get_free_cmd_desc(&tlk_dev);
	params = te_get_free_params(&tlk_dev, no_of_params);

	if (!cmd_desc || !params) {
		pr_err("%s: failed to get cmd_desc/params\n", __func__);
		goto error;
	}

	/* launch operation */
	request = cmd_desc->req_addr;
	memset(request, 0, sizeof(struct te_request));
	request->params = (uintptr_t)params;
	request->params_size = no_of_params;
	request->session_id = session_id;
	request->command_id = ta_cmd;
	request->type = TE_SMC_LAUNCH_OPERATION;
	user_param.index = 0;
	user_param.u.Mem.len = buf_len;
	user_param.type = TE_PARAM_TYPE_MEM_RW;
	user_param.u.Mem.type = TE_MEM_TYPE_NS_KERNEL;
	user_param.u.Mem.base = (uint64_t)(uintptr_t)buf_ptr;

	if (uuid_size != sizeof(request->dest_uuid)) {
		pr_err("%s: Invalid size sent\n", __func__);
		goto error;
	}

	memcpy(request->dest_uuid, ta_uuid, sizeof(*ta_uuid)*4);

	param_array = (struct te_oper_param *)(uintptr_t)request->params;

	for (i = 0; i < no_of_params; i++)
		memcpy(param_array + i, &user_param,
		sizeof(struct te_oper_param));

	do_smc(request, &tlk_dev);

	if (request->result) {
		pr_err("%s: error launching session: 0x%08x\n",
		__func__, request->result);
		goto error;
	} else {
		if (cmd_desc)
			te_put_used_cmd_desc(&tlk_dev, cmd_desc);

		if (params)
			te_put_free_params(&tlk_dev, params, no_of_params);

		mutex_unlock(&smc_lock);
		return OTE_SUCCESS;
	}
error:
	if (cmd_desc)
		te_put_used_cmd_desc(&tlk_dev, cmd_desc);

	if (params)
		te_put_free_params(&tlk_dev, params, no_of_params);

	mutex_unlock(&smc_lock);
	return OTE_ERROR_GENERIC;
}
EXPORT_SYMBOL(te_launch_trusted_oper);
>>>>>>> update/master
