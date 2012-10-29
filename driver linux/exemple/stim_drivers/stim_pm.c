/*
 *  stim_pm.c
 *
 *  Copyright (C) 2008 STim
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

#include <linux/pm.h>
#include <linux/pm_legacy.h>

#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>

#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include "../../arch/arm/mach-mx3/mx31_pins.h"
#include "../../arch/arm/mach-mx3/iomux.h"

#include <linux/suspend.h>
#include <asm/arch/stim_pm.h>
#include <asm/arch/dlgi2c.h>

/***************************************************************************
 * Global stuff
 ***************************************************************************
 */
//#undef DEBUG
#define TEST	1



/*
 * For character devices
 */
#include <linux/fs.h>		/* character device definitions	*/


/*
 * Device Declarations
 */

extern void gpio_clear_int(u32 port, u32 sig_no);
extern int pm_suspend(suspend_state_t state);
extern int kernel_execve(const char *filename, char *const argv[], char *const envp[]);
/*
 * The name for our device, as it will appear in /proc/devices
 */
static const char stim_pm_name[] = "stim_pm";
#define _VERSION	"$Revision: 1.0 $ STim"

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

/****************************************************************************/
static irqreturn_t Reveil_irq (int irq, void *devid);
static irqreturn_t Reveil_irq (int irq, void *devid) {	
	//printk ("Reveil_irq ISR=%08x DR=%08x\n",__raw_readl(IO_ADDRESS(0x53fcc018)),__raw_readl(IO_ADDRESS (0x53fcc000)));
	int i=0;

	for (i=0;i<1000;i++)
		__raw_readl(IO_ADDRESS(CS4_BASE_ADDR));
	
	gpio_clear_int (0,4);
	return IRQ_HANDLED;
}


/****************************************************************************/
/* avoid multiple access */
static int major=0;
static struct class *stim_pm_class;

static int stim_pm_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg);
static int stim_pm_open(struct inode * inode, struct file * file);
static int stim_pm_release(struct inode * inode, struct file * file);

static const struct file_operations stim_pm_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= stim_pm_ioctl,
	.open		= stim_pm_open,
	.release	= stim_pm_release,
};

/****************************************************************************/
static int stim_pm_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg) {

	switch (cmd) {
	case STIM_SET_VEILLE:
		{
			pm_suspend(PM_SUSPEND_MEM);
		}
		break;
	case STIM_SET_OFF:
		{
		}
		break;
	case STIM_SET_ON:
		{
		}
		break;
	default :
		return -EINVAL;
		break;
	}

	return 0;
}

static int stim_pm_open(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();
	
	unlock_kernel();	
	return ret;
}


static int stim_pm_release(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();
	
	
	unlock_kernel();
	return ret;
}

/************************************************************************************************/
static int stim_pm_resume(struct platform_device *pdev);
static int stim_pm_suspend(struct platform_device *pdev);
static void run_resume_process(void *startup);

static void run_resume_process(void *startup)
{
	#define MAX_INIT_ARGS CONFIG_INIT_ENV_ARG_LIMIT
	#define MAX_INIT_ENVS CONFIG_INIT_ENV_ARG_LIMIT

	static char * argv_init[MAX_INIT_ARGS+2] = { "init", "start",NULL };
	char * envp_init[MAX_INIT_ENVS+2] = { "HOME=/", "TERM=linux", NULL, };

	char init_filename [32]="/etc/rc.d/init.d/suspend";

	argv_init[0] = init_filename;
	printk ("init_filename=%s\t",init_filename);
	printk ("%d\n",kernel_execve(init_filename, argv_init, envp_init));
}


static struct platform_driver stim_pm_driver = {
	.driver = {
		   .name = "stim_pm",
		   },
	.resume = stim_pm_resume,
	.suspend = stim_pm_suspend,

};


static struct platform_device stim_pm_device = {
	.name		= "stim_pm",
	.dev = {
		.release = stim_pm_release,
	},
};

extern ssize_t i2c_sorhea_Send (struct _i2c_sorhea_client *client_i2c,char *buf,size_t nbytes);
static int stim_pm_suspend(struct platform_device *pdev){
	char *argv[10], *envp[3];
	char Buf [2];
	int i;
	int fd_dlg_sohrea;

	i = 0;
	envp[i++] = "HOME=/";
	envp[i++] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
	envp[i] = NULL;
	
	/* set up the argument list */
	i = 0;
	argv[i++] = "/etc/rc.d/init.d/suspend";
	argv[i++] = "start";
	argv[i] = NULL;

	call_usermodehelper_keys (argv[0], argv, envp,NULL,1);

	
	Buf [0]=GET_ETAT;
	Buf [1]=(char)SET_VEILLE;
	i2c_sorhea_Send (NULL,Buf,2);

	return 0;
}

