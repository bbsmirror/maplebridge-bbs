/*-------------------------------------------------------*/
/* newbrd.c   ( YZU_CSE WindTop BBS )                    */
/*-------------------------------------------------------*/
/* target : �s�p�\��    			 	 */
/* create : 00/01/02				 	 */
/* update : 02/04/29				 	 */
/*-------------------------------------------------------*/
/* run/newbrd/_/.DIR - newbrd control header		 */
/* run/newbrd/_/@/@_ - newbrd description file		 */
/* run/newbrd/_/@/G_ - newbrd voted id loG file		 */
/*-------------------------------------------------------*/


#include "bbs.h"


#ifdef HAVE_COSIGN

extern XZ xz[];
extern char xo_pool[];
extern BCACHE *bshm;		/* itoc.010805: �}�s�O�� */

static int nbrd_add();
static int nbrd_body();
static int nbrd_head();

static char *split_line = "\033[33m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w\033[m\n";

static int check_adm;

typedef struct
{
  char userid[IDLEN + 1];
  char email[60];
}      LOG;


static int
cmpbtime(nbrd)
  NBRD *nbrd;
{
  return nbrd->btime == currchrono;
}


static char
nbrd_attr(nbrd)
  NBRD *nbrd;
{
  int xmode = nbrd->mode;

  /* �����V�֪��A�V�ɦV���� */
  if (xmode & NBRD_FINISH)
    return ' ';
  if (xmode & NBRD_END)
    return '-';
#ifdef SYSOP_START_COSIGN
  if (xmode & NBRD_START)
    return '+';
  else
    return 'x';
#else
  return '+';
#endif
}


static int
nbrd_stamp(folder, nbrd, fpath)
  char *folder;
  NBRD *nbrd;
  char *fpath;
{
  char *fname;
  char *family = NULL;
  int rc;
  int token;

  fname = fpath;
  while (rc = *folder++)
  {
    *fname++ = rc;
    if (rc == '/')
      family = fname;
  }

  fname = family;
  *family++ = '@';

  token = time(0);

  archiv32(token, family);

  rc = open(fpath, O_WRONLY | O_CREAT | O_EXCL, 0600);
  nbrd->btime = token;
  str_stamp(nbrd->date, &nbrd->btime);
  strcpy(nbrd->xname, fname);

  return rc;
}


static void
nbrd_fpath(fpath, folder, nbrd)
  char *fpath;
  char *folder;
  NBRD *nbrd;
{
  char *str;
  int cc;

  while (cc = *folder++)
  {
    *fpath++ = cc;
    if (cc == '/')
      str = fpath;
  }
  strcpy(str, nbrd->xname);
}


static int
nbrd_init(xo)
  XO *xo;
{
  xo_load(xo, sizeof(NBRD));
  return nbrd_head(xo);
}


static int
nbrd_load(xo)
  XO *xo;
{
  xo_load(xo, sizeof(NBRD));
  return nbrd_body(xo);
}


static void
nbrd_item(num, nbrd)
  int num;
  NBRD *nbrd;
{
  prints("%6d %c %-5s %-13s [%-13s] %.*s\n", 
    num, nbrd_attr(nbrd), nbrd->date + 3, nbrd->owner, 
    (nbrd->mode & NBRD_NEWBOARD) ? nbrd->brdname : "\033[1;33m�����s�p\033[m", d_cols + 20, nbrd->title);
}


#ifdef HAVE_LIGHTBAR
static int
nbrd_item_bar(xo, mode)
  XO *xo;
  int mode;     /* 1:�W����  0:�h���� */
{
  NBRD *nbrd;

  nbrd = (NBRD *) xo_pool + xo->pos - xo->top;

  prints("%s%6d %c %-5s %-13s [%-13s%s] %-*.*s%s%s",
    mode ? UCBAR[UCBAR_NBRD] : "",         //�o�̬O���Ϊ��C��A�i�H�ۤv��
    xo->pos + 1, nbrd_attr(nbrd), nbrd->date + 3, nbrd->owner,
    (nbrd->mode & NBRD_NEWBOARD) ? nbrd->brdname : "\033[1;33m�����s�p\033[m",
    mode ? UCBAR[UCBAR_NBRD] : "", d_cols + 22, d_cols + 22, nbrd->title,
    (nbrd->mode & NBRD_NEWBOARD) ? "           " :"                ",
    mode ? "\033[m" : "");

  move(xo->pos - xo->top + 3, 0);
  return XO_NONE;
}
#endif


static int
nbrd_body(xo)
  XO *xo;
{
  NBRD *nbrd;
  int num, max, tail;

  max = xo->max;
  if (max <= 0)
  {
    if (vans("�n�s�W�s�p���ض�(Y/N)�H[N] ") == 'y')
      return nbrd_add(xo);
    return XO_QUIT;
  }

  nbrd = (NBRD *) xo_pool;
  num = xo->top;
  tail = num + XO_TALL;

  if (max > tail)
    max = tail;

  move(3, 0);  
  do
  {
    nbrd_item(++num, nbrd++);
  } while (num < max);
  clrtobot();

  return XO_FOOT;
}


