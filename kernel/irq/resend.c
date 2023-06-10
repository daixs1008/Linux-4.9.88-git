/*
 * linux/kernel/irq/resend.c
 *
 * Copyright (C) 1992, 1998-2006 Linus Torvalds, Ingo Molnar
 * Copyright (C) 2005-2006, Thomas Gleixner
 *
 * This file contains the IRQ-resend code
 *
 * If the interrupt is waiting to be processed, we try to re-run it.
 * We can't directly run it from here since the caller might be in an
 * interrupt-protected region. Not all irq controller chips can
 * retrigger interrupts at the hardware level, so in those cases
 * we allow the resending of IRQs via a tasklet.
 */

#include <linux/irq.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/interrupt.h>

#include "internals.h"

#ifdef CONFIG_HARDIRQS_SW_RESEND

/* Bitmap to handle software resend of interrupts: */
static DECLARE_BITMAP(irqs_resend, IRQ_BITMAP_BITS);

/*
 * Run software resends of IRQ's
 */
static void resend_irqs(unsigned long arg)
{
	struct irq_desc *desc;
	int irq;

	while (!bitmap_empty(irqs_resend, nr_irqs)) {
		irq = find_first_bit(irqs_resend, nr_irqs);
		clear_bit(irq, irqs_resend);
		desc = irq_to_desc(irq);
		local_irq_disable();
		desc->handle_irq(desc);
		local_irq_enable();
	}
}

/* Tasklet to handle resend: */
static DECLARE_TASKLET(resend_tasklet, resend_irqs, 0);

#endif

/*
 * IRQ resend
 *
 * Is called with interrupts disabled and desc->lock held.
 */
void check_irq_resend(struct irq_desc *desc)
{
	/*
	 * We do not resend level type interrupts. Level type
	 * interrupts are resent by hardware when they are still
	 * active. Clear the pending bit so suspend/resume does not
	 * get confused.
	 */
	if (irq_settings_is_level(desc)) {  //电平中断不存在resend的问题
		desc->istate &= ~IRQS_PENDING;
		return;
	}
	if (desc->istate & IRQS_REPLAY)   //如果已经设定resend的flag，退出就OK了，这个应该和irq的enable disable能多层嵌套相关
		return;
	if (desc->istate & IRQS_PENDING) {  //如果有pending的flag则进行处理
		desc->istate &= ~IRQS_PENDING;
		desc->istate |= IRQS_REPLAY;   //设置retrigger标志

		if (!desc->irq_data.chip->irq_retrigger ||
		    !desc->irq_data.chip->irq_retrigger(&desc->irq_data)) {   //调用底层irq chip的callback
#ifdef CONFIG_HARDIRQS_SW_RESEND                             //也可以使用软件手段来完成resend一个中断，
			unsigned int irq = irq_desc_get_irq(desc);

			/*
			 * If the interrupt is running in the thread
			 * context of the parent irq we need to be
			 * careful, because we cannot trigger it
			 * directly.
			 */
			if (irq_settings_is_nested_thread(desc)) {
				/*
				 * If the parent_irq is valid, we
				 * retrigger the parent, otherwise we
				 * do nothing.
				 */
				if (!desc->parent_irq)
					return;
				irq = desc->parent_irq;
			}
			/* Set it pending and activate the softirq: */
			set_bit(irq, irqs_resend);
			tasklet_schedule(&resend_tasklet);
#endif
		}
	}
}
