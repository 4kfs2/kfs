#include "../includes/mm.h"
#include "../includes/panic.h"
#include <sys/wait.h>

vm_struct* vmlist; //allocated vm space
vm_struct* vmpool; //available vm space

static vm_struct*	get_newvms(uint32_t addr, unsigned long length)
{
	vm_struct *new = kmalloc(sizeof(vm_struct));
	if (!new)
		panic_1("out of phys space!");
	new->addr = addr;
	new->length = length;
	new->next = 0;
	// new->frames = 0;
	// new->frames_count = 0;
	return new;
}

void init_vmpool(uint32_t start_addr, uint32_t end_addr)
{
	vm_struct* new = get_newvms(start_addr, end_addr - start_addr);
	vmpool = new;
}

static void insrt_vms_list(vm_struct *vms) //insert in ascending order of addr
{
	vm_struct* tmp = 0;
	vm_struct* prev = 0;

	for (tmp = vmlist; tmp; vmlist = vmlist->next)
	{
		if (tmp->addr > vms->addr)
		{
			vms->next = tmp;
			if (prev) prev->next = vms;
			return ;
		}
		prev = tmp;
	}
	if (!vmlist) vmlist = vms; else prev->next = vms;
}

static void insrt_vms_pool(vm_struct *vms) //insert vms on avaliable vms
{
	vm_struct* tmp = 0;
	vm_struct* prev = 0;

	//find pos and merge if address space contiguous
	for (tmp = vmpool; tmp; tmp = tmp->next)
	{
		if (tmp->addr > vms->addr)
		{
			vms->next = tmp;
			if(prev) prev->next = vms;
			if ((vms->addr + vms->addr) == tmp->addr) //merge into right node
			{
				vms->length += tmp->length;
				vms->next = tmp->next;
				kfree(tmp);
			}
			if (prev && (prev->addr + prev->length) == vms->addr) //merge into left node
			{
				prev->length += vms->length;
				prev->next = vms->next;
				kfree(vms);
			}
			return ;
		}
		prev = tmp;
	}
	if (!vmpool) vmpool = vms; else prev->next = vms;
}

static void rmv_vms(vm_struct* src, vm_struct *vms)
{
	vm_struct* tmp = 0;
	vm_struct* prev = 0;

	for (tmp = src; tmp; tmp = tmp->next)
	{
		if (tmp == vms)
		{
			if (prev)
				prev->next = tmp->next;
			else
				src = tmp->next;
			tmp->next = 0;
		}
		prev = tmp;
	}
}

static void alloc_uncont(vm_struct* vms, unsigned int size)
{
	// unsigned long frames_count = size / 0x1000;
	// vms->frames_count = frames_count;
	// vms->frames = kmalloc(sizeof(uint32_t) * frames_count);
	// if (!vms->frames)
	// 	panic_1("out of phys space");
	// for (int i = 0; i < frames_count; ++i)
	// {
	// 	vms->frames[i] = alloc_frame(get_page());
	// }
	uint32_t addr = vms->addr;
	while (size /= 0x1000)
	{
		alloc_frame(get_page(addr));
		addr += 0x1000;
	}
}

// static void linkaddr(vm_struct* vms)
// {
// }

void *vmalloc(unsigned long size)
{
	void *addr;

	if (size & 0x00000FFF) //page align
	{
		size &= 0xFFFFF000;
		size += 0x1000;
	}
	vm_struct* vms;
	for (vms = vmpool; vms; vms = vms->next) //search through the pool
	{
		if (vms->length == size)
		{
			rmv_vms(vmpool, vms);
			insrt_vms_list(vms);
		}
		else if (vms->length > size)
		{
			vm_struct *newvms = get_newvms(vms->addr + size, vms->length - size);
			if (newvms->length < 0x1000)
				panic_1("virtual address space corrupted!"); //sanity check
			vms->length = size; //resize current vms
			rmv_vms(vmpool, vms);
			insrt_vms_list(vms);
			insrt_vms_pool(newvms);
		}
		else
			panic_1("out of vm space!");
	}
	alloc_uncont(vms, size);	//allocated (un)contiguous memory
	//linkaddr(vms);				//link virt addr and phys addr on pgt
	return addr;
}

void vfree(void *addr)
{
}
