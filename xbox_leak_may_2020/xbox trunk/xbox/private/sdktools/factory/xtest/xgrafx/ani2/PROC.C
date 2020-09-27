/*************************************************************************
*                                                                        *
* 	Routines for process creation and control			  *
*                                                                        *
*************************************************************************/

#include "error.h"
#include "sos.h"
#include "protos.h"
#include "externs.h"

extern	int	(*event_fcns[])();

/*
 *	return a new process or NULL if error
 *	Put the process in the process queue
 */

struct process *pp;

int create_process(
ushort	*mem_ptr,			/* pointer to event list */
uchar	level,				/* sound level */
uchar	channel)			/* channel for this process */
{
 	struct process	*tpp;		

	pp = get_process_packet();

	pp->mem_ptr.i = mem_ptr;		/* get event-list pointer */
	pp->level = level;		/* get level		  */
	pp->timer = 0;		/* timer for process 	  */
	pp->hard_channel = channel;	/* get hardware channel   */
	pp->function = *mem_ptr;	/* get function byte	  */
	pp->type = 0;				/* clear type field			*/
	pp->prev = process_queue;	/* put process at head of q */

	if (level == 0)
		pp->prev_timer = system_clock_music;  /* make us current	  */
	else
		pp->prev_timer = system_clock_fx;  /* make us current	  */

	pp->next = tpp = process_queue->next;
	if (tpp != (struct process *)NULL) {
	    tpp->prev = pp;
	}
	process_queue->next = pp;
	return(1);
}

/*
 * 	remove a process from the process queue and free up the memory
 */

void kill_process(struct process *pp)
{
	pp->prev->next = pp->next;
	if (pp->next != (struct process *)NULL)
	   pp->next->prev = pp->prev;		/* remove ourselves */

	pp->next = queue_list[0].next;
	pp->prev = &(queue_list[0]);
/*	pp->function = 0;*/
	queue_list[0].next = pp;
	if (pp->next == (struct process *)NULL)
	    return;
	pp->next->prev = pp;
}

struct process *get_process_packet()
{
	struct process * pp;

	pp = queue_list[0].next;
	queue_list[0].next = pp->next;
	if (pp->next == (struct process *)NULL)
	    return(pp);
	pp->next->prev = &(queue_list[0]);
	return(pp);
}

