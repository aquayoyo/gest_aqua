#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sched.h>		// sched_setscheduler
#include <sys/time.h>
#include <sys/ioctl.h>
#include <semaphore.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <linux/fb.h>	//framebuffer device
#include <sys/ioctl.h>	//access fb device

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "dlg_i2c_sorhea.h"


void UseLigneCmd (int idIReg,int argc, char *argv[]) {
	struct DlgImxPowerCam stDlgImxPowerCam;	
	char Ecriture=0;
	unsigned char ucCmd=0;	
	unsigned char ucAdd=0;
	unsigned char ucVal=0;
	unsigned char TrameVie=1;
	int endptr;

	argc--;
	argv++;
	while(argc > 0)
	{
		if (**argv == '-')
		{
			switch (argv[0][1]) {						
			case 'r':
				if (argc <= 0) {
					fprintf(stderr, "erreur arg\n");
					exit(1);
				}
				Ecriture=0;
				break;
			case 'w':
				if (argc <= 0) {
					fprintf(stderr, "erreur arg\n");
					exit(1);
				}
				Ecriture=1;
				break;
			case 'c':
				if (argc <= 0) {
					fprintf(stderr, "erreur arg\n");
					exit(1);
				}
				argc--;
				argv++;

				ucCmd = strtoul(*argv, &endptr, 16);
				break;
			case 'a':
				if (argc <= 0) {
					fprintf(stderr, "erreur arg\n");
					exit(1);
				}
				argc--;
				argv++;

				ucAdd = strtoul(*argv, &endptr, 16);
				break;
			case 'v':
				if (argc <= 0) {
					fprintf(stderr, "erreur arg\n");
					exit(1);
				}
				argc--;
				argv++;
				ucVal = strtoul(*argv, &endptr, 16);
				break;
			case 't':
				TrameVie=0;
				break;
			default:
				break;
			}
		}		
		argc--;
		argv++;
	}

	
	if (!TrameVie) {
		if (ioctl(idIReg, I2C_SET_EN_TRAME_VIE, (unsigned long)&TrameVie) < 0) {
			printf ("erreur I2C_SET_EN_TRAME_VIE TrameVie=%d\n",TrameVie);			
		}
		return;
	}else {
		
		if (Ecriture) {
			
			
			stDlgImxPowerCam.ucCmd=ucCmd;
			stDlgImxPowerCam.cBuffer[0]=ucAdd;
			stDlgImxPowerCam.cBuffer[1]=ucVal;
			stDlgImxPowerCam.cBuffer[2]=0;
			stDlgImxPowerCam.ucTaille=2;
			
			if (ioctl(idIReg, I2C_SET_CMD, (unsigned long)&stDlgImxPowerCam) < 0) {
				printf ("erreur ecriture\n");
				return;
			}
			
		}else {
			if (ucCmd==150 && ucAdd==80) {
				// etat bouton imx
				stDlgImxPowerCam.cBuffer[0]=0;
				int a=ioctl(idIReg, I2C_GET_VAl_BOUTON_G, (unsigned long)&stDlgImxPowerCam.cBuffer[0]);
				if (a < 0) {
					printf ("erreur lecture %d\n",a);
					return;
				}
			}else {
				stDlgImxPowerCam.ucCmd=ucCmd<<8|ucAdd;
				stDlgImxPowerCam.cBuffer[0]=0;
				stDlgImxPowerCam.ucTaille=1;		
				int a=ioctl(idIReg, I2C_GET_CMD, (unsigned long)&stDlgImxPowerCam);
				if (a < 0) {
					printf ("erreur lecture %d\n",a);
					return;
				}
			}
			printf ("La Valeur du cmd=%d add=%d est %x\n",ucCmd,ucAdd,stDlgImxPowerCam.cBuffer[0]);
		}
	}

}


int main(int argc, char *argv[]) {

	int idI2cSorhea=-1;
	fd_set rfds;
	struct timeval tv;
	int e;
	int n=0;	
	
	idI2cSorhea=open(DRV_DLG_SORHEA,O_RDWR);
	printf ("%s=%d\n",DRV_DLG_SORHEA,idI2cSorhea);
	if (idI2cSorhea>=0) {
		UseLigneCmd (idI2cSorhea,argc,argv);
	}	close (idI2cSorhea);	
	return 0;
}