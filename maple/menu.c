/*-------------------------------------------------------*/
/* menu.c	( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* target : menu/help/movie routines		 	 */
/* create : 95/03/29				 	 */
/* update : 97/03/29				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"


extern UCACHE *ushm;
extern FCACHE *fshm;


#ifndef ENHANCED_VISIT
extern time4_t brd_visit[];
#endif


/* ----------------------------------------------------- */
/* ���} BBS ��						 */
/* ----------------------------------------------------- */


#define	FN_RUN_NOTE_PAD	"run/note.pad"
#define	FN_RUN_NOTE_TMP	"run/note.tmp"


typedef struct
{
  time4_t tpad;
  char msg[400];
}      Pad;


int
pad_view()
{
  int fd, count;
  Pad *pad;

  if ((fd = open(FN_RUN_NOTE_PAD, O_RDONLY)) < 0)
    return XEASY;

  count = 0;
  mgets(-1);

  for (;;)
  {
    pad = mread(fd, sizeof(Pad));
    if (!pad)
    {
      vmsg(NULL);
      break;
    }
    else if (!(count % 5))	/* itoc.020122: �� pad �~�L */
    {
      clear();
      move(0, 23);
      prints("�i �� �� �W �� �d �� �O �j                �� %d ��\n\n", count / 5 + 1);
    }

    outs(pad->msg);
    count++;

    if (!(count % 5))
    {
      move(b_lines, 0);
      outs("�Ы� [SPACE] �~���[��A�Ϋ���L�䵲���G ");
      /* itoc.010127: �ץ��b�������k����ΤU�A������|�����G�h��檺���D */

      if (vkey() != ' ')
	break;
    }
  }

  close(fd);
  return 0;
}


static int
pad_draw()
{
  int i, cc, fdr, color;
  FILE *fpw;
  Pad pad;
  char *str, buf[3][71];

  /* itoc.����: ���Q�ΰ��m�סA�Ӫ� */
  static char pcolors[6] = {31, 32, 33, 34, 35, 36};

  /* itoc.010309: �d���O���Ѥ��P���C�� */
  color = vans("�߱��C�� 1) \033[41m  \033[m 2) \033[42m  \033[m 3) \033[43m  \033[m "
    "4) \033[44m  \033[m 5) \033[45m  \033[m 6) \033[46m  \033[m [Q] ");

  if (color < '1' || color > '6')
    return XEASY;
  else
    color -= '1';

  do
  {
    buf[0][0] = buf[1][0] = buf[2][0] = '\0';
    move(MENU_XPOS, 0);
    clrtobot();
    outs("\n�Яd�� (�ܦh�T��)�A��[Enter]����");
    for (i = 0; (i < 3) &&
      vget(16 + i, 0, "�G", buf[i], 71, DOECHO); i++);
    if (!buf[0][0])
      return 0;
    cc = vans("(S)�s���[�� (E)���s�ӹL (Q)��F�H[S] ");
    if (cc == 'q' || i == 0)
      return 0;
  } while (cc == 'e');

  time4(&pad.tpad);

  /* itoc.020812.����: �睊�����ɭԭn�`�N struct Pad.msg[] �O�_���j */
  str = pad.msg;
  sprintf(str, "�~�t\033[1;46m %s �� %s \033[m�u", cuser.userid, cuser.username);

  for (cc = strlen(str); cc < 60; cc += 2)
    strcpy(str + cc, "�w");
  if (cc == 60)
    str[cc++] = ' ';

  sprintf(str + cc,
    COLOR_SITE " %s \033[m��\n"
    "�x  \033[1;%dm%-70s\033[m  �x\n"
    "�x  \033[1;%dm%-70s\033[m  �x\n"
    "��  \033[1;%dm%-70s\033[m  ��\n",
    Btime(&pad.tpad),
    pcolors[color], buf[0],
    pcolors[color], buf[1],
    pcolors[color], buf[2]);

  f_cat(FN_RUN_NOTE_ALL, str);

  if (!(fpw = fopen(FN_RUN_NOTE_TMP, "w")))
    return 0;

  fwrite(&pad, sizeof(pad), 1, fpw);

  if ((fdr = open(FN_RUN_NOTE_PAD, O_RDONLY)) >= 0)
  {
    Pad *pp;

    i = 0;
    cc = pad.tpad - NOTE_DUE * 60 * 60;
    mgets(-1);
    while (pp = mread(fdr, sizeof(Pad)))
    {
      fwrite(pp, sizeof(Pad), 1, fpw);
      if ((++i > NOTE_MAX) || (pp->tpad < cc))
	break;
    }
    close(fdr);
  }

  fclose(fpw);

  rename(FN_RUN_NOTE_TMP, FN_RUN_NOTE_PAD);
  pad_view();
  return 0;
}


static int
goodbye()
{
  /* itoc.010803: �q�i�������� */
  clear();
  film_out(FILM_GOODBYE, 0);

  int ch;
#ifdef HAVE_LOGOUTY
  if (cuser.ufo2 & UFO2_LOGOUTY)
  {
    if ((ch = vans(GOODBYE_NMSG)) == 'n')
      ch = 'q';
    else if (ch != 'm' && ch != 'p' && ch != 'q')
      ch = 'y';
  }
  else
#endif
    ch = vans(GOODBYE_MSG);

  switch (ch)
  {
  /* lkchu.990428: ���w�אּ������ */
  case 'g':
  case 'y':
    break;

  case 'm':
    m_sysop();
    break;

  case 'n':
  case 'p':
    /* if (cuser.userlevel) */
    if (HAS_PERM(PERM_POST)) /* Thor.990118: �n��post�~��d��, �������e */
      pad_draw();
    break;

  case 'q':
  default:
    /* return XEASY; */
    return 0;	/* itoc.010803: �q�F FILM_GOODBYE �n��ø */
  }

#ifdef LOG_BMW
  bmw_log();			/* lkchu.981201: ���y�O���B�z */
#endif

  //if (!(cuser.ufo & UFO_MOTD))	/* itoc.000407: �����e���@��²�� */
  if(1)
  {
    clear();
    prints("�˷R�� \033[32m%s(%s)\033[m�A�O�ѤF�A�ץ��{�i %s �j\n"
      "�H�U�O�z�b���������U��ơG\n",
      cuser.userid, cuser.username, str_site);
    acct_show(&cuser, 0);
    vmsg(NULL);
  }

  u_exit("EXIT ");
  move (0, 0);
  clrtobot();
  refresh();
  exit(0);
}


