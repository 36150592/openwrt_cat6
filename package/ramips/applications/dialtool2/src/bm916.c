/*
	This file define all about module ,depend on bm817c at  command set  V1.0.4
*/
#include <unistd.h>
#include <signal.h>
#include <errno.h>

//#include "common.h"
#include "bm916.h"
#include "include.h"
//#include "boardtype_306A.h"
extern int global_sleep_interval_long;
extern CSA scan_att_configs;

#if 0
struct ASBM806
{
	const char* at_cgmi;			//query manufacturer
	const char* at_cgmm;		//query module hardware version
	const char* at_qcgmm;		//query module hardware version
	const char* at_cgmr;			//query module softvware version
	const char* at_cgsn;			//query module imei
	const char* at_cimi;			//query module imsi
	const char* at_iccid;			//query sim card iccid
	const char* ati;				//query manufacturer info
	const char* at_bmswver;		//query detail module software version
#if 0
	const char* at_csta;			//set phone number type
	const char* at_cmod;		//set call type,only support single mode
	const char* att;				//set call type audio call
	const char* atp;				//set call type pulse call
	const char* atd;				//mobile station call
	const char* atdl;			//redial last phone number
	const char* atd_mem;		//call phone number from telephone book
	const char* atd_n;			//call phone number from memory cell 
	const char* ata;				//answer call 
	const char* at_chup;			//hang on call or cancel call out
	const char* ath;				//hang up call
	const char* at_cbst;			//choose data call business type
	const char* at_clcc;			//list current call of ME
#endif
	const char* at_crlp;			//set radio link protocol
	const char* at_cr;			//whether enable intermediate result report from TA to TE
	const char* at_ceer;			//extend error report for TA 
	const char* at_cscs;			//set TA used const character set
	const char* at_cfun;			//set TA fun level
	const char* at_repeat;		//cmd A/will repeater last AT cmd
	const char* at_W;			//AT&W save some user configration
	const char* atz;				//recover configration saved by AT&W
	const char* at_f;			//AT&F recover factory configration of TA
	const char* at_gcap;			//return additional function list
	const char* ats3;			//set const character of cmd end
	const char* ats4;			//set const character of cmd reponse
	const char* ats5;			//set const character of delete const character way
	const char* ate;				//whether echo TA received const characters or not
	const char* ats0;			//set times of ringing before auto reponse call
	const char* ats6;			//delay time of blind dial,default 2 second,i don't understand
	const char* atq;				//whether return result code from TA to TE  or not
	const char* atv	;			//set format of result code 
	const char* atx;				//whether return special result code from TA to TE or not
	const char* at_cclk;			//set real-time clock
	const char* at_cmee;		//set report error format from TA
	const char* at_cmer;			//whether report unsolicited result or not
	
	const char* at_creg;			//query network register info or search network by force
	const char* at_cereg;		//query lte network register status or seach lte network by force
	const char* at_cops;			//set operator or query current operator
	const char* at_csq;			//query signal quality
	const char* at_signalind;		//whether enable  indicator of signal const charnged or not
	const char* at_bmmododr;	//set search network mode
	const char* at_ws46;		//choose wireless network according bmmododr
	const char* at_bmrat;		//query network service type
	const char* at_ctzu;			//whether enable auto update time zone  or not ,it relate to ME function
	const char* at_ctzr;			//whether enable report time zone const charnged or not 
	const char* at_nwtypeind;		//whether enable report network const charnged  or not
	const char* at_urcind;		//whether enable report init process of TA or not
	const char* at_bmtcellinfo;	//get cell info
	const char* at_bmbandpref;	//get support band or set search network band
	const char* at_nwlcfg;		//query plmn lock configration:network lock configration
	const char* at_nwlplmn;		//add ,edit or delete plmn list
	const char* at_nwlset;		//lock or unlock plmn
	const char* at_psdialind;		//indicat network type while dialing
	
	const char* at_clck;			//device lock, PIN lock and unlock
	const char* at_cpin;			//PIN  or PUK input and query
	const char* at_cpwd;		//const charnge PIN  of +clck set 
	const char* at_bmcpncnt;		//query pin or puk times of try
#if 0
	const char* at_cpas;			//activity of ME
	const char* at_fclass;		//set detail work mode of TA,such as data,fax,audio
	const char* at_clac;			//query all available AT cmd
	const char* at_clip;			//enable of disable CID(Caller Identification),active
	const char* at_colp;			//enable of disable CTID(Called to identify),passive
	const char* at_ccug;			//control CUG supplementary service 
	const char* at_ccfc;			//control supplementary service of call before
	const char* at_ccwa;			//control supplementary service of call wait
	const char* at_chld;			//call hold,keep muti  call connected
	const char* at_cusd;			//enable or disable UUSD result code
	const char* at_caoc;			//query call costs
	const char* at_cssn;			//enable or disable supplementary service report
#endif
	const char* at_cpol;			//set plmns priority
	const char* at_copn;			//query known operator name
	const char* at_cgdcont;		//set PDP context
	const char* at_cgdscont;		//set second PDP context
	const char* at_cgqreq;		//query service quality report,or set qos 
	const char* at_cgeqreq;		//query UMTS network quality report .or set qos
	const char* at_cgqmin;		//query acceptable min service quality
	const char* at_cgeqmin;		//query 3g acceptable min service quality
	const char* at_cgatt;			//attach to gprs or disconnect to grps
	const char* at_cgact;		//activate or deactivate PDP
	const char* at_cgdata;		//establish internet comunication
	const char* at_cgpaddr;		//display PDP address
	const char* at_cgclass;		//query gprs mobile station type
	const char* at_cgerep;		//enable or disable MT report result code to TE
	const char* at_cgreg;		//GPRS network regist status
	const char* at_cgsms;		//set service or serivce priority of GPRS and CS(circuit switching) when MT send MO SMS
	const char* at_qcpdpp;		//AT$QCPDPP set dial authentication method , user and password
	
	const char* at_csms;		//set ability of SMS(short message service)
	const char* at_cpms;		//set message storage area
	const char* at_cmgf;			//set message format:pdu or text
	const char* at_csca;			//set SMSC(Short Message Service Center)
	const char* at_csmp;		//set  some parms that SMS text format  need
	const char* at_csdh;			//enable or disable display detail head of SMS,for text format result code
	const char* at_cscb;			//choose accept cell boradcast type
	const char* at_csas;			//save +csca,+csmp,+cscb parms permanently
	const char* at_cres;			//recover factory set of +csca,+csmp,+cscb
	const char* at_cnmi;			//send new message indicator to TE
	const char* at_cmgl;			//query news ,and set unread messages status to read
	const char* at_cmgr;			//read out messages,and set messages status into received
	const char* at_cnma;		//judge new messages received right or error
	const char* at_cmgs;		//send messages
	const char* at_cmgw;		//write message into storage
	const char* at_cmss;		//send messages from storage
	const char* at_cmgd;		//delete message
	
	const char* at_gpsmode;		//set gps mode
	const char* at_gpsstart;		//start gps
	const char* at_gpssend;		//stop gps report data
	
	const char* at_cpbs;			//choose storage block for telephone book
	const char* at_cpbr;			//read record of telephone book
	const char* at_cpbw;		//write record into telephone book
	const char* at_cpbf;			//query specified record in telephone book
	const char* at_cnum;		// query own number form SIM,need SIM support

	const char* at_bm3gpp2authinfo;	//set ppp dial user and password 
	const char* at_bm3gpp2cgdcont;	//set 3gpp2 PDP context set
	const char* at_hdrcsq;			//at^hdrcsq,signal indicator query in HDR mode
	const char* at_sysinfo;			//at^sysinfo,query system network register status
	const char* at_prefmode;			//at^prefmode,force ME register expect network mode
	const char* at_qclck;				//lock UIM,set PIN in 3gpp2 mode
	const char* at_qcpwd;			//const charnge PIN in 3gpp2 mode
	const char* at_qcpin;			//input PIN in 3gpp2 mode
	const char* atcpin;				//at^cpin extend PIN manage cmd
	const char* at_reset;				//at^reset reset module
	const char* at_ipr;				//set fixed rate TE-TA
	
	const char* at_qcpms;			//at$qcpms ,set SM storage position in 3gpp2 mode
	const char* at_qcmgf;			//at$qcmgf, set SMS format :pdu or text in 3gpp2 mode
	const char* at_qcnmi;			//at$qcnmi, send new message indicator to TE in 3gpp2 mode
	const char* at_hcmt;				// at^hcmt,set parms of HCMT,sometimes new message indicator use ^HCMT in 3gpp2 mode
	const char* at_qcmgd;			// at$qcmgd,delete message in 3gpp2 mode
	const char* at_hsmsss;			// at^hsmsss,SM parms choose cmd in 3gpp2 mode
	const char* at_qcmgl;			// at$qcmgl, check SM list status in 3gpp2 mode
	const char* at_qcmgr;			// at$qcmgr,read a SM in 3gpp2 mode
	const char* at_qcmgw;			// at$qcmgw,storage SM
	const char* at_qcmgs;			// at$qcmgs,send  SM
	const char* at_qcmss;			// at$qcmss,send SM of storage
	
