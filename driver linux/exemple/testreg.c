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


#include "testreg.h"


int main(int argc, char *argv[]) {

	int idIReg=-1;
	idIReg=open("/dev/regdebug",O_RDWR);
	//printf ("idIReg=%d\n",idIReg);
	if (idIReg>=0) {
		if (argc>1) {
			UseLigneCmd (idIReg,argc,argv);
		}else {
			UseMenu (idIReg);
		}
	}
	return 0;
}

void UseMenu (int idIReg) {
	char choix;
	char quit=0;

	while (!quit) {
		choix=MenuGene ();
		switch (choix) {
		case 'i': //imx
			AccesRegImx (idIReg);
			break;
		case 'v': // capteur video
			AccesCapteurVideo(idIReg);
			break;
		case 'q':
			quit=1;
			break;
		default:
			printf ("Choix non disponible q pour quitter\n");
			break;
			
		}
	}
}

void UseLigneCmd (int idIReg,int argc, char *argv[]) {
	struct StimDebugReg StReg;
	int i=1;
	char Ecriture=0;
	char Clear=0;
	char Type;
	int endptr;
	
	StReg.Registre=0;
	StReg.ValeurRead=0;
	
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
				Clear=1;
				Ecriture=1;
				break;		
			default:
				Type=argv[0][1];
				break;
			}
		}else
		{
			if (i == 1) StReg.Registre = strtoul(*argv, &endptr, 16);
			else if (i == 2) StReg.ValeurWrite = strtoul(*argv, &endptr, 16);
			i++;
		}		
		argc--;
		argv++;
	}

	switch (Type) {
		case 'i': //imx
			if (Ecriture) {
				if (ioctl(idIReg, REG_W_L_IMX, (unsigned long)&StReg) < 0) {
					printf ("erreur ecriture\n");
					return;
				}

			}else {
				printf ("StReg.Registre=%08x\n",StReg.Registre);
				int a=ioctl(idIReg, REG_R_L_IMX, (unsigned long)&StReg);
				if (a < 0) {
					printf ("erreur lecture %d\n",a);
					return;
				}
			}
			printf ("la valeur du registre de l'IMX 0x%08x est : 0x%08x\n",StReg.Registre,StReg.ValeurRead);
			
			break;
		case 'v': // capteur video
			if (Ecriture) {
				if (ioctl(idIReg, REG_W_OV7720, (unsigned long)&StReg) < 0) {
					if (ioctl(idIReg, REG_W_VS6624, (unsigned long)&StReg) < 0) {
						if (ioctl(idIReg, REG_W_OV9655, (unsigned long)&StReg) < 0) {
							printf ("erreur ecriture\n");
							return;
						}
					}
				}
			}else {
				printf ("StReg.Registre=%08x\n",StReg.Registre);
				if (ioctl(idIReg, REG_R_OV7720, (unsigned long)&StReg) < 0) {
					if (ioctl(idIReg, REG_R_VS6624, (unsigned long)&StReg) < 0) {
						if (ioctl(idIReg, REG_R_OV9655, (unsigned long)&StReg) < 0) {
							printf ("erreur lecture\n");
							return;
						}
					}
				}
			}
			printf ("la valeur du registre du Capteur Video 0x%08x est : 0x%08x\n",StReg.Registre,StReg.ValeurRead);
			break;
		case 'n': // net
			if (Ecriture) {
				if (!Clear) {
					if (ioctl(idIReg, REG_W_SMSC9115, (unsigned long)&StReg) < 0) {
						printf ("erreur ecriture\n");
						return;
					}
				}else { // test lecture //		
					int cpt=0;
					for (cpt=0;cpt<1000;cpt++) {
						if (ioctl(idIReg, REG_R_SMSC9115, (unsigned long)&StReg) < 0) {
							printf ("erreur ecriture\n");
							return;
						}
					}
					return;
				}
			}else {
				printf ("StReg.Registre=%08x\n",StReg.Registre);
				if (ioctl(idIReg, REG_R_SMSC9115, (unsigned long)&StReg) < 0) {
					printf ("erreur lecture\n");
					return;
				}
			}
			printf ("la valeur du registre du smsc9115 0x%08x est : 0x%08x\n",StReg.Registre,StReg.ValeurRead);
			break;
		case 's':
				ioctl(idIReg, STIM_PM_SUSPEND, (unsigned long)&StReg);
			break;
		case 'u': // usb
			if (Ecriture) {
				if (Clear) {
					if (ioctl(idIReg, ISP1504_CLEAR, (unsigned long)&StReg) < 0) {
						printf ("erreur clear\n");
						return;
					}
				}else {
					if (ioctl(idIReg, ISP1504_SET, (unsigned long)&StReg) < 0) {
						printf ("erreur ecriture\n");
						return;
					}
				}
			}else {
				printf ("StReg.Registre=%08x\n",StReg.Registre);
				if (ioctl(idIReg, ISP1504_READ, (unsigned long)&StReg) < 0) {
					printf ("erreur lecture\n");
					return;
				}
			}
			printf ("la valeur du registre du chip usb 0x%08x est : 0x%08x\n",StReg.Registre,StReg.ValeurRead);
			break;
		case 'x': // test registre video
			{
				for (StReg.Registre=0x53fc0000;StReg.Registre<0x53fc00b4;StReg.Registre+=4) {
					StReg.ValeurRead=0;
					
					if (ioctl(idIReg, REG_R_L_IMX, (unsigned long)&StReg)>=0){
						printf ("registre=%08x valeur=%08x\n",StReg.Registre,StReg.ValeurRead);
					}
				}
			}
			break;
		case 'm':
			if (Ecriture) {
				if (!Clear) {
					if (ioctl(idIReg, REG_W_SMSC9115_MAC, (unsigned long)&StReg) < 0) {
						printf ("erreur ecriture\n");
						return;
					}
				}else { // test lecture //		
					int cpt=0;
					for (cpt=0;cpt<1000;cpt++) {
						if (ioctl(idIReg, REG_R_SMSC9115_MAC, (unsigned long)&StReg) < 0) {
							printf ("erreur ecriture\n");
							return;
						}
					}
					return;
				}
			}else {				
				if (ioctl(idIReg, REG_R_SMSC9115_MAC, (unsigned long)&StReg) < 0) {
					printf ("erreur lecture\n");
					return;
				}
			}
			printf ("la valeur du registre mac du smsc9115 0x%08x est : 0x%08x\n",StReg.Registre,StReg.ValeurRead);
			break;
		case 'a': 
			{
				int idVideo=open("/dev/video",O_RDWR);
				if (idVideo>=0) {
					if (Ecriture) {				
						if (ioctl(idVideo, VIDEO_SET_AEC, (unsigned long)StReg.ValeurWrite) < 0) {
							printf ("erreur ecriture\n");
							close (idVideo);
							return;
						}	
					}else {
						
						if (ioctl(idVideo, VIDEO_GET_AEC, (unsigned long)&StReg.ValeurRead) < 0) {
							printf ("erreur lecture\n");
							close (idVideo);
							return;
						}
						
						printf ("la valeur de l'aec 0x%08x\n",StReg.ValeurRead);
					}
					close (idVideo);
				}
				
			}
			break;
		case 'g':
			{
				int idVideo=open("/dev/video",O_RDWR);
				if (idVideo>=0) {
					if (Ecriture) {				
						if (ioctl(idVideo, VIDEO_SET_AGC, (unsigned long)StReg.ValeurWrite) < 0) {
							printf ("erreur ecriture\n");
							close (idVideo);
							return;
						}				
					}else {				
						if (ioctl(idVideo, VIDEO_GET_AGC, (unsigned long)&StReg.ValeurRead) < 0) {
							printf ("erreur lecture\n");
							close (idVideo);
							return;
						}
					}
					printf ("la valeur de l'agc 0x%08x\n",StReg.ValeurRead);
					close (idVideo);
				}
				
			}
			break;
		case 'l':
			if (Ecriture) {				
				if (ioctl(idIReg, REG_W_L2_IMX, (unsigned long)&StReg) < 0) {
					printf ("erreur ecriture\n");
					return;
				}				
			}else {				
				if (ioctl(idIReg, REG_R_L2_IMX, (unsigned long)&StReg) < 0) {
					printf ("erreur lecture\n");
					return;
				}
			}
			printf ("la valeur du registre est 0x%08x\n",StReg.ValeurRead);
			break;
		default :	
			printf ("Choix non disponible\n");
			break;
	}			
	
}