/* ----------------------------------------------------- */
/* help & menu processring				 */
/* ----------------------------------------------------- */


void
vs_head(title, mid)
  char *title, *mid;
{
  char buf[40], ttl[60];
  int spc, len;

  int broken = 0;

  if (mid)	/* xxxx_head() ���O�� vs_head(title, str_site); */
  {
    clear();
  }
  else		/* menu() ���~�� vs_head(title, NULL); ��椤�L�� clear() */
  {
    move(0, 0);
    clrtoeol();
    mid = str_site;
  }

  len = d_cols + 69 - strlen(title) - strlen(currboard);

  if (HAS_STATUS(STATUS_BIFF))
  {
    mid = "\033[5;41m �l�t�ӫ��a�F \033[m";
    spc = 14;
  }
  else
  {
    if ((spc = strlen(mid)) >= (len-7))
    {
      spc = len -7;                  /* smiler.080614 : �쪩�p����צ��~�A�~�t��7 */
      memcpy(ttl, mid, spc);
      mid = ttl;
      mid[spc] = '\0';
      broken = 1;
    }
  }

  spc = 2 + len - spc;
  len = 1 - spc & 1;
  memset(buf, ' ', spc >>= 1);
  buf[spc] = '\0';

#ifdef COLOR_HEADER
  spc = (time(0) % 7) + '1';
  prints("\033[1;4%cm�i%s�j%s\033[33m%s\033[1;37;4%cm%s�m%s�n\033[m\n",
    spc, title, buf, mid, spc, buf + len, currboard);
#else
  if(!broken)
  {
    prints(COLOR_SITE "�i%s�j%s\033[33m%s" COLOR_SITE "%s�ݪO�m%s�n\033[m\n",
      title, buf, mid, buf + len+4, currboard);
  }
  else
  {
    prints(COLOR_SITE "�i%s�j%s\033[33m%s" COLOR_SITE "�ݪO�m%s�n\033[m\n",
    title, buf, mid, currboard);
  }

/*20070325	dexter:buf + len  modified to buf+len+2	*/
/*		not knowing why it works. :p		*/

#endif

}


/* ------------------------------------- */
/* �ʵe�B�z				 */
/* ------------------------------------- */


static char feeter[300];


/* itoc.010403: �� feeter �� status �W�ߥX�ӡA�w�ƨѨ�L function �s�� */

static void
status_foot()
{
  static int orig_flag = -1;
  static time_t uptime = -1;
  static int orig_money = -1;
  static int orig_gold = -1;
  static char flagmsg[100];
  static char coinmsg[100];

  int ufo;
  time_t now;

  ufo = cuser.ufo;
  time(&now);

  /* Thor: �P�� ��� �I�s�� �W���q�� ���� */

#ifdef HAVE_ALOHA
  ufo &= UFO_PAGER | UFO_ALOHA | UFO_CLOAK | UFO_QUIET;
  if (orig_flag != ufo)
  {
    orig_flag = ufo;
#ifndef MENU_FEAST
    sprintf(flagmsg,
      "%s%s%s%s",
      (ufo & UFO_PAGER) ? "\033[m\033[30;41m7\033[m" : COLOR2" ",     //7
      (ufo & UFO_QUIET) ? "\033[m\033[30;42m9\033[m" : COLOR2" ",     //9
      (ufo & UFO_ALOHA) ? "\033[m\033[30;43mB\033[m" : COLOR2" ",     //B
      (ufo & UFO_CLOAK) ? "\033[m\033[30;45mO\033[m" : COLOR2" ");    //O
#else
    sprintf(flagmsg,
      "%s%s%s%s",
      (ufo & UFO_PAGER) ? "��" : "�}",     //7
      (ufo & UFO_ALOHA) ? "�W" : "  ",     //B
      (ufo & UFO_QUIET) ? "�R" : "  ",     //9
      (ufo & UFO_CLOAK) ? "��" : "  ");    //O
#endif
  }
#else
  ufo &= UFO_PAGER | UFO_CLOAK | UFO_QUIET;
  if (orig_flag != ufo)
  {
    orig_flag = ufo;
#ifndef MENU_FEAST
    sprintf(flagmsg,
      "%s%s%s  ",
      (ufo & UFO_PAGER) ? "\033[m\033[30;41m7" : COLOR2" ",     //7
      (ufo & UFO_QUIET) ? "\033[m\033[30;43mB" : COLOR2" ",     //B
      (ufo & UFO_CLOAK) ? "\033[m\033[30;45mO" : COLOR2" ");    //O
#else
    sprintf(flagmsg,
      "%s%s%s",
      (ufo & UFO_PAGER) ? "��" : "�}",   //7
      (ufo & UFO_QUIET) ? "�R" : "  ",   //B
      (ufo & UFO_CLOAK) ? "��" : "  ");  //O
#endif
  }
#endif

  if (now > uptime)	/* �L�F�l�]�n��s�ͤ�X�� */
  {
    struct tm *ptime;

    ptime = localtime(&now);

    if (cuser.day == ptime->tm_mday && cuser.month == ptime->tm_mon + 1)
      cutmp->status |= STATUS_BIRTHDAY;
    else
      cutmp->status &= ~STATUS_BIRTHDAY;

    uptime = now + 86400 - ptime->tm_hour * 3600 - ptime->tm_min * 60 - ptime->tm_sec;
  }

  if (cuser.money != orig_money)
  {
    orig_money = cuser.money;
    /* ryanlei.081018: ��ȹ��]�tCOLOR11����X�A�G�ק�coinmsg��index */
    sprintf(coinmsg, "��" COLOR11 "%4d%c",
      (orig_money & 0x7FF00000) ? (orig_money / 1000000) :
      (orig_money & 0x7FFFFC00) ? (orig_money / 1000) : orig_money,
      (orig_money & 0x7FF00000) ? 'M' : (orig_money & 0x7FFFFC00) ? 'K' : ' ');
    coinmsg[7 + strlen(COLOR11)] = ' ';
  }

  if (cuser.gold != orig_gold)
  {
    orig_gold = cuser.gold;
    sprintf(coinmsg + 8 + strlen(COLOR11), COLOR2 "��" COLOR11 "%4d%c " COLOR2,
      (orig_gold & 0x7FF00000) ? (orig_gold / 1000000) :
      (orig_gold & 0x7FFFFC00) ? (orig_gold / 1000) : orig_gold,
      (orig_gold & 0x7FF00000) ? 'M' : (orig_gold & 0x7FFFFC00) ? 'K' : ' ');
  }

  /* Thor.980913.����: �̱`���I�s status_foot() ���ɾ��O�C����s film�A�b 60 ��H�W�A
                       �G���ݰw�� hh:mm �ӯS�O�@�@�r���x�s�H�[�t */

  ufo = (now - (uptime - 86400)) / 60;	/* �ɥ� ufo �Ӱ��ɶ�(��) */

  /* itoc.010717: ��@�U feeter �Ϫ��שM FEETER_XXX �@�P */
#ifndef MENU_FEAST
  sprintf(feeter, COLOR1 " %8.8s %02d:%02d "
    COLOR2 " �H�� " COLOR11 "%-4d" COLOR2 " �ڬO" COLOR11 "%-12s"
    COLOR2 "%s[�I�s]%-4s\033[m" COLOR2 " " COLOR8 "(h)����",
    fshm->today, ufo / 60, ufo % 60, total_user, cuser.userid, coinmsg, flagmsg);
#else
  sprintf(feeter, COLOR1 "[%10.10s %02d:%02d] " COLOR_SITE "%-10.10s "
    COLOR2 " [�X ��]" COLOR11 "%d" COLOR2 "�H [�즹�@�C]" COLOR11 "%-12s"
    COLOR2 "[�I�s]" COLOR11 "%6.6s\033[m" COLOR2 ,
    fshm->today, ufo / 60, ufo % 60, fshm->feast, total_user, cuser.userid, flagmsg);
#endif
  outf(feeter);
}


