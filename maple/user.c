/*-------------------------------------------------------*/
/* user.c	( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* target : account / user routines		 	 */
/* create : 95/03/29				 	 */
/* update : 96/04/05				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"


extern char *ufo_tbl[];
extern char *usr_show_tbl[];


void
tn_user_show()	/* �ϥΪ̤W���A��ϤƻP���J�ϥΪ̤峹�C����ܳߦn */
{
  FILE *fp;
  char fpath[64];

  /* Ū�X�ϥΪ̭ӤH�]�w�� USR_SHOW */
  usr_fpath(fpath, cuser.userid, "MY_USR_SHOW");
  if (fp = fopen(fpath, "r"))	//�Y�ɮצs�b�hŪ�X��
  {
    fscanf(fp, "%ud", &USR_SHOW);
    fclose(fp);
  }
  else					//�Ϥ��h�N�ڭ� initial �� USR_SHOW �g�^�h
  {
    /* initialization USR_SHOW */
    USR_SHOW = -1 & ~(-1 << NUM_USR_SHOW);
    fp = fopen(fpath, "w");
    fprintf(fp, "%ud", USR_SHOW);
    fclose(fp);
  }
}

/* ----------------------------------------------------- */
/* �W���ɪ�l�ƭӤH����					 */
/* ----------------------------------------------------- */

static char *DEFCBAR[CBAR_NUM] = {
		COLORBAR_MENU, COLORBAR_BRD, COLORBAR_POST, COLORBAR_GEM,
		COLORBAR_PAL, COLORBAR_USR, COLORBAR_BMW, COLORBAR_MAIL,
		COLORBAR_ALOHA, COLORBAR_VOTE, COLORBAR_NBRD, COLORBAR_SONG, COLORBAR_RSS };

#if 1
static void
trans_bar_set()		/* �L��ɴ��A�ϥΪ̤W�������� */
{
  FILE *fp;
  char fpath[64];
  int i;

  char color[32];
  char barname_in[24];
  char *old_set[CBAR_NUM] = {"_MENU", "_BRD", "_POST", "_GEM", "_PAL", "_USR",
		"_BMW", "_MAIL", "_ALOHA", "_VOTE", "_NBRD", "_SONG", "_RSS"};

  for (i = 0; i < CBAR_NUM; i++)
  {
    sprintf(barname_in, "%s.bar", old_set[i]);
    usr_fpath(fpath, cuser.userid, barname_in);
    if (fp = fopen(fpath, "r"))
    {
      if (fgets(color,32,fp))
      {
	if (color[0]!='\0' && color[0]!=' ')
	{
	  strcpy(UCBAR[i], color);
	}
      }
      fclose(fp);
      unlink(fpath);	/* �ª��]�w�ɨS�ΤF */
    }
    else	/* �S���³]�w�ɡA�H���w�]���J */
      strcpy(UCBAR[i], DEFCBAR[i]);
  }

  /* �إ߷s���]�w�� */
  usr_fpath(fpath, cuser.userid, ".BARSET");
  if (fp = fopen(fpath, "w"))
  {
    for (i = 0; i < CBAR_NUM; i++)
    {
      fprintf(fp, "%s\n", UCBAR[i]);
    }
    fclose(fp);
  }
}
#endif


void
tn_user_bar()		/* �ϥΪ̤W���A��ϤƻP���J�ϥΪ̳]�w�� */
{
  FILE *fp;
  char fpath[64];
  char color[32];
  int i;

#if 1		/* �L��ɴ��A�ϥΪ̤W�������� */
  usr_fpath(fpath, cuser.userid, ".BARSET");
  if (!dashf(fpath))	/* �S�� .BARSET, ��ܤ��e�L��������]�w�A�άO�³]�w�|������ */
    trans_bar_set();
#endif

  if (fp = fopen(fpath, "r"))
  {
    for (i = 0; i < CBAR_NUM; i++)
    {
      fgets(color, 32, fp);
      if (color[0] != '\0' && color[0] != ' ')
      {
	 color[strlen(color) - 1] = '\0';
	 strcpy(UCBAR[i], color);
      }
    }
    fclose(fp);
  } 
}


/* ----------------------------------------------------- */
/* �]�w�ӤH����						 */
/* ----------------------------------------------------- */


