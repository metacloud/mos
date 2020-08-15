#include "poll.h"

#include <kernel/memory/vmm.h>
#include <kernel/proc/task.h>

void poll_table_free(struct poll_table *pt)
{
	struct poll_table_entry *iter, *next;

	list_for_each_entry_safe(iter, next, &pt->list, sibling)
	{
		list_del(&iter->wait.sibling);
		list_del(&iter->sibling);
		kfree(iter);
	}
	kfree(pt);
}

void poll_wakeup(struct thread *t)
{
	t->state = THREAD_READY;
}

void poll_wait(struct vfs_file *file, struct wait_queue_head *wh, struct poll_table *pt)
{
	struct poll_table_entry *pe = kcalloc(sizeof(struct poll_table_entry), 1);
	pe->file = file;
	pe->wait.func = poll_wakeup;
	pe->wait.thread = current_thread;
	list_add_tail(&pe->wait.sibling, &wh->list);
	list_add_tail(&pe->sibling, &pt->list);
}

int do_poll(struct pollfd *fds, uint32_t nfds)
{
	while (true)
	{
		struct poll_table *pt = kcalloc(sizeof(struct poll_table), 1);
		bool is_ready = false;
		for (uint32_t i = 0; i < nfds; ++i)
		{
			struct pollfd *pfd = &fds[i];
			struct vfs_file *f = current_thread->parent->files->fd[pfd->fd];
			pfd->revents = f->f_op->poll(f, pt);
			if (pfd->events & pfd->revents)
				is_ready = true;
		}
		if (is_ready)
			break;
		update_thread(current_thread, THREAD_WAITING);
		schedule();
		poll_table_free(pt);
	}
	return 0;
}