void
movie()
{
  /* Thor: it is depend on which state */

  if ((bbsmode <= M_XMENU) && (cuser.ufo & UFO_MOVIE))
    film_out(FILM_MOVIE, MENU_XNOTE);

  /* itoc.010403: �� feeter �� status �W�ߥX�� */
  status_foot();
}


typedef struct
{
  void *func;
  /* int (*func) (); */
  usint level;
  int umode;
  char *desc;
}      MENU;


#define	MENU_LOAD	1
#define	MENU_DRAW	2
#define	MENU_FILM	4


#define	PERM_MENU	PERM_PURGE


static MENU menu_main[];
static MENU menu_system[];
static MENU menu_thala[];
#ifdef HAVE_BITLBEE
static MENU menu_msn[];
#endif

/* ----------------------------------------------------- */
/* administrator's maintain menu			 */
/* ----------------------------------------------------- */


static MENU menu_admin[] =
{
  "bin/admutil.so:a_user", PERM_ALLACCT, - M_SYSTEM,
  "User       �i �U�ȸ�� �j",

  "bin/admutil.so:a_search", PERM_ALLACCT, - M_SYSTEM,
  "Hunt       �i �j�M���� �j",

  "bin/admutil.so:a_editbrd", PERM_ALLBOARD, - M_SYSTEM,
  "QSetBoard  �i �]�w�ݪO �j",

  "bin/innbbs.so:a_innbbs", PERM_ALLBOARD, - M_SYSTEM,
  "InnBBS     �i ��H�]�w �j",

#ifdef HAVE_REGISTER_FORM
  "bin/admutil.so:a_register", PERM_ALLREG, - M_SYSTEM,
  "Register   �i �f���U�� �j",

  "bin/admutil.so:a_regmerge", PERM_ALLREG, - M_SYSTEM,
  "Merge      �i �_��f�� �j",
#endif

  menu_system, PERM_ALLADMIN, M_AMENU,
  "System     �i �t�γ]�w �j",

  "bin/admutil.so:a_resetsys", PERM_ALLADMIN, - M_SYSTEM,
  "BBSreset   �i ���m�t�� �j",

  "bin/admutil.so:a_restore", PERM_SYSOP, - M_SYSTEM,
  "TRestore   �i �٭�ƥ� �j",

  menu_main, PERM_MENU + Ctrl('A'), M_AMENU,
  "�t�κ��@"
};


static MENU menu_system[] =
{
  "bin/admutil.so:a_system_setup", PERM_SYSOP, - M_XFILES,
  "Setup      �i ����]�w �j",

  "bin/admutil.so:a_xfile", PERM_SYSOP, - M_XFILES,
  "Xfile      �i �t���ɮ� �j",

  "bin/admutil.so:a_ias_bank", PERM_ALLADMIN, - M_XFILES,
  "IAS_Bank   �i �����֧Q �j",

  menu_admin, PERM_MENU + Ctrl('A'), M_AMENU,
  "�t�κ��@"
};


/* ----------------------------------------------------- */
/* mail menu						 */
/* ----------------------------------------------------- */


static int
XoMbox()
{
  xover(XZ_MBOX);
  return 0;
}


