#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

static struct lock myLock1;               /* lock for syschronization between files */

static void syscall_handler (struct intr_frame *);



#define charptrparam1 (char*)(*((int*)f->esp + 1))

#define intparm1        *((int*)f->esp + 1)
#define intparm2        *((int*)f->esp + 2)
#define intparm3        *((int*)f->esp + 3)

#define unsignedparm1   *((unsigned*)f->esp + 1)
#define unsignedparm2   *((unsigned*)f->esp + 2)

#define voidptrparam2   (void*)(*((int*)f->esp + 2))

void exit_wrapper(struct intr_frame *f);
void exec_wrapper(struct intr_frame *f);
void wait_wrapper(struct intr_frame *f);
void create_wrapper(struct intr_frame *f);
void remove_wrapper(struct intr_frame *f);
void open_wrapper(struct intr_frame *f);
void filesize_wrapper(struct intr_frame *f);
void read_wrapper(struct intr_frame *f);
void write_wrapper(struct intr_frame *f);
void seek_wrapper(struct intr_frame *f);
void tell_wrapper(struct intr_frame *f);
void close_wrapper(struct intr_frame *f);


int exec(char* file_name);
int call_wait(int pid);
bool call_create(char* name, size_t size);
bool call_remove(char* name);
int call_open(char* name);
int call_filesize(int fd);
int call_read(int fd,void* buffer, int size);
int call_write(int fd, void* buffer, int size);
void call_seek(int fd, unsigned pos);
unsigned call_tell(int fd);
void call_close(int fd);

struct open_file* file_get(int fd);










void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  lock_init(&myLock1);
}

static void
syscall_handler (struct intr_frame *f) 
{ 

  if(*(int *)(f->esp) == SYS_HALT){ 
    Halt();
  }else if(*(int *)(f->esp) == SYS_EXIT){ 
    exit_wrapper(f);  
  }else if(*(int *)(f->esp) == SYS_EXEC){
    exec_wrapper(f);    
  }else if(*(int *)(f->esp) == SYS_WAIT){  
    wait_wrapper(f);    
  }else if(*(int *)(f->esp) == SYS_CREATE){  
    create_wrapper(f);    
  } else if(*(int *)(f->esp) == SYS_REMOVE){  
    remove_wrapper(f);    
  } else if(*(int *)(f->esp) == SYS_OPEN){  
    open_wrapper(f);    
  }else if(*(int *)(f->esp) == SYS_FILESIZE){  
    filesize_wrapper(f);  
  }else if(*(int *)(f->esp) == SYS_READ){  
    read_wrapper(f);    
  }else if(*(int *)(f->esp) == SYS_WRITE){  
    write_wrapper(f);    
  }else if(*(int *)(f->esp) == SYS_SEEK){  
    seek_wrapper(f);    
  }else if(*(int *)(f->esp) == SYS_TELL){  
    tell_wrapper(f);   
  }else if(*(int *)(f->esp) == SYS_CLOSE){ 
    close_wrapper(f);  
  }

}

void 
validate_void_ptr(const void* pt)
{
  if (!(pt != NULL && is_user_vaddr(pt) && pagedir_get_page(thread_current()->pagedir, pt) != NULL)) 
  {
    exit(-1);
  }
}



void exit_wrapper(struct intr_frame *f){
  validate_void_ptr(f->esp+4);
  exit(intparm1);
}

void exec_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp+4);
  if (charptrparam1 == NULL) 
    exit(-1);
  lock_acquire(&myLock1);
  f->eax = exec(charptrparam1);
  lock_release(&myLock1);
}

void wait_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp+4);
  f->eax = call_wait(intparm1);
}

void create_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp + 4);
  validate_void_ptr(f->esp + 8);

  if (charptrparam1 == NULL) 
    exit(-1);

  f->eax = call_create(charptrparam1,intparm2);
}

void remove_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp + 4);

  if (charptrparam1 == NULL) //refrence NULL = error
    exit(-1);

  f->eax = call_remove(charptrparam1);
}

void open_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp + 4);


  if (charptrparam1 == NULL) //refrence NULL = error
    exit(-1);

  f->eax = call_open(charptrparam1);
}

void filesize_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp + 4);
  f->eax = call_filesize(intparm1);
}

void read_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp + 4);
  validate_void_ptr(f->esp + 8);
  validate_void_ptr(f->esp + 12);

  validate_void_ptr(voidptrparam2 + intparm3); // to validate if it dos=es not exceed buffer size (buffer + size)
  
  f->eax = call_read(intparm1,voidptrparam2,intparm3);
}

void write_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp + 4);
  validate_void_ptr(f->esp + 8);
  validate_void_ptr(f->esp + 12);

  if (voidptrparam2 == NULL) //refrence NULL = error
    exit(-1);
  
  f->eax = call_write(intparm1,voidptrparam2,intparm3);
}

