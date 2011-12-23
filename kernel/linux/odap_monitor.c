/* 
 *   This file is part of the Open Database Audit Project (ODAP).
 *
 *   ODAP is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Foobar is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   The code was developed by Rob Williams
 *
 *   The main idea here is to observe all communications with clients to common DMBS.
 *   There have been many online resources to derieve the structure of this module.
 *   Some code has been copied into here which is open sourced or licensed under GPL.
 *   In particular would like to give thanks to the following resources which helped:
 *
 *   1.(C) 2006,2009 Sebastian Krahmer <krahmer@suse.de> - Unix socket tcpdump style app
 *   2. Kernel Documentation/kprobes.txt - For reading Linux Kernel system calls
 *   3. styx^ @ http://memset.wordpress.com for code to scan System.map for function offsets
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <asm/uaccess.h>	/* for copy_from_user */
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/ctype.h>
#include <net/sock.h>
#include <net/tcp.h>
#include <linux/version.h>

#define DEVICE_MAX_SIZE		1024*1024		// How much audit information to keep before discarding
#define PROC_V			"/proc/version" 	// Location of the current running kernel version
#define BOOT_PATH		"/boot/System.map-" 	// Location of system map for figuring out jprobes

#define MAX_LEN			256



static char device_buffer[DEVICE_MAX_SIZE];		// Buffer of auditing information
static int  device_buffer_size = 0;			// Keep track of the end of the buffer
							// so we know where to add information
							// or how much to send on read request


static const char *NAME = "odap";			// The name for the /dev/ virtual device
static int MAJOR = 0;					// Stores the chardev id
static int nreaders = 0;				// Keep track of the number of applications which have
							// opened /dev/*NAME
int sys_found = 0;

static int mode = -1;					// Which database are we tracking

DECLARE_WAIT_QUEUE_HEAD(wp);				// Block the reader 

#if LINUX_VERSION_CODE > 132624
rwlock_t curr_lock = __RW_LOCK_UNLOCKED(NAME);		// Lock for critical sections so different threads don't
							// collide
#else
rwlock_t curr_lock = RW_LOCK_UNLOCKED;
#endif
static struct class * class_foo;
static struct device * dev_foo;

unsigned long long * kernel_function_ptr = NULL;	// Stores the offset of the 

char line_buff[1024*64];				// Max length for an auditing line is 64k. Need to verify
							// no dbms allows bigger SQL although they probably do.
char arg_buff[1024*64];					// Not currently used - for execve tracing
char arg_line_buff[1024*256];				// Not currently used - for execve tracing


module_param(mode,int,S_IRUSR);				// Parameter for insmod to let us know what DBMS were tracing



/*	chardev_read is called when the reading application calls the os syscall "read" on 
 *      the virtual device. Internally we grab the critical section lock and take as much out 
 *      of device_buffer as possible.
 */
static ssize_t chardev_read(struct file *file, char __user *buffer, size_t read_size, loff_t *ppos){

	char is_empty;	// Do we have any information which we can send the reader

	write_lock(&curr_lock);
	is_empty = (device_buffer_size == 0);
	write_unlock(&curr_lock);

	if (is_empty ) {
		/* if non-blocking, return */
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;
		interruptible_sleep_on(&wp);
		if (signal_pending(current))
			return -ERESTARTSYS;
	}


	write_lock(&curr_lock);

	// If the read request is bigger than what we have in buffer only return what we 
	// have in buffer
	if(read_size > device_buffer_size ){
		read_size = device_buffer_size;
	}

	// Copy the data into userland
	if(copy_to_user(buffer, device_buffer,read_size) != 0){
		read_size  = -EINVAL;
		goto unlock;
	}
	

	// Remove the data which has been sent to the user
	if(read_size ==device_buffer_size){
		strcpy(device_buffer,"");
		device_buffer_size = 0;
	}else{
		strcpy(device_buffer,device_buffer+read_size);
		device_buffer_size-=read_size;
	}
	
	unlock:	write_unlock(&curr_lock);
	return read_size;

}


// The user application has issued close syscall or been terminated
static int chardev_release(struct inode *inode, struct file *filp){
	if (nreaders > 0)
		--nreaders;
	return 0;
}

// We do not support more than one reader; it wont make sense
static int chardev_open(struct inode *inode, struct file *filp)
{
	if (nreaders != 0)
		return -EPERM;
	++nreaders;
	return 0;
}

