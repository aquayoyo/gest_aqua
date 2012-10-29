/*******************************************************************************
*
*  Implements an interface for i2c compatible PIC18Fs to run under linux.
*  Supports  16k 32K. Uses adaptive timing adjustents
*
* Parametres
* 	mode					mode=0 (normal tempo usec_delay_writecycles),
*								 =1 (PIC18F_ADAPTIVE_TIMING) 2=(wait ack).
*
*	usec_delay_writecycles		tempo d'attente pour l'ecriture d'une page.
*
*	size						taille de l'PIC18F en kilo octets.
*
*	sequential_write_pagesize	taille de la page en octets.
*
*	addr						adresse de l'PIC18F.
*
*	retry						Nombre de tentatives d'ecriture sur l'PIC18F.
*
*  Probing results:
*
*  Test
*   mknod /dev/PIC18F c 122 0;
*   insmod ./PIC18F.ko
*
*-------------------------------------------------------------
*  HISTORY
*
*  DATE          NAME              CHANGES
*  ----          ----              -------
*  Mai  28 2003  Pascal    Initial Version
*
*  Revision 1.0  2003/05/28
*
*
*        (c) 2003 Stim .SA
******************************************************************************/
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
#include <linux/major.h>
/*
 * For character devices
 */
#include <linux/fs.h>		/* character device definitions	*/

#include <mach/gpio.h>
#include <mach/hardware.h>
#include <mach/mx31_pins.h>
#include <mach/pic18f.h>
#include <linux/pmic_external.h>


extern void InitPic18f ();
extern void UnInitPic18f();
#define IO_SDATA_MASK		MX31_PIN_SCLK0
#define IO_SCLK_MASK		MX31_PIN_GPIO1_6
#define IO_PGM__MASK		MX31_PIN_SD1_DATA1
#define IO_VPP_MASK			MX31_PIN_DTR_DCE1		
#define IO_SDATA_IN_MASK	MX31_PIN_SCLK0

/* USER CONFIGURATION 
 * 
 * Set SDATA_BIT, SCLK_BIT, etc to the appropriate bits of the data port. 
 * Set SDATA_IN_BIT to the appropriate bit of the status port.  Active low
 * bits are inverted automatically in software, so you can use those, too. 
 * For reference, the parallel port pins are listed here.
 *
 * PIN	PORT BIT	NAME
 * 2	data 0
 * 3	data 1
 * 4	data 2
 * 5	data 3
 * 6	data 4
 * 7	data 5
 * 8	data 6
 * 9	data 7
 * 10	status 6	ACK
 * 11	status 7	BUSY
 * 12	status 5	PE
 * 13	status 4	SLCT
 * 15	status 3	ERROR
 */
#define SDATA_BIT 0	/* data port */
#define SCLK_BIT  1	/* data port */
#define PGM_BIT   2	/* data port */
#define VPP_BIT   3	/* data port */
#define VDD_BIT   2	/* data port */
#define TS_BIT    4	/* data port */
#define SDATA_IN_BIT 6	/* status port */
/* Don't touch anything below this line.
 */
#define SDATA_MASK (1<<SDATA_BIT)
#define SCLK_MASK  (1<<SCLK_BIT)
#define PGM_MASK   (1<<PGM_BIT)
#define VPP_MASK   (1<<VPP_BIT)
#define VDD_MASK   (1<<VDD_BIT)
#define TS_MASK    (1<<TS_BIT)	// TS=1 Turn SDATA high impedance on (input mode).
#define SDATA_IN_MASK (1<<SDATA_IN_BIT)

#define DATA_INVERT	((SDATA_INVERT<<SDATA_BIT) | (SCLK_INVERT<<SCLK_BIT) \
	| (PGM_INVERT<<PGM_BIT) | (VPP_INVERT<<VPP_BIT) \
	| (VDD_INVERT<<VDD_BIT) | (TS_INVERT<<TS_BIT))
#define STATUS_INVERT	(SDATA_IN_INVERT<<SDATA_IN_BIT)

static unsigned int EtatOut = 0;
static int major=0;
static struct class *PIC18F_class;


static int  PIC18F_open(struct inode * inode, struct file * file);
static int  PIC18F_close(struct inode * inode, struct file * file);
static int	PIC18F_ioctl (struct inode * inode, struct file * file, uint cmd, unsigned long arg);

static const char PIC18F_name[] = "pic18f";


/* This is the exported file-operations structure for this device. */
static const struct file_operations PIC18F_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= PIC18F_ioctl,
	.open		= PIC18F_open,
	.release	= PIC18F_close
};

/* PIC18F init call. Probes for different PIC18F models. */

