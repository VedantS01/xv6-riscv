#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_exit)) {
    printf("%d: syscall exit: \n:arg(s): %d\n", myproc()->pid, n);
  }
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_getpid)) {
    printf("%d: syscall getpid: \n:arg(s): \n", myproc()->pid);
  }
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_fork)) {
    printf("%d: syscall fork: \n:arg(s): \n", myproc()->pid);
  }
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_wait)) {
    printf("%d: syscall wait: \n:arg(s): %d\n", myproc()->pid, p);
  }
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_sbrk)) {
    printf("%d: syscall sbrk: \n:arg(s): %d\n", myproc()->pid, n);
  }
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_sleep)) {
    printf("%d: syscall sleep: \n:arg(s): %d\n", myproc()->pid, n);
  }
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_kill)) {
    printf("%d: syscall kill: \n:arg(s): \n", myproc()->pid);
  }
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_uptime)) {
    printf("%d: syscall uptime: \n:arg(s): \n", myproc()->pid);
  }
  return xticks;
}


uint64
sys_trace(void)
{
  struct proc* p = myproc();
  int mask;

  if(argint(0, &mask) < 0)
    return -1;

  p->mask = mask;
  p->traced = 1;
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_trace)) {
    printf("%d: syscall trace: \n:arg(s): %d\n", myproc()->pid, mask);
  }
  return 0;
}

uint64
sys_trace_x(void)
{
  struct proc* p = myproc();
  int mask;

  if(argint(0, &mask) < 0)
    return -1;

  p->mask = mask;
  p->traced = 2;
  if(myproc()->traced == 2 && myproc()->mask & (1 << SYS_trace_x)) {
    printf("%d: syscall trace_x: \n:arg(s): %d\n", myproc()->pid, mask);
  }
  return 0;
}