	const char* at_bmbandlock;		//lock FDD or TDD,or FDD and TDD
	const char* at_qcrmcall;			//at$qcrmcall,ndis dial cmd
	const char* at_nwminden;			//enable or disable mode and hrssilvl report 
	const char* at_bmmobapref;		//module  frequency locking
	const char* at_ehrpdenable;		//enable or disable ehrpd
	const char* at_bmdatastatusen;	//enable or disable report when data link  changed
	const char* at_bmdatastatus;		//query current data link status
	const char* at_bmnvuart;			//open or close uart of module
	const char* at_bmsimtype;		//query SIM type
	const char* at_curc;				//enable or disable nwtime report
	const char* at_cellinfo;			//query cell info
	const char* at_bmoptmod;		//query communication protcol:3gpp or 3gpp2
	const char* at_bmband;			//query current register BAND
	const char* at_simact;			//set SIM register process
	const char* at_bmrmcallstat;		//query flow statics
	const char* at_bmcat;			//query max rate of module support
	const char* at_dhcp4;			//query ip ,mask,gw,dns1,dns2
	const char* at_mipprofile;			//tcp/ip PDP set
	const char* at_mipcall;			//control  ggsn ppp connect
	const char* at_mipopen;			//init socket which connect remote host
	const char* at_mipclose;			//close socket
	const char* at_miplisten;			//establish a TCP or UDP server,listen
	const char* at_mipsrvclose;		//close server
	const char* at_mipsend;			//send data to buff
	const char* at_mippush;			//send buff to remote host
	const char* at_mipwrite;			//send data to remote host
	const char* at_mipflush;			//flush socket buff
	const char* at_mipdnsr;			//query domain address correspond to IP address
	const char* at_mping;			//just a ping function
	const char* at_bmimei;			//query imei in 3gpp and 3gpp2
	
}AT_SET_BM806=
{
	"at+cgmi",		//query manufacturer
	"at+cgmm",		//query module hardware version
	"at+qcgmm",		//query module hardware version
	"at+cgmr",		//query module softvware version
	"at+cgsn",			//query module imei
	"at+cimi",			//query module imsi
	"at+iccid",			//query sim card iccid
	"ati",				//query manufacturer info
	"at+bmswver",		//query detail module software version
#if 0
	"at+csta",			//set phone number type
	"at+cmod",		//set call type,only support single mode
	"att",				//set call type audio call
	"atp",			//set call type pulse call
	"atd",			//mobile station call
	"atdl",			//redial last phone number
	"atd+mem",		//call phone number from telephone book
	"atd+n",			//call phone number from memory cell 
	"ata",			//answer call 
	"at+chup",			//hang on call or cancel call out
	"ath",			//hang up call
	"at+cbst",			//choose data call business type
	"at+clcc",			//list current call of ME
#endif
	"at+crlp",			//set radio link protocol
	"at+cr",			//whether enable intermediate result report from TA to TE
	"at+ceer",			//extend error report for TA 
	"at+cscs",			//set TA used const character set
	"at+cfun",			//set TA fun level
	"A/",			//cmd A/will repeater last AT cmd
	"at&W",			//AT&W save some user configration
	"atz",			//recover configration saved by AT&W
	"at&f",			//AT&F recover factory configration of TA
	"at+gcap",		//return additional function list
	"ats3",			//set const character of cmd end
	"ats4",			//set const character of cmd reponse
	"ats5",			//set const character of delete const character way
	"ate",			//whether echo TA received const characters or not
	"ats0",			//set times of ringing before auto reponse call
	"ats6",			//delay time of blind dial,default 2 second,i don't understand
	"atq",			//whether return result code from TA to TE  or not
	"atv	",			//set format of result code 
	"atx",			//whether return special result code from TA to TE or not
	"at+cclk",			//set real-time clock
	"at+cmee",		//set report error format from TA
	"at+cmer",		//whether report unsolicited result or not
	
	"at+creg",			//query network register info or search network by force
	"at+cereg",		//query lte network register status or seach lte network by force
	"at+cops",		//set operator or query current operator
	"at+csq",			//query signal quality
	"at+signalind",		//whether enable  indicator of signal const charnged or not
	"at+bmmododr",	//set search network mode
	"at+ws46",		//choose wireless network according bmmododr
	"at+bmrat",		//query network service type
	"at+ctzu",			//whether enable auto update time zone  or not ,it relate to ME function
	"at+ctzr",			//whether enable report time zone const charnged or not 
	"at+nwtypeind",		//whether enable report network const charnged  or not
	"at+urcind",		//whether enable report init process of TA or not
	"at+bmtcellinfo",	//get cell info
	"at+bmbandpref",	//get support band or set search network band
	"at+nwlcfg",		//query plmn lock configration:network lock configration
	"at+nwlplmn",		//add ,edit or delete plmn list
	"at+nwlset",		//lock or unlock plmn
	"at+psdialind",		//indicat network type while dialing

	"at+clck",			//device lock, PIN lock and unlock
	"at+cpin",			//PIN  or PUK input and query
	"at+cpwd",		//const charnge PIN  of +clck set 
	"at+bmcpncnt",		//query pin or puk times of try
#if 0
	"at+cpas",		//activity of ME
	"at+fclass",		//set detail work mode of TA,such as data,fax,audio
	"at+clac",			//query all available AT cmd
	"at+clip",			//enable of disable CID(Caller Identification),active
	"at+colp",			//enable of disable CTID(Called to identify),passive
	"at+ccug",			//control CUG supplementary service 
	"at+ccfc",			//control supplementary service of call before
	"at+ccwa",		//control supplementary service of call wait
	"at+chld",			//call hold,keep muti  call connected
	"at+cusd",			//enable or disable UUSD result code
	"at+caoc",		//query call costs
	"at+cssn",			//enable or disable supplementary service report
#endif
	"at+cpol",			//set plmns priority
	"at+copn",		//query known operator name
	"at+cgdcont",		//set PDP context
	"at+cgdscont",		//set second PDP context
	"at+cgqreq",		//query service quality report,or set qos 
	"at+cgeqreq",		//query UMTS network quality report .or set qos
	"at+cgqmin",		//query acceptable min service quality
	"at+cgeqmin",		//query 3g acceptable min service quality
	"at+cgatt",		//attach to gprs or disconnect to grps
	"at+cgact",		//activate or deactivate PDP
	"at+cgdata",		//establish internet comunication
	"at+cgpaddr",		//display PDP address
	"at+cgclass",		//query gprs mobile station type
	"at+cgerep",		//enable or disable MT report result code to TE
	"at+cgreg",		//GPRS network regist status
	"at+cgsms",		//set service or serivce priority of GPRS and CS(circuit switching) when MT send MO SMS
	"at$qcpdpp",		//AT$QCPDPP set dial authentication method , user and password

	"at+csms",		//set ability of SMS(short message service)
	"at+cpms",		//set message storage area
	"at+cmgf",		//set message format:pdu or text
	"at+csca",			//set SMSC(Short Message Service Center)
	"at+csmp",		//set  some parms that SMS text format  need
	"at+csdh",			//enable or disable display detail head of SMS,for text format result code
	"at+cscb",			//choose accept cell boradcast type
	"at+csas",			//save +csca,+csmp,+cscb parms permanently
	"at+cres",			//recover factory set of +csca,+csmp,+cscb
	"at+cnmi",			//send new message indicator to TE
	"at+cmgl",			//query news ,and set unread messages status to read
	"at+cmgr",		//read out messages,and set messages status into received
	"at+cnma",		//judge new messages received right or error
	"at+cmgs",		//send messages
	"at+cmgw",		//write message into storage
	"at+cmss",		//send messages from storage
	"at+cmgd",		//delete message

	"at+gpsmode",		//set gps mode
	"at+gpsstart",		//start gps
	"at+gpssend",		//stop gps report data

	"at+cpbs",		//choose storage block for telephone book
	"at+cpbr",			//read record of telephone book
	"at+cpbw",		//write record into telephone book
	"at+cpbf",			//query specified record in telephone book
	"at+cnum",		// query own number form SIM,need SIM support

	"at+bm3gpp2authinfo",	//set ppp dial user and password 
	"at+bm3gpp2cgdcont",	//set 3gpp2 PDP context set
	"at+hdrcsq",			//at^hdrcsq,signal indicator query in HDR mode
	"at^sysinfo",			//at^sysinfo,query system network register status
	"at+prefmode",			//at^prefmode,force ME register expect network mode
	"at+qclck",			//lock UIM,set PIN in 3gpp2 mode
	"at+qcpwd",			//change PIN in 3gpp2 mode
	"at+qcpin",			//input PIN in 3gpp2 mode
	"at^cpin",				//extend PIN manage cmd
	"at^reset",				//reset module
	"at+ipr",				//set fixed rate TE-TA

	"at$qcpms",			//at$qcpms ,set SM storage position in 3gpp2 mode
	"at$qcmgf",			//at$qcmgf, set SMS format :pdu or text in 3gpp2 mode
	"at$qcnmi",			//at$qcnmi, send new message indicator to TE in 3gpp2 mode
	"at^hcmt",				// at^hcmt,set parms of HCMT,sometimes new message indicator use ^HCMT in 3gpp2 mode
	"at$qcmgd",			// at$qcmgd,delete message in 3gpp2 mode
	"at^hsmsss",			// at^hsmsss,SM parms choose cmd in 3gpp2 mode
	"at$qcmgl",			// at$qcmgl, check SM list status in 3gpp2 mode
	"at$qcmgr",			// at$qcmgr,read a SM in 3gpp2 mode
	"at$qcmgw",			// at$qcmgw,storage SM
	"at$qcmgs",			// at$qcmgs,send  SM
	"at$qcmss",			// at$qcmss,send SM of storage

	"at+bmbandlock",		//lock FDD or TDD,or FDD and TDD
	"at$qcrmcall",			//at$qcrmcall,ndis dial cmd
	"at+nwminden",		//enable or disable mode and hrssilvl report 
	"at+bmmobapref",		//module  frequency locking
	"at+ehrpdenable",		//enable or disable ehrpd
	"at+bmdatastatusen",	//enable or disable report when data link  changed
	"at+bmdatastatus",		//query current data link status
	"at+bmnvuart",			//open or close uart of module
	"at+bmsimtype",		//query SIM type
	"at+curc",				//enable or disable nwtime report
	"at+cellinfo",			//query cell info
	"at+bmoptmod",		//query communication protcol:3gpp or 3gpp2
	"at+bmband",			//query current register BAND
	"at+simact",			//set SIM register process
	"at+bmrmcallstat",		//query flow statics
	"at+bmcat",			//query max rate of module support
	"at+dhcp4",			//query ip ,mask,gw,dns1,dns2
	"at+mipprofile",			//tcp/ip PDP set

	"at+mipcall",			//control  ggsn ppp connect
	"at+mipopen",			//init socket which connect remote host
	"at+mipclose",			//close socket
	"at+miplisten",			//establish a TCP or UDP server,listen
	"at+mipsrvclose",		//close server
	"at+mipsend",			//send data to buff
	"at+mippush",			//send buff to remote host
	"at+mipwrite",			//send data to remote host
	"at+mipflush",			//flush socket buff
	"at+mipdnsr",			//query domain address correspond to IP address
	"at+mping",			//just a ping function
	"at+bmiei",			//query module imei in 3gpp and 3gpp2,result format is ascii
};
#endif

/*reporter indicator define*/
/*
network_web_mode:value
1-4g only 5
2-4g pro 2(TDS) or 11(CDMA) or 17(WCDMA)
3-3g only 7 or 10(EVDO) or 1:WCDMA ONLY
4-3g pro 4(GSM) or 12(EVDO) or 16(LTE) 18(GSM) or 19(LTE)
0-2g 3g 21:WCDMA+GSM 14:CDMA+EVDO 8:TDS+GSM 


*/
MCT bm916c_mode_table[]=
{
	{"China",	"China Mobile",	"46000",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,8}},},
	{"China",	"China Mobile",	"46002",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,8}},},
	{"China",	"China Mobile",	"46004",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},
	{"China",	"China Mobile",	"46007",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},
	{"China",	"China Telecom",	"46003",	{{1,5},	{2,11},	{3,10},	{4,12},	{0,14}},},
	{"China",	"China Telecom",	"46005",	{{1,5},	{2,11},	{3,10},	{4,12},	{0,14}},	},
	{"China",	"China Telecom",	"46011",	{{1,5},	{2,11},	{3,10},	{4,12},	{0,14}},},
	{"China",	"China TieTong",	"46020",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},			//TieTong card can not query,use China Mobile
	{"China",	"China Unicom",	"46001",	{{1,5},	{2,17},	{3,1},	{4,19},	{0,21}},},
//	{"Sri Lanka",	"Airtel",	"41305",	{{3,5},	{0,17},	{13,1},	{18,21},	{20,3}},},
	{"default",	"default",	"default",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},					//default use China Mobile mode
};

const MCT bm916_mode_table[]=
{
	{"China",	"China Mobile",	"46000",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,8}},},
	{"China",	"China Mobile",	"46002",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,8}},},
	{"China",	"China Mobile",	"46004",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},
	{"China",	"China Mobile",	"46007",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},
	{"China",	"China Telecom",	"46003",	{{1,5},	{2,11},	{3,10},	{4,12},	{0,14}},},
	{"China",	"China Telecom",	"46005",	{{1,5},	{2,11},	{3,10},	{4,12},	{0,14}},},
	{"China",	"China Telecom",	"46011",	{{1,5},	{2,11},	{3,10},	{4,12},	{0,14}},},
	{"China",	"China TieTong",	"46020",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},			//TieTong card can not query,use China Mobile
	{"China",	"China Unicom",	"46001",	{{1,5},	{2,17},	{3,1},	{4,19},	{0,21}},},
//	{"Sri Lanka",	"Airtel",	"41305",	{{3,5},	{0,17},	{13,1},	{18,21},	{20,3}},},
	{"default",	"default",	"default",	{{1,5},	{2,2},	{3,7},	{4,16},	{0,21}},},					//default use China Mobile mode
};

#if 0
//module hardware BM806C and BM806U-C1 support evdo and cdma1x,other don't support
static MPAIR support_mode_bm806c[]=
{
	{0,11,"LTE>TDS>GSM>WCDMA>HDR>CDMA"},
//	{0,2,"4G/3G/2G Multi-mode"},
	{1,13,"LTE+EVDO"},
	{2,14,"EVDO+CDMA"},
	{3,5,"4g only"},
//	{4,,"3g only"},
//	{5,,"Automatic(LTE>TDS>other)"},
//	{6,,"Automatic(LTE>UMTS>other)"},
//	{7,,"Automatic(LTE>TDS>UMTS>other)"},
	{8,11,"Automatic(LTE>EVDO>CDMA>TDS>UMTS>GSM)"},
//	{9,,"Automatic(LTE>UMTS>TDS>other)"},
//	{10,,"LTE+UMTS(4G+3G only)"},
	{11,13,"LTE+EVDO(4G+3G only)"},
	{12,9,"LTE+TDS(4G+3G only)"},
	{13,7,"WCDMA/TDS-CDMA"},
	{14,1,"WCDMA Only(3G Only)"},
	{15,10,"EVDO only(3G only)"},
	{16,6,"TDS-CDMA only(3G only)"},
//	{17,8,"UMTS+GSM(3G+2G only)"},   //17 and 18 is the same UMTS Include tds and wcdma
	{18,8,"TDS+GSM(3g+2g only)"},
	{19,14,"EVDO+CDMA Only(3G+2G Only)"},
	{20,3,"GSM+CDMA Only"},
//	{21,,"GSM Only"},
	{22,15,"CDMA Only"},
};

static MPAIR support_mode_bm806[]=
{
//	{ 0,2,"4G/3G/2G Multi-mode" },
	{0,2,"LTE>TDS>GSM>WCDMA>HDR>CDMA"},

	{ 3,5,"Only 4G" },
	{1,9,"LTE+TDS"},
	{2,8,"TDS+GSM"},
//	{ 17,"Automatic(LTE>UMTS>TDS>Other)" },

//	{ 20,"LTE+UMTS Only(4G+3G Only)" },
	{12, 9,"LTE+TDS Only(4G+3G Only)" },

	{ 13,7,"WCDMA/TDS-CDMA(3G Only)" },
	{ 14,1,"WCDMA Only(3G Only)" },
	{ 16,6,"TDS-CDMA Only(3G Only)" },


//	{ 21,"UMTS+GSM Only(3G+2G Only)" },
	{ 18,8,"TDS+GSM Only(3G+2G Only)" },
};

#endif
static const char* bm916_hardware_summary[]=
{
	"BM906"
};

 
const MDI bm916_moduleinfo=
{
	{TZ_VERDOR_BM916_ID,TZ_PRODUCT_BM916_ID},
	"2-1:1.2",
	bm916_get_moduleinfo,
	"0",
	"0",
	"0",
	"0",
	"bm_gobi_net_906.ko",
	"bmwan0",
}; 

static int check_cpin_num = 0;

static void  prepare_v6_environment()
{
	char bmwan0_local_ipv6_exist[64] = "";
	read_memory("ifconfig bmwan0 | grep fe80:c810",bmwan0_local_ipv6_exist ,sizeof(bmwan0_local_ipv6_exist));
	if (bmwan0_local_ipv6_exist[0] == 0 )
	{
		system("ifconfig bmwan0 down");//ifconfig bmwan0 down
		system("ip addr add dev bmwan0 fe80:C810:3001:D00::3/56");
		system("ifconfig bmwan0 up");
	}
	
	system("pgrep -f odhcp6c.*bmwan0 | xargs kill ");
}

int down_udhcpc(char *card_name)
{
	FILE* pstr; char cmd[128],buff[512],pidfile[128];
	memset(pidfile,0,sizeof(pidfile));
    sprintf(pidfile, "/var/run/udhcpc-%s.pid",card_name);
    if(check_file_exist(pidfile))
    {
      FILE *file = NULL;
      file = fopen(pidfile,"r");
      if(NULL == file)
        return 1;

      memset(buff,0,sizeof(buff));
      fgets(buff,512,file);
      log_info("buff = %s", buff);
	  fclose(file);
      if(strlen(buff)==0)
        return 1; 

      memset(cmd,0,sizeof(cmd));
      snprintf(cmd,sizeof(cmd),"kill -9 %s", buff);
      int ret = system(cmd);
      log_info("down udhcpc ret: %d\n",ret);

	  if(ret != 0)
	  {
			memset(cmd,0,sizeof(cmd));
			log_info("card_name = %s", card_name);
			sprintf(cmd, "pgrep -f udhcpc.*%s",card_name);
			pstr=popen(cmd, "r");

			if(pstr==NULL)
			  return 1; 
			memset(buff,0,sizeof(buff));
			fgets(buff,512,pstr);
			pclose(pstr);
			log_info("buff = %s", buff);
			if(strlen(buff)==0)
			  return 1; 

			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"kill -9 %s", buff);
			ret = system(cmd);	
			log_info("down udhcpc ret: %d\n",ret);
	  }

	  sleep(3);
    }

	return 0;
}



