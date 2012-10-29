/*
 *  dlgi2c.c
 *
 *  Copyright (C) 2008 STim
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Driver for dialogue i2c avec la carte PowerCam
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
#include <linux/delay.h>
#include <linux/rtc.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/jiffies.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <mach/gpio.h>
#include <mach/iomux-mx3.h>

#include <mach/dlgi2c.h>
/***************************************************************************
 * Global stuff
 ***************************************************************************
 */
#undef DEBUG
//#define DEBUG 0

#define DEBUG_TR_VIE 0

#define MXC_I2C_FLAG_READ 0x1

#if DEBUG_TR_VIE
	#define TIMER_TRAME_VIE   (1 * HZ)/10
#else
	#define TIMER_TRAME_VIE   (1 * HZ)
#endif
#define TIMER_ETAT_BOUTON HZ/100

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
static const char i2c_PowerCam_name[] = "i2c_PowerCam";

#define _VERSION	"1.0.20 STim"
#define VERSION_STR "Dialogue i2c carte PowerCam driver version : "_VERSION

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

#define I2C_READ 0x1
#define I2C_WRITE 0x2


#define TAILLE_BUF 10
#define TAILLE_BUF_W 20
#define TAILLE_BUF_R 2

struct  _i2c_PowerCam_client {
	struct i2c_client *client;
	wait_queue_head_t wait_q;

	struct semaphore Monlock;
	spinlock_t			lock;

	char is_open;

	char mode;
	char buf_read [TAILLE_BUF] [TAILLE_BUF_R];
	char index_bufr [2];
	int data_on;

	char buf_write [TAILLE_BUF] [TAILLE_BUF_W+1]; //taille donne sur 1 octet + donnees;
	char index_bufw [2];
	unsigned char ucIntValide;
	unsigned int uiCptClear;
	unsigned int uiOldCptClear;

	struct timer_list stTimerTrameVie;
	unsigned char ucValeurTrameVie;
	unsigned char ucNbIntZero;

	unsigned char ucEtatBoutonG;

}i2c_PowerCam_client;

/* avoid multiple access */
static int major=0;
static struct class *i2c_PowerCam_class;


static struct i2c_driver i2c_PowerCam_driver;


static unsigned short ignore[] = { I2C_CLIENT_END };
static unsigned short normal_addr[] = { 0x10, I2C_CLIENT_END }; // carte PowerCam sur le bus 1

static struct i2c_client_address_data addr_data = {
	.normal_i2c	= normal_addr,
	.probe		= ignore,
	.ignore		= ignore,
};


extern int In2_IRQ_I2C_2(void);


static irqreturn_t I2c_irq (int irq, void *devid);
static irqreturn_t IrqGpio1_2 (int irq, void *devid);

static int PowerCam_read_i2c (struct i2c_client *client,u16 Addr, u8 *data, int taille);
static int PowerCam_write_i2c (struct i2c_client *client, u8 *data, int taille);

static ssize_t i2c_PowerCam_Send (struct _i2c_PowerCam_client *client_i2c,char *buf,size_t nbytes);
static int SendCmd (struct i2c_client *client,unsigned char ucCmd,unsigned char ucAdd,unsigned char ucData);

static void I2c_interrupt_task(struct work_struct *);
static DECLARE_WORK(I2c_int_task, I2c_interrupt_task);
static struct workqueue_struct *i2c_wq=NULL;

static void I2c_TrameVie_task(struct work_struct *);
static DECLARE_WORK(I2c_trVie_task, I2c_TrameVie_task);
static struct workqueue_struct *i2c_TrameVie_wq=NULL;


static ssize_t i2c_PowerCam_read(struct file * file, char __user * buf, size_t nbytes, loff_t *off);
static ssize_t i2c_PowerCam_write(struct file *file, const char __user *buf, size_t nbytes, loff_t *off);
static int i2c_PowerCam_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg);
static int i2c_PowerCam_open(struct inode * inode, struct file * file);
static int i2c_PowerCam_release(struct inode * inode, struct file * file);
static int i2c_PowerCam_fasync(int fd, struct file *file, int on);
static unsigned int i2c_PowerCam_poll(struct file *file, poll_table *wait);

static short DataIn (struct _i2c_PowerCam_client *client_i2c,unsigned char data1,unsigned char data2);

static void fctTimerTrameVie (unsigned long data);
static void ClearIntPowerCam (struct _i2c_PowerCam_client *client_i2c,unsigned char ucStatusInt);

static const struct file_operations i2c_PowerCam_fops = {
	.owner		= THIS_MODULE,
	.read		= i2c_PowerCam_read,
	.write		= i2c_PowerCam_write,
	.ioctl		= i2c_PowerCam_ioctl,
	.open		= i2c_PowerCam_open,
	.release	= i2c_PowerCam_release,
	.fasync		= i2c_PowerCam_fasync,
	.poll		= i2c_PowerCam_poll,
};


extern void gpio_free_irq(u32 port, u32 sig_no,  void *dev_id);
extern void gpio_clear_int(u32 port, u32 sig_no);
extern int gpio_request_irq(__u32 port, __u32 sig_no, enum gpio_prio prio,
							gpio_irq_handler handler, __u32 irq_flags,const char *devname, void *dev_id);
/****************************************************************************/