static int
nbrd_head(xo)
  XO *xo;
{
  vs_head("�ӽЬݪO", str_site);
  prints(NECKER_COSIGN, d_cols, "");
  return nbrd_body(xo);
}


static int
nbrd_find(fpath, brdname)
  char *fpath, *brdname;
{
  NBRD old;
  int fd;
  int rc = 0;

  if ((fd = open(fpath, O_RDONLY)) >= 0)
  {
    while (read(fd, &old, sizeof(NBRD)) == sizeof(NBRD))
    {
      if (!str_cmp(old.brdname, brdname) && !(old.mode & NBRD_FINISH))
      {
	rc = 1;
	break;
      }
    }
    close(fd);
  }
  return rc;
}


static int
nbrd_add(xo)
  XO *xo;
{
  int i, fd, ans, days, numbers, ntype, readlevel, isinn;
  char *dir, fpath[64], path[64], buf[32], group[64];
  char class[BCLEN + 1], sub[BNLEN + 1];
  char *brdname, *title, *prefix, *plevel;
  FILE *fp;
  NBRD nbrd;

  if (HAS_PERM(PERM_ALLADMIN))
  {
    ans = vans("�s�p�Ҧ� 1)�}�s�O 2)�O�W 3)�L�O�W�G[Q] ");
    if (ans < '1' || ans > '3')
      return xo->max ? XO_FOOT : nbrd_body(xo);	/* itoc.020122: �p�G�S������s�p�A�n�^�� nbrd_body() */
    /* itoc.030613: ���U���� return XO_FOOT; �]���ӳo�˧� */
  }
  else if (HAS_PERM(PERM_POST))
  {
    /* �@�q�ϥΪ̥u��}�s�O�s�p */
    ans = '1';
  }
  else
  {
    vmsg("�藍�_�A���ݪO�O��Ū��");
    return XO_FOOT;
  }

  memset(&nbrd, 0, sizeof(NBRD));

  brdname = nbrd.brdname;
  title = nbrd.title;

  clear();
  vs_bar("�ӽЬݪO");
  if (ans == '1')	/* �s�O�s�p */
  {
    move (i = 2, 0);
    outs("          \033[1;33m1)\033[m�ӤH\033[1;36m/\033[m���Q     "
	"\033[1;33m2)\033[m��O     \033[1;33m3)\033[m�����\n"
	"          \033[1;33m4)\033[m����\033[1;36m/\033[m�դͷ|   "
	"\033[1;33m5)\033[m�U�t(��)�Ҭ���|���ʪO\033[1;36m/\033[m�x��ҵ{�O\n"
	"          \033[1;33m6)\033[m�ն�\033[1;36m/\033[m�հȬݪO "
	"\033[1;33m7)\033[m����O\033[1;36m/\033[m�ҵ{�p�հQ�תO "
	"\033[1;33m8)\033[m��X\033[1;36m/\033[m��L\n");
    i += 3;
    if (!vget(i, 0, "�}�O�����H[Q] ", buf, 3, LCECHO))
      return XO_HEAD;

    switch (ntype = (*buf - '0'))
    {
    case 1:
      plevel = "�ӤH";
      prefix = "P_";
      break;
    case 2:
      plevel = "��O";
      prefix = "R_";
      break;
    case 3:
      plevel = "�֧B";
      prefix = "LAB_";
      break;
    case 4:
      plevel = "����";
      prefix = "";
      move(++i, 0);
      outs("\n                  1. [���ΥD�n�ݪO] �^��O�W�����Gxxxxxxxxxxxx\n");
      outs("                  2. �Y�z�Q�ӽЪ��Ϊ����ݬݪO�A\n");
      outs("                         �O�W�����G[�^���Y�g].xxxxxxx�A\n");
      outs("                         [���Τ���²��]�B[�^���Y�g] �ж�g�@�P�������W��\n");
      i += 4;
      break;
    case 5:
      plevel = prefix = "";	/* �������ۤv��ʧ� */
      move(++i, 0);
      outs("\n                �t(��)�O�G[�t�W].[�t��]        �ҡGcs.12 �M�j��u 12 �ůŪO\n");
      outs("                  ���ʪO�G[�t�W].xxxxxx        �ҡGcs.camp09 ��u2009�綤�O\n");
      outs("                  �ҵ{�O�G�H�ҰȲդ��Ҹ����O�W �ҡGisa5571  (���Юv�ΧU�ж}�O)\n");
      i += 3;
      break;
    case 6:
      plevel = prefix = "";
      move(++i, 0);
      outs("\n                  �ն��ݪO�G nthu_xxxxxxx\n");
      outs("                  �հȬݪO�G nthu.xxxxxxx\n");
      i += 2;
      break;
    case 7:
      plevel = "����";
      prefix = "G_";
      move(++i, 0);
      outs("\n                �Y�w�z������w����L�ݪO�A[�������²��] �ж�g�@�P�������W��\n");
      i++;
      break;
    case 8:
      plevel = prefix = "";
      break;
    default:
      return XO_HEAD;
    }

    if (ntype == 4)
    {
      if (vget(++i, 0, "���Τ���²�١G", class, BCLEN + 1, DOECHO) && strlen(class) == 4)
	plevel = class;

      if (vget(++i, 0, "�O�_�n�ӽ� [���ΥD�n�ݪO](y/n)�H[N] ", buf, 3, LCECHO) != 'y')
      {
	if (!vget(++i, 0, "���έ^���Y�g�G", sub, sizeof(nbrd.brdname) - 3, DOECHO))
	  return XO_HEAD;
	strcat(sub, ".");
	prefix = sub;
      }
    }
    else if (ntype == 7)
    {
      if (vget(++i, 0, "�������²�١G", class, BCLEN + 1, DOECHO) && strlen(class) == 4)
	plevel = class;
    }

    sprintf(buf, "�^��O�W�G%s", prefix);
    if (!vget(++i, 0, buf, brdname, sizeof(nbrd.brdname) - strlen(prefix), DOECHO))
      return XO_HEAD;

    sprintf(buf, "%s%s", prefix, brdname);
    if (brd_bno(buf) >= 0 || !valid_brdname(buf))
    {
      vmsg("�w�����O�ΪO�W���X�k");
      return XO_HEAD;
    }
    if (nbrd_find(xo->dir, buf))
    {
      vmsg("�w�����ݪO���ӽЮ�");
      return XO_HEAD;
    }

    strcpy(brdname, buf);
    strcpy(nbrd.class, plevel);

    if (ntype >= 4 && ntype <= 7)
    {
      switch (ntype)
      {
      case 4:
	vget(++i, 0, "�ݪO���ݪ���/���κ����G", group, 50, DOECHO);
	break;
      case 5:
	vget(++i, 0, "�ݪO���ݨt(��)��/����/�ҵ{�G", group, 50, DOECHO);
	break;
      case 6:
	vget(++i, 0, "�ݪO���ݮն�/�ǮճB�ǡG", group, 50, DOECHO);
	break;
      }
      if (!*group)
	return XO_HEAD;
    }

    if (!vget(++i, 0, "����O�W�G", title, sizeof(nbrd.title), DOECHO))
      return XO_HEAD;

    readlevel = vget(++i, 0, "�ݪO�ݩʡG1)���} 2)���K 3)�n�� [Q] ", buf, 3, DOECHO) - '0';
    switch (readlevel)
    {
    case 1:
      plevel = "���}";
      break;
    case 2:
      plevel = "\033[1;31m���K\033[m";
      break;
    case 3:
      plevel = "\033[1;33m�n��\033[m";
      break;
    default:
      return XO_HEAD;
    }

    days = NBRD_DAY_BRD;
    numbers = NBRD_NUM_BRD;

#ifdef SYSOP_START_COSIGN
    nbrd.mode = NBRD_NEWBOARD;
#else
    nbrd.mode = NBRD_NEWBOARD | NBRD_START;
#endif
  }
  else			/* ��L�s�p */
  {
    char tmp[8];

    if (!vget(b_lines, 0, "�s�p�D�D�G", title, sizeof(nbrd.title), DOECHO))
      return XO_FOOT;

    /* �s�p����̦h 30 �ѡA�s�p�H�Ƴ̦h 500 �H */
    if (!vget(b_lines, 0, "�s�p�ѼơG", tmp, 5, DOECHO))
      return XO_FOOT;
    days = atoi(tmp);
    if (days > 30 || days < 1)
      return XO_FOOT;
    if (!vget(b_lines, 0, "�s�p�H�ơG", tmp, 6, DOECHO))
      return XO_FOOT;
    numbers = atoi(tmp);
    if (numbers > 500 || numbers < 1)
      return XO_FOOT;

    nbrd.mode = (ans == '2') ? (NBRD_OTHER | NBRD_START) : (NBRD_OTHER | NBRD_START | NBRD_ANONYMOUS);
  }

  if (vget(++i, 0, "�O�_�ӽ���H(Y/N)�H[N] ", buf, 3, LCECHO) == 'y')
  {
    isinn = 1;
    strcpy(nbrd.innsrv, "group.nctu.edu.tw");
    strcpy(nbrd.inngrp, "group.nthucs.");
    if (!vget(++i, 0, "��H���x�W�١G", nbrd.innsrv, sizeof(nbrd.innsrv), GCARRY) ||
      !vget(++i, 0, "��H�s�զW�١G", nbrd.inngrp, sizeof(nbrd.inngrp), GCARRY) ||
      !str_cmp(nbrd.inngrp, "group.nthucs."))
    {
      vmsg("�����]�w��H");
      isinn = 0;
      nbrd.innsrv[0] = nbrd.inngrp[0] = '\0';
    }
  }
  else
    isinn = 0;

  if (isinn)
    nbrd.mode |= NBRD_INN;

  if ((ntype >= 4 && ntype <= 6) || ntype == 8)
    nbrd.mode |= NBRD_PUBLIC;

  vmsg("�}�l�s�� [�ݪO�����P�O�D��t]�A���ݦA�N�ӽЬݪO����ƶK�W�@���I");
  sprintf(path, "tmp/%s.nbrd", cuser.userid);	/* �s�p��]���Ȧs�ɮ� */
  if (fd = vedit(path, 0))
  {
    unlink(path);
    vmsg(msg_cancel);
    return nbrd_head(xo);
  }

  dir = xo->dir;
  if ((fd = nbrd_stamp(dir, &nbrd, fpath)) < 0)
    return nbrd_head(xo);
  close(fd);

  if (ans != '1' || ntype >= 7)
  {
    nbrd.etime = nbrd.btime + days * 86400;
    nbrd.mode = NBRD_NEWBOARD;
    nbrd.total = (ntype == 8) ? 20 : numbers;
  }
  else
  {
    nbrd.mode |= NBRD_END;	/* ���γs�p�A�ӽдN�}���ݪO */
  }

  nbrd.readlevel = readlevel;
  strcpy(nbrd.owner, cuser.userid);

  fp = fopen(fpath, "a");
  fprintf(fp, "�@��: %s (%s) ����: newboard\n", cuser.userid, cuser.username);
  if (ans == '1')
    fprintf(fp, "���D: [����] �ӽ� %s �ݪO\n", brdname);
  else
    fprintf(fp, "���D: %s\n", title);
  fprintf(fp, "�ɶ�: %s\n\n", Now());

  if (ans == '1')
  {
    fprintf(fp, "�^��O�W�G%s\n", brdname);
    fprintf(fp, "����O�W�G%s\n", title);
    fprintf(fp, "�O�D�W��G%s\n", cuser.userid);
    fprintf(fp, "�q�l�H�c�G%s\n", cuser.email);
    fprintf(fp, "�ݪO�����G%s\n", nbrd.class);
    if (ntype >= 4 && ntype <= 7)
    {
      switch (ntype)
      {
      case 4:
	fprintf(fp, "    ���ݪ���/���κ����G%s\n", group);
	break;
      case 5:
	fprintf(fp, "    ���ݨt(��)��/����/�ҵ{�G%s\n", group);
	break;
      case 6:
	fprintf(fp, "    ���ݮն�/�ǮճB�ǡG%s\n", group);
	break;
      }
    }
    fprintf(fp, "�ݪO�ݩʡG%s\n", plevel);
    if (isinn)
    {
      fprintf(fp, "�ӽ���H�G\n    ���x�G%s\n", nbrd.innsrv);
      fprintf(fp, "    �s�աG%s\n", nbrd.inngrp);
    }
  }
  else
  {
    fprintf(fp, "�s�p�D�D�G%s\n", title);
  }

  if (ans != '1' || ntype >= 7)
  {
    fprintf(fp, "�|�����G%s\n", nbrd.date);
    fprintf(fp, "����ѼơG%d\n", days);
    fprintf(fp, "�ݳs�p�H�G%d\n", numbers);
    fprintf(fp, split_line);
    fprintf(fp, "�s�p�����G\n");
  }

  f_suck(fp, path);
  unlink(path);
  fprintf(fp, "\n--\n�� ���峹�� %s �q %s �ӽ�\n", cuser.userid, fromhost);
  if (ans != '1' || ntype >= 7)
    fprintf(fp, split_line);
  fclose(fp);

  rec_add(dir, &nbrd, sizeof(NBRD));

#ifdef SYSOP_START_COSIGN
  vmsg(ans == '1' ? "�e��ӽФF�A�е��Ԯ֭�a" : "�s�p�}�l�F�I");
#else
  if (ans != '1' || ntype >= 7)
    vmsg("�s�p�}�l�F�I");
  else
    vmsg("�е��ݯ����}�O�I");
#endif
  return nbrd_init(xo);
}


