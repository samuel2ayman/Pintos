#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

/* check if the pointer is valid */
void validate_void_ptr(const void* pt);

void exit(int status);

#endif /* userprog/syscall.h */