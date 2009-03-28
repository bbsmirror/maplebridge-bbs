/*-------------------------------------------------------*/
/* struct.h	( NTHU CS MapleBBS Ver 2.36 )		 */
/*-------------------------------------------------------*/
/* target : all definitions about data structure	 */
/* create : 95/03/29				 	 */
/* update : 95/12/15				 	 */
/*-------------------------------------------------------*/


#ifndef _STRUCT_H_
#define _STRUCT_H_


/* screen control */

#define STRLEN		80	/* Length of most string data */
#define ANSILINELEN	900	/* Maximum Screen width in chars */

/* itoc.031123: �ù����e�׳]�� 80 �]�L���A�u�O���� telnet term �b�K�W��r�Ӫ��ɡA
                �|�۰��_��b 79�A�ҥH�b���N�q��]�w */
#define SCR_WIDTH	78	/* edit/more/talk/visio screen width */

#define TAB_STOP	4	/* �� TAB �����X��ť� (�n�O 2 ������) */
#define TAB_WIDTH	(TAB_STOP - 1)

#define T_LINES		50	/* maximum total lines */
#define T_COLS		120	/* maximum total columns�A�n�� ANSILINELEN �p */

/* board structure */

#define BNLEN		12	/* Length of board name */
#define BCLEN		4	/* Length of board class */
#define BTLEN		43	/* Length of board title */
#define BMLEN		36	/* Length of board managers */

/* file structure */

#define TTLEN		72	/* Length of title */
#define FNLEN		28	/* Length of filename  */

/* user structure */
/* DES �s�X�k�A���� 8 �Ӧr�����ת��r��A���ͪ���Ȭ� 13 �줸�� */

#define IDLEN		12	/* Length of user id */
#define PASSLEN		13	/* Length of encrypted passwd field */
#define PSWDLEN		8	/* Length of passwd (���[�K������) */
#define RNLEN		19	/* Length of real name */
#define UNLEN		23	/* Length of user name */


typedef char const *STRING;
typedef unsigned char uschar;	/* length = 1 */
typedef unsigned int usint;	/* length = 4 */
typedef struct UTMP UTMP;


/* ----------------------------------------------------- */
/* �ϥΪ̱b�� .ACCT struct : 512 bytes			 */
/* ----------------------------------------------------- */


typedef struct
{
  int userno;			/* unique positive code */

  char userid[IDLEN + 1];	/* ID */
  char passwd[PASSLEN + 1];	/* �K�X */
  char realname[RNLEN + 1];	/* �u��m�W */
  char username[UNLEN + 1];	/* �ʺ� */

  usint userlevel;		/* �v�� */
  usint ufo;			/* habit */
  uschar signature;		/* �w�]ñ�W�� */

  char year;			/* �ͤ�(����~) */
  char month;			/* �ͤ�(��) */
  char day;			/* �ͤ�(��) */
  char sex;			/* �ʧO 0:���� �_��:�k�� ����:�k�� */
  int money;			/* �ȹ� */
  int gold;			/* ���� */

  int numlogins;		/* �W������ */
  int numposts;			/* �o���� */
  int numemails;		/* �H�o Inetrnet E-mail ���� */

  time4_t firstlogin;		/* �Ĥ@���W���ɶ� */
  time4_t lastlogin;		/* �W�@���W���ɶ� */
  time4_t tcheck;		/* �W�� check �H�c/�B�ͦW�檺�ɶ� */
  time4_t tvalid;		/* �Y���v�A���v�������ɶ��F
                                   �Y�����v�B�q�L�{�ҡA�q�L�{�Ҫ��ɶ��F
                                   �Y�����v�B���q�L�{�ҡA�{�Ҩ窺 time-seed */
  usint staytime;			/* �`�@���d�ɶ� */

  char lasthost[30];		/* �W���n�J�ӷ� */
  char email[56];		 /* �ثe�n�O���q�l�H�c */
  int good_article;		/* �u�� */
  int poor_article;		/* �H�� */
  int violation;		/* �H�W�O�� */
  usint ufo2;
  int reserved_1;
  char cmode[20];		/* �ۭq�ʺA */
  char cfrom[34];		/* �ۭq�ӷ� */
  char reserved[202];		/* �O�d */
}      ACCT;