int __init PIC18F_init(void)
{
	struct class_device *temp_class;
	/*
	 * Register the driver as a character device, for applications
	 * to access it for ioctls.
	 * First argument (major) to register_chrdev implies a dynamic
	 * major number allocation.
	 */
	major =register_chrdev(0, PIC18F_name, &PIC18F_fops);
	if (!major) {
    		printk(KERN_INFO "%s: unable to get major for PIC18F device\n",
      		PIC18F_name);
    		goto  err;
  	}

	PIC18F_class = class_create(THIS_MODULE, PIC18F_name);
	if (IS_ERR(PIC18F_class)) {
		printk("Error creating regdebug class.\n");
		goto err1;
	}

	temp_class = device_create(PIC18F_class, NULL,MKDEV(major, 0),NULL, PIC18F_name);
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating regdebug_init class device.\n");		
		goto err2;
	}

	pmic_write_reg(REG_LED_CONTROL_2, 7, PMIC_ALL_BITS);
	pmic_write_reg(REG_LED_CONTROL_2, 0x0e00, 0x1e00);
	pmic_write_reg(REG_LED_CONTROL_0, 0x1, 0x1);
	mdelay(1000);
	InitPic18f ();
	mxc_set_gpio_dataout(IO_SDATA_MASK, 0);	// mettre les bits a zero
	mxc_set_gpio_dataout(IO_SCLK_MASK, 0);	// mettre les bits a zero
	mxc_set_gpio_dataout(IO_PGM__MASK, 0);	// mettre les bits a zero
	mxc_set_gpio_dataout(IO_VPP_MASK, 1);	// mettre a 1 inversion sur PWC


  	printk("PIC 16F6X2X/8X2X char device v1.0, (c) 2010 STim\n");

	return 0;
err3:
	device_destroy(PIC18F_class, MKDEV(major, 0));
err2:
	class_destroy(PIC18F_class);
err1:
	unregister_chrdev (major,PIC18F_name);
	major=0;
err:
	return -1;
}

void __exit PIC18F_exit(void)
{
	/*
     * Unregister the device
     */

	if (major) {
		UnInitPic18f ();
		
		pmic_write_reg(REG_LED_CONTROL_2, 0, PMIC_ALL_BITS);
		pmic_write_reg(REG_LED_CONTROL_0, 0x0, 0x1);
		
		device_destroy(PIC18F_class, MKDEV(major, 0));
		class_destroy(PIC18F_class);
		unregister_chrdev (major,PIC18F_name);
		
		major=0;		
	}    
}

/* Opens the device. */

static int PIC18F_open(struct inode * inode, struct file * file) {
	EtatOut = 0;
	return 0;
}


/* Closes the device. */

static int PIC18F_close(struct inode * inode, struct file * file)
{
	mxc_set_gpio_dataout(IO_SDATA_MASK, 0);	// mettre les bits a zero
	mxc_set_gpio_dataout(IO_SCLK_MASK, 0);	// mettre les bits a zero
	mxc_set_gpio_dataout(IO_PGM__MASK, 0);	// mettre les bits a zero
	mxc_set_gpio_dataout(IO_VPP_MASK, 1);	// mettre a 1 inversion sur PWC

	return 0;
}


static int PIC18F_ioctl (struct inode * inode, struct file * file,
                         uint cmd, unsigned long arg)
{
	int err;
    unsigned int param = 0;
	unsigned long flags;

	/*
     * Do as much checks as possible from the command definition.
     */
    if (_IOC_TYPE(cmd) != PIC18F_MAGIC) return -ENOTTY;

	local_irq_save(flags);

    switch (cmd) {

	case PIC18F_READ_DATA:
		{	
			param = EtatOut;
			
			/*param = 0;
			if (mxc_get_gpio_datain (IO_SDATA_MASK))
				param |= SDATA_MASK;
			if (mxc_get_gpio_datain (IO_SCLK_MASK))
				param |= SCLK_MASK;
			if (mxc_get_gpio_datain (IO_PGM__MASK))
				param |= PGM_MASK;
			if (mxc_get_gpio_datain (IO_VPP_MASK))
				param |= VPP_MASK;
				*/
		}
		break;

	case PIC18F_WRITE_DATA:
		{
			if ((err = get_user(param, (unsigned int *)arg)) != 0)
				return err;
			
			
			if (param & SDATA_MASK)		mxc_set_gpio_dataout(IO_SDATA_MASK,1);  else mxc_set_gpio_dataout(IO_SDATA_MASK,0);
			if (param & SCLK_MASK)		mxc_set_gpio_dataout(IO_SCLK_MASK,1);  else mxc_set_gpio_dataout(IO_SCLK_MASK,0);
			if (param & PGM_MASK)		mxc_set_gpio_dataout(IO_PGM__MASK,1);  else mxc_set_gpio_dataout(IO_PGM__MASK,0);
			if (param & VPP_MASK)		mxc_set_gpio_dataout(IO_VPP_MASK,0);  else mxc_set_gpio_dataout(IO_VPP_MASK,1);
			
			if (param & TS_MASK)
			{
				mxc_set_gpio_direction(IO_SDATA_IN_MASK, 1);	//1 = input		
			}else 
			{
				mxc_set_gpio_direction(IO_SDATA_IN_MASK, 0);	//0 = output
			}
			EtatOut=param;
		}
		break;

	case PIC18F_READ_STAT:
		{
			int data1;
			
			param = 0;
			data1 = mxc_get_gpio_datain (IO_SDATA_IN_MASK);			
			if (data1)		
				param |= SDATA_IN_MASK;
		}

		break;

	case PIC18F_WRITE_STAT:
		

		break;
	}
	
	local_irq_restore(flags);
	//printk("cmd=%x param=%x\n",cmd,param);
    return param;
}

MODULE_DESCRIPTION("Driver PIC18F char device v1.0, (c) 2010 STim");
MODULE_AUTHOR("Pascal/Lionel");
MODULE_SUPPORTED_DEVICE("pic18f");
MODULE_LICENSE("GPL");

module_init(PIC18F_init);
module_exit(PIC18F_exit);
