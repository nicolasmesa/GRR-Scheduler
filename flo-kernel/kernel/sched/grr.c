#include "sched.h"
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/string.h>


#ifdef CONFIG_CGROUP_SCHED
#define PATH_MAX 4096

static char group_path[PATH_MAX];

static char *task_group_path(struct task_group *tg)
{
        if (autogroup_path(tg, group_path, PATH_MAX))
                return group_path;

        /*
         * May be NULL if the underlying cgroup isn't fully-created yet
         */
        if (!tg->css.cgroup) {
                group_path[0] = '\0';
                return group_path;
        }
        cgroup_path(tg->css.cgroup, group_path, PATH_MAX);
        return group_path;
}


static int get_task_group(struct task_struct *p)
{
	char *path;
	int ret = 0;

	path = task_group_path(task_group(p));

	if (strcmp(path, "/") == 0 || strcmp(path, "/apps") == 0)
		ret = 1;
	else if (strcmp(path, "/apps/bg_non_interactive") == 0)
		ret = 2;

	return ret;
}
#endif


/*
 * Grouper Round-Robin scheduling class.
 */

#ifdef CONFIG_SMP

void trigger_load_balance_grr(struct rq *rq, int cpu) 
{
	struct grr_rq *grr_rq = &rq->grr;

	grr_rq->tick_count++;

	if (grr_rq->tick_count >= grr_rq->load_balance_thresh) {
		grr_rq->load_balance_thresh += GRR_LB_THRESH;
		raise_softirq(SCHED_SOFTIRQ_GRR);
	}
}

static int
find_min_rq_cpu(struct task_struct *p)
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

	int min_cpu;
	struct rq *rq;

	min_cpu = find_min_rq_cpu(p);
	rq = cpu_rq(min_cpu);
	//trace_printk("Selected CPU: %d\tNR: %d\n", min_cpu, rq->grr.nr_running);

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
	int id;

	grr_rq->nr_running++;

	id = get_task_group(p);

	//list_add_tail(&entity->list, &grr_rq->queue);
	list_add(&entity->list, &grr_rq->queue);

	//printk(KERN_WARNING "NR_RUNNING: %d\n", grr_rq->nr_running);
	//trace_printk("SCHED: %d\n", p->policy);
}



static void put_prev_task_grr(struct rq *rq, struct task_struct *p)
{
	//printk(KERN_WARNING "NICOLAS: Put prev task called\n");
}

static void task_tick_grr(struct rq *rq, struct task_struct *curr, int queued)
{
	struct sched_grr_entity *entity = &curr->grr;
	int i;

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

static struct sched_grr_entity *get_next_elegible_entity(struct rq *rq, int dst_cpu) {
	struct grr_rq *grr_rq = &rq->grr;
	int num = 1;
	struct sched_grr_entity *entity;
	struct task_struct *p;

	list_for_each_entry(entity, &grr_rq->queue, list) {
		if (num >= 2) {
			p = container_of(entity, struct task_struct, grr);
			if (cpumask_test_cpu(dst_cpu, tsk_cpus_allowed(p)) && 
					!task_running(rq, p))
				return entity;
		}

		num++;
	}

	return NULL;
}


static void run_rebalance_domains_grr(struct softirq_action *h)
{
	int cpu, min_running = 0, max_running = 0, first = 1;
	struct rq *min_rq, *max_rq;
	struct grr_rq *grr_rq;
	struct sched_grr_entity *entity;
	struct task_struct *p;
	struct rq *rq;

	for_each_possible_cpu(cpu) {
		rq = cpu_rq(cpu);

		raw_spin_lock(&rq->lock);

		grr_rq = &rq->grr;

		if (first) {
			min_rq = max_rq = rq;
			min_running = max_running = grr_rq->nr_running;
			first = 0;

			raw_spin_unlock(&rq->lock);
			continue;
		}

		if (grr_rq->nr_running < min_running) {
			min_running = grr_rq->nr_running;
			min_rq = rq;
		}

		if (grr_rq->nr_running > max_running) {
			max_running = grr_rq->nr_running;
			max_rq = rq;
		}

		raw_spin_unlock(&rq->lock);
	}

	if (max_running - min_running >= 2) {
		raw_spin_lock_irq(&max_rq->lock);
		double_lock_balance(max_rq, min_rq);

		min_running = min_rq->grr.nr_running;
		max_running = max_rq->grr.nr_running;

		/* Maybe this changed while grabbing the locks */
		if (max_running - min_running < 2) {
			double_unlock_balance(max_rq, min_rq);
			raw_spin_unlock_irq(&max_rq->lock);
			return;
		}

		grr_rq = &max_rq->grr;

		entity = get_next_elegible_entity(max_rq, cpu_of(min_rq));

		if (entity != NULL) {
			p = container_of(entity, struct task_struct, grr);

			deactivate_task(max_rq, p, 0);
			set_task_cpu(p, cpu_of(min_rq));
			activate_task(min_rq, p, 0);
			check_preempt_curr(min_rq, p, 0);
		}

		double_unlock_balance(max_rq, min_rq);
		raw_spin_unlock_irq(&max_rq->lock);
	}
}

void init_grr_rq(struct grr_rq *grr_rq, struct rq *rq, int cpu)
{
#ifdef CONFIG_SMP
	if (cpu < nr_cpu_ids / 2) 
		grr_rq->group = 1;
	else
		grr_rq->group = 2;

	open_softirq(SCHED_SOFTIRQ_GRR, run_rebalance_domains_grr);
#endif

	grr_rq->nr_running = 0;

	grr_rq->tick_count = 0;
	grr_rq->load_balance_thresh = GRR_LB_THRESH;

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
