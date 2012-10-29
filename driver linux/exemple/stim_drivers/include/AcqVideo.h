#ifndef ACQVIDEO_
#define ACQVIDEO_
/*
#if !defined (u32)
#define u32 unsigned int
*/
#define VIDEO_STIM  'v'

#define X_RES(x) (x >> 16)
#define Y_RES(x) (x & 0xFFFF)
#define MSB(x)(x >> 8)
#define LSB(x)(x & 0xFF)

#define RES(x, y)	(x << 16) | (y & 0xFFFF)

#define RES_SXGA	RES(1280, 1024)
#define RES_VGA		RES(640, 480)
#define RES_QVGA	RES(320, 240)
#define RES_QQVGA	RES(160, 120)

#define RES_CIF		RES(352, 288)
#define RES_QCIF	RES(176, 144)
#define RES_SQCIF	RES(128, 96)

#define MODE_AUTO 0x01
#define MODE_MANUEL 0x0

#define VIDEO_SET_FORMAT			_IOW (VIDEO_STIM, 0, unsigned int *)
#define VIDEO_START_ACQ				_IOW (VIDEO_STIM, 1, unsigned int *)
#define VIDEO_GET_IMAGE				_IOR (VIDEO_STIM, 2, unsigned int *)
#define VIDEO_DEL_IMAGE				_IOW (VIDEO_STIM, 3, unsigned int *)
#define VIDEO_STOP_ACQ				_IOW (VIDEO_STIM, 4, unsigned int *)
#define VIDEO_SET_ETAT_LED			_IOW (VIDEO_STIM, 5, unsigned int *)
#define VIDEO_GET_AEC				_IOR (VIDEO_STIM, 6, unsigned int *)
#define VIDEO_SET_AEC				_IOW (VIDEO_STIM, 7, unsigned int)
#define VIDEO_GET_AGC				_IOR (VIDEO_STIM, 8, unsigned int *)
#define VIDEO_SET_AGC				_IOW (VIDEO_STIM, 9, unsigned int)
#define VIDEO_SET_AEC_MODE			_IOW (VIDEO_STIM, 10, unsigned int)
#define VIDEO_SET_AGC_MODE			_IOW (VIDEO_STIM, 11, unsigned int)
#define VIDEO_SET_AWB_MODE			_IOW (VIDEO_STIM, 12, unsigned int)
#define VIDEO_SET_FREQUENCE			_IOW (VIDEO_STIM, 13, unsigned int)
#define VIDEO_GET_ADV				_IOR (VIDEO_STIM, 14, unsigned int *)
#define VIDEO_SET_ADV				_IOW (VIDEO_STIM, 15, unsigned int)
#define VIDEO_GET_IMAGE_DISPO		_IOR (VIDEO_STIM, 16, unsigned int *)

#define VIDEO_SET_SAT				_IOW (VIDEO_STIM, 17, unsigned int *)
#define VIDEO_SET_LENS_CORRECTION	_IOW (VIDEO_STIM, 18, unsigned int)
#define VIDEO_SET_AWB				_IOW (VIDEO_STIM, 19, unsigned int *)



typedef struct _DefFormat {
	unsigned int uiResolution;
	unsigned char ucFrequence;
}DefFormat;

typedef struct _setetatled {
	unsigned char ucLed;
	unsigned char ucEtat;
}SetEtatLed;


typedef struct _DefImage {
	unsigned int pImage;
	unsigned int uiResolution;	
	unsigned int uiTailleBuffer;
	unsigned int uiTailleImage;
}DefImage;

typedef struct _ImageVideo {
	unsigned char ucIndex;
	struct timeval stTimeStamp;
	unsigned int uiFlagAsserv;
	DefImage ImageTotale;
	DefImage ImageReelle;
	DefImage TabMoy;
}ImageVideo;


typedef struct _Saturation {
	unsigned char MTX_1;
	unsigned char MTX_2;
	unsigned char MTX_3;
	unsigned char MTX_4;
	unsigned char MTX_5;
	unsigned char MTX_6;	
}Saturation;

typedef struct _Awb {
	unsigned char Red;
	unsigned char Green;	
	unsigned char Blue;	
}Awb;

#endif //ACQVIDEO_