int up_udhcpc(char *card_name)
{
	  	FILE *pstr; char cmd[128],buff[512];
		int num = 0;
		memset(cmd,0,sizeof(cmd));
		
		log_info("card_name = %s", card_name);
		sprintf(cmd, "ps|grep 'udhcpc' | grep %s | grep -v grep | wc -l ",card_name);
		pstr=popen(cmd, "r");
		
		if(pstr==NULL)
			return 1; 
		memset(buff,0,sizeof(buff));
		fgets(buff,512,pstr);
		pclose(pstr);
		log_info("buff = %s", buff);
		if(strlen(buff)==0)
			return 1; 
		log_info("num: %d\n",atoi(buff));
		
		if((num=atoi(buff))==0)
		{
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"udhcpc -b -i %s -p %s -s /etc/udhcpc.script ",
					global_system_info.module_info.network_card_name,UDHCPC_PIDFILE_PATH);
			system(cmd);
		}
		
		return 0;
}

static int get_network_mode_module(int network_mode_web,char* module_hardware)
{
	int i,j;
	const MCT *p=NULL;
	int len=0;
	for(i=0;i<ARRAY_MEMBER_COUNT(bm916_hardware_summary);i++)
	{
		if(!strncmp(bm916_hardware_summary[i],module_hardware,strlen(bm916_hardware_summary[i])))
			break;
	}
	switch(i)
	{
		case 0:
		case 4:
			p=bm916c_mode_table;
			len=ARRAY_MEMBER_COUNT(bm916c_mode_table);
			break;
		default:
			p=bm916_mode_table;
			len=ARRAY_MEMBER_COUNT(bm916_mode_table);
			break;


	}
	for(j=0;j<len;j++)
	{
		log_info("imsi = %s, operator_num = %s",global_dial_vars.imsi, (p+j)->operator_num);
		if(A_CMP_B_ACCORDING_B(global_dial_vars.imsi,(p+j)->operator_num))
		{
			for(i=0;i<5;i++)
			{
				if(((p+j)->web_mode_table+i)->web_mode == network_mode_web)
					break;					
			}
			break;
		}
	}
	if(j>=len)
	{
		for(i=0;i<5;i++)
		{
			if(((p+len-1)->web_mode_table+i)->web_mode == network_mode_web)
				break;					
		}
		if(i>=5)
		{
			return ((p+len-1)->web_mode_table+1)->real_mode;
//			return FALSE;
		}
		else
		{
			char buffer[8];
			snprintf(buffer,6,"%s",global_dial_vars.imsi);
			strncpy(global_dial_vars.operator_current,buffer,6);
			return ((p+len-1)->web_mode_table+i)->real_mode;
		}
	}
	else
	{
		if(i>=5)
		{
			return ((p+len-1)->web_mode_table+1)->real_mode;
//			return FALSE;
		}
		else
		{
			strncpy(global_dial_vars.operator_current,(p+j)->operator_str,strlen((p+j)->operator_str));
			return ((p+j)->web_mode_table+i)->real_mode;
		}
	}
	
}

void check_band_bit(int max_bit, char *band_bit_str, char * output_str)
{
/*	if(strlen(band_bit_str)<max_bit)
	{
		snprintf(output_str,max_bit+1,"%0*d%s",max_bit-strlen(band_bit_str),0,band_bit_str);
	}
	else
	{*/
		int i = 0;
		for(i=0; i < strlen(band_bit_str); i++)
		{
			if(band_bit_str[i] !='0')
				break;
		}
		if(i == strlen(band_bit_str))
			snprintf(output_str,max_bit+1,"%s","0" );
		else
			snprintf(output_str,max_bit+1,"%s",&band_bit_str[i] );
	//}


}