static MENU menu_mail[] =
{
  XoMbox, PERM_BASIC, M_RMAIL,
  "Read       �i �\\Ū�H�� �j",

  m_send, PERM_LOCAL, M_SMAIL,
  "Mail       �i �����H�H �j",

#ifdef MULTI_MAIL  /* Thor.981009: ����R�����B�H */
  m_list, PERM_LOCAL, M_SMAIL,
  "List       �i �s�ձH�H �j",
#endif

  m_internet, PERM_INTERNET, M_SMAIL,
  "Internet   �i �H�̩f�� �j",

#ifdef HAVE_SIGNED_MAIL
  m_verify, 0, M_XMODE,
  "Verify     �i ���ҫH�� �j",
#endif

#ifdef HAVE_MAIL_ZIP
  m_zip, PERM_INTERNET, M_SMAIL,
  "Zip        �i ���]��� �j",
#endif

  m_sysop, 0, M_SMAIL,
  "Yes Sir!   �i ��ѯ��� �j",

  "bin/admutil.so:m_bm", PERM_ALLADMIN, - M_SMAIL,
  "BM All     �i �O�D�q�i �j",	/* itoc.000512: �s�W m_bm */

  "bin/admutil.so:m_all", PERM_ALLADMIN, - M_SMAIL,
  "User All   �i �����q�i �j",	/* itoc.000512: �s�W m_all */

  menu_main, PERM_MENU + Ctrl('A'), M_MMENU,	/* itoc.020829: �� guest �S�ﶵ */
  "�q�l�l��"
};


/* ----------------------------------------------------- */
/* talk menu						 */
/* ----------------------------------------------------- */


static int
XoUlist()
{
  xover(XZ_ULIST);
  return 0;
}


static MENU menu_talk[];


  /* --------------------------------------------------- */
  /* list menu						 */
  /* --------------------------------------------------- */

static MENU menu_list[] =
{
  t_pal, PERM_BASIC, M_PAL,
  "Pal        �i �B�ͦW�� �j",

#ifdef HAVE_LIST
  t_list, PERM_BASIC, M_PAL,
  "List       �i �S�O�W�� �j",
#endif

#ifdef HAVE_ALOHA
  "bin/aloha.so:t_aloha", PERM_PAGE, - M_PAL,
  "Aloha      �i �W���q�� �j",
#endif

#ifdef LOGIN_NOTIFY
  t_loginNotify, PERM_PAGE, M_PAL,
  "Notify     �i �t�Ψ�M �j",
#endif

  menu_talk, PERM_MENU + 'P', M_TMENU,
  "�U���W��"
};


static MENU menu_talk[] =
{
  XoUlist, 0, M_LUSERS,
  "Users      �i �C�ȦW�� �j",

  menu_list, PERM_BASIC, M_TMENU,
  "ListMenu   �i �]�w�W�� �j",

  t_pager, PERM_BASIC, M_XMODE,
  "Pager      �i �����I�s �j",

  t_cloak, PERM_CLOAK, M_XMODE,
  "Invis      �i �����K�k �j",

  t_query, 0, M_QUERY,
  "Query      �i �d�ߺ��� �j",

  t_talk, PERM_PAGE, M_PAGE,
  "Talk       �i ���ܺ��� �j",

  /* Thor.990220: ��ĥ~�� */
  "bin/chat.so:t_chat", PERM_CHAT, - M_CHAT,
  "ChatRoom   �i ���f��� �j",

  t_display, PERM_BASIC, M_BMW,
  "Display    �i �s�����y �j",

  t_bmw, PERM_BASIC, M_BMW,
  "Write      �i �^�U���y �j",

  menu_thala, PERM_MENU + 'U', M_TMENU,
  "���H���y"
};


#ifdef HAVE_BITLBEE
static MENU menu_msn[] =
{
  bit_main,   PERM_BASIC, M_LUSERS,
  "MSN     �i MSN Messenger �j",
  bit_display, PERM_BASIC, M_LUSERS,
  "Log     �i �^�U MSN �T�� �j",
  menu_thala, PERM_MENU + 'M', M_TMENU,
  "�Y�ɳq�T"
};
#endif


static MENU menu_thala[] =
{
  menu_talk, 0, M_TMENU,
  "TALK         �i ���H���y �j ",
#ifdef HAVE_BITLBEE
  menu_msn,  PERM_BASIC, M_TMENU,
  "MSN          �i �Y�ɳq�T �j ",
#endif
  menu_main, PERM_MENU + 'T', M_TMENU,
  "�𶢲��"
};

/* ----------------------------------------------------- */
/* user menu						 */
/* ----------------------------------------------------- */


static MENU menu_user[];
static MENU menu_user_set[];
static MENU menu_color_bar[];

  /* --------------------------------------------------- */
  /* register menu                                      */
  /* --------------------------------------------------- */

static MENU menu_register[] =
{
  u_addr, PERM_BASIC, M_XMODE,
  "Address    �i �q�l�H�c �j",

#ifdef HAVE_REGISTER_FORM
  u_register, PERM_BASIC, M_UFILES,
  "Register   �i ����U�� �j",
#endif

#ifdef HAVE_REGKEY_CHECK
  u_verify, PERM_BASIC, M_UFILES,
  "Verify     �i ��{�ҽX �j",
#endif

  u_deny, PERM_BASIC, M_XMODE,
  "Perm       �i ��_�v�� �j",

  menu_user, PERM_MENU + 'A', M_UMENU,
  "���U���"
};


static MENU menu_color_bar[] =
{
  u_menu_bar, PERM_BASIC, M_UFILES,
  "Menu   �i   ������   �j",

  menu_bpg_color_bar, PERM_BASIC, M_UFILES,
  "Bpg    �i �ݪO�峹��� �j",

  menu_pl_color_bar, PERM_BASIC, M_UFILES,
  "Pl     �i �O�ͦn�ͺ��� �j",

  menu_wma_color_bar, PERM_BASIC, M_UFILES,
  "Wma    �i ���y�H��W�� �j",

  menu_vns_color_bar, PERM_BASIC, M_UFILES,
  "Vns    �i �벼�s�p�I�q �j",

  u_rss_bar, PERM_BASIC, M_UFILES,
  "Rss    �i   �ݪO RSS   �j",

  menu_user_set, PERM_MENU + 'M', M_UMENU,
  "���γ]�w"
};