static inline unsigned char check_I2C(unsigned char *data, short length)
{
	short i;
	unsigned char s = 0x5A;
	
	for (i=0; i < length; i++) s ^= data[i]; 
	return(s);
}
static short DataIn (struct _i2c_PowerCam_client *client_i2c,unsigned char data1,unsigned char data2) {
	short ret=0;
	if (client_i2c) {
		int oldindex_bufr=client_i2c->index_bufr [0];
		if (client_i2c->index_bufr [0]>=(TAILLE_BUF-1))
			client_i2c->index_bufr [0]=0;
		else
			client_i2c->index_bufr [0]++;
		
		if (client_i2c->index_bufr [0]!=client_i2c->index_bufr [1]) {
			client_i2c->data_on=1;
			client_i2c->buf_read [(short)client_i2c->index_bufr [0]] [0]=(char)data1;
			client_i2c->buf_read [(short)client_i2c->index_bufr [0]] [1]=(char)data2;
		}else {
			debugk ("DataIn erreur index buffer [0] [1] = %d\t%d\n",client_i2c->index_bufr [0],client_i2c->index_bufr [1]);
			client_i2c->index_bufr [0]=oldindex_bufr; //erreur 
			ret=-2;
		}

	}else
		ret=-1;

	return ret;
}

static void I2c_TrameVie_task(struct work_struct *param)  {
	struct _i2c_PowerCam_client *client_i2c=&i2c_PowerCam_client;
	char err=0;
	unsigned char ucCpt=3;
	if (client_i2c) {
		//printk ("1-->I2c_TrameVie_task\n");
		down(&client_i2c->Monlock);
		//printk ("2-->I2c_TrameVie_task\n");
		do {
			ucCpt--;
			// envoie trame de vie 
			if (SendCmd (client_i2c->client,CMD_W_TRAME_VIE,TRAME_VIE,client_i2c->ucValeurTrameVie)>=0) {
				unsigned char ucValeurTrameVieRecue=0;
				// reception du complement à 1 de la trame envoyée
				if (PowerCam_read_i2c (client_i2c->client,CMD_R_TRAME_VIE,&ucValeurTrameVieRecue,1)>=0) {
					//printk ("tr vie=%02x \t tr vie rec=%02x \t tr vi cp=%02x jiffies=%x\n",client_i2c->ucValeurTrameVie,ucValeurTrameVieRecue,~ucValeurTrameVieRecue,jiffies);
					if ((client_i2c->ucValeurTrameVie&0x7F)==((~ucValeurTrameVieRecue)&0x7F)) {
						// le complement est correct
						client_i2c->ucValeurTrameVie++;
						if (client_i2c->ucValeurTrameVie>=0x80)
							client_i2c->ucValeurTrameVie=0;

						if ((ucValeurTrameVieRecue&0x80)==0) {
							// l'interuption est au niveau bas ATTENTION peut être probleme
							// au bout de dix consecutifs on clear les it
							if (client_i2c->ucNbIntZero==0) {
								client_i2c->uiOldCptClear=client_i2c->uiCptClear;
								client_i2c->ucNbIntZero++;
							}else {								
								if (client_i2c->uiCptClear == client_i2c->uiOldCptClear) {
									client_i2c->ucNbIntZero++;		
									if (client_i2c->ucNbIntZero>=10) {
										printk ("Erreur interruption non clear\n");
										ClearIntPowerCam (client_i2c,0);
										client_i2c->ucNbIntZero=0;
									}
								}else
									client_i2c->ucNbIntZero=0;
							}
						}else
							client_i2c->ucNbIntZero=0;
					}else {
						err=-1;
						printk ("Erreur trame de vie recue est incorrecte\n");
					}
				}else {
					err=-2;
					printk ("Erreur lors de la reception de la trame de vie\n");
				}
			}else {
				err=-3;
				printk ("Erreur lors de la transmission de la trame de vie\n");
			}
		}while (err!=0 && ucCpt>0);

		if (err<0) {
			// probleme trame de vie avec carte PowerCam 
			// il faut envoyer l'info à l'application
			printk ("probleme trame de vie avec carte PowerCam\n");
			DataIn (client_i2c,ALARME_HARD,PB_TRAME_VIE);
		}

		up(&client_i2c->Monlock);
	}else
		err=-4;

	client_i2c->stTimerTrameVie.expires = jiffies + TIMER_TRAME_VIE;
	add_timer(&client_i2c->stTimerTrameVie);
}


