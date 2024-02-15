#include "../includes/mm.h"
#include "../includes/panic.h"

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
	return new;
}

static void insert_vms_list(vm_struct *vms) //insert in ascending order of addr
{
	vm_struct* tmp = 0;
	vm_struct* prev = 0;

	for (tmp = vmlist; tmp; tmp = tmp->next)
	{
		if (tmp->addr > vms->addr)
		{
			vms->next = tmp;
			if (prev)
				prev->next = vms;
			else
				vmlist = vms;
			return ;
		}
		prev = tmp;
	}
	if (!vmlist)
		vmlist = vms;
	else
		prev->next = vms;
}

//insert vms on avaliable vms
static void insert_vms_pool(vm_struct *vms)
{
	vm_struct* tmp = 0;
	vm_struct* prev = 0;

	//find pos and merge if address space contiguous
	for (tmp = vmpool; tmp; tmp = tmp->next)
	{
		if (tmp->addr > vms->addr)
		{
			//link the nodes
			vms->next = tmp;
			if (prev)
				prev->next = vms;
			//merge into right node
			if ((vms->addr + vms->length) == tmp->addr)
			{
				vms->length += tmp->length;
				vms->next = tmp->next;
				kfree(tmp);
			}
			//merge into left node
			if (prev && (prev->addr + prev->length) == vms->addr)
			{
				prev->length += vms->length;
				prev->next = vms->next;
				kfree(vms);
			}
			else
				vmpool = vms;
			return ;
		}
		prev = tmp;
	}
	if (!vmpool)
		vmpool = vms;
	else
		prev->next = vms;
}

static void remove_vms_list(vm_struct *vms)
{
	vm_struct* tmp = 0;
	vm_struct* prev = 0;

	for (tmp = vmlist; tmp; tmp = tmp->next)
	{
		if (tmp == vms)
		{
			if (prev)
				prev->next = tmp->next;
			else
				vmlist = tmp->next;
			tmp->next = 0;
			return ;
		}
		prev = tmp;
	}
}

static void remove_vms_pool(vm_struct *vms)
{
	vm_struct* tmp = 0;
	vm_struct* prev = 0;

	for (tmp = vmpool; tmp; tmp = tmp->next)
	{
		if (tmp == vms)
		{
			if (prev)
				prev->next = tmp->next;
			else
				vmpool = tmp->next;
			tmp->next = 0;
			return ;
		}
		prev = tmp;
	}
}

static void linkphys(vm_struct* vms, unsigned int size)
{
	uint32_t addr = vms->addr;
	while (size)
	{
		alloc_frame(get_page(addr));
		addr += PAGE_SIZE;
		size -= PAGE_SIZE;
	}
}

static void  unlinkphys(vm_struct *vms, unsigned int size)
{
	uint32_t addr = vms->addr;
	while (size)
	{
		free_frame(get_page(addr));
		addr += PAGE_SIZE;
		size -= PAGE_SIZE;
	}
}

void init_vmpool(uint32_t start_addr, uint32_t end_addr)
{
	vm_struct* new = get_newvms(start_addr, end_addr - start_addr);
	vmpool = new;
}

void *vmalloc(unsigned long size)
{
	vm_struct* vms;

	ALIGN(size);
	//search through the pool
	for (vms = vmpool; vms; vms = vms->next)
	{
		if (vms->length == size)
		{
			remove_vms_pool(vms);
			insert_vms_list(vms);
			break;
		}
		else if (vms->length > size)
		{
			vm_struct *newvms = get_newvms(vms->addr + size, vms->length - size);
			if (newvms->length < PAGE_SIZE)
				panic_1("virtual address space corrupted!"); //sanity check
			remove_vms_pool(vms);
			vms->length = size; //resize current vms
			insert_vms_list(vms);
			insert_vms_pool(newvms);
			break;
		}
	}
	if (!vms)
		panic_1("out of vm space!");
	//find pages and mark the frames as occupied
	linkphys(vms, size);
	return (void *)(vms->addr);
}

void vfree(void *addr)
{
	struct vm_struct *vms = 0;
	struct vm_struct *prev= 0;

	//search through the list
	for (vms = vmlist; vms; vms = vms->next)
	{
		if (vms->addr == addr)
		{
			//find pages and mark the frames as available
			unlinkphys(vms, vms->length);
			remove_vms_list(vms);
			insert_vms_pool(vms);
			break;
		}
		prev = vms;
	}
	if (!vms)
		panic_1("pointer being freed was not allocated!");
	
}