void bm916_sendat(int num)
{
	switch(global_dialtool.Dial_proc_state)
	{
		case Dial_State_initialized:
			util_send_cmd(global_dialtool.dev_handle,"ATE0\r",&global_dialtool.pthread_moniter_flag);			//test AT
			break;
		case Dial_State_CMEE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CMEE=1\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case Dial_State_PLMN_LOCK_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+NWLSET?\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case Dial_State_PLMN_LOCK:
			{
				char cmd_buffer[64] = {0};
				if ('0' == global_init_parms.plmn_lock[0] || '\0' == global_init_parms.plmn_lock[0])
					strcpy(cmd_buffer,"AT+NWLSET=0\r");
				else// plmn_lock like : "46011,46002" 
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+NWLSET=1,\"%s\"\r",global_init_parms.plmn_lock);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);  	
				break;
			}
		case Dial_State_PCI_LOCK_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMPCILOCK=2,2\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case Dial_State_PCI_LOCK:
			{
				char cmd_buffer[64] = {0};
				if('\0' == global_init_parms.lte_pci_lock[0] ||
					'\0' == global_init_parms.lte_earfcn_lock[0] )
					strcpy(cmd_buffer,"AT+BMPCILOCK=0,2\r");
				else
				{
					int pci = atoi(global_init_parms.lte_pci_lock );
					int earfcn = atoi(global_init_parms.lte_earfcn_lock );
					if(pci>=0 && pci<=503 && earfcn >=0 && earfcn <= 65535)
						sprintf(cmd_buffer,"AT+BMPCILOCK=1,2,%d,%d\r",earfcn, pci);
					else
						strcpy(cmd_buffer,"AT+BMPCILOCK=0,2\r");
				}
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag); 
			}
			break;
		case Dial_State_ICCID:
			util_send_cmd(global_dialtool.dev_handle,"AT+ICCID\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPIN_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CPIN?\r",&global_dialtool.pthread_moniter_flag);
			break;			
		case Dial_State_QCPIN_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+QCPIN?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_ATI:
			util_send_cmd(global_dialtool.dev_handle,"ATI\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMSIMTYPE:
			util_send_cmd(global_dialtool.dev_handle,"AT+bmsimtype?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPIN_SET:
			{
				char* pin_code=global_init_parms.pin;
				char cmd_buffer[64];
				if(global_dial_vars.device_lock_pin)
				{
					if(strlen(pin_code))
					{
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+CPIN=%s\r",pin_code);
						util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
					}
				}
				break;
			}
		case Dial_State_QCPIN_SET:
			{
				char* pin_code=global_init_parms.pin;
				char cmd_buffer[64];
				if(global_dial_vars.device_lock_pin)
				{
					if(strlen(pin_code))
					{
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+QCPIN=%s\r",pin_code);
						util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
					}
				}
				break;
			}
		case Dial_State_CIMI:
			util_send_cmd(global_dialtool.dev_handle,"AT+CIMI\r",&global_dialtool.pthread_moniter_flag);  //mode 3gpp2 can use also
			break;
		case Dial_State_QCIMI:
			util_send_cmd(global_dialtool.dev_handle,"AT+QCIMI\r",&global_dialtool.pthread_moniter_flag);  //mode 3gpp2 can use also
			break;			
		case Dial_State_CPUK_SET:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CLCK_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CLCK_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPWD:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMDATASTATUSEN:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMDATASTATUSEN=0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_PSDIALIND:
			util_send_cmd(global_dialtool.dev_handle,"AT+PSDIALIND=0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMTCELLINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMTCELLINFO\r",&global_dialtool.pthread_moniter_flag);
			break;	
		case Dial_State_CFUN_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CFUN_DISABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CFUN_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMMODODR:
			{
				char cmd_buffer[64];
				int network_mode_module=get_network_mode_module(global_init_parms.network_mode_web,global_system_info.module_info.hardver);
				if(FALSE == network_mode_module)
				{
					log_error("module dont support the network mode\n");
					write_str_file(DIAL_INDICATOR,"module don't support the network mode","w+");
					exit(-1);					
				}
				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BMMODODR=%d\r",network_mode_module);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BMMODODR_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMMODODR?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CEREG:
			util_send_cmd(global_dialtool.dev_handle,"AT+CEREG=2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CEREG_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CEREG?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGDCONT:
			{
				char cmd_buffer[64];

				//add the apn setting to support the request of the scan at&t sim card
				if(strcmp(scan_att_configs.scan_att_check_enable.Value, "yes") == 0)
				{
					//log_info("scan_att_check_enable:%s scan_att_mcc_mnc:%s scan_att_apn_name:%s\n",
					//scan_att_configs.scan_att_check_enable.Value,
					//scan_att_configs.scan_att_mcc_mnc.Value,
					//scan_att_configs.scan_att_apn_name.Value);
					if(A_CMP_B_ACCORDING_B(global_dial_vars.imsi, scan_att_configs.scan_att_mcc_mnc.Value))
					{
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+CGDCONT=1,\"%s\",\"%s\",,0,0\r",global_init_parms.ipstack,scan_att_configs.scan_att_apn_name.Value);
						util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
						break;
					}
				}

				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+CGDCONT=1,\"%s\",\"%s\",,0,0\r",global_init_parms.ipstack,global_init_parms.apn);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_CGDCONT2:
			{
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				char cmd_buffer[256];
				int ipstack=2;

				if( !strncmp( global_init_parms.ipstack,"IPV4V6",6 ) )
				{
					ipstack=2;
				}
				else if( !strncmp( global_init_parms.ipstack,"IPV6",4 ) )
				{
					ipstack=1;
				}
				else if( !strncmp( global_init_parms.ipstack,"IP",2) )
				{
					ipstack=0;
				}
				if(strlen(global_init_parms.apn) == 0)
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BM3GPP2CGDCONT=0,3,ctnet@mycdma.cn,vnet.mobi,ctnet,2,3,0\r");
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BM3GPP2CGDCONT=0,%s,%s,%s,%s,%d,3,0\r",
						global_init_parms.ppp_auth_type,global_init_parms.ppp_username,
						global_init_parms.ppp_password,global_init_parms.apn,ipstack);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_QCPDPP:
			{
				char cmd_buffer[64];
				if(A_CMP_B_ACCORDING_B(global_init_parms.ppp_username,"0") || 
					A_CMP_B_ACCORDING_B(global_init_parms.ppp_auth_type,"0") ||
					A_CMP_B_ACCORDING_B(global_init_parms.ppp_password,"0"))
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT\r");
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT$QCPDPP=1,%s,\"%s\",\"%s\"\r",
						global_init_parms.ppp_auth_type,global_init_parms.ppp_password,global_init_parms.ppp_username);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BM3GPP2AUTHINFO:
			{
				char cmd_buffer[256];
				if(A_CMP_B_ACCORDING_B(global_init_parms.ppp_username,"0") || 
					A_CMP_B_ACCORDING_B(global_init_parms.ppp_password,"0"))
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT\r");
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BM3GPP2AUTHINFO=%s,%s\r",
						global_init_parms.ppp_username,global_init_parms.ppp_password);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BMBANDPREF_SUPPORT_BAND_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMBANDPREF=?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMBANDPREF_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMBANDPREF?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_RECOVERY_BAND:
			{
				char cmd_buffer[64];
				char* tmplte = global_dial_vars.lte_all_band;
				tmplte = tmplte + 2;
				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BMBANDPREF=,%s,\r",tmplte);
				printf("cmd_buffer = %s\n",cmd_buffer);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
			}
			break;
		case Dial_State_BMBANDPREF:
			{
				char cmd_buffer[256] = "";
				char lte_str[64] ="";
				char gw_str[64] ="";
				char tds_str[64] ="";
				check_band_bit(63, global_init_parms.lte_band_choose, lte_str);
				check_band_bit(63, global_init_parms.gw_band_choose, gw_str);
				check_band_bit(63, global_init_parms.tds_band_choose, tds_str);
				log_info("lte_lock_band:%s\n",lte_str);
				log_info("gw_lock_band:%s\n",gw_str);
				log_info("tds_lock_band:%s\n",tds_str);
				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BMBANDPREF=%s,%s,%s\r",gw_str,lte_str,tds_str);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BMRAT:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMRAT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_COPS_SET:
			{
				char cmd_buffer[64];
				if(global_init_parms.plmn_mode == PLMN_SELECTION_MODE_MANUAL && strlen(global_init_parms.plmn) != 0)
				{
					if(global_init_parms.plmn_act != -1 )
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+COPS=1,2,\"%s\",%d\r",global_init_parms.plmn,global_init_parms.plmn_act);
					else
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+COPS=1,2,\"%s\"\r",global_init_parms.plmn);
				}
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+COPS=0\r");
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}

		case Dial_State_QCRMCALL_DISCONNECT_V6:
			{
				util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL=0,1,2\r",&global_dialtool.pthread_moniter_flag);
				break;
			}

		case Dial_State_QCRMCALL_V6:
			{
				prepare_v6_environment();
				
				char cmd_buffer[64] = "";
				
				if(global_dial_vars.evdo_cdma_flag!=0)
				{
					snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,2,1\r");
				}
				else
				{
					snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,2,1\r");
				}
				
				util_send_cmd(global_dialtool.dev_handle, cmd_buffer, &global_dialtool.pthread_moniter_flag);
				break;
			}

		case Dial_State_QCRMCALL_QUERY_V6:
			{
				log_info("query_ipv6\n");
				util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL?\r",&global_dialtool.pthread_moniter_flag);
				break;
			}

		case Dial_State_QCRMCALL_DISCONNECT_V4:
			{
				util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL=0,1,1\r",&global_dialtool.pthread_moniter_flag);
				break;
			}

		case Dial_State_QCRMCALL_V4:
			{
				down_udhcpc(global_system_info.module_info.network_card_name);
				up_udhcpc(global_system_info.module_info.network_card_name);
				
				char cmd_buffer[64] = "";
				
				if(global_dial_vars.evdo_cdma_flag!=0)
				{
					snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,1,1\r");
				}
				else
				{
					snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,1,2,1\r");
				}
				
				util_send_cmd(global_dialtool.dev_handle, cmd_buffer, &global_dialtool.pthread_moniter_flag);
				break;
			}

		case Dial_State_QCRMCALL_QUERY_V4:
			{
				log_info("query_ipv4\n");
				util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL?\r",&global_dialtool.pthread_moniter_flag);
				break;
			}

		case Dial_State_QCRMCALL_V4V6:
			{
				log_info("query_ipv4v6\n");
				util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL?\r",&global_dialtool.pthread_moniter_flag);
				break;
			}

		
		case Dial_State_QCRMCALL:
			{
				down_udhcpc(global_system_info.module_info.network_card_name);
				up_udhcpc(global_system_info.module_info.network_card_name);
				char cmd_buffer[64];
				timeLog("start to qcrmcall!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				if(global_dial_vars.evdo_cdma_flag!=0)
				{
					if(!strncmp("IP",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,1,1,1,0\r");
					else if(!strncmp("IPV6",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,1,1,0\r");
					else
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,3,1,1,0\r");
				}
				else
				{
					if(!strncmp("IP",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,1,2,1\r");
					else if(!strncmp("IPV6",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,2,1\r");
					else
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,2,1\r"); //occur a mistake
				}
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_QCRMCALL_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL?\r",&global_dialtool.pthread_moniter_flag);
			break;	
		case Dial_State_CPMS_SM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGL_SM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPMS_ME:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGL_ME:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGD:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGL:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_SYSINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT^SYSINFO\r",&global_dialtool.pthread_moniter_flag);		
			break;
		case Dial_State_BMBAND:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMBAND\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_COPS_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+COPS?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_COPS_OPER_NUM_SET:
			util_send_cmd(global_dialtool.dev_handle,"AT+COPS=3,2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCS:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCS_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CNMI:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGSMS:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCA_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCA_SET:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGF:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGF_DM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;

		case Dial_State_CMGS:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGS_DM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGEQREQ:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGACT_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGCONTRDP:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGACT_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_SIGNALIND:
			util_send_cmd(global_dialtool.dev_handle,"AT+SIGNALIND=0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_QCRMCALL_DISCONNECT:
			util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL=0,1,3\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_HDRCSQ:
			util_send_cmd(global_dialtool.dev_handle,"AT^HDRCSQ\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMDATASTATUS:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMDATASTATUS\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMCPNCNT:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMCPNCNT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_GPSSTART:
			util_send_cmd(global_dialtool.dev_handle,"AT+GPSSTART\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_GPSINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT+GPSINFO\r",&global_dialtool.pthread_moniter_flag);
			break;
		default:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
	}	
}

void bm916_init(int*  Dial_proc_state)
{
	log_info("%s %d\n",__FUNCTION__,__LINE__);
	static int hasSetPin = 0;
	static int isChangeConfig = 0;
	switch(*Dial_proc_state)
	{
		case Dial_State_initialized:
			memcpy(global_dial_vars.moduleType,"bm916",5);
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_BMBANDPREF_SUPPORT_BAND_QUERY;
			break;
		case Dial_State_BMBANDPREF_SUPPORT_BAND_QUERY:
			{
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF: support band:");
					if(NULL !=ptr_tmp)
					{
						char* ptr_gw,	*ptr_lte, *ptr_tds;
						ptr_gw = strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF: support band:")+strlen("+BMBANDPREF: support band:");
						ptr_lte = strstr(ptr_gw,",");
						if(NULL!= ptr_lte)
							*ptr_lte = 0, ptr_lte=ptr_lte+1;
						ptr_tds = strstr(ptr_lte,",");
						if(NULL!= ptr_tds)
							*ptr_tds = 0, ptr_tds=ptr_tds+1;
						ptr_tmp = strstr(ptr_tds,"OK");
						if(NULL!= ptr_tmp)
							*ptr_tmp = 0;
						ptr_gw=strip_head_tail_space(ptr_gw);
						ptr_lte=strip_head_tail_space(ptr_lte);
						ptr_tds=strip_head_tail_space(ptr_tds);
						if( (strlen(ptr_gw)<64) && (strlen(ptr_lte)<64) && (strlen(ptr_tds)<64) )
						{
							strncpy(global_dial_vars.gw_all_band,ptr_gw,strlen(ptr_gw));
							strncpy(global_dial_vars.lte_all_band,ptr_lte,strlen(ptr_lte));
							strncpy(global_dial_vars.tds_all_band,ptr_tds,strlen(ptr_tds));
						}
						
						*Dial_proc_state=Dial_State_BMBANDPREF_QUERY;
					}
				}
			}
			break;
		case Dial_State_BMBANDPREF_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:");
					if(NULL !=ptr_tmp)
					{
						char* ptr_gw,	*ptr_lte, *ptr_tds;
						ptr_gw = strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:")+strlen("+BMBANDPREF:");
						ptr_lte = strstr(ptr_gw,",");
						if(NULL!= ptr_lte)
							*ptr_lte = 0, ptr_lte=ptr_lte+1;
						ptr_tds = strstr(ptr_lte,",");
						if(NULL!= ptr_tds)
							*ptr_tds = 0, ptr_tds=ptr_tds+1;
						ptr_tmp = strstr(ptr_tds,"OK");
						if(NULL!= ptr_tmp)
							*ptr_tmp = 0;
						ptr_gw=strip_head_tail_space(ptr_gw);
						ptr_lte=strip_head_tail_space(ptr_lte);
						ptr_tds=strip_head_tail_space(ptr_tds);
						if( (strlen(ptr_gw)<64) && (strlen(ptr_lte)<64) && (strlen(ptr_tds)<64) )
						{
							strncpy(global_dial_vars.gw_lock_band,ptr_gw,strlen(ptr_gw));
							strncpy(global_dial_vars.lte_lock_band,ptr_lte,strlen(ptr_lte));
							strncpy(global_dial_vars.tds_lock_band,ptr_tds,strlen(ptr_tds));
						}
						*Dial_proc_state=Dial_State_BMBANDPREF;

						if(global_init_parms.lte_band_choose[0]=='\0')
						{
							if( 0 == strcmp(global_dial_vars.lte_all_band, global_dial_vars.lte_lock_band))
							{
								*Dial_proc_state=Dial_State_PCI_LOCK_QUERY;
							}
							else
								*Dial_proc_state=Dial_State_RECOVERY_BAND;
						}
						else if(strcmp(global_dial_vars.gw_lock_band, global_init_parms.gw_band_choose) == 0 &&
								strcmp(global_dial_vars.lte_lock_band, global_init_parms.lte_band_choose) == 0 &&
								strcmp(global_dial_vars.tds_lock_band, global_init_parms.tds_band_choose) == 0)
						{
								*Dial_proc_state=Dial_State_PCI_LOCK_QUERY;
						}
							
					}
				}
				break;	
		case Dial_State_BMBANDPREF:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{	
					isChangeConfig = 1;
					*Dial_proc_state=Dial_State_PCI_LOCK_QUERY;	
				}
				break;
		case Dial_State_RECOVERY_BAND:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					isChangeConfig = 1;
					*Dial_proc_state=Dial_State_PCI_LOCK_QUERY;
				}
					
				break;
		case Dial_State_PCI_LOCK_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMPCILOCK:");
					if(NULL !=ptr_tmp)
					{
						//isON: 0-off  1-on   2-query
						//type: 1-NB cell  2-EMTC cell
						
						char *earfcn,*pci;
						earfcn = strstr(global_dialtool.buffer_at_sponse,"+BMPCILOCK:")+strlen("+BMPCILOCK:");
						pci = strstr(earfcn,",");
						if(NULL!= earfcn)
							*earfcn = 0, earfcn=earfcn+1;
						pci = strstr(earfcn,",");
						if(NULL!= pci )
							*pci = 0, pci =pci +1;
						ptr_tmp = strstr(pci,"OK");
						if(NULL!= ptr_tmp)
							*ptr_tmp = 0;
						if (NULL != earfcn)
							earfcn=strip_head_tail_space(earfcn);
						if (NULL != pci)
							pci=strip_head_tail_space(pci);

						log_info("lock_pci_query:earfcn = %s, pci = %s\n", earfcn, pci);
						log_info("lock_pci_query:init_parms.earfcn = %s, init_parms.pci = %s\n", global_init_parms.lte_earfcn_lock,global_init_parms.lte_pci_lock);
						*Dial_proc_state=Dial_State_PCI_LOCK;

						if('\0' == global_init_parms.lte_pci_lock[0] ||
							'\0' == global_init_parms.lte_earfcn_lock[0] )
						{
							if ('0' == earfcn[0] && '0' == pci[0])
								*Dial_proc_state=Dial_State_PLMN_LOCK_QUERY;
						}
						else if(NULL != earfcn &&strcmp(earfcn, global_init_parms.lte_earfcn_lock) == 0&&
								NULL != pci &&strcmp(pci, global_init_parms.lte_pci_lock) == 0)
								*Dial_proc_state=Dial_State_PLMN_LOCK_QUERY;
						
						


					}
					
				}
				break;
		case Dial_State_PCI_LOCK:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					isChangeConfig = 1;
					*Dial_proc_state=Dial_State_PLMN_LOCK_QUERY;
				}
				break;
		case Dial_State_PLMN_LOCK_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+NWLCFG:");
					if(NULL !=ptr_tmp)
					{
						char* isOn, *plmn;
						isOn = strstr(global_dialtool.buffer_at_sponse,"+NWLCFG:")+strlen("+NWLCFG:");
						plmn = strstr(isOn,",");
						if(NULL!= plmn)
						{
							*plmn = 0, plmn=plmn+1;
							ptr_tmp = strstr(plmn,"OK");
						}
						else
						{
							ptr_tmp = strstr(isOn,"OK");
						}
							
						
						if(NULL!= ptr_tmp)
							*ptr_tmp = 0;
						isOn=strip_head_tail_space(isOn);
						if(NULL != plmn)
							plmn=strip_head_tail_space(plmn);

						log_info("lock_plmn_query:isON = %s, plmn = %s\n",isOn, plmn);
						*Dial_proc_state=Dial_State_PLMN_LOCK;
							
						if('0' == global_init_parms.plmn_lock[0] ||
							'\0' == global_init_parms.plmn_lock[0] )
						{
							if ('0' == isOn[0])
							{
								if(1 == isChangeConfig )
									*Dial_proc_state=Dial_State_CFUN_DISABLE;
								else
									*Dial_proc_state=Dial_State_BMDATASTATUSEN;
							}
						}
						else if(NULL != plmn && strcmp(plmn, global_init_parms.plmn_lock) == 0)
						{
							if(1 == isChangeConfig )
								*Dial_proc_state=Dial_State_CFUN_DISABLE;
							else
								*Dial_proc_state=Dial_State_BMDATASTATUSEN;
						
						}


					}
					
				}
				break;

		case Dial_State_PLMN_LOCK:
				if (NULL == strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					log_error("plmn lock error");
				}
				else
					isChangeConfig = 1;
				
				if(1 == isChangeConfig )
					*Dial_proc_state=Dial_State_CFUN_DISABLE;
				else
					*Dial_proc_state=Dial_State_BMDATASTATUSEN;
				break;
		case Dial_State_BMDATASTATUSEN:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
					*Dial_proc_state=Dial_State_CPIN_QUERY;
				break;	
		case Dial_State_CPIN_QUERY:
				log_info("in Dial_State_CPIN_QUERY\n");
				global_dial_vars.pin_qpin_flag=0;		//set flag ,so we can choose which cmd to input pin 
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_READY))
				{
					log_info("in cpin ready\n");
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					check_cpin_num = 0;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PIN))
				{
					global_dial_vars.sim_real_lock_pin = 1;
					log_info("in cpin need pin\n");
					check_cpin_num = 0;
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=1;
					global_dial_vars.device_lock_puk=0;
					if(global_init_parms.enable_pin[0] == '1' )
						log_error(">>>>>>>>>>had enable auto verification pin,begin to auto unlock pin\n");
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PUK))
				{
					log_info("in cpin puk\n");
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=1;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					check_cpin_num = 0;
					break;
				}
				else
				{
					log_info("in else\n");
					check_cpin_num++;
					if(check_cpin_num > 4)
					{
						check_cpin_num = 0;
						*Dial_proc_state=Dial_State_CFUN_DISABLE;
						break;
					}	
					global_dial_vars.is_sim_exist=0;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_QCPIN_QUERY;
					break;
				}
		case Dial_State_QCPIN_QUERY:
				global_dial_vars.pin_qpin_flag=1;
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_READY))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					hasSetPin = 0;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_QCPIN_NEED_PIN))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=1;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_QCPIN_NEED_PUK))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=1;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else
				{
					global_dial_vars.is_sim_exist=0;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_CPIN_QUERY;
					break;
				}
		case Dial_State_BMCPNCNT:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					log_error(">>>>>>>>>>>>>>>>>>int case Dial_State_BMCPNCNT\n");
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMCPNCNT:");
					
					if(NULL != ptr_tmp )
					{
						//remember pin times and puk times left
						ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"PIN1=");
						if(NULL != ptr_tmp)
							global_dial_vars.pin_left_times=atoi(ptr_tmp+strlen("PIN1="));
						ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"PUK1=");
						if(NULL != ptr_tmp)
							global_dial_vars.puk_left_times=atoi(ptr_tmp+strlen("PUK1="));
						//we only try first time cpin,so if cpin times not equal to 3,we wait web change pin code,pin default value is 3
						if( global_dial_vars.device_lock_pin ==1 || global_dial_vars.device_lock_puk == 1)
						{
							if(global_init_parms.enable_pin[0] == '1' && strlen(global_init_parms.pin) && !hasSetPin)
							{
								if(global_dial_vars.pin_qpin_flag == 1 )
									*Dial_proc_state=Dial_State_QCPIN_SET;
								else
								{
									*Dial_proc_state=Dial_State_CPIN_SET;
									hasSetPin = 1;
								}
								break;
							}
							else 				//we already input pin error one time,or need input puk,so we just query and wait
							{
								if(global_dial_vars.pin_qpin_flag == 1)
								{
									*Dial_proc_state=Dial_State_QCPIN_QUERY;
								}
								else
								{
									log_error(">>>>>>>>>>>>>>>>manager should input pin on WEB!!!\n");
									*Dial_proc_state=Dial_State_CPIN_QUERY;
								}
								break;
							}
						}
						else
						{
							*Dial_proc_state=Dial_State_ICCID;
						}
					}
				}
				break;
		case Dial_State_CPIN_SET:
				*Dial_proc_state=Dial_State_CPIN_QUERY;
				sleep(5);
				break;
		case Dial_State_QCPIN_SET:
				*Dial_proc_state=Dial_State_QCPIN_QUERY;	
				sleep(5);
				break;
		case Dial_State_ICCID:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"ICCID:");
					if(NULL!=ptr_tmp)
					{
						char* ptr_tmp1=NULL;
						ptr_tmp1=ptr_tmp+strlen("ICCID:");
						char* ptr_tmp2=strstr(ptr_tmp1,"OK");
						if(NULL!= ptr_tmp2)
							ptr_tmp2[0]='\0';
						ptr_tmp=strip_head_tail_space(ptr_tmp1);
						if(strlen(ptr_tmp)<64)
							strncpy(global_dial_vars.iccid,ptr_tmp,strlen(ptr_tmp));
						if(global_dial_vars.pin_qpin_flag==1)
							*Dial_proc_state=Dial_State_QCIMI;
						else
							*Dial_proc_state=Dial_State_CIMI;
					}
				}		
				break;
		case Dial_State_CIMI:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"\n")+strlen("\n");
				char* ptr_tmp2=strstr(ptr_tmp,"OK");
				if(NULL!= ptr_tmp2)
					ptr_tmp2[0]='\0';
		
				ptr_tmp=strip_head_tail_space(ptr_tmp);
				if(strlen(ptr_tmp)<64)
				{
					strncpy(global_dial_vars.imsi,ptr_tmp,strlen(ptr_tmp));
					log_info("imsi:%s,%d\n",ptr_tmp,__LINE__);
					*Dial_proc_state=Dial_State_BMMODODR;
				}
			}				
			break;
		case Dial_State_QCIMI:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"\n")+strlen("\n");
				char* ptr_tmp2=strstr(ptr_tmp,"OK");
				if(NULL!= ptr_tmp2)
					ptr_tmp2[0]='\0';
		
				ptr_tmp=strip_head_tail_space(ptr_tmp);
				if(strlen(ptr_tmp)<64)
				{
					strncpy(global_dial_vars.imsi,ptr_tmp,strlen(ptr_tmp));
					log_info("imsi:%s,%d\n",ptr_tmp,__LINE__);
					*Dial_proc_state=Dial_State_BMMODODR;
				}
				
			}
			else
					*Dial_proc_state=Dial_State_CPIN_QUERY;
			break;

		case Dial_State_BMMODODR:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				if( !check_file_exist(GPS_IS_ENABLE) )
				{
					*Dial_proc_state=Dial_State_CEREG;
				}
				else
				{
					*Dial_proc_state=Dial_State_GPSSTART;
				}
			}
			break;

		case Dial_State_CEREG:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_COPS_OPER_NUM_SET;
			break;

		case Dial_State_COPS_OPER_NUM_SET:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_CGDCONT;
			break;

		case Dial_State_GPSSTART:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_CEREG;
			break;

		case Dial_State_CGDCONT:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_QCPDPP;
			break;
		case Dial_State_QCPDPP:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_BM3GPP2AUTHINFO;
			break;
		case Dial_State_BM3GPP2AUTHINFO:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_CGDCONT2;
			break;		
		case Dial_State_CGDCONT2:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				global_dialtool.Dial_Lvl_1=DIAL_DIAL;
				*Dial_proc_state=Dial_State_SYSINFO;
				timeLog("will change to DIAL_DIAL");
			}
			break;
		case Dial_State_CFUN_DISABLE:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				*Dial_proc_state=Dial_State_CFUN_ENABLE;
				break;
			}
		case Dial_State_CFUN_ENABLE:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				sleep(5);
				*Dial_proc_state=Dial_State_initialized;

				if(1 == isChangeConfig )
				{
					isChangeConfig	= 0;
					*Dial_proc_state=Dial_State_BMDATASTATUSEN;
				}
				
				break;
			}	

				
	#if 0
		case Dial_State_CLCK_QUERY:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) )
				{
					char* ptr_tmp= strstr(global_dialtool.buffer_at_sponse,"+CLCK:");
					if(NULL!= ptr_tmp)
					{
						global_dial_vars.pinlock_enable_flag=atoi(ptr_tmp+strlen("+CLCK:"));
						global_dialtool.Dial_Lvl_1=DIAL_DIAL;
						*Dial_proc_state=Dial_State_SYSINFO;
					}					
				}
				break;	
		case Dial_State_QCLCK_QUERY:	
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) )
				{
					char* ptr_tmp= strstr(global_dialtool.buffer_at_sponse,"+QCLCK:");
					if(NULL!= ptr_tmp)
					{
						global_dial_vars.pinlock_enable_flag=atoi(ptr_tmp+strlen("+QCLCK:"));
						global_dialtool.Dial_Lvl_1=DIAL_DIAL;
						*Dial_proc_state=Dial_State_SYSINFO;
					}					
				}
				break;
	#endif
		default:
			break;
	}
	log_info("%s_%d\n",__FUNCTION__,__LINE__);
}