typedef struct			/* 16 bytes */
{
  time4_t uptime;
  char userid[IDLEN];
}      SCHEMA;


#ifdef HAVE_REGISTER_FORM

/* itoc.041025: RFROM �����X�{�M ACCT �@�˦��ȥi�ण�P�����
   RFORM �M ACCT �ߤT�ۦP�����O userno�Buserid�Brealname */

typedef struct	/* ���U��� (Register From) 256 bytes */
{
  int userno;
  time4_t rtime;
  char userid[IDLEN + 1];
  char agent[IDLEN + 1];
  char realname[RNLEN + 1];
  char career[50];
  char address[60];
  char phone[20];
  char reply[72];
}      RFORM;

#endif


#include "hdr.h"


/* ----------------------------------------------------- */
/* control of board vote : 256 bytes			 */
/* ----------------------------------------------------- */


/* itoc.041101.����: VCH �M HDR �� xname ��m�B���׭n�ǰt */

typedef struct VoteControlHeader
{
  time4_t chrono;		/* �벼�}��ɶ� */	/* Thor: �� key �ӥB match HDR chrono */
  time4_t bstamp;		/* �ݪO���ѥN�X */	/* Thor: �� key */
  time4_t vclose;		/* �벼�����ɶ� */

  char xname[32];		/* �D�ɦW */		/* Thor: match HDR �� xname */
  char date[9];			/* �}�l��� */		/* Thor: match HDR �� date */
  char cdate[9];		/* ������� */		/* Thor: �u����ܡA������� */  
  char owner[IDLEN + 1];	/* �|��H */
  char title[TTLEN + 1];	/* �벼�D�D */

  char vgamble;			/* �O�_����L        '$':��L  ' ':�@��벼 */
  char vsort;			/* �}�����G�O�_�Ƨ�  's':�Ƨ�  ' ':���Ƨ� */
  char vpercent;		/* �O�_��ܦʤ����  '%':�ʤ�  ' ':�@�� */
  char vprivate;		/* �O�_���p�H�벼    ')':�p�H  ' ':���} */

  int maxblt;			/* �C�H�i��X�� */
  int price;			/* �C�i�䲼����� */

  int limitlogins;		/* ����n�n�J�W�L�X���H�W���ϥΪ̤~��벼 */
  int limitposts;		/* ����n�o��W�L�X���H�W���ϥΪ̤~��벼 */

  char nouse[88];
}      VCH;


typedef char vitem_t[32];	/* �벼�ﶵ */


typedef struct
{
  char userid[IDLEN + 1];
  char numvotes;		/* ��X�i */
  usint choice;
}      VLOG;


/* filepath : brd/<board>/.VCH, brd/<board>/@/... */


/* ----------------------------------------------------- */
/* Mail-Queue struct : 256 bytes			 */
/* ----------------------------------------------------- */


typedef struct
{
  time_t mailtime;		/* �H�H�ɶ� */
  char method;
  char sender[IDLEN + 1];
  char username[UNLEN + 1];
  char subject[TTLEN + 1];
  char rcpt[60];
  char filepath[77];
  char *niamod;			/* reverse domain */
}      MailQueue;


#define MQ_JUSTIFY	0x01	/* �����{�ҫH�� */
#define MQ_ATTACH	0x02	/* �� attachment ���H�� */


/* ----------------------------------------------------- */
/* PAL : friend struct : 64 bytes			 */
/* ----------------------------------------------------- */


typedef struct
{
  char userid[IDLEN + 1];
  char ftype;
  char ship[46];
  int userno;
}      PAL;

#define	PAL_BAD		0x02	/* �n�� vs �a�H */
#define PAL_MATE      	0x04	/* �S��n�� */
#define MATE_MASK     	0x01000000	/* �H�Ƥ��|�W�L 16^7-1 */


/* ----------------------------------------------------- */
/* structure for call-in message : 100 bytes		 */
/* ----------------------------------------------------- */


typedef struct
{
  time4_t btime;
  UTMP *caller;			/* who call-in me ? */
  int sender;			/* calling userno */
  int recver;			/* called userno */
  char userid[IDLEN + 1 + 2];	/* itoc.010529: �O�d 2 byte ���s���Ÿ� > */
  char msg[69];			/* ���y */
  char nick[20];		/* msn �ǰe�һݪ� nick */
}      BMW;			/* bbs message write */


