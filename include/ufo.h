/*-------------------------------------------------------*/
/* ufo.h	( NTHU CS MapleBBS Ver 2.36 )		 */
/*-------------------------------------------------------*/
/* target : User Flag Option				 */
/* create : 95/03/29				 	 */
/* update : 95/12/15				 	 */
/*-------------------------------------------------------*/


#ifndef	_UFO_H_
#define	_UFO_H_


/* ----------------------------------------------------- */
/* User Flag Option : flags in ACCT.ufo			 */
/* ----------------------------------------------------- */


#define BFLAG(n)	(1 << n)	/* 32 bit-wise flag */


#define UFO_NOUSE00	BFLAG(0)	/* �S�Ψ� */
#define UFO_MOVIE	BFLAG(1)	/* �ʺA�ݪO��� */
#define UFO_BRDPOST	BFLAG(2)	/* 1: �ݪO�C����ܽg��  0: �ݪO�C����ܸ��X itoc.010912: ���s�峹�Ҧ� */
#define UFO_BRDNAME	BFLAG(3)	/* itoc.010413: �ݪO�C��� 1:brdname 0:class+title �Ƨ� */
#define UFO_BRDNOTE	BFLAG(4)	/* ��ܶi�O�e�� */
#define UFO_VEDIT	BFLAG(5)	/* ²�ƽs�边 */
//#define UFO_MOTD	BFLAG(6)	/* ²�ƶi/�����e�� */
#define UFO_NOUSE6	BFLAG(6)

#define UFO_PAGER	BFLAG(7)	/* �����I�s�� */
#define UFO_RCVER	BFLAG(8)	/* itoc.010716: �ڦ��s�� */
#define UFO_QUIET	BFLAG(9)	/* ���f�b�H�ҡA�ӵL������ */
#define UFO_PAL		BFLAG(10)	/* �ϥΪ̦W��u��ܦn�� */
#define UFO_ALOHA	BFLAG(11)	/* �����W���q�� */
#define UFO_NOALOHA	BFLAG(12)	/* itoc.010716: �W�����q��/��M */

//#define UFO_BMWDISPLAY	BFLAG(13)	/* itoc.010315: ���y�^�U���� */
#define UFO_NOUSE13		BFLAG(13)
//#define UFO_NWLOG       BFLAG(14)	/* lkchu.990510: ���s��ܬ��� */
//#define UFO_NTLOG       BFLAG(15)	/* lkchu.990510: ���s��Ѭ��� */
#define UFO_NOUSE14		BFLAG(14)
#define UFO_NOUSE15		BFLAG(15)

#define UFO_NOSIGN	BFLAG(16)	/* itoc.000320: ���ϥ�ñ�W�� */
#define UFO_SHOWSIGN	BFLAG(17)	/* itoc.000319: �s�ɫe���ñ�W�� */

#define UFO_ZHC		BFLAG(18)	/* hightman.060504: �����r���� */
#define UFO_JUMPBRD	BFLAG(19)	/* itoc.020122: �۰ʸ��h�U�@�ӥ�Ū�ݪO */
//#define UFO_TIMEKICKER	BFLAG(20)	/* smiler.070724: TIME_KICKER */
#define UFO_NOUSE20	BFLAG(20)
#define UFO_LIGHTBAR  BFLAG(21)       /* ������ */
#define UFO_NOUSE22	BFLAG(22)
#define UFO_NOUSE23	BFLAG(23)

#define UFO_CLOAK	BFLAG(24)	/* 1: �i�J���� */
#define UFO_SUPERCLOAK	BFLAG(25)	/* 1: �W������ */
#define UFO_ACL		BFLAG(26)	/* 1: �ϥ� ACL */
#define UFO_NOUSE27	BFLAG(27)
#define UFO_NOUSE28	BFLAG(28)
#define UFO_NOUSE29	BFLAG(29)
#define UFO_NOUSE30	BFLAG(30)
#define UFO_NOUSE31	BFLAG(31)

/* --------------------------------------------------------------------------- */

#define UFO2_LOGOUTY	BFLAG(0)	/* bossliaw.081019: LEXEL-�w�]���u */
#define UFO2_CMODE	BFLAG(1)	/* bossliaw.081019: LEXEL-�ۭq/���ðʺA */
#define UFO2_CFROM	BFLAG(2)	/* bossliaw.081019: LEXEL-�ۭq/���ìG�m */