static int stim_pm_resume(struct platform_device *pdev){
	char *argv[10], *envp[3];
	char Buf [2];
	int i;

	i = 0;
	envp[i++] = "HOME=/";
	envp[i++] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
	envp[i] = NULL;
	
	/* set up the argument list */
	i = 0;
	argv[i++] = "/etc/rc.d/init.d/resume";
	argv[i++] = "start";
	argv[i] = NULL;

	call_usermodehelper_keys (argv[0], argv, envp,NULL,0);

	Buf [0]=GET_ETAT;
	Buf [1]=(char)SET_ON;
	i2c_sorhea_Send (NULL,Buf,2);

	return 0;

}
/************************************************************************************************/
//EXPORT_NO_SYMBOLS;

/***************************************************************************
 *
 * Initialize the driver - Register the character device
 */
static __init int stim_pm_init(void)
{
	struct class_device *temp_class;

	printk (KERN_INFO "gestion de l'alimentation\n driver version " _VERSION "\n");

	if (platform_device_register(&stim_pm_device)==0) {
		if (platform_driver_register(&stim_pm_driver) != 0) {
			printk(KERN_ERR"Driver register failed for stim_pm_driver\n");

			platform_device_unregister (&stim_pm_device);
			return -ENODEV;
		}
	}

	/*
	 * Register the driver as a character device, for applications
	 * to access it for ioctls.
	 * First argument (major) to register_chrdev implies a dynamic
	 * major number allocation.
	 */
	major = register_chrdev(0,stim_pm_name,&stim_pm_fops);
	if (!major) {
		printk("stim_pm_init: failed to register char device\n");		
		goto  err;
	}else
		printk("gestion de l'alimentation MAJOR=%d\n",major);

	stim_pm_class = class_create(THIS_MODULE, stim_pm_name);
	if (IS_ERR(stim_pm_class)) {
		printk("Error creating stim_pm class.\n");
		goto err1;
	}

	temp_class = class_device_create(stim_pm_class, NULL,MKDEV(major, 0),NULL, stim_pm_name);
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating stim_pm_init class device.\n");		
		goto err2;
	}

#if 1
	mxc_iomux_set_pad(MX31_PIN_GPIO1_4,(PAD_CTL_SRE_SLOW | PAD_CTL_DRV_NORMAL| PAD_CTL_PKE_NONE | PAD_CTL_HYS_SCHMITZ));
	//if (gpio_request_irq (0,4,GPIO_LOW_PRIO,&Reveil_irq,IRQF_TRIGGER_FALLING,stim_pm_name,NULL)) {

	if (gpio_request_irq (0,4,GPIO_LOW_PRIO,&Reveil_irq,IRQF_TRIGGER_FALLING,stim_pm_name,NULL)) {	
		printk(KERN_ERR "Error request interupt.\n");		
		goto err3;
	}

	printk ("IOMUX_TO_IRQ(MX31_PIN_GPIO1_4)=%d\n",IOMUX_TO_IRQ(MX31_PIN_GPIO1_4));
	//set_irq_wake (IOMUX_TO_IRQ(MX31_PIN_GPIO1_4),1);
#endif

	return 0;
err3:
	class_device_destroy(stim_pm_class, MKDEV(major, 0));
err2:
	class_destroy(stim_pm_class);
err1:
	unregister_chrdev (major,stim_pm_name);
	major=0;
err:
	return -1;
}

static void __exit stim_pm_exit(void) {
	gpio_free_irq (0,4,NULL);

	if (major) {
		class_device_destroy(stim_pm_class, MKDEV(major, 0));
		class_destroy(stim_pm_class);
		unregister_chrdev (major,stim_pm_name);
		major=0;		
	}

	platform_driver_unregister (&stim_pm_driver);
	platform_device_unregister (&stim_pm_device);
	
	debugk ("gestion de l'alimentation de-registered\n");
}

MODULE_DESCRIPTION("gestion de l'alimentation");
MODULE_AUTHOR("Lionel");
MODULE_LICENSE("GPL");

module_init(stim_pm_init);
module_exit(stim_pm_exit);
