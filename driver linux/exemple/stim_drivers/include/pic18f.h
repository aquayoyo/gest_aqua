#ifndef PIC18F_H
#define PIC18F_H

#include <linux/ioctl.h>

/*
	PIC18F ioctl
*/
#define PIC18F_MAGIC  'P'

#define PIC18F_READ_DATA		_IOW (PIC18F_MAGIC, 0, unsigned int *)
#define PIC18F_WRITE_DATA		_IOW (PIC18F_MAGIC, 1, unsigned int *)
#define PIC18F_READ_STAT		_IOW (PIC18F_MAGIC, 2, unsigned int *)
#define PIC18F_WRITE_STAT		_IOW (PIC18F_MAGIC, 3, unsigned int *)


#endif // PIC18F_H