/* �s���U�b���Bguest ���w�] ufo */

#define UFO_DEFAULT_NEW		(UFO_BRDNOTE | UFO_MOVIE | UFO_LIGHTBAR)
#define UFO_DEFAULT_GUEST	(UFO_MOVIE | UFO_BRDNOTE | UFO_QUIET | UFO_NOALOHA | UFO_NOSIGN | UFO_LIGHTBAR)

#define UFO2_DEFAULT_NEW	0
#define UFO2_DEFAULT_GUEST	0


/* ----------------------------------------------------- */
/* Status : flags in UTMP.status			 */
/* ----------------------------------------------------- */


#define STATUS_BIFF	BFLAG(0)	/* ���s�H�� */
#define STATUS_REJECT	BFLAG(1)	/* true if reject any body */
#define STATUS_BIRTHDAY	BFLAG(2)	/* ���ѥͤ� */
#define STATUS_COINLOCK	BFLAG(3)	/* ������w */
#define STATUS_DATALOCK	BFLAG(4)	/* �����w */
#define STATUS_MQUOTA	BFLAG(5)	/* �H�c�����L�����H�� */
#define STATUS_MAILOVER	BFLAG(6)	/* �H�c�L�h�H�� */
#define STATUS_MGEMOVER	BFLAG(7)	/* �ӤH��ذϹL�h */
#define STATUS_EDITHELP	BFLAG(8)	/* �b edit �ɶi�J help */
#define STATUS_PALDIRTY BFLAG(9)	/* ���H�b�L���B�ͦW��s�W�β����F�� */


#define	HAS_STATUS(x)	(cutmp->status&(x))

/* ----------------------------------------------------- */
/* User Show Habits					 */
/* ----------------------------------------------------- */


#define USR_SHOW_POST_ATTR_RESTRICT_F	BFLAG(0)	/* �n�ͤ���� ��� F */
#define USR_SHOW_POST_ATTR_RESTRICT	BFLAG(1)	/* ��� ��� L */
#define USR_SHOW_POST_ATTR_GEM_MARKED	BFLAG(2)	/* mark + gem ��� B */
#define USR_SHOW_POST_ATTR_GEM		BFLAG(3)	/* gem ��� G */
#define USR_SHOW_POST_ATTR_DELETE	BFLAG(4)	/* �ݧR�峹��� T */
#define USR_SHOW_POST_ATTR_NOFORWARD	BFLAG(5)	/* �峹�T��Ÿ���� X */
#define USR_SHOW_POST_ATTR_NOSCORE	BFLAG(6)	/* �峹�T�������� N */
#define USR_SHOW_POST_ATTR_MARKED	BFLAG(7)	/* �峹�аO�Ÿ� M */
#define USR_SHOW_POST_SCORE_0		BFLAG(8)	/* �峹���嬰 0 ��� */
#define USR_SHOW_POST_SCORE		BFLAG(9)	/* �峹�������� */
#define USR_SHOW_POST_MODIFY_UNREAD	BFLAG(10)	/* �峹�פ�/���奼Ū���� */
#define USR_SHOW_MF_FOLDER_UNREAD	BFLAG(11)	/* �̷R���v��Ū��� */
#define USR_SHOW_MORE_IP		BFLAG(12)	/* �峹�������IP�X */

#define NUM_USR_SHOW	13


/* ----------------------------------------------------- */
/* �U�زߺD������N�q					 */
/* ----------------------------------------------------- */


/* itoc.000320: �W��حn��� NUMUFOS_* �j�p, �]�O�ѤF�� STR_UFO */

#define NUMUFOS		27
#define NUMUFOS_GUEST	5	/* guest �i�H�Ϋe 5 �� ufo */
#define NUMUFOS_USER	22	/* �@��ϥΪ� �i�H�Ϋe 22 �� ufo */

#define STR_UFO		"-mpsnefPBQFAN---SHZJ-L--CHA-----"	/* itoc: �s�W�ߺD���ɭԧO�ѤF��o�̰� */

#define NUMUFOS2	3
#define NUMUFOS2_GUEST	1	/* guest �i�H�Ϋe 1 �� ufo */
#define NUMUFOS2_USER	3	/* �@��ϥΪ� �i�H�Ϋe 3 �� ufo */