static MENU menu_user_set[] =
{
  u_setup, 0, M_UFILES,
  "Habit      �i �ߦn�Ҧ� �j",

#ifdef HAVE_UFO2
  u_setup2, PERM_VALID, M_UFILES,
  "Advance    �i �i���]�w �j",
#endif

#ifdef HAVE_MULTI_SIGN
  u_sign_set, 0, M_UFILES,
  "Sign       �i ��ñ�]�w �j",
#endif

  menu_color_bar, 0, M_UMENU,
  "Colorbar   �i ���γ]�w �j",

  u_usr_show_set, 0, M_UFILES,
  "Usrshow    �i �Ÿ���� �j",

  menu_user, PERM_MENU + 'H', M_UMENU,
  "�ߦn�Ҧ�"
};


static MENU menu_user[] =
{
  u_info, PERM_BASIC, M_XMODE,
  "Info       �i �ӤH��� �j",

  menu_user_set, 0, M_UMENU,
  "Set        �i �ߦn�Ҧ� �j",

  menu_register, PERM_BASIC, M_UMENU,
  "Register   �i ���U��� �j",

  pad_view, 0, M_READA,
  "Note       �i �[�ݯd�� �j",

  /* itoc.010309: ���������i�H�g�d���O */
  pad_draw, PERM_POST, M_POST,
  "Pad        �i �߱���~ �j",

  u_lock, PERM_BASIC, M_IDLE,
  "Lock       �i ��w�ù� �j",

  u_xfile, PERM_BASIC, M_UFILES,
  "Xfile      �i �ӤH�ɮ� �j",

  u_log, PERM_BASIC, M_UFILES,
  "ViewLog    �i �W���O�� �j",

  menu_main, PERM_MENU + 'S', M_UMENU,
  "�ӤH�]�w"
};


#ifdef HAVE_EXTERNAL

/* ----------------------------------------------------- */
/* tool menu						 */
/* ----------------------------------------------------- */


static MENU menu_tool[];


#ifdef HAVE_SONG
  /* --------------------------------------------------- */
  /* song menu						 */
  /* --------------------------------------------------- */

static MENU menu_song[] =
{
  "bin/song.so:XoSongLog", 0, - M_XMODE,
  "KTV        �i �I�q���� �j",

  "bin/song.so:XoSongMain", 0, - M_XMODE,
  "Book       �i �۩ұ��� �j",

  "bin/song.so:XoSongSub", 0, - M_XMODE,
  "Note       �i �q����Z �j",

  menu_tool, PERM_MENU + 'K', M_XMENU,
  "���I�q��"
};
#endif


#ifdef HAVE_GAME

#if 0

  itoc.010426.����:
  �q���C�����ν����סA�����a���n�����A�u�[���A������C

  itoc.010714.����:
  (a) �C�����C�����`��������b 1.01�A�@�ӱߤW���i�� 100 ���C���A
      �Y�N�`�]����J�h���C���A�h 1.01^100 = 2.7 ��/�C���@�ӱߤW�C
  (b) �Y�U�����v�������A�]�������b 1.0 ~ 1.02 �����A�����a�@�w���ȿ��A
      �B�Y�@����̰�����Ȫ����@���A�]���|�ȱo�L�����СC
  (c) ��h�W�A���v�V�C�̨��������� 1.02�A���v�����̨��������� 1.01�C

  itoc.011011.����:
  ���F�קK user multi-login ���Ӭ~���A
  �ҥH�b���C�����}�l�N�n�ˬd�O�_���� login �Y if (HAS_STATUS(STATUS_COINLOCK))�C

#endif

  /* --------------------------------------------------- */
  /* game menu						 */
  /* --------------------------------------------------- */

static MENU menu_game[];

static MENU menu_game1[] =
{
  "bin/liteon.so:main_liteon", 0, - M_GAME,
  "0LightOn   �i �ж��}�O �j",

  "bin/guessnum.so:guessNum", 0, - M_GAME,
  "1GuessNum  �i ���q�Ʀr �j",

  "bin/guessnum.so:fightNum", 0, - M_GAME,
  "2FightNum  �i ���q�Ʀr �j",

  "bin/km.so:main_km", 0, - M_GAME,
  "3KongMing  �i �թ����� �j",

  "bin/recall.so:main_recall", 0, - M_GAME,
  "4Recall    �i �^�Ф��Z �j",

  "bin/mine.so:main_mine", 0, - M_GAME,
  "5Mine      �i �ý�a�p �j",

  "bin/fantan.so:main_fantan", 0, - M_GAME,
  "6Fantan    �i �f�u���s �j",

  "bin/dragon.so:main_dragon", 0, - M_GAME,
  "7Dragon    �i ���s�C�� �j",

  "bin/nine.so:main_nine", 0, - M_GAME,
  "8Nine      �i �Ѧa�E�E �j",

  menu_game, PERM_MENU + '0', M_XMENU,
  "�q���Ŷ�"
};

static MENU menu_game2[] =
{
  "bin/dice.so:main_dice", 0, - M_GAME,
  "0Dice      �i �g�Y��l �j",

  "bin/gp.so:main_gp", 0, - M_GAME,
  "1GoldPoker �i ���P���J �j",

  "bin/bj.so:main_bj", 0, - M_GAME,
  "2BlackJack �i �G�Q�@�I �j",

  "bin/chessmj.so:main_chessmj", 0, - M_GAME,
  "3ChessMJ   �i �H�ѳ±N �j",

  "bin/seven.so:main_seven", 0, - M_GAME,
  "4Seven     �i �䫰�C�i �j",

  "bin/race.so:main_race", 0, - M_GAME,
  "5Race      �i �i�ɰ��� �j",

  "bin/bingo.so:main_bingo", 0, - M_GAME,
  "6Bingo     �i ���G�j�� �j",

  "bin/marie.so:main_marie", 0, - M_GAME,
  "7Marie     �i �j�p���� �j",

  "bin/bar.so:main_bar", 0, - M_GAME,
  "8Bar       �i �a�x���� �j",

  menu_game, PERM_MENU + '0', M_XMENU,
  "�C���ֶ�"
};