static int
nbrd_seek(fpath)
  char *fpath;
{
  LOG old;
  int fd;
  int rc = 0;

  if ((fd = open(fpath, O_RDONLY)) >= 0)
  {
    while (read(fd, &old, sizeof(LOG)) == sizeof(LOG))
    {
      if (!strcmp(old.userid, cuser.userid) || !str_cmp(old.email, cuser.email))
      {
	rc = 1;
	break;
      }
    }
    close(fd);
  }
  return rc;
}


static void
addreply(hdd, ram)
  NBRD *hdd, *ram;
{
  if (--hdd->total <= 0)
  {
    if (hdd->mode & NBRD_NEWBOARD)	/* �s�O�s�p�� END */
      hdd->mode |= NBRD_END;
    else				/* ��L�s�p�� FINISH */
      hdd->mode |= NBRD_FINISH;
  }
}


static int
nbrd_reply(xo)
  XO *xo;
{
  NBRD *nbrd;
  char *fname, fpath[64], reason[80];
  LOG mail;

  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);
  fname = NULL;

  if (nbrd->mode & (NBRD_FINISH | NBRD_END))
    return XO_NONE;

#ifdef SYSOP_START_COSIGN
  if (!(nbrd->mode & NBRD_START))
  {
    vmsg("�|���}�l�s�p");
    return XO_FOOT;
  }