static int
u_set_bar(bar)
  int bar;
{
  FILE *fp;
  char bright[2];
  char flash[2];
  char front[3];
  char back[3];
  char imaplecolor[32];
  char orgcolor[32];
  char color_write[32];
  char fpath[64];
  char ans;
  int i;

  /* load ������l���� */
  if (bar >= 13)
    strcpy(imaplecolor, "\033[m");
  else
  {
    strcpy(imaplecolor, DEFCBAR[bar]);
    strcpy(orgcolor, UCBAR[bar]);		/* load �ϥΪ̦۩w���� */
  }

  move(i = 1, 0);
  clrtobot();
  move(2, 0);
  prints("\033[1;37m�G   : \033[m\033[1;31m31\033[32m32\033[33m33\033[34m34\033[35m35\033[36m36\033[37m37\033[m\n");
  prints("\033[0;37m�t   : \033[m\033[31m31\033[32m32\033[33m33\033[34m34\033[35m35\033[36m36\033[37m37\033[m\n");
  prints("\033[0m���� : \033[m\033[41m41\033[42m42\033[43m43\033[44m44\033[45m45\033[46m46\033[47m47\033[m\n");
  prints("\033[m�����]�w : %s����\033[m\n", imaplecolor);
  prints("\033[m�ثe�]�w : \033[m%s����\033[m", orgcolor);

  i = 8;
  vget(i, 0, "�e���G�G(1)/�t(0)/���L(Enter)", bright, 2, DOECHO);
  if (bright[0]!='\0' && bright[0] != '0' && bright[0] != '1')
  {
    return vmsg("��J���~ !!");
  }

  sprintf(color_write,"\033[m\033[%s%s",
	(bright[0] == '\0') ? "" : bright, (bright[0] == '\0') ? "" : ";");
  color_write[strlen(color_write) - 1] = 'm';
  prints("\033[m�ثe�]�w : \033[m%s%s����\033[m", orgcolor, color_write);

  i += 2;
  vget(i, 0, "�e���G�{�{(5)/���L(Enter)", flash, 2, DOECHO);
  if ((flash[0]!='\0') && (flash[0]!='5'))
  {
    return vmsg("��J���~ !!");
  }
  sprintf(color_write,"\033[m\033[%s%s%s%s",
	(bright[0]=='\0') ? "" : bright, (bright[0]=='\0') ? "" : ";",
	(flash[0]=='\0')  ? "" : flash , (flash[0]=='\0')  ? "" : ";");
  color_write[strlen(color_write) - 1] = 'm';
  prints("\033[m�ثe�]�w : \033[m%s%s����\033[m", orgcolor, color_write);
	
  i += 2;
  vget(i, 0, "�e���G��X(31~37)/���L(Enter)", front, 3, DOECHO);
  if ((front[0] != '\0') && ((front[0] != '3') && (front[1] < '1' || front[1] > '7')))
  {
    return vmsg("��J���~ !!");
  }
  sprintf(color_write,"\033[m\033[%s%s%s%s%s%s",
	(bright[0] == '\0') ? "" : bright, (bright[0] == '\0') ? "" : ";",
	(flash[0] == '\0')  ? "" : flash , (flash[0] == '\0')  ? "" : ";",
	(front[0] == '\0')  ? "" : front , (front[0] == '\0')  ? "" : ";");
  color_write[strlen(color_write) - 1] = 'm';
  prints("\033[m�ثe�]�w : \033[m%s%s����\033[m", orgcolor, color_write);

  i += 2;
  vget(i, 0, "�I���G��X(41~47)/���L(Enter)", back, 3, DOECHO);
  if ((back[0] != '\0') && ((back[0] != '4') && (back[1] < '1' || back[1] > '7')))
  {
    return vmsg("��J���~ !!");
  }

  sprintf(color_write,"\033[m\033[%s%s%s%s%s%s%s%s",
	(bright[0] == '\0') ? "" : bright, (bright[0] == '\0') ? "" : ";",
	(flash[0] == '\0')  ? "" : flash , (flash[0] == '\0')  ? "" : ";",
	(front[0] == '\0')  ? "" : front , (front[0] == '\0')  ? "" : ";",
	(back[0] == '\0')   ? "" : back  , (back[0] == '\0')   ? "" : ";");
  color_write[strlen(color_write) - 1] = 'm';
  prints("\033[m�ثe�]�w : \033[m%s%s����\033[m", orgcolor, color_write);

  if (bright[0] == '\0' && flash[0] == '\0' && front[0] == '\0' && back[0] == '\0')
  {
    return vmsg("������� !!");
  }

  switch (ans = vans("�� ��� Y)�T�w N)���� D)�w�] [Q] "))
  {
    case 'd' :
      strcpy(UCBAR[bar], DEFCBAR[bar]);
      break;
    case 'y' :
      strcpy(UCBAR[bar], color_write);
      break;
    default :
      return 0;
  }

  usr_fpath(fpath, cuser.userid, ".BARSET");
  if (fp = fopen(fpath, "w"))		/* ��s�������g�� */
  {
    for (i = 0; i < CBAR_NUM; i++)
    {
      fprintf(fp, "%s\n", UCBAR[i]);
    }
    fclose(fp);
    return vmsg("��s���\\�I");
  }
  else
    return vmsg("��s���ѡA�Э��դ@���I");
}