#ifdef LOGIN_NOTIFY

/* ----------------------------------------------------- */
/* BENZ : �t�Ψ�M�W�� : 20 bytes			 */
/* ----------------------------------------------------- */


typedef struct
{
  int userno;
  char userid[IDLEN + 1];
}       BENZ;

#endif	/* LOGIN_NOTIFY */


#ifdef HAVE_ALOHA

/* ----------------------------------------------------- */
/* ALOHA : �W���q���W�� : 20 bytes			 */
/* ----------------------------------------------------- */


typedef struct
{
  char userid[IDLEN + 1];
  int userno;
}      ALOHA;


/* ----------------------------------------------------- */
/* FRIENZ : �W���q���W�� : 100 bytes			 */
/* ----------------------------------------------------- */


/* itoc.041011.����: �ڥ��S���n�γo��j */

typedef struct
{
  int nouse1;
  int nouse2;
  int nouse3;
  int userno;
  char userid[IDLEN + 1];
  char nouse4[71];
}      FRIENZ;

#endif	/* HAVE_ALOHA */


/* ----------------------------------------------------- */
/* PAYCHECK : �䲼 : 32 bytes				 */
/* ----------------------------------------------------- */


typedef struct
{
  time4_t tissue;		/* �o�䲼�ɶ� */
  int money;
  int gold;
  char reason[20];
}      PAYCHECK;


/* ----------------------------------------------------- */
/* Structure used in UTMP file : 128 bytes		 */
/* ----------------------------------------------------- */


struct UTMP
{
  pid_t pid;			/* process ID */
  int userno;			/* user number */

  int mode;			/* bbsmode */
  usint userlevel;		/* the save as ACCT.userlevel */
  usint ufo;			/* the same as ACCT.ufo */
  usint ufo2;			/* the same as ACCT.ufo2 */
  usint status;			/* status */

  time4_t idle_time;		/* active time for last event */
  u_long in_addr;		/* Internet address */
  int sockport;			/* socket port for talk */
  UTMP *talker;			/* who talk-to me ? */

  BMW *mslot[BMW_PER_USER];

  char userid[IDLEN + 1];	/* user's ID */
  char mateid[IDLEN + 1];	/* partner's ID */
  char username[UNLEN + 1];	/* user's nickname */
  char from[34];		/* remote host */
  char cmode[11];
  char cfrom[34];		/* user's custom from */
#ifdef HAVE_BRDMATE
  char reading[BNLEN + 1];	/* reading board */
#endif

  int pal_max;			/* ���X�ӪB�� */
  int pal_spool[PAL_MAX];	/* �Ҧ��B�ͪ� userno */

#ifdef BMW_COUNT
  int bmw_count;		/* �O�����F�X�Ӥ��y */
#endif
};


/* ----------------------------------------------------- */
/* BOARDS struct : 128 bytes				 */
/* ----------------------------------------------------- */


typedef struct BoardHeader
{
  char brdname[BNLEN + 1];	/* board name */
  char class[BCLEN + 1];
  char title[BTLEN + 1];
  char BM[BMLEN + 1];		/* BMs' uid, token '/' */

  char bvote;			/* 0:�L�벼 -1:����L(�i�঳�벼) 1:���벼 */

  time4_t bstamp;		/* �إ߬ݪO���ɶ�, unique */
  usint readlevel;		/* �\Ū�峹���v�� */
  usint postlevel;		/* �o��峹���v�� */
  usint battr;			/* �ݪO�ݩ� */
  time4_t btime;		/* -1:bpost/blast �ݭn��s */
  int bpost;			/* �@���X�g post */
  time4_t blast;		/* �̫�@�g post ���ɶ� */
}           BRD;


typedef struct
{
  int pal_max;			/* ���X�ӪO�� */
  int pal_spool[PAL_MAX];	/* �Ҧ��O�ͪ� userno */
}	BPAL;


