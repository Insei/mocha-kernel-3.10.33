/*
 * gk20a GPU driver
 *
 * Copyright (c) 2014, NVIDIA Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GK20A_H
#define __GK20A_H

#include <linux/errno.h>

struct channel_gk20a;
struct platform_device;

#ifdef CONFIG_GK20A
<<<<<<< HEAD
void gk20a_channel_update(struct channel_gk20a *c, int nr_completed);
int nvhost_vpr_info_fetch(void);
void gk20a_debug_dump_device(struct platform_device *pdev);
int gk20a_do_idle(void);
int gk20a_do_unidle(void);
#else
static inline void gk20a_channel_update(struct channel_gk20a *c,
					int nr_completed) {}
static inline void gk20a_debug_dump_device(struct platform_device *pdev) {}
=======
int nvhost_vpr_info_fetch(void);
int gk20a_do_idle(void);
int gk20a_do_unidle(void);
#else
>>>>>>> update/master
static inline int nvhost_vpr_info_fetch(void)
{
	return -ENOSYS;
}
<<<<<<< HEAD
int gk20a_do_idle(void) { return -ENOSYS; }
int gk20a_do_unidle(void) { return -ENOSYS; }
=======
static inline int gk20a_do_idle(void) { return -ENOSYS; }
static inline int gk20a_do_unidle(void) { return -ENOSYS; }
>>>>>>> update/master
#endif
#endif