void bm916_dial(int* Dial_proc_state )
{
	static int exception_count=0;      //can not search  valid  network count,if more than 20 times,we go into exception mode:reboot module
	static int data_link_count=0;
	log_info("%s_%d exception_count:%d\n",__FUNCTION__,__LINE__,exception_count);
	switch(*Dial_proc_state)
	{
	
		case Dial_State_QCRMCALL_DISCONNECT:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)||NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR) )
			{
				if(check_file_exist(MANUAL_DISCONNECT))
				{
					sleep(1);
					break;
				}
				else
				{
					*Dial_proc_state=Dial_State_SYSINFO;
				}
				break;
			}
		case Dial_State_SYSINFO: 
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:"))
			{
				timeLog(global_dialtool.buffer_at_sponse);
				int srv_status,srv_domain;
				int roam_status,sys_mode,sim_state;
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:");
				if(NULL != ptr_tmp)
					ptr_tmp=ptr_tmp+strlen("^SYSINFO:");
				char* ptr_tmp_1=strstr(ptr_tmp,CMD_EXE_OK);
				if(NULL!=ptr_tmp_1)
					ptr_tmp_1[0]='\0';
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(ptr_tmp,",",result_useful);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				srv_status=atoi(result_useful[0]);
				srv_domain=atoi(result_useful[1]);
				roam_status=atoi(result_useful[2]);
				sys_mode=atoi(result_useful[3]);
				sim_state=atoi(result_useful[4]);
				global_dial_vars.service_status=srv_status;
				global_dial_vars.ps_cs_region=srv_status;
				if(sim_state == 255 )
				{
					global_dial_vars.is_sim_exist=0;
				}
				else
				{
					global_dial_vars.is_sim_exist=1;
					if(roam_status == 1)
						global_dial_vars.roam_status=1;
					else
						global_dial_vars.roam_status=0;
					//srv_domain:4 CS/PS not registered,but still in searching state
					if(sys_mode != 0 && srv_status != 0 && srv_status != 4 && srv_domain != 0 && srv_domain != 4)
					{
						*Dial_proc_state=Dial_State_COPS_QUERY;
						break;
					}
				}
				exception_count++;
				free_two(result_useful,5,8);
				if((exception_count>300 && srv_domain != 4 && srv_status != 0)
					|| (exception_count>600))
				{
					global_dialtool.Dial_Lvl_1=DIAL_INIT;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=Dial_State_CFUN_DISABLE;
				}
			}
			break;

		//+COPS: 0,2,"46000",7 OK
		case Dial_State_COPS_QUERY:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"+COPS:"))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+COPS: 0,2,\"");
				if(NULL!=ptr_tmp)
				{
					char* ptr_tmp1=NULL;
					ptr_tmp1=ptr_tmp+strlen("+COPS: 0,2,\"");
					if(NULL!= ptr_tmp1)
					{
						memset(global_dial_vars.operator_plmn, 0,sizeof(global_dial_vars.operator_plmn));
						strncpy(global_dial_vars.operator_plmn,ptr_tmp1,5);
					}
				}
			}
			*Dial_proc_state=Dial_State_BMRAT;
			break;
		case Dial_State_BMRAT:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				timeLog(global_dialtool.buffer_at_sponse);
				char *network_type_string=strstr(global_dialtool.buffer_at_sponse,"+BMRAT:");
				if(NULL!= network_type_string)
					network_type_string=strip_head_tail_space(network_type_string+strlen("+BMRAT:"));
				else
				{
					break;
				}
				/*
				LTE
				TDS
				HSPA+
				HSUPA
				HSDPA
				HSPA
				HSDPA+
				DC HSDPA+
				UMTS
				HDR RevA
				HDR RevB
				HDR Rev0
				GPRS
				EDGE
				GSM
				1x
				NONE
				
				+PSRAT: HDR - EMPA EHRPD
				
				+NWTYPEIND:31 //"no service",
				+NWTYPEIND:32 //"network:gsm",
				+NWTYPEIND:33 //"network:gprs",
				+NWTYPEIND:34 //"network:edge",
				+NWTYPEIND:35 //"network:wcdma",
				+NWTYPEIND:36 //"network:hsdpa",
				+NWTYPEIND:37 //"network:hsupa"
				+NWTYPEIND:38 //"network (hsdpa_plus)"
				+NWTYPEIND:39 //"network (td_scdma)"
				+NWTYPEIND:40 //"network (lte_fdd)"
				+NWTYPEIND:41 //"network (lte_tdd)"
				+NWTYPEIND:42 //"network (EVDO)"
				+NWTYPEIND:43 //CDMA
				+NWTYPEIND:44 //CDMA&EVDO
				+NWTYPEIND:45 //EVDO
				*/
				if( A_CMP_B_ACCORDING_B( network_type_string,"TD-LTE" ) )
				{
					global_dial_vars.network_mode_bmrat=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDD LTE
				else if(A_CMP_B_ACCORDING_B( network_type_string,"TDD" ) )
				{
					global_dial_vars.network_mode_bmrat=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//FDD LTE
				//FDD-LTE
				else if( A_CMP_B_ACCORDING_B( network_type_string,"FDD" ) )
				{
					global_dial_vars.network_mode_bmrat=40;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//LTE
				//LTE TDD
				//LTE FDD
				else if( A_CMP_B_ACCORDING_B( network_type_string,"LTE" ) )
				{
					global_dial_vars.network_mode_bmrat=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status == 1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDSCDMA
				else if( A_CMP_B_ACCORDING_B( network_type_string,"TDS" ) )
				{
					global_dial_vars.network_mode_bmrat=39;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				//HSPA+
				//HSUPA
				//HSDPA
				//HSPA
				//HSDPA+
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HS" ) )
				{
					global_dial_vars.network_mode_bmrat=38;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"DC HSDPA+" ) )
				{
					global_dial_vars.network_mode_bmrat=36;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"WCDMA" ) )
				{
					global_dial_vars.network_mode_bmrat=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"UMTS" ) )
				{
					global_dial_vars.network_mode_bmrat=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EDGE" ) )
				{
					global_dial_vars.network_mode_bmrat=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GPRS" ) )
				{
					global_dial_vars.network_mode_bmrat=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GSM" ) )
				{
					global_dial_vars.network_mode_bmrat=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( strstr( network_type_string,"EHRPD" ) )
				{
					global_dial_vars.network_mode_bmrat=44;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EVDO" ) )
				{
					global_dial_vars.network_mode_bmrat=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//HDR RevA
				//HDR RevB
				//HDR Rev0
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HDR" ) )
				{
					global_dial_vars.network_mode_bmrat=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//CDMA
				//CDMA&EVDO
				else if( A_CMP_B_ACCORDING_B( network_type_string,"CDMA" ) )
				{
					global_dial_vars.network_mode_bmrat=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"1x" ) )
				{
					global_dial_vars.network_mode_bmrat=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"NONE" ) )
				{
					global_dial_vars.network_mode_bmrat=31;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=-1;
				}
				else
				{
					global_dial_vars.network_mode_bmrat=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}

				char * ptr_tmp=strstr(network_type_string,"OK");
				if(NULL != ptr_tmp)
				{
					ptr_tmp[0]='\0';
					char * ptr_tmp1=strip_head_tail_space(network_type_string);
//					printf("network:%s %s\n",ptr_tmp1,network_type_string);
					strlcpy(global_dial_vars.network_mode_detail,ptr_tmp1,strlen(ptr_tmp1)+1);
				}
				//sometimes have no signal,and we search nothing,but we must be sure wihich network we are on 
				if(global_dial_vars.network_mode_bmrat!=31)
				{
					if (strcmp(global_init_parms.ipstack,"IPV4V6") == 0)
					{
						*Dial_proc_state=Dial_State_QCRMCALL_V6;
					}
					else
					{
						*Dial_proc_state=Dial_State_QCRMCALL_V4;
					}
					//*Dial_proc_state=Dial_State_QCRMCALL;
					exception_count=0;
				}
				else
					exception_count++;;
				log_info("networktype:%s %d\n",network_type_string,global_dial_vars.evdo_cdma_flag);
				if(exception_count>60)
				{
					global_dialtool.Dial_Lvl_1=DIAL_INIT;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=Dial_State_CFUN_DISABLE;
				}

				if('0' == global_init_parms.enable_auto_dial[0])
					*Dial_proc_state=Dial_State_SYSINFO;
				
				break;
			}
			break;

		case Dial_State_QCRMCALL_DISCONNECT_V6:
			{
				*Dial_proc_state=Dial_State_QCRMCALL_V6;
			}
			break;

		case Dial_State_QCRMCALL_V6:
			{
				sleep(5);
				*Dial_proc_state=Dial_State_QCRMCALL_QUERY_V6;
			}
			break;
			
		case Dial_State_QCRMCALL_QUERY_V6:
			{
				if(NULL!= strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					if( NULL!= strstr(global_dialtool.buffer_at_sponse,"1, V6") ) 
						global_dial_vars.ipv6_register_status = 1;
					else
						global_dial_vars.ipv6_register_status = 0;
				}
				else
					global_dial_vars.ipv6_register_status = 0;

				if( NULL!= strstr(global_dialtool.buffer_at_sponse,"1, V4") ) 
					global_dial_vars.ipv4_register_status = 1;
				else
					global_dial_vars.ipv4_register_status = 0;

				if (strcmp(global_init_parms.ipstack,"IPV4V6") == 0)
				{
					*Dial_proc_state=Dial_State_QCRMCALL_V4;
				}
				else
				{
					*Dial_proc_state=Dial_State_BMDATASTATUS;
				}
			}
			break;

		case Dial_State_QCRMCALL_DISCONNECT_V4:
			{
				
				*Dial_proc_state=Dial_State_QCRMCALL_V4;
			}
			break;

		case Dial_State_QCRMCALL_V4:
			{
				
				*Dial_proc_state=Dial_State_QCRMCALL_QUERY_V4;
			}
			break;
			
		case Dial_State_QCRMCALL_QUERY_V4:
			{
				if(NULL!= strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					if( NULL!= strstr(global_dialtool.buffer_at_sponse,"1, V4") ) 
					{
						global_dial_vars.ipv4_register_status = 1;
						exception_count=0;
					}
					else
					{
						global_dial_vars.ipv4_register_status = 0;
						exception_count++;
					}
				}
				else
				{
					global_dial_vars.ipv4_register_status = 0;
					exception_count++;
				}
				
				printf("exception_count = %d\n", exception_count);
				if( global_dial_vars.ipv4_register_status == 1 || exception_count > 20)
				{
					printf("go out 60\n");
					system("ifconfig eth1 up");
					exception_count=0;
					*Dial_proc_state=Dial_State_BMDATASTATUS;
				}
				
			}
			break;
			
		case Dial_State_QCRMCALL:
				*Dial_proc_state=Dial_State_QCRMCALL_QUERY;
				//system("cat /proc/uptime | awk '{print $1}' >> /root/begin_dial_time");
				break;
		case Dial_State_QCRMCALL_QUERY:		
			if(NULL!= strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				int ip_type=3;
				//int udhcpc_pid;
				//char buffer_cmd[1024];
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"$QCRMCALL:");
				if(NULL!= ptr_tmp)
				{
					//static int udhcpc_sig_count=0;
					ptr_tmp=strip_head_tail_space(ptr_tmp+strlen("$QCRMCALL:"));
					ip_type=atoi(ptr_tmp);
					global_dial_vars.ipstack_registered=ip_type;
					//here we dial successful,so we should told udhcpc to get ip and dns
					/*snprintf(buffer_cmd,sizeof(buffer_cmd),"udhcpc -b -i %s -p %s -s /etc/udhcpc.script",
					global_system_info.module_info.network_card_name,UDHCPC_PIDFILE_PATH);
			REGET_IP_BY_UDHCPC:
					udhcpc_pid=getpid_by_pidfile(UDHCPC_PIDFILE_PATH);
					if(FALSE!=udhcpc_pid)
					{
						if(0!=kill(udhcpc_pid,SIGUSR1))
						{
							sleep(1);
							udhcpc_sig_count++;
							if(udhcpc_sig_count>3)
							{
								system("killall udhcpc");
							}
							log_info("udhcpc send USR1 ERROR:%s\n",strerror(errno));
							goto REGET_IP_BY_UDHCPC;
						}
						udhcpc_sig_count=0;
					}
					else
						system(buffer_cmd);
					*/
					exception_count=0;
					global_sleep_interval_long = 1;
					//sleep(5);
					//system("cat /proc/uptime | awk '{print $1}' >> /root/dial_success_time");
					*Dial_proc_state=Dial_State_BMDATASTATUS;
				}
				else
				{
					exception_count++;
					global_dial_vars.ipstack_registered=0;
					*Dial_proc_state=Dial_State_QCRMCALL;
				}			
			}
			else
			{
				exception_count++;
			}
			if(exception_count>60)
			{
				global_dialtool.Dial_Lvl_1=DIAL_INIT;
				global_dial_vars.network_link=0;
				exception_count=0;
				*Dial_proc_state=Dial_State_CFUN_DISABLE;
			}
			break;
		case Dial_State_BMDATASTATUS:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:"))
			{
				timeLog(global_dialtool.buffer_at_sponse);
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:")+strlen("+BMDATASTATUS:");
				global_dial_vars.network_link=atoi(tmp_ptr);
				if(global_dial_vars.network_link!=0)
				{
					data_link_count=0;
					if(global_dial_vars.evdo_cdma_flag == 1)
						*Dial_proc_state=Dial_State_HDRCSQ;
					else
						*Dial_proc_state=Dial_State_CMEE;
				}
				else
				{
					data_link_count++;
					if(data_link_count >10)
						*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
					if(data_link_count >30)
					{
						global_dialtool.Dial_Lvl_1=DIAL_INIT;
						global_dial_vars.network_link=0;
						exception_count=0;
						*Dial_proc_state=Dial_State_CFUN_DISABLE;
					}
				}
			}
			break;

		case Dial_State_CMEE:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				*Dial_proc_state=Dial_State_PSDIALIND;
			}
			break;	
		case Dial_State_PSDIALIND:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_SIGNALIND;
			break;
		case Dial_State_SIGNALIND:
			if(NULL!= strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				global_dialtool.Dial_Lvl_1=DIAL_DEAMON;
				*Dial_proc_state=Dial_State_BMTCELLINFO;
			}
			break;	
		case Dial_State_HDRCSQ:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL != strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:"))
			{
				
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:")+strlen("^HDRCSQ:");
				int rssi_hdr=atoi(tmp_ptr);
				if(rssi_hdr==99)
				{
					global_dial_vars.signal_rssi_level=5;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"60");
				}
				else if(rssi_hdr ==80)
				{
					global_dial_vars.signal_rssi_level=4;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"65");
				}
				else if(rssi_hdr ==60)
				{
					global_dial_vars.signal_rssi_level=3;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"80");
				}
				else if(rssi_hdr ==40)
				{
					global_dial_vars.signal_rssi_level=2;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"95");
				}
				else if(rssi_hdr ==20)
				{
					global_dial_vars.signal_rssi_level=1;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"105");
				}
				else
				{
					global_dial_vars.signal_rssi_level=0;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"0");
				}
				global_dialtool.Dial_Lvl_1=DIAL_DEAMON;
				*Dial_proc_state=Dial_State_BMTCELLINFO;	
			}
			break;
		default:
			break;			
	}

}