static MENU menu_game3[] =
{
  "bin/pip.so:main_pip", PERM_BASIC, - M_GAME,
  "0Chicken   �i �q�l�p�� �j",

  "bin/pushbox.so:main_pushbox", 0, - M_GAME,
  "1PushBox   �i �ܮw�f�f �j",

  "bin/tetris.so:main_tetris", 0, - M_GAME,
  "2Tetris    �i �Xù���� �j",

  "bin/gray.so:main_gray", 0, - M_GAME,
  "3Gray      �i �L�Ǥj�� �j",

  menu_game, PERM_MENU + '0', M_XMENU,
  "�Ϥ�S��"
};

static MENU menu_game[] =
{
  menu_game1, PERM_BASIC, M_XMENU,
  "1Game      �i �q���Ѱ� �j",

  menu_game2, PERM_BASIC, M_XMENU,
  "2Game      �i �C���ֶ� �j",

  menu_game3, PERM_BASIC, M_XMENU,
  "3Game      �i �Ϥ�S�� �j",

  menu_tool, PERM_MENU + '1', M_XMENU,
  "�C���H��"
};
#endif


#ifdef HAVE_BUY
  /* --------------------------------------------------- */
  /* buy menu						 */
  /* --------------------------------------------------- */

static MENU menu_buy[] =
{
  "bin/bank.so:x_bank", PERM_BASIC, - M_GAME,
  "Bank       �i �H�U�Ȧ� �j",

  "bin/bank.so:b_invis", PERM_BASIC, - M_GAME,
  "Invis      �i ���β{�� �j",

  "bin/bank.so:b_cloak", PERM_BASIC, - M_GAME,
  "Cloak      �i �L������ �j",

  "bin/bank.so:b_mbox", PERM_BASIC, - M_GAME,
  "Mbox       �i �H�c�L�� �j",

  "bin/bank.so:b_xempt", PERM_VALID, - M_GAME,
  "Xempt      �i �ä[�O�d �j",

  "bin/bank.so:b_xvalid", PERM_VALID, - M_GAME,
  "Valid      �i �çK�{�� �j",

  "bin/bank.so:b_nthu", PERM_VALID, - M_GAME,
  "Nthu       �i �M�ئ��� �j",
/*
  "bin/bank.so:b_celebrate", PERM_VALID, - M_GAME,
  "Present    �� �}���n§ ��",
*/
  menu_tool, PERM_MENU + 'B', M_XMENU,
  "���ĥ���"
};
#endif


  /* --------------------------------------------------- */
  /* other tools menu					 */
  /* --------------------------------------------------- */

static MENU menu_other[] =
{
  "bin/vote.so:vote_all", PERM_BASIC, - M_VOTE,	/* itoc.010414: �벼���� */
  "VoteAll    �i �벼���� �j",

#ifdef HAVE_TIP
  "bin/xyz.so:x_tip", 0, - M_READA,
  "Tip        �i �оǺ��F �j",
#endif

#ifdef HAVE_LOVELETTER
  "bin/xyz.so:x_loveletter", 0, - M_READA,
  "LoveLetter �i ���Ѽ��g �j",
#endif

  "bin/xyz.so:x_password", PERM_VALID, - M_XMODE,
  "Password   �i �ѰO�K�X �j",

#ifdef HAVE_CLASSTABLE
  "bin/classtable.so:main_classtable", PERM_BASIC, - M_XMODE,
  "ClassTable �i �\\�Үɬq �j",
#endif

#ifdef HAVE_CREDIT
  "bin/credit.so:main_credit", PERM_BASIC, - M_XMODE,
  "MoneyNote  �i �O�b�⥾ �j",
#endif

#ifdef HAVE_CALENDAR
  "bin/todo.so:main_todo", PERM_BASIC, - M_XMODE,
  "XTodo      �i �ӤH��{ �j",

  "bin/calendar.so:main_calendar", 0, - M_XMODE,
  "YCalendar  �i �U�~��� �j",
#endif

  "bin/dictd.so:main_dictd", 0, - M_XMODE,
  "Dictd      �i �^�~�r�� �j",

  menu_tool, PERM_MENU + Ctrl('A'), M_XMENU,	/* itoc.020829: �� guest �S�ﶵ */
  "��L�\\��"
};


static MENU menu_tool[] =
{
#ifdef HAVE_SONG
  menu_song, 0, M_XMENU,
  "KTV        �i �u���I�q �j",
#endif

#ifdef HAVE_COSIGN
  "bin/newbrd.so:XoNewBoard", PERM_VALID, - M_XMODE,
  "Join       �i �ݪO�s�p �j",
#endif

#ifdef HAVE_GAME
  menu_game, PERM_BASIC, M_XMENU,
  "Game       �i �C���H�� �j",
#endif

#ifdef HAVE_BUY
  menu_buy, PERM_BASIC, M_XMENU,
  "Market     �i ���ĥ��� �j",
#endif

  menu_other, 0, M_XMENU,
  "Other      �i ���C���K �j",

  "bin/nthuctable.so:main_ctable", 0, - M_XMODE,
  "ClassTable �i �M�ؽҪ� �j",

  "bin/xyz.so:x_sysinfo", 0, - M_XMODE,
  "SysInfo    �i �t�θ�T �j",

  menu_main, PERM_MENU + Ctrl('A'), M_XMENU,	/* itoc.020829: �� guest �S�ﶵ */
  "�ӤH�u��"
};

#endif	/* HAVE_EXTERNAL */