#endif

  if (time(0) >= nbrd->etime)
  {
    currchrono = nbrd->btime;
    if (nbrd->mode & NBRD_NEWBOARD)	/* �s�O�s�p�� END */
    {
      if (!(nbrd->mode & NBRD_END))
      {
	nbrd->mode ^= NBRD_END;
	currchrono = nbrd->btime;
	rec_put(xo->dir, nbrd, sizeof(NBRD), xo->pos, cmpbtime);
      }
    }
    else				/* ��L�s�p�� FINISH */
    {
      if (!(nbrd->mode & NBRD_FINISH))
      {
	nbrd->mode ^= NBRD_FINISH;
	currchrono = nbrd->btime;
	rec_put(xo->dir, nbrd, sizeof(NBRD), xo->pos, cmpbtime);
      }
    }
    vmsg("�s�p�w�g�I��F");
    return XO_FOOT;
  }


  /* --------------------------------------------------- */
  /* �ˬd�O�_�w�g�s�p�L					 */
  /* --------------------------------------------------- */

  nbrd_fpath(fpath, xo->dir, nbrd);
  fname = strrchr(fpath, '@');
  *fname = 'G';

  if (nbrd_seek(fpath))
  {
    vmsg("�z�w�g�s�p�L�F�I");
    return XO_FOOT;
  }

  /* --------------------------------------------------- */
  /* �}�l�s�p						 */
  /* --------------------------------------------------- */

  *fname = '@';

  if (vans("�n�[�J�s�p��(Y/N)�H[N] ") == 'y' && 
    vget(b_lines, 0, "�ڦ��ܭn���G", reason, 65, DOECHO))
  {
    FILE *fp;

    if (fp = fopen(fpath, "a"))
    {
      if (nbrd->mode & NBRD_ANONYMOUS)
	fprintf(fp, "%3d -> " STR_ANONYMOUS "\n    %s\n", nbrd->total, reason);
      else
	fprintf(fp, "%3d -> %s (%s)\n    %s\n", nbrd->total, cuser.userid, cuser.email, reason);
      fclose(fp);
    }

    currchrono = nbrd->btime;
    rec_ref(xo->dir, nbrd, sizeof(NBRD), xo->pos, cmpbtime, addreply);

    memset(&mail, 0, sizeof(LOG));
    strcpy(mail.userid, cuser.userid);
    strcpy(mail.email, cuser.email);
    *fname = 'G';
    rec_add(fpath, &mail, sizeof(LOG));

    vmsg("�[�J�s�p����");
    return nbrd_init(xo);
  }

  return XO_FOOT;
}