void seek_wrapper(struct intr_frame *f)
{
  validate_void_ptr(f->esp + 4);
  validate_void_ptr(f->esp + 8);
  call_seek(intparm1,unsignedparm2);
}

void tell_wrapper(struct intr_frame *f){

  validate_void_ptr(f->esp + 4);
  f->eax = call_tell(unsignedparm1);
}

void close_wrapper(struct intr_frame *f){
  
  validate_void_ptr(f->esp + 4);
  call_close(intparm1);
}

void Halt(){
  shutdown_power_off();
}
void exit(int status){

  struct thread* parent = thread_current()->parent_process;
  printf("%s: exit(%d)\n", thread_current()->name, status);
  if(parent) parent->child_status = status;
  thread_exit();
}

int exec(char* file_name){

  return process_execute(file_name);
}



int call_wait(int pid){

  return process_wait(pid);
}


//////////////////////////////////////////////////////
bool call_create(char* name, size_t size){

  bool sucess;
  lock_acquire(&myLock1);
  sucess = filesys_create(name,size);
  lock_release(&myLock1);
  return sucess;
}


//========================================================================
bool call_remove(char* str){

  bool removal;
  lock_acquire(&myLock1);
  removal = filesys_remove(str);
  lock_release(&myLock1);
  return removal;
}


//==============================================================================================
int call_open(char* name){

  struct open_file* current_file = palloc_get_page(0); // alocating page to currently open file
  if (current_file == NULL) // wrong file return 0
  {
    palloc_free_page(current_file); // releasing page for wrong file
    return -1;
  }

  lock_acquire(&myLock1);
  current_file->ptr = filesys_open(name); //open that file
  lock_release(&myLock1);
  if (current_file->ptr == NULL) 
  {
    return -1;
  }
  current_file->fd = ++thread_current()->fd_last; // add fd mumber for open file

  list_push_back(&thread_current()->open_file_list,&current_file->elem); // add file to open files list for each thread
  return current_file->fd;
}


//==============================================================================================
int call_filesize(int fd){

  struct file* myFile = file_get(fd)->ptr;

  if (myFile == NULL) 
    return -1;
  int length;
  lock_acquire(&myLock1);
  length = file_length(myFile);
  lock_release(&myLock1);
  return length;
}

//==============================================================================================
int call_read(int fd,void* buffer, int size){
  if (fd == 0){
    
    for (size_t i = 0; i < size; i++)
    {
      lock_acquire(&myLock1);
      ((char*)buffer)[i] = input_getc();
      lock_release(&myLock1);
    }
    return size;
    
  } else if(fd==1) {

    //negative area

  }else{

    struct file* myFile = file_get(fd)->ptr;

    if (myFile == NULL)
      return -1;
    int reading;
    lock_acquire(&myLock1);
    reading = file_read(myFile,buffer,size);
    lock_release(&myLock1);
    return reading;
    
  }
}

//==============================================================================================
int call_write(int fd, void* buffer, int size){

  if (fd == 1)
  {
    
    lock_acquire(&myLock1);
    putbuf(buffer,size);
    lock_release(&myLock1);
    return size;

  } else {
    
    struct file* my_file = file_get(fd)->ptr;

    if (my_file == NULL)
      return -1;
    int res;
    lock_acquire(&myLock1);
    res = file_write(my_file,buffer,size);
    lock_release(&myLock1);
    return res;
  }

}

//==============================================================================================
void call_seek(int fd, unsigned pos){

  struct file* myFile = file_get(fd)->ptr;
  if (myFile == NULL)
    return;
  lock_acquire(&myLock1);
  file_seek(myFile,pos);
  lock_release(&myLock1);
}

//==============================================================================================
unsigned call_tell(int fd){ 
  
  struct file* myFile = file_get(fd)->ptr;
  if (myFile == NULL)
    return -1;
  unsigned res;
  lock_acquire(&myLock1);
  res = file_tell(myFile);
  lock_release(&myLock1);
  return res;
}


//==============================================================================================
void call_close(int fd){
  
  struct open_file* myFile = file_get(fd);
  if (myFile == NULL)  
    return;
  lock_acquire(&myLock1);
  file_close(myFile->ptr);
  lock_release(&myLock1);
  list_remove(&myFile->elem);
  palloc_free_page(myFile);
}

//==============================================================================================
struct open_file* file_get(int fd){
    struct thread* t = thread_current();
    //struct file* myFile = NULL;
    for (struct list_elem* list_elemnnt = list_begin (&t->open_file_list); list_elemnnt != list_end (&t->open_file_list);list_elemnnt = list_next (list_elemnnt))
    {
      struct open_file* myFile = list_entry (list_elemnnt, struct open_file, elem);
      if (myFile->fd == fd)
      {
        return myFile;
      }
    }
    return NULL; // Can.t find required file  so retrun NULL to catch error
}