/* ----------------------------------------------------- */
/* main menu						 */
/* ----------------------------------------------------- */


static int
Gem()
{
  /* itoc.001109: �ݪO�`�ަb (A)nnounce �U�� GEM_X_BIT�A��K�}�O */
  XoGem("gem/"FN_DIR, "��اG�i��", (HAS_PERM(PERM_ALLBOARD) ? (GEM_W_BIT | GEM_X_BIT | GEM_M_BIT) : 0));
  return 0;
}


static MENU menu_main[] =
{
  menu_admin, PERM_ALLADMIN, M_AMENU,
  "0Admin    �i �t�κ��@�� �j",

  Gem, 0, M_GEM,
  "Announce  �i ��ؤ��G�� �j",

  Boards, 0, M_BOARD,
  "Boards    �i �G�i�Q�װ� �j",

  Class, 0, M_BOARD,
  "Class     �i ���հQ�װ� �j",

#ifdef MEICHU_WIN
  Class2, 0, M_BOARD,
  "WinMeichu \033[1;36m�i !!! �v������ �M�j���� �ֶi�ӥ[�o !!! �j\033[m",
#endif

#ifdef MY_FAVORITE
  MyFavorite, PERM_BASIC, M_MF,
  "Favorite  �i �ۭq�Q�װ� �j",
#endif

  menu_mail, 0, M_MMENU,
  "Mail      �i �H��u��� �j",

  menu_thala, 0, M_TMENU,
  "Thala     �i �𶢲�Ѱ� �j",

  menu_user, 0, M_UMENU,
  "User      �i �ӤH�u��� �j",
/* HAVE_EXTERNAL*/
#ifdef HAVE_EXTERNAL
  menu_tool, 0, M_XMENU,
  "Xyz       �i �S��۫ݩ� �j",
#endif

#if 0	/* itoc.010209: ���� s �����i�J Select() ��ֿ����� */
  Select, 0, M_BOARD,
  "Select    �m ��ܥD�ݪO �m",
#endif

  goodbye, 0, M_XMODE,
  "Goodbye   �A�O" BBSNAME3 "�A�������ڨ��F",

  NULL, PERM_MENU + 'B', M_0MENU,
  "�D�\\���"
};


