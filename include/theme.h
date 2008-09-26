/*-------------------------------------------------------*/
/* theme.h	( NTHU CS MapleBBS Ver 3.10 )		 */
/*-------------------------------------------------------*/
/* target : custom theme				 */
/* create : 02/08/17				 	 */
/* update :   /  /  				 	 */
/*-------------------------------------------------------*/


#ifndef	_THEME_H_
#define	_THEME_H_


/* ----------------------------------------------------- */
/* ���C��w�q�A�H�Q�����ק�				 */
/* ----------------------------------------------------- */

//#define COLOR1		"\033[34;46m"	/* footer/feeter ���e�q�C�� */
#define COLOR1		"\033[m\033[1;33;44m"	/* footer/feeter ���e�q�C�� */
#define COLOR2		"\033[m\033[30;47m"	/* footer/feeter ����q�C�� */
#define COLOR3		"\033[30;47m"	
#define COLOR4		"\033[1;44m"		/* ���� ���C�� */
#define COLOR5		"\033[34;47m"		/* more ���Y�����D�C�� */
#define COLOR6		"\033[37;44m"		/* more ���Y�����e�C�� */
#define COLOR7		"\033[1m"		/* �@�̦b�u�W���C�� */
#define COLOR8		"\033[m\033[1;34;47m"	/* feeter �[�j�е�(��/��) */
#define COLOR9		"\033[m\033[31;47m"	/* feeter �[�j�е�(��/��) */
#define COLOR10		"\033[34;47m"		/* neck ���C�� */
#define COLOR11		"\033[35;47m"		/* menu feeter ���C�� */

/* ----------------------------------------------------- */
/* �ϥΪ̦W���C��					 */
/* ----------------------------------------------------- */

#define COLOR_NORMAL	""		/* �@��ϥΪ� */
#define COLOR_MYBAD	"\033[1;31m"	/* �a�H */
#define COLOR_MYGOOD	"\033[1;32m"	/* �ڪ��n�� */
#define COLOR_OGOOD	"\033[1;33m"	/* �P�ڬ��� */
#define COLOR_CLOAK	"\033[1;35m"	/* ���� */	/* itoc.����: �S�Ψ�A�n���H�Цۦ�[�J ulist_body() */
#define COLOR_SELF	"\033[1;36m"	/* �ۤv */
#define COLOR_BOTHGOOD	"\033[1;37m"	/* ���]�n�� */
#define COLOR_BRDMATE	"\033[36m"	/* �O�� */
#define COLOR_SUPER_BOTHGOOD	"\033[1;35m"	/* ���]�W�Ŧn�� */
#define COLOR_SUPER_MYGOOD	"\033[32m"	/* �ڪ��W�Ŧn�� */
#define COLOR_SUPER_OGOOD	"\033[33m"	/* �P�ڬ��W�Ŧn�� */


/* ----------------------------------------------------- */
/* ����m						 */
/* ----------------------------------------------------- */

/* itoc.����: �`�N MENU_XPOS �n >= MENU_XNOTE + MOVIE_LINES */

#define MENU_XNOTE	2		/* �ʺA�ݪO�� (2, 0) �}�l */
#define MOVIE_LINES	10		/* �ʵe�̦h�� 10 �C */

#define MENU_XPOS	13		/* ���}�l�� (x, y) �y�� */
#define MENU_YPOS	((d_cols >> 1) + 18)


/* ----------------------------------------------------- */
/* �T���r��G*_neck() �ɪ� necker ����X�өw�q�b�o	 */
/* ----------------------------------------------------- */

/* necker ����Ƴ��O�G��A�q (1, 0) �� (2, 80) */

/* �Ҧ��� XZ_* ���� necker�A�u�O���Ǧb *_neck()�A�����æb *_head() */

/* ulist_neck() �� xpost_head() ���Ĥ@�����S�O�A���b���w�q */