#ifdef DO_POST_FILTER
/* �ݪO�v��: 36 bytes */
typedef struct
{
  char exist;		/* �v���ɬO�_�s�b: �O�_�ݭn�P�_ */
  char age;
  char sex;
  char regmonth;
  int numlogins;
  int numposts;
  int good_article;
  int poor_article;
  int violation;
  int money;
  int gold;
  int numemails;
}	BPERM;
#endif


/* ----------------------------------------------------- */
/* Class image						 */
/* ----------------------------------------------------- */


#define CLASS_INIFILE		"Class"

/* itoc.010413: �� class.img �����G�� */
#define CLASS_IMGFILE_NAME	"run/classname.img"
#define CLASS_IMGFILE_TITLE	"run/classtitle.img"


#define CH_MAX		10000	/* �̤j�����ƥ� */
#define	CH_END		-1
#define	CH_TTLEN	64


typedef	struct
{
  int count;
  char title[CH_TTLEN];
  short chno[0];
}	ClassHeader;


#ifdef MY_FAVORITE

/* ----------------------------------------------------- */
/* favor.c ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */


typedef struct MF
{
  time4_t chrono;		/* �إ߮ɶ� */
  int mftype;			/* type */
  char xname[BNLEN + 1];	/* �O�W���ɦW */
  char class[BCLEN + 1];	/* ���� */
  char title[BTLEN + 1];	/* �D�D */
}	MF;

#define	MF_MARK		0x01	/* �Q mark �_�Ӫ� */
#define	MF_BOARD    	0x02	/* �ݪO���| */
#define	MF_FOLDER   	0x04	/* ���v */
#define	MF_GEM      	0x08	/* ��ذϱ��| */
#define MF_LINE		0x10	/* ���j�u */
#define MF_CLASS	0x20	/* �����s�� */
#define MF_LABEL	0x8000	/* �аO: �γ̫�@�� flag */

#endif  /* MY_FAVORITE */


#ifdef HAVE_COSIGN

/* ----------------------------------------------------- */
/* newbrd.c ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */


typedef struct NewBoardHeader
{
  char brdname[BNLEN + 1];
  char class[BCLEN + 1];
  char title[BTLEN + 1];
  time4_t btime;
  time4_t etime;
  char xname[32];
  char owner[IDLEN + 1];
  char innsrv[32];
  char inngrp[32];
  char date[9];
  usint mode;
  int total;
  int readlevel;
}	NBRD;


#define NBRD_FINISH	0x00001	/* �w���� */
#define NBRD_END	0x00002	/* �s�p���� */
#define NBRD_START	0x00004	/* �s�p�}�l */
#define NBRD_ANONYMOUS	0x00100	/* �ΦW */
#define NBRD_NEWBOARD	0x10000	/* �s�O�s�p */
#define NBRD_OTHER	0x20000	/* ��L�s�p */
#define NBRD_INN	0x01000	/* �]�w��H */
#define NBRD_PUBLIC	0x02000	/* �����ݪO */


#endif	/* HAVE_COSIGN */


#ifdef LOG_SONG_USIES

/* ----------------------------------------------------- */
/* SONG log ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */

typedef struct SONGDATA
{
  time4_t chrono;
  int count;		/* �Q�I���� */
  char title[80];
}	SONGDATA;

#endif	/* LOG_SONG_USIES */


/* ----------------------------------------------------- */
/* cache.c ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */


typedef struct
{
  int shot[MOVIE_MAX];	/* Thor.980805: �X�z�d�� 0..MOVIE_MAX - 1 */
  char film[MOVIE_SIZE];
  char today[16];
  char feast[16];
} FCACHE;


#define	FILM_SIZ	4000	/* max size for each film */


