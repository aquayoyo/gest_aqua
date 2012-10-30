/*
 *  AquaPwm.c
 *
 *  Copyright (C) 2012 Lionel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  
 *
 *
 *
 */

#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/rtc.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <mach/gpio.h>
/***************************************************************************
 * Global stuff
 ***************************************************************************
 */
#undef DEBUG
//#define DEBUG 0

/*
 * For character devices
 */
#include <linux/fs.h>		/* character device definitions	*/


/*
 * Device Declarations
 */


/*
 * The name for our device, as it will appear in /proc/devices
 */
static const char PwmAquaGest_name[] = "PwmAquaGest";

#define _VERSION	"1.0.1 lionel"
#define VERSION_STR "getsion pwm pour AquaGest : "_VERSION

/***************************************************************************
 * Local stuff
 ***************************************************************************
 */

#ifdef	DEBUG
# define debugk(fmt,args...)	printk(fmt ,##args)
# define DEBUG_OPTARG(arg)	arg,
#else
# define debugk(fmt,args...)
# define DEBUG_OPTARG(arg)
#endif

#if defined(CONFIG_AQUA_PWM) || defined(CONFIG_AQUA_PWM_MODULE)
static struct pin_desc imx233_olinuxino_pwm_pins[] = {
		/* Configurations of PWM 2 port pins */
		{
				.name = "PWM2",
				.id = PINID_PWM2,
				.fun = PIN_FUN1,
				.strength = PAD_8MA,
				.voltage = PAD_3_3V,
				.pullup = 1,
				.drive = 1,
				.pull = 1,
		},
};
#endif
/* avoid multiple access */
static int major=0;
static struct class *pPwmAquaGest_class;

static int PwmAquaGest_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg);
static int PwmAquaGest_open(struct inode * inode, struct file * file);
static int PwmAquaGest_release(struct inode * inode, struct file * file);

static const struct file_operations regdebug_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= PwmAquaGest_ioctl,
	.open		= PwmAquaGest_open,
	.release	= PwmAquaGest_release,
};


extern void gpio_free_irq(u32 port, u32 sig_no,  void *dev_id);
extern void gpio_clear_int(u32 port, u32 sig_no);
extern int gpio_request_irq(__u32 port, __u32 sig_no, enum gpio_prio prio,
							gpio_irq_handler handler, __u32 irq_flags,const char *devname, void *dev_id);
/****************************************************************************/

static int PwmAquaGest_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg) {
	int err=0;
	switch (cmd) {
		default :
			err=-EINVAL;
			break;
	}
	return err;
}

static int PwmAquaGest_open(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();
		unlock_kernel();
	
	return ret;
}


static int PwmAquaGest_release(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();	
	unlock_kernel();
	return ret;
}

/***************************************************************************
 *
 * Initialize the driver - Register the character device
 */
 
static __init int PwmAquaGest_init(void)
{
	struct class_device *temp_class;

	printk (KERN_INFO VERSION_STR "\n");

	major = register_chrdev(0,PwmAquaGest_name,&regdebug_fops);
	if (!major) {
		printk("PwmAquaGest_init: failed to register char device\n");		
		goto  err;
	}else
		printk("PwmAquaGest_init MAJOR=%d\n",major);

	pPwmAquaGest_class = class_create(THIS_MODULE, PwmAquaGest_name);
	if (IS_ERR(pPwmAquaGest_class)) {
		printk("Error creating PwmAquaGest class.\n");
		goto err1;
	}

	temp_class = device_create(pPwmAquaGest_class, NULL,MKDEV(major, 0),NULL, PwmAquaGest_name);
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating PwmAquaGest_init class device.\n");		
		goto err2;
	}

	return 0;
err3:
	device_destroy(pPwmAquaGest_class, MKDEV(major, 0));
err2:
	class_destroy(pPwmAquaGest_class);
err1:
	unregister_chrdev (major,PwmAquaGest_name);
	major=0;
err:
	return -1;
}

static void __exit PwmAquaGest_exit(void)
{
	if (major) {
		device_destroy(pPwmAquaGest_class, MKDEV(major, 0));
		class_destroy(pPwmAquaGest_class);
		unregister_chrdev (major,PwmAquaGest_name);
		major=0;		
	}

	debugk (VERSION_STR "de-registered\n");
}

MODULE_DESCRIPTION(VERSION_STR);
MODULE_AUTHOR("Lionel");
MODULE_LICENSE("GPL");

module_init(PwmAquaGest_init);
module_exit(PwmAquaGest_exit);