#define NECKER_CLASS	"[��]�D��� [��]�\\Ū [����]��� [c]�g�� [y]���J [/?]�j�M [s]�ݪO [h]����\n" \
			COLOR10 "  %s   ��  �O       ���O��H��   ��   ��   �z%*s              �H�� �O    �D%*s    \033[m"

#define NECKER_ULIST	"\n" \
			COLOR10 "  �s��  �N��         �ʺ�%*s                 %-*s               �ʺA        ���m \033[m"

#define NECKER_PAL	"[��]���} [a]�s�W [c]�ק� [d]�R�� [m]�H�H [w]���y [s]��z [��]�d�� [h]����\n" \
			COLOR10 "  �s��    �N ��         ��       ��%*s                                           \033[m"

#define NECKER_ALOHA	"[��]���} [a]�s�W [d]�R�� [D]�Ϭq�R�� [m]�H�H [w]���y [s]���� [f]�ޤJ [h]����\n" \
			COLOR10 "  �s��   �W �� �q �� �W ��%*s                                                    \033[m"

#define NECKER_VOTE	"[��]���} [R]���G [^P]�|�� [E]�ק� [V]�w�� [^Q]��� [o]�W�� [h]����\n" \
			COLOR10 "  �s��      �}����   �D��H       ��  ��  �v  ��%*s                              \033[m"

#define NECKER_BMW	"[��]���} [d]�R�� [D]�Ϭq�R�� [m]�H�H [M]�x�s [w]���y [s]��s [��]�d�� [h]����\n" \
			COLOR10 "  �s�� �N  ��       ��       �e%*s                                          �ɶ� \033[m"

#define NECKER_MF	"[��]���} [��]�i�J [^P]�s�W [d]�R�� [c]���� [C]�ƻs [^V]�K�W [m]���� [h]����\n" \
			COLOR10 "  %s   ��  �O       ���O��H��   ��   ��   �z%*s              �H�� �O    �D%*s    \033[m"

#define NECKER_COSIGN	"[��]���} [��]�\\Ū [^P]�ӽ� [d]�R�� [o]�}�O [h]����\n" \
			COLOR10 "  �s��   �� ��  �|��H       ��  �O  ��  �D%*s                                   \033[m"

#define NECKER_SONG	"[��]���} [��]�s�� [o]�I�q��ݪO [m]�I�q��H�c [Enter]�s�� [h]����\n" \
			COLOR10 "  �s��     �D              �D%*s                            [�s      ��] [��  ��]\033[m"

#define NECKER_NEWS	"[��]���} [��]�\\Ū [h]����\n" \
			COLOR10 "  �s��     �� �� �@  ��       �s  �D  ��  �D%*s                                  \033[m"

#define NECKER_XPOST	"\n" \
			COLOR10 "  �s��     �� �� �@  ��       ��  ��  ��  �D%*s                           ��:%s  \033[m"

#define NECKER_MBOX	"[��]���} [��,r]Ū�H [d]�R�� [R,y](�s��)�^�H [s]�H�H [x]��� [X]��F [h]����\n" \
			COLOR10 "  �s��   �� �� �@  ��       �H  ��  ��  �D%*s                                    \033[m"

#define NECKER_POST	"[��]���} [��]�\\Ū [^P]�o�� [b]�i�O�e�� [d]�R�� [V]�벼 [TAB]��ذ� [h]����\n" \
			COLOR10 "  �s��     �� �� �@  ��       ��  ��  ��  �D%*s                 ��:%s  �H��:%-4d \033[m"

#define NECKER_POST_FILE	"[��]���} [��]�\\Ū [^P]�o�� [b]�i�O�e�� [d]�R�� [V]�벼 [TAB]��ذ� [h]����\n" \
			COLOR10 "  �ɦW(f)  �� �� �@  ��       ��  ��  ��  �D%*s                 ��:%s  �H��:%-4d \033[m"