#ifdef SYSOP_START_COSIGN
static int
nbrd_start(xo)
  XO *xo;
{
  NBRD *nbrd;
  char fpath[64], buf[80], tmp[10];
  time_t etime;

  if (!HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;

  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);

  if (nbrd->mode & (NBRD_FINISH | NBRD_END | NBRD_START))
    return XO_NONE;

  if (vans("�нT�w�}�l�s�p(Y/N)�H[N] ") != 'y')
    return XO_FOOT;

  nbrd_fpath(fpath, xo->dir, nbrd);
  etime = time(0) + NBRD_DAY_BRD * 86400;

  str_stamp(tmp, &etime);
  sprintf(buf, "�}�l�s�p�G      �������G%s\n", tmp);
  f_cat(fpath, buf);
  f_cat(fpath, split_line);

  nbrd->etime = etime;
  nbrd->mode ^= NBRD_START;
  currchrono = nbrd->btime;
  rec_put(xo->dir, nbrd, sizeof(NBRD), xo->pos, cmpbtime);

  return nbrd_head(xo);
}
#endif


static int
nbrd_finish(xo)
  XO *xo;
{
  NBRD *nbrd;
  char fpath[64], path[64];
  int fd;
  FILE *fp;

  if (!HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;

  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);

  if (nbrd->mode & NBRD_FINISH)
    return XO_NONE;

  if (vans("�нT�w�����s�p(Y/N)�H[N] ") != 'y')
    return XO_FOOT;

  vmsg("�нs�赲���s�p��]");
  sprintf(path, "tmp/%s", cuser.userid);	/* �s�p��]���Ȧs�ɮ� */
  if (fd = vedit(path, 0))
  {
    unlink(path);
    vmsg(msg_cancel);
    return nbrd_head(xo);
  }

  nbrd_fpath(fpath, xo->dir, nbrd);

  f_cat(fpath, "�����s�p��]�G\n\n");
  fp = fopen(fpath, "a");
  f_suck(fp, path);
  fclose(fp);
  f_cat(fpath, split_line);
  unlink(path);

  nbrd->mode ^= NBRD_FINISH;
  currchrono = nbrd->btime;
  rec_put(xo->dir, nbrd, sizeof(NBRD), xo->pos, cmpbtime);

  return nbrd_head(xo);
}