static void I2c_interrupt_task(struct work_struct *param)  {

	struct _i2c_PowerCam_client *client_i2c=&i2c_PowerCam_client;

	down(&client_i2c->Monlock);
	if (client_i2c->mode==I2C_READ) {		
		unsigned char ucStatusInt;
		unsigned char ucCptReadStatus=3;
		unsigned char ucRetReadStatus=0;


		debugk ("traitement irq i2c en mode lecture\n");
		//msleep (10);		
		// traitement intteruption 
		// lire status int
		do {
			ucCptReadStatus--;
			ucRetReadStatus=PowerCam_read_i2c (client_i2c->client,CMD_R_STATUS_INT,&ucStatusInt,1);
		}while (ucRetReadStatus!=0 && ucCptReadStatus>0);
		
		if (ucRetReadStatus!=-1) {
			//printk ("ucStatusInt recu=%d\n",ucStatusInt);
			if (ucStatusInt) {
				unsigned char ucCpt;
				
				for (ucCpt=(sizeof (unsigned char)*8)-1;ucCpt>0;ucCpt--) {					
					if ((ucStatusInt & (1<<ucCpt-1))) {
						if (DataIn (client_i2c,SET_INT,1<<(ucCpt-1))<0) {
							// erreur							
							ucStatusInt&=~(1<<(ucCpt-1));
						}
					}
				}
			}
			//printk ("ucStatusInt clear=%d\n",ucStatusInt);
			ClearIntPowerCam (client_i2c,ucStatusInt);
		}else {
			printk ("erreur lecture status\n");
			ClearIntPowerCam (client_i2c,0);
		}

	}else if (client_i2c->mode==I2C_WRITE) {
		debugk ("traitement irq i2c en mode ecriture\n");
		int oldindex_bufw=client_i2c->index_bufw [1];
		if (client_i2c->index_bufw [0]!=client_i2c->index_bufw [1]) {
			if (client_i2c->index_bufw [1]>=(TAILLE_BUF-1))
				client_i2c->index_bufw [1]=0;
			else
				client_i2c->index_bufw [1]++;

			debugk ("index buffer [0] [1] = %d\t%d\n",client_i2c->index_bufw [0],client_i2c->index_bufw [1]);

			if (PowerCam_write_i2c (client_i2c->client,&client_i2c->buf_write [(short)client_i2c->index_bufw [1]][1],client_i2c->buf_write [(short)client_i2c->index_bufw [1]][0])==-1)
				client_i2c->index_bufw [1]=oldindex_bufw; //erreur
		}
	}
	wake_up_interruptible (&client_i2c->wait_q);

	up(&client_i2c->Monlock);

}

static void fctTimerTrameVie (unsigned long data) {
	//printk ("fctTimerTrameVie %x\n",jiffies);
	queue_work(i2c_TrameVie_wq, &I2c_trVie_task);
}

static void ClearIntPowerCam (struct _i2c_PowerCam_client *client_i2c,unsigned char ucStatusInt) {
	unsigned char dataI2c[5];

	dataI2c[0] = CMD_W_INT;		// cmd;
	dataI2c[1] = 3;	// taille;
	dataI2c[2]=CLEAR_INT;
	dataI2c[3]=ucStatusInt;	
	dataI2c[4] = check_I2C((unsigned char *)&dataI2c, 4);	

	if (PowerCam_write_i2c (client_i2c->client,dataI2c,5)<0)
		printk ("Erreur : probleme pour clear les Interuptions\n");
	else
		client_i2c->uiCptClear++;
}


static int SendCmd (struct i2c_client *client,unsigned char ucCmd,unsigned char ucAdd,unsigned char ucData) {
	unsigned char dataI2c[5];

	dataI2c[0] = ucCmd;		// cmd;
	dataI2c[1] = 3;	// taille;
	dataI2c[2]=ucAdd;
	dataI2c[3]=ucData;	
	dataI2c[4] = check_I2C((unsigned char *)&dataI2c, 4);	
	return PowerCam_write_i2c (client,dataI2c,5);	
}


static irqreturn_t I2c_irq (int irq, void *devid) {
	struct _i2c_PowerCam_client *client_i2c = devid;
	// la carte sohrea veut communiquer avec nous.	

	spin_lock(&client_i2c->lock); 
	//printk ("I2c_irq irq=%d\t%08x\n",irq,__raw_readl(IO_ADDRESS(0x53fcc018)));	
	//__raw_readl(IO_ADDRESS(0x53fcc018));
	gpio_clear_int (0,3);
	client_i2c->mode=I2C_READ;	
	queue_work(i2c_wq, &I2c_int_task);	
	spin_unlock(&client_i2c->lock); 

	return IRQ_HANDLED;
}
int iCptPoll=0;
static void FctEtatBoutonG(unsigned long arg);
static unsigned char GetEtatBoutonG	(struct _i2c_PowerCam_client *client_i2c);
static DEFINE_TIMER(TimerEtatBoutonG, FctEtatBoutonG, 0, &i2c_PowerCam_client);

static void FctEtatBoutonG(unsigned long arg) {
	struct _i2c_PowerCam_client *client_i2c = arg;
	if (client_i2c) {
		if (GetEtatBoutonG (client_i2c)) {
			//revalide l'interuption
			gpio_request_irq (0,2,GPIO_LOW_PRIO,&IrqGpio1_2,IRQF_DISABLED|IRQF_TRIGGER_LOW,i2c_PowerCam_name,client_i2c);
			DataIn (client_i2c,ALARME_HARD,BOUTON_G_FACE_AVANT);
			wake_up_interruptible (&client_i2c->wait_q);
		}else {
			if (!iCptPoll) {
				DataIn (client_i2c,ALARME_HARD,BOUTON_G_FACE_AVANT);			
				wake_up_interruptible (&client_i2c->wait_q);
			}
			mod_timer(&TimerEtatBoutonG, jiffies + TIMER_ETAT_BOUTON);
			iCptPoll++;
		}
	}
}

static unsigned char GetEtatBoutonG	(struct _i2c_PowerCam_client *client_i2c) {
	if (client_i2c) {
		int iReg=0;		
		iReg=__raw_readl(IO_ADDRESS(0x53fcc000)); //gpio_1 data
		client_i2c->ucEtatBoutonG=(iReg>>2)&1;
		//printk ("GetEtatBoutonG %d\n",client_i2c->ucEtatBoutonG);		
	}else
		return 0xff;

	return client_i2c->ucEtatBoutonG;
}