#define NECKER_GEM	"[��]���} [��]�s�� [B]�Ҧ� [C]�Ȧs [F]��H [d]�R�� [h]����  %s\n" \
			COLOR10 "  �s��     �D              �D%*s                            [�s      ��] [��  ��]\033[m"

/* �H�U�o�ǫh�O�@���� XZ_* ���c�� necker */

#define NECKER_VOTEALL	"[��/��]�W�U [PgUp/PgDn]�W�U�� [Home/End]���� [��]�벼 [��][q]���}\n" \
			COLOR10 "  �s��   ��  �O       ���O��H��   ��   ��   �z%*s                  �O    �D%*s     \033[m"

#define NECKER_CREDIT	"[��]���} [C]���� [1]�s�W [2]�R�� [3]���R [4]�`�p\n" \
			COLOR10 "  �s��   ��  ��   ����  ��  �B  ����     ��  ��%*s                               \033[m"

#define NECKER_HELP	"[��]���} [��]�\\Ū [^P]�s�W [d]�R�� [T]���D [E]�s�� [m]����\n" \
			COLOR10 "  �s��    �� ��         ��       �D%*s                                           \033[m"

#define NECKER_INNBBS	"[��]���} [^P]�s�W [d]�R�� [E]�s�� [/]�j�M [Enter]�Բ�\n" \
			COLOR10 "  �s��            ��         �e%*s                                               \033[m"

#define NECKER_RSS	"[��]���} [��]�s�� [o]���� [u]UTF8�s�X [n]Html/Txt [s]�Ұ�/�Ȱ� [R]���e        \n" \
			COLOR10 "  �s��  �� ��%*s ��  ñ       �� ��    ���}                                      \033[m"

/* ----------------------------------------------------- */
/* �T���r��Gmore() �ɪ� footer ����X�өw�q�b�o	 */
/* ----------------------------------------------------- */

/* itoc.010914.����: ��@�g�A�ҥH�s FOOTER�A���O 78 char */

/* itoc.010821: �`�N \\ �O \�A�̫�O�|�F�@�Ӫť��� :p */

#define FOOTER_POST	\
COLOR1 " �峹��Ū " COLOR9 " (ry)"COLOR2"�^�� "COLOR9"(=\\[]<>-+;'`)"COLOR2"�D�D "COLOR9"(|?QA)"COLOR2"�j�M���D�@�� "COLOR9"(kj)"COLOR2"�W�U�g "COLOR9"(C)"COLOR2"�Ȧs   "

#define FOOTER_MAILER	\
COLOR1 " �������� " COLOR9 " (ry)"COLOR2"�^�H/�s�� "COLOR9"(X)"COLOR2"��F "COLOR9"(d)"COLOR2"�R�� "COLOR9"(m)"COLOR2"�аO "COLOR9"(C)"COLOR2"�Ȧs "COLOR9"(=\\[]<>-+;'`|?QAkj)"COLOR2"  "

#define FOOTER_GEM	\
COLOR1 " ��ؿ�Ū " COLOR9 " (=\\[]<>-+;'`)"COLOR2"�D�D "COLOR9"(|?QA)"COLOR2"�j�M���D�@�� "COLOR9"(kj)"COLOR2"�W�U�g "COLOR9"(������)"COLOR2"�W�U���}   "

#ifdef HAVE_GAME
#define FOOTER_TALK	\
COLOR1 " ��ͼҦ� " COLOR9 " (^O)"COLOR2"�﫳�Ҧ� "COLOR9"(^C,^D)"COLOR2"������� "COLOR9"(^T)"COLOR2"�����I�s�� "COLOR9"(^Z)"COLOR2"�ֱ��C�� "COLOR9"(^G)"COLOR2"�͹�  "
#else
#define FOOTER_TALK	\
COLOR1 " ��ͼҦ� " COLOR9 " (^C,^D)"COLOR2"������� "COLOR9"(^T)"COLOR2"�����I�s�� "COLOR9"(^Z)"COLOR2"�ֱ��C�� "COLOR9"(^G)"COLOR2"�͹� "COLOR9"(^Y)"COLOR2"�M��      "
#endif

