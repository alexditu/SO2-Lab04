/*
 * SO2 Lab - Linux device drivers (#4)
 * All tasks
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/atomic.h>
// #include <linux/cdev.h>

#include "../include/so2_cdev.h"

MODULE_DESCRIPTION("SO2 character device");
MODULE_AUTHOR("SO2");
MODULE_LICENSE("GPL");

#define LOG_LEVEL	KERN_ALERT

#define MY_MAJOR		42
#define MY_MINOR		0
#define NUM_MINORS		1
#define MODULE_NAME		"so2_cdev"
#define MESSAGE			"hello\n\0"
#define IOCTL_MESSAGE		"Hello ioctl"

#ifndef BUFSIZ
#define BUFSIZ		4096
#endif

static int cdev_open(struct inode *inode, struct file *file);
static int cdev_release(struct inode *inode, struct file *file);
static int cdev_read(struct file *file, char __user *user_buf, size_t size,
			loff_t *offset);
static int cdev_write(struct file *file, const char __user *user_buf,
			size_t size, loff_t *offset);


static char buffer[BUFSIZ] = MESSAGE;
static atomic_t atomic;

struct cdev my_cdev;

struct file_operations my_fops = {
	.owner 		= THIS_MODULE,
	.open 		= cdev_open,
	.release	= cdev_release,
	.read		= cdev_read,
	.write		= cdev_write,
};

static int cdev_open(struct inode *inode, struct file *file)
{
	int old;

	printk(LOG_LEVEL "cdev_open\n");

	old = atomic_cmpxchg(&atomic, 0, 1);
	if (old == 0) {
		printk("Succes: first open\n");
	} else {
		printk("Failed: already opened\n");
		return -EBUSY;
	}

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(1000);

	return 0;
}

static int cdev_release(struct inode *inode, struct file *file)
{
	int old;

	printk(LOG_LEVEL "cdev_release\n");

	old = atomic_cmpxchg(&atomic, 1, 0);
	printk("Release: old = %d\n", old);
	return 0;
}

static int cdev_read(struct file *file, char __user *user_buf,
			size_t size, loff_t *offset)
{
	int ret;
	long unsigned off;

	ret = copy_to_user(user_buf, buffer, strlen(buffer));
	if (ret != 0) {
		printk("Error copy_to_user: %d\n", ret);
		return -EFAULT;
	}

	// ret = copy_from_user(&off, offset, sizeof(loff_t));
	// if (ret != 0) {
	// 	printk("Error copy_from_user: %d\n", ret);
	// 	return -EFAULT;
	// } else {
	// 	printk("Offset is: %lu\n", off);
	// }

	return strlen(buffer);
}

static int cdev_write(struct file *file, const char __user *user_buf,
			size_t size, loff_t *offset)
{
	int ret;

	ret = copy_from_user(buffer, user_buf, size);
	if (ret != 0) {
		printk("Error: cdev_write from user\n");
		return -1;
	}

	buffer[size] = '\0';
	printk("Buffer is: %s\n", buffer);

	return size;
}

static int so2_cdev_init(void)
{
	int ret;

	atomic_set(&atomic, 0);

	ret = register_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR),
					NUM_MINORS, MODULE_NAME);

	// ret = register_chrdev_region(13, 1, "input");
	if (ret != 0) {
		printk(LOG_LEVEL "Error register_chardev_region ret: %d\n", ret);
		return ret;
	}

	cdev_init(&my_cdev, &my_fops);
	ret = cdev_add(&my_cdev, MKDEV(MY_MAJOR, MY_MINOR), 1);

	return 0;
}

static void so2_cdev_exit(void)
{
	cdev_del(&my_cdev);
	unregister_chrdev_region(13, NUM_MINORS);
}

module_init(so2_cdev_init);
module_exit(so2_cdev_exit);