static irqreturn_t IrqGpio1_2 (int irq, void *devid) {
	struct _i2c_PowerCam_client *client_i2c = devid;
	if (client_i2c) {
		spin_lock(&client_i2c->lock);
		gpio_clear_int(0,2);
		gpio_free_irq (0,2,client_i2c); // devalide l'interuption	
		iCptPoll=0;
		mod_timer(&TimerEtatBoutonG, jiffies + TIMER_ETAT_BOUTON);
		spin_unlock(&client_i2c->lock);	
	}
	return IRQ_HANDLED;
}

static int PowerCam_read_i2c (struct i2c_client *client,u16 Addr, u8 *data, int taille)
{
	if (client) {
		char cCmdWrite [3];
		cCmdWrite [0]=(unsigned char)(Addr>>8);
		cCmdWrite [1]=1;
		cCmdWrite [2]=(unsigned char)Addr;		
		if (i2c_master_send (client, &cCmdWrite [0], 3) >= 0) {			
			if (i2c_master_recv (client,data,taille) < 0) {
				printk("%s : i2c_master_recv error\n",__func__);
				return -1;
			}			
		}else {
			printk("%s : read reg error\n",__func__);
			return -1;
		}
	}else
		return -1;
	return 0;
}

static int PowerCam_write_i2c (struct i2c_client *client, u8 *data, int taille)
{
	if (client) {
		unsigned char ucCpt=3;
		unsigned char ucI2cMasterSend=0;

		do {
			ucCpt--;
			ucI2cMasterSend=i2c_master_send (client, data, taille);
		}while (ucI2cMasterSend<0 && ucCpt>0);

		if (ucI2cMasterSend < 0) {
			printk("%s:write reg error: reg=%x, val=%x\n",  __func__, data[0], data[1]);
			return -1;
		}
	}else
		return -1;
	
	return 0;
}
static int
i2cPowerCam_attach(struct i2c_adapter *adap, int addr, int kind)
{
	int rc;
	int id = adap->id;
	int cpt=0;
	unsigned char ucStatusInt;
	unsigned char ucPicVersion;


	if (id != 1)
		return -ENOMEM;
	
	i2c_PowerCam_client.client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (!i2c_PowerCam_client.client)
		return -ENOMEM;	

	memset(i2c_PowerCam_client.client, 0, sizeof(struct i2c_client));	
	i2c_set_clientdata(i2c_PowerCam_client.client, NULL);
	
	strlcpy(i2c_PowerCam_client.client->name, i2c_PowerCam_name, I2C_NAME_SIZE);
	
	i2c_PowerCam_client.client->flags        = 0;
	i2c_PowerCam_client.client->addr         = addr;
	i2c_PowerCam_client.client->adapter      = adap;
	i2c_PowerCam_client.client->driver       = &i2c_PowerCam_driver;	
	if ((rc = i2c_attach_client(i2c_PowerCam_client.client)))
	{
		kfree(i2c_PowerCam_client.client);
		i2c_PowerCam_client.client = NULL;
		return rc;
	}else { // le peripherique esclave est présent
		
		PowerCam_read_i2c (i2c_PowerCam_client.client,CMD_R_VERSION_PIC,&ucPicVersion,1);
		printk ("Version Pic PowerCam : %d\n",ucPicVersion);

		SendCmd (i2c_PowerCam_client.client,CMD_W_LED,LED_PUISSANCE,LED_OFF);
				
		// on initialise l'irq qui va être utilisé pour recevoir des commandes de la carte PowerCam		
		mxc_iomux_set_pad(MX31_PIN_GPIO1_3,(PAD_CTL_SRE_SLOW | PAD_CTL_DRV_NORMAL| PAD_CTL_PKE_NONE | PAD_CTL_HYS_SCHMITZ));
		rc = gpio_request_irq (0,3,GPIO_LOW_PRIO,&I2c_irq,IRQF_TRIGGER_FALLING,i2c_PowerCam_name,&i2c_PowerCam_client);
		if (rc) {
			i2c_detach_client(i2c_PowerCam_client.client);
			kfree(i2c_PowerCam_client.client);
			i2c_PowerCam_client.client = NULL;
			return rc;
		}		

		i2c_PowerCam_client.uiCptClear=0;
		i2c_PowerCam_client.uiOldCptClear=0;
		ClearIntPowerCam (&i2c_PowerCam_client,0);
		
		init_timer(&i2c_PowerCam_client.stTimerTrameVie);
		i2c_PowerCam_client.stTimerTrameVie.function = fctTimerTrameVie;
		i2c_PowerCam_client.stTimerTrameVie.data = (unsigned long)&i2c_PowerCam_client;
		i2c_PowerCam_client.ucNbIntZero=0;
		i2c_PowerCam_client.ucValeurTrameVie=0;

		i2c_PowerCam_client.ucEtatBoutonG=(__raw_readl(IO_ADDRESS(0x53fcc000))>>2)&1;

		if (i2c_wq)
			flush_workqueue(i2c_wq);
		i2c_wq = create_workqueue(i2c_PowerCam_name);

		if (i2c_TrameVie_wq)
			flush_workqueue(i2c_TrameVie_wq);
		i2c_TrameVie_wq = create_workqueue("WQ_TrameVie");

		init_waitqueue_head(&i2c_PowerCam_client.wait_q);
		init_MUTEX(&i2c_PowerCam_client.Monlock);
		spin_lock_init(&i2c_PowerCam_client.lock);


		/**************** init ***********************************/
		for (cpt=0;cpt<TAILLE_BUF;cpt++) {
			memset (i2c_PowerCam_client.buf_read [cpt],0,TAILLE_BUF_R);
			memset (i2c_PowerCam_client.buf_write [cpt],0,TAILLE_BUF_W+1);
		}
		memset (i2c_PowerCam_client.index_bufr,-1,2);
		memset (i2c_PowerCam_client.index_bufw,-1,2);
		i2c_PowerCam_client.data_on=0;

	}
	
	return 0;
}

