/*
 * iwinfo - Wireless Information Library - Linux Wireless Extension Backend
 *
 *   Copyright (C) 2009 Jo-Philipp Wich <xm@subsignal.org>
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 *
 * Parts of this code are derived from the Linux wireless tools, iwlib.c,
 * iwlist.c and iwconfig.c in particular.
 */

#include "iwinfo_wext.h"
#include "api/mtk.h"

struct survey_table {
	long channel;
	long strength;
	char mode[12];
	char ssid[66];
	char bssid[18];
	char enc[16];
	char crypto[16];
};

static int mtk_wrq(struct iwreq *wrq, const char *ifname, int cmd, void *data, size_t len)
{
	strncpy(wrq->ifr_name, ifname, IFNAMSIZ);

	if( data != NULL )
	{
		if( len < IFNAMSIZ )
		{
			memcpy(wrq->u.name, data, len);
		}
		else
		{
			wrq->u.data.pointer = data;
			wrq->u.data.length = len;
		}
	}

	return iwinfo_ioctl(cmd, wrq);
}

static int mtk_get80211priv(const char *ifname, int op, void *data, size_t len)
{
	struct iwreq iwr;

	if( mtk_wrq(&iwr, ifname, op, data, len) < 0 )
		return -1;

	return iwr.u.data.length;
}

static int mtk_isap(const char *ifname)
{
	int ret=0;

	if( strlen(ifname) <= 7 )
	{
		static char wifiname[IFNAMSIZ];
		snprintf(wifiname, sizeof(wifiname), "%s", ifname);
		if( !strncmp(wifiname, "ra", 2) || !strncmp(wifiname, "rai", 3) ) ret=1;
	}

	return ret;
}

static int mtk_iscli(const char *ifname)
{
	int ret=0;

	if( strlen(ifname) <= 7 )
	{
		static char wifiname[IFNAMSIZ];
		snprintf(wifiname, sizeof(wifiname), "%s", ifname);
		if( !strncmp(wifiname, "apcli", 5) || !strncmp(wifiname, "apclii", 6) ) ret=1;
	}

	return ret;
}

static int mtk_probe(const char *ifname)
{
	return ( mtk_isap(ifname) || mtk_iscli(ifname) );
}

static void mtk_close(void)
{
	/* Nop */
}

static int mtk_get_mode(const char *ifname, int *buf)
{
	if( mtk_isap(ifname) ) *buf = IWINFO_OPMODE_MASTER;
	else if( mtk_iscli(ifname) ) *buf = IWINFO_OPMODE_CLIENT;
	else *buf = IWINFO_OPMODE_UNKNOWN;
	return 0;
}

static int mtk_get_ssid(const char *ifname, char *buf)
{
	return wext_ops.ssid(ifname, buf);
}

static int mtk_get_bssid(const char *ifname, char *buf)
{
	char cmd[256];
	FILE *fp = NULL;

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "ifconfig %s | grep UP", ifname);
	fp = popen(cmd, "r");
	fscanf(fp, "%s\n", buf);
	pclose(fp);

	return wext_ops.bssid(ifname, buf);
}

static int mtk_get_bitrate(const char *ifname, int *buf)
{
	return wext_ops.bitrate(ifname, buf);
}

static int mtk_get_channel(const char *ifname, int *buf)
{
	return wext_ops.channel(ifname, buf);
}

static int mtk_get_frequency(const char *ifname, int *buf)
{
	int channel,chidx,chnum;

	chnum=(sizeof(CH_HZ_ID_MAP)/sizeof(CH_FREQ_MAP));

	if(mtk_get_channel(ifname, &channel) >= 0);
	{
		for (chidx = 0; chidx < chnum; chidx++)
		{
			if (channel == CH_HZ_ID_MAP[chidx].channel)
			{
				*buf = CH_HZ_ID_MAP[chidx].freqMHz;
				return 0;
			}
		}
	}
	return -1;
}

static int mtk_get_txpower(const char *ifname, int *buf)
{
	return -1;
}

static int mtk_get_signal(const char *ifname, int *buf)
{
	return -1;
}

static int mtk_get_noise(const char *ifname, int *buf)
{
	return -1;
}

static int mtk_get_quality(const char *ifname, int *buf)
{
	return -1;
}

static int mtk_get_quality_max(const char *ifname, int *buf)
{
	return -1;
}

