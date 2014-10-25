#include "sched.h"

/*
 * Grouper Round-Robin scheduling class.
 */

#ifdef CONFIG_SMP
static int
select_task_rq_grr(struct task_struct *p, int sd_flag, int flags)
{
	printk(KERN_WARNING "Select task rq called\n");
	return task_cpu(p); /* IDLE tasks as never migrated */
}
#endif /* CONFIG_SMP */
/*
 * Idle tasks are unconditionally rescheduled:
 */
static void check_preempt_curr_grr(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_WARNING "NICOLAS: Check preempt curr\n");
	resched_task(rq->idle);
}

static struct task_struct *pick_next_task_grr(struct rq *rq)
{
	//printk(KERN_WARNING "NICOLAS: Pick next task called\n");
	return NULL;
}

static void
dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	//raw_spin_unlock_irq(&rq->lock);
	//printk(KERN_ERR "bad: scheduling from the idle thread!\n");
	//dump_stack();
	//raw_spin_lock_irq(&rq->lock);
	printk(KERN_WARNING "NICOLAS: Dequeue task called\n");
}

static void
yield_task_grr(struct rq *rq)
{
	printk(KERN_WARNING "NICOLAS: Called yield task\n");
}

static void
enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_WARNING "NICOLAS: Enqueue task called\n");
}



static void put_prev_task_grr(struct rq *rq, struct task_struct *p)
{
	printk(KERN_WARNING "NICOLAS: Put prev task called\n");
}

static void task_tick_grr(struct rq *rq, struct task_struct *curr, int queued)
{
	printk(KERN_WARNING "NICOLAS: Task tick called\n");
}

static void set_curr_task_grr(struct rq *rq)
{
	printk(KERN_WARNING "Set curr task called");
}

static void switched_to_grr(struct rq *rq, struct task_struct *p)
{
	printk(KERN_WARNING "NICOLAS: Switched to grr\n");
}

static void
prio_changed_grr(struct rq *rq, struct task_struct *p, int oldprio)
{
	printk(KERN_WARNING "NICOLAS: Prio changed grr\n");
}

static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *task)
{
	return 0;
}

/*
 * Class for Grouped Round Robin Scheduler
 */
const struct sched_class sched_grr_class = {
	.next = &fair_sched_class,

	.enqueue_task		= enqueue_task_grr,

	.dequeue_task		= dequeue_task_grr,

	.yield_task		= yield_task_grr,

	.check_preempt_curr	= check_preempt_curr_grr,

	.pick_next_task		= pick_next_task_grr,
	.put_prev_task		= put_prev_task_grr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_grr,
#endif

	.set_curr_task          = set_curr_task_grr,
	.task_tick		= task_tick_grr,

	.get_rr_interval	= get_rr_interval_grr,

	.prio_changed		= prio_changed_grr,
	.switched_to		= switched_to_grr,
};