static int i2cPowerCam_probe(struct i2c_adapter *adap)
{
	int err;	
	err = i2c_probe(adap, &addr_data, i2cPowerCam_attach);	
	return err;
}

static int i2cPowerCam_detach(struct i2c_client *client)
{
	flush_workqueue(i2c_wq);

	if (i2c_TrameVie_wq)
		flush_workqueue(i2c_TrameVie_wq);

	gpio_free_irq (0,3,&i2c_PowerCam_client);
	gpio_free_irq (0,2,&i2c_PowerCam_client);


	i2c_detach_client(client);
	kfree(client);	
	i2c_PowerCam_client.client=NULL;
	
	return 0;
}


static int i2cPowerCam_suspend (struct i2c_client *client,pm_message_t mesg) {

	//printk ("i2cPowerCam_suspend\n");
	flush_workqueue(i2c_wq);

	if (i2c_TrameVie_wq)
			flush_workqueue(i2c_TrameVie_wq);

	return 0;

}

static struct i2c_driver i2c_PowerCam_driver = {
		.driver = {
        .name         =  	"PowerCam_i2c",
		},
        .attach_adapter		= i2cPowerCam_probe,
        .detach_client		= i2cPowerCam_detach,
		.suspend			= i2cPowerCam_suspend,
};


/*
 *	Read a message from i2c (reception d'un commande de PowerCam)
 */
static ssize_t i2c_PowerCam_read(struct file * file, char __user * buf, size_t nbytes, loff_t *off) {
	struct _i2c_PowerCam_client *client_i2c = file->private_data;	
	if (!client_i2c)
		return -ENODEV;

	
	if (client_i2c->is_open) {
		int cpt=0;
		down(&client_i2c->Monlock);
		//printk ("1-->i2c_PowerCam_read\n");	
		while (!client_i2c->data_on) {
			if (cpt>=3)
				break;
			interruptible_sleep_on_timeout(&client_i2c->wait_q,(20*1000 * HZ) / 1000); // 20s
			cpt++;
		}

		//printk ("2-->i2c_PowerCam_read\n");	
		if (client_i2c->data_on) {
			char oldindex_bufr=client_i2c->index_bufr [1];
			if (client_i2c->index_bufr [0]!=client_i2c->index_bufr [1]) {
				if (client_i2c->index_bufr [1]>=(TAILLE_BUF-1))
					client_i2c->index_bufr [1]=0;
				else
					client_i2c->index_bufr [1]++;

				if (copy_to_user (buf,client_i2c->buf_read [(short)client_i2c->index_bufr [1]],nbytes)) {
					client_i2c->index_bufr [1]=oldindex_bufr;
					up(&client_i2c->Monlock);
					return -EIO;
				}

				if (client_i2c->index_bufr [0]==client_i2c->index_bufr [1])
					client_i2c->data_on=0; // tout a été lu
			}else {
				up(&client_i2c->Monlock);
				return -ENOSPC;
			}
		}
		up(&client_i2c->Monlock);
	}else
		return -ENODEV;
	return nbytes;
}

static ssize_t i2c_PowerCam_write(struct file *file, const char __user *buf, size_t nbytes, loff_t *off) {
	char Buf [TAILLE_BUF_W+1];
	struct _i2c_PowerCam_client *client_i2c = file->private_data;
	if (!client_i2c)
		return -ENODEV;
	if (client_i2c->is_open) {
		if (nbytes<=TAILLE_BUF_W) {
			debugk ("i2c_PowerCam_write 1\n");
			if (copy_from_user (Buf,buf,nbytes)==0) {
				debugk ("i2c_PowerCam_write 2\n"); 
				nbytes=i2c_PowerCam_Send (client_i2c,Buf,nbytes);
				
			}else
				return -EINVAL;
		}else
			return -ENOMEM;
	}else
		return -ENODEV;
	
	return nbytes;
}

static ssize_t i2c_PowerCam_Send (struct _i2c_PowerCam_client *client_i2c,char *buf,size_t nbytes) {
	
	if (!client_i2c)
		client_i2c=&i2c_PowerCam_client;
	
	int oldindex_bufw=client_i2c->index_bufw [0];
	if (client_i2c->index_bufw [0]>=(TAILLE_BUF-1))
		client_i2c->index_bufw [0]=0;
	else
		client_i2c->index_bufw [0]++;
	
	debugk ("index buffer w [0] [1] = %d\t%d\n",client_i2c->index_bufw [0],client_i2c->index_bufw [1]);
	
	if (client_i2c->index_bufw [0]!=client_i2c->index_bufw [1]) {
		if (nbytes<=TAILLE_BUF_W) {
			
			memcpy (&client_i2c->buf_write [(short)client_i2c->index_bufw [0]][1],buf,nbytes);

			client_i2c->buf_write [(short)client_i2c->index_bufw [0]][0]=nbytes;
			
			spin_lock(&client_i2c->lock); 
			client_i2c->mode=I2C_WRITE;			
			queue_work(i2c_wq, &I2c_int_task);				
			spin_unlock(&client_i2c->lock);
		}else {
			debugk ("erreur 2\n");
			client_i2c->index_bufw [0]=oldindex_bufw;
			return -ENOMEM;
		}
	}else {
		debugk ("erreur 3\n");
		client_i2c->index_bufw [0]=oldindex_bufw;
		return -ENOSPC;
	}

	return nbytes;
	
}
EXPORT_SYMBOL (i2c_PowerCam_Send);