static int mtk_get_rate(MACHTTRANSMIT_SETTING HTSetting)

{
	int MCSMappingRateTable[] =
	{2,  4,   11,  22, /* CCK*/
	12, 18,   24,  36, 48, 72, 96, 108, /* OFDM*/
	13, 26,   39,  52,  78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260, /* 20MHz, 800ns GI, MCS: 0 ~ 15*/
	39, 78,  117, 156, 234, 312, 351, 390,										  /* 20MHz, 800ns GI, MCS: 16 ~ 23*/
	27, 54,   81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540, /* 40MHz, 800ns GI, MCS: 0 ~ 15*/
	81, 162, 243, 324, 486, 648, 729, 810,										  /* 40MHz, 800ns GI, MCS: 16 ~ 23*/
	14, 29,   43,  57,  87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288, /* 20MHz, 400ns GI, MCS: 0 ~ 15*/
	43, 87,  130, 173, 260, 317, 390, 433,										  /* 20MHz, 400ns GI, MCS: 16 ~ 23*/
	30, 60,   90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600, /* 40MHz, 400ns GI, MCS: 0 ~ 15*/
	90, 180, 270, 360, 540, 720, 810, 900};

	int rate_count = sizeof(MCSMappingRateTable)/sizeof(int);
	int rate_index = 0;
	int value = 0;

	if (HTSetting.field.MODE >= MODE_HTMIX)
	{
    		rate_index = 12 + ((UCHAR)HTSetting.field.BW *24) + ((UCHAR)HTSetting.field.ShortGI *48) + ((UCHAR)HTSetting.field.MCS);
	}
	else if (HTSetting.field.MODE == MODE_OFDM)
		rate_index = (UCHAR)(HTSetting.field.MCS) + 4;
	else if (HTSetting.field.MODE == MODE_CCK)   
		rate_index = (UCHAR)(HTSetting.field.MCS);

	if (rate_index < 0)
		rate_index = 0;
    
	if (rate_index >= rate_count)
		rate_index = rate_count-1;

	value = (MCSMappingRateTable[rate_index] * 5)/10;

	return value;
}

static int mtk_get_assoclist(const char *ifname, char *buf, int *len)
{
	struct iwinfo_assoclist_entry entry;
	RT_802_11_MAC_TABLE *mt;
	MACHTTRANSMIT_SETTING rxrate;
	char raname[IFNAMSIZ],raidx[IFNAMSIZ],raiidx[IFNAMSIZ];
	int mtlen=sizeof(RT_802_11_MAC_TABLE);
	int i,j;

	if ((mt = (RT_802_11_MAC_TABLE *) malloc(mtlen)) == NULL)
	{
		return -1;
	}

	memset(mt, 0, mtlen);
	snprintf(raname, sizeof(raname), "%s", ifname);

	if (mtk_get80211priv(ifname, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, mt, mtlen) > 0)
	{
		j = 0;

		for (i = 0; i < mt->Num; i++)
		{
			snprintf(raidx, sizeof(raidx), "ra%d", mt->Entry[i].ApIdx);
			snprintf(raiidx, sizeof(raiidx), "rai%d", mt->Entry[i].ApIdx);

			if( strncmp(raname, raidx, IFNAMSIZ) && strncmp(raname, raiidx, IFNAMSIZ) ) continue;

			memset(&entry, 0, sizeof(entry));

			memcpy(entry.mac, &mt->Entry[i].Addr, 6);

			if(mt->Entry[i].AvgRssi0 > mt->Entry[i].AvgRssi1)
				entry.signal = mt->Entry[i].AvgRssi0;
			else
				entry.signal = mt->Entry[i].AvgRssi1;

			entry.noise  = -95;

			entry.inactive = mt->Entry[i].ConnectedTime * 1000;

			rxrate.word = mt->Entry[i].LastRxRate;
		//	printf("%X \n ",mt->Entry[i].TxRate);
			entry.tx_rate.rate = mtk_get_rate(mt->Entry[i].TxRate) * 1000;
			entry.rx_rate.rate = mtk_get_rate(rxrate) * 1000;

			entry.tx_rate.mcs = mt->Entry[i].TxRate.field.MCS;
			entry.rx_rate.mcs = rxrate.field.MCS;

			entry.tx_packets = 0;
			entry.rx_packets = 0;

			if(mt->Entry[i].TxRate.field.BW) entry.tx_rate.is_40mhz = 1;
			if(mt->Entry[i].TxRate.field.ShortGI) entry.tx_rate.is_short_gi = 1;
			if(rxrate.field.BW) entry.rx_rate.is_40mhz = 1;
			if(rxrate.field.ShortGI) entry.rx_rate.is_short_gi = 1;

			memcpy(&buf[j], &entry, sizeof(struct iwinfo_assoclist_entry));
			j += sizeof(struct iwinfo_assoclist_entry);
		}
		*len = j;
		free(mt);
		return 0;
	}

	free(mt);
	return -1;
}