static void
add_class(brd, class_name)
  BRD *brd;
  char *class_name;
{
  HDR hdr;
  char fpath[64];

  sprintf(fpath, "gem/@/@%s", class_name);

  /* �[�J�A������ */
  brd2gem(brd, &hdr);
  rec_add(fpath, &hdr, sizeof(HDR));
}


static int			/* 1:�}�O���\ */
nbrd_newbrd(xo, nbrd)		/* �}�s�O */
  XO *xo;
  NBRD *nbrd;
{
  BRD newboard;
  ACCT acct;

  /* itoc.030519: �קK���ж}�O */
  if (brd_bno(nbrd->brdname) >= 0)
  {
    vmsg("�w�����O");
    return 1;
  }

  memset(&newboard, 0, sizeof(BRD));

  /* itoc.010805: �s�ݪO�w�] battr = ����H; postlevel = PERM_POST; �ݪO�O�D�����_�s�p�� */
  /* itoc.010805: �s�ݪO�w�] battr = ����H; �ݪO�O�D�����_�s�p�� */
  switch (nbrd->readlevel)
  {
  case 2:	/* ���K */
    newboard.readlevel = PERM_SYSOP;
    newboard.battr = BRD_NOTRAN | BRD_NOSTAT | BRD_NOVOTE;
    break;
  case 3:	/* �n�� */
    newboard.readlevel = PERM_BOARD;
    newboard.battr = BRD_NOTRAN | BRD_NOSTAT | BRD_NOVOTE;
    break;
  default:	/* ���} */
    newboard.postlevel = PERM_POST;
    newboard.battr = BRD_NOTRAN;
    break;
  }
  if (nbrd->mode & NBRD_PUBLIC)
    newboard.battr = BRD_PUBLIC;
  newboard.postlevel = PERM_POST;
  strcpy(newboard.brdname, nbrd->brdname);
  strcpy(newboard.class, nbrd->class);
  strcpy(newboard.title, nbrd->title);
  strcpy(newboard.BM, nbrd->owner);

  if (acct_load(&acct, nbrd->owner) >= 0)
    acct_setperm(&acct, PERM_BM, 0);

  if (brd_new(&newboard) < 0)
    return 0;

  /* �[�J�����s�� */
  add_class(&newboard, "NewBoard");

  vmsg("�s�O���ߡA�{�b�[�J�����s��");

  XoGem("gem/@/@Class", "�ݪO���", (GEM_W_BIT | GEM_X_BIT | GEM_M_BIT));
  nbrd_init(xo);

  return 1;
}


static int	/* 1:���\ 0:���� */
nbrd_nf_add(nbrd)
  NBRD *nbrd;
{
  nodelist_t nl;
  newsfeeds_t nf;
  int fd, high;
  char fpath[32], ans[12];
  BRD *brd;

  strcpy(fpath, "innd/newsfeeds.bbs");
  memset(&nf, 0, sizeof(newsfeeds_t));

  /* ��X��H���x�b nodelist.bbs ������T */
  if ((fd = open("innd/nodelist.bbs", O_RDONLY)) >= 0)
  {
    while (read(fd, &nl, sizeof(nodelist_t)) == sizeof(nodelist_t))
    {
      if (!strcmp(nl.host, nbrd->innsrv))
      {
	strcpy(nf.path, nl.name);
	break;
      }
    }
    close(fd);
  }

  nf.high = INT_MAX;		/* �Ĥ@�����H�j�� reload */
  strcpy(nf.board, nbrd->brdname);
  strcpy(nf.newsgroup, nbrd->inngrp);

  if (!*nf.path)
  {
    vmsg("��H�]�w���x�|���Q�]�w�b newsfeeds.bbs ���A�Ф�ʳ]�w��H�I");
    return 0;
  }

  if (vget(b_lines, 0, "�^�寸�W�G", nf.path, sizeof(nf.path), GCARRY) &&
    vget(b_lines, 0, "�s�աG", nf.newsgroup, /* sizeof(nf.newsgroup) */ 70, GCARRY))
  {
    if (!vget(b_lines, 0, "�r�� [" MYCHARSET "]�G", nf.charset, sizeof(nf.charset), GCARRY))
      str_ncpy(nf.charset, MYCHARSET, sizeof(nf.charset));
    nf.xmode = (vans("�O�_��i(Y/N)�H[Y] ") == 'n') ? INN_NOINCOME : 0;

    if (vans("�O�_�����H�� high-number �]�w�A�o�]�w��Q���H���s�յL��(Y/N)�H[N] ") == 'y')
    {
      sprintf(ans, "%d", nf.high);
      vget(b_lines, 0, "�ثe�g�ơG", ans, 11, GCARRY);
      if ((high = atoi(ans)) >= 0)
	nf.high = high;
    }

    rec_add(fpath, &nf, sizeof(newsfeeds_t));

    high = brd_bno(nf.board);
    brd = bshm->bcache + high;
    if ((brd->battr & BRD_NOTRAN) && vans("���O�ݩʥثe������X�A�O�_�אּ��X(Y/N)�H[Y] ") != 'n')
    {
      brd->battr &= ~BRD_NOTRAN;
      rec_put(FN_BRD, brd, sizeof(BRD), high, NULL);
    }

    return 1;
  }
  return 0;
}