// The struct is what maps open/read/close to our UDFs
static struct file_operations chardev_fops = {
	.open = chardev_open,
	.read = chardev_read,
	.release = chardev_release,
};

/*  This is how we grab db2 commands entered via the command line locally on the
 *  server and do not go through TCP/IP and is done with IPC messages. There are
 *  some improvements which need to be done to tract static sql via db2bp as well
 *  as sql c applications. The better appraoch would be to grab all the quids
 */
static int my_msgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg){
        int i;
	int information_between_91_97 = 0;
	char count_line[16];
	struct timespec time;
	#if LINUX_VERSION_CODE > 132633
	int euid = current_euid();
	#else
	int euid = current->euid;
	#endif

	if(strcmp(current->comm,"db2bp") == 0){
		if(msgsz > 118*16+17 && *(char*)(msgp+118*16+16) > 0){
			
				// Capture on send out only
				for (i=91*16; (i < 96*16 ) ; i++)
				{
					if(*(char*)(msgp+i) != 0){
						information_between_91_97 = 1;
					}
				}

			if(information_between_91_97 == 0 && (*(char*)(msgp+118*16+16)) != 0x03){

				write_lock(&curr_lock);
				time = current_kernel_time();
				
				if(*(char*)(msgp+118*16+16) == 0x04){
					snprintf(line_buff,1024*256,"%d:%d:%lu:%lu:%s:connect to %s\n",current->pid,euid,time.tv_sec,time.tv_nsec,(char*)(msgp+99*16+16),(char*)(msgp+119*16+2));
				}else if(*(char*)(msgp+118*16+16) == 0x05){
					snprintf(line_buff,1024*256,"%d:%d:%lu:%lu:%s:connect to %s user %s\n",current->pid,euid,time.tv_sec,time.tv_nsec,"",(char*)(msgp+119*16+2),(char*)(msgp+121*16+8));
				}else if(*(char*)(msgp+118*16+16) == 0x03){
					//looks like static sql here .... cough cough.... someone implement......
					
				}else if(*(char*)(msgp+118*16+16) == 0x01){
					snprintf(line_buff,1024*256,"%d:%d:%lu:%lu:%s:reorg table %s\n",current->pid,euid,time.tv_sec,time.tv_nsec,(char*)(msgp+99*16+16),(char*)(msgp+119*16+2));
				}else{
					if(*(char*)(msgp+157*16+8) == 0x4c){
						snprintf(line_buff,1024*256,"%d:%d:%lu:%lu:%s:backup database %s\n",current->pid,euid,time.tv_sec,time.tv_nsec,(char*)(msgp+99*16+16),(char*)(msgp+118*16+16));
					}else{
						if(strlen((char*)(msgp+118*16+16)) <6 || *(char*)(msgp+118*16+16) < 0x20){
							write_unlock(&curr_lock);
							goto jprob_ret_msg;
						}
					  	snprintf(line_buff,1024*256,"%d:%d:%lu:%lu:%s:%s\n",current->pid,euid,time.tv_sec,time.tv_nsec,(char*)(msgp+99*16+16),(char*)(msgp+118*16+16));
					}
				  }
				  snprintf(count_line,16,"%d:",(int)strlen(line_buff));
			

				  if((device_buffer_size +strlen(line_buff)+strlen(count_line)) > DEVICE_MAX_SIZE ){
					write_unlock(&curr_lock);
					goto jprob_ret_msg;
				 }
		
				  strcat(device_buffer,count_line);
				  strcat(device_buffer,line_buff);
				  device_buffer_size += strlen(line_buff)+strlen(count_line)+1;
				  write_unlock(&curr_lock);
				  wake_up_interruptible(&wp);
			}
		}
	}
          /* Always end with a call to jprobe_return(). */
jprob_ret_msg:        jprobe_return();
          /*NOTREACHED*/
          return 0;
}