void
menu()
{
  MENU *menu, *mptr, *table[12];
  usint level, mode;
  int cc, cx;			/* current / previous cursor position */
  int max, mmx;			/* current / previous menu max */
  int cmd, depth;
  char *str;

  mode = MENU_LOAD | MENU_DRAW | MENU_FILM;
  menu = menu_main;
  level = cuser.userlevel;
  depth = mmx = 0;

  for (;;)
  {
    if (mode & MENU_LOAD)
    {
      for (max = -1;; menu++)
      {
	cc = menu->level;
	if (cc & PERM_MENU)
	{

#ifdef	MENU_VERBOSE
	  if (max < 0)		/* �䤣��A�X�v�����\��A�^�W�@�h�\��� */
	  {
	    menu = (MENU *) menu->func;
	    continue;
	  }
#endif

	  break;
	}
	if (cc && !(cc & level))	/* �����v���~�q�X */
	  continue;

	table[++max] = menu;
      }

      if (mmx < max)
	mmx = max;

      if ((depth == 0) && HAS_STATUS(STATUS_BIFF))	/* �Ĥ@���W���Y���s�H�A�i�J Mail ��� */
	cmd = 'M';
      else
	cmd = cc ^ PERM_MENU;	/* default command */
      utmp_mode(menu->umode);
    }

    if (mode & MENU_DRAW)
    {
      if (mode & MENU_FILM)
      {
	clear();
	movie();
	cx = -1;
      }

      vs_head(menu->desc, NULL);

      mode = 0;
      do
      {
	move(MENU_XPOS + mode, MENU_YPOS + 2);
	if (mode <= max)
	{
	  mptr = table[mode];
	  str = mptr->desc;
	  prints("(\033[1;36m%c\033[m)", *str++);
	  outs(str);
	}
	clrtoeol();
      } while (++mode <= mmx);

      mmx = max;
      mode = 0;
    }

    switch (cmd)
    {
    case KEY_DOWN:
      cc = (cc == max) ? 0 : cc + 1;
      break;

    case KEY_UP:
      cc = (cc == 0) ? max : cc - 1;
      break;

    case Ctrl('A'):	/* itoc.020829: �w�]�ﶵ�Ĥ@�� */
    case KEY_HOME:
      cc = 0;
      break;

    case KEY_END:
      cc = max;
      break;

    case KEY_PGUP:
      cc = (cc == 0) ? max : 0;
      break;

    case KEY_PGDN:
      cc = (cc == max) ? 0 : max;
      break;

    case '\n':
    case KEY_RIGHT:
      mptr = table[cc];
      cmd = mptr->umode;
#if 1
     /* Thor.990212: dynamic load , with negative umode */
      if (cmd < 0)
      {
	void *p = DL_get(mptr->func);
	if (!p)
	  break;
	mptr->func = p;
	cmd = -cmd;
	mptr->umode = cmd;
      }
#endif
      utmp_mode(cmd);

      if (cmd <= M_XMENU)	/* �l�ؿ��� mode �n <= M_XMENU */
      {
	mode = 0;
	if ((cmd == M_AMENU))
	{
	  if (!adm_check())
	    goto every_key;
	  else
	    mode = MENU_FILM;
	}

	menu->level = PERM_MENU + mptr->desc[0];
	menu = (MENU *) mptr->func;

	mode |= MENU_LOAD | MENU_DRAW;
	/* mode = MENU_LOAD | MENU_DRAW | MENU_FILM;	/* itoc.010304: �i�J�l��歫�� movie */

	depth++;
	continue;
      }

      {
	int (*func) ();

	func = mptr->func;
	mode = (*func) ();
      }

      utmp_mode(menu->umode);

      if (mode == XEASY)
      {
	outf(feeter);
	mode = 0;
      }
      else
      {
	mode = MENU_DRAW | MENU_FILM;
      }

      cmd = mptr->desc[0];
      continue;

#ifdef EVERY_Z
    case Ctrl('Z'):
      every_Z(0);
      goto every_key;

    case Ctrl('U'):
      every_U(0);
      goto every_key;

    case Ctrl('W'):
      DL_func("bin/dictd.so:main_dictd");
      goto every_key;

#endif

    case Ctrl('B'):		/* �ݪO�C�� */
      utmp_mode(M_BOARD);
      Boards();
      goto every_key;

    case Ctrl('C'):		/* ��ѫ� */
      if (cuser.userlevel)
      {
	utmp_mode(- M_CHAT);
	DL_func("bin/chat.so:t_chat");
      }
      goto every_key;

#ifdef MY_FAVORITE
    /* itoc.010911: Favorite everywhere�A���A����O�b M_0MENU */
    case Ctrl('F'):
      if (cuser.userlevel)	/* itoc.010407: �n�ˬd�v�� */
      {
	utmp_mode(M_MF);
	MyFavorite();
      }
      goto every_key;
#endif

    case Ctrl('I'):		/* �\Ū�H�� */
      if (cuser.userlevel)
      {
	utmp_mode(M_RMAIL);
	XoMbox();
      }
      goto every_key;

#ifdef HAVE_BITLBEE
    case Ctrl('N'):		/* MSN */
      if (cuser.userlevel)
      {
	utmp_mode(M_LUSERS);
	bit_main();
      }
      goto every_key;
#endif

    /* itoc.010911: Select everywhere�A���A����O�b M_0MENU */
    case Ctrl('S'):
      utmp_mode(M_BOARD);
      Select();
      goto every_key;

    case 's':
      if (bbsmode != M_UMENU && bbsmode != M_XMENU)
      {
	utmp_mode(M_BOARD);
	Select();
	goto every_key;
      }
      goto default_key;	/* �Y�b M_UMENU / M_XMENU ���� s ���ܡA�n�����@����� */

    case Ctrl('T'):		/*�ߦn�Ҧ� */
      utmp_mode(M_UFILES);
      u_setup();
      goto every_key;

    case 'h':			/* smiler.080222: menu �� help ��� */
      xo_help("menu");
      goto every_key;

    /* itoc.020301: Read currboard in M_0MENU */
    case 'r':
      if (bbsmode == M_0MENU)
      {
	if (currbno >= 0)
	{
	  utmp_mode(M_BOARD);
	  XoPost(currbno);
	  xover(XZ_POST);
#ifndef ENHANCED_VISIT
	  time4(&brd_visit[currbno]);
#endif
	}
	goto every_key;
      }
      goto default_key;	/* �Y���b M_0MENU ���� r ���ܡA�n�����@����� */

every_key:	/* �S����B�z���� */
      utmp_mode(menu->umode);
      mode = MENU_DRAW | MENU_FILM;
      cmd = table[cc]->desc[0];
      continue;

    case KEY_LEFT:
    case 'e':
      if (depth > 0)
      {
	menu->level = PERM_MENU + table[cc]->desc[0];
	menu = (MENU *) menu->func;
	mode = MENU_LOAD | MENU_DRAW;
	/* mode = MENU_LOAD | MENU_DRAW | MENU_FILM;	/* itoc.010304: �h�X�l��歫�� movie */
	depth--;
	continue;
      }
      cmd = 'G';

default_key:
    default:

      if (cmd >= 'a' && cmd <= 'z')
	cmd ^= 0x20;			/* �ܤj�g */

      cc = 0;
      for (;;)
      {
	if (table[cc]->desc[0] == cmd)
	  break;
	if (++cc > max)
	{
	  cc = cx;
	  goto menu_key;
	}
      }
    }

    if (cc != cx)	/* �Y��в��ʦ�m */
    {
//#ifdef CURSOR_BAR
      if(cuser.ufo & UFO_LIGHTBAR)
      {
	if (cx >= 0)
	{
	  move(MENU_XPOS + cx, MENU_YPOS);
	  if (cx <= max)
	  {
	    mptr = table[cx];
	    str = mptr->desc;
	    prints("  (\033[1;36m%c\033[m)%s  ", *str, str + 1);
	  }
	  else
	  {
	    outs("  ");
	  }
	}
	move(MENU_XPOS + cc, MENU_YPOS);
	mptr = table[cc];
	str = mptr->desc;
	//prints(COLORBAR_MENU "[ (\033[m\033[0;34;47m%c\033[m"COLORBAR_MENU")%s ]\033[m", *str, str + 1);
	prints("%s[ (%c)%s%s ]\033[m", UCBAR[UCBAR_MENU], *str, str + 1, UCBAR[UCBAR_MENU]);
	cx = cc;
      }
//#else		/* �S�� CURSOR_BAR */
      else
      {
	if (cx >= 0)
	{
	  move(MENU_XPOS + cx, MENU_YPOS);
	  outc(' ');
	}
	move(MENU_XPOS + cc, MENU_YPOS);
	outc('>');
	cx = cc;
      }
//#endif
    }
    else		/* �Y��Ъ���m�S���� */
    {
//#ifdef CURSOR_BAR
      if(cuser.ufo & cuser.ufo & UFO_LIGHTBAR)
      {
	move(MENU_XPOS + cc, MENU_YPOS);
	mptr = table[cc];
	str = mptr->desc;
	//prints(COLORBAR_MENU "[ (\033[m\033[0;34;47m%c"COLORBAR_MENU")%s ]\033[m", *str, str + 1);
	prints("%s[ (%c)%s%s ]\033[m", UCBAR[UCBAR_MENU], *str, str + 1, UCBAR[UCBAR_MENU]);
      }
//#else
      else
	move(MENU_XPOS + cc, MENU_YPOS + 1);
//#endif
    }

menu_key:

    cmd = vkey();
  }
}