int
u_menu_bar()
{
  return u_set_bar(UCBAR_MENU);
}

int
u_brd_bar()
{
  return u_set_bar(UCBAR_BRD);
}

int
u_post_bar()
{
  return u_set_bar(UCBAR_POST);
}

int
u_gem_bar()
{
  return u_set_bar(UCBAR_GEM);
}

int
u_pal_bar()
{
  return u_set_bar(UCBAR_PAL);
}

int
u_usr_bar()
{
  return u_set_bar(UCBAR_USR);
}

int
u_bmw_bar()
{
  return u_set_bar(UCBAR_BMW);
}

int
u_mail_bar()
{
  return u_set_bar(UCBAR_MAIL);
}

int
u_aloha_bar()
{
  return u_set_bar(UCBAR_ALOHA);
}

int
u_vote_bar()
{
  return u_set_bar(UCBAR_VOTE);
}

int
u_newbrd_bar()
{
  return u_set_bar(UCBAR_NBRD);
}

int
u_song_bar()
{
  return u_set_bar(UCBAR_SONG);
}

int
u_rss_bar()
{
  return u_set_bar(UCBAR_RSS);
}


/* ----------------------------------------------------- */
/* �{�ҥΨ禡						 */
/* ----------------------------------------------------- */


void
justify_log(userid, justify)	/* itoc.010822: ���� .ACCT �� justify �o���A��O���b FN_JUSTIFY */
  char *userid;
  char *justify;	/* �{�Ҹ�� RPY:email-reply  KEY:�{�ҽX  POP:pop3�{��  REG:���U�� */
{
  char fpath[64];
  FILE *fp;

  usr_fpath(fpath, userid, FN_JUSTIFY);
  if (fp = fopen(fpath, "a"))		/* �Ϊ��[�ɮסA�i�H�O�s�����{�ҰO�� */
  {
    fprintf(fp, "%s\n", justify);
    fclose(fp);
  }
}


static int
ban_addr(addr)
  char *addr;
{
  char *host;
  char foo[128];	/* SoC: ��m���ˬd�� email address */

  /* Thor.991112: �O���Ψӻ{�Ҫ�email */
  sprintf(foo, "%s # %s (%s)\n", addr, cuser.userid, Now());
  f_cat(FN_RUN_EMAILREG, foo);

  /* SoC: �O���� email ���j�p�g */
  str_lower(foo, addr);

  /* check for acl (lower case filter) */

  host = (char *) strchr(foo, '@');
  *host = '\0';

  /* *.bbs@xx.yy.zz�B*.brd@xx.yy.zz �@�ߤ����� */
  if (host > foo + 4 && (!str_cmp(host - 4, ".bbs") || !str_cmp(host - 4, ".brd")))
    return 1;

  /* ���b�զW��W�Φb�¦W��W */
  return (!acl_has(TRUST_ACLFILE, foo, host + 1) ||
    acl_has(UNTRUST_ACLFILE, foo, host + 1) > 0);
}


/* ----------------------------------------------------- */
/* POP3 �{��						 */
/* ----------------------------------------------------- */


#ifdef HAVE_POP3_CHECK

