/*	$NetBSD: db_trace.c,v 1.7 1997/10/14 10:12:04 mark Exp $	*/

/* 
 * Copyright (c) 1996 Scott K. Stevens
 *
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include <sys/param.h>
#include <sys/proc.h>
#include <machine/db_machdep.h>

#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_output.h>
 
#define INKERNEL(va)	(((vm_offset_t)(va)) >= VM_MIN_KERNEL_ADDRESS)

void
db_stack_trace_cmd(addr, have_addr, count, modif)
	db_expr_t       addr;
	int             have_addr;
	db_expr_t       count;
	char            *modif;
{
	struct frame	*frame, *lastframe;
	boolean_t	kernel_only = TRUE;

	{
		register char c, *cp = modif;
		while ((c = *cp++) != 0)
			if (c == 'u')
				kernel_only = FALSE;
	}

	if (count == -1)
		count = 65535;

	/*
	 * The frame pointer points to the top word of the stack frame so we
	 * need to adjust it by sizeof(struct frame) - sizeof(u_int))
	 * to get the address of the start of the frame structure.
	 */

	if (!have_addr)
		frame = (struct frame *)(DDB_TF->tf_r11
		    - (sizeof(struct frame) - sizeof(u_int)));
	else
		frame = (struct frame *)(addr - (sizeof(struct frame)
		    - sizeof(u_int)));

	lastframe = NULL;

	while (count--) {
		db_expr_t	offset;
		char		*name;
		db_addr_t	pc;

/*		db_printf("fp=%08x: fp=%08x sp=%08x lr=%08x pc=%08x\n",
		    (u_int)frame, frame->fr_fp, frame->fr_sp, frame->fr_lr,
		    frame->fr_pc);*/

		pc = frame->fr_pc;
		/* Adjust the PC so the same address is printed no matter what CPU */
		if (cputype == ID_SA110 || cputype == ID_ARM810)
			pc += 4;
		if (!INKERNEL(pc))
			break;

		db_find_sym_and_offset(pc, &name, &offset);
		if (name == NULL)
			name = "?";

		db_printf("%s(", name);
		db_printsym(pc, DB_STGY_PROC);
		db_printf(")");
		db_printf("\n");

		/*
		 * Switch to next frame up
		 */
		lastframe = frame;
		frame = (struct frame *)(frame->fr_fp - (sizeof(struct frame)
		    - sizeof(u_int)));

		if (frame == NULL)
			break;

		if (INKERNEL((int)frame)) {
			/* staying in kernel */
			if (frame <= lastframe) {
				db_printf("Bad frame pointer: %p\n", frame);
				break;
			}
		} else if (INKERNEL((int)lastframe)) {
			/* switch from user to kernel */
			if (kernel_only)
				break;	/* kernel stack only */
		} else {
			/* in user */
			if (frame <= lastframe) {
				db_printf("Bad user frame pointer: %p\n",
					  frame);
				break;
			}
		}
	}
}