void bm916_deamon(int* Dial_proc_state)
{
	static int data_link_status_count=0;
	static int connect_status_fail_count = 0;
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	
	if(check_file_exist(MANUAL_DISCONNECT))
	{
		global_dialtool.Dial_Lvl_1=DIAL_DIAL;
		data_link_status_count=0;
		global_dial_vars.network_link=0;
		*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
		return;
	}
	int network_type=0;

	switch(*Dial_proc_state)
	{
		case Dial_State_BMTCELLINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:"))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:")+strlen("+BMTCELLINFO:");
				char* ptr_tmp1=strstr(ptr_tmp,CMD_EXE_OK);
				char* ptr_tmp2=NULL;
				char* ptr_tmp3=NULL;

				*Dial_proc_state=Dial_State_SYSINFO;
				if(NULL!=ptr_tmp1)
					ptr_tmp1='\0';
				char buffer_tmp[RECV_BUFF_SIZE];
	#if 0
				int i=0;
				char** str_affer=(char**)m_malloc_two(64,128);
				separate_str(ptr_tmp,"\n",str_affer);

				for(i=0;i<64;i++)
				{
					if(str_affer[i][0] == '\0')
						continue;
	#endif
					if(NULL!= strstr(ptr_tmp,"CELL_ID:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"CELL_ID:")+strlen("CELL_ID:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.cell_id, 0, sizeof(global_dial_vars.cell_id));
						strncpy(global_dial_vars.cell_id,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"PCI:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"PCI:")+strlen("PCI:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.pci, 0, sizeof(global_dial_vars.pci));
						strncpy(global_dial_vars.pci,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"RSSI:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RSSI:")+strlen("RSSI:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.signal_rssi_value, 0, sizeof(global_dial_vars.signal_rssi_value));
						strncpy(global_dial_vars.signal_rssi_value,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"RSRP:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RSRP:")+strlen("RSRP:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);
						memset(global_dial_vars.signal_rsrp, 0, sizeof(global_dial_vars.signal_rsrp));
						strncpy(global_dial_vars.signal_rsrp,ptr_tmp3,strlen(ptr_tmp3));

						int rsrp = atoi(global_dial_vars.signal_rsrp);
						{
							int *p = global_init_parms.signal_rsrp_lvl;
							if (rsrp == 0)
								global_dial_vars.signal_rssi_level=-1;
							else if(rsrp>=p[4])
								global_dial_vars.signal_rssi_level=5;
							else if(rsrp>p[3]&& rsrp<=p[4])
								global_dial_vars.signal_rssi_level=4;
							else if(rsrp>p[2] && rsrp<=p[3])
								global_dial_vars.signal_rssi_level=3;
							else if(rsrp>p[1] && rsrp<=p[2])
								global_dial_vars.signal_rssi_level=2;
							else if(rsrp>p[0] && rsrp<=p[1])
								global_dial_vars.signal_rssi_level=1;
							else
								global_dial_vars.signal_rssi_level=0;
						}

					}
					if(NULL!= strstr(ptr_tmp,"RSRQ:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RSRQ:")+strlen("RSRQ:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);
						memset(global_dial_vars.signal_rsrq, 0, sizeof(global_dial_vars.signal_rsrq));
						strncpy(global_dial_vars.signal_rsrq,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"SINR:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"SINR:")+strlen("SINR:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.signal_sinr, 0, sizeof(global_dial_vars.signal_sinr));
						strncpy(global_dial_vars.signal_sinr,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"ACTIVE BAND:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"ACTIVE BAND:")+strlen("ACTIVE BAND:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.active_band, 0, sizeof(global_dial_vars.active_band));
						strncpy(global_dial_vars.active_band,ptr_tmp3,strlen(ptr_tmp3));
						global_dial_vars.active_band[strlen(global_dial_vars.active_band)] = 0;

					}
					if(NULL!= strstr(ptr_tmp,"ACTIVE CHANNEL:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"ACTIVE CHANNEL:")+strlen("ACTIVE CHANNEL:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);
						
						memset(global_dial_vars.active_channel, 0, sizeof(global_dial_vars.active_channel));
						strncpy(global_dial_vars.active_channel,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"EARFCN:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"EARFCN:")+strlen("EARFCN:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.earfcn, 0, sizeof(global_dial_vars.earfcn));
						strncpy(global_dial_vars.earfcn,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"ENODEBID:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"ENODEBID:")+strlen("ENODEBID:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.enodebid, 0, sizeof(global_dial_vars.enodebid));
						strncpy(global_dial_vars.enodebid,ptr_tmp3,strlen(ptr_tmp3));

					}
					
					if(NULL!= strstr(ptr_tmp,"TAC:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"TAC:")+strlen("TAC:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.tac, 0, sizeof(global_dial_vars.tac));
						strncpy(global_dial_vars.tac,ptr_tmp3,strlen(ptr_tmp3));
						
						//web get tac(4g)/lac(2g,3g) only from lac_id
						memset(global_dial_vars.lac_id, 0, sizeof(global_dial_vars.lac_id));
						strncpy(global_dial_vars.lac_id,ptr_tmp3,strlen(ptr_tmp3));

					}
					else
					{
						if(NULL!= strstr(ptr_tmp,"LAC_ID:"))
						{
							ptr_tmp2=strstr(ptr_tmp,"LAC_ID:")+strlen("LAC_ID:");
							memset(buffer_tmp,0,RECV_BUFF_SIZE);
							strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);

							if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
							{
								ptr_tmp3[0]='\0';
							}
							if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
							{
								ptr_tmp3[0]='\0';
							}
							ptr_tmp3=strip_head_tail_space(buffer_tmp);
							//log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,ptr_tmp3);
							memset(global_dial_vars.lac_id, 0, sizeof(global_dial_vars.lac_id));
							strncpy(global_dial_vars.lac_id,ptr_tmp3,strlen(ptr_tmp3));
						}
						*Dial_proc_state=Dial_State_CEREG_QUERY;
					}
					
					if(NULL!= strstr(ptr_tmp,"RRC_STATUS:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RRC_STATUS:")+strlen("RRC_STATUS:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.rrc_status, 0, sizeof(global_dial_vars.rrc_status));
						strncpy(global_dial_vars.rrc_status,ptr_tmp3,strlen(ptr_tmp3));

					}

					if(NULL!= strstr(ptr_tmp,"CQI:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"CQI:")+strlen("CQI:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.cqi, 0, sizeof(global_dial_vars.cqi));
						strncpy(global_dial_vars.cqi,ptr_tmp3,strlen(ptr_tmp3));

					}

					if(NULL!= strstr(ptr_tmp,"MSC:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"MSC:")+strlen("MSC:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}	
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\r")) )
						{
							ptr_tmp3[0]='\0';
						}
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						memset(global_dial_vars.mcs, 0, sizeof(global_dial_vars.mcs));
						strncpy(global_dial_vars.mcs,ptr_tmp3,strlen(ptr_tmp3));

					}
					
	#if 0
				}
	#endif				
			}
			break;

		case Dial_State_CEREG_QUERY:
			//+CEREG: 2,1,'27BD','D0','33B2483',7 OK
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+CEREG: 2,");
				
				if(NULL!=ptr_tmp)
				{
					unsigned int tac;
					int reg_status;
					char* ptr_tmp1=NULL;

					*Dial_proc_state=Dial_State_SYSINFO;
					ptr_tmp1=ptr_tmp+strlen("+CEREG: 2,");
					reg_status = atoi(ptr_tmp1);
					//registered,1:home network,5:roaming
					if( ( reg_status == 1 ) || ( reg_status == 5 ) )
					{
						//tac
						ptr_tmp = strstr(ptr_tmp1,",\"");
						if(NULL != ptr_tmp)
						{
							ptr_tmp = ptr_tmp+2;

							//27BD","D6","AC17D03",7
							tac = strtoul( ptr_tmp,NULL,16 );
							//log_info("%s_%d:tac:%d\n",__FUNCTION__,__LINE__,tac);
							sprintf(global_dial_vars.lac_id,"%d",tac);
							//web get tac(4g)/lac(2g,3g) only from lac_id
							strcpy(global_dial_vars.tac,global_dial_vars.lac_id);
						}
					}
				}
			}
			break;
		
		case Dial_State_SYSINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:"))
			{
			
				int srv_status,srv_domain;
				int roam_status,sys_mode,sim_state;
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
	
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:");
				if(NULL != ptr_tmp)
					ptr_tmp=ptr_tmp+strlen("^SYSINFO:");
				char* ptr_tmp1=strstr(ptr_tmp,CMD_EXE_OK);
				if(NULL!=ptr_tmp1)
					ptr_tmp1[0]='\0';
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(ptr_tmp,",",result_useful);
				
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				srv_status=atoi(result_useful[0]);
				srv_domain=atoi(result_useful[1]);
				roam_status=atoi(result_useful[2]);
				sys_mode=atoi(result_useful[3]);
				sim_state=atoi(result_useful[4]);

				global_dial_vars.service_status=srv_status;
				global_dial_vars.ps_cs_region=srv_domain;

				if(sim_state == 255 )
				{
					global_dial_vars.is_sim_exist=0;
				}
				else
				{
					global_dial_vars.is_sim_exist=1;
					if(roam_status == 1)
						global_dial_vars.roam_status=1;
					else
						global_dial_vars.roam_status=0;
				}
				
				free_two(result_useful,5,8);
				if(srv_status==0||srv_status==4||srv_domain==0)
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					global_dial_vars.network_link=0;
					*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
				}
				else
				{
					*Dial_proc_state=Dial_State_BMBAND;		
				}
				sys_mode=sys_mode;

			}
			break;
		case Dial_State_BMBAND:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMBAND:"))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMBAND:");
				if(NULL!=ptr_tmp)
				{
					char* ptr_tmp1=NULL;
					ptr_tmp1=ptr_tmp+strlen("+BMBAND:");
					char* ptr_tmp2=strstr(ptr_tmp1,"OK");
					if(NULL!= ptr_tmp2)
						ptr_tmp2[0]='\0';
					ptr_tmp=strip_head_tail_space(ptr_tmp1);
					if(strlen(ptr_tmp)<64)
						strncpy(global_dial_vars.band,ptr_tmp,strlen(ptr_tmp));
				}
			}
			*Dial_proc_state=Dial_State_BMRAT;
			break;
			
		case Dial_State_BMRAT:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMRAT:"))
			{
				char *network_type_string=strstr(global_dialtool.buffer_at_sponse,"+BMRAT:");
				if(NULL!= network_type_string)
					network_type_string=strip_head_tail_space(network_type_string+strlen("+BMRAT:"));
				else
				{
					break;
				}
				
				/*
				LTE
				TDS
				HSPA+
				HSUPA
				HSDPA
				HSPA
				HSDPA+
				DC HSDPA+
				UMTS
				HDR RevA
				HDR RevB
				HDR Rev0
				GPRS
				EDGE
				GSM
				1x
				NONE
				
				+PSRAT: HDR - EMPA EHRPD
				
				+NWTYPEIND:31 //"no service",
				+NWTYPEIND:32 //"network:gsm",
				+NWTYPEIND:33 //"network:gprs",
				+NWTYPEIND:34 //"network:edge",
				+NWTYPEIND:35 //"network:wcdma",
				+NWTYPEIND:36 //"network:hsdpa",
				+NWTYPEIND:37 //"network:hsupa"
				+NWTYPEIND:38 //"network (hsdpa_plus)"
				+NWTYPEIND:39 //"network (td_scdma)"
				+NWTYPEIND:40 //"network (lte_fdd)"
				+NWTYPEIND:41 //"network (lte_tdd)"
				+NWTYPEIND:42 //"network (EVDO)"
				+NWTYPEIND:43 //CDMA
				+NWTYPEIND:44 //CDMA&EVDO
				+NWTYPEIND:45 //EVDO
				*/
				if( A_CMP_B_ACCORDING_B( network_type_string,"TD-LTE" ) )
				{
					network_type=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDD LTE
				else if(A_CMP_B_ACCORDING_B( network_type_string,"TDD" ) )
				{
					network_type=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//FDD LTE
				//FDD-LTE
				else if( A_CMP_B_ACCORDING_B( network_type_string,"FDD" ) )
				{
					network_type=40;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//LTE
				//LTE TDD
				//LTE FDD
				else if( A_CMP_B_ACCORDING_B( network_type_string,"LTE" ) )
				{
					network_type=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status == 1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDSCDMA
				else if( A_CMP_B_ACCORDING_B( network_type_string,"TDS" ) )
				{
					network_type=39;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				//HSPA+
				//HSUPA
				//HSDPA
				//HSPA
				//HSDPA+
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HS" ) )
				{
					network_type=38;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"DC HSDPA+" ) )
				{
					network_type=36;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"WCDMA" ) )
				{
					network_type=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"UMTS" ) )
				{
					network_type=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EDGE" ) )
				{
					network_type=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GPRS" ) )
				{
					network_type=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GSM" ) )
				{
					network_type=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( strstr( network_type_string,"EHRPD" ) )
				{
					network_type=44;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EVDO" ) )
				{
					network_type=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//HDR RevA
				//HDR RevB
				//HDR Rev0
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HDR" ) )
				{
					network_type=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//CDMA
				//CDMA&EVDO
				else if( A_CMP_B_ACCORDING_B( network_type_string,"CDMA" ) )
				{
					network_type=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"1x" ) )
				{
					network_type=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"NONE" ) )
				{
					network_type=31;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=-1;
					global_dial_vars.network_link=0;
				}
				else
				{
					network_type=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				char * ptr_tmp=strstr(network_type_string,"OK");
				if(NULL != ptr_tmp)
				{
					ptr_tmp[0]='\0';
					char * ptr_tmp1=strip_head_tail_space(network_type_string);
					strlcpy(global_dial_vars.network_mode_detail,ptr_tmp1,strlen(ptr_tmp1)+1);
				}
			}
			if(network_type != global_dial_vars.network_mode_bmrat &&
				(network_type ==45 ||network_type==43 ||global_dial_vars.network_mode_bmrat ==45 
				|| global_dial_vars.network_mode_bmrat ==43) )
			{
				global_dial_vars.network_mode_bmrat=network_type;
				global_dial_vars.network_link=0;
				global_dialtool.Dial_Lvl_1=DIAL_DIAL;
				*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
			}
			else
			{
				if(global_dial_vars.evdo_cdma_flag == 1)
				{
					if(global_dial_vars.network_mode_bmrat ==43)
					{
						static char init_cdma_rssi_flag=0;
						if(init_cdma_rssi_flag==0)
						{
							sprintf(global_dial_vars.signal_rssi_value,"65");
							global_dial_vars.signal_rssi_level=4;
							init_cdma_rssi_flag=1;
						}		
						*Dial_proc_state=Dial_State_BMDATASTATUS;
					}
					else
						*Dial_proc_state=Dial_State_HDRCSQ;
				}
				else
					*Dial_proc_state=Dial_State_QCRMCALL_V4V6;
			}


			break;
		case Dial_State_HDRCSQ:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL != strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:"))
			{
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:")+strlen("^HDRCSQ:");
				int rssi_hdr=atoi(tmp_ptr);
				if(rssi_hdr==99)
				{
					global_dial_vars.signal_rssi_level=5;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"60");
				}
				else if(rssi_hdr ==80)
				{
					global_dial_vars.signal_rssi_level=4;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"65");
				}
				else if(rssi_hdr ==60)
				{
					global_dial_vars.signal_rssi_level=3;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"80");
				}
				else if(rssi_hdr ==40)
				{
					global_dial_vars.signal_rssi_level=2;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"95");
				}
				else if(rssi_hdr ==20)
				{
					global_dial_vars.signal_rssi_level=1;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"105");
				}
				else
				{
					global_dial_vars.signal_rssi_level=0;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"0");
				}
				*Dial_proc_state=Dial_State_BMDATASTATUS;	
			}
			break;

		case Dial_State_QCRMCALL_V4V6:
			{
				if(NULL!= strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					if( NULL!= strstr(global_dialtool.buffer_at_sponse,"1, V6") ) 
						global_dial_vars.ipv6_register_status = 1;
					else
						global_dial_vars.ipv6_register_status = 0;

					if( NULL!= strstr(global_dialtool.buffer_at_sponse,"1, V4") ) 
						global_dial_vars.ipv4_register_status = 1;
					else
						global_dial_vars.ipv4_register_status = 0;
				}

				if (strcmp(global_init_parms.ipstack,"IPV4V6") == 0)
				{
					if( global_dial_vars.ipv6_register_status == 1 && global_dial_vars.ipv4_register_status == 1 )
					{
						connect_status_fail_count = 0;
					}
					else
					{
						connect_status_fail_count ++;
					}
				}
				else
				{
					if(global_dial_vars.ipv4_register_status == 1 )
					{
						connect_status_fail_count = 0;
					}
					else
					{
						connect_status_fail_count ++;
					}
				}

				if(connect_status_fail_count > 1)
				{
					if( data_link_status_count > 0)
					{
						global_dialtool.Dial_Lvl_1=DIAL_DIAL;
						*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
					}
					else
					{
						if (strcmp(global_init_parms.ipstack,"IPV4V6") == 0)
						{
							if ( global_dial_vars.ipv6_register_status == 0 && global_dial_vars.ipv4_register_status == 0)
							{
								global_dialtool.Dial_Lvl_1=DIAL_DIAL;
								*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
							}
							else if ( global_dial_vars.ipv6_register_status == 0)
							{
								global_dialtool.Dial_Lvl_1=DIAL_DIAL;
								*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT_V6;
							}
							else
							{
								global_dialtool.Dial_Lvl_1=DIAL_DIAL;
								*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT_V4;
							}
						}
						else
						{
							global_dialtool.Dial_Lvl_1=DIAL_DIAL;
							*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
						}
					}
					data_link_status_count = 0;
					connect_status_fail_count = 0;
				}
				else
					*Dial_proc_state=Dial_State_BMDATASTATUS;
			}
			break;
		case Dial_State_BMDATASTATUS:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:"))
			{
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:")+strlen("+BMDATASTATUS:");
				global_dial_vars.network_link=atoi(tmp_ptr);
				if(global_dial_vars.network_link!=1)
					data_link_status_count++;
				else
					data_link_status_count=0;
				if(data_link_status_count>5)
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					data_link_status_count=0;
					*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
				}
				else
				{
					if( !check_file_exist(GPS_IS_ENABLE) )
					{
						*Dial_proc_state=Dial_State_BMTCELLINFO;
						sleep(5);
					}
					else
					{
						*Dial_proc_state=Dial_State_GPSINFO;
					}
				}
			}
			break;
		case Dial_State_GPSINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+GPSINFO:"))
			{
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+GPSINFO:")+strlen("+GPSINFO:");
				char* tmp_ptr_1=strstr(tmp_ptr,CMD_EXE_OK);
				if(NULL!=tmp_ptr_1)
					tmp_ptr_1[0]='\0';
				cmd_echo(tmp_ptr,"/tmp/.gpsinfo");
				*Dial_proc_state=Dial_State_BMTCELLINFO;
			}
			break;
#if 0
		case Dial_State_BMTCELLINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:"))
			{
				char tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:")+strlen("+BMTCLELLINFO:");
				
			}
			*Dial_proc_state=Dial_State_SYSINFO;
#endif
		default:
			break;
	}
}