static int		/* >=0:socket -1:�s�u���� */
Get_Socket(site)	/* site for hostname */
  char *site;
{
  int sock;
  struct sockaddr_in sin;
  struct hostent *host;

  sock = 110;

  /* Getting remote-site data */

  memset((char *) &sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(sock);

  if (!(host = gethostbyname(site)))
    sin.sin_addr.s_addr = inet_addr(site);
  else
    memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);

  /* Getting a socket */

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return -1;
  }

  /* perform connecting */

  if (connect(sock, (struct sockaddr *) & sin, sizeof(sin)) < 0)
  {
    close(sock);
    return -1;
  }

  return sock;
}


static int		/* 0:���\ */
POP3_Check(sock, account, passwd)
  int sock;
  char *account, *passwd;
{
  FILE *fsock;
  char buf[512];

  if (!(fsock = fdopen(sock, "r+")))
  {
    outs("\n�Ǧ^���~�ȡA�Э��մX���ݬ�\n");
    return -1;
  }

  sock = 1;

  while (1)
  {
    switch (sock)
    {
    case 1:		/* Welcome Message */
      fgets(buf, sizeof(buf), fsock);
      break;

    case 2:		/* Verify Account */
      fprintf(fsock, "user %s\r\n", account);
      fflush(fsock);
      fgets(buf, sizeof(buf), fsock);
      break;

    case 3:		/* Verify Password */
      fprintf(fsock, "pass %s\r\n", passwd);
      fflush(fsock);
      fgets(buf, sizeof(buf), fsock);
      sock = -1;
      break;

    default:		/* 0:Successful 4:Failure  */
      fprintf(fsock, "quit\r\n");
      fclose(fsock);
      return sock;
    }

    if (!strncmp(buf, "+OK", 3))
    {
      sock++;
    }
    else
    {
      outs("\n���ݨt�ζǦ^���~�T���p�U�G\n");
      prints("%s\n", buf);
      sock = -1;
    }
  }
}


static int		/* -1:���䴩 0:�K�X���~ 1:���\ */
do_pop3(addr)		/* itoc.010821: ��g�@�U :) */
  char *addr;
{
  int sock, i;
  char *ptr, *str, buf[80], username[80];
  char *alias[] = {"", "pop.", "pop3.", "mail.", NULL};
  ACCT acct;

  strcpy(username, addr);
  *(ptr = strchr(username, '@')) = '\0';
  ptr++;

  clear();
  move(2, 0);
  prints("�D��: %s\n�b��: %s\n", ptr, username);
  outs("\033[1;5;36m�s�u���ݥD����...�еy��\033[m\n");
  refresh();

  for (i = 0; str = alias[i]; i++)
  {
    sprintf(buf, "%s%s", str, ptr);	/* itoc.020120: �D���W�٥[�W pop3. �ոլ� */
    if ((sock = Get_Socket(buf)) >= 0)	/* ���o�����B���䴩 POP3 */
      break;
  }

  if (sock < 0)
  {
    outs("�z���q�l�l��t�Τ��䴩 POP3 �{�ҡA�ϥλ{�ҫH�稭���T�{\n\n\033[1;36;5m�t�ΰe�H��...\033[m");
    return -1;
  }

  if (vget(15, 0, "�п�J�H�W�ҦC�X���u�@���b�����K�X�G", buf, 20, NOECHO))
  {
    move(17, 0);
    outs("\033[5;37m�����T�{��...�еy��\033[m\n");

    if (!POP3_Check(sock, username, buf))	/* POP3 �{�Ҧ��\ */
    {
      /* �����v�� */
      sprintf(buf, "POP: %s", addr);
      justify_log(cuser.userid, buf);
      strcpy(cuser.email, addr);
      if (acct_load(&acct, cuser.userid) >= 0)
      {
	time(&acct.tvalid);
	acct_setperm(&acct, PERM_VALID, 0);
      }

      /* �H�H�q���ϥΪ� */
      mail_self(FN_ETC_JUSTIFIED, str_sysop, msg_reg_valid, 0);
      cutmp->status |= STATUS_BIFF;
      vmsg(msg_reg_valid);

      close(sock);
      return 1;
    }
  }

  close(sock);

  /* POP3 �{�ҥ��� */
  outs("�z���K�X�γ\\�����F�A�ϥλ{�ҫH�稭���T�{\n\n\033[1;36;5m�t�ΰe�H��...\033[m");
  return 0;
}
#endif


