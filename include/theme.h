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
#define COLOR1		"\033[;1;33;44m"	/* footer/feeter ���e�q�C�� */
#define COLOR2		"\033[;30;47m"		/* footer/feeter ����q�C�� */
#define COLOR3		"\033[;30;47m"	
#define COLOR4		"\033[1;44m"		/* ���� ���C�� */
#define COLOR5		"\033[34;47m"		/* more ���Y�����D�C�� */
#define COLOR6		"\033[37;44m"		/* more ���Y�����e�C�� */
#define COLOR7		"\033[1m"		/* �@�̦b�u�W���C�� */
#define COLOR8		"\033[;1;34;47m"	/* feeter �[�j�е�(��/��) */
#define COLOR9		"\033[;31;47m"		/* feeter �[�j�е�(��/��) */
#define COLOR10		"\033[34;47m"		/* neck ���C�� */
#define COLOR11		"\033[35;47m"		/* menu feeter ���C�� */
#define COLOR_SITE	"\033[1;37;44m"		/* ryanlei.081017: ���x�D�� */


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
/* �U�������C��						 */
/* ----------------------------------------------------- */

#define COLORBAR_MENU	"\033[1;37;44m"	/* menu.c ������ */
#define	COLORBAR_BRD	"\033[1;41m"	/* board.c, favor.c ������ */
#define COLORBAR_POST	"\033[1;43m"	/* post.c ������ */
#define COLORBAR_GEM	"\033[1;42m"	/* gem.c  ������ */
#define COLORBAR_PAL	"\033[1;45m"	/* pal.c  ������ */
#define COLORBAR_USR	"\033[1;45m"	/* ulist.c ������ */
#define COLORBAR_BMW	"\033[1;43m"	/* bmw.c ������ */
#define COLORBAR_MAIL	"\033[1;42m"	/* mail.c ������ */
#define COLORBAR_ALOHA	"\033[1;41m"	/* aloha.c ������ */
#define COLORBAR_VOTE	"\033[;30;43m"	/* vote.c ������ */
#define COLORBAR_NBRD	"\033[1;46m"	/* newbrd.c ������ */
#define COLORBAR_SONG	"\033[1;42m"	/* song.c ������ */
#define COLORBAR_RSS	"\033[1;46m"	/* rss.c ������ */

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
			COLOR10 "  �s��     �� �� �@  ��       ��  ��  ��  �D%*s                                  \033[m"

#define NECKER_MBOX	"[��]���} [��,r]Ū�H [d]�R�� [R,y](�s��)�^�H [s]�H�H [x]��� [X]��F [h]����\n" \
			COLOR10 "  �s��   �� �� �@  ��       �H  ��  ��  �D%*s                                    \033[m"

#define NECKER_POST	"[��]���} [��]�\\Ū [^P]�o�� [b]�i�O�e�� [d]�R�� [V]�벼 [TAB]��ذ� [h]����\n" \
			COLOR10 "  �s��     �� �� �@  ��       ��  ��  ��  �D%*s                        �H��:%-4d \033[m"

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
COLOR1 " �s�� P.%d (%d%%) " COLOR9 " (h)"COLOR2"���� " COLOR9 "(@)"COLOR2"�ʵe���� "COLOR9"(����0$)"COLOR2"���� "COLOR9"(/n)"COLOR2"�j�M "COLOR9"(C)"COLOR2"�Ȧs "COLOR9"(��q)"COLOR2"����  "

#define FOOTER_VEDIT	\
COLOR1 " %s " COLOR9 " (^Z)"COLOR2"���� "COLOR9"(^W)"COLOR2"�Ÿ� "COLOR9"(^L)"COLOR2"��ø "COLOR9"(^X)"COLOR2"�ɮ׳B�z ��"COLOR9"%s"COLOR2"�x"COLOR9"%s"COLOR2"��"COLOR9"%5d:%3d"COLOR2"    \033[m"

#define FOOTER_VEDIT_RONLY	\
"%.0s\033[1;5m             ��Ū�Ҧ�                 \033[m"COLOR9"(^X)"COLOR2"�ɮ׳B�z ��"COLOR9"%s"COLOR2"�x"COLOR9"%s"COLOR2"��"COLOR9"%5d:%3d"COLOR2"    \033[m"

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
COLOR1 " �s���W�� " COLOR9 " ���W��Ȩ��s���A�O�D�ק�O�ͦW��аh�X��� o ��ܭק�              "

