/*
 * All the contents of this file are overwritten during automated
 * testing. Please consider this before changing anything in this file.
 */

#include <types.h>
#include <lib.h>
#include <clock.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include <kern/test161.h>
#include <spinlock.h>

#define CREATELOOPS	8
#define NTHREADS	32

static volatile unsigned long testval1;
static volatile unsigned long testval2;
static volatile unsigned long testval3;
static volatile int32_t testval4;

static struct semaphore *donesem = NULL;
static struct rwlock *testrwlock = NULL;

struct spinlock status_lock;
static bool test_status = TEST161_FAIL;

static
void
readertestthread(void *junk, unsigned long num)
{
	(void)junk;

	//int i;
	
	kprintf_t(".");
	rwlock_acquire_read(testrwlock);
	kprintf_n("Thread %lu has entered\n", num);
	//random_yielder(4);
		
	testval1 = num;
	testval2 = num*num;
	testval3 = num%3;

	if (testval2 != testval1*testval1) {
		goto fail;
	}
	//random_yielder(4);

	if (testval2%3 != (testval3*testval3)%3) {
		goto fail;
	}
	//random_yielder(4);

	if (testval3 != testval1%3) {
		goto fail;
	}
	//random_yielder(4);

	if (testval1 != num) {
		goto fail;
	}
	//random_yielder(4);
	kprintf_n("Thread %lu has left\n", num);
	rwlock_release_read(testrwlock);
	
	
	V(donesem);
	return;

fail:
	kprintf_n("Thread %lu has left\n", num);
	rwlock_release_read(testrwlock);

}

static
void
writertestthread(void *junk, unsigned long num)
{
	(void)junk;
	
	//int i;

	kprintf_t(".");
	rwlock_acquire_write(testrwlock);
	kprintf_n("Thread %lu has entered\n", num);
	//random_yielder(4);

	testval1 = num;
	testval2 = num*num;
	testval3 = num%3;

	if (testval2 != testval1*testval1) {
		goto fail;
	}
	//random_yielder(4);

	if (testval3 != testval1%3) {
		goto fail;
	}
	//random_yielder(4);
		
	if (testval1 != num) {
		goto fail;
	}
	//random_yielder(4);

	if (testval3 != num%3) {
		goto fail;
	}
	//random_yielder(4);
	kprintf_n("Thread %lu has left\n", num);
	rwlock_release_write(testrwlock);

	V(donesem);
	return;

fail:
	kprintf_n("Thread %lu has left\n", num);
	rwlock_release_write(testrwlock);

}
		
/*
 * Use these stubs to test your reader-writer locks.
 */

int rwtest(int nargs, char **args) {
	(void)nargs;
	(void)args;
	
	int i, result;
	kprintf_n("Starting rwlt1...\n");
	for (i = 0; i<CREATELOOPS; i++) {
		kprintf_t(".");
		testrwlock = rwlock_create("testrwlock");
		if (testrwlock == NULL) {
			panic("rwlt1: rwlock_create failed\n");
		}
		donesem = sem_create("donesem", 0);
		if (donesem == NULL) {
			panic("rwlt1: sem_create failed\n");
		}
		if (i != CREATELOOPS - 1) {
			rwlock_destroy(testrwlock);
			sem_destroy(donesem);
		}
	}
	spinlock_init(&status_lock);
	test_status = TEST161_SUCCESS;
	
	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		if (i == 1) {
			result = thread_fork("rwlt", NULL, writertestthread, NULL, i);
		}
		else {
			result = thread_fork("rwlt", NULL, readertestthread, NULL, i);
		}
		if (result) {
			panic("rwlt1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		P(donesem);
	}
	
	rwlock_destroy(testrwlock);
	sem_destroy(donesem);
	testrwlock = NULL;
	donesem = NULL;
	
	kprintf_t("\n");
	success(test_status, SECRET, "rwlt1");

	return 0;
}

int rwtest2(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt2 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt2");

	return 0;
}

int rwtest3(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt3 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt3");

	return 0;
}

int rwtest4(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt4 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt4");

	return 0;
}

int rwtest5(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt5 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt5");

	return 0;
}