void bm916_report_handle(char* buffer)
{
	if(NULL!=strstr(buffer,"+SIGNALIND:"))
	{
		//char* ptr_tmp=strstr(buffer,"+SIGNALIND:")+strlen("+SIGNALIND:");
		//global_dial_vars.signal_rssi_level=atoi(ptr_tmp);
	}
	if(NULL!=strstr(buffer,"^DATACON:"))
	{
		char* ptr_tmp=strstr(buffer,"^DATACON:")+strlen("^DATACON:");
		global_dial_vars.network_link=atoi(ptr_tmp);
	}
	if(NULL!=strstr(buffer,"^DSDORMANT:"))
	{
		char* ptr_tmp=strstr(buffer,"^DSDORMANT:")+strlen("^DSDORMANT:");
		global_dial_vars.module_dormant=atoi(ptr_tmp);
	}
	if(NULL!=strstr(buffer,"^RSSILVL:"))
	{
		char* ptr_tmp=strstr(buffer,"^RSSILVL:")+strlen("^RSSILVL:");
		int signal_lvl=atoi(ptr_tmp);
		if(signal_lvl==0)
		{
			sprintf(global_dial_vars.signal_rssi_value,"0");
			//global_dial_vars.signal_rssi_level=0;
		}
		else if(signal_lvl==20)
		{
			sprintf(global_dial_vars.signal_rssi_value,"105");
			//global_dial_vars.signal_rssi_level=1;
		}
		else if(signal_lvl==40)
		{
			sprintf(global_dial_vars.signal_rssi_value,"100");
			//global_dial_vars.signal_rssi_level=2;
		}
		else if(signal_lvl==60)
		{
			sprintf(global_dial_vars.signal_rssi_value,"80");
			//global_dial_vars.signal_rssi_level=3;
		}
		else if(signal_lvl==80)
		{
			sprintf(global_dial_vars.signal_rssi_value,"65");
			//global_dial_vars.signal_rssi_level=4;
		}
		else
		{
			sprintf(global_dial_vars.signal_rssi_value,"999");
			//global_dial_vars.signal_rssi_level=5;
		}
	}
	if(NULL!=strstr(buffer,"^ HRSSILVL:"))
	{
		char* ptr_tmp=strstr(buffer,"^HRSSILVL:")+strlen("^HRSSILVL:");
		int signal_lvl=atoi(ptr_tmp);
		if(signal_lvl==0)
			global_dial_vars.signal_rssi_level=0;
		else if(signal_lvl==20)
			global_dial_vars.signal_rssi_level=1;
		else if(signal_lvl==40)
			global_dial_vars.signal_rssi_level=2;
		else if(signal_lvl==60)
			global_dial_vars.signal_rssi_level=3;
		else if(signal_lvl==80)
			global_dial_vars.signal_rssi_level=4;
		else
			global_dial_vars.signal_rssi_level=5;

	}
	if(NULL!=strstr(global_dialtool.buffer_at_sponse,"ICCID:"))
	{
		char* ptr_tmp1=strstr(global_dialtool.buffer_at_sponse,"ICCID:")+strlen("ICCID:");
		char* ptr_tmp2=strstr(ptr_tmp1,"OK");
		if(NULL!= ptr_tmp2)
			ptr_tmp2[0]='\0';
		char* ptr_tmp=strip_head_tail_space(ptr_tmp1);
		if(strlen(ptr_tmp) < 64)
			strncpy(global_dial_vars.iccid,ptr_tmp,strlen(ptr_tmp));
	}
	if(NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:"))
	{
		char* ptr_tmp1=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:")+strlen("+BMBANDPREF:");
		char* ptr_tmp3=strstr(ptr_tmp1,",");
		if(NULL!= ptr_tmp3)
			ptr_tmp3=ptr_tmp3+1;
		else
			return;
		char* ptr_tmp4=strstr(ptr_tmp3,",");
		if(NULL!=ptr_tmp4)
			ptr_tmp4='\0';
		char* ptr_tmp=strip_head_tail_space(ptr_tmp3);
		strncpy(global_dial_vars.lte_lock_band,ptr_tmp,strlen(ptr_tmp));
	}
	/*
	if(NULL!=strstr(buffer,"^MODE:"))
	{
	}
	*/

#if 0
	/*short mesg dealwith*/
	if(NULL!=strstr(buffer,"^UIMST:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMT:"))
	{
	}
	if(NULL!=strstr(buffer,"^SMMEMFULL:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMGSS:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMGSF:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMT:"))
	{
	}

#endif


}
void bm916_get_moduleinfo(MDI* p)
{
	int flag_cmd;
	int bytes_n=0;
	fd_set readfds;
	char *buffer_recv=NULL;
	int maxfd;
	int out_flag;
	int reply_count=0;	//we must make sure all data received,we judge by CMD_EXE_OK and so on
	struct timeval tv;
	int imei_check_flag=0;
	int length;
	char buffer[1024]={0};
	char* str_ptr=NULL;
	char* str_sub_ptr=NULL;
	char* tmp_ptr=NULL;
	char* result_ptr=NULL;	
	buffer_recv=(char *)m_malloc(RECV_BUFF_SIZE);
	char* ptr_tmp=buffer_recv;
	char* ptr_tmp1=NULL;
	//int retry = 3;
	
	/*//cfun=0, 1 to reset modem
	memset(buffer_recv,0,RECV_BUFF_SIZE);
	util_send_cmd(global_dialtool.dev_handle,"at+cfun=0\r",&global_dialtool.pthread_moniter_flag);
	sleep(1);
	read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
	log_info("cfun==0: receive: %s\n",buffer_recv);
	memset(buffer_recv,0,RECV_BUFF_SIZE);
	util_send_cmd(global_dialtool.dev_handle,"at+cfun=1\r",&global_dialtool.pthread_moniter_flag);
	sleep(4);
	read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
	log_info("cfun==1: receive: %s\n",buffer_recv);
	while(!strstr(buffer_recv,"OK") && --retry)
	{
		log_info("restart cfun fail,now retry %d", retry);
		//system("/etc/rc.d/rc.reset_module");
		//exit(-1);

		memset(buffer_recv,0,RECV_BUFF_SIZE);
		util_send_cmd(global_dialtool.dev_handle,"at+cfun=0\r",&global_dialtool.pthread_moniter_flag);
		sleep(1);
		util_send_cmd(global_dialtool.dev_handle,"at+cfun=1\r",&global_dialtool.pthread_moniter_flag);
		sleep(1);
		read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
		log_info("cfun retry: receive: %s\n",buffer_recv);
	}

	if(0 == retry)
	{
		log_info("restart cfun fail,now reset the module");
		system("/etc/rc.d/rc.reset_module");
		exit(-1);
	}
	*/
	while(1)
	{
		memset(buffer_recv,0,RECV_BUFF_SIZE);
		flag_cmd=util_send_cmd(global_dialtool.dev_handle,"ATI\r",&global_dialtool.pthread_moniter_flag);
		reply_count++;
		log_info("we are sending ati ,and get module info detailed:%d\n",flag_cmd);
		if(TRUE!=flag_cmd)
		{
			sleep(2);
			continue;
		}
		do
		{
			tv.tv_sec=6;
			tv.tv_usec=0;

			FD_ZERO(&readfds);
			FD_SET(global_dialtool.dev_handle,&readfds);
			maxfd=global_dialtool.dev_handle+1;
			if((out_flag=select(maxfd,&readfds,NULL,NULL,&tv))>0)
			{
				sleep(1);
				if(out_flag>0)
				{
					out_flag=read(global_dialtool.dev_handle,buffer_recv+bytes_n,RECV_BUFF_SIZE-bytes_n-1);
					log_info("%s_%d:%s %d\n",__FUNCTION__,__LINE__,buffer_recv,strlen(buffer_recv));
					bytes_n+=out_flag;
				}
			}

			if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_EXE_OK)))
			{
				
				log_info("%s_%d:AT OK %s %d\n",__FUNCTION__,__LINE__,ptr_tmp,strlen(ptr_tmp));
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_EXE_OK);
				
			}else if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_EXE_ERROR)))
			{
				
				log_info("%s_%d:AT ERROR %s %d\n",__FUNCTION__,__LINE__,ptr_tmp,strlen(ptr_tmp));
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_EXE_ERROR);
			}else if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_RESULT_CME_ERROR)))
			{
				log_info("%s_%d:AT CME %s %d\n",__FUNCTION__,__LINE__,ptr_tmp,strlen(ptr_tmp));
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_RESULT_CME_ERROR);
			}else if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_RESULT_CMS_ERROR)))
			{
				log_info("%s_%d:AT CMS %s %d\n",__FUNCTION__,__LINE__,ptr_tmp,strlen(ptr_tmp));
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_RESULT_CMS_ERROR);
			}
		}while(reply_count!=0);

		log_info("%s_%d:Getting info %s %d\n",__FUNCTION__,__LINE__,buffer_recv + bytes_n - out_flag,strlen(buffer_recv + bytes_n - out_flag));
		
		str_ptr=strstr(buffer_recv + bytes_n - out_flag,"Manufacturer:");
		
		if(NULL!=str_ptr)
		{
			tmp_ptr=str_ptr+strlen("Manufacturer:");
			str_sub_ptr=strstr(tmp_ptr,"\n");
			if(NULL != str_sub_ptr)
			{
				length=str_sub_ptr-tmp_ptr;
				strncpy(buffer,tmp_ptr,length);
				buffer[length]='\0';
				result_ptr=strip_head_tail_space(buffer);
				if(NULL != result_ptr)
				strncpy(p->manufacturer,result_ptr,strlen(result_ptr));
			}
		}
		else
		{
			continue;
		}
		
		str_ptr=strstr(buffer_recv + bytes_n - out_flag,"Model:");
		if(NULL!=str_ptr)
		{
			tmp_ptr=str_ptr+strlen("Model:");
			str_sub_ptr=strstr(tmp_ptr,"\n");
			if(NULL != str_sub_ptr)
			{
				length=str_sub_ptr-tmp_ptr;
				strncpy(buffer,tmp_ptr,length);
				buffer[length]='\0';
				result_ptr=strip_head_tail_space(buffer);
				strncpy(p->hardver,result_ptr,strlen(result_ptr));
			}
		}
		else
		{
			continue;
		}

		str_ptr=strstr(buffer_recv + bytes_n - out_flag,"Revision:");
		if(NULL!=str_ptr)
		{
			tmp_ptr=str_ptr+strlen("Revision:");
			str_sub_ptr=strstr(tmp_ptr,"\n");
			if(NULL != str_sub_ptr)
			{
				length=str_sub_ptr-tmp_ptr;
				strncpy(buffer,tmp_ptr,length);
				buffer[length]='\0';
				result_ptr=strip_head_tail_space(buffer);
				strncpy(p->softver,result_ptr,strlen(result_ptr));
			}
		}
		else
		{
			continue;
		}

		str_ptr=strstr(buffer_recv + bytes_n - out_flag,"IMEI:");
		if(NULL!=str_ptr)
		{
			tmp_ptr=str_ptr+strlen("IMEI:");
			str_sub_ptr=strstr(tmp_ptr,"\n");
			if(NULL != str_sub_ptr)
			{
				length=str_sub_ptr-tmp_ptr;
				strncpy(buffer,tmp_ptr,length);
				buffer[length]='\0';
				result_ptr=strip_head_tail_space(buffer);
				if(imei_check_flag!=1)
				{
					strncpy(p->imei,result_ptr,strlen(result_ptr));
				}
				else
				{
					char tmp_buffer[48]={0};
					num_asciistr_to_decimalstr(result_ptr,tmp_buffer);
					if(tmp_buffer[0] != '\0')
						strncpy(p->imei,tmp_buffer,strlen(tmp_ptr));
				}
			}
		}

		if(p->imei[0] == '\0' )
		{
			char imeiBuffer[100] = "";
			char* imeiAsciiBegin = NULL;
			log_info("%s_%d:send amd AT+BM IMEI \n",__FUNCTION__,__LINE__);
			util_send_cmd(global_dialtool.dev_handle,"AT+BMSN=0,7\r",&global_dialtool.pthread_moniter_flag);
			tv.tv_sec=6;
			tv.tv_usec=0;
			FD_ZERO(&readfds);
			FD_SET(global_dialtool.dev_handle,&readfds);
			maxfd=global_dialtool.dev_handle+1;
			
			if((out_flag=select(maxfd,&readfds,NULL,NULL,&tv))>0)
			{
				sleep(1);
				if(out_flag>0)
				{
					out_flag=read(global_dialtool.dev_handle,imeiBuffer,sizeof(imeiBuffer)-1);
					log_info("%s_%d:cdma mode, get imei: %s\n",__FUNCTION__,__LINE__,imeiBuffer);
				}
			}
			if( (imeiAsciiBegin = strstr(imeiBuffer,"BMIMEI:")) != NULL && 
			    	strlen(imeiAsciiBegin) > 29 + strlen("BMIMEI:") )
			{
				int i = 1;
				imeiAsciiBegin = imeiAsciiBegin + strlen("BMIMEI:");
				for(i = 0; i < 15; i++)
				{
					p->imei[i] = imeiAsciiBegin[i*2+1];
				}
			}
			else
			{
				log_info("%s_%d: when cdma mode,get imei error\n",__FUNCTION__,__LINE__);
				continue;
			}
			break;
		}

		memset(buffer_recv,0,RECV_BUFF_SIZE);
		flag_cmd=util_send_cmd(global_dialtool.dev_handle,"AT+BMSWVER\r",&global_dialtool.pthread_moniter_flag);
		sleep(1);
		read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
		log_info("%s_%d: >>>>>>>>>>>>real version: %s\n",__FUNCTION__,__LINE__, buffer_recv);

		if(buffer_recv[0])
		{
			str_ptr=strstr(buffer_recv,"+BMSWVER:");
			if(NULL!=str_ptr)
			{
				tmp_ptr=str_ptr+strlen("+BMSWVER: ");
				str_sub_ptr=strstr(tmp_ptr,"\n");
				if(NULL != str_sub_ptr)
				{
					length=str_sub_ptr-tmp_ptr;
					strncpy(buffer,tmp_ptr,length);
					buffer[length]='\0';
					result_ptr=strip_head_tail_space(buffer);
					strncpy(p->softver,result_ptr,strlen(result_ptr));
				}
			}
		}

		m_free(buffer_recv);
		break;	
	}		
}

DialProc process_bm916=
{
	bm916_init,
	bm916_dial,
	bm916_deamon,
	bm916_sendat,
	bm916_report_handle,
};