char MenuGene () {
	char choix='\0';
	printf ("--------- Menu general --------\n");
	printf ("	i : Registre IMX31\n");
	printf ("	v : Registre Capteur video\n");
	printf ("	q : quitter\n");
	printf ("Choix : ");
	scanf ("%c",&choix);
	getc (stdin);
	return choix;
}

void AccesRegImx (int idIReg) {
	struct StimDebugReg StReg;
	char choix='\0';

	StReg.Registre=0;
	StReg.ValeurRead=0;


	printf ("--------- Menu IMX --------\n");
	printf ("	r : lecture registre \n");
	printf ("	w : ecriture registre \n");
	printf ("Choix : ");
	scanf ("%c",&choix);
	getc (stdin);
	switch (choix) {
	case 'r':
		printf ("registre : ");
		scanf ("%x",&StReg.Registre);
		getc (stdin);		
		if (ioctl(idIReg, REG_R_L_IMX, (unsigned long)&StReg) < 0) {
			printf ("erreur lecture\n");
			return;
		}
		
		break;
	case 'w':
		printf ("registre valeur : ");
		scanf ("%x %x",&StReg.Registre,&StReg.ValeurWrite);
		getc (stdin);
		if (ioctl(idIReg, REG_W_L_IMX, (unsigned long)&StReg) < 0) {
			printf ("erreur ecriture\n");
			return;
		}
		break;
	default:
		printf ("erreur %c\n",choix);
		break;
	}
	printf ("la valeur du registre de l'IMX 0x%08x est : 0x%08x\n",StReg.Registre,StReg.ValeurRead);
}