#define STR_UFO2	"YMF-----------------------------"	/* itoc: �s�W�ߺD���ɭԧO�ѤF��o�̰� */


#ifdef _ADMIN_C_

char *ufo_tbl[NUMUFOS] =
{
  "�O�d",				/* UFO_NOUSE */
  "�ʺA�ݪO        (�}��/����)",	/* UFO_MOVIE */

  "�ݪO�C�����    (�峹/�s��)",	/* UFO_BRDPOST */
  "�ݪO�C��Ƨ�    (�r��/����)",	/* UFO_BRDNAME */	/* itoc.010413: �ݪO�̷Ӧr��/�����Ƨ� */
  "�i�O�e��        (���/���L)",	/* UFO_BRDNOTE */
  "�峹�s�边      (²��/����)",	/* UFO_VEDIT */
//  "�i/�����e��     (²��/����)",	/* UFO_MOTD */
  "�O�d",

  "�I�s��          (�n��/�Ҧ�)",	/* UFO_PAGER */
#ifdef HAVE_NOBROAD
  "�s���ѽu        (�ڦ�/����)",	/* UFO_RCVER */
#else
  "�O�d",
#endif
  "��������        (�w�R/����)",	/* UFO_QUITE */

  "�ϥΪ̦W�����  (�n��/����)",	/* UFO_PAL */

#ifdef HAVE_ALOHA
  "�����W���q��    (�q��/����)",	/* UFO_ALOHA */
#else
  "�O�d",
#endif
#ifdef HAVE_NOALOHA
  "�W���e�q��/��M (���e/�q��)",	/* UFO_NOALOHA */
#else
  "�O�d",
#endif

//#ifdef BMW_DISPLAY
//  "���y�^�U����    (����/�W��)",	/* UFO_BMWDISPLAY */
//#else
  "�O�d",
//#endif
//  "���x�s���y����  (�R��/���)",	/* UFO_NWLOG */
//  "���x�s��Ѭ���  (�R��/���)",	/* UFO_NTLOG */
  "�O�d",
  "�O�d",
  "���ϥ�ñ�W��    (����/���)",	/* UFO_NOSIGN */
  "���ñ�W��      (���/����)",	/* UFO_SHOWSIGN */

#ifdef HAVE_MULTI_BYTE
  "�����r����      (����/����)",	/* UFO_ZHC */
#else
  "�O�d",
#endif

#ifdef AUTO_JUMPBRD
  "���h��Ū�ݪO    (���h/����)",	/* UFO_JUMPBRD */
#else
  "�O�d",
#endif

//  "IDLE�L�[�۰�����(���/����)",	/*  UFO_TIMEKICKER */ /* smiler.070724 */
  "�O�d",
#ifdef HAVE_LIGHTBAR
  "������        (����/���q)",	/* UFO_LIGHTBAR */
#else
  "�O�d",
#endif

  "�O�d",
  "�O�d",

  "�����N          (����/�{��)",	/* UFO_CLOAK */
#ifdef HAVE_SUPERCLOAK
  "�W�������N      (����/�{��)",	/* UFO_SUPERCLOAK */
#else
  "�O�d",
#endif

  "�����W���ӷ�    (����/���N)"	/* UFO_ACL */
};


char *ufo_tbl2[NUMUFOS2] =
{
  "�����w�][Y]",			/* UFO2_LOGOUTY */

  "�ۭq/���ðʺA",			/* UFO2_CMODE */
  "�ۭq/���ìG�m"			/* UFO2_CFROM */
};


/* ----------------------------------------------------- */
/* �U����ܪ�����N�q					 */
/* ----------------------------------------------------- */

char *usr_show_tbl[NUM_USR_SHOW] =
{
  "�n�ͤ����F|f   (���/����)",
  "�[�K�����L|l   (���/����)",
  "�аO��ؤ�B|b   (���/����)",
  "������ؤ�G|g   (���/����)",
  "�ݧR�����T|t   (���/����)",
  "�T������X|x   (���/����)",
  "�T�������N|n   (���/����)",
  "�аO�����M|m   (���/����)",
  "�����k0 ���    (���/����)",
  "����������    (���/����)",
  "���奼Ū����    (���/����)",
  "�̷R���v��Ū���(���/����)",
  "����IP�X�m�����(�m��/���)"
};


#endif

#endif				/* _UFO_H_ */