static int mtk_get_txpwrlist(const char *ifname, char *buf, int *len)
{
	return -1;
}

static int ascii2num(char ascii)
{
	int num;
	if ((ascii >= '0') && (ascii <= '9'))
		num=ascii - 48;
	else if ((ascii >= 'a') && (ascii <= 'f'))
		num=ascii - 'a' + 10;
        else if ((ascii >= 'A') && (ascii <= 'F'))
		num=ascii - 'A' + 10;
	else
		num = 0;
	return num;
}

static void next_field(char **line, char *output, int n, int m) {
	while (**line == ' ') (*line)++;
	char *sep = strchr(*line, ' ');

	while (m-- >0) {
		if (*(*line+m) != ' ') {
			sep= *line+m+1;
			break;
		}
	}

	if (sep) {
		*sep = '\0';
		sep++;
	}

	strncpy(output, *line, n);

	/* Update the line token for the next call */
	*line = sep;
}

static int mtk_get_scan(const char *ifname, struct survey_table *st)
{
	int survey_count = 0;
	char *s = malloc(IW_SCAN_MAX_DATA);
	char ss[64] = "SiteSurvey=1";
	char *line, *start ,*p;

	if( mtk_get80211priv(ifname, RTPRIV_IOCTL_SET, ss, sizeof(ss)) < 0 )
		return -1;

	sleep(5);
	memset(s, 0x00, IW_SCAN_MAX_DATA);

	if( mtk_get80211priv(ifname, RTPRIV_IOCTL_GSITESURVEY, s, IW_SCAN_MAX_DATA) < 1 )
		return -1;

	start = s;
	while (*start == '\n') start++;
	strtok(start, "\n");
	line = strtok(NULL, "\n");

	while(line && (survey_count < 64))
	{
		char tmp[32];
		next_field(&line, tmp, sizeof(tmp),0);
		st[survey_count].channel = atol(tmp);
		next_field(&line, st[survey_count].ssid, sizeof(st->ssid),33);
		next_field(&line, st[survey_count].bssid, sizeof(st->bssid),0);
		next_field(&line, tmp, sizeof(tmp),0);
		p = strchr(tmp, '/');
		if(p != NULL) {
			*p = '\0';
			strncpy(st[survey_count].crypto, p+1, sizeof(st->crypto));
		}
		strncpy(st[survey_count].enc, tmp, sizeof(st->enc));
		next_field(&line, tmp, sizeof(tmp),0);
		st[survey_count].strength = atol(tmp);
		next_field(&line, st[survey_count].mode, sizeof(st->mode),0);

		line = strtok(NULL, "\n");
		if(st[survey_count].ssid[0] == '!')
		{
			int i = 0;
			char ssid2[33];
			char *s = (char*)st[survey_count].ssid;
			s++;
			ssid2[32] = '\0';
			for(i = 0; i < 32; i++)
			{
				ssid2[31 - i] = (char)strtol((char*)&s[62 - (2 * i)], 0, 16);
				s[62 - (2 * i)] = '\0';
			}
			memcpy(st[survey_count].ssid, ssid2, 33);
		}
		survey_count++;
	}
	free(s);
	return survey_count;
}

