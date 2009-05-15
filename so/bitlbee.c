/*-------------------------------------------------------*/
/* bitlbee.c   ( NTHU CS MapleBBS Ver 3.10 )		 */
/*-------------------------------------------------------*/
/* author : ono.bbs@lexel.twbbs.org			 */
/* modify : smiler.bbs@bbs.cs.nthu.edu.tw		 */
/* target : MSN on MapleBBS				 */
/* create : 05/06/08					 */
/* update : 08/11/03					 */
/*-------------------------------------------------------*/

#include "bbs.h"


#ifdef HAVE_BITLBEE

extern XZ xz[];

static XO bit_xo;
static FILE *fw, *fr;
static char buf[512];
extern int bit_sock;


/* �w�]�W�u�̦h 250 �� */
static BITUSR bit_pool[250];

void bit_abort(void);


static
bit_fgets()
{
  char *tmp;

  fgets(buf, sizeof (buf), fr);
  tmp = strstr(buf, "Error: Someone else logged in with your account");

  if (tmp)
  {
    bit_abort();
    vmsg ("�s�u���_�I�z�w�b��L�a��n�J msn�I");
  }
}


static void
bit_item(num, pp)
  int num;
  BITUSR *pp;
{
  prints("%5d   \033[1;37m%-18.17s\033[m  \033[30;1m%-*.*s\033[m \033[1;%-18.17s\033[m \n",
    num, pp->nick, d_cols + 34, d_cols + 33, pp->addr,
    strstr(pp->status, "Online") ? "36m�u�W" :
    strstr(pp->status, "Away") ? "33m���}" :
    strstr(pp->status, "Busy") ? "31m���L" :
    strstr(pp->status, "Idle") ? "34m���m" :
    strstr(pp->status, "Right") ? "35m���W�^��" :
    strstr(pp->status, "Phone") ? "32m�q�ܤ�" :
    strstr(pp->status, "Lunch") ? "m�~�X�Y��" : pp->status);
}


#ifdef HAVE_LIGHTBAR
static int
bit_item_bar(xo, mode)
  XO *xo;
  int mode;	/* 1:�W����  0:�h���� */
{
  BITUSR *pp;

  //pp = bit_pool + xo->pos - xo->top;
  pp = bit_pool + xo->pos;

  prints("%s%5d   \033[1;37m%-18.17s\033[m%s  \033[1;%dm%-*.*s\033[m%s ",
    mode ? UCBAR[UCBAR_USR] : "", xo->pos + 1, pp->nick,
    mode ? UCBAR[UCBAR_USR] : "", mode ? 37 : 30,
    d_cols + 34, d_cols + 33, pp->addr, mode ? UCBAR[UCBAR_USR] : "");

  prints("\033[1;%-18.18s\033[m",
    strstr(pp->status, "Online") ? "36m�u�W" :
    strstr(pp->status, "Away") ? "33m���}" :
    strstr(pp->status, "Busy") ? "31m���L" :
    strstr(pp->status, "Idle") ? "34m���m" :
    strstr(pp->status, "Right") ? "35m���W�^��" :
    strstr(pp->status, "Phone") ? "32m�q�ܤ�" :
    strstr(pp->status, "Lunch") ? "37m�~�X�Y��" : "");

  return XO_NONE;
}
#endif


static int
bit_body(xo)
  XO *xo;
{
  BITUSR *pp;
  int num, max, tail;

  max = xo->max;
  num = xo->top;
  pp = &bit_pool[num];
  tail = num + XO_TALL;
  if (max > tail)
    max = tail;

  move(3, 0);
  do
  {
    bit_item(++num, pp++);
  } while (num < max);
  clrtobot();

  return XO_FOOT;
}


static int
bit_head(xo)
  XO *xo;
{
  clear();
  vs_head("MSN �C��", str_site);
  move(1, 0);
  prints(
    " [w]�ǰT [c]��ʺ� [^k]�_�u [a]�W�R�p���H [d]�R���p���H [l]msn���� [h]����   \n"
    "\033[30;47m �s��   �N   ��             �H          �c%-*.*s��  �A         \033[m",
    d_cols + 21, d_cols + 21, "");

  return bit_body(xo);
}


