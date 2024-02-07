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
	new->frames = 0;
	new->frames_count = 0;
	return new;
}

void init_vmpool(uint32_t start_addr, uint32_t end_addr)
{
	vm_struct* new = get_newvms(start_addr, end_addr - start_addr);
	vmpool = new;
}

static void insrt_vms_list(vm_struct *vms) //
{
}

static void insrt_vms_pool(vm_struct *vms) //insert vms on avaliable vms
{
	//find pos and merge if address space contiguous
}

static void rmv_vms_list(vm_struct *vms)
{
}

static void rmv_vms_pool(vm_struct *vms)
{
}

static void alloc_uncont(vm_struct* vms, unsigned int size)
{
	unsigned long frames_count = size / 0x1000;
	vms->frames_count = frames_count;
	vms->frames = kmalloc(sizeof(uint32_t) * frames_count);
	if (!vms->frames)
		panic_1("out of phys space");
	for (int i = 0; i < frames_count; ++i)
	{
		vms->
	}
}

static void linkaddr(vm_struct* vms)
{
}

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
			rmv_vms_pool(vms);
			insrt_vms_list(vms);
		}
		else if (vms->length > size)
		{
			vm_struct *newvms = get_newvms(vms->addr + size, vms->length - size);
			if (newvms->length < 0x1000)
				panic_1("virtual address space corrupted!"); //sanity check
			vms->length = size; //resize current vms
			rmv_vms_pool(vms);
			insrt_vms_list(vms);
			insrt_vms_pool(newvms);
		}
		else
			panic_1("out of vm space!");
	}
	alloc_uncont(vms, size);	//allocated (un)contiguous memory
	linkaddr(vms);				//link virt addr and phys addr on pgt
	return addr;
}

void vfree(void *addr)
{
}
