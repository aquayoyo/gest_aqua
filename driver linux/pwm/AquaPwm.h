#ifndef _LINUX_AQUA_PWM_H
#define _LINUX_AQUA_PWM_H

typedef struct {
	
};

#define REG_MAGIC  'P'

#define SET_FREQ		_IOW (REG_MAGIC, 0, unsigned int *)
#define SET_FREQ		_IOW (REG_MAGIC, 1, unsigned int *)


#define GEt_FREQ		_IOR (REG_MAGIC, 2, unsigned int *)

#endif //_LINUX_AQUA_PWM_H