#define FOOTER_COSIGN	\
COLOR1 " �s�p���� " COLOR9 " (ry)"COLOR2"�[�J�s�p "COLOR9"(kj)"COLOR2"�W�U�g "COLOR9"(������)"COLOR2"�W�U���} "COLOR9"(h)"COLOR2"����                   " 

//#define FOOTER_MORE	\
//COLOR1 " �s�� P.%d (%d%%) " COLOR2 " (h)���� [PgUp][PgDn][0][$]���� (/n)�j�M (C)�Ȧs (��q)���� "

#define FOOTER_MORE	\
COLOR1 " �s�� P.%d (%d%%) " COLOR9 " (h)"COLOR2"���� " COLOR9 "(@)"COLOR2"�ʵe����" COLOR2 " "COLOR9"(����0$)"COLOR2"���� "COLOR9"(/n)"COLOR2"�j�M "COLOR9"(C)"COLOR2"�Ȧs "COLOR9"(��q)"COLOR2"����  "

#define FOOTER_VEDIT	\
COLOR1 " %s " COLOR9 " (^Z)"COLOR2"���� "COLOR9"(^W)"COLOR2"�Ÿ� "COLOR9"(^L)"COLOR2"��ø "COLOR9"(^X)"COLOR2"�ɮ׳B�z ��"COLOR9"%s"COLOR2"�x"COLOR9"%s"COLOR2"��"COLOR9"%5d:%3d"COLOR2"    \033[m"


/* ----------------------------------------------------- */
/* �T���r��Gxo_foot() �ɪ� feeter ����X�өw�q�b�o      */
/* ----------------------------------------------------- */


/* itoc.010914.����: �C��h�g�A�ҥH�s FEETER�A���O 78 char */

#define FEETER_CLASS	\
COLOR1 " �ݪO��� " COLOR9 " (c)"COLOR2"�s�峹 "COLOR9"(vV)"COLOR2"�аO�wŪ��Ū "COLOR9"(y)"COLOR2"�����C�X "COLOR9"(z)"COLOR2"��q "COLOR9"(A)"COLOR2"����j�M "COLOR9"(S)"COLOR2"�Ƨ� "

#define FEETER_ULIST	\
COLOR1 " ���ͦC�� " COLOR9 " (f)"COLOR2"�n�� "COLOR9"(w/t/m)"COLOR2"���y/���/�H�H "COLOR9"(q)"COLOR2"�d�� "COLOR9"(ad)"COLOR2"��� "COLOR9"(s)"COLOR2"��s "COLOR9"(TAB)"COLOR2"����   "

#define FEETER_PAL	\
COLOR1 " �I�B�ަ� " COLOR9 " (a/d/m)"COLOR2"�s�W/�R��/�H�H "COLOR9"(c)"COLOR2"�ͽ� "COLOR9"(f)"COLOR2"�ޤJ�n�� "COLOR9"(r^Q)"COLOR2"�d�� "COLOR9"(s)"COLOR2"��s        "

#define FEETER_ALOHA	\
COLOR1 " �W���q�� " COLOR9 " (a)"COLOR2"�s�W "COLOR9"(d)"COLOR2"�R�� "COLOR9"(D)"COLOR2"�Ϭq�R�� "COLOR9"(f)"COLOR2"�ޤJ�n�� "COLOR9"(r^Q)"COLOR2"�d�� "COLOR9"(s)"COLOR2"��s          "

#define FEETER_VOTE	\
COLOR1 " �ݪO�벼 " COLOR9 " (��/r/v)"COLOR2"�벼 "COLOR9"(R)"COLOR2"���G "COLOR9"(^P)"COLOR2"�s�W�벼 "COLOR9"(E)"COLOR2"�ק� "COLOR9"(V/b/o)"COLOR2"�w��/�}��/�W��    "