#define FEETER_RSS	\
COLOR1 " RSS �]�w " COLOR9 " (^P/a)"COLOR2"�s�W "COLOR9"(d)"COLOR2"�R�� "COLOR9"(E)"COLOR2"�s�� "COLOR9"(T)"COLOR2"��ñ�W�� "COLOR9"(H)"COLOR2"RSS���} "COLOR9"(m)"COLOR2"���� "COLOR9"(h)"COLOR2"����  "

/* ----------------------------------------------------- */
/* ���x�ӷ�ñ�W						 */
/* ----------------------------------------------------- */

/* itoc: ��ĳ banner ���n�W�L�T��A�L������ñ�i��|�y���Y�ǨϥΪ̪��ϷP */

/*dexter  ��origin,From�令���*/
#define EDIT_BANNER	"\n--\n" \
"\033[;32m�� Origin: �����毸<bbs.cs.nthu.edu.tw>\n\033[;32m�� From: %-*.*s\033[m\n"

#define MODIFY_BANNER	"\033[;36m%s �� %s �q %s �ק�\033[m\n"

#define EDIT_BANNER_1	"\n--\n" \
"              \033[;30m\242\033[47m\253 \033[m��  \033[31m�w\033[1;37m �������寸�����}�i�I\033[;31m�w\033[1;31m\243\033[;31m\273�w\033[1;31m\243\033[;31m\273�w\033[30;41m�l\033[m \033[1;36m\267\033[;36m\254\033[1;36m\276\033[;36m\364\033[1;36m\305\033[;36m\346\033[1;36m\257\033[;36m\270\033[1;36m\243\033[;36m\273\033[m\n" \
"   \033[1;37m���M���O ��\033[m \033[30;45m��\033[47m�g\033[m�p \033[1;36m�@�@�@�@�@�@�@�@�@�@\033[m            \033[30;41m�n �x\033[31;40m�p�e�f�g\033[m\n" \
"   \033[1;37m���ڽ��I\033[m�e�f�h\033[1;30;47m \033[m;\033[47m \033[;30;47m \033[m \033[1;36m�֨ӴM��A�R���@�a�C\033[m       \033[1;31m�d�g\033[41m��\033[;30;41m ���@\033[31m     \033[36m��\033[46m    \033[30;47m��\033[m\n" \
"          \033[30m \033[47m���d\033[45m\242\033[37;47m\146 \033[1;31m\241\033[41m\100\033[;30;47m\242\033[40m\250\033[36m���A�ä��w�媺�@�~��ӺۡC\033[37m \033[30;41m��       \033[31m\242\033[36m\251\033[30m   \033[31m��\033[30;46m�סס�\033[47m \033[37m\241\033[33;40m\273\033[m\n" \
"                 \033[30;47m ��\033[m  \033[1;36m��Ǫ��Ϭ��u�O�A�j�P�ܽбz�C\033[m \033[30;41m���b�c  \033[36;46m \033[41m�g\033[46m         \033[30;47m��\033[m\n" \
"      \033[30;42m\242\033[32m\251     \033[37m   \241\033[47m\264\033[30m.\033[37m\241\033[42m\264 %-*.*s\033[30m \033[37m \033[31m��\033[32;41m��\033[42m          \242\033[30m\252\033[m\n"

#define EDIT_BANNER_2	"\n--\n" \
"  �e       �� �e�e�e �e�e�e �e     �e�e�e \033[1;37m �M�j��u\033[m\n" \
"\033[1;37;44m  �i �i�����i �i�e�i �i�e�i �i     �i�e�e  \033[33m%-*.*s\033[m\n" \
"  �i �i�����i �i  �i �i     �i�e�e �i�e�e \033[1;37m�i�����毸�j telnet://imaple.tw\033[m\n"

