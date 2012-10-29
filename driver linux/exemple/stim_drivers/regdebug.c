/*
 *  regdebug.c
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
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/rtc.h>
#include <linux/delay.h>

#include <mach/hardware.h>
#include <mach/gpio.h>
#include <mach/mx31_pins.h>
#include <linux/ipu.h>

#include <mach/regdebug.h>

#include <mach/pmic_power.h>
#include <mach/mxc_pm.h>
#include <linux/suspend.h>

#include <mach/arc_otg.h>
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


/*
 * The name for our device, as it will appear in /proc/devices
 */
static const char regdebug_name[] = "regdebug";
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
/*
extern static u8 isp1504_read(int reg, volatile u32 * view);
extern static void isp1504_set(u8 bits, int reg, volatile u32 * view);
*/

extern int ov7720_read_reg(u8 reg, u8 *val);
extern int ov7720_write_reg(u8 reg, u8 val);

#if (defined (CONFIG_MXC_CAMERA_VS6624) || defined (CONFIG_MXC_CAMERA_VS6624_MODULE) || defined (CONFIG_MXC_CAMERA_VS6624_TEST) || defined (CONFIG_MXC_CAMERA_VS6624_TEST_MODULE))
extern int vs6624_read_reg(u16 reg, u8 *val);
extern int vs6624_write_reg(u16 reg, u8 val);
#endif

#if (defined (CONFIG_MXC_CAMERA_OV9655) || defined (CONFIG_MXC_CAMERA_OV9655_MODULE) || defined (CONFIG_MXC_CAMERA_OV9655_V2) || defined (CONFIG_MXC_CAMERA_OV9655_V2_MODULE))
extern int ov9655_read_reg(u16 reg, u8 *val);
extern int ov9655_write_reg(u16 reg, u8 val);
extern int ov9655_get_aec(u16 *usAec);
extern int ov9655_set_aec(unsigned short usAec);
extern int ov9655_get_agc(u16 *usAgc);
extern int ov9655_set_agc(u16 usAgc);
#endif

#include <linux/clk.h>

static void set_mclk_rate(uint32_t * p_mclk_freq);

/*
 * set_mclk_rate
 *
 * @param       p_mclk_freq  mclk frequence
 *
 */
void set_mclk_rate(uint32_t * p_mclk_freq)
{
	struct clk *clk;
	int i;
	uint32_t freq = 0;
	uint32_t step = *p_mclk_freq / 8;

	clk = clk_get(NULL, "csi_clk");

	for (i = 0; i <= 8; i++) {
		freq = clk_round_rate(clk, *p_mclk_freq - (i * step));
		if (freq <= *p_mclk_freq)
			break;
	}
	clk_set_rate(clk, freq);

	*p_mclk_freq = freq;

	clk_put(clk);
	printk("mclk frequency = %d\n", *p_mclk_freq);
}

#if (defined (CONFIG_SMSC91XX) || defined(CONFIG_SMSC91XX_MODULE))
typedef unsigned long DWORD;
extern void Lan_SetRegDW (DWORD dwOffset,DWORD dwVal);
extern  DWORD Lan_GetRegDW (DWORD dwOffset);
extern  DWORD Extern_Mac_GetRegDW (DWORD dwRegOffset);
extern void Extern_Mac_SetRegDW (DWORD dwRegOffset,DWORD dwVal);
#endif
/****************************************************************************/
/* avoid multiple access */
static int major=0;
static struct class *regdebug_class;

static int regdebug_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg);
static int regdebug_open(struct inode * inode, struct file * file);
static int regdebug_release(struct inode * inode, struct file * file);

static const struct file_operations regdebug_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= regdebug_ioctl,
	.open		= regdebug_open,
	.release	= regdebug_release,
};

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
/****************************************************************************/
static int regdebug_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg) {
		
	struct StimDebugReg ArgReg;
	switch (cmd) {
	case REG_R_L_IMX:
		{
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
		
				ArgReg.ValeurWrite=0;
				ArgReg.ValeurRead= __raw_readl(IO_ADDRESS(ArgReg.Registre));		
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
				
			}else
				return -EINVAL;
		}
		break;
	case REG_W_L_IMX:
		{			
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				
				__raw_writel(ArgReg.ValeurWrite, IO_ADDRESS(ArgReg.Registre));
				ArgReg.ValeurRead= __raw_readl(IO_ADDRESS(ArgReg.Registre));					
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
				
			}else
				return -EINVAL;
		}
		break;