// For mysql local connections they use local sockets. What's nice is the protocol is the exact same
// as what is for TCP. Simply write the packets out to our /dev/ device
static int udmp_stream_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *msg, size_t total_len)
{
	struct task_struct *p = NULL;
	int dont_skip = 0;
	int i = 0;
	int r = 0;
	char count_line[16];

	if(sock != NULL){

		if (sock->sk) {
			for_each_process(p) {
				/* Peercred->pid contains tgid of the process */
				#if LINUX_VERSION_CODE > 132656
					if(p->tgid == sock->sk->sk_peer_cred->tgcred->tgid)
				#else
					if (p->tgid == sock->sk->sk_peercred.pid)
				#endif
					break;
			}

			if( p != NULL && strcmp(p->comm,"mysqld") == 0){
				dont_skip = 1;
			}
		}
	}
	if(dont_skip){
		write_lock(&curr_lock);
		snprintf(line_buff, sizeof(line_buff), "@ [%s:%d->%s:%d]\n", current->comm, current->pid, p->comm, p->pid);

		// Check to see if we will fill the buffer and return if we will

		for (i = 0; i < msg->msg_iovlen; ++i) {
			snprintf(count_line,16,"%lu:",msg->msg_iov[i].iov_len);
			r += strlen(line_buff) + strlen(count_line)+msg->msg_iov[i].iov_len;
		}

		if(r+device_buffer_size > DEVICE_MAX_SIZE){
			write_unlock(&curr_lock);
			goto jprob_ret_sendmsg;
		}

		//were ok lets copy it in
		memcpy(&device_buffer[device_buffer_size],line_buff,strlen(line_buff));
		device_buffer_size += strlen(line_buff);

		for (i = 0; i < msg->msg_iovlen; ++i) {
			
			snprintf(count_line,16,"%lu:",msg->msg_iov[i].iov_len);
			memcpy(&device_buffer[device_buffer_size],count_line,strlen(count_line));
			device_buffer_size += strlen(count_line);
			if(copy_from_user(&device_buffer[device_buffer_size], msg->msg_iov[i].iov_base, msg->msg_iov[i].iov_len) != 0){
				printk(KERN_ERR "Unable top copy data from client process\n");
			}
			device_buffer_size += msg->msg_iov[i].iov_len;
		}

		write_unlock(&curr_lock);
		wake_up_interruptible(&wp);
	}


          /* Always end with a call to jprobe_return(). */
jprob_ret_sendmsg:        jprobe_return();
          /*NOTREACHED*/
          return 0;
}


static int find_sys_call_table (char *kern_ver, char * lookup_function)
 {

	char buf[MAX_LEN];
	int i = 0;
	char *filename;
	char *p;
	struct file *f = NULL;

	mm_segment_t oldfs;

	oldfs = get_fs();
	set_fs (KERNEL_DS);
	
	filename = kmalloc(strlen(kern_ver)+strlen(BOOT_PATH)+1, GFP_KERNEL);
	
	if ( filename == NULL ) {
	
		return -1;
	
	}
	
	memset(filename, 0, strlen(BOOT_PATH)+strlen(kern_ver)+1);
	
	strncpy(filename, BOOT_PATH, strlen(BOOT_PATH));
	strncat(filename, kern_ver, strlen(kern_ver));
	
	f = filp_open(filename, O_RDONLY, 0);
	
	if ( IS_ERR(f) || ( f == NULL )) {
	
		return -1;
	
	}

	memset(buf, 0x0, MAX_LEN);

	p = buf;

	while (vfs_read(f, p+i, 1, &f->f_pos) == 1) {

		if ( p[i] == '\n' || i == 255 ) {
		
			i = 0;
			
			if ( (strstr(p, lookup_function)) != NULL ) {
				
				char *sys_string;
				
				sys_string = kmalloc(MAX_LEN, GFP_KERNEL);	
				
				if ( sys_string == NULL ) { 
				
					filp_close(f, 0);
					set_fs(oldfs);
	
					kfree(filename);
	
					return -1;
	
				}

				memset(sys_string, 0, MAX_LEN);
				strncpy(sys_string, strsep(&p, " "), MAX_LEN);
			
				kernel_function_ptr = (unsigned long long *) simple_strtoull(sys_string, NULL, 16);
				
				kfree(sys_string);
				
				break;
			}
			
			memset(buf, 0x0, MAX_LEN);
			continue;
		}
		
		i++;
	
	}

	filp_close(f, 0);
	set_fs(oldfs);
	
	kfree(filename);

	return 0;
}