static int mtk_get_scanlist(const char *ifname, char *buf, int *len)
{
	int sc,i,j=0,h;
	char *p;
	struct survey_table stl[64];
	struct iwinfo_scanlist_entry sce;

	sc = mtk_get_scan(ifname, stl);
	if ( sc < 1)
		return -1;

	for (i = 0; i < sc; i++)
	{
		memset(&sce, 0, sizeof(sce));

		for (h = 0; h < 6; h++)
			sce.mac[h] = (uint8_t)(ascii2num(stl[i].bssid[h*3]) * 16 + ascii2num(stl[i].bssid[h*3+1]));

		memcpy(sce.ssid, stl[i].ssid, sizeof(stl[i].ssid));

		sce.channel=(uint8_t) stl[i].channel;
		sce.quality = (uint8_t) stl[i].strength;
		sce.quality_max = 100;
		sce.signal = 156 + sce.quality;
		sce.mode=1;

		if (strcmp(stl[i].enc,"NONE"))
		{
			sce.crypto.enabled=1;
			if (!strcmp(stl[i].enc,"WPA1PSKWPA2PSK"))
			{
				sce.crypto.wpa_version = 3;
				sce.crypto.auth_suites = IWINFO_KMGMT_PSK;
			}
			else if (!strcmp(stl[i].enc,"WPA2PSK"))
			{
				sce.crypto.wpa_version = 2;
				sce.crypto.auth_suites = IWINFO_KMGMT_PSK;
			}
			else if (!strcmp(stl[i].enc,"WPAPSK"))
			{
				sce.crypto.wpa_version = 1;
				sce.crypto.auth_suites = IWINFO_KMGMT_PSK;
			}

			if (!strcmp(stl[i].crypto,"AES"))
				sce.crypto.pair_ciphers = IWINFO_CIPHER_CCMP;
			else if (!strcmp(stl[i].crypto,"TKIP"))
				sce.crypto.pair_ciphers = IWINFO_CIPHER_TKIP;
		}
		else
		{
			sce.crypto.enabled=0;
		}

		memcpy(&buf[j], &sce, sizeof(struct iwinfo_scanlist_entry));
		j += sizeof(struct iwinfo_scanlist_entry);
	}
	*len = j;
	return 0;
}

static int mtk_get_freqlist(const char *ifname, char *buf, int *len)
{
	return -1;
}

static int mtk_get_country(const char *ifname, char *buf)
{
	sprintf(buf, "00");
	return 0;
}

static int mtk_get_countrylist(const char *ifname, char *buf, int *len)
{
	/* Stub */
	return -1;
}

static int mtk_get_hwmodelist(const char *ifname, int *buf)
{
	return -1;
}

static int mtk_get_encryption(const char *ifname, char *buf)
{
	/* No reliable crypto info in mtk */
	return -1;
}

static int mtk_get_phyname(const char *ifname, char *buf)
{
	/* No suitable api in mtk */
	strcpy(buf, ifname);
	return 0;
}

static int mtk_get_mbssid_support(const char *ifname, int *buf)
{
	return -1;
}

static int mtk_get_hardware_id(const char *ifname, char *buf)
{
	return -1;
}

static int mtk_get_hardware_name(const char *ifname, char *buf)
{
	sprintf(buf, "Generic Mediatek/Ralink");
	return 0;
}

static int mtk_get_txpower_offset(const char *ifname, int *buf)
{
	/* Stub */
	*buf = 0;
	return -1;
}

static int mtk_get_frequency_offset(const char *ifname, int *buf)
{
	/* Stub */
	*buf = 0;
	return -1;
}

const struct iwinfo_ops mtk_ops = {
	.name             = "mtk",
	.probe            = mtk_probe,
	.channel          = mtk_get_channel,
	.frequency        = mtk_get_frequency,
	.frequency_offset = mtk_get_frequency_offset,
	.txpower          = mtk_get_txpower,
	.txpower_offset   = mtk_get_txpower_offset,
	.bitrate          = mtk_get_bitrate,
	.signal           = mtk_get_signal,
	.noise            = mtk_get_noise,
	.quality          = mtk_get_quality,
	.quality_max      = mtk_get_quality_max,
	.mbssid_support   = mtk_get_mbssid_support,
	.hwmodelist       = mtk_get_hwmodelist,
	.mode             = mtk_get_mode,
	.ssid             = mtk_get_ssid,
	.bssid            = mtk_get_bssid,
	.country          = mtk_get_country,
	.hardware_id      = mtk_get_hardware_id,
	.hardware_name    = mtk_get_hardware_name,
	.encryption       = mtk_get_encryption,
	.phyname          = mtk_get_phyname,
	.assoclist        = mtk_get_assoclist,
	.txpwrlist        = mtk_get_txpwrlist,
	.scanlist         = mtk_get_scanlist,
	.freqlist         = mtk_get_freqlist,
	.countrylist      = mtk_get_countrylist,
	.close            = mtk_close
};