static void
personal_bm_list(userid)			/* ��� userid �O���ǭӤH�O���O�D */
  char *userid;
{
  int len;
  char *list;
  BRD *bhead, *btail;

  len = strlen(userid);
  outs("  \033[32m����ӤH�O�D�G\033[37m");	/* itoc.010922: �� user info ���� */

  bhead = bshm->bcache;
  btail = bhead + bshm->number;

  do
  {
    if (str_ncmp(bhead->brdname, "P_", 2))
      continue;

    list = bhead->BM;
    if (str_has(list, userid, len))
    {
      outs(bhead->brdname);
      outc(' ');
    }
  } while (++bhead < btail);

  outc('\n');
}


static int
nbrd_open(xo)		/* itoc.010805: �}�s�O�s�p�A�s�p�����}�s�ݪO */
  XO *xo;
{
  NBRD *nbrd;
  char fpath[64], title[TTLEN + 1], buf[256];
  time_t now;
  struct tm *ptime;

  if (!HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;

  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);

  if (nbrd->mode & NBRD_FINISH || !(nbrd->mode & NBRD_NEWBOARD))
    return XO_NONE;

  if (!check_adm)
  {
    if (!adm_check())
      return XO_FOOT;
    check_adm = 1;
  }

  if (vans("�нT�w�}�ҬݪO(Y/N)�H[N] ") == 'y')
  {
    if (!str_ncmp(nbrd->brdname, "P_", 2))	/* �������ˬd�ӽФH�w������ǬݪO�O�D */
    {
      clear();
      move(1, 0);
      personal_bm_list(nbrd->owner);
      if (vans("�O�_�~��(Y/N)�H[N] ") != 'y')
	return nbrd_init(xo);
    }

    if (nbrd_newbrd(xo, nbrd))
    {
      nbrd->mode ^= NBRD_FINISH;
      currchrono = nbrd->btime;
      rec_put(xo->dir, nbrd, sizeof(NBRD), xo->pos, cmpbtime);

      if ((nbrd->mode & NBRD_INN) && vans("�O�_�@�ֳ]�w�ݪO��H(Y/N)�H[Y] ") != 'n')
	nbrd_nf_add(nbrd);

      time(&now);
      ptime = localtime(&now);
      sprintf(buf, "\033[1;30m== %s\033[m�G\033[1;33m%-*s\033[1;30m%02d/%02d %02d:%02d:%02d\033[m\n",
         cuser.userid, 56 - strlen(cuser.userid), "�ӽЮ׳q�L",
      ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);
      nbrd_fpath(fpath, xo->dir, nbrd);
      f_cat(fpath, buf);

      sprintf(title, "[�}�O] %s", nbrd->brdname);
      add_post("newboard", fpath, title, nbrd->owner, "", POST_MARKED | POST_SCORE, NULL);
    }
    return nbrd_init(xo);
  }

  return XO_FOOT;
}


static int
nbrd_browse(xo)
  XO *xo;
{
  int key;
  NBRD *nbrd;
  char fpath[80];

  /* itoc.010304: ���F���\Ū��@�b�]�i�H�[�J�s�p�A�Ҽ{ more �Ǧ^�� */
  for (;;)
  {
    nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);
    nbrd_fpath(fpath, xo->dir, nbrd);

    if ((key = more(fpath, FOOTER_COSIGN)) < 0)
      break;

    if (!key)
      key = vkey();

    switch (key)
    {
    case KEY_UP:
    case KEY_PGUP:
    case '[':
    case 'k':
      key = xo->pos - 1;

      if (key < 0)
        break;

      xo->pos = key;

      if (key <= xo->top)
      {
	xo->top = (key / XO_TALL) * XO_TALL;
	nbrd_load(xo);
      }
      continue;

    case KEY_DOWN:
    case KEY_PGDN:
    case ']':
    case 'j':
    case ' ':
      key = xo->pos + 1;

      if (key >= xo->max)
        break;

      xo->pos = key;

      if (key >= xo->top + XO_TALL)
      {
	xo->top = (key / XO_TALL) * XO_TALL;
	nbrd_load(xo);
      }
      continue;

    case 'y':
    case 'r':
      nbrd_reply(xo);
      break;

    case 'h':
      xo_help("cosign");
      break;
    }
    break;
  }

  return nbrd_head(xo);
}


