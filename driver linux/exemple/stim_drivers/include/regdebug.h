#ifndef _LINUX_REG_DEBUG_H
#define _LINUX_REG_DEBUG_H

typedef struct StimDebugReg{
	unsigned int Registre;
	unsigned int ValeurRead;
	unsigned int ValeurWrite;
};

#define REG_MAGIC  'R'

#define REG_R_L_IMX		_IOW (REG_MAGIC, 0, unsigned int *)
#define REG_W_L_IMX  	_IOW (REG_MAGIC, 1, unsigned int *)
#define REG_R_OV7720	_IOR (REG_MAGIC, 2, unsigned int *)
#define REG_W_OV7720	_IOR (REG_MAGIC, 3, unsigned int *)
#define REG_R_SMSC9115	_IOR (REG_MAGIC, 4, unsigned int *)
#define REG_W_SMSC9115	_IOR (REG_MAGIC, 5, unsigned int *)

#define STIM_PM_SUSPEND	_IOW (REG_MAGIC, 6, unsigned int *)
#define STIM_PM_RESUME	_IOW (REG_MAGIC, 7, unsigned int *)


#define ISP1504_SET		_IOW (REG_MAGIC, 8, unsigned int *)
#define ISP1504_CLEAR	_IOW (REG_MAGIC, 9, unsigned int *)
#define ISP1504_READ	_IOR (REG_MAGIC, 10, unsigned int *)

#define REG_R_VS6624	_IOR (REG_MAGIC, 11, unsigned int *)
#define REG_W_VS6624	_IOW (REG_MAGIC, 12, unsigned int *)

#define REG_R_OV9655	_IOR (REG_MAGIC, 13, unsigned int *)
#define REG_W_OV9655	_IOW (REG_MAGIC, 14, unsigned int *)

#define REG_R_SMSC9115_MAC	_IOR (REG_MAGIC, 15, unsigned int *)
#define REG_W_SMSC9115_MAC		_IOW (REG_MAGIC, 16, unsigned int *)

#define REG_R_OV9655_AGC	_IOR (REG_MAGIC, 17, unsigned int *)
#define REG_W_OV9655_AGC	_IOW (REG_MAGIC, 18, unsigned int *)

#define REG_R_OV9655_AEC	_IOR (REG_MAGIC, 19, unsigned int *)
#define REG_W_OV9655_AEC	_IOW (REG_MAGIC, 20, unsigned int *)

#define REG_R_L2_IMX	_IOR (REG_MAGIC, 21, unsigned int *)
#define REG_W_L2_IMX	_IOW (REG_MAGIC, 22, unsigned int *)


#define REG_AECH	0xA1
#define REG_AEC		0x10
#define REG_AECL	0x04



/*
#define REG_R_SMSC9115_MAC	_IOR (REG_MAGIC, 21, unsigned int *)
#define REG_W_SMSC9115_MAC	_IOW (REG_MAGIC, 22, unsigned int *)
*/



#endif //_LINUX_REG_DEBUG_H