static int i2c_PowerCam_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg) {
	int err=0;
	struct DlgImxPowerCam stDlgImxPowerCam;
	struct _i2c_PowerCam_client *client_i2c = filp->private_data;
	unsigned char dataI2c[256];

	if (!client_i2c)
		return -ENODEV;
	down(&client_i2c->Monlock);
	//printk ("1-->i2c_PowerCam_ioctl cmd=%d\n",cmd);
	if (client_i2c->is_open) {
		switch (cmd) {
		case I2C_SET_CMD:
			{			
				if (copy_from_user (&stDlgImxPowerCam,(void __user *)arg,sizeof (struct DlgImxPowerCam))==0) {
					
					dataI2c[0] = (unsigned char)stDlgImxPowerCam.ucCmd;		// cmd;
					dataI2c[1] = stDlgImxPowerCam.ucTaille+1;	// taille;
					if (stDlgImxPowerCam.ucTaille) 
						memcpy(&dataI2c[2], stDlgImxPowerCam.cBuffer, stDlgImxPowerCam.ucTaille);

					dataI2c[stDlgImxPowerCam.ucTaille+2] = check_I2C((unsigned char *)&dataI2c, stDlgImxPowerCam.ucTaille+2);
					
					if (i2c_PowerCam_Send (client_i2c,dataI2c,stDlgImxPowerCam.ucTaille+3)<0)
						printk ("Erreur ecriture registre Pic sur PowerCam\n");
				}else
					err=-EINVAL;
				
			}
			break;
		case I2C_GET_CMD:
			{
				if (copy_from_user (&stDlgImxPowerCam,(void __user *)arg,sizeof (struct DlgImxPowerCam))==0) {
			
					if (PowerCam_read_i2c (client_i2c->client,stDlgImxPowerCam.ucCmd,stDlgImxPowerCam.cBuffer,stDlgImxPowerCam.ucTaille)>=0) {			
						copy_to_user ((void __user *)arg,&stDlgImxPowerCam,sizeof (struct DlgImxPowerCam));						
					}else {
						printk ("Erreur lecture registre Pic sur PowerCam\n");
					}
				}			
			}
			break;
		case I2C_SET_ETAT_LED:
			{
				struct EtatLed stEtatLed;
				if (copy_from_user (&stEtatLed,(void __user *)arg,sizeof (struct EtatLed))==0) {
					if (SendCmd (client_i2c->client,CMD_W_LED,stEtatLed.ucLed,stEtatLed.ucEtat)<0) {
						printk ("Erreur : changement etat led %d échoué\n",stEtatLed.ucLed);
						err=-EINVAL;
					}
				}else
					err=-EINVAL;
			}
			break;
		case I2C_GET_INT:
			{
				stDlgImxPowerCam.ucCmd=((CMD_R_INT<<8)|INT_SET);
				if (PowerCam_read_i2c (client_i2c->client,stDlgImxPowerCam.ucCmd,stDlgImxPowerCam.cBuffer,1)>=0) {			
					copy_to_user ((void __user *)arg,&stDlgImxPowerCam.cBuffer[0],1);						
				}else {
					printk ("Erreur lecture registre Valide_Int sur le Pic de PowerCam\n");
				}
			}
			break;
		case I2C_SET_INT:
			{
				unsigned char Interupt;
				if (copy_from_user (&Interupt,(void __user *)arg,sizeof (unsigned char))==0) {
					//stDlgImxPowerCam.ucCmd=((CMD_R_INT<<8)|INT_SET);
					//if (PowerCam_read_i2c (client_i2c->client,stDlgImxPowerCam.ucCmd,stDlgImxPowerCam.cBuffer,1)>=0) {
					
					if (Interupt==INT_BOUTON) {
						// on initialise l'irq qui va être utilisé pour le bouton de gauche		
						//gpio_request_irq (0,2,GPIO_LOW_PRIO,&IrqGpio1_2,IRQF_DISABLED|IRQF_TRIGGER_LOW,i2c_PowerCam_name,&i2c_PowerCam_client);
						iCptPoll=0;
						mod_timer(&TimerEtatBoutonG, jiffies + HZ);
					}
					
					dataI2c[0] = CMD_W_INT;		// cmd;
					dataI2c[1] = 3;	// taille;
					dataI2c[2]=VALIDE_INT;
					//dataI2c[3]=stDlgImxPowerCam.cBuffer[0]|Interupt;
					dataI2c[3]=client_i2c->ucIntValide|Interupt;
					dataI2c[4] = check_I2C((unsigned char *)&dataI2c, 4);	
					if (PowerCam_write_i2c (client_i2c->client,dataI2c,5)<0) {
						printk ("Erreur : set Interupt %d échoué\n",Interupt);
						err=-EINVAL;
					}else
						client_i2c->ucIntValide=dataI2c[3];
					//}else
					//return -EINVAL;
				}else
					err=-EINVAL;
			}
			break;
		case I2C_DEL_INT:
			{
				unsigned char Interupt;
				if (copy_from_user (&Interupt,(void __user *)arg,sizeof (unsigned char))==0) {
					//stDlgImxPowerCam.ucCmd=((CMD_R_INT<<8)|INT_SET);
					//if (PowerCam_read_i2c (client_i2c->client,stDlgImxPowerCam.ucCmd,stDlgImxPowerCam.cBuffer,1)>=0) {
					if (Interupt==INT_BOUTON) {
						// on supprime l'irq qui est utilisée pour le bouton de gauche		
						gpio_free_irq (0,2,client_i2c); // devalide l'interuption	
					}
					
						if (SendCmd (client_i2c->client,CMD_W_INT,VALIDE_INT,client_i2c->ucIntValide&~Interupt)<0) {
							printk ("Erreur : set Interupt %d échoué\n",Interupt);
							err=-EINVAL;
						}else
							client_i2c->ucIntValide=client_i2c->ucIntValide&~Interupt;
					//}else
					//	return -EINVAL;
				}else
					err=-EINVAL;
			}
			break;
		case I2C_SET_ETAT_BUZZER:
			{
				unsigned char ucEtatBuzzer;
				if (copy_from_user (&ucEtatBuzzer,(void __user *)arg,sizeof (unsigned char))==0) {
					if (SendCmd (client_i2c->client,CMD_W_BUZZER,BUZZER,ucEtatBuzzer)<0) {
						printk ("Erreur : changement etat buzzer échoué\n");
						err=-EINVAL;
					}
				}else
					err=-EINVAL;
			}
			break;

			case I2C_SET_BIP_BUZZER:
			{
				unsigned char ucEtatBouton;
				if (copy_from_user (&ucEtatBouton,(void __user *)arg,sizeof (unsigned char))==0) {
					if (SendCmd (client_i2c->client,CMD_W_BUZZER,BIP_BUZZER,ucEtatBouton)<0) {
						printk ("Erreur : changement etat bouton buzzer échoué\n");
						err=-EINVAL;
					}
				}else
					err=-EINVAL;
			}
			break;
			case I2C_SET_ETAT_RELAIS:
			{
				unsigned char ucEtatRelais;
				if (copy_from_user (&ucEtatRelais,(void __user *)arg,sizeof (unsigned char))==0) {
					if (SendCmd (client_i2c->client,CMD_W_RELAIS,RELAIS,ucEtatRelais)<0) {
						printk ("Erreur : changement etat relais échoué\n");
						err=-EINVAL;
					}
				}else
					err=-EINVAL;
			}
			break;
			case I2C_SET_SEUIL_PIR:
				{
					unsigned char ucSeuilPir;
					if (copy_from_user (&ucSeuilPir,(void __user *)arg,sizeof (unsigned char))==0) {
						if (SendCmd (client_i2c->client,CMD_W_SEUIL_PIR,SEUIL_PIR,ucSeuilPir)<0) {
							printk ("Erreur : changement seuil pir échoué\n");
							err=-EINVAL;
						}
					}else
						err=-EINVAL;
				}
				break;
			case I2C_SET_ALIM:
				{
					unsigned char ucEtatAlim;
					if (copy_from_user (&ucEtatAlim,(void __user *)arg,sizeof (unsigned char))==0) {
						if (SendCmd (client_i2c->client,CMD_W_STOP_ALIM,STOP_ALIM,ucEtatAlim)<0) {
							printk ("Erreur : changement etat alim échoué\n");
							err=-EINVAL;
						}
					}else
						err=-EINVAL;
				}				
				break;
			case I2C_GET_ALIM:
				{
					unsigned char ucAlim=0xff;					
					if (PowerCam_read_i2c (client_i2c->client,CMD_R_POE,stDlgImxPowerCam.cBuffer,1)>=0) {
						ucAlim=stDlgImxPowerCam.cBuffer[0]<<4;
						if (PowerCam_read_i2c (client_i2c->client,CMD_R_12V,stDlgImxPowerCam.cBuffer,1)>=0) {
							ucAlim|=stDlgImxPowerCam.cBuffer[0];							
							copy_to_user ((void __user *)arg,&ucAlim,1);
						}else {
							printk ("Erreur lecture registre Presence_POE\n");
							err=-EAGAIN;
						}
					}else {
						printk ("Erreur lecture registre Presence_POE\n");
						err=-EAGAIN;
					}					
				}
				break;
			case I2C_GET_STATUS_ON:	
				{
					if (PowerCam_read_i2c (client_i2c->client,CMD_R_STATUS_ON,stDlgImxPowerCam.cBuffer,1)>=0) {			
						copy_to_user ((void __user *)arg,&stDlgImxPowerCam.cBuffer[0],1);						
					}else {
						printk ("Erreur lecture registre Status ON\n");
						err=-EINVAL;
					}
				}
				break;
			case I2C_SET_EN_TRAME_VIE :	
				{
					unsigned char ucEnTrameVie;
					if (copy_from_user (&ucEnTrameVie,(void __user *)arg,sizeof (unsigned char))==0) {
						if (ucEnTrameVie)
							SendCmd (client_i2c->client,CMD_W_TRAME_VIE,TRAME_VIE_EN,ucEnTrameVie);
						if (ucEnTrameVie) {
							client_i2c->stTimerTrameVie.expires = jiffies + TIMER_TRAME_VIE;
							add_timer(&client_i2c->stTimerTrameVie);
						}else {
							del_timer(&client_i2c->stTimerTrameVie);
						}
						
					}else
						err=-EINVAL;
				}
				break;
			case I2C_GET_BAT_LOW:
				{
					if (PowerCam_read_i2c (client_i2c->client,CMD_R_BAT_LOW,stDlgImxPowerCam.cBuffer,1)>=0) {			
						copy_to_user ((void __user *)arg,&stDlgImxPowerCam.cBuffer[0],1);						
					}else {
						printk ("Erreur lecture registre batterie low\n");
						err=-EINVAL;
					}
				}
				break;
			case I2C_GET_VAl_INPUT:
				{
					if (PowerCam_read_i2c (client_i2c->client,CMD_R_INPUT,stDlgImxPowerCam.cBuffer,1)>=0) {			
						copy_to_user ((void __user *)arg,&stDlgImxPowerCam.cBuffer[0],1);						
					}else {
						printk ("Erreur lecture registre Input\n");
						err=-EINVAL;
					}
				}
				break;
			case I2C_GET_VAl_BOUTON_D:
				{
					if (PowerCam_read_i2c (client_i2c->client,CMD_R_BOUTON,stDlgImxPowerCam.cBuffer,1)>=0) {			
						copy_to_user ((void __user *)arg,&stDlgImxPowerCam.cBuffer[0],1);						
					}else {
						printk ("Erreur lecture registre bouton\n");
						err=-EINVAL;
					}
				}
				break;
			case I2C_GET_VAl_BOUTON_G:
				{				
					copy_to_user ((void __user *)arg,&client_i2c->ucEtatBoutonG,1);
				}
				break;
			default :
				err=-EINVAL;
				break;
		}
	}else 
		err=-ENODEV;
	up(&client_i2c->Monlock);
	//printk ("2-->i2c_PowerCam_ioctl cmd=%d err=%d\n",cmd,err);
	return err;
}