#define FEETER_BMW	\
COLOR1 " ���y�^�U " COLOR9 " (d/D)"COLOR2"�R��/�Ϭq�R�� "COLOR9"(m)"COLOR2"�H�H "COLOR9"(w)"COLOR2"���y "COLOR9"(^R)"COLOR2"�^�T "COLOR9"(^Q)"COLOR2"�d�� "COLOR9"(s)"COLOR2"��s       "

#define FEETER_MF	\
COLOR1 " �̷R�ݪO " COLOR9 " (^P)"COLOR2"�s�W "COLOR9"(Cg)"COLOR2"�ƻs "COLOR9"(p^V)"COLOR2"�K�W "COLOR9"(d)"COLOR2"�R�� "COLOR9"(c)"COLOR2"�s�峹 "COLOR9"(vV)"COLOR2"�аO�wŪ/��Ū    "

#define FEETER_COSIGN	\
COLOR1 " �s�p�p�� " COLOR9 " (r/y)"COLOR2"Ū��/�^�� "COLOR9"(^P)"COLOR2"�o�� "COLOR9"(d)"COLOR2"�R�� "COLOR9"(o)"COLOR2"�}�O "COLOR9"(c)"COLOR2"���� "COLOR9"(E/B)"COLOR2"�s��/�]�w     "

#define FEETER_SONG	\
COLOR1 " �I�q�t�� " COLOR9 " (r)"COLOR2"Ū�� "COLOR9"(o)"COLOR2"�I�q��ݪO "COLOR9"(m)"COLOR2"�I�q��H�c "COLOR9"(E)"COLOR2"�s���ɮ� "COLOR9"(T)"COLOR2"�s����D        "

#define FEETER_NEWS	\
COLOR1 " �s�D�I�� " COLOR9 " (��/��)"COLOR2"�W�U "COLOR9"(PgUp/PgDn)"COLOR2"�W�U�� "COLOR9"(Home/End)"COLOR2"���� "COLOR9"(��r)"COLOR2"��� "COLOR9"(��)(q)"COLOR2"���} "

#define FEETER_XPOST	\
COLOR1 " ��C�j�M " COLOR9 " (y/x)"COLOR2"�^��/���  "COLOR9"(m/t)"COLOR2"�аO/���� "COLOR9"(d)"COLOR2"�R�� "COLOR9"(^P)"COLOR2"�o�� "COLOR9"(^Q)"COLOR2"�d�ߧ@��       "

#define FEETER_MBOX	\
COLOR1 " �H�H�۱� " COLOR9 " (y)"COLOR2"�^�H "COLOR9"(F/X/x)"COLOR2"��H/��F/��� "COLOR9"(d)"COLOR2"�R�� "COLOR9"(D)"COLOR2"�Ϭq�R�� "COLOR9"(m)"COLOR2"�аO "COLOR9"(E)"COLOR2"�s��  "

#define FEETER_POST	\
COLOR1 " �峹�C�� \033[m" COLOR9 " (f)"COLOR8"�s��/�ɦW����\033[m" COLOR9 " (ry)"COLOR2"�^�H " COLOR9 "(/)"COLOR8"�j�M�\\���X��\033[m" COLOR9 " (x/V/u)"COLOR2"���/�벼/�s�D  "

#define FEETER_GEM	\
COLOR1 " �ݪO��� " COLOR9 " (^P/a/f)"COLOR2"�s�W/�峹/�ؿ� "COLOR9"(E)"COLOR2"�s�� "COLOR9"(T)"COLOR2"���D "COLOR9"(m)"COLOR2"���� "COLOR9"(c)"COLOR2"�ƻs "COLOR9"(p^V)"COLOR2"�K�W   "

