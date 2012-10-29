#ifndef _LINUX_DLG_I2C_SORHEA_H
#define _LINUX_DLG_I2C_SORHEA_H

#define DRV_DLG_SORHEA	"/dev/PowerCam_i2c"

#define IS_POE(x) (x>>4)
#define IS_12V(x) (x|0x0f)

struct DlgImxPowerCam {
	unsigned short ucCmd;
	char cBuffer [65];
	unsigned char ucTaille;
};

struct EtatLed {
	unsigned char ucLed;
	unsigned char ucEtat;
};

#define CMD_READ 0x96

#define CMD_W_BANK1 0x1
#define CMD_W_BANK2 0x2
#define CMD_W_BANK3 0x3
#define CMD_W_BANK4 0x4
#define CMD_W_BANK5 0x5
#define CMD_W_BANK6 0x6

#define CMD_W_LED CMD_W_BANK1
#define CMD_W_BUZZER CMD_W_BANK1
#define CMD_W_RELAIS CMD_W_BANK1


#define CMD_W_INT CMD_W_BANK2
#define CMD_R_INT CMD_READ


#define CMD_W_SEUIL_PIR	CMD_W_BANK3
#define CMD_W_STOP_ALIM	CMD_W_BANK4
#define CMD_W_TRAME_VIE	CMD_W_BANK6

/*********************** cmd CMD_W_BANK1 ************************/

#define LED_ROUGE_DEBUG 0
#define LED_VERTE_DEBUG 1

#define LED_VERTE 2
#define LED_JAUNE 3
#define LED_ROUGE 4

#define BUZZER 5
#define BIP_BUZZER 6

#define RELAIS 7

#define LED_PUISSANCE 8

/*********************** cmd CMD_W_BANK2 ************************/
#define VALIDE_INT	0x0
#define CLEAR_INT	0x1
#define TEST_INT	0x2
/****************************************************************/
#define STATUS_INT	0x2
#define INT_SET		0x3
/****************************************************************/
#define SET_INT 0x5
	#define ALARME_IRP	1<<0
	#define INT_BOUTON	1<<1
	#define INT_IN		1<<2
	#define INT_ALIM	1<<3
	#define INT_BAT_LOW	1<<4

#define ALARME_HARD	0x1
	#define PB_TRAME_VIE 1<<0
	#define BOUTON_G_FACE_AVANT 1<<1

/*********************** cmd CMD_W_BANK3 ************************/
#define SEUIL_PIR	0x0
/*********************** cmd CMD_W_BANK4 ************************/
#define STOP_ALIM	0x0
/*********************** cmd CMD_W_BANK6 ************************/
#define TRAME_VIE		0x0
#define TRAME_VIE_EN	0x1
#define TRAME_VIE_ERR	0x2
#define ERR_CLEAR		0x3

/*********************** cmd read *******************************/
#define CMD_R_VERSION_PIC	((CMD_READ<<8)|0)
#define CMD_R_STATUS_INT	((CMD_READ<<8)|STATUS_INT)
#define CMD_R_BOUTON		((CMD_READ<<8)|4)
#define CMD_R_INPUT			((CMD_READ<<8)|5)
#define CMD_R_POE			((CMD_READ<<8)|7)
#define CMD_R_12V			((CMD_READ<<8)|8)
#define CMD_R_BAT_LOW		((CMD_READ<<8)|9)
#define CMD_R_TRAME_VIE		((CMD_READ<<8)|60)
#define CMD_R_STATUS_ON		((CMD_READ<<8)|61)
/****************************************************************/

#define LED_OFF 0
#define LED_ON 1
#define CLIGNO_R 2
#define CLIGNO_M 3
#define CLIGNO_L 4

/****************************************************************/

#define BUZZER_OFF 0
#define BUZZER_ON 1
#define BUZZER_BIP_R 2
#define BUZZER_BIP_M 3
#define BUZZER_BIP_L 4

#define ALIM_ON 1
#define ALIM_OFF 0

	
/****************************************************************/
#define I2C_MAGIC  'I'

#define I2C_SET_CMD				_IOW (I2C_MAGIC, 0, unsigned int *)
#define I2C_SET_ETAT_LED		_IOW (I2C_MAGIC, 1, unsigned int *)
#define I2C_SET_INT				_IOW (I2C_MAGIC, 2, unsigned int *)
#define I2C_DEL_INT				_IOW (I2C_MAGIC, 3, unsigned int *)
#define I2C_SET_ETAT_BUZZER		_IOW (I2C_MAGIC, 4, unsigned int *)
#define I2C_SET_BIP_BUZZER		_IOW (I2C_MAGIC, 5, unsigned int *)
#define I2C_SET_ETAT_RELAIS		_IOW (I2C_MAGIC, 6, unsigned int *)
#define I2C_GET_CMD				_IOR (I2C_MAGIC, 7, unsigned int *)
#define I2C_GET_INT				_IOR (I2C_MAGIC, 8, unsigned int *)
#define I2C_SET_SEUIL_PIR		_IOW (I2C_MAGIC, 9, unsigned int *)
#define I2C_SET_ALIM			_IOW (I2C_MAGIC, 10, unsigned int *)
#define I2C_GET_ALIM			_IOR (I2C_MAGIC, 11, unsigned int *)
#define I2C_GET_STATUS_ON		_IOR (I2C_MAGIC, 12, unsigned int *)
#define I2C_SET_EN_TRAME_VIE	_IOR (I2C_MAGIC, 13, unsigned int *)
#define I2C_GET_BAT_LOW			_IOR (I2C_MAGIC, 14, unsigned int *)
#define I2C_GET_VAl_INPUT		_IOR (I2C_MAGIC, 15, unsigned int *)
#define I2C_GET_VAl_BOUTON_D	_IOR (I2C_MAGIC, 16, unsigned int *)
#define I2C_GET_VAl_BOUTON_G	_IOR (I2C_MAGIC, 17, unsigned int *)
#endif