#define EDIT_BANNER_3	"\n--\n" \
"\033[;30m\242\033[44m\143_\241\033[1;30m\304��\033[33m�]_\033[;34m�h�g\033[1;31;44m' * \033[;34;44m\242\033[31m\251_ \033[34;41m\242\033[31;44m\251\242\033[41m\250\033[34;40m�g�h\033[30;44m \033[37m�b_\033[30m �e�g�h\033[m\241\033[33m\103\033[31m.\033[1;34m�����毸\241\033[37m\104\033[30mtelnet://imaple.tw\033[;30m\241\033[41m\273\033[31;40m\242\033[41m\251\033[40m}\033[m\n" \
" \033[34m=\033[37m\242\033[44m\142  �v��\033[30m\241\033[34m\303   \033[31m*\033[33m.   \033[34;41m�g\033[1;30;44m\244\033[;30;41m\164@\033[31;44m�n \033[30m�d\033[m�v   \033[1;35m�c~\033[5m+\033[;33m                                  \033[30m\242\033[31m\253\033[1;31m\244\033[;30m\241\033[m\n" \
"   ��\033[30;44m�d�c�b�c\241\033[31m\103\033[30m�b�c��\033[31m_\242\033[41m\250\033[30m��\033[44m_�h\033[33;40m.\033[31m*   \033[1;35m�u=rom�G\033[37m%-*.*s\033[m\n" \
"   \033[31m�ǡáǡááǡǡááǡáǡáá�\033[35m@\244\033[30m\110\033[31m�ááǡáǡááǡǡááǡǡááǡǡáǡ�\033[m\n"

#define EDIT_BANNER_MEICHU_WIN	"\n--\n" \
"\033[42m   \033[0;30;42m��\033[0;42m�j\033[0;30;42m��\033[0;42m�j \033[0;31;43m  �ڭn  \033[0;42m                                                  \033[0;30;42m(((�k  \033[m\n" \
"\033[42m \033[0;32;47m�p\033[1;37;40m�h�i�h\033[42m�� \033[0;31;43m �Y�ˤl \033[42m  \033[0;33m   �v�������ɱN�� 3/6 ~ 3/8 �i�}    \033[0;31;42m          ��  ��   \033[m\n" \
"\033[42m \033[0;32;47m�o\033[1;37;40m�i�i�i\033[1;37m -\033[1;37;42m�j   \033[0;33;42m�k                                                  \033[0;30;41m @) @)\033[0;31;40m\033[42m   \033[m\n" \
"\033[42m \033[1;37;42m �i�i�i�i�i\033[42m    \033[0;33;42m�k    \033[0;33m �����毸�ܽбz  �@�P���M�ؤj�ǥ[�o \033[0;31;42m    @     \033[0;31;42m��\033[0;30;41m�H\033[0;31;42m��   \033[m\n" \
"\033[0;32;47m�n\033[1;37;42m�i�i�i�i��  \033[1;37;42m,,\033[0;33;42m�k                                             \033[0;31;42m\\���i�i\033[42m\033[1;30m�i\033[0;31;42m�k   \033[m\n" \
"\033[0;32;47m�m\033[1;37;40m�i��    \033[0;32m��\033[0;30;42m���i\033[m\033[42m     \033[1;34m From:%-*.*s      \033[0;31;42m/\\     /\\     \033[m\n"

/* ----------------------------------------------------- */
/* ��L�T���r��						 */
/* ----------------------------------------------------- */

#define VMSG_NULL	"                           " COLOR1 " �� �Ы����N���~�� �� \033[m"

#define ICON_UNREAD_BRD		"\033[1;33m�E\033[m"	/* ��Ū�ݪO */
#define ICON_READ_BRD		"  "			/* �wŪ�ݪO */

#define ICON_GAMBLED_BRD	"\033[1;31m��\033[m"	/* �|���L�����ݪO */
#define ICON_VOTED_BRD		"\033[1;33m��\033[m"	/* �|��벼�����ݪO */
#define ICON_NOTRAN_BRD		"��"			/* ����H�O */
#define ICON_TRAN_BRD		"��"			/* ��H�O */

#define TOKEN_ZAP_BRD		'-'			/* zap �O */
#define TOKEN_FRIEND_BRD	'.'			/* �n�ͪO */
#define TOKEN_SECRET_BRD	')'			/* ���K�O */

#define GOODBYE_MSG		"G)�H���ӳu M)���i���� N)�d���O Q)�����H[Q] "

#ifdef SITE_LEXEL
#include "lexel.h"
#endif

#endif				/* _THEME_H_ */
