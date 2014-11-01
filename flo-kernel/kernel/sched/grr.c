#include "sched.h"
#include <linux/slab.h>

/*
 * Grouper Round-Robin scheduling class.
 */

#ifdef CONFIG_SMP

void trigger_load_balance_grr(struct rq *rq, int cpu) {

	trace_printk("Calling load balance now.\n");
	return;
}

static int
find_min_rq_cpu(void)
{
	int cpu, min_cpu, min_running = 0, first = 1;
	struct rq *rq;

	for_each_possible_cpu(cpu) {
		rq = cpu_rq(cpu);
		
		if(first) {
			min_running = rq->grr.nr_running;
			min_cpu = cpu;
			first = 0;
			continue;
		}
		
		if (min_running > rq->grr.nr_running) {
			min_running = rq->grr.nr_running;
			min_cpu = cpu;

		}

	}

	return min_cpu;

}




static int
select_task_rq_grr(struct task_struct *p, int sd_flag, int flags)
{

	int min_cpu = 0;

	min_cpu = find_min_rq_cpu();
	struct rq *rq = cpu_rq(min_cpu);
	trace_printk("Selected CPU: %d\tNR: %d\n", min_cpu, rq->grr.nr_running);

	return min_cpu;
}
#endif /* CONFIG_SMP */
/*
 * Idle tasks are unconditionally rescheduled:
 */
static void check_preempt_curr_grr(struct rq *rq,
		struct task_struct *p, int flags)
{
	//printk(KERN_WARNING "NICOLAS: Check preempt curr\n");
}

static struct task_struct *pick_next_task_grr(struct rq *rq)
{
	struct grr_rq *grr_rq = &rq->grr;
	struct  sched_grr_entity *entity;
	struct task_struct *p;

	/* No tasks in queue */
	if (list_empty(&grr_rq->queue))
		return NULL;

	entity = list_first_entry(&grr_rq->queue,
		struct sched_grr_entity, list);

	p = container_of(entity, struct task_struct, grr);

	//printk(KERN_WARNING "pick next: %d, %d\n", p->pid, grr_rq->nr_running);

	return p;
}

static void
dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	struct grr_rq *grr_rq = &rq->grr;
	struct  sched_grr_entity *entity = &p->grr;

	if (grr_rq->nr_running) {
		grr_rq->nr_running--;
		list_del(&entity->list);
	}

	//printk(KERN_WARNING "NICOLAS: Dequeue task called\n");
}

static void
yield_task_grr(struct rq *rq)
{
	//printk(KERN_WARNING "NICOLAS: Called yield task\n");
}

static void
enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	struct grr_rq *grr_rq = &rq->grr;
	struct sched_grr_entity *entity = &p->grr;

	grr_rq->nr_running++;

	//list_add_tail(&entity->list, &grr_rq->queue);
	list_add(&entity->list, &grr_rq->queue);

	//printk(KERN_WARNING "NR_RUNNING: %d\n", grr_rq->nr_running);
	//printk(KERN_WARNING "SCHED: %d\n", p->policy);
}



static void put_prev_task_grr(struct rq *rq, struct task_struct *p)
{
	//printk(KERN_WARNING "NICOLAS: Put prev task called\n");
}

static void task_tick_grr(struct rq *rq, struct task_struct *curr, int queued)
{
	struct sched_grr_entity *entity = &curr->grr;

	if ((--(entity->time_slice)) > 0)
		return;

	entity->time_slice = GRR_TIMESLICE;

	if (rq->grr.nr_running > 1) {
		//printk(KERN_WARNING "Entered\n");
		list_move(&rq->grr.queue, &entity->list);
		set_tsk_need_resched(curr);
	}
}

static void set_curr_task_grr(struct rq *rq)
{
	struct task_struct *p = rq->curr;

	//printk(KERN_WARNING "Set curr task called: \n");
	p->se.exec_start = rq->clock_task;
	p->grr.time_slice = GRR_TIMESLICE;
}

static void switched_to_grr(struct rq *rq, struct task_struct *p)
{
	//printk(KERN_WARNING "NICOLAS: Switched to grr\n");
}

static void
prio_changed_grr(struct rq *rq, struct task_struct *p, int oldprio)
{
	//printk(KERN_WARNING "NICOLAS: Prio changed grr\n");
}

static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *task)
{
	return 0;
}

void init_grr_rq(struct grr_rq *grr_rq, struct rq *rq)
{
	grr_rq->nr_running = 0;

	INIT_LIST_HEAD(&grr_rq->queue);
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