void AccesCapteurVideo (int idIReg) {
	struct StimDebugReg StReg;
	char choix='\0';

	StReg.Registre=0;
	StReg.ValeurRead=0;

	printf ("--------- Menu Capteur Video  --------\n");
	printf ("	r : lecture registre \n");
	printf ("	w : ecriture registre \n");
	printf ("Choix : ");
	scanf ("%c",&choix);
	getc (stdin);
	switch (choix) {
	case 'r':
		printf ("registre : ");
		scanf ("%x",&StReg.Registre);
		getc (stdin);		
		if (ioctl(idIReg, REG_R_OV7720, (unsigned long)&StReg) < 0) {
			if (ioctl(idIReg, REG_R_VS6624, (unsigned long)&StReg) < 0) {
				printf ("erreur lecture\n");
				return;
			}
		}
		
		break;
	case 'w':
		printf ("registre valeur : ");
		scanf ("%x %x",&StReg.Registre,&StReg.ValeurWrite);
		getc (stdin);
		if (ioctl(idIReg, REG_W_OV7720, (unsigned long)&StReg) < 0) {
			if (ioctl(idIReg, REG_W_VS6624, (unsigned long)&StReg) < 0) {
				printf ("erreur ecriture\n");
				return;
			}
		}
		break;
	default:
		printf ("erreur %c\n",choix);
		break;
	}
	printf ("la valeur du registre du Capteur Video 0x%08x est : 0x%08x\n",StReg.Registre,StReg.ValeurRead);
}