static int i2c_PowerCam_open(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();
	if (!i2c_PowerCam_client.is_open) {
		i2c_PowerCam_client.is_open=1;		
		file->private_data = &i2c_PowerCam_client;
		/**************** fin init ********************************/
	}else {
		//ret= -EBUSY;
		i2c_PowerCam_client.is_open++;
		file->private_data = &i2c_PowerCam_client;
	}
		
	unlock_kernel();
	
	return ret;
}


static int i2c_PowerCam_release(struct inode * inode, struct file * file) {
	int ret=0;
	lock_kernel();	
	if (i2c_PowerCam_client.is_open) {
		i2c_PowerCam_client.is_open--;
	}else
		ret=-1;
	
	unlock_kernel();
	return ret;
}


static int i2c_PowerCam_fasync(int fd, struct file *file, int on)
{
	struct _i2c_PowerCam_client *client_i2c = file->private_data;
	if (!client_i2c)
		return -ENODEV;

	return 0;
} 

/* No kernel lock held - fine */
static unsigned int i2c_PowerCam_poll(struct file *file, poll_table *wait)
{
	struct _i2c_PowerCam_client *client_i2c= file->private_data;
	if (!client_i2c)
		return -ENODEV;

	poll_wait(file, &client_i2c->wait_q, wait);

	if (client_i2c->data_on)
	{
		return POLLIN | POLLRDNORM;
	}

	return 0;
}