static int
bit_set(xo)
  XO *xo;
{
  char *tmp, max = 0;
  char seps[] = " \t";

  fprintf(fw, "PRIVMSG root :blist\r\n");
  fflush(fw);

  /* NICK ��n�b�� 48 �Ӧr, �e�����T�����L */
  do
  {
    bit_fgets();
    if (bit_sock <= 0)
      return XO_QUIT;
    tmp = strstr(buf, "Nick");
  }
  while (!tmp);

  for (;;)
  {
    bit_fgets ();
    if (bit_sock <= 0)
      return XO_QUIT;

    tmp = strstr(buf, "msn(");

    if (!tmp)
      break;
    else
    {
      tmp = strtok(buf, ":");
      tmp = strtok(NULL, seps);
      sprintf (bit_pool[max].nick, "%s", tmp);
      tmp = strtok(NULL, seps);
      sprintf (bit_pool[max].addr, "%s", tmp);
      tmp = strtok(NULL, seps);
      tmp = strtok(NULL, "\n");

      do
      {
	*tmp++;
      }
      while (*tmp == ' ');

      sprintf(bit_pool[max].status, "%s", tmp);
      max++;
    }
  };

  xo->max = max;
  if (xo->pos >= max)
    xo->pos = xo->top = 0;

  return bit_head(xo);
}


static int
bit_help(xo)
  XO *xo;
{
  xo_help("msn");
  return XO_HEAD;
}


static int
bit_write(xo)
  XO *xo;
{
  int pos;
  char hint[30], *nick, str[65], file[128];
  screenline sl[b_lines + 1];
  FILE *fp;

  pos = xo->pos;
  nick = bit_pool[pos].nick;
  sprintf (hint, "��[%s] ", nick);

  vs_save(sl);

  if (vget(b_lines - 1, 0, hint, str, 60, DOECHO) &&
    vans("�T�w�n�e�X MSN �T�� (Y/N)�H[Y] ") != 'n')
  {
    usr_fpath(file, cuser.userid, FN_MSN);
    fp = fopen(file, "a");
    fprintf(fp, "To %s (@msn)�G%s\n", nick, str);
    fclose(fp);

    fprintf(fw, "PRIVMSG %s :%s\r\n", nick, str);
    fflush(fw);
  }

  vs_restore(sl);
  return bit_body(xo);
//  return XO_INIT;
}


/* smiler.080319: �^�Ф��y */
void
bit_reply(nick, msg)
  char *nick;
  char *msg;
{
  char hint[30];
  char file[128];
  FILE *fp;

  sprintf(hint, "��<%s> ", nick);

  usr_fpath(file, cuser.userid, FN_MSN);
  fp = fopen(file, "a");
  fprintf(fp, "To %s (@msn)�G%s\n", nick, msg);
  fclose(fp);

  fprintf(fw, "PRIVMSG %s :%s\r\n", nick, msg);
  fflush(fw);
}


static int
bit_unblock(xo)
  XO *xo;
{
  if (vans("�T�w�n�Ѱ�����L (Y/N)�H[N]") == 'y')
  {
    int pos;
    char *nick;

    pos = xo->pos;
    nick = bit_pool[pos].nick;
    fprintf(fw, "PRIVMSG root :allow %s\r\n", nick);
    fflush(fw);
  }
  return XO_FOOT;
}


static int
bit_block(xo)
  XO *xo;
{
  if (vans("�T�w�n����L (Y/N)�H[N]") == 'y')
  {
    int pos;
    char *nick;

    pos = xo->pos;
    nick = bit_pool[pos].nick;
    fprintf(fw, "PRIVMSG root :block %s\r\n", nick);
    fflush(fw);
  }
  return XO_FOOT;
}


#if 0
static int
bit_save(xo)
  XO *xo;
{
  vmsg ("�]�w�n�O�H�ʺ٫�s�_�ӡA���s�W���ᤣ�ΦA�]�w�@�� :p");
  vmsg ("���L���٨S�g�n�� ^^;;");
  return XO_FOOT;
}
#endif


static int
bit_delpal(xo)
  XO *xo;
{
  int pos;
  char *nick;

  pos = xo->pos;
  nick = bit_pool[pos].nick;

  if (vans("�T�w�n�R���n�� (Y/N)�H[N]") == 'y')
  {
    fprintf(fw, "PRIVMSG root :remove %s\r\n", nick);
    fflush(fw);
  }
  return XO_INIT;
}