/* ----------------------------------------------------- */
/* �]�w E-mail address					 */
/* ----------------------------------------------------- */


int
u_addr()
{
  char *msg, addr[64];

  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  /* itoc.050405: ���������v�̭��s�{�ҡA�]���|�ﱼ�L�� tvalid (���v����ɶ�) */
  if (HAS_PERM(PERM_ALLDENY))
  {
    vmsg("�z�Q���v�A�L�k��H�c");
    return XEASY;
  }

  film_out(FILM_EMAIL, 0);

  if (vget(b_lines - 2, 0, "E-mail �a�}�G", addr, sizeof(cuser.email), DOECHO))
  {
    if (not_addr(addr))
    {
      msg = err_email;
    }
    else if (ban_addr(addr))
    {
      msg = "�����������z���H�c�����{�Ҧa�}";
    }
    else
    {
#ifdef EMAIL_JUSTIFY
      if (vans("�ק� E-mail �n���s�{�ҡA�T�w�n�ק��(Y/N)�H[Y] ") == 'n')
	return 0;

#  ifdef HAVE_POP3_CHECK
      if (vans("�O�_�ϥ� POP3 �{��(Y/N)�H[N] ") == 'y')
      {
	if (do_pop3(addr) > 0)	/* �Y POP3 �{�Ҧ��\�A�h���}�A�_�h�H�{�ҫH�H�X */
	  return 0;
      }
#  endif

      if (bsmtp(NULL, NULL, addr, MQ_JUSTIFY) < 0)
      {
	msg = "�����{�ҫH��L�k�H�X�A�Х��T��g E-mail address";
      }
      else
      {
	ACCT acct;

	strcpy(cuser.email, addr);
	cuser.userlevel &= ~PERM_ALLVALID;
	if (acct_load(&acct, cuser.userid) >= 0)
	{
	  strcpy(acct.email, addr);
	  acct_setperm(&acct, 0, PERM_ALLVALID);
	}

	film_out(FILM_JUSTIFY, 0);
	prints("\n%s(%s)�z�n�A�ѩ�z��s E-mail address ���]�w�A\n\n"
	  "�бz���֨� \033[44m%s\033[m �Ҧb���u�@���^�Сy�����{�ҫH��z�C",
	  cuser.userid, cuser.username, addr);
	msg = NULL;
      }
#else
      msg = NULL;
#endif

    }
    vmsg(msg);
  }

  return 0;
}


/* ----------------------------------------------------- */
/* ��g���U��						 */
/* ----------------------------------------------------- */


#ifdef HAVE_REGISTER_FORM

static void
getfield(line, len, buf, desc, hint)
  int line, len;
  char *hint, *desc, *buf;
{
  move(line, 0);
  prints("%s%s", desc, hint);
  vget(line + 1, 0, desc, buf, len, GCARRY);
}


int
u_register()
{
  FILE *fn;
  int ans;
  RFORM rform;

#ifdef JUSTIFY_PERIODICAL
  if (HAS_PERM(PERM_VALID) && cuser.tvalid + VALID_PERIOD - INVALID_NOTICE_PERIOD >= ap_start)
#else
  if (HAS_PERM(PERM_VALID))
#endif
  {
    zmsg("�z�������T�{�w�g�����A���ݶ�g�ӽЪ�");
    return XEASY;
  }

  if (fn = fopen(FN_RUN_RFORM, "rb"))
  {
    while (fread(&rform, sizeof(RFORM), 1, fn))
    {
      if ((rform.userno == cuser.userno) && !strcmp(rform.userid, cuser.userid))
      {
	fclose(fn);
	zmsg("�z�����U�ӽг�|�b�B�z���A�Э@�ߵ���");
	return XEASY;
      }
    }
    fclose(fn);
  }

  if (vans("�z�T�w�n��g���U���(Y/N)�H[N] ") != 'y')
    return XEASY;

  move(1, 0);
  clrtobot();
  prints("\n%s(%s) �z�n�A�оڹ��g�H�U����ơG\n(�� [Enter] ������l�]�w)",
    cuser.userid, cuser.username);

  memset(&rform, 0, sizeof(RFORM));
  for (;;)
  {
    getfield(5, 50, rform.career, "�A�ȳ��G", "�Ǯըt�ũγ��¾��");
    getfield(8, 60, rform.address, "�ثe��}�G", "�]�A��ǩΪ��P���X");
    getfield(11, 20, rform.phone, "�s���q�ܡG", "�]�A���~�����ϰ�X");
    ans = vans("�H�W��ƬO�_���T(Y/N/Q)�H[N] ");
    if (ans == 'q')
      return 0;
    if (ans == 'y')
      break;
  }

  rform.userno = cuser.userno;
  strcpy(rform.userid, cuser.userid);
  time(&rform.rtime);
  rec_add(FN_RUN_RFORM, &rform, sizeof(RFORM));
  return 0;
}
#endif


