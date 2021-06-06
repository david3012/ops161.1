#include <kern/unistd.h>
#include <file_syscall.h>

struct f_handle * 
fhandle_create(char *name, int flags, int offset, struct vnode *vn) {
	struct f_handle *fdesc;
	fdesc = kmalloc(sizeof(*f_handle));
	if (fdesc == NULL) {
		return NULL;
	}
	
	fdesc->f_name = kstrdup(name);
	if (fdesc->f_name == NULL) {
		kfree(fdesc);
	}

	fdesc->f_lock = lock_create(fdesc->f_name);
	if (fdesc->f_lock == NULL) {
		kfree(fdesc->f_name);
		kfree(fdesc);
		return NULL;
	}

	fdesc->offset = offset;
	fdesc->flags = flags
	fdesc->ref_count = 1;
	fdesc->f_vnode = vn;
	
	return fdesc;
}

void 
fhandle_destroy(struct f_handle* fdesc) {
	KASSERT(fdesc != NULL);
	fdesc->f_vnode = NULL;
	lock_destroy(fdesc->f_lock);
	kfree(fdesc->f_name);	
	kfree(fdesc); 
} 

void 
initialize_console(char *fname, int flag, int mode, int fd) {
	struct vnode *vn;
	int err; 
	char *fname_temp;
	fname_temp = kstrdup(fname);

	switch(fd) {
		case STDIN_FILENO:
			err = vfs_open(fname_temp, flag, mode, &vn);
			if (err) {
				kprintf("Could not open %s for read: %s\n", fname_temp, strerror(err));
			}
			curproc->f_table[STDOUT_FILENO] = create_fhandle(fname, O_RDONLY, 0664, STDIN_FILENO, vn);
			break;
		case STDOUT_FILENO:
			err = vfs_open(fname_temp, flag, mode, &vn);
			if (err) {
				kprintf("Could not open %s for write: %s\n", fname_temp, strerror(err));
}
			curproc->f_table[STDOUT_FILENO] = create_fhandle(fname, O_WRONLY, 0664, STDOUT_FILENO, vn);
			break;
		case STDERR_FILENO:
			err = vfs_open(fname_temp, flag, mode, &vn);
			if (err) {
				kprintf("Could not open %s for write: %s\n", fname_temp, strerror(err));
}
			curproc->f_table[STDERR_FILENO] = create_fhandle(fname, O_WRONLY, 0664, STDERR_FILENO, vn);
			break;
		
	}
	kfree(fname_temp);
}

ssize_t
write(int fd, const void *buf, size_t buflen) {
	struct iovec iov;
	struct uio u;
	int result;
	
	struct f_handle *fdesc;
	fdesc = curproc->f_table[fd];
	lock_acquire(fdesc->f_lock);
	iov.iov_ubase = (userptr_t)buf;
	iov.iov_len = buflen;
	u.uio_iov = &iov;
	u.uio_iovcnt = 1;
	u.uio_resid = buflen;
	u.uio_offset = fdesc->offset;
	u.uio_segflg = UIO_USERSPACE;
	u.uio_rw = UIO_WRITE;
	u.uio_space = curproc->p_addrspace;
	
	result = VOP_WRITE(fdesc->v, &u);
	if (result) {
		return result;
	}
	fdesc->offset = u.uio_offset;
	lock_release(fdesc->f_lock);
	return (buflen - u.uio_resid);
	
}
