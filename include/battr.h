/*-------------------------------------------------------*/
/* battr.h	( NTHU CS MapleBBS Ver 2.36 )		 */
/*-------------------------------------------------------*/
/* target : Board Attribution				 */
/* create : 95/03/29				 	 */
/* update : 95/12/15				 	 */
/*-------------------------------------------------------*/


#ifndef	_BATTR_H_
#define	_BATTR_H_


/* ----------------------------------------------------- */
/* Board Attribution : flags in BRD.battr		 */
/* ----------------------------------------------------- */


#define BRD_NOZAP	0x01	/* ���i zap */
#define BRD_NOTRAN	0x02	/* ����H */
#define BRD_NOCOUNT	0x04	/* ���p�峹�o��g�� */
#define BRD_NOSTAT	0x08	/* ���ǤJ�������D�έp */
#define BRD_NOVOTE	0x10	/* �����G�벼���G�� [record] �O */
#define BRD_ANONYMOUS	0x20	/* �ΦW�ݪO */
#define BRD_NOSCORE	0x40	/* �������ݪO */
#define BRD_PUBLIC	0x80	/* �����O */
#define BRD_NOL		0x00000100	/* ���i��� */
#define	BRD_SHOWPAL	0x00000200	/* ��ܪO�ͦW�� */
#define BRD_PREFIX	0x00000400	/* �O�_�ҰʬݪO POST_PREFIX �\��:0 �Ұ� :1 ���� */
#define BRD_NOFORWARD	0x00000800  /* �O�_�ݪO�i���  0:�i  1:�_ */
#define BRD_SHOWTURN	0x00001000	/* �O�_�ݪO�nshow����O��  0:�_  1:�n */
#define BRD_IAS		0x00002000	/* �����]�ݪO */
#define BRD_ATOM	0x00004000	/* ATOM �����ݪO */
#define BRD_BBS_DOG	0x00008000	/* �[�Jimaple BBS DOG �p�e */

/* ----------------------------------------------------- */
/* �U�غX�Ъ�����N�q					 */
/* ----------------------------------------------------- */


#define NUMBATTRS	16

#define STR_BATTR	"zTcsvA%PLGpFRIaB"			/* itoc: �s�W�X�Ъ��ɭԧO�ѤF��o�̰� */


#ifdef _ADMIN_C_
static char *battr_tbl[NUMBATTRS] =
{
  "���i Zap",			/* BRD_NOZAP */
  "����H�X�h",			/* BRD_NOTRAN */
  "���O���g��",			/* BRD_NOCOUNT */
  "�����������D�έp",		/* BRD_NOSTAT */
  "�����}�벼���G",		/* BRD_NOVOTE */
  "�ΦW�ݪO",			/* BRD_ANONYMOUS */
  "�������ݪO",			/* BRD_NOSCORE */
  "�����O",			/* BRD_PUBLIC */
  "���i���",		/* BRD_NOL */
  "��ܪO�ͦW��",	/* BRD_SHOWPAL */
  "����POST_PREFIX", /* BRD_PREFIX */
  "�ݪO�T��",		/* BRD_NOFORWARD */
  "����O��",       /* BRD_SHOWTURN */
  "�����]�ݪO",		/* BRD_IAS */
  "ATOM�����ݪO",	/* BRD_ATOM */
  "�[�JBBS DOG �p�e"	/* BRD_BBS_DOG */
};

#endif

#endif				/* _BATTR_H_ */