/* ----------------------------------------------------- */
/* ��g���{�X						 */
/* ----------------------------------------------------- */


#ifdef HAVE_REGKEY_CHECK
int
u_verify()
{
  char buf[80], key[10];
  ACCT acct;

  if (HAS_PERM(PERM_VALID))
  {
    zmsg("�z�������T�{�w�g�����A���ݶ�g�{�ҽX");
  }
  else
  {
    if (vget(b_lines, 0, "�п�J�{�ҽX�G", buf, 8, DOECHO))
    {
      archiv32(str_hash(cuser.email, cuser.tvalid), key);	/* itoc.010825: ���ζ}�ɤF�A������ tvalid �Ӥ�N�O�F */

      if (str_ncmp(key, buf, 7))
      {
	zmsg("��p�A�z���{�ҽX���~");
      }
      else
      {
	/* �����v�� */
	sprintf(buf, "KEY: %s", cuser.email);
	justify_log(cuser.userid, buf);
	if (acct_load(&acct, cuser.userid) >= 0)
	{
	  time(&acct.tvalid);
	  acct_setperm(&acct, PERM_VALID, 0);
	}

	/* �H�H�q���ϥΪ� */
	mail_self(FN_ETC_JUSTIFIED, str_sysop, msg_reg_valid, 0);
	cutmp->status |= STATUS_BIFF;
	vmsg(msg_reg_valid);
      }
    }
  }

  return XEASY;
}
#endif


/* ----------------------------------------------------- */
/* ��_�v��						 */
/* ----------------------------------------------------- */


int
u_deny()
{
  ACCT acct;
  time_t diff;
  struct tm *ptime;
  char msg[80];

  if (!HAS_PERM(PERM_ALLDENY))
  {
    zmsg("�z�S�Q���v�A���ݴ_�v");
  }
  else
  {
    if ((diff = cuser.tvalid - time(0)) < 0)      /* ���v�ɶ���F */
    {
      if (acct_load(&acct, cuser.userid) >= 0)
      {
	time(&acct.tvalid);
#ifdef JUSTIFY_PERIODICAL
	/* xeon.050112: �b�{�ҧ֨���e�� Cross-Post�A�M�� tvalid �N�|�Q�]�w�쥼�Ӯɶ��A
	   ���_�v�ɶ���F�h�_�v�A�o�˴N�i�H�׹L���s�{�ҡA�ҥH�_�v��n���s�{�ҡC */
	acct_setperm(&acct, 0, PERM_ALLVALID | PERM_ALLDENY);
#else
	acct_setperm(&acct, 0, PERM_ALLDENY);
#endif
	vmsg("�U���ФŦA�ǡA�Э��s�W��");
      }
    }
    else
    {
      ptime = gmtime(&diff);
      sprintf(msg, "�z�٭n�� %d �~ %d �� %d �� %d �� %d ��~��ӽд_�v",
	ptime->tm_year - 70, ptime->tm_yday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);
      vmsg(msg);
    }
  }

  return XEASY;
}


/* ----------------------------------------------------- */
/* �ӤH�u��						 */
/* ----------------------------------------------------- */


int
u_info()
{
  char *str, username[UNLEN + 1];

  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  move(1, 0); 
  strcpy(username, str = cuser.username);
  acct_setup(&cuser, 0);
  if (strcmp(username, str))
    memcpy(cutmp->username, str, UNLEN + 1);
  return 0;
}