//EXPORT_NO_SYMBOLS;

/***************************************************************************
 *
 * Initialize the driver - Register the character device
 */
static __init int I2cPowerCam_init(void)
{
	struct class_device *temp_class;

	printk (KERN_INFO "Dialogue i2c carte PowerCam driver version " _VERSION "\n");

	i2c_add_driver(&i2c_PowerCam_driver);
	if (!i2c_PowerCam_client.client)
	{
		i2c_del_driver(&i2c_PowerCam_driver);
		printk("I2cPowerCam_init erreur\n");
		return -1;
	}


	/*
	 * Register the driver as a character device, for applications
	 * to access it for ioctls.
	 * First argument (major) to register_chrdev implies a dynamic
	 * major number allocation.
	 */
	major = register_chrdev(0,i2c_PowerCam_name,&i2c_PowerCam_fops);
	if (!major) {
		printk("I2cPowerCam_init: failed to register char device\n");		
		goto  err;
	}else
		printk("Dialogue i2c carte PowerCam MAJOR=%d\n",major);

	i2c_PowerCam_class = class_create(THIS_MODULE, i2c_PowerCam_name);
	if (IS_ERR(i2c_PowerCam_class)) {
		printk("Error creating I2cPowerCam class.\n");
		goto err1;
	}

	temp_class = device_create(i2c_PowerCam_class, NULL,MKDEV(major, 0),NULL, "PowerCam_i2c");
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating I2cPowerCam class device.\n");		
		goto err2;
	}
	return 0;
err3:
	device_destroy(i2c_PowerCam_class, MKDEV(major, 0));
err2:
	class_destroy(i2c_PowerCam_class);
err1:
	unregister_chrdev (major,i2c_PowerCam_name);
	major=0;
err:
	i2c_del_driver(&i2c_PowerCam_driver);
	return -1;
}

static void __exit I2cPowerCam_exit(void)
{
	if (i2c_PowerCam_client.client) {
		i2c_del_driver(&i2c_PowerCam_driver);
	}

	if (major) {
		device_destroy(i2c_PowerCam_class, MKDEV(major, 0));
		class_destroy(i2c_PowerCam_class);
		unregister_chrdev (major,i2c_PowerCam_name);
		major=0;		
	}

	debugk ("Dialogue i2c carte PowerCam de-registered\n");
}

MODULE_DESCRIPTION(VERSION_STR);
MODULE_AUTHOR("Lionel");
MODULE_LICENSE("GPL");

module_init(I2cPowerCam_init);
module_exit(I2cPowerCam_exit);