#if 0
	case REG_R_L2_IMX:
		{
			void *l2_base;
			/* Initialize L2 cache */
			l2_base = ioremap(L2CC_BASE_ADDR, SZ_4K);		
			if (l2_base) {			
				if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
					
					ArgReg.ValeurWrite=0;
					ArgReg.ValeurRead=readl(l2_base + ArgReg.Registre);		
					copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
					
				}else
					return -EINVAL;
				
			}else
				return -EINVAL;
		}
		break;
		case REG_W_L2_IMX:
		{
			void *l2_base;
			/* Initialize L2 cache */
			l2_base = ioremap(L2CC_BASE_ADDR, SZ_4K);		
			if (l2_base) {			
				if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
					
					writel(ArgReg.ValeurWrite,l2_base + ArgReg.Registre);
					
					ArgReg.ValeurRead=readl(l2_base + ArgReg.Registre);		
					copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
					
				}else
					return -EINVAL;
				
			}else
				return -EINVAL;
		}
		break;
#endif
#if (defined (CONFIG_MXC_CAMERA_VS6624) || defined (CONFIG_MXC_CAMERA_VS6624_MODULE) || defined (CONFIG_MXC_CAMERA_VS6624_TEST) || defined (CONFIG_MXC_CAMERA_VS6624_TEST_MODULE))
	case REG_R_VS6624:
		{
			int mclk=12000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);
				ArgReg.ValeurWrite=0;				
				if (vs6624_read_reg(ArgReg.Registre,&ArgReg.ValeurRead)>=0) {
					copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
				}else
					return -EINVAL;
				
				ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
			}else
				return -EINVAL;
		}
		break;
	case REG_W_VS6624:
		{
			int mclk=12000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);
				if (vs6624_write_reg (ArgReg.Registre,ArgReg.ValeurWrite)>=0) {
					mdelay(100);
					if (vs6624_read_reg(ArgReg.Registre,&ArgReg.ValeurRead)) {	
						copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
					}
				ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
				}else
					return -EINVAL;
				
			}else
				return -EINVAL;

		}
		break;
#endif
		
#if (defined (CONFIG_MXC_CAMERA_OV9655) || defined (CONFIG_MXC_CAMERA_OV9655_MODULE) || defined (CONFIG_MXC_CAMERA_OV9655_V2) || defined (CONFIG_MXC_CAMERA_OV9655_V2_MODULE))
	case REG_R_OV9655:
		{
			int mclk=24000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);
				ArgReg.ValeurWrite=0;				
				if (ov9655_read_reg(ArgReg.Registre,&ArgReg.ValeurRead)>=0) {
					copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
				}else
					return -EINVAL;
				
				ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
			}else
				return -EINVAL;
		}
		break;
	case REG_W_OV9655:
		{
			int mclk=24000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);
				if (ov9655_write_reg (ArgReg.Registre,ArgReg.ValeurWrite)>=0) {
					msleep (500);
					if (ov9655_read_reg(ArgReg.Registre,&ArgReg.ValeurRead)) {	
						copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
					}
					ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
				}else
					return -EINVAL;
				
			}else
				return -EINVAL;
			
		}
		break;
	case REG_R_OV9655_AEC:
		{
			int mclk=24000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				u16 ValeurRead=0;
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);

				ArgReg.ValeurWrite=0;				
				if (ov9655_get_aec(&ValeurRead)>=0) {
					ArgReg.ValeurRead=ValeurRead;
					copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
				}else
					return -EINVAL;
				
				ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
			}else
				return -EINVAL;			
				
		}
		break;
	case REG_W_OV9655_AEC:
		{
			int mclk=24000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);
				
				if (ov9655_set_aec ((u16)ArgReg.ValeurWrite)>=0) {
					u16 ValeurRead=0;
					msleep (500);
					if (ov9655_get_aec(&ValeurRead)>=0) {
						ArgReg.ValeurRead=ValeurRead;
						copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
					}
					ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
				}else {
					ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
					return -EINVAL;
				}
				
			}else
				return -EINVAL;
			
		}
		break;
		case REG_R_OV9655_AGC:
		{
			int mclk=24000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				u16 ValeurRead=0;
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);

				ArgReg.ValeurWrite=0;				
				if (ov9655_get_agc(&ValeurRead)>=0) {
					ArgReg.ValeurRead=ValeurRead;
					copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
				}else
					return -EINVAL;
				
				ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
			}else
				return -EINVAL;			
				
		}
		break;
	case REG_W_OV9655_AGC:
		{
			int mclk=24000000;
			set_mclk_rate(&mclk);
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				ipu_csi_enable_mclk(CSI_MCLK_I2C, true, true);
				
				if (ov9655_set_agc ((u16)ArgReg.ValeurWrite)>=0) {
					u16 ValeurRead=0;
					msleep (500);
					if (ov9655_get_agc(&ValeurRead)>=0) {
						ArgReg.ValeurRead=ValeurRead;
						copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
					}
					ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
				}else {
					ipu_csi_enable_mclk(CSI_MCLK_I2C, false, true);
					return -EINVAL;
				}
				
			}else
				return -EINVAL;
			
		}
		break;
#endif