char *search_file(char *buf) {
	
	struct file *f;
	char *ver;
	mm_segment_t oldfs;

	oldfs = get_fs();
	set_fs (KERNEL_DS);
	
	f = filp_open(PROC_V, O_RDONLY, 0);
	
	if ( IS_ERR(f) || ( f == NULL )) {
	
		return NULL;
	
	}
	
	memset(buf, 0, MAX_LEN);
	
	vfs_read(f, buf, MAX_LEN, &f->f_pos);
	
	ver = strsep(&buf, " ");
	ver = strsep(&buf, " ");
	ver = strsep(&buf, " ");
		
	filp_close(f, 0);	
	set_fs(oldfs);
	
	return ver;

}


static struct jprobe my_jprobe = { .entry = (kprobe_opcode_t *) my_msgrcv };
static struct jprobe my_jprobe_stream_sendmsg = { .entry = (kprobe_opcode_t *) udmp_stream_sendmsg };


int init_module(void)
{
	char *kern_ver;
	char *buf;
	int ret;
	void * ptr_err;

	buf = kmalloc(MAX_LEN, GFP_KERNEL);
	
	if ( buf == NULL ) {
	
		sys_found = 1;
		return -1;
	
	}	
	kern_ver = search_file(buf);
		
	if ( kern_ver == NULL ) {

		sys_found = 1;
		return -1;
	
	}
	

	if(mode == 1){
		//TRACE DB2
		if ( find_sys_call_table(kern_ver," sys_msgrcv") == -1 ) {
	
			sys_found = 1;
			return -1; 
		}

			my_jprobe.kp.addr = (kprobe_opcode_t *) kernel_function_ptr;
		 if (!my_jprobe.kp.addr) {
		         printk("Couldn't find %s to plant jprobe\n", "do_execve");
		         return -1;
		 }
	 
		 if ((ret = register_jprobe(&my_jprobe)) <0) {
		         printk("register_jprobe failed, returned %d\n", ret);
		         return -1;
		 }

	}else if(mode == 0){
		//TRACE MySQL
		if ( find_sys_call_table(kern_ver," unix_stream_sendmsg") == -1 ) {
	
			sys_found = 1;
			return -1; 
		}


		my_jprobe_stream_sendmsg.kp.addr = (kprobe_opcode_t *) kernel_function_ptr;
		 if (!my_jprobe_stream_sendmsg.kp.addr) {
		         printk("Couldn't find %s to plant jprobe\n", "do_execve");
		         return -1;
		 }
	 
		 if ((ret = register_jprobe(&my_jprobe_stream_sendmsg)) <0) {
		         printk("register_jprobe failed, returned %d\n", ret);
		         return -1;
		 }

	}else{
		         printk("Invalid mode type %d\n", mode);
		         return -1;
	}


	MAJOR = register_chrdev(0, NAME, &chardev_fops);
	if (MAJOR < 0) {
		printk(KERN_ERR ": unable to register character device\n");
		return MAJOR;
	}

	class_foo = class_create(THIS_MODULE,NAME);
	if(IS_ERR(ptr_err = class_foo))
		goto err2;

#if LINUX_VERSION_CODE > 132624
	dev_foo = device_create(class_foo,NULL,MKDEV(MAJOR,0),NULL,NAME);
#else
	dev_foo = class_device_create(class_foo,NULL,MKDEV(MAJOR,0),NULL,NAME);
#endif
	if(IS_ERR(ptr_err = dev_foo)){
		goto err;
	}

         return 0;

	err:
		class_destroy(class_foo);

	err2:
		unregister_chrdev(MAJOR,NAME);
		if(mode == 1){
			unregister_jprobe(&my_jprobe);
		}else if(mode == 0){
			unregister_jprobe(&my_jprobe_stream_sendmsg);
		}
		return PTR_ERR(ptr_err);
}

// Remove all the devices created
void cleanup_module(void)
{
	if(mode == 1){
		unregister_jprobe(&my_jprobe);
	}else if(mode == 0){
		unregister_jprobe(&my_jprobe_stream_sendmsg);
	}
#if LINUX_VERSION_CODE > 132624
	device_destroy(class_foo,MKDEV(MAJOR,0));
#else
	class_device_destroy(class_foo,MKDEV(MAJOR,0));
#endif
	class_destroy(class_foo);
	unregister_chrdev(MAJOR,NAME);

}
 
MODULE_LICENSE("GPL");