#define FILM_OPENING0	0	/* �}�Y�e��(��) */
#define FILM_OPENING1	1	/* �}�Y�e��(��) */
#define FILM_OPENING2	2	/* �}�Y�e��(��) */
#define FILM_OPENING3	3	/* �}�Y�e��(��) */
#define FILM_OPENING4	4	/* �}�Y�e��(��) */
#define FILM_OPENING5	5	/* �}�Y�e��(��) */
#define FILM_OPENING6	6	/* �}�Y�e��(��) */
#define FILM_OPENING7	7	/* �}�Y�e��(��) */
#define FILM_OPENING8	8	/* �}�Y�e��(��) */
#define FILM_OPENING9	9	/* �}�Y�e��(��) */
#define FILM_GOODBYE	10	/* �A���e�� */
#define FILM_NOTIFY	11	/* �|���q�L�{�ҳq�� */
#define FILM_MQUOTA	12	/* �H��W�L�O�s�����q�� */
#define FILM_MAILOVER	13	/* �H��ʼƹL�h�q�� */
#define FILM_MGEMOVER	14	/* �ӤH��ذϹL�h�q�� */
#define FILM_BIRTHDAY	15	/* �ͤ��Ѫ��W���e�� */
#define FILM_APPLY	16	/* ���U���ܵe�� */
#define FILM_JUSTIFY	17	/* �����{�Ҫ���k */
#define FILM_REREG	18	/* ���s�{�һ��� */
#define FILM_EMAIL	19	/* �l��H�c�{�һ��� */
#define FILM_NEWUSER	20	/* �s��W������ */
#define FILM_TRYOUT	21	/* �K�X���~ */
#define FILM_POST	22	/* �峹�o����� */
#define FILM_MOVIE	23	/* �ʺA�ݪO FILM_MOVIE �n��b�̫᭱ */


typedef struct
{
  UTMP uslot[MAXACTIVE];	/* UTMP slots */
  usint count;			/* number of active session */
  usint offset;			/* offset for last active UTMP */

  double sysload[3];
  int avgload;

  BMW *mbase;			/* sequential pointer for BMW */
  BMW mpool[BMW_MAX];
} UCACHE;


typedef struct
{
  BRD bcache[MAXBOARD];
  BPAL pcache[MAXBOARD];
#ifdef DO_POST_FILTER
  BPERM rperm[MAXBOARD];	/* �ݪOŪ���v�� */
  BPERM wperm[MAXBOARD];	/* �ݪO�g�J�v�� */
  BPERM lperm[MAXBOARD];	/* �ݪO�C�X�v�� */
#endif
  int mantime[MAXBOARD];	/* �U�O�ثe�����h�֤H�b�\Ū */
  int number;			/* �����ݪO���ƥ� */
  int numberOld;		/* ��}���ɬݪO���ƥ� */
  int min_chn;			/* �O���`�@���X�Ӥ��� */
  time4_t uptime;
} BCACHE;


/* ----------------------------------------------------- */
/* visio.c ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */


/* Screen Line buffer modes */


#define SL_MODIFIED	(1)	/* if line has been modifed, screen output */
#define SL_STANDOUT	(2)	/* if this line contains standout code */
#define SL_ANSICODE	(4)	/* if this line contains ANSI code */


typedef struct screenline
{
  int oldlen;		/* previous line length */
  int len;		/* current length of line */
  int width;		/* padding length of ANSI codes */
  int smod;		/* start of modified data */
  int emod;		/* end of modified data */
  int sso;		/* start of standout data */
  int eso;		/* end of standout data */
  uschar mode;		/* status of line, as far as update */ /* �ѩ� SL_* �� mode ���W�L�K�ӡA�G�� uschar �Y�i */
  uschar data[ANSILINELEN];
}          screenline;


typedef struct LinkList
{
  struct LinkList *next;
  char data[0];
}        LinkList;


/* ----------------------------------------------------- */
/* xover.c ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */


typedef struct OverView
{
  int pos;			/* current position */
  int top;			/* top */
  int max;			/* max */
  int key;			/* key */
  char *xyz;			/* staff */
  struct OverView *nxt;		/* next */
  char dir[0];			/* data path */
}        XO;


typedef struct
{
  int key;
  int (*func) ();
}      KeyFunc;


typedef struct
{
  XO *xo;
  KeyFunc *cb;
  int mode;
  char *feeter;
} XZ;


typedef struct
{
  time4_t chrono;
  int recno;
}      TagItem;


/* ----------------------------------------------------- */
/* poststat.c ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */


typedef struct
{
  char author[IDLEN + 1];
  char board[BNLEN + 1];
  char title[66];
  time_t date;		/* last post's date */
  int number;		/* post number */
} POSTLOG;


#ifdef MODE_STAT

/* ----------------------------------------------------- */
/* modestat.c ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */

typedef struct
{
  time4_t logtime;
  time4_t used_time[M_MAX + 1];	/* itoc.010901: depend on mode.h */
} UMODELOG;


