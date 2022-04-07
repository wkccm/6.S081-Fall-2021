// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
}kmem;

struct memnode{
  struct spinlock lock;
  struct run *freelist;
};

struct memnode cpu_mem[NCPU];

void
kinit()
{
  for(int i=0;i<NCPU;i++){
    initlock(&cpu_mem[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  push_off();
  int this_cpu=cpuid();
  pop_off();
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&cpu_mem[this_cpu].lock);
  r->next = cpu_mem[this_cpu].freelist;
  cpu_mem[this_cpu].freelist = r;
  release(&cpu_mem[this_cpu].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int this_cpu=cpuid();
  pop_off();

  acquire(&cpu_mem[this_cpu].lock);
  r = cpu_mem[this_cpu].freelist;
  if(r){
    cpu_mem[this_cpu].freelist = r->next;
    release(&cpu_mem[this_cpu].lock);
  }
  else{
    int j;
    int free_cpu;
    for (j = 0; j < NCPU; j++)
    {
      free_cpu=(this_cpu+j)%NCPU;
      if(cpu_mem[free_cpu].freelist!=0){
        break;
      }
    }
    release(&cpu_mem[this_cpu].lock);
    acquire(&cpu_mem[free_cpu].lock);
    r=cpu_mem[free_cpu].freelist;
    if(r){
      cpu_mem[free_cpu].freelist=r->next;
    }
    release(&cpu_mem[free_cpu].lock);
  }
  

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