#if 0
static int
bit_onick(xo)
  XO *xo;
{
  int pos;
  char *nick, str[10];

  pos = xo->pos;
  nick = bit_pool[pos].nick;

  vmsg("�Ȯɥu����^��");
  move(b_lines - 1, 0);
  clrtoeol ();

  if (!vget(b_lines, 0, "���L���ӷs�W�r�a�G", str, 10, DOECHO))
    return XO_FOOT;
  if (strchr(str, ';') || strchr(str, ','))
    return XO_FOOT;

  fprintf(fw, "PRIVMSG root :rename %s %s\r\n", nick, str);
  fflush(fw);

  bit_fgets();
  if (bit_sock <= 0)
    return XO_QUIT;

  while (!strstr(buf, "Nick"))
  {
    bit_fgets();
    if (bit_sock <= 0)
      return XO_QUIT;

  };

  sleep(1);

  return XO_INIT;
}
#endif


static int
bit_mynick()
{
  char nick[40];

  if (!vget(b_lines, 0, "�ڪ��s�ʺ١G", nick, 38, DOECHO))
    return XO_FOOT;
  if (strchr(nick, ';') || strchr(nick, ','))
    return XO_FOOT;

  fprintf(fw, "PRIVMSG root :nick 0 %s\r\n", nick);
  fflush(fw);

  return XO_FOOT;
}


static int
bit_addpal()
{
  char addr[40];

  if (!vget(b_lines, 0, "��J�s�W�n�ͫH�c�G", addr, 38, DOECHO))
    return XO_FOOT;

  fprintf(fw, "PRIVMSG root :add 0 %s\r\n", addr);
  fflush(fw);

  return XO_INIT;
}


void
bit_abort()
{
  if (bit_sock > 0)
  {
    fprintf(fw, "QUIT :bye\r\n");
    fflush(fw);
    fclose(fw);
    fclose(fr);
    bit_sock = 0;
  }
}


static int
bit_close()
{
  if (vans("�T�w�n���_ MSN �s�u�H(y/N) [N]") != 'y')
    return XO_FOOT;

  bit_abort();

  zmsg("�еy�� ..... ");
  sleep (1);
  vmsg("�s�u���_�I");

  return XO_QUIT;
}


static int
bit_show(xo)
  XO *xo;
{
  bit_display();
  return bit_head(xo);
}


#if 0
static int
bit_test()
{
  char smiler_buf[32];
  sprintf(smiler_buf, "%d", cutmp->pid);
  vmsg(smiler_buf);
  return XO_INIT;
}
#endif


#ifndef NEW_KeyFunc
static KeyFunc bit_cb[] = 
{
#ifdef  HAVE_LIGHTBAR
  XO_ITEM, bit_item_bar,
#endif
  XO_INIT, bit_set,	/* bit_init */
  XO_LOAD, bit_body,	/* bit_load */
  XO_HEAD, bit_head,
//  XO_BODY, bit_body,

  'b', bit_unblock,	/* �Ѱ����� */
  'B', bit_block,	/* ����s���H */
  'a', bit_addpal,	/* �s�W�s���H */
  'd', bit_delpal,	/* �R���s���H */
  'l', bit_show,	/* �^�U FN_MSN �T�� */
//  's', bit_save,
//  'n', bit_onick,
  'c', bit_mynick,	/* ���ۤv���ʺ� */
  'w', bit_write,	/* �e MSN �T�� */
  Ctrl ('K'), bit_close,/* ���_�s�u */
  'h', bit_help
};
#else
static NewKeyFunc bit_cb[] = 
{
#ifdef  HAVE_LIGHTBAR
  XO_ITEM, bit_item_bar,        XO_ITEM,        'n',    "XO_ITEM",      NULL,
#endif
  /* bit_init */
  XO_INIT, bit_set,             XO_INIT,        'n',    "XO_INIT",      NULL,
  /* bit_load */
  XO_LOAD, bit_body,            XO_LOAD,        'n',    "XO_LOAD",      NULL,
  XO_HEAD, bit_head,            XO_HEAD,        'n',    "XO_HEAD",      NULL,
//  XO_BODY, bit_body,

  /* �Ѱ����� */
  'b', bit_unblock,             'b',    'p',    "�Ѱ�����",     NULL,
  /* ����s���H */
  'B', bit_block,               'B',    'p',    "�����p�Y�H",   NULL,
  /* �s�W�s���H */
  'a', bit_addpal,              'a',    'p',    "�s�W�p���H",   NULL,
  /* �R���s���H */
  'd', bit_delpal,              'd',    'p',    "�R���p���H",   NULL,
  /* �^�U FN_MSN �T�� */
  'l', bit_show,                'l',    'p',    "�^�U�T��",     NULL,
//  's', bit_save,
//  'n', bit_onick,
  /* ���ۤv���ʺ� */
  'c', bit_mynick,              'c',    'p',    "���ۤv�ʺ�", NULL,
  /* �e MSN �T�� */
  'w', bit_write,               'w',    'p',    "�ǰeMSN�T��",  NULL,
  /* ���_�s�u */
  Ctrl ('K'), bit_close,        Ctrl ('K'),     'p',    "���_MSN�s�u",  NULL,
  'h', bit_help                 'h',    'z',    "\�\\�໡��",   NULL
};
#endif