int
u_sign_set()	/*ikulan.080726:�N�ﯸñ�\��W�ߥX��*/
{
   FILE *hello;		/* smiler.071030: ��ñ�ӤH�Ƥ��ϥΪ̷Q��j�a������ */
   FILE *file_host;	/* smiler.071110: �ӤH��ܯ�ñ */
   int  choice;		/* ��βĴX�ӯ�ñ */
   char buf[ANSILINELEN];
   char helloworld[38];

   /*ikulan.080727: �L�X��ñ*/
   FILE *host_sign;

#define HOST_SIGN_NUM	4	/*ikulan.080727: ��ñ���ƥءA�����|�ӯ�ñ*/

   move(1, 0);
   clrtobot();

   /* smiler.071030: ��J�ϥΪ̷Q��j�a������ */
   usr_fpath(buf, cuser.userid, "hello");
   if (hello = fopen(buf, "r"))
   {
     fgets(helloworld, 38, hello);
     fclose(hello);
     unlink(buf);
   }
   else
     helloworld[0] = '\0';

   if(!vget(9, 0, "�Q��j�a�����ܡG", helloworld, 38, GCARRY))
   {
     if(!hello)
     {
       hello = fopen(buf, "w");
       fprintf(hello,"�w��j�a�h�ӷ����}�}\\(*�á`��*)/");
       fclose(hello);
     }
   }
   else
   {
     hello = fopen(buf, "w");
     fputs(helloworld, hello);
     fclose(hello);
   }

   usr_fpath(buf, cuser.userid, "host");
   if (file_host = fopen(buf,"r"))
   {
     fgets(buf, 3, file_host);
     fclose(file_host);
   }
   choice = atoi(buf);

   vmsg(NULL);
   move(1,0);
   clrtobot();

   int i;
   for(i = 1; i <= HOST_SIGN_NUM; i++)
   {
     prints("\n%d����ñ�G\n", i);
     sprintf(buf, "gem/@/@host_%d", i - 1);
     if(host_sign = fopen(buf, "r"))
     {
       while(fgets(buf, ANSILINELEN, host_sign))
         prints("%s", buf);
       fclose(host_sign);
     }
   }

   sprintf(buf, "�� ��� 0)�H�� 1~4)��ܯ�ñ [%d] ", choice);
   choice = vans(buf);
   if (choice >= '0' && choice <= HOST_SIGN_NUM + '0')
     choice -= '0';
   else
     return 0;

   usr_fpath(buf, cuser.userid, "host");
   if (file_host = fopen(buf, "w"))
   {
     fprintf(file_host, "%d", choice);
     fclose(file_host);
   }

}	/*ikulan.080726:(end of function)�N�ﯸñ�\��W�ߥX��*/


int
u_setup()
{
  usint ulevel;
  int len;

  /* itoc.000320: �W��حn��� len �j�p, �]�O�ѤF�� ufo.h ���X�� STR_UFO */

  ulevel = cuser.userlevel;
  if (!ulevel)
    len = NUMUFOS_GUEST;
  else if (ulevel & PERM_ALLADMIN)
    len = NUMUFOS;		/* ADMIN ���F�i�� acl�A�ٶ��K�]�i�H�������N */
  else if (ulevel & PERM_CLOAK)
    len = NUMUFOS - 2;		/* ����ε����Bacl */
  else
    len = NUMUFOS_USER;

  cuser.ufo = cutmp->ufo = bitset(cuser.ufo, len, len, MSG_USERUFO, ufo_tbl);

  return 0;
}

int
u_usr_show_set()
{
  FILE *fp;
  char filepath[64];
  int len;

  len = NUM_USR_SHOW;
  USR_SHOW = bitset(USR_SHOW, len, len, MSG_USR_SHOW, usr_show_tbl);

  usr_fpath(filepath, cuser.userid, "MY_USR_SHOW");
  fp = fopen(filepath, "w");
  fprintf(fp, "%ud", USR_SHOW);
  fclose(fp);

  return 0;
}

int
u_lock()
{
  char buf[PSWDLEN + 1];

  switch (vans("�O�_�i�J�ù���w���A�A�N����ǰe/�������y(Y/N/C)�H[N] "))
  {
  case 'c':		/* itoc.011226: �i�ۦ��J�o�b���z�� */
    if (vget(b_lines, 0, "�п�J�o�b���z�ѡG", cutmp->mateid, IDLEN + 1, DOECHO))
      break;

  case 'y':
    strcpy(cutmp->mateid, "����");
    break;

  default:
    return XEASY;
  }

  bbstate |= STAT_LOCK;		/* lkchu.990513: ��w�ɤ��i�^���y */
  cutmp->status |= STATUS_REJECT;	/* ��w�ɤ������y */

  clear();
  move(5, 20);
  prints("\033[1;33m" BBSNAME "    ���m/��w���A\033[m  [%s]", cuser.userid);

  do
  {
    vget(b_lines, 0, "�� �п�J�K�X�A�H�Ѱ��ù���w�G", buf, PSWDLEN + 1, NOECHO);
  } while (chkpasswd(cuser.passwd, buf));

  cutmp->status ^= STATUS_REJECT;
  bbstate ^= STAT_LOCK;

  return 0;
}


