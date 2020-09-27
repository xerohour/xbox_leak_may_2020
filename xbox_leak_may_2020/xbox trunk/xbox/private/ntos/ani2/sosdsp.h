#define	GPXMEM	0xfe830000
#define	GPYMEM	0xfe836000
#define	GPPMEM	0xfe83a000



struct	patch {
	unsigned int	start;
	unsigned int	end;
	unsigned int	loop;
	unsigned char	pitch;
	unsigned char	bank;
};
struct	word {
	unsigned int	start;
	unsigned int	end;
	unsigned char	bank;
	unsigned int	volume;
};


struct	two_pole_lpf {
	unsigned short	c0;
	unsigned short	c1;
	unsigned short	c2;
};
#define	N_OSCS		11
#define	OSC_SIZE	6

#define	ST_VOLUME_R			66
#define	ST_VOLUME_L			77


/*
 *	adpcm equates
 */

#define		ADP_END			109
#define		ADP_BANK		111
#define		ADP_PRE_VOLUME_R 116
#define		ADP_PRE_VOLUME_L 120
#define		ADP_PRE_START	117
#define		ADP_STATUS		115
#define		ADP_VOLUME_R 110
#define		ADP_VOLUME_L 112


#define	OSC_0_MOD_INDEX	119
