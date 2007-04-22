/*-------------------------------------------------------*/
/* util/snap.h						 */
/*-------------------------------------------------------*/
/* target : Maple �ഫ					 */
/* create : 98/12/15					 */
/* update : 02/04/29					 */
/* author : itoc.bbs@bbs.tnfsh.tn.edu.tw		 */
/*-------------------------------------------------------*/


#if 0

  0. �O�d��� brd/ gem/ usr/ .USR�A��l�����s����

  1. �Q�� snap2brd �ഫ .BRD

  2. �Q�� snap2usr �ഫ .ACCT

  3. �N�s���� gem/@/ �U���o���ɮ׽ƻs�L��
     @apply @e-mail @goodbye @index @justify @newuser @opening.0
     @opening.1 @opening.2 @post @re-reg @tryout @welcome

  4. �W BBS ���A�b (A)nnounce �̭��A�إH�U�G�Ө��v���Ҧ����
     {���D} �����Q��
     {�Ʀ�} �έp���

#endif


#include "bbs.h"

#define	MAK_DIRS	/* �إؿ� MF/ �� gem/ */


/* ----------------------------------------------------- */
/* (�ª�) �ϥΪ̱b�� .ACCT struct			 */
/* ----------------------------------------------------- */


typedef struct			/* �n�M�ª��{�� struct �@�� */
{
  int userno;			/* unique positive code */
  char userid[13];
  char passwd[14];
  uschar signature;
  char realname[20];
  char username[24];
  usint userlevel;
  int numlogins;
  int numposts;
  usint ufo;
  time_t firstlogin;
  time_t lastlogin;
  time_t staytime;		/* �`�@���d�ɶ� */
  time_t tcheck;		/* time to check mbox/pal */
  char lasthost[32];
  int numemail;			/* �H�o Inetrnet E-mail ���� */
  time_t tvalid;		/* �q�L�{�ҡB��� mail address ���ɶ� */
  char email[60];
  char address[60];
  char justify[60];		/* FROM of replied justify mail */
  char vmail[60];		/* �q�L�{�Ҥ� email */
  char ident[140 -20];
/*  char cmode[20];                �ۭq�ʺA */
/*  char cfrom[34];                �ۭq�ӷ� */
  time_t vtime;			/* validate time */
/*  int money;                     ���� 
  char pmail[60];                �w�]�H�H�H�c 
  char rmail[60];                �w�]��O�H�c 
  char pad[388];*/
}	userec;


/* ----------------------------------------------------- */
/* PAL : friend struct : 64 bytes                        */
/* ----------------------------------------------------- */


typedef struct
{ 
    char userid[IDLEN + 1];
    char ftype;
    char ship[46];
    int userno;
}      MAPLE_PAL;

#define PAL_BAD         0x02

/* ----------------------------------------------------- */
/* (�ª�) �ϥΪ̲ߺD ufo				 */
/* ----------------------------------------------------- */

/* old UFO */			/* �n�M�ª��{�� struct �@�� */

#define HABIT_COLOR       BFLAG(0)        /* true if the ANSI color mode open */
#define HABIT_MOVIE       BFLAG(1)        /* true if show movie */
#define HABIT_BRDNEW      BFLAG(2)        /* �s�峹�Ҧ� */
#define HABIT_BNOTE       BFLAG(3)        /* ��ܶi�O�e�� */
#define HABIT_VEDIT       BFLAG(4)        /* ²�ƽs�边 */
#define HABIT_PAGER       BFLAG(5)        /* �����I�s�� */
#define HABIT_QUIET       BFLAG(6)        /* ���f�b�H�ҡA�ӵL������ */
#define HABIT_PAL         BFLAG(7)        /* true if show pals only */
#define HABIT_ALOHA       BFLAG(8)        /* �W���ɥD�ʳq���n�� */
#define HABIT_MOTD        BFLAG(9)        /* ²�ƶi���e�� */
#define HABIT_CLOAK       BFLAG(19)       /* true if cloak was ON */
#define HABIT_ACL         BFLAG(20)       /* true if ACL was ON */
/*#define HABIT_MPAGER      BFLAG(10)       /* lkchu.990428: �q�l�l��ǩI */
/*#define HABIT_NWLOG       BFLAG(11)       /* lkchu.990510: ���s��ܬ��� */
/*#define HABIT_NTLOG       BFLAG(12)       /* lkchu.990510: ���s��Ѭ��� */