int
u_log()
{
  char fpath[64];

  usr_fpath(fpath, cuser.userid, FN_LOG);
  more(fpath, NULL);
  return 0;
}


/* ----------------------------------------------------- */
/* �]�w�ɮ�						 */
/* ----------------------------------------------------- */


/* static */			/* itoc.010110: �� a_xfile() �� */
void
x_file(mode, xlist, flist)
  int mode;			/* M_XFILES / M_UFILES */
  char *xlist[];		/* description list */
  char *flist[];		/* filename list */
{
  int n, i;
  char *fpath, *desc;
  char buf[64];

  //move(MENU_XPOS, 0);
  move(1, 0);
  clrtobot();
  n = 0;
  while (desc = xlist[n])
  {
    n++;
    if (n <= 18)			/* itoc.020123: ���G��A�@��Q�K�� */
    {
      move(n + 1 - 1, 0);
      //prints("\033[m  (%d)",n);         /* smiler.071112: �ץ�note��X�h�X�� */
      clrtoeol();
      //move(n + 1 - 1, 2);
    }
    else
    {
      move(n + 1 - 19, 40);
    }
    prints("\033[m  (%d) %s", n, desc);

    if (mode == M_XFILES)	/* Thor.980806.����: �L�X�ɦW */
    {
      if (n <= 18)
	move(n + 1 - 1, 22);
      else
	move(n + 1 - 19, 62);
      outs(flist[n - 1]);
    }
  }

  vget(b_lines, 0, "�п���ɮ׽s���A�Ϋ� [0] �����G", buf, 3, DOECHO);
  i = atoi(buf);
  if (i <= 0 || i > n)
    return;

  n = vget(b_lines, 36, "(D)�R�� (E)�s�� [Q]�����H", buf, 3, LCECHO);
  if (n != 'd' && n != 'e')
    return;

  fpath = flist[--i];
  if (mode == M_UFILES)
    usr_fpath(buf, cuser.userid, fpath);
  else			/* M_XFILES */
    strcpy(buf, fpath);

  if (n == 'd')
  {
    if (vans(msg_sure_ny) == 'y')
      unlink(buf);
  }
  else
  {
	  if(mode == M_XFILES)
	  {
		char fpath_info[64];
		sprintf(fpath_info, BBSHOME"/gem/@/@mxfile.info");
		more(fpath_info, NULL);
	  }

    vmsg(vedit(buf, 0) ? "��ʤ���" : "��s����");	/* Thor.981020: �`�N�Qtalk�����D  */
  }
}


int
u_xfile()
{
  int i;

  static char *desc[] = 
  {
    "�W���a�I�]�w��",
    "�W����",
    "�Ȧs��.1",
    "�Ȧs��.2",
    "�Ȧs��.3",
    "�Ȧs��.4",
    "�Ȧs��.5",
    "ñ�W��.1",
    "ñ�W��.2",
    "ñ�W��.3",
    "ñ�W��.4",
    "ñ�W��.5",
    "ñ�W��.6",
    "ñ�W��.7",
    "ñ�W��.8",
    "ñ�W��.9",
    NULL
  };

  static char *path[] = 
  {
    "acl",
    "plans",
    "buf.1",
    "buf.2",
    "buf.3",
    "buf.4",
    "buf.5",
    FN_SIGN ".1",
    FN_SIGN ".2",
    FN_SIGN ".3",
    FN_SIGN ".4",
    FN_SIGN ".5",
    FN_SIGN ".6",
    FN_SIGN ".7",
    FN_SIGN ".8",
    FN_SIGN ".9"
  };

  i = HAS_PERM(PERM_ALLADMIN) ? 0 : 1;
  x_file(M_UFILES, &desc[i], &path[i]);
  return 0;
}