#if (defined (CONFIG_SMSC91XX) || defined(CONFIG_SMSC91XX_MODULE))

	case REG_R_SMSC9115 :
		{
		if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
		
				ArgReg.ValeurWrite=0;
				

				ArgReg.ValeurRead= Lan_GetRegDW(ArgReg.Registre);
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
				
			}else
				return -EINVAL;

		}
		break;
	case REG_W_SMSC9115:
		{			
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				
				Lan_SetRegDW(ArgReg.Registre,ArgReg.ValeurWrite);
				ArgReg.ValeurRead= Lan_GetRegDW(ArgReg.Registre);
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
				
			}else
				return -EINVAL;
		}
		break;
	case REG_R_SMSC9115_MAC:
		{			
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				ArgReg.ValeurWrite=0;
				ArgReg.ValeurRead= Extern_Mac_GetRegDW(ArgReg.Registre);
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
				
			}else
				return -EINVAL;
		}
		break;

		case REG_W_SMSC9115_MAC:
		{			
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {				
				Extern_Mac_SetRegDW(ArgReg.Registre,ArgReg.ValeurWrite);
				ArgReg.ValeurRead= Extern_Mac_GetRegDW(ArgReg.Registre);
			}else
				return -EINVAL;
		}
		break;
#endif
	case STIM_PM_SUSPEND:
		{
			pm_suspend(PM_SUSPEND_MEM);
		}
		break;
	case STIM_PM_RESUME:
		{
		}
		break;

#if defined (CONFIG_USB)
#if 0
	case ISP1504_SET:
		{
			printk ("ISP1504_SET\n");
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				
				isp1504_set(ArgReg.ValeurWrite,ArgReg.Registre,&UOG_ULPIVIEW);
				ArgReg.ValeurRead= isp1504_read(ArgReg.Registre,&UOG_ULPIVIEW);
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
				
			}else
				return -EINVAL;
		}
		break;
	case ISP1504_CLEAR :
		{
			printk ("ISP1504_CLEAR\n");
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				
				isp1504_clear(ArgReg.ValeurWrite,ArgReg.Registre,&UOG_ULPIVIEW);
				/*ArgReg.ValeurRead= isp1504_read(ArgReg.Registre,&UOG_ULPIVIEW);
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));
				*/
				
			}else
				return -EINVAL;
		}
		break;
	case ISP1504_READ:
		{
			printk ("ISP1504_READ\n");
			if (copy_from_user (&ArgReg,(void __user *)arg,sizeof (struct StimDebugReg))==0) {
				
				ArgReg.ValeurWrite=0;
				ArgReg.ValeurRead= isp1504_read(ArgReg.Registre,&UOG_ULPIVIEW);
				copy_to_user ((void __user *)arg,&ArgReg,sizeof (struct StimDebugReg));				
				
			}else
				return -EINVAL;
		}
		break;
#endif
#endif
	default :
		return -EINVAL;
		break;
	}

	return 0;
}

static int regdebug_open(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();
	
	unlock_kernel();	
	return ret;
}


static int regdebug_release(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();
	
	
	unlock_kernel();
	return ret;
}

//EXPORT_NO_SYMBOLS;

/***************************************************************************
 *
 * Initialize the driver - Register the character device
 */
static __init int regdebug_init(void)
{
	struct class_device *temp_class;

	printk (KERN_INFO "acces au registre imx + perpih\n driver version " _VERSION "\n");

	/*
	 * Register the driver as a character device, for applications
	 * to access it for ioctls.
	 * First argument (major) to register_chrdev implies a dynamic
	 * major number allocation.
	 */
	major = register_chrdev(0,regdebug_name,&regdebug_fops);
	if (!major) {
		printk("regdebug_init: failed to register char device\n");		
		goto  err;
	}else
		printk("acces au registre imx MAJOR=%d\n",major);

	regdebug_class = class_create(THIS_MODULE, regdebug_name);
	if (IS_ERR(regdebug_class)) {
		printk("Error creating regdebug class.\n");
		goto err1;
	}

	temp_class = device_create(regdebug_class, NULL,MKDEV(major, 0),NULL, regdebug_name);
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating regdebug_init class device.\n");		
		goto err2;
	}

	return 0;
err3:
	device_destroy(regdebug_class, MKDEV(major, 0));
err2:
	class_destroy(regdebug_class);
err1:
	unregister_chrdev (major,regdebug_name);
	major=0;
err:
	return -1;
}

static void __exit regdebug_exit(void) {
	if (major) {
		device_destroy(regdebug_class, MKDEV(major, 0));
		class_destroy(regdebug_class);
		unregister_chrdev (major,regdebug_name);
		major=0;		
	}
	debugk ("acces au registre imx + perpih de-registered\n");
}

MODULE_DESCRIPTION("acces au registre imx + perpih driver version 0.0.1");
MODULE_AUTHOR("Lionel");
MODULE_LICENSE("GPL");

module_init(regdebug_init);
module_exit(regdebug_exit);