/* ----------------------------------------------------- */
/* (�ª�) BOARDS struct					 */
/* ----------------------------------------------------- */

typedef struct//Maple BBS
{
  char brdname[12 + 1];      /* board ID */
  char title[47 + 1];
  char bottom;
  char BM[36 + 1];
  char bclass[5];               /* �ݪO���� */
  /*char bholiday[5];             �ݪO�� */
  /*char BM[36 + 1];           BMs' uid, token '/' */
          
  uschar bvote;                 /* �@���X���벼�|�椤 */
             
  time_t bstamp;                /* �إ߬ݪO���ɶ�, unique */
  usint readlevel;              /* �\Ū�峹���v�� */
  usint postlevel;              /* �o��峹���v�� */
  usint battr;                  /* �ݪO�ݩ� */
  time_t btime;                 /* .DIR �� st_mtime */
  int bpost;                    /* �@���X�g post */
  time_t blast;                 /* �̫�@�g post ���ɶ� */
  char pad[120];
} 	boardheader;

/* ----------------------------------------------------- */
/* (�ª�) HDR struct					 */
/* ----------------------------------------------------- */
typedef struct
{
  time_t chrono;		/* timestamp */
  int xmode;

  int xid;			/* reserved */

  char xname[28];		/* �ɮצW�� */
  
  time_t ochrono;		/* �̭�l�� timestamp */
  
  char owner[80];		/* �@�� (E-mail address) */
  char nick[50];		/* �ʺ� */

  char date[9];			/* [96/12/01] */
  /* Thor.990329:�S�O�`�N, date�u����ܥ�, ���@���, �H�קK y2k ���D,
                 �w�q 2000�� 00, 2001��01 */

  char title[73];		/* �D�D (TTLEN + 1) */

  int score;
  
}          LEXEL_HDR;

typedef struct
{
  time_t chrono;		/* timestamp */
  int xmode;

  int xid;			/* reserved */

  char xname[32];		/* �ɮצW�� */
  char owner[80];		/* �@�� (E-mail address) */
  char nick[50];		/* �ʺ� */

  char date[9];			/* [96/12/01] */
  /* Thor.990329:�S�O�`�N, date�u����ܥ�, ���@���, �H�קK y2k ���D,
                 �w�q 2000�� 00, 2001��01 */

  char title[73];		/* �D�D (TTLEN + 1) */
}         MAPLECS_HDR;


typedef struct
{
  time_t chrono;                /* timestamp */
  char buf1[4];
  int xmode;
                                                                                
  int xid;                      /* reserved */
                                                                                
  char xname[32];
  char owner[76];
  time_t stamp;
  char nick[49];
  char score;
                                                                                
  char date[9];                 /* 96/12/31 */
                                                                                
  char title[73];
  char buf2[4];
}     WRETCH_HDR;


/* ----------------------------------------------------- */
/* (�ª�) GEM struct                                        */
/* ----------------------------------------------------- */
typedef struct
{
  time_t chrono;		/* timestamp */
  int xmode;
  int xid;			/* reserved */
  char xname[28];		/* �ɮצW�� */
  time_t ochrono;		/* �̭�l�� timestamp */
  char owner[80];		/* �@�� (E-mail address) */
  char nick[50];		/* �ʺ� */
  char date[9];			/* [96/12/01] */
  /* Thor.990329:�S�O�`�N, date�u����ܥ�, ���@���, �H�קK y2k ���D,
     �w�q 2000�� 00, 2001��01 */
  char title[73];		/* �D�D (TTLEN + 1) */
  int score;
  }          LEXEL_GEM;



/*----------�ª�MF----------*/
typedef struct
{
  time_t chrono;/*�إ߮ɶ�*/
  usint mftype;/*type*/
  int bno;/*board #*/
/*  char title[38+1];/*���D*/  
  char xname[13];/*�ɦW*/
  char title[38+1];/*���D*/
} MyF;
#define OLDMF_LINE	0x00000001/*���j�u*/
#define OLDMF_BOARD	0x00000002/*�ݪO���|*/
#define OLDMF_FOLDER	0x00000004/*���v*/
#define OLDMF_GEM	0x00000008/*��ذϱ��|*/
#define OLDMF_DELETED	0x10000000/*�Q�������s��*/