typedef struct
{
  time4_t logtime;
  time4_t used_time[M_MAX + 1];	/* itoc.010901: depend on mode.h */
  int count[30];
  int usercount;
} MODELOG;

#endif	/* MODE_STAT */


/* ----------------------------------------------------- */
/* innbbsd ���B�Ϊ���Ƶ��c				 */
/* ----------------------------------------------------- */


typedef struct
{
  time4_t chrono;	/* >=0:stamp -1:cancel */
  char board[BNLEN + 1];

  /* �H�U��쪺�j�p�P HDR �ۦP */
  char xname[32];
  char owner[80];
  char nick[49];
  char title[TTLEN + 1];
} bntp_t;


#define INN_USEIHAVE	0x0001
#define INN_USEPOST	0x0002
#define INN_FEEDED	0x0010

typedef struct
{
  char name[13];	/* �ӯ��� short-name */
  char host[83];	/* �ӯ��� host */
  int port;		/* �ӯ��� port */
  usint xmode;		/* �ӯ��� xmode */
  char blank[20];	/* �O�d */
  int feedfd;		/* bbslink.c �ϥ� */
} nodelist_t;


#define INN_NOINCOME	0x0001
#define INN_ERROR	0x0004

typedef struct
{
  char path[13];	/* �Ӹs�թҹ��઺���x */
  char newsgroup[83];	/* �Ӹs�ժ��W�� */
  char board[BNLEN + 1];/* �Ӹs�թҹ������ݪO */
  char charset[11];	/* �Ӹs�ժ��r�� */
  usint xmode;		/* �Ӹs�ժ� xmode */
  int high;		/* �ثe���Ӹs�ժ����@�g */
} newsfeeds_t;


typedef struct
{
  char issuer[128];	/* NCM message ���o�H�H */
  char type[64];	/* NCM message �������W�� */
  int perm;		/* ���\�� NCM message �R�H (1:�} 0:��) */
  char blank[60];	/* �O�d */
} ncmperm_t;


#define INN_SPAMADDR	0x0001
#define INN_SPAMNICK	0x0002
#define INN_SPAMSUBJECT	0x0010
#define INN_SPAMPATH	0x0020
#define INN_SPAMMSGID	0x0040
#define INN_SPAMBODY	0x0100
#define INN_SPAMSITE	0x0200
#define INN_SPAMPOSTHOST	0x0400

typedef struct
{
  char detail[80];	/* �ӳW�h�� ���e */
  usint xmode;		/* �ӳW�h�� xmode */
  char board[BNLEN + 1];/* �ӳW�h�A�Ϊ��ݪO */
  char path[13];	/* �ӳW�h�A�Ϊ����x */
  char blank[18];	/* �O�d */
} spamrule_t;


#ifdef HAVE_BITLBEE
/* ----------------------------------------------------- */
/* bitlbee �� user					 */
/* ----------------------------------------------------- */

typedef struct
{
  time4_t btime;
  char nick[20];
  char addr[56];
  char status[10];
}   BITUSR;
#endif


#ifdef HAVE_RSS
/* ----------------------------------------------------- */
/* RSS							 */
/* ----------------------------------------------------- */

#define RSS_START	0x0001	/* RSS���ҥΪ��A */
#define	RSS_UTF8	0x0002	/* RSS�ĥ�UTF8�s�X�A�Ψ�l�s�X */
#define RSS_TXT		0x0004	/* RSS����Ķ��txt�榡�A�Ϥ���html�榡 */
#define RSS_RESTRICT	0x0008	/* ���æ� RSS ���e */
#define RSS_RESTART	0x0010	/* ���e�� RSS ���e */

typedef struct
{
  char bookmark[IDLEN + 1];
  time4_t chrono;
  int  xmode;
  int  xid;			/* reserved */
  char owner[IDLEN + 1];	/* �@��ID */
  char date[9];			/* ��� */
  char url[73];			/* ���} */
  char info[73];		/* �������� */
  char modified[64];		/* feed->modified�A�Yrss/atom�̫��s�ɶ� */
  char code_type[64];		/* �ĥνs�X�W�� */
}   RSS;
#endif

#endif				/* _STRUCT_H_ */
