#ifndef __MTK_H__
#define __MTK_H__

#define MAC_ADDR_LENGTH		6
#define MAX_NUMBER_OF_MAC	32
typedef unsigned char 	UCHAR;
typedef char		CHAR;
typedef unsigned int	UINT32;
typedef unsigned short	USHORT;
typedef short		SHORT;
typedef unsigned long	ULONG;

#if WIRELESS_EXT <= 11
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE				0x8BE0
#endif
#define SIOCIWFIRSTPRIV				SIOCDEVPRIVATE
#endif

#define RTPRIV_IOCTL_SET			(SIOCIWFIRSTPRIV + 0x02)
#define RTPRIV_IOCTL_GSITESURVEY		(SIOCIWFIRSTPRIV + 0x0D)
#define RTPRIV_IOCTL_GET_MAC_TABLE		(SIOCIWFIRSTPRIV + 0x0F)
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT	(SIOCIWFIRSTPRIV + 0x1F)

#define MODE_CCK	0
#define MODE_OFDM	1
#define MODE_HTMIX	2

/* MIMO Tx parameter, ShortGI, MCS, STBC, etc.  these are fields in TXWI. Don't change this definition!!! */
typedef union _MACHTTRANSMIT_SETTING {
        struct {
		#if 0
                USHORT MCS:7;   /* MCS */
                USHORT BW:1;    /*channel bandwidth 20MHz or 40 MHz */
                USHORT ShortGI:1;
                USHORT STBC:2;  /*SPACE */
                USHORT rsv:3;
                USHORT MODE:2;  /* Use definition MODE_xxx. */
		#endif
        	USHORT MCS:6;
		USHORT ldpc:1;
		USHORT BW:2;
		USHORT ShortGI:1;
		USHORT STBC:1;
		USHORT eTxBF:1;
		USHORT iTxBF:1;
		USHORT MODE:3;
	} field;
        USHORT word;
} MACHTTRANSMIT_SETTING, *PMACHTTRANSMIT_SETTING;

typedef struct _RT_802_11_MAC_ENTRY {
        UCHAR ApIdx;
        UCHAR Addr[MAC_ADDR_LENGTH];
        UCHAR Aid;
        UCHAR Psm;              /* 0:PWR_ACTIVE, 1:PWR_SAVE */
        UCHAR MimoPs;           /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
        CHAR AvgRssi0;
        CHAR AvgRssi1;
        CHAR AvgRssi2;
        UINT32 ConnectedTime;
        MACHTTRANSMIT_SETTING TxRate;
        UINT32          LastRxRate;
      

	SHORT           StreamSnr[3];                           /* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
        SHORT           SoundingRespSnr[3];                     /* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
/*      SHORT           TxPER;  */                                      /* TX PER over the last second. Percent */
/*      SHORT           reserved;*/
} RT_802_11_MAC_ENTRY, *PRT_802_11_MAC_ENTRY;

typedef struct _RT_802_11_MAC_TABLE {
        ULONG Num;
        RT_802_11_MAC_ENTRY Entry[MAX_NUMBER_OF_MAC];
} RT_802_11_MAC_TABLE, *PRT_802_11_MAC_TABLE;

typedef struct _CH_FREQ_MAP_{
	int	channel;
	int	freqMHz;
}CH_FREQ_MAP;

CH_FREQ_MAP CH_HZ_ID_MAP[]=
	{
		{1, 2412},
		{2, 2417},
		{3, 2422},
		{4, 2427},
		{5, 2432},
		{6, 2437},
		{7, 2442},
		{8, 2447},
		{9, 2452},
		{10, 2457},
		{11, 2462},
		{12, 2467},
		{13, 2472},
		{14, 2484},

		/*  UNII */
		{36, 5180},
		{40, 5200},
		{44, 5220},
		{48, 5240},
		{52, 5260},
		{56, 5280},
		{60, 5300},
		{64, 5320},
		{149, 5745},
		{153, 5765},
		{157, 5785},
		{161, 5805},
		{165, 5825},
		{167, 5835},
		{169, 5845},
		{171, 5855},
		{173, 5865},

		/* HiperLAN2 */
		{100, 5500},
		{104, 5520},
		{108, 5540},
		{112, 5560},
		{116, 5580},
		{120, 5600},
		{124, 5620},
		{128, 5640},
		{132, 5660},
		{136, 5680},
		{140, 5700},

		/* Japan MMAC */
		{34, 5170},
		{38, 5190},
		{42, 5210},
		{46, 5230},

		/*  Japan */
		{184, 4920},
		{188, 4940},
		{192, 4960},
		{196, 4980},

		{208, 5040},	/* Japan, means J08 */
		{212, 5060},	/* Japan, means J12 */   
		{216, 5080},	/* Japan, means J16 */
};

#endif // __MTK_H__