static int
nbrd_delete(xo)
  XO *xo;
{
  NBRD *nbrd;
  char *fname, fpath[80];
  char *list = "@G";		/* itoc.����: �M newbrd file */

  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);
  if (strcmp(cuser.userid, nbrd->owner) && !HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;

  if (nbrd->mode & NBRD_FINISH)
    return XO_NONE;

  if (vans(msg_del_ny) != 'y')
    return XO_FOOT;

  nbrd_fpath(fpath, xo->dir, nbrd);
  fname = strrchr(fpath, '@');
  while (*fname = *list++)
  {
    unlink(fpath);	/* Thor: �T�w�W�r�N�� */
  }

  currchrono = nbrd->btime;
  rec_del(xo->dir, sizeof(NBRD), xo->pos, cmpbtime);
  return nbrd_init(xo);
}


static int
nbrd_edit(xo)
  XO *xo;
{
  if (HAS_PERM(PERM_ALLBOARD))
  {
    char fpath[64];
    NBRD *nbrd;

    nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);
    nbrd_fpath(fpath, xo->dir, nbrd);
    vedit(fpath, 0);
    return nbrd_head(xo);
  }

  return XO_NONE;
}


static int
nbrd_setup(xo)
  XO *xo;
{
  int numbers;
  char ans[6];
  NBRD *nbrd, newnh;

  if (!HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;

  vs_bar("�s�p�]�w");
  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);
  memcpy(&newnh, nbrd, sizeof(NBRD));

  prints("�ݪO�W�١G%s\n�ݪO�����G%4.4s %s\n�s�p�o�_�G%s\n",
    newnh.brdname, newnh.class, newnh.title, newnh.owner);
  prints("�}�l�ɶ��G%s\n", Btime(&newnh.btime));
  prints("�����ɶ��G%s\n", Btime(&newnh.etime));
  prints("�ٻݤH�ơG%d\n", newnh.total);

  if (vget(8, 0, "(E)�]�w (Q)�����H[Q] ", ans, 3, LCECHO) == 'e')
  {
    vget(11, 0, MSG_BID, newnh.brdname, BNLEN + 1, GCARRY);
    vget(12, 0, "�ݪO�����G", newnh.class, sizeof(newnh.class), GCARRY);
    vget(13, 0, "�ݪO�D�D�G", newnh.title, sizeof(newnh.title), GCARRY);
    sprintf(ans, "%d", newnh.total);
    vget(14, 0, "�s�p�H�ơG", ans, 6, GCARRY);
    numbers = atoi(ans);
    if (numbers <= 500 && numbers >= 1)
      newnh.total = numbers;

    if (memcmp(&newnh, nbrd, sizeof(newnh)) && vans(msg_sure_ny) == 'y')
    {
      memcpy(nbrd, &newnh, sizeof(NBRD));
      currchrono = nbrd->btime;
      rec_put(xo->dir, nbrd, sizeof(NBRD), xo->pos, cmpbtime);
    }
  }

  return nbrd_head(xo);
}


static int
nbrd_uquery(xo)
  XO *xo;
{
  NBRD *nbrd;

  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);

  move(1, 0);
  clrtobot();
  my_query(nbrd->owner);
  return nbrd_head(xo);
}


static int
nbrd_usetup(xo)
  XO *xo;
{
  NBRD *nbrd;
  ACCT acct;

  if (!HAS_PERM(PERM_ALLACCT))
    return XO_NONE;

  nbrd = (NBRD *) xo_pool + (xo->pos - xo->top);
  if (acct_load(&acct, nbrd->owner) < 0)
    return XO_NONE;

  move(3, 0);
  acct_setup(&acct, 1);
  return nbrd_head(xo);
}


static int
nbrd_help(xo)
  XO *xo;
{
  xo_help("cosign");
  return nbrd_head(xo);
}


static KeyFunc nbrd_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, nbrd_item_bar,
#endif
  XO_INIT, nbrd_init,
  XO_LOAD, nbrd_load,
  XO_HEAD, nbrd_head,
  XO_BODY, nbrd_body,

  'y', nbrd_reply,
  'r', nbrd_browse,
  'o', nbrd_open,
#ifdef SYSOP_START_COSIGN
  's', nbrd_start,
#endif
  'c', nbrd_finish,
  'd', nbrd_delete,
  'E', nbrd_edit,
  'B', nbrd_setup,

  Ctrl('P'), nbrd_add,
  Ctrl('Q'), nbrd_uquery,
  Ctrl('O'), nbrd_usetup,

  'h', nbrd_help
};


int
XoNewBoard()
{
  XO *xo;
  char fpath[64];

  check_adm = 0;
  sprintf(fpath, "run/newbrd/%s", fn_dir);
  xz[XZ_COSIGN - XO_ZONE].xo = xo = xo_new(fpath);
  xz[XZ_COSIGN - XO_ZONE].cb = nbrd_cb;
  xo->key = XZ_COSIGN;
  xo->pos = XO_TAIL;
  xover(XZ_COSIGN);
  free(xo);

  return 0;
}
#endif	/* HAVE_COSIGN */
