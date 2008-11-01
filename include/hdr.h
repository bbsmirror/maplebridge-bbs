/*-------------------------------------------------------*/
/* lib/hdr.h            ( NTHU CS MapleBBS Ver 3.00 )    */
/*-------------------------------------------------------*/
/* target : header file for HDR routines                 */
/* create : 96/11/20                                     */
/* update : 96/12/15                                     */
/*-------------------------------------------------------*/


#ifndef	_HDR_H_
#define	_HDR_H_


#include <sys/types.h>


#include "time4.h"


/* ----------------------------------------------------- */
/* DIR of post / mail struct : 256 bytes		 */
/* ----------------------------------------------------- */


typedef struct
{
  time4_t chrono;		/* timestamp */
  int xmode;

  int xid;			/* reserved */

  char xname[28];               /* �ɮצW�� */
  time4_t parent_chrono;         /* �m���媺�����O���g */
  char owner[76];               /* �@�� (E-mail address) */
  time4_t stamp;                 /* �̪�@������ɮת��ɶ� */
  char nick[40];		/* �ʺ� */
  char value[9];		/* �峹����/�ΦW�峹userno */
  char score;			/* �峹������� */

  char date[9];			/* 96/12/31 */
  /* Thor.990329:�S�O�`�N, date�u����ܥ�, ���@���, �H�קK y2k ���D,
                 �w�q 2000�� 00, 2001��01 */

  char title[73];		/* �D�D TTLEN + 1 */
}          HDR;


/* gopher url �r��Gxname + owner + nick + date */

#define	GEM_URLEN	(32 + 80 + 50 + 9 - 1)


/* ----------------------------------------------------- */
/* post.xmode ���w�q					 */
/* ----------------------------------------------------- */


#define POST_READ	0x00000001	/* already read */
#define POST_MARKED	0x00000002	/* marked */
#define POST_NOFORWARD	0x00000004	/* ������H */
#define POST_NOSCORE	0x00000008	/* ������� */
#define POST_GOOD	0x00000010	/* �u�� */
#define POST_GEM	0x00000020	/* ������ذϼаO */
#define POST_BOTTOM	0x00000040	/* �m�� */
#define POST_DELETE	0x00000080	/* �аO�ݬ媺 */
#define	POST_INCOME	0x00000100	/* ��H�i�Ӫ� */
#define	POST_FRIEND	0x00000200	/* �n�ͤ峹 */
#define	POST_OUTGO	0x00000400	/* ����H�X�h */
#define	POST_RESTRICT	0x00000800	/* ����Ť峹�A�� manager/owner �~��� */
#define POST_RESERVED	0x00001000	/* ����Ť峹�A�� sysop �~���� */
#define POST_14		0x00002000
#define POST_SCORE	0x00004000	/* �аO�����L�� */
#define POST_16		0x00008000


/* ----------------------------------------------------- */
/* mail.xmode ���w�q					 */
/* ----------------------------------------------------- */


#define MAIL_READ	0x00000001	/* already read */
#define MAIL_MARKED	0x00000002	/* marked */
#define MAIL_REPLIED	0x00000004	/* �w�g�^�L�H�F */
#define	MAIL_MULTI	0x00000008	/* mail list */
#define	MAIL_5		0x00000010
#define	MAIL_NOREPLY	0x00000020	/* ���i�^�H */
#define MAIL_BOTTOM	0x00000040	/* �m�� */
#define MAIL_DELETE	0x00000080	/* �N�D�R�� */
#define	MAIL_INCOME	0x00000100	/* bbsmail �i�Ӫ� */
#define MAIL_10		0x00000200
#define MAIL_11		0x00000400
#define	MAIL_RESTRICT	0x00000800	/* ����ūH��A�� manager/owner �~��� */
#define MAIL_RESERVED	0x00001000	/* ����ūH��A�� sysop �~���� */
#define MAIL_14		0x00002000
#define MAIL_15		0x00004000
#define MAIL_16		0x00008000


/* ----------------------------------------------------- */
/* gem(gopher).xmode ���w�q				 */
/* ----------------------------------------------------- */


/* itoc.010602.����: GEM_RESTRICT/RESERVED �M POST_RESTRICT/RESERVED �n�ǰt */
#define	GEM_RESTRICT	0x00000800	/* ����ź�ذϡA�� manager/owner �~��� */
#define	GEM_RESERVED	0x00001000	/* ����ź�ذϡA�� sysop �~���� */
#define GEM_FULLPATH    0x00008000      //lexel �� itoc �S��/* ������| */
#define	GEM_FOLDER	0x00010000	/* folder / article */
#define	GEM_BOARD	0x00020000	/* �ݪO��ذ� */
#define GEM_19		0x00040000
#define GEM_LINE	0x00080000	/* ���j�u */
#define GEM_GOPHER      0x10000000	//lexel �� itoc �S��/* gopher */
#define GEM_HTTP        0x20000000	//lexel �� itoc �S��/* http */
#define GEM_EXTEND      0x80000000	//lexel �� itoc �S��/* ������ URL */
#define HDR_URL		(GEM_GOPHER | GEM_HTTP)	//lexel �� itoc �S��



/* ----------------------------------------------------- */
/* hdr_stamp() ���w�q					 */
/* ----------------------------------------------------- */


#define	HDR_LINK	0x400		/* link() */
#define HDR_COPY	0x800		/* copy */

#endif	/* _HDR_H_ */
