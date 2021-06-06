#ifndef _FILE_SYSCALL_H_
#define _FILE_SYSCALL_H_

struct f_handle {
	char *f_name;
	int offset;
	int flag;
	int ref_count;
	struct lock *f_lock;
	struct vnode *f_vnode;
};

void initialize_console(char *fname, int flag, int mode, int fd);

//struct f_handle *fhandle_create();

void fhandle_destroy();

ssize_t write(int fd, const void *buf, size_t buflen);

#endif /* _FILE_SYSCALL_H_ */