void
bit_rqst()
{
  FILE *fp;
  char *nick, *msg/*, send[600]*/, file[128];

  while (fgets(buf, sizeof (buf), fr))
  {
    if (msg = strstr(buf, "PRIVMSG"))
    {
      msg = strstr(msg, ":");
      *msg++;
      nick = strtok(buf, "!");
      *nick++;
//      sprintf(send, "\033[1;33;46m��%s (@msn) \033[37;45m %s \033[m", nick, msg);

      usr_fpath(file, cuser.userid, FN_MSN);
      fp = fopen(file, "a");
      fprintf(fp, "\033[1;33;46m��%s (@msn) \033[m�G%s", nick, msg);
      fclose(fp);
      cursor_save();

      /***  smiler.080319:�e��bmw����  ***/
      UTMP *up;
      BMW bmw;
      char buf[20];
      char bmw_msg[49];

      up = utmp_find(cuser.userno);
      sprintf(buf, "��<%s>", up->userid);
      if (strlen(msg) < 49)
	strcpy(bmw_msg, msg);
      else
	str_ncpy(bmw_msg, msg, 48);

      bmw_msg[strlen(bmw_msg)-1] = '\0';/* smiler.080319:�B�zbmw_msg������ '\n' */
      strcpy(bmw.nick, nick);		/* smiler.080319: �Ω�bmw���� reply msn */
      strcpy(bmw.msg, bmw_msg);
      bit_bmw_edit(up, buf, &bmw);
      /***********************************/
      cursor_restore();
      refresh();
      bell();
      if (strlen(msg) >= 49)	/* ���׶W�L���y�e�\�d��,�~�L�X */
	vmsg("MSN�T���L���A�Цܡi �^�U msn �T�� �j�[�ݧ���T�� !!");
      break;
    }
  }
}


int
bit_start(account, pass)
  char *account, *pass;
{
  int i = 0;
  char *tmp;

  if (bit_sock <= 0)
  {
    if (!account || !pass)	/* �H���U�@ */
      return 0;

    move(b_lines - 1, 0);
    sleep(1);
    clrtoeol();
    bit_sock = dns_open("127.0.0.1", 6667);
    zmsg("�s���� :p");
    sleep(1);

    if (bit_sock > 0)
    {
      zmsg("�n�J���A�֤F�O��A���ӵn�J�N�n���@�U�� :p (�Q���p��H�b�� ^^O)");

      fr = fdopen(bit_sock, "r");
      fw = fdopen(bit_sock, "w");

      fprintf(fw, "NICK b%d\r\n", cutmp->pid);
      fflush(fw);
      fprintf(fw, "USER bitlbee ono ccy :bitlbee run\r\n");
      fflush(fw);
      fprintf(fw, "JOIN #bitlbee\r\n");
      fflush(fw);

      fprintf(fw, "PRIVMSG root :account add msn %s %s\r\n", account, pass);
      fflush(fw);
      fprintf(fw, "PRIVMSG root :account on\r\n");
      fflush(fw);
      fprintf(fw, "PRIVMSG root :set charset BIG-5\r\n");
      fflush(fw);

      sleep (10);


      i = 0;

      while(1)
      {
        bit_fgets();

	if (bit_sock <= 0)
	  return XO_QUIT;

	if (strstr(buf, "Error"))
	{
	  vmsg("�b���αK�X��J���~�� :p");
	  bit_abort();
	  sleep(1);
	  return 0;
	}
        else if(i >= 100)
        {
          return vmsg("MSN�t�β��Ͳ��`���A�A�Ц�sysop�O�^��");
          bit_abort();
          sleep(1);
          return 0;
        }
        else if (strstr(buf, "Logging in: Logged in"))
          break;
	
	i++;
	
      }
    }
  }

  if (bit_sock > 0)
  {
    xz[XZ_BITLBEE - XO_ZONE].xo = &bit_xo;
    xz[XZ_BITLBEE - XO_ZONE].cb = bit_cb;
    if (i)
      bell();
    xover(XZ_BITLBEE);
  }
  else
    vmsg("�L�k�}�ҳs�u�A�Ц� sysop �O�^��");

  return 0;
}
#endif