#define FEETER_VOTEALL	\
COLOR1 " �벼���� " COLOR9 " (��/��)"COLOR2"�W�U "COLOR9"(PgUp/PgDn)"COLOR2"�W�U�� "COLOR9"(Home/End)"COLOR2"���� "COLOR9"(��)"COLOR2"�벼 "COLOR9"(��)(q)"COLOR2"���}  "

#define FEETER_HELP	\
COLOR1 " ������� " COLOR9 " (��/��)"COLOR2"�W�U "COLOR9"(PgUp/PgDn)"COLOR2"�W�U�� "COLOR9"(Home/End)"COLOR2"���� "COLOR9"(��r)"COLOR2"�s�� "COLOR9"(��)(q)"COLOR2"���} "

#define FEETER_INNBBS	\
COLOR1 " ��H�]�w " COLOR9 " (��/��)"COLOR2"�W�U "COLOR9"(PgUp/PgDn)"COLOR2"�W�U�� "COLOR9"(Home/End)"COLOR2"���� "COLOR9"(��)(q)"COLOR2"���}           "

#define FEETER_BITLBEE \
COLOR1 "  �Y�ɳq  "COLOR9" (w)"COLOR2"�ǰe�T�� " COLOR9 "(^r)"COLOR2"�^�аT�� " COLOR9 "(a/d)"COLOR2"�W/�R�p���H " COLOR9 "(b/B)"COLOR2"����/�Ѱ� "COLOR9"(^k)"COLOR2"�_�u  " 

#define FEETER_FAKE_PAL	\
COLOR1 " �s���W�� " COLOR9 " ���W��Ȩ��s���A�O�D�ק�O�ͦW��аh�X���(B)��ܭק�              "

#define FEETER_RSS	\
COLOR1 " RSS �]�w " COLOR9 " (^P/a)"COLOR2"�s�W "COLOR9"(d)"COLOR2"�R�� "COLOR9"(E)"COLOR2"�s�� "COLOR9"(T)"COLOR2"��ñ�W�� "COLOR9"(H)"COLOR2"RSS���} "COLOR9"(m)"COLOR2"���� "COLOR9"(h)"COLOR2"����  "

/* ----------------------------------------------------- */
/* ���x�ӷ�ñ�W						 */
/* ----------------------------------------------------- */

/* itoc: ��ĳ banner ���n�W�L�T��A�L������ñ�i��|�y���Y�ǨϥΪ̪��ϷP */

/*dexter  ��origin,From�令���*/
#define EDIT_BANNER	"\n--\n" \
			"\033[;32m�� Origin: �����毸<bbs.cs.nthu.edu.tw>\n\033[;32m�� From: %s @%s\033[m\n"

//#define MODIFY_BANNER	"\033[m\033[1;45m(^_^)�r\033[m \033[1mMo\033[30mdi\033[mfy: %s @%s �� \033[1;34m%s\033[m �ק�\n"
#define MODIFY_BANNER	"\033[;36m%s �� %s �q %s �ק�\033[m\n"


/* ----------------------------------------------------- */
/* ��L�T���r��						 */
/* ----------------------------------------------------- */

#define VMSG_NULL	"                           \033[1;33;46m �� �Ы����N���~�� �� \033[m"

#define ICON_UNREAD_BRD		"\033[1;33m��\033[m"	/* ��Ū�ݪO */
#define ICON_READ_BRD		"  "			/* �wŪ�ݪO */

#define ICON_GAMBLED_BRD	"\033[1;31m��\033[m"	/* �|���L�����ݪO */
#define ICON_VOTED_BRD		"\033[1;33m��\033[m"	/* �|��벼�����ݪO */
#define ICON_NOTRAN_BRD		"��"			/* ����H�O */
#define ICON_TRAN_BRD		"��"			/* ��H�O */

#define TOKEN_ZAP_BRD		'-'			/* zap �O */
#define TOKEN_FRIEND_BRD	'.'			/* �n�ͪO */
#define TOKEN_SECRET_BRD	')'			/* ���K�O */

#endif				/* _THEME_H_ */
