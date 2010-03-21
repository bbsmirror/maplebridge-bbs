/*-------------------------------------------------------*/
/* post.c       ( NTHU CS MapleBBS Ver 2.39 )		 */
/*-------------------------------------------------------*/
/* target : bulletin boards' routines		 	 */
/* create : 95/03/29				 	 */
/* update : 96/04/05				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"


extern BCACHE *bshm;
extern XZ xz[];


extern int wordsnum;		/* itoc.010408: �p��峹�r�� */
extern int TagNum;
extern char xo_pool[];
extern char brd_bits[];


#ifdef HAVE_ANONYMOUS
extern char anonymousid[];	/* itoc.010717: �۩w�ΦW ID */
#endif


#ifdef DO_POST_FILTER
static char bbs_dog_str[80];
#endif


int
cmpchrono(hdr)
  HDR *hdr;
{
  return hdr->chrono == currchrono;
}


static int post_show_dog(BPERM *wperm, char *prompt);


static void
post_no_right()
{
  int i;
  usint postlevel;
  BRD *brd;
#ifdef HAVE_MODERATED_BOARD
  BPAL *bpal;
#endif
#ifdef DO_POST_FILTER
  BPERM *wperm;
#endif
  extern char *perm_tbl[];

#ifdef HAVE_MODERATED_BOARD
  bpal = bshm->pcache + currbno;

  if (is_bmate(bpal))	/* �Y�b����W�椤 */
    vmsg("�ܩ�p�A�z�ثe�b�O�D�]�w������W�椺");
  else			/* ���}�ݪO�A��L���v���P�w */
#endif
  {
    brd = bshm->bcache + currbno;
    postlevel = brd->postlevel;
    if (postlevel && !(postlevel & cuser.userlevel))
    {
      for (i = 5; i < 8; i++)
      {
	move(i, 0);
	clrtoeol();
      }
      move(b_lines, 0);
      clrtoeol();

      move(7, 0);
      prints("\n���ݪO�o��ݦܤ־֦��H�U�v�����@�G\n\033[1;31m");
      for (i = 0; i < 32; i++)
      {
	if (postlevel & (1 << i))
	  prints("%s  ", perm_tbl[i]);
      }
      prints("\033[m\n");
    }

#ifdef DO_POST_FILTER
    wperm = bshm->wperm + currbno;
    if (wperm->exist)
    {
      vmsg("�����N���[��[�ݪO�o�孭��]...");
      post_show_dog(wperm, "�o�������󭭨�");
    }
    else
#endif
      vmsg(NULL);
  }
}


#ifdef DO_POST_FILTER

#define	COLOR_NOT_ACP	"\033[1;31m"
#define	COLOR_ACP	"\033[1;37m"	


/* smiler.080830 : �ݪ��� */
static int
IS_BRD_DOG_FOOD(fpath, board)
  char *fpath;
  char *board;
{
  FILE *fp;
  char fpath_filter[64];
  char filter[73];

  brd_fpath(fpath_filter, board, FN_BBSDOG);

  if (!(fp = fopen(fpath_filter, "r")))
    return 0;

  while (fgets(filter, sizeof(filter), fp))
  {
    if (filter[0] == '\0' || filter[0] == '\n')
      continue;
    else
      filter[strlen(filter) - 1] = '\0';

    if (f_str_sub_space_lf(fpath, filter))
    {
      strcpy(bbs_dog_str, filter);
      fclose(fp);
      return 1;
    }
  }

  fclose(fp);
  return 0;

}


static int
IS_BBS_DOG_FOOD(fpath)
  char *fpath;
{
  FILE *fp;
  char fpath_filter[64];
  char filter[73];

  sprintf(fpath_filter, FN_ETC_BBSDOG);

  if (!(fp = fopen(fpath_filter, "r")))
    return 0;

  while (fgets(filter, sizeof(filter), fp))
  {
    if (filter[0] == '\0' || filter[0] == '\n')
      continue;
    else
      filter[strlen(filter) - 1] = '\0';

     if (f_str_sub_all_chr(fpath, filter))
     {
       strcpy(bbs_dog_str, filter);
       fclose(fp);
       return 1;
     }
  }

  fclose(fp);
  return 0;

}


static int	/* 0: �@�륿�`�峹  1: �Q�פU�ӤF */
post_filter(fpath, title)	/* smiler.080830 : �ݪ�����峹���e�L�o */
  char *fpath;		/* file path to be test */
  char *title;
{
  BRD *brd;
  char warn[70];
  char fpath_log[64];
  char content_log[256];

  brd = bshm->bcache + currbno;

  /* smiler.080910: ���ϥΪ̨M�w�O�_�[�JBBS�ݪ����p�e */
  if (((currbattr & BRD_BBS_DOG) || brd->BM[0] <= ' ') && (IS_BBS_DOG_FOOD(fpath)))
  {
    brd_fpath(fpath_log, currboard, FN_BBSDOG_LOG);
    sprintf(content_log, "%s BBS�ݪ����p�e: �峹�H�^����po\n�@��: %s\n���D: %s\n\n",
      Now(), cuser.userid, title);
    f_cat(fpath_log, content_log);

    sprintf(content_log, "%s BBS�ݪ����p�e: �峹�H�^����po\n�@��: %s\n�ݪO: %s\n���D: %s\n�r��: %s\n\n",
      Now(), cuser.userid, currboard, title, bbs_dog_str);
    f_cat(FN_ETC_BBSDOG_LOG, content_log);

    vmsg("�z��post�峹�������������A�Ь��������ȸs");
    sprintf(warn, "[ĵ�i] ���g�峹�������������A�����D�Ь����ȸs");
    mail_self(fpath, cuser.userid, warn, 0);
    sprintf(warn, "[ĵ�i] ���g�峹�������������A�����D�Ь����ȸs\n");
    f_cat(fpath, warn);
    unlink(fpath);
    return 1;
  }

  if (IS_BRD_DOG_FOOD(fpath, currboard))
  {
    brd_fpath(fpath_log, currboard, FN_BBSDOG_LOG);
    sprintf(content_log, "%s �峹���e����: �峹�H�^����po\n�@��: %s\n���D: %s\n\n",
      Now(), cuser.userid, title);
    f_cat(fpath_log, content_log);

    sprintf(content_log, "%s �峹���e����: �峹�H�^����po\n�@��: %s\n�ݪO: %s\n���D: %s\n�r��: %s\n\n",
      Now(), cuser.userid, currboard, title, bbs_dog_str);
    f_cat(FN_ETC_BBSDOG_LOG, content_log);

    vmsg("�z��post�峹�������ݪO�����A�Ь����ݪO�O�D");
    sprintf(warn, "[ĵ�i] ���g�峹���� %s �O�����A�����D�Ь��ݪO�O�D", currboard);
    mail_self(fpath, cuser.userid, warn, 0);
    sprintf(warn, "[ĵ�i] ���g�峹���� %s �O�����A�����D�Ь��ݪO�O�D\n", currboard);
    f_cat(fpath, warn);
    unlink(fpath);
    return 1;
  }
#if 0
  char filter[256];

  while (fgets(filter, sizeof(filter), fp))
  {
    if (strstr(filter, "paperdo@gmail.com") || strstr(filter, "http://paperdo.googlepages.com/index.htm")
	|| strstr(filter, "�פ�N�g") || strstr(filter, "�N�g�פ�"))
      return 1;
  }
#endif

  return 0;
}
#endif


/* ----------------------------------------------------- */
/* ��} innbbsd ��X�H��B�s�u��H���B�z�{��		 */
/* ----------------------------------------------------- */


void
btime_update(bno)
  int bno;
{
  if (bno >= 0)
    (bshm->bcache + bno)->btime = -1;	/* �� class_item() ��s�� */
}


#ifndef HAVE_NETTOOL
static			/* �� enews.c �� */
#endif
void
outgo_post(hdr, board)
  HDR *hdr;
  char *board;
{
  bntp_t bntp;

  memset(&bntp, 0, sizeof(bntp_t));

  if (board)		/* �s�H */
  {
    bntp.chrono = hdr->chrono;
  }
  else			/* cancel */
  {
    bntp.chrono = -1;
    board = currboard;
  }
  strcpy(bntp.board, board);
  strcpy(bntp.xname, hdr->xname);
  strcpy(bntp.owner, hdr->owner);
  strcpy(bntp.nick, hdr->nick);
  strcpy(bntp.title, hdr->title);
  rec_add("innd/out.bntp", &bntp, sizeof(bntp_t));
}


void
cancel_post(hdr)
  HDR *hdr;
{
  if ((hdr->xmode & POST_OUTGO) &&		/* �~��H�� */
    (hdr->chrono > ap_start - 7 * 86400))	/* 7 �Ѥ������� */
  {
    outgo_post(hdr, NULL);
  }
}


static inline int		/* �^�Ǥ峹 size �h���� */
move_post(hdr, folder, by_bm)	/* �N hdr �q folder �h��O���O */
  HDR *hdr;
  char *folder;
  int by_bm;
{
  HDR post;
  int xmode;
  char fpath[64], fnew[64], *board;
  struct stat st;

  xmode = hdr->xmode;
  hdr_fpath(fpath, folder, hdr);

  if (!(xmode & POST_BOTTOM))	/* �m����Q�夣�� move_post */
  {
#ifdef HAVE_REFUSEMARK
    board = by_bm && !(xmode & POST_RESTRICT) ? BN_DELETED : BN_JUNK;	/* �[�K�峹��h junk */
#else
    board = by_bm ? BN_DELETED : BN_JUNK;
#endif

    brd_fpath(fnew, board, fn_dir);
    hdr_stamp(fnew, HDR_LINK | 'A', &post, fpath);

    /* �����ƻs trailing data�Gowner(�t)�H�U�Ҧ���� */

    memcpy(post.owner, hdr->owner, sizeof(HDR) -
      (sizeof(post.chrono) + sizeof(post.xmode) + sizeof(post.xid) + sizeof(post.xname)));

    if (by_bm)
      sprintf(post.title, "%-13s%.59s", cuser.userid, hdr->title);

    rec_bot(fnew, &post, sizeof(HDR));
    btime_update(brd_bno(board));
  }

  by_bm = stat(fpath, &st) ? 0 : st.st_size;

  unlink(fpath);
  btime_update(currbno);
  cancel_post(hdr);

  return by_bm;
}


#ifdef HAVE_DETECT_CROSSPOST
/* ----------------------------------------------------- */
/* ��} cross post ���v					 */
/* ----------------------------------------------------- */


#define MAX_CHECKSUM_POST	20	/* �O���̪� 20 �g�峹�� checksum */
#define MAX_CHECKSUM_LINE	6	/* �u���峹�e 6 ��Ӻ� checksum */


typedef struct
{
  int sum;			/* �峹�� checksum */
  int total;			/* ���峹�w�o��X�g */
}      CHECKSUM;


static CHECKSUM checksum[MAX_CHECKSUM_POST];
static int checknum = 0;


static inline int
checksum_add(str)		/* �^�ǥ��C��r�� checksum */
  char *str;
{
  int i, len, sum;

  len = strlen(str);

  sum = len;	/* ��r�ƤӤ֮ɡA�e�|�����@�ܥi�৹���ۦP�A�ҥH�N�r�Ƥ]�[�J sum �� */
  for (i = len >> 2; i > 0; i--)	/* �u��e�|�����@�r���� sum �� */
    sum += *str++;

  return sum;
}


static inline int		/* 1:�Ocross-post 0:���Ocross-post */
checksum_put(sum)
  int sum;
{
  int i;

  if (sum)
  {
    for (i = 0; i < MAX_CHECKSUM_POST; i++)
    {
      if (checksum[i].sum == sum)
      {
	checksum[i].total++;

	if (checksum[i].total > MAX_CROSS_POST)
	{
	  /* smiler.090611: �������v�A�אּ���� */
	  checksum[i].total = 1;
	  return 1;
	}
	return 0;	/* total <= MAX_CROSS_POST */
      }
    }

    if (++checknum >= MAX_CHECKSUM_POST)
      checknum = 0;
    checksum[checknum].sum = sum;
    checksum[checknum].total = 1;
  }
  return 0;
}


static int			/* 1:�Ocross-post 0:���Ocross-post */
checksum_find(fpath)
  char *fpath;
{
  int i, sum;
  char buf[ANSILINELEN];
  FILE *fp;

  sum = 0;
  if (fp = fopen(fpath, "r"))
  {
    for (i = -(LINE_HEADER + 1);;)	/* �e�X�C�O���Y */
    {
      if (!fgets(buf, ANSILINELEN, fp))
	break;

      if (i < 0)	/* ���L���Y */
      {
	i++;
	continue;
      }

      if (*buf == QUOTE_CHAR1 || *buf == '\n' || !strncmp(buf, "��", 2))	 /* ���L�ި� */
	continue;

      sum += checksum_add(buf);
      if (++i >= MAX_CHECKSUM_LINE)
	break;
    }
    fclose(fp);
  }

  return checksum_put(sum);
}


static int
check_crosspost(fpath, bno)
  char *fpath;
  int bno;			/* �n��h���ݪO */
{
  char *blist, folder[64];
  HDR hdr;

  if (HAS_PERM(PERM_ALLADMIN))
    return 0;

  /* �O�D�b�ۤv�޲z���ݪO���C�J��K�ˬd */
  blist = (bshm->bcache + bno)->BM;
  if (HAS_PERM(PERM_BM) && blist[0] > ' ' && is_bm(blist, cuser.userid))
    return 0;

  /* FN_ETC_NOCROSSPOST �s�񤹳\ cross-post ���S�ҦW�� */
  str_lower(folder, cuser.userid);
  if (acl_has(FN_ETC_NOCROSSPOST, "", folder) > 0)
    return 0;

  if (checksum_find(fpath))
  {
    /* �p�G�O cross-post�A������h BN_SECURITY �ê������v */
    brd_fpath(folder, BN_SECURITY, fn_dir);
    hdr_stamp(folder, HDR_COPY | 'A', &hdr, fpath);
    strcpy(hdr.owner, cuser.userid);
    strcpy(hdr.nick, cuser.username);
    sprintf(hdr.title, "%s %s Cross-Post", cuser.userid, Now());
    rec_bot(folder, &hdr, sizeof(HDR));
    btime_update(brd_bno(BN_SECURITY));

    /* smiler.090611: �������v�A�אּ���� */
#if 0
    bbstate &= ~STAT_POST;
    cuser.userlevel &= ~PERM_POST;
    cuser.userlevel |= PERM_DENYPOST;
    if (acct_load(&acct, cuser.userid) >= 0)
    {
      acct.tvalid = time4(NULL) + CROSSPOST_DENY_DAY * 86400;
      acct_setperm(&acct, PERM_DENYPOST, PERM_POST);
    }
    board_main();
    mail_self(FN_ETC_CROSSPOST, str_sysop, "Cross-Post ���v", 0);
    vmsg("�z�]���L�� Cross-Post �w�Q���v�A�Э��s�W�����o�s�v��");
    abort_bbs();
#endif

    return 1;
  }
  return 0;
}
#endif	/* HAVE_DETECT_CROSSPOST */


/* ----------------------------------------------------- */
/* �o��B�^���B�s��B����峹				 */
/* ----------------------------------------------------- */


#ifdef HAVE_ANONYMOUS
static void
log_anonymous(fname)
  char *fname;
{
  char buf[512];

  sprintf(buf, "%s %-13s(%s)\n%-13s %s %s\n",
    Now(), cuser.userid, fromhost, currboard, fname, ve_title);
  f_cat(FN_RUN_ANONYMOUS, buf);
}
#endif


#ifdef HAVE_UNANONYMOUS_BOARD
static void
do_unanonymous(fpath)
  char *fpath;
{
  HDR hdr;
  char folder[64];

  brd_fpath(folder, BN_UNANONYMOUS, fn_dir);
  hdr_stamp(folder, HDR_LINK | 'A', &hdr, fpath);

  strcpy(hdr.owner, cuser.userid);
  strcpy(hdr.title, ve_title);

  rec_bot(folder, &hdr, sizeof(HDR));
  btime_update(brd_bno(BN_UNANONYMOUS));
}
#endif


static void
copy_post_IAS(hdr, fpath)
  HDR *hdr;
  char *fpath;
{
  char folder[64];
  HDR post;
  BRD *brdp, *bend;

  brdp = bshm->bcache;
  bend = brdp + bshm->number;

  while (brdp < bend)
  {
    if (brdp->battr & BRD_IAS)
    {
      brd_fpath(folder, brdp->brdname, fn_dir);
      hdr_stamp(folder, HDR_COPY | 'A', &post, fpath);
      memcpy(post.owner, hdr->owner, TTLEN + 140);
      rec_bot(folder, &post, sizeof(HDR));
    }
    brdp++;
  }
}


void
do_ias_post_log(hdr)
  HDR hdr;
{
  char fpath[64];
  char buf[512];

  if (((currbattr & BRD_IAS) || (strstr(currboard, "IS_")) ||
    (strstr(currboard, "IA_")) || (strstr(currboard, "IAS_"))) && 
    (bbstate & STAT_BM))
  {
    sprintf(buf, "�ɶ�:%s\n�@��:%s �ݪO:%s\n���D:%s\n�ɦW:%s\n--\n\n", Now(), cuser.userid, currboard, hdr.title, hdr.xname);
    f_cat("run/IAS_POLOG", buf);
    sprintf(fpath, "gem/brd/IAS_Meeting/@/@IAS_POLOG");
    f_cat(fpath, buf);
  }
}


#ifdef HAVE_TEMPLATE
static char post_title_ch = -1;
#endif


static int
do_post(xo, title)
  XO *xo;
  char *title;
{
  /* Thor.981105: �i�J�e�ݳ]�n curredit �� quote_file */
  HDR hdr;
  char fpath[64], *folder, *nick, *rcpt;
  int mode = -1, value;
  time_t spendtime;

  /* �i�J�e�n�����n STAT_POST �ˬd */

  /* ����� Internet �v���̡A�u��b�����o��峹 */
  /* Thor.990111: �S��H�X�h���ݪO, �]�u��b�����o��峹 */

  if (!HAS_PERM(PERM_INTERNET) || (currbattr & BRD_NOTRAN))
    curredit &= ~EDIT_OUTGO;

  utmp_mode(M_POST);
  fpath[0] = '\0';
#ifdef HAVE_TEMPLATE
  if (post_title_ch >= 0 && post_title_ch < NUM_PREFIX)
  {
    char buf1[32], buf2[64];
    sprintf(buf1, "prefix/template_%d", post_title_ch + 1);
    post_title_ch = -1;
    brd_fpath(buf2, currboard, buf1);
    if (dashf(buf2))
    {
      sprintf(fpath, "tmp/%s.%d", cuser.userid, time(0));
      f_cp(buf2, fpath, O_TRUNC);
    }
  }
#endif
  time(&spendtime);
  if (vedit(fpath, 1) < 0)
  {
    unlink(fpath);
    vmsg(msg_cancel);
    pcurrhdr = NULL;
    return XO_HEAD;
  }

#ifdef DO_POST_FILTER
  if (post_filter(fpath, ve_title))	/* smiler.080830: �w��峹���D���e�������L�����B */
  {
    unlink(fpath);
    pcurrhdr = NULL;
    return XO_HEAD;
  }
#endif

  spendtime = time(0) - spendtime;	/* itoc.010712: �`�@�᪺�ɶ�(���) */

  if (currbattr & BRD_NOCOUNT || wordsnum < 30)
  {				/* itoc.010408: �H���������{�H */
    value = 0;
  }
  else
  {
    /* itoc.010408: �̤峹����/�ҶO�ɶ��ӨM�w�n���h�ֿ��F����~�|���N�q */
    value = BMIN(wordsnum, spendtime) / 10;	/* �C�Q�r/�� �@�� */
  }

  /* build filename */

  folder = xo->dir;
  hdr_stamp(folder, HDR_LINK | 'A', &hdr, fpath);

  /* set owner to anonymous for anonymous board */

#ifdef HAVE_ANONYMOUS
  /* Thor.980727: lkchu�s�W��[²�檺��ܩʰΦW�\��] */
  if (curredit & EDIT_ANONYMOUS)
  {
    rcpt = anonymousid;	/* itoc.010717: �۩w�ΦW ID */
    nick = STR_ANONYMOUS;
    value = cuser.userno;

    /* Thor.980727: lkchu patch: log anonymous post */
    /* Thor.980909: gc patch: log anonymous post filename */
    log_anonymous(hdr.xname);

#ifdef HAVE_UNANONYMOUS_BOARD
    do_unanonymous(fpath);
#endif
  }
  else
#endif
  {
    rcpt = cuser.userid;
    nick = cuser.username;
  }
  title = ve_title;
  mode = (curredit & EDIT_OUTGO) ? POST_OUTGO : 0;
#ifdef HAVE_REFUSEMARK
  if (curredit & EDIT_RESTRICT)
    mode |= POST_RESTRICT;
#endif

  hdr.xmode = mode;
  sprintf(hdr.value, "%d", value);
  strcpy(hdr.owner, rcpt);
  strcpy(hdr.nick, nick);
  strcpy(hdr.title, title);

  rec_bot(folder, &hdr, sizeof(HDR));
  btime_update(currbno);

#ifdef HAVE_DETECT_CROSSPOST  
  /* smiler.090611: �������v�A�אּ���� */
  if (check_crosspost(fpath, currbno))
    vmsg("�����z�A�Y�ݵo�����šA�Ь� CrossPost �O");
#endif

  if (mode & POST_OUTGO)
    outgo_post(&hdr, currboard);

  post_history(xo, &hdr);

#if 1
  /* smiler.080820: �̯��ȭn�D������ nthu.forsale */
  if (strstr(hdr.title, "��") || strstr(hdr.title, "��") || strstr(hdr.title, "�X�M"))
  {
    /* smiler.080820: �̯��ȭn�D������ nthu.forsale */
    /* smiler.070916: for ���� nthu.forsale */
    HDR  hdr2;
    char fpath2[64], folder2[64];	// smiler.070916
    char board_from[30];		// smiler.070916

   /* smiler.080820: �̯��ȭn�D�� nctu nthu ��R���� nthu.forsale */
    if ((!strcmp(board_from, "nctu")) || (!strcmp(board_from, "nthu")))
    {
      strcpy(fpath2, fpath);			// smiler.070916
      brd_fpath(folder2, "nthu.forsale", FN_DIR);
      hdr_stamp(folder2, HDR_LINK | 'A', &hdr2, fpath2);	// smiler.070916
      strcpy(board_from, currboard);		// smiler.070916

      hdr2.xmode = mode;	// smiler.070916
      strcpy(hdr2.owner, rcpt);	// smiler.070916
      strcpy(hdr2.nick, nick);	// smiler.070916
      strcpy(hdr2.title, title);	// smiler.070916

      rec_bot(folder2, &hdr2, sizeof(HDR));
      btime_update(brd_bno("nthu.forsale"));
    }
  }
#endif

  clear();
  outs("���Q�K�X�峹�A");

  /* smiler.090120: �����]�O�Dpo��O�� */
  do_ias_post_log(hdr);

  if ((currbattr & BRD_NOCOUNT) || (curredit & EDIT_ANONYMOUS) || (wordsnum < 30))
  {				/* itoc.010408: �H���������{�H */
    outs("�峹���C�J�����A�q�Х]�[�C");
  }
  else
  {
    prints("�o�O�z���� %d �g�峹�A�o %d �ȡC", ++cuser.numposts, value);
    addmoney(value);
  }

  /* �^�����@�̫H�c */

  if (curredit & EDIT_BOTH)
  {
    rcpt = quote_user;

    if (strchr(rcpt, '@'))	/* ���~ */
      mode = bsmtp(fpath, title, rcpt, 0);
    else			/* �����ϥΪ� */
      mode = mail_him(fpath, rcpt, title, 0);

    outs(mode >= 0 ? "\n\n���\\�^���ܧ@�̫H�c" : "\n\n�@�̵L�k���H");
  }

  /* smiler.080705:�۰ʶK��ܦU�����] */
  if (!strcmp(currboard, "IAS_Announce") && HAS_PERM(PERM_ALLADMIN))
    copy_post_IAS(&hdr, fpath);

  unlink(fpath);
  pcurrhdr = NULL;

  vmsg(NULL);

#ifdef HAVE_ANONYMOUS
  /* gaod: �o�ΦW��ᤣ���� EDIT_ANONYMOUS �|�ɭP����峹 header ��T���~. Thanks for om@cpu.tfcis.org. */
  if (curredit & EDIT_ANONYMOUS)
    curredit &= ~EDIT_ANONYMOUS;
#endif

  return XO_INIT;
}


static int
do_post_title(xo, title)
  XO *xo;
  char *title;
{
  /* Thor.981105: �i�J�e�ݳ]�n curredit �� quote_file */

  if (!(bbstate & STAT_POST))
  {
#ifdef NEWUSER_LIMIT
    if (cuser.lastlogin - cuser.firstlogin < 3 * 86400)
      vmsg("�s��W���A�T���l�i�i�K�峹");
    else
#endif
      post_no_right();

    pcurrhdr = NULL;
    return XO_HEAD;
  }

  if (HAS_PERM(PERM_VALID))
  {
    if (!strcmp(currboard, "newbm"))
    {
      void (*p)();
      switch (vans("(1)�ӽФ����O�O�D�W�沧�� (2)����O�D/��L [Q] "))
      {
	case '1':
	  p = DL_get("bin/bmtrans.so:bmt_add");
	  (*p)(xo);
	  return XO_INIT;
	case '2':
	  break;
	default:
	  return XO_FOOT;
      }
    }
    else if (!strcmp(currboard, "newboard"))
    {
      if (vans("�O�_�n�ӽж}�O(Y/N)�H[N] ") == 'y')
      {
	vmsg("�i�J�}�O/�s�p�t�ΡC�i�J��Ы� Ctrl + P �ӽж}�O");
	DL_func("bin/newbrd.so:XoNewBoard");
	return XO_INIT;
      }
    }
  }

  film_out(FILM_POST, 0);

  move(19, 0);
  prints("�o��峹��i %s �j�ݪO", currboard);

#ifdef POST_PREFIX
  char *subject;

  if (title)
  {
    subject = NULL;
  }
  else		/* itoc.020113: �s�峹��ܼ��D���� */
  {
    if (!(currbattr & BRD_NOPREFIX))
    {
      FILE *fp;
      int ch, len = 6, pnum, newline = 0;
      char fpath[64];
      char prefix[NUM_PREFIX][16];
      char *prefix_default[NUM_PREFIX] = DEFAULT_PREFIX;

      for (ch = 0; ch < NUM_PREFIX; ch++)
	strcpy(prefix[ch], prefix_default[ch]);

      move(21, 0);
      prints("���O�G");

      brd_fpath(fpath, currboard, "prefix.new");
      if (fp = fopen(fpath, "r"))
      {
	/* ���J�]�w�� */
	for (ch = 0; ch < NUM_PREFIX; ch++)
	{
	  if (!fgets(fpath, 14, fp))
	    break;
	  if (strlen(fpath) == 1)	/* '\n' */
	    break;
	  fpath[strlen(fpath) - 1] = '\0';
	  strcpy(prefix[ch], fpath);
	  if ((len + 3 + strlen(fpath)) >= 70)	/* ����C�L */
	  {
	    move(22, 0);
	    prints("      ");
	    len = 6;
	    newline = 1;
	  }
	  prints("%d.%s ", ch + 1, fpath);
	  len += (3 + strlen(fpath));
	  pnum = ch + 1;
	}
	fclose(fp);
      }
      else	/* �S���]�w��, �ϥιw�] */
      {
	pnum = NUM_PREFIX;
	for (ch = 0; ch < NUM_PREFIX; ch++)
	{
	  prints("%d.%s ", ch + 1, prefix[ch]);
	  len += (3 + strlen(prefix[ch]));
	}
      }

      ch = vget(21 + newline, len, "", fpath, 3, DOECHO) - '1';
      if (ch >= 0 && ch < pnum)	/* ��J�Ʀr�ﶵ */
      {
	if (prefix[ch][0] == '[' ||
	    !strncmp(prefix[ch], "�i", 2) ||
	    !strncmp(prefix[ch], "�m", 2) ||
	    !strncmp(prefix[ch], "�q", 2))
	  sprintf(fpath, "%s ", prefix[ch]);
	else
	  sprintf(fpath, "[%s] ", prefix[ch]);

	subject = fpath;
#ifdef HAVE_TEMPLATE
	post_title_ch = ch;
#endif
      }
      else				/* �ťո��L */
	subject = NULL;

      move(20, 0);
      clrtobot();
    }
    else	/* �ݪO�]�w���ϥΤ峹���O */
      subject = NULL;
  }

  if (!ve_subject(21, title, subject))
#else
  if (!ve_subject(21, title, NULL))
#endif
  {
    pcurrhdr = NULL;
#ifdef HAVE_TEMPLATE
    post_title_ch = -1;
#endif
    return XO_HEAD;
  }

  return do_post(xo, title);
}


void
add_post(brdname, fpath, title, owner, nick, mode, fhdr)	/* �o���ݪO */
  char *brdname;	/* �� post ���ݪO */
  char *fpath;		/* �ɮ׸��| */
  char *title;		/* �峹���D */
  char *owner, *nick;	/* �峹�@��/�ʺ� */
  int mode;
  HDR *fhdr;		/* return */
{
  HDR hdr;
  char folder[64];

  brd_fpath(folder, brdname, fn_dir);
  hdr_stamp(folder, HDR_LINK | 'A', &hdr, fpath);
  hdr.xmode = mode;
  strcpy(hdr.owner, owner);
  strcpy(hdr.nick, nick);
  strcpy(hdr.title, title);
  rec_bot(folder, &hdr, sizeof(HDR));
  if (fhdr)
    memcpy(fhdr, &hdr, sizeof(HDR));

  btime_update(brd_bno(brdname));
}


int
do_reply(xo, hdr)
  XO *xo;
  HDR *hdr;
{
  curredit = 0;

  pcurrhdr = hdr;
  switch (vans("�� �^���� (F)�ݪO (M)�@�̫H�c (B)�G�̬ҬO (R)Repost�ܬݪO (Q)�����H[F] "))
  {
  case 'm':
    hdr_fpath(quote_file, xo->dir, hdr);
    return do_mreply(hdr, 0);

  case 'q':
    return XO_FOOT;

  case 'b':
    /* �Y�L�H�H���v���A�h�u�^�ݪO */
    if (HAS_PERM(strchr(hdr->owner, '@') ? PERM_INTERNET : PERM_LOCAL))
      curredit = EDIT_BOTH;
    break;

  case 'r':
  case 'y':	/* �䴩 yy �䪽�� Repost */
    /* �]�����e post_reply() �w���L STAT_POST �ˬd�A�ҥH�o�̥i�H�����i�� do_post() */
    hdr_fpath(quote_file, xo->dir, hdr);
    curredit = EDIT_REPOST;
    /* Thor.981105: ���׬O��i��, �άO�n��X��, ���O�O���i�ݨ쪺, �ҥH�^�H�]��������X */
    if (hdr->xmode & (POST_INCOME | POST_OUTGO))
      curredit |= EDIT_OUTGO;
    sprintf(ve_title, "Re: %s", str_ttl(hdr->title));
    ve_title[TTLEN] = '\0';
    return do_post(xo, ve_title);
    break;
  }

  /* Thor.981105: ���׬O��i��, �άO�n��X��, ���O�O���i�ݨ쪺, �ҥH�^�H�]��������X */
  if (hdr->xmode & (POST_INCOME | POST_OUTGO))
    curredit |= EDIT_OUTGO;

  hdr_fpath(quote_file, xo->dir, hdr);
  strcpy(quote_user, hdr->owner);
  strcpy(quote_nick, hdr->nick);
  return do_post_title(xo, hdr->title);
}


#ifdef HAVE_REFUSEMARK
int		/* 0: restrict  1: can_see  */
chkrestrict(hdr)
  HDR *hdr;
{
  return !(hdr->xmode & POST_RESTRICT) || RefusePal_belong(currboard, hdr);
}
#endif


int		/* 0: restrict  1: can_do_some_action */
chkrescofo(hdr)	/* restrict / copy / forward */
  HDR *hdr;
{
  return
#ifdef HAVE_REFUSEMARK
    chkrestrict(hdr) &&
#endif
    !(hdr->xmode & POST_NOFORWARD) && (!(currbattr & BRD_NOFORWARD) || (bbstate & STAT_BM));
}


#ifdef SYSOP_MBOX_BRD
extern int sysop_reply;


static int
post_sysop_reply(xo, hdr)
  XO *xo;
  HDR *hdr;
{
  char fpath[64];

  hdr_fpath(fpath, xo->dir, hdr);
  strcpy(quote_file, fpath);
  sysop_reply = 1;
  do_mreply(hdr, 1);
  sysop_reply = 0;
  rec_put(xo->dir, hdr, sizeof(HDR), xo->pos, NULL);
  btime_update(brd_bno(currboard));
  return XO_HEAD;
}


static int
post_sysop_send(xo)
  XO *xo;
{
  sysop_reply = 1;
  m_send();
  sysop_reply = 0;
  btime_update(brd_bno(currboard));
  return XO_HEAD;
}
#endif


static int
post_reply(xo)
  XO *xo;
{
  if (bbstate & STAT_POST)
  {
    HDR *hdr;

    hdr = (HDR *) xo_pool + (xo->pos - xo->top);

#ifdef SYSOP_MBOX_BRD
    if (!strcmp(currboard, BN_SYSOPMBOX))
      return post_sysop_reply(xo, hdr);
#endif

#ifdef HAVE_REFUSEMARK
    if (!chkrestrict(hdr))
      return XO_NONE;
#endif

    return do_reply(xo, hdr);
  }
  else
    post_no_right();

  return XO_HEAD;
}


static int
post_add(xo)
  XO *xo;
{
#ifdef SYSOP_MBOX_BRD
  if (!strcmp(currboard, BN_SYSOPMBOX))
    return post_sysop_send(xo);
#endif

  curredit = EDIT_OUTGO;
  *quote_file = '\0';
  return do_post_title(xo, NULL);
}


/* ----------------------------------------------------- */
/* �L�X hdr ���D					 */
/* ----------------------------------------------------- */


int
tag_char(chrono)
  int chrono;
{
  return TagNum && !Tagger(chrono, 0, TAG_NIN) ? '*' : ' ';
}


#ifdef HAVE_DECLARE
inline int
cal_day(date)		/* itoc.010217: �p��P���X */
  char *date;
{
#if 0
   ���Ǥ����O�@�ӱ�����@�ѬO�P���X������.
   �o�����O:
         c                y       26(m+1)
    W= [---] - 2c + y + [---] + [---------] + d - 1
         4                4         10
    W �� ���ҨD������P����. (�P����: 0  �P���@: 1  ...  �P����: 6)
    c �� ���w�������~�����e���Ʀr.
    y �� ���w�������~��������Ʀr.
    m �� �����
    d �� �����
   [] �� ��ܥu���Ӽƪ���Ƴ��� (�a�O���)
    ps.�ҨD������p�G�O1���2��,�h�������W�@�~��13���14��.
       �ҥH������m�����Ƚd�򤣬O1��12,�ӬO3��14
#endif

  /* �A�� 2000/03/01 �� 2099/12/31 */

  int y, m, d;

  y = 10 * ((int) (date[0] - '0')) + ((int) (date[1] - '0'));
  d = 10 * ((int) (date[6] - '0')) + ((int) (date[7] - '0'));
  if (date[3] == '0' && (date[4] == '1' || date[4] == '2'))
  {
    y -= 1;
    m = 12 + (int) (date[4] - '0');
  }
  else
  {
    m = 10 * ((int) (date[3] - '0')) + ((int) (date[4] - '0'));
  }
  return (-1 + y + y / 4 + 26 * (m + 1) / 10 + d) % 7;
}
#endif


void
hdr_outs(hdr, cc)		/* print HDR's subject */
  HDR *hdr;
  int cc;			/* �L�X�̦h cc - 1 �r�����D */
{
  /* �^��/���/���/�\Ū�����P�D�D�^��/�\Ū�����P�D�D���/�\Ū�����P�D�D��� */
  static char *type[6] = {"Re", "Fw", "��", "\033[1;33m=>", "\033[1;33m->", "\033[1;32m��"};
  uschar *title, *mark;
  int ch, len;
  int in_chi;		/* 1: �b����r�� */
#ifdef HAVE_DECLARE
  int square;		/* 1: �n�B�z��A */
#endif
#ifdef CHECK_ONLINE
  UTMP *online;
#endif

  /* --------------------------------------------------- */
  /* �L�X���						 */
  /* --------------------------------------------------- */

#ifdef HAVE_DECLARE
  /* itoc.010217: ��άP���X�ӤW�� */
  /*smiler.070724: ����L�X�e,�h�Ť@��*/
  prints(" \033[1;3%dm%s\033[m ", cal_day(hdr->date) + 1, hdr->date + 3);

#else
  prints(" %s ", hdr->date + 3);
#endif

  /* --------------------------------------------------- */
  /* �L�X�@��						 */
  /* --------------------------------------------------- */

#ifdef CHECK_ONLINE
  if (online = utmp_seek(hdr))
    outs(COLOR7);
#endif

  mark = hdr->owner;
  len = IDLEN + 1;
  in_chi = 0;

  while (ch = *mark)
  {
    if (--len <= 0)
    {
      /* ��W�L len ���ת������������� */
      /* itoc.060604.����: �p�G��n���b����r���@�b�N�|�X�{�ýX�A���L�o���p�ܤֵo�͡A�ҥH�N���ޤF */
      ch = '.';
    }
    else
    {
      /* ���~���@�̧� '@' ���� '.' */
      if (in_chi || IS_ZHC_HI(ch))	/* ����r���X�O '@' ������ */
	in_chi ^= 1;
      else if (ch == '@')
	ch = '.';
    }

    outc(ch);

    if (ch == '.')
      break;

    mark++;
  }

  while (len--)
    outc(' ');

#ifdef CHECK_ONLINE
  if (online)
    outs(str_ransi);
#endif

  /* --------------------------------------------------- */
  /* �L�X���D������					 */
  /* --------------------------------------------------- */

  /* len: ���D�O type[] �̭������@�� */
#ifdef HAVE_REFUSEMARK
  if (!chkrestrict(hdr))
  {
    title = "<< �峹�O�K >>";
    len = 2;
  }
  else
#endif
  {
    title = str_ttl(mark = hdr->title);
    len = (title == mark) ? 2 : (*mark == 'R') ? 0 : 1;
    if (!strcmp(currtitle, title))
      len += 3;
  }
  outs(type[len]);
  outc(' ');


  /* --------------------------------------------------- */
  /* �L�X���D						 */
  /* --------------------------------------------------- */

  mark = title + cc;

#ifdef HAVE_DECLARE	/* Thor.980508: Declaration, ���ըϬY��title����� */
  square = in_chi = 0;
  if (len < 3)
  {
    if (*title == '[')
    {
      outs("\033[1m");
      square = 1;
    }
  }
#endif

  /* ��W�L cc ���ת������������� */
  /* itoc.060604.����: �p�G��n���b����r���@�b�N�|�X�{�ýX�A���L�o���p�ܤֵo�͡A�ҥH�N���ޤF */
  while ((ch = *title++) && (title < mark - 3))
  {
#ifdef HAVE_DECLARE
    if (square)
    {
      if (in_chi || IS_ZHC_HI(ch))	/* ����r���ĤG�X�Y�O ']' ����O��A */
      {
	in_chi ^= 1;
      }
      else if (ch == ']')
      {
	outs("]\033[m");
	square = 0;			/* �u�B�z�@�դ�A�A��A�w�g�B�z���F */
	continue;
      }
    }
#endif

    outc(ch);
  }

  if (title == mark - 3)	/* �٨S�L�� */
  {
    if (strlen(title) < 4)
    {
      outc(ch);
      while (ch = *title++)
	outc(ch);
    }
    else
    {
      if (in_chi)
	prints("%c...", ch);
      else
	outs(" ...");
    }
  }

#ifdef HAVE_DECLARE
  if (square || len >= 3)	/* Thor.980508: �ܦ��٭�� */
#else
  if (len >= 3)
#endif
    outs("\033[m");

  outc('\n');
}


#ifdef HAVE_LIGHTBAR
void
hdr_outs_bar(hdr, cc)	/* print HDR's subject */
  HDR *hdr;
  int cc;		/* �L�X�̦h cc - 1 �Ӧr */
{
  /* �^��/���/���/�\Ū�����P�D�D�^��/�\Ū�����P�D�D���/�\Ū�����P�D�D��� */
  static char *type[6] = {"Re", "Fw", "��", "\033[1;33m=>", "\033[1;33m->", "\033[1;32m��"};
  uschar *title, *mark;
  int ch, len;
  int in_chi;		/* 1: �b����r�� */
#ifdef HAVE_DECLARE
  int square;		/* 1: �n�B�z��A */
#endif
#ifdef CHECK_ONLINE
  UTMP *online;
#endif

  /* --------------------------------------------------- */
  /* �L�X���						 */
  /* --------------------------------------------------- */

#ifdef HAVE_DECLARE
  /* itoc.010217: ��άP���X�ӤW�� */
  /*smiler.070724: ����L�X�e,�h�Ť@��*/
  prints("%s \033[1;3%dm%s\033[m%s ",
    UCBAR[UCBAR_POST], cal_day(hdr->date) + 1, hdr->date + 3, UCBAR[UCBAR_POST]);
#else
  prints(" %s ", hdr->date + 3);
#endif

  /* --------------------------------------------------- */
  /* �L�X�@��						 */
  /* --------------------------------------------------- */

#ifdef CHECK_ONLINE
  if (online = utmp_seek(hdr))
    outs(COLOR7);
#endif

  mark = hdr->owner;
  len = IDLEN + 1;
  in_chi = 0;

  while (ch = *mark)
  {
    if (--len <= 0)
    {
      /* ��W�L len ���ת������������� */
      /* itoc.060604.����: �p�G��n���b����r���@�b�N�|�X�{�ýX�A���L�o���p�ܤֵo�͡A�ҥH�N���ޤF */
      ch = '.';
    }
    else
    {
      /* ���~���@�̧� '@' ���� '.' */
      if (in_chi || IS_ZHC_HI(ch))	/* ����r���X�O '@' ������ */
	in_chi ^= 1;
      else if (ch == '@')
	ch = '.';
    }

    outc(ch);

    if (ch == '.')
      break;

    mark++;
  }

  while (len--)
    outc(' ');

#ifdef CHECK_ONLINE
  if (online)
    outs(str_ransi);
#endif

  /* --------------------------------------------------- */
  /* �L�X���D������					 */
  /* --------------------------------------------------- */

  /* len: ���D�O type[] �̭������@�� */
#ifdef HAVE_REFUSEMARK
  if (!chkrestrict(hdr))
  {
    title = "<< �峹�O�K >>";
    len = 2;
  }
  else
#endif
  {
    title = str_ttl(mark = hdr->title);
    len = (title == mark) ? 2 : (*mark == 'R') ? 0 : 1;
    if (!strcmp(currtitle, title))
      len += 3;
  }
  outs(UCBAR[UCBAR_POST]);
  outs(type[len]);
  outs(UCBAR[UCBAR_POST]);
  outc(' ');

  /* --------------------------------------------------- */
  /* �L�X���D						 */
  /* --------------------------------------------------- */

  mark = title + cc;

#ifdef HAVE_DECLARE	/* Thor.980508: Declaration, ���ըϬY��title����� */
  square = in_chi = 0;
  if (len < 3)
  {
    if (*title == '[')
    {
      outs("\033[1m");
      square = 1;
    }
  }
#endif

  outs(UCBAR[UCBAR_POST]);

  /* ��W�L cc ���ת������������� */
  /* itoc.060604.����: �p�G��n���b����r���@�b�N�|�X�{�ýX�A���L�o���p�ܤֵo�͡A�ҥH�N���ޤF */
  while ((ch = *title++) && (title < mark - 3))
  {
#ifdef HAVE_DECLARE
    if (square)
    {
      if (in_chi || IS_ZHC_HI(ch))	/* ����r���ĤG�X�Y�O ']' ����O��A */
      {
	in_chi ^= 1;
      }
      else if (ch == ']')
      {
	prints("]\033[m%s", UCBAR[UCBAR_POST]);
	square = 0;			/* �u�B�z�@�դ�A�A��A�w�g�B�z���F */
	continue;
      }
    }
#endif

    outc(ch);
  }

  if (ch && (title == mark - 3))	/* �٨S�L�� */
  {

    if (strlen(title) < 4)
    {
      outc(ch);
      while (ch = *title++)
	outc(ch);
    }
    else
    {
      if (in_chi)
	prints("%c...", ch);
      else
	outs(" ...");
      title += 4;
    }
  }

  title--;	/* �Ҽ{ (title == mark - 3) �ɭ�n�L��������(*title == '\0'), �n�ɦ^�� */
  while(title++ < mark)
    outc(' ');

  outs("\033[m");
}
#endif


/* ----------------------------------------------------- */
/* �ݪO�\���						 */
/* ----------------------------------------------------- */


static int post_body();
static int post_head();


static int
post_init(xo)
  XO *xo;
{
  xo_load(xo, sizeof(HDR));
  return post_head(xo);
}


static int
post_load(xo)
  XO *xo;
{
  xo_load(xo, sizeof(HDR));
  return post_body(xo);
}


#ifdef SYSOP_MBOX_BRD
static char *
sysop_mbox_attr(hdr, attr)
  HDR *hdr;
  int attr;
{
  static char color_attr[32];
  int mode;
  char attr_tmp[16] = {0};

  mode = hdr->xmode;

  if (mode & MAIL_REPLIED)
  {
    attr = (mode & MAIL_MARKED) ? 'R' : 'r';
    strcpy(attr_tmp, "\033[1;31m");
  }
  else if (mode & MAIL_MARKED)
  {
    attr |= 'M';
    strcpy(attr_tmp, "\033[1;36m");
  }
  else if (!attr)
  {
    attr = '+';
    strcpy(attr_tmp, "");
  }

  sprintf(color_attr, "%s%c\033[m", attr_tmp, attr);
  return color_attr;
}
#endif


static char *
post_attr(hdr)
  HDR *hdr;
{
  static char color_attr[32];
  int mode, attr, read, unread;
  char attr_tmp[16];
  attr_tmp[0] = '\0';

  mode = hdr->xmode;

  /* �w�\Ū���p�g�A���\Ū���j�g */
  /* �ѩ�m����S���\Ū�O���A�ҥH�����wŪ */
  /* �[�K�峹�����wŪ */
  read = (USR_SHOW & USR_SHOW_POST_MODIFY_UNREAD) ? !brh_unread(hdr->chrono) : !brh_unread(BMAX(hdr->chrono, hdr->stamp));
#ifdef HAVE_REFUSEMARK
  attr = ((mode & POST_BOTTOM) || read ||
    ((mode & POST_RESTRICT) && strcmp(hdr->owner, cuser.userid) && !(bbstate & STAT_BM))) ? 0x20 : 0;
#else
  attr = ((mode & POST_BOTTOM) || read ? 0x20 : 0;
#endif

  unread = ((USR_SHOW & USR_SHOW_POST_MODIFY_UNREAD) && attr && brh_unread(BMAX(hdr->chrono, hdr->stamp))) ? 1 : 0;

#ifdef SYSOP_MBOX_BRD
  if (!strcmp(currboard, BN_SYSOPMBOX))
    return sysop_mbox_attr(hdr, attr);
#endif

#ifdef HAVE_REFUSEMARK
  if ((RefusePal_level(currboard, hdr) == 1) && (USR_SHOW & USR_SHOW_POST_ATTR_RESTRICT_F))
  {
    attr |= 'F',
    strcpy(attr_tmp, "\033[1;33m");
  }
  else if ((RefusePal_level(currboard, hdr) == -1) && (USR_SHOW & USR_SHOW_POST_ATTR_RESTRICT))
  {
    attr |= 'L';
    strcpy(attr_tmp, "\033[1;34m");
  }
  else
#endif
  if ((bbstate & STAT_BOARD) && (mode & POST_GEM) && (mode & POST_MARKED) && (USR_SHOW & USR_SHOW_POST_ATTR_GEM_MARKED))   /* �O�D�~�ݱo�� G/B */
  {
    attr |= 'B';	/* �Y�� mark+gem�A��� B */
    strcpy(attr_tmp, "\033[1;31m");
  }
  else if ((bbstate & STAT_BOARD) && (mode & POST_GEM) && (!(mode & POST_MARKED)) && (USR_SHOW & USR_SHOW_POST_ATTR_GEM))
  {
    attr |= 'G';
    strcpy(attr_tmp, "\033[1;35m");
  }
  else
#ifdef HAVE_LABELMARK
  if ((mode & POST_DELETE) && (USR_SHOW & USR_SHOW_POST_ATTR_DELETE))
  {
    attr |= 'T';
    strcpy(attr_tmp, "\033[1;32m");
  }
  else
#endif
  if ((mode & POST_NOFORWARD) && (USR_SHOW & USR_SHOW_POST_ATTR_NOFORWARD))
  {
    attr |= 'X';
    strcpy(attr_tmp, "\033[1;34m");
  }
  else if ((mode & POST_NOSCORE) && (USR_SHOW & USR_SHOW_POST_ATTR_NOSCORE))
  {
    attr |= 'N';
    strcpy(attr_tmp, "\033[1;34m");
  }
  else if ((mode & POST_MARKED) && (USR_SHOW & USR_SHOW_POST_ATTR_MARKED))
  {
    attr |= 'M';
    strcpy(attr_tmp, "\033[1;36m");
    if (mode & POST_GOOD)
      strcpy(attr_tmp, "\033[1;33m");
  }
  else if (!attr)
  {
    attr = '+';
    strcpy(attr_tmp, "");
  }

  if (unread && (RefusePal_level(currboard, hdr) >= 0))
  {
#ifdef HAVE_REFUSEMARK
    if ((mode & POST_RESTRICT) && (USR_SHOW & USR_SHOW_POST_ATTR_RESTRICT_F))
    {
      attr = 'F',
      strcpy(attr_tmp, "\033[33m");
    }
    else
#endif
    {
      if ((attr == 'm' || attr == 'b') || (mode & POST_BOTTOM))
	attr = '=';
      else
	attr = '~';
      strcpy(attr_tmp, (mode & POST_GOOD) ? "\033[33m" : "\033[36m");
    }
  }

  sprintf(color_attr, "%s%c\033[m", attr_tmp, attr);
  return color_attr;
}


static void
post_item(num, hdr)
  int num;
  HDR *hdr;
{
  if (hdr->xmode & POST_BOTTOM)
    prints("  \033[1;3%cm%4s\033[m%c%s",
      hdr->color, hdr->mark, tag_char(hdr->chrono), post_attr(hdr));
  else
    prints("%6d%c%s", num, tag_char(hdr->chrono), post_attr(hdr));

#ifdef HAVE_SCORE
  if (((hdr->xmode & POST_SCORE) && (USR_SHOW & USR_SHOW_POST_SCORE)) &&
    chkrestrict(hdr))
  {
    num = hdr->score;
    if (!num && !(USR_SHOW & USR_SHOW_POST_SCORE_0))
      outs("  ");
    else if (num <= 99 && num >= -99)
      prints("\033[1;3%cm%2d\033[m",
	num > 0 ? '1' : num < 0 ? '2' : '0', abs(num));
    else
      prints("\033[1;3%s\033[m", num >= 0 ? "1m�z" : "2m�N");
 }
 else
 {
   outs("  ");
 }

  hdr_outs(hdr, d_cols + 45);	/* �֤@��ө���� */
#else
 hdr_outs(hdr, d_cols + 47);
#endif
}


#ifdef HAVE_LIGHTBAR
static int
post_item_bar(xo, mode)
  XO *xo;
  int mode;
{
  HDR *hdr;
  int num;

  hdr = (HDR *) xo_pool + xo->pos - xo->top;
  num = xo->pos + 1;

  if (hdr->xmode & POST_BOTTOM)
  {
    prints("%s  \033[1;3%cm%4s\033[m%s%c%s%s",
      mode ? UCBAR[UCBAR_POST] : "",
      hdr->color, hdr->mark, mode ? UCBAR[UCBAR_POST] : "",
      tag_char(hdr->chrono), post_attr(hdr), mode ? UCBAR[UCBAR_POST] : "");
  }
  else
  {
    prints("%s%6d%c%s%s",
      mode ? UCBAR[UCBAR_POST] : "",
      num, tag_char(hdr->chrono), post_attr(hdr), mode ? UCBAR[UCBAR_POST] : "");
  }

#ifdef HAVE_SCORE
  if (((hdr->xmode & POST_SCORE) && (USR_SHOW & USR_SHOW_POST_SCORE)) &&
    chkrestrict(hdr))
  {
    num = hdr->score;
    if (!num && !(USR_SHOW & USR_SHOW_POST_SCORE_0))
      outs("  ");
    else if (num <= 99 && num >= -99)
      prints("%s\033[1;3%cm%s%2d\033[m%s",
	mode ? UCBAR[UCBAR_POST] : "", num > 0 ? '1' : num < 0 ? '2' : mode ? '7' : '0',
	mode ? UCBAR[UCBAR_POST] : "", abs(num), mode ? UCBAR[UCBAR_POST] : "");
    else
      prints("%s\033[1;3%s\033[m%s", mode ? UCBAR[UCBAR_POST] : "", num >= 0 ? "1m�z" : "2m�N",
	mode ? UCBAR[UCBAR_POST] : "");
  }
  else
  {
    outs("  ");
  }

  if (mode)
    hdr_outs_bar(hdr, d_cols + 45);	/* �֤@��ө���� */
  else
    hdr_outs(hdr, d_cols + 45);
#else
  if (mode)
    hdr_outs_bar(hdr, d_cols + 47);
  else
    hdr_outs(hdr, d_cols + 47);
#endif

  move(xo->pos - xo->top + 3, 0);
  return XO_NONE;
}
#endif


static int
post_body(xo)
  XO *xo;
{
  HDR *hdr;
  int num, max, tail;

  max = xo->max;
  if (max <= 0)
  {
    if (bbstate & STAT_POST)
    {
      if (vans("�n�s�W��ƶ�(Y/N)�H[N] ") == 'y')
	return post_add(xo);
    }
    else
    {
      vmsg("���ݪO�|�L�峹");
    }
    return XO_QUIT;
  }

  hdr = (HDR *) xo_pool;
  num = xo->top;
  tail = num + XO_TALL;
  if (max > tail)
    max = tail;

  move(3, 0);
  do
  {
    post_item(++num, hdr++);
  } while (num < max);
  clrtobot();

  /* return XO_NONE; */
  return XO_FOOT;	/* itoc.010403: �� b_lines ��W feeter */
}


static int
post_head(xo)
  XO *xo;
{
  vs_head(currBM, xo->xyz);
  prints(NECKER_POST, d_cols, "", bshm->mantime[currbno]);

  return post_body(xo);
}


/* ----------------------------------------------------- */
/* ��Ƥ��s���Gbrowse / history				 */
/* ----------------------------------------------------- */


static int
post_visit(xo)
  XO *xo;
{
  int ans, row, max;
  HDR *hdr;

  ans = vans("�]�w�Ҧ��峹 (U)��Ū (V)�wŪ (W)�e�wŪ�᥼Ū (Q)�����H[Q] ");
  if (ans == 'v' || ans == 'u' || ans == 'w')
  {
    row = xo->top;
    max = xo->max - row + 3;
    if (max > b_lines)
      max = b_lines;

    hdr = (HDR *) xo_pool + (xo->pos - row);
    /* brh_visit(ans == 'w' ? hdr->chrono : ans == 'u'); */
    /* weiyu.041010: �b�m����W�� w ���������wŪ */
    brh_visit((ans == 'u') ? 1 : (ans == 'w' && !(hdr->xmode & POST_BOTTOM)) ? hdr->chrono : 0);

    return XO_BODY;
  }
  return XO_FOOT;
}


void
post_history(xo, hdr)	/* �N hdr �o�g�[�J brh */
  XO *xo;
  HDR *hdr;
{
  int fd;
  time4_t prev, chrono, maxchrono, next, this;
  HDR buf;

  maxchrono = BMAX(hdr->chrono, hdr->stamp);

  chrono = hdr->chrono;
  if (!brh_unread(chrono))	/* �Y hdr->chrono �w�b brh ���A�N���̧� hdr->stamp �[�J brh �� */
    chrono = maxchrono;

add_brh:
  if (!brh_unread(chrono))	/* �p�G�w�b brh ���A�N�L�ݰʧ@ */
    return;

  if ((fd = open(xo->dir, O_RDONLY)) >= 0)
  {
    prev = chrono + 1;
    next = chrono - 1;

    while (read(fd, &buf, sizeof(HDR)) == sizeof(HDR))
    {
      this = BMAX(buf.chrono, buf.stamp);

      if (chrono - this < chrono - prev)
	prev = this;
      else if (this - chrono < next - chrono)
	next = this;
    }
    close(fd);

    if (prev > chrono)	/* �S���U�@�g */
      prev = chrono;
    if (next < chrono)	/* �S���W�@�g */
      next = chrono;

    brh_add(prev, chrono, next);
  }

  if (chrono == maxchrono)
    return;

  chrono = maxchrono;
  goto add_brh;		/* �A�ˬd�@�� hdr->stamp */
}


static int
post_browse(xo)
  XO *xo;
{
  HDR *hdr;
  int xmode, pos, key;
  char *dir, fpath[64];
  extern int more_ip;

  dir = xo->dir;

  for (;;)
  {
    pos = xo->pos;
    hdr = (HDR *) xo_pool + (pos - xo->top);
    xmode = hdr->xmode;

#ifdef HAVE_REFUSEMARK
    if (!chkrestrict(hdr))
      return XO_NONE;
#endif

    hdr_fpath(fpath, dir, hdr);
    pcurrhdr = hdr;

    /* Thor.990204: ���Ҽ{more �Ǧ^�� */
    if ((key = more(fpath, FOOTER_POST)) < 0)
      break;

    pcurrhdr = NULL;
    post_history(xo, hdr);
    strcpy(currtitle, str_ttl(hdr->title));

re_key:
    switch (xo_getch(xo, key))
    {
    case XO_BODY:
      continue;

    case 'y':
    case 'r':
      if (bbstate & STAT_POST)
      {
#ifdef SYSOP_MBOX_BRD
	if (!strcmp(currboard, BN_SYSOPMBOX))
	  return post_sysop_reply(xo, hdr);
#endif
	if (do_reply(xo, hdr) == XO_INIT)	/* �����\�a post �X�h�F */
	  return post_init(xo);
      }
      break;

    case 'm':
      if ((bbstate & STAT_BOARD) && !(xmode & POST_MARKED))
      {
	/* hdr->xmode = xmode ^ POST_MARKED; */
	/* �b post_browse �ɬݤ��� m �O���A�ҥH����u�� mark */
	hdr->xmode = xmode | POST_MARKED;
	currchrono = hdr->chrono;
	rec_put(dir, hdr, sizeof(HDR), pos, cmpchrono);
      }
      break;

#ifdef HAVE_SCORE
    case 'e':
      post_e_score(xo);
      return post_init(xo);

    case '%':
      post_score(xo);
      return post_init(xo);
#endif

    case '!':
      more_ip ^= 1;
      continue;

    case '/':
      if (vget(b_lines, 0, "�j�M�G", hunt, sizeof(hunt), DOECHO))
      {
	more(fpath, FOOTER_POST);
	goto re_key;
      }
      continue;

    case 'E':
      return post_edit(xo);

    case 'C':	/* itoc.000515: post_browse �ɥi�s�J�Ȧs�� */
      if (chkrescofo(hdr))
      {
	FILE *fp;
	if (fp = tbf_open())
	{
	  f_suck(fp, fpath);
	  fclose(fp);
	}
      }
      break;

    case 'h':
      xo_help("post");
      break;

    case 'o':
      if ((bbstate & STAT_BOARD) && !(xmode & POST_NOFORWARD))
      {
	/* hdr->xmode = xmode ^ POST_NOFORWARD; */
	/* �b post_browse �ɬݤ��� x �O���A�ҥH����u�� mark */
	hdr->xmode = xmode | POST_NOFORWARD;
	currchrono = hdr->chrono;
	rec_put(dir, hdr, sizeof(HDR), pos, cmpchrono);
      }
      break;

    }
    break;
  }

  return post_head(xo);
}


/* ----------------------------------------------------- */
/* ��ذ�						 */
/* ----------------------------------------------------- */


static int
post_gem(xo)
  XO *xo;
{
  int level;
  char fpath[64];

  strcpy(fpath, "gem/");
  strcpy(fpath + 4, xo->dir);

  level = 0;
  if (bbstate & STAT_BOARD)
    level ^= GEM_W_BIT;
  if (HAS_PERM(PERM_SYSOP))
    level ^= GEM_X_BIT;
  if (bbstate & STAT_BM)
    level ^= GEM_M_BIT;

  XoGem(fpath, "��ذ�", level);
  return XO_INIT;
}


/* ----------------------------------------------------- */
/* �i�O�e��						 */
/* ----------------------------------------------------- */


static int
post_memo(xo)
  XO *xo;
{
  char fpath[64];

  brd_fpath(fpath, currboard, fn_note);
  /* Thor.990204: ���Ҽ{more �Ǧ^�� */
  if (more(fpath, NULL) < 0)
  {
    vmsg("���ݪO�|�L�u�i�O�e���v");
    return XO_FOOT;
  }

  return post_head(xo);
}


/* ----------------------------------------------------- */
/* �\��Gtag / switch / cross / forward			 */
/* ----------------------------------------------------- */


static int
post_tag(xo)
  XO *xo;
{
  HDR *hdr;
  int tag, pos, cur;

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;

  if (xo->key == XZ_XPOST)
    pos = hdr->xid;

  if (tag = Tagger(hdr->chrono, pos, TAG_TOGGLE))
  {
    move(3 + cur, 6);
    outc(tag > 0 ? '*' : ' ');
  }

  /* return XO_NONE; */
  return xo->pos + 1 + XO_MOVE; /* lkchu.981201: ���ܤU�@�� */
}


static int
post_switch(xo)
  XO *xo;
{
  int bno;
  BRD *brd;
  char bname[BNLEN + 1];

  if (brd = ask_board(bname, BRD_R_BIT, NULL))
  {
    if ((bno = brd - bshm->bcache) >= 0 && currbno != bno)
    {
      XoPost(bno);
      return XZ_POST;
    }
  }
  else
  {
    vmsg(err_bid);
  }
  return post_head(xo);
}


int
post_cross(xo)
  XO *xo;
{
  /* �ӷ��ݪO */
  char *dir, *ptr;
  HDR *hdr, xhdr;

  /* ����h���ݪO */
  int xbno;
  usint xbattr;
  char xboard[BNLEN + 1], xfolder[64];
  HDR xpost;

  HDR *hdr_org;
  int pos, cur;

  int tag, rc, locus, finish;
  int method;		/* 0:������ 1:�q���}�ݪO/��ذ�/�H�c����峹 2:�q���K�ݪO����峹 */
  usint tmpbattr;
  char tmpboard[BNLEN + 1];
  char fpath[64], buf[ANSILINELEN];
  FILE *fpr, *fpw;

  char fpath_log[64];
  char content_log[256];

  /*  �ѨM�H�c������D */
  int comefrom = (xo->dir[0] == 'u') ? 0 : 1;	/* 0: �q�H�c��� 1: �q�ݪO��� */

  /* smiler.080830: �P�_����O�_���Q BBS �ݪ����Y�� */
  int is_bite = 0;
  BRD *xbrd;

  if (!cuser.userlevel)	/* itoc.000213: �קK guest ����h sysop �O */
    return XO_NONE;

  int can_showturn = 0;
  if (xo->dir[0] == 'b')
  {
    if ((currbattr & BRD_NOFORWARD) && !(bbstate & STAT_BM))
    {
      vmsg("���ݪO�T�����");
      return XO_NONE;
    }

    if (currbattr & BRD_SHOWTURN)
      can_showturn = 1;
  }

#ifdef HAVE_REFUSEMARK
  pos = xo->pos;	//smiler 1108
  cur = pos - xo->top;	//smiler 1108
  hdr_org = (HDR *) xo_pool + (xo->pos - xo->top);
  if (hdr_org->xmode & POST_RESTRICT)	/* �Y��L���F��,�ȪO�D�Χ@�̥i��� */
  {
    if (strcmp(hdr_org->owner, cuser.userid) && !(bbstate & STAT_BM))
      return XO_NONE;
  }
#endif

  tag = AskTag("���");
  if (tag < 0)
    return XO_FOOT;

  dir = xo->dir;

  if (!ask_board(xboard, BRD_W_BIT, "\n\n\033[1;33m�ЬD��A���ݪO�A��������W�L�T�O�C\033[m\n\n") ||
    (*dir == 'b' && !strcmp(xboard, currboard)))	/* �H�c�B��ذϤ��i�H�����currboard */
    return XO_HEAD;

  hdr = tag ? &xhdr : (HDR *) xo_pool + (xo->pos - xo->top);	/* lkchu.981201: ������ */

  /* ��@������ۤv�峹�ɡA�i�H��ܡu�������v */
  method = (HAS_PERM(PERM_ALLBOARD) || (!tag && !strcmp(hdr->owner, cuser.userid))) &&
    (HAS_PERM(PERM_ALLBOARD) || comefrom != 0) && (vget(2, 0, "(1)������ (2)����峹�H[2] ", buf, 3, DOECHO) == '1') ? 0 : 1;

  if (!tag)	/* lkchu.981201: �������N���n�@�@�߰� */
  {
    char *title = hdr->title;
    if (method)
    {
      if ((title[0] == '[' && title[5] == ']') && 	/* �� str_ttl(), �d�@�� "[���]" �N�n */
	(title[1] == 0xffffffc2 && title[2] == 0xffffffe0 && title[3] == 0xffffffbf && title[4] == 0xfffffffd))
      {
	title += 6;
	if (*title == ' ')
	  title++;
      }
      sprintf(ve_title, "[���] %.65s", title); /* smiler.070602: �אּ�����,���D��[���]�}�Y */
    }
    else
      strcpy(ve_title, title);

    if (!vget(2, 0, "���D�G", ve_title, TTLEN + 1, GCARRY))
      return XO_HEAD;
  }

  xbno = brd_bno(xboard);
  xbattr = (bshm->bcache + xbno)->battr;

#ifdef HAVE_REFUSEMARK
  if (xbattr & BRD_PUBLIC)
     rc = vget(2, 0, "(S)�s�� (L)���� (Q)�����H[Q] ", buf, 3, LCECHO);
  else
     rc = vget(2, 0, "(S)�s�� (L)���� (X)�K�� (Q)�����H[Q] ", buf, 3, LCECHO);

  if (rc != 'l' && rc != 's' && rc != 'x')
#else
  rc = vget(2, 0, "(S)�s�� (L)���� (Q)�����H[Q] ", buf, 3, LCECHO);
  if (rc != 'l' && rc != 's')
#endif
    return XO_HEAD;

  if ((xbattr & BRD_PUBLIC) && (rc == 'x'))	/* smiler.090308: ���ܤ����O���o���� */
    return XO_HEAD;

  if (method && *dir == 'b')	/* �q�ݪO��X�A���ˬd���ݪO�O�_�����K�O */
  {
    /* �ɥ� tmpbattr */
    tmpbattr = (bshm->bcache + currbno)->readlevel;
    if (tmpbattr == PERM_SYSOP || tmpbattr == PERM_BOARD)
      method = 2;
  }

  /* Thor.990111: �b�i�H��X�e�A�n�ˬd���S����X���v�O? */
  if ((rc == 's') && (!HAS_PERM(PERM_INTERNET) || (xbattr & BRD_NOTRAN)))
    rc = 'l';

  /* �ƥ� currboard */
  if (method)
  {
    /* itoc.030325: �@������I�s ve_header�A�|�ϥΨ� currboard�Bcurrbattr�A���ƥ��_�� */
    strcpy(tmpboard, currboard);
    strcpy(currboard, xboard);
    tmpbattr = currbattr;
    currbattr = xbattr;
  }

  locus = 0;
  do	/* lkchu.981201: ������ */
  {
    if (tag)
    {
      EnumTag(hdr, dir, locus, sizeof(HDR));

      if (method)
	sprintf(ve_title, "Fw: %.68s", str_ttl(hdr->title));	/* �w�� Re:/Fw: �r�˴N�u�n�@�� Fw: */
      else
	strcpy(ve_title, hdr->title);
    }

    if (comefrom)	/* smiler.071114: �ݬ��B�b�ݪO,�U���X��~�ݧ@�P�_ */
    {
      if (hdr->xmode & GEM_FOLDER)	/* �D plain text ������ */
	continue;

#ifdef HAVE_REFUSEMARK
      if (hdr->xmode & POST_RESTRICT)
	continue;
#endif
      if (hdr->xmode & POST_NOFORWARD)
	continue;
    }
    hdr_fpath(fpath, dir, hdr);

#ifdef DO_POST_FILTER
    xbrd = bshm->bcache + xbno;

    if (((xbattr & BRD_BBS_DOG) || xbrd->BM[0] <= ' ') && IS_BBS_DOG_FOOD(fpath))
    {
      brd_fpath(fpath_log, xboard, FN_BBSDOG_LOG);
      sprintf(content_log, "%s BBS�ݪ����p�e: �������\n�@��: %s\n�ӷ�: %s\n���D: %s\n\n",
	Now(), cuser.userid, comefrom ? (method ? tmpboard : currboard) : "�ӤH�H�c", ve_title);
      f_cat(fpath_log, content_log);

      sprintf(fpath_log, FN_ETC_BBSDOG_LOG);
      sprintf(content_log, "%s BBS�ݪ����p�e: �������\n�@��: %s\n�ӷ�: %s\n�ݪO: %s\n���D: %s\n�r��: %s\n\n",
	Now(), cuser.userid, comefrom ? (method ? tmpboard : currboard) : "�ӤH�H�c",
	xboard, ve_title, bbs_dog_str);
      f_cat(fpath_log, content_log);

      vmsg("�z������峹�������������A�Ь��������ȸs");
      is_bite = 1;
      continue;
    }

    if (IS_BRD_DOG_FOOD(fpath, xboard))
    {
      brd_fpath(fpath_log, xboard, FN_BBSDOG_LOG);
      sprintf(content_log, "%s �峹���e����: �������\n�@��: %s\n�ӷ�: %s\n���D: %s\n\n", Now(), cuser.userid, comefrom ? (method ? tmpboard : currboard) : "�ӤH�H�c", ve_title);
      f_cat(fpath_log, content_log);

      sprintf(fpath_log, FN_ETC_BBSDOG_LOG);
      sprintf(content_log, "%s �峹���e����: �������\n�@��: %s\n�ӷ�: %s\n�ݪO: %s\n���D: %s\n�r��: %s\n\n", Now(), cuser.userid, comefrom ? (method ? tmpboard : currboard) : "�ӤH�H�c", xboard, ve_title, bbs_dog_str);
      f_cat(fpath_log, content_log);

      vmsg("�z������峹�������ݪO�����A�Ь��ݪO�O�D");
      is_bite = 1;
      continue;
    }
#endif

#ifdef HAVE_DETECT_CROSSPOST
    if (check_crosspost(fpath, xbno))
      vmsg("�����z�A�Y�ݵo�����šA�Ь� CrossPost �O");
#endif

    brd_fpath(xfolder, xboard, fn_dir);

    if (method)		/* �@����� */
    {
      /* itoc.030325: �@������n���s�[�W header */
      fpw = fdopen(hdr_stamp(xfolder, 'A', &xpost, buf), "w");
      ve_header(fpw);

      /* itoc.040228: �p�G�O�q��ذ�����X�Ӫ��ܡA�|�������� [currboard] �ݪO�A
	 �M�� currboard �����O�Ӻ�ذϪ��ݪO�C���L���O�ܭ��n�����D�A�ҥH�N���ޤF :p */
      fprintf(fpw, "�� ��������� [%s] %s\n\n",
	*dir == 'u' ? cuser.userid : method == 2 ? "���K" : tmpboard,
	*dir == 'u' ? "�H�c" : "�ݪO");

      /* Kyo.051117: �Y�O�q���K�ݪO��X���峹�A�R���峹�Ĥ@��ҰO�����ݪO�W�� */
      finish = 0;
      if ((method == 2) && (fpr = fopen(fpath, "r")))
      {
	if (fgets(buf, sizeof(buf), fpr) &&
	  ((ptr = strstr(buf, str_post1)) || (ptr = strstr(buf, str_post2))) && (ptr > buf))
	{
	  ptr[-1] = '\n';
	  *ptr = '\0';

	  do
	  {
	    fputs(buf, fpw);
	  } while (fgets(buf, sizeof(buf), fpr));
	  finish = 1;
	}
	fclose(fpr);
      }
      if (!finish)
	f_suck(fpw, fpath);

      ve_banner(fpw, 0);
      fclose(fpw);

      strcpy(xpost.owner, cuser.userid);
      strcpy(xpost.nick, cuser.username);
    }
    else		/* ������ */
    {
      /* itoc.030325: ���������� copy �Y�i */
      hdr_stamp(xfolder, HDR_COPY | 'A', &xpost, fpath);

      strcpy(xpost.owner, hdr->owner);
      strcpy(xpost.nick, hdr->nick);
      strcpy(xpost.date, hdr->date);	/* �������O�d���� */
    }

    strcpy(xpost.title, ve_title);

    if (rc == 's')
      xpost.xmode = POST_OUTGO;
#ifdef HAVE_REFUSEMARK
    else if (rc == 'x')
      xpost.xmode = POST_RESTRICT;
#endif

    rec_bot(xfolder, &xpost, sizeof(HDR));

    if (rc == 's')
      outgo_post(&xpost, xboard);


    char str_tag_score[50];
    sprintf(str_tag_score, "����� %s �ݪO ", xboard);

    if (can_showturn)	/* �u���ݪO�~���i�঳ can_showturn */
      post_t_score(xo,str_tag_score,hdr);

  } while (++locus < tag);

  btime_update(xbno);

  /* Thor.981205: check �Q�઺�O���S���C�J����? */
  if (!(xbattr & BRD_NOCOUNT))
    cuser.numposts += tag ? tag : 1;	/* lkchu.981201: �n�� tag */

  /* �_�� currboard�Bcurrbattr */
  if (method)
  {
    strcpy(currboard, tmpboard);
    currbattr = tmpbattr;
  }

  if (!is_bite)
    vmsg("�������");
  else
    vmsg("�����������");

  return XO_HEAD;
}


int
post_forward(xo)
  XO *xo;
{
  ACCT muser;
  HDR *hdr;

  if (!HAS_PERM(PERM_LOCAL))
    return XO_NONE;

  hdr = (HDR *) xo_pool + (xo->pos - xo->top);

  if (hdr->xmode & GEM_FOLDER)	/* �D plain text ������ */
    return XO_NONE;

#ifdef HAVE_REFUSEMARK
  if (hdr->xmode & POST_RESTRICT)	/* �Y��L���F��,�ȪO�D�Χ@�̥i��� */
  {
    if (strcmp(hdr->owner, cuser.userid) && !(bbstate & STAT_BM))
      return XO_NONE;
  }
#endif

  if (xo->dir[0] == 'b')
  {
    if ((currbattr & BRD_NOFORWARD) && !(bbstate & STAT_BM))
    {
      vmsg("���ݪO�T�����");
      return XO_NONE;
    }
    if (hdr->xmode & POST_NOFORWARD)		/* ���g�峹�T�� */
      return XO_NONE;
  }

  move(b_lines - 1, 0);
  clrtoeol();
  outs("\033[1;44m�� �Y�z�n����A�Ч�Ϋ��� \033[33m%\033[37m (shift + 5) �άO \033[33me\033[37m �I\033[m");

  if (acct_get("��F�H�󵹡G", &muser) > 0)
  {
    strcpy(quote_user, hdr->owner);
    strcpy(quote_nick, hdr->nick);
    hdr_fpath(quote_file, xo->dir, hdr);
    sprintf(ve_title, "%.64s (fwd)", hdr->title);
    move(1, 0);
    clrtobot();
    prints("��F��: %s (%s)\n��  �D: %s\n", muser.userid, muser.username, ve_title);

    mail_send(muser.userid);
    *quote_file = '\0';

    char str_tag_score[50];
    sprintf(str_tag_score, "��H�� %s ��bbs�H�c ", muser.userid);
    if (xo->dir[0] == 'b')
    {
      if (currbattr & BRD_SHOWTURN)
	post_t_score(xo,str_tag_score,hdr);
    }
  }
  return XO_HEAD;
}


/* ----------------------------------------------------- */
/* �O�D�\��Gmark / delete / label			 */
/* ----------------------------------------------------- */


static int
post_mark(xo)
  XO *xo;
{
  if (bbstate & STAT_BOARD)
  {
    HDR *hdr;
    int pos, cur, xmode;

    pos = xo->pos;
    cur = pos - xo->top;
    hdr = (HDR *) xo_pool + cur;
    xmode = hdr->xmode;

#ifdef HAVE_LABELMARK
    if (xmode & POST_DELETE)	/* �ݬ媺�峹���� mark */
      return XO_NONE;
#endif

    if (xmode & POST_GOOD)
    {
      vmsg("���g���u��A�Ы� M ��������g�u��");
      return XO_FOOT;
    }

    hdr->xmode = xmode ^ POST_MARKED;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono);

    move(3 + cur, 7);
    outs(post_attr(hdr++));
  }

  return XO_NONE;
}


/* smiler.080827: �]�w�u�� */
static int
post_mark_good(xo)
  XO *xo;
{
  ACCT x, acct;
  char buf[512];
  char fpath[64];

  if (bbstate & STAT_BOARD)
  {
    HDR *hdr;
    int pos, cur, xmode;

    pos = xo->pos;
    cur = pos - xo->top;
    hdr = (HDR *) xo_pool + cur;
    xmode = hdr->xmode;

    if (!strncmp(currboard, "P_", 2) || !strncmp(currboard, "R_", 2))
    {
      vmsg("�ӤH�ݪO�A��O���}���u��]�w");
      return XO_FOOT;
    }

    if (!strcmp(hdr->owner, cuser.userid))
    {
      vmsg("�ۤv���i�[�ۤv�u��");
      return XO_BODY;
    }

#ifdef HAVE_LABELMARK
    if (xmode & POST_DELETE)	/* �ݬ媺�峹���� mark */
      return XO_NONE;
#endif

    if ((xmode & POST_MARKED) && (!(xmode & POST_GOOD)))
    {
      vmsg("�Х��� m �������g�峹�аO�A�A�� M ���]���g�峹���u��");
      return XO_BODY;
    }

    if ( (!strchr(hdr->owner, '.')) && (acct_load(&acct, hdr->owner) >= 0))
    {
      memcpy(&x, &acct, sizeof(ACCT));
      x.good_article = acct.good_article;
      if (vans(msg_sure_ny) != 'y')
	return XO_BODY;
      else
      {
	usr_fpath(fpath, acct.userid, FN_GOOD_ARTICLE);
	hdr->xmode = xmode ^ POST_MARKED;
	if (hdr->xmode & POST_MARKED)
	{
	  hdr->xmode |= POST_GOOD;
	  x.good_article++;
	  sprintf(buf, "%s %-13s �u��W %s %s %s\n", Now(), cuser.userid, currboard, hdr->xname, hdr->title);
	}
	else
	{
	  hdr->xmode &= (~POST_GOOD);
	  x.good_article--;
	  sprintf(buf, "%s %-13s �u��� %s %s %s\n", Now(), cuser.userid, currboard, hdr->xname, hdr->title);
	}

	/* smiler.080827: �ҥ� acct_setup() ���������ϥΪ̸�ƼҦ� */
	utmp_admset(x.userno, STATUS_DATALOCK | STATUS_COINLOCK);
	memcpy(&acct, &x, sizeof(ACCT));
	acct_save(&acct);
	f_cat(fpath, buf);
      }
    }

    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono);

    return XO_INIT;
  }
}


static void
reset_parent_chrono(hdd, ram)	/*ryancid: reset parent_chrono*/
  HDR *hdd, *ram;
{
  hdd->parent_chrono = 0;
}


static void
set_parent_chrono(hdd, ram)	/*ryancid: set parent_chrono*/
  HDR *hdd, *ram;
{
  hdd->parent_chrono = 1;
}


static int
post_bottom(xo)
  XO *xo;
{
  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  int pos;
  HDR *hdr, post;
  char fpath[64];

  hdr = (HDR *) xo_pool + (xo->pos - xo->top);
  pos = xo->pos;

  if (!(hdr->xmode & POST_BOTTOM))
  {
    if (hdr->parent_chrono)	/* Allow only one bottom post per article */
      return post_load(xo);

    hdr_fpath(fpath, xo->dir, hdr);
    hdr_stamp(xo->dir, HDR_LINK | 'A', &post, fpath);
    post.parent_chrono = hdr->chrono;
#ifdef HAVE_REFUSEMARK
    post.xmode = POST_BOTTOM | (hdr->xmode & POST_RESTRICT);
#else
    post.xmode = POST_BOTTOM;
#endif
    /*ryancid: copy the score*/
    if (hdr->xmode & POST_SCORE)
    {
      post.xmode |= POST_SCORE;
      post.score = hdr->score;
    }

    strcpy(post.owner, hdr->owner);
    strcpy(post.nick, hdr->nick);
    strcpy(post.mark, "���n");
    post.color = '3';
    strcpy(post.title, hdr->title);

    vget(b_lines, 0, "�п�J���ܤ�r�G", post.mark, 5, GCARRY);
    post.color = vans("�������C�� 0) \033[1;30m�i\033[m 1) \033[1;31m�i\033[m 2) \033[1;32m�i\033[m "
      "3) \033[1;33m�i\033[m ""4) \033[1;34m�i\033[m 5) \033[1;35m�i\033[m "
      "6) \033[1;36m�i\033[m 7) \033[1;37m�i\033[m [3] ");

    if (post.color < '0' || post.color > '7')
      post.color = '3';

    rec_add(xo->dir, &post, sizeof(HDR));

    /*ryancid: set the parent_chrono*/
    currchrono = hdr->chrono;
    rec_ref(xo->dir, hdr, sizeof(HDR),
      xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono , set_parent_chrono);
  }
  else if (hdr->xmode & POST_MARKED)
  {
    vmsg("�Х����� m �аO�A�R���m���峹�I");
    return XO_FOOT;
  }
  else
  {
    if (vans("�T�w�n�������g�m���峹(y/N)�H[N] ") != 'y')
      return XO_FOOT;

    /*reset the original chrono*/
    currchrono = hdr->parent_chrono;
    rec_ref(xo->dir, hdr, sizeof(HDR),
      xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono , reset_parent_chrono);

    currchrono = hdr->chrono;
    if (!rec_del(xo->dir, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono))
    {
      pos = move_post(hdr, xo->dir, bbstate & STAT_BOARD);
    }
  }

  return post_load(xo); /* ckm.070325: ���s���J�C�� */
}


#ifdef HAVE_REFUSEMARK
static void
RefusePal_fpath(fpath, board, mode, hdr)
  char *fpath;
  char *board;
  char mode;	/* 'C':Cansee  'R':Refimage */
  HDR *hdr;
{
  sprintf(fpath, "brd/%s/RefusePal_DIR/%s_%s",
    board, mode == 'C' ? "Cansee" : "Refimage", hdr->xname);
}


void
RefusePal_kill(board, hdr)	/* amaki.030322: �ΨӬ�W��p�� */
  char *board;
  HDR *hdr;
{
  char fpath[64];

  RefusePal_fpath(fpath, board, 'C', hdr);
  unlink(fpath);
  RefusePal_fpath(fpath, board, 'R', hdr);
  unlink(fpath);
}


int	/* -1:�[�K(L)��  1:�n��(F)��  0:���q�� */
RefusePal_level(board, hdr)	//smiler 1108
  char *board;
  HDR *hdr;
{
  int fsize;
  char fpath[64];
  int *fimage;
  struct stat st;

  if (!(hdr->xmode & POST_RESTRICT))
    return 0;

  RefusePal_fpath(fpath, board, 'R', hdr);	//0709
  if (dashf(fpath))	/* �� POST_RESTRICT �� pal �ɤ��s�b���N�O�[�K(L)�� */
  {
    if ((!stat(fpath, &st) && S_ISREG(st.st_mode) && !st.st_size) ||
      !(hdr->xmode & POST_FRIEND))	/* �� pal �ɦӨS POST_FRIEND */
    {	/* ���ǦW�檺 size �� 0 */
      RefusePal_kill(currboard, hdr);
      return -1;
    }

    if (!strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM))
      return 1;

    if (fimage = (int *) f_img(fpath, &fsize))
    {
      fsize = belong_pal(fimage, fsize / sizeof(int), cuser.userno);
      free(fimage);
      if (fsize)
	return fsize;
    }
  }

  return -1;
}


int	/* 1:�b�i���W��W 0:���b�i���W��W */
RefusePal_belong(board, hdr)
  char *board;
  HDR *hdr;
{
  int fsize;
  char fpath[64];
  int *fimage;

  if (!strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM))
    return 1;

  RefusePal_fpath(fpath, board, 'R', hdr);
  if (fimage = (int *) f_img(fpath, &fsize))
  {
    fsize = belong_pal(fimage, fsize / sizeof(int), cuser.userno);
    free(fimage);
    return fsize;
  }
  return 0;
}


static void
refusepal_cache(hdr, board)
  HDR *hdr;
  char *board;
{
  int fd, max;
  char fpath[64];
  int pool[PAL_MAX];

  RefusePal_fpath(fpath, board, 'C', hdr);

  if (max = image_pal(fpath, pool))
  {
    RefusePal_fpath(fpath, board, 'R', hdr);
    if ((fd = open(fpath, O_WRONLY | O_CREAT | O_TRUNC, 0600)) >= 0)
    {
      write(fd, pool, max * sizeof(int));
      close(fd);
    }
  }
  else
    RefusePal_kill(board, hdr);
}


static int
XoBM_Refuse_pal(hdr)
  HDR *hdr;
{
  XO *xt;
  char fpath[64];
  char fpath_friend[64];
  int ans, ans2;

  if (strcmp(hdr->owner, cuser.userid) && !(bbstate & STAT_BM))
    return 0;

  brd_fpath(fpath, currboard, "RefusePal_DIR");
  if (!dashd(fpath))
    mkdir(fpath, 0700);
  RefusePal_fpath(fpath, currboard, 'C', hdr);

  if (dashf(fpath))	/* �ק��ª��W�� */
    ans = '3';
  else if (bbstate & STAT_BM)
    ans = vans("����� 1)�n�� 2)�O�� 3)�s��s�W�� Q)�����[�K [Q] ");
  else
    ans = '1';

  switch (ans)
  {
  case '1':
    ans = vans("����� 1~5)�n�͸s�զW�� 6)�s��s�W�� Q)�����[�K [Q] ");
    if (ans < '1' || ans > '6')
      ans = -1;
    else if (ans == '6')
      ans2 = '9';
    else	/* 1-5, �ޤJ�ӤH�n�͸s�զW�� */
      ans2 = 0;
    break;

  case '2':
    ans2 = vans("����� 0)�O�ͦW�� 1~8)�O�ͯS�O�W�� 9)�s��s�W�� Q)�����[�K [Q] ");
    if (ans2 < '0' || ans2 > '9')
      ans = -1;
    break;

  case '3':
    ans2 = '9';
    break;

  default:
    return 0;
  }

  if (ans == -1)
    return 0;

  if (!ans2)		/* ans:1-5, ans2 �|�����, �ޤJ�ӤH�n�͸s�զW�� */
  {
    usr_fpath(fpath_friend, cuser.userid, "list.0");
    fpath_friend[strlen(fpath_friend) - 1] = ans;
  }
  else if (ans2 != '9')	/* �ޤJ�O��/�O�ͯS�O�W�� */
  {
    if (ans2 == '0')	/* �ޤJ�O�ͦW�� */
      brd_fpath(fpath_friend, currboard, "friend");
    else
      sprintf(fpath_friend, "brd/%s/friend_%c", currboard, ans2);
  }
  else			/* (ans2 = '9') �s��s�W�� */
  {
    xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath);
    xt->key = PALTYPE_BPAL;	//smiler 1106
    xover(XZ_PAL);
//    (xo->key == XZ_XPOST) ? xpost_init(xo) : post_init(xo);
    refusepal_cache(hdr, currboard);
    free(xt);
  }

  if (ans2 != '9')
  {
    vmsg("�ޤJ�W��I");
    f_cp(fpath_friend, fpath, O_TRUNC);
    xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath);
    xt->key = PALTYPE_BPAL;	//smiler 1106
    xover(XZ_PAL);
//    (xo->key == XZ_XPOST) ? xpost_init(xo) : post_init(xo);
    refusepal_cache(hdr, currboard);
    free(xt);
  }

  vmsg("�]�w�����I");
  return 1;
}


static int
post_refuse(xo)	/* itoc.010602: �峹�[�K */
  XO *xo;
{
  HDR *hdr, fhdr;
  int pos, cur, ans, xmode;

  if (!cuser.userlevel)	/* itoc.020114: guest ������L guest ���峹�[�K */
    return XO_NONE;

  if (currbattr & BRD_PUBLIC)
    return XO_NONE;

  if ((currbattr & BRD_NOL) && !(bbstate & STAT_BM))
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;
  xmode = hdr->xmode;

  /* XoBM_Refuse_pal() �i xover(XZ_PAL) ��, �|���g xo_pool, �o�̥��s�_�� */
  memcpy(&fhdr, hdr, sizeof(HDR));

  if (!strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM))
  {
    switch (RefusePal_level(currboard, hdr))
    {
    case 0:	/* �@��峹 */
      ans = vans("�N���g�峹 1)�[�K 2)�]���n�ͤ� Q)�����H[Q] ");
      break;

    case -1:	/* �[�K(L)�峹 */
      ans = vans("�N���g�峹 1)�ѱK 2)�]���n�ͤ� Q)�����H[Q] ");
      break;

    case 1:	/* �n��(F)�峹 */
      ans = vans("�N���g�峹 1)�[�K/�ѱK(�N�尣�i���n�ͦW��) 2)�ק�i���n�ͦW�� Q)�����H[Q] ");
      if (ans == '1')
      {
	ans = vans("�N���g�峹 1)�[�K 2)�ѱK (�[�ѱK���N�尣�i���n�ͦW��) Q)�����H[Q] ");
	if ((ans == '1') || (ans == '2'))
	{
	  xmode &= ~POST_FRIEND;
	  RefusePal_kill(currboard, hdr);
	  if (ans == '1')		/* ������, �ݷ|�A�[�^�h */
	    xmode ^= POST_RESTRICT;
	  else
	    ans = '1';
	}
      }
      break;
    }

    switch (ans)
    {
    case '1':	/* ����[�ѱK�ʧ@ */
//      xmode &= ~POST_FRIEND;	/* �H���U�@ */
//      RefusePal_kill(currboard, hdr);
      xmode ^= POST_RESTRICT;
      break;

    case '2':	/* �s��i���n�ͦW�� */
      xmode |= (POST_RESTRICT | POST_FRIEND);
      XoBM_Refuse_pal(&fhdr);		/* �Ǧ^ 0 ��ܨ����ʧ@ */
      break;

    default:	/* �����ʧ@ */
      return XO_FOOT;
    }

    fhdr.xmode = xmode;
    currchrono = fhdr.chrono;
    rec_put(xo->dir, &fhdr, sizeof(HDR), xo->key == XZ_XPOST ? fhdr.xid : pos, cmpchrono);
    btime_update(brd_bno(currboard));	/* �ѨM�]�L�k�\Ū���Ӥ@���G�O�����p */
    return XO_LOAD;
  }

  return XO_NONE;
}


static int
XoBM_add_pal()
{
  int ans;
  char fpath[64];
  XO *xt;

  if (!(bbstate & STAT_BM))
    return 0;

  ans = vans("�� �s��O�ͯS�O�W�� 1~8�H[Q] ");
  if (ans > '0' && ans < '9')
  {
    sprintf(fpath, "brd/%s/friend_%c", currboard, ans);
    xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath);
    xt->key = PALTYPE_BPAL;	//smiler 1106
    xover(XZ_PAL);
    free(xt);
    return 1;
  }
  else
    return 0;
}
#endif


#ifdef HAVE_LABELMARK
static int
post_label(xo)
  XO *xo;
{
  if (bbstate & STAT_BOARD)
  {
    HDR *hdr;
    int pos, cur, xmode;

    pos = xo->pos;
    cur = pos - xo->top;
    hdr = (HDR *) xo_pool + cur;
    xmode = hdr->xmode;

    if (xmode & (POST_MARKED | POST_RESTRICT))	/* mark �� �[�K���峹����ݬ� */
      return XO_NONE;

    hdr->xmode = xmode ^ POST_DELETE;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono);

    move(3 + cur, 7);
    outs(post_attr(hdr));

    return pos + 1 + XO_MOVE;	/* ���ܤU�@�� */
  }

  return XO_NONE;
}


static int
post_delabel(xo)
  XO *xo;
{
  int fdr, fsize, xmode;
  char fnew[64], fold[64], *folder;
  HDR *hdr;
  FILE *fpw;

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  if (vans("�T�w�n�R���ݬ�峹��(Y/N)�H[N] ") != 'y')
    return XO_FOOT;

  folder = xo->dir;
  if ((fdr = open(folder, O_RDONLY)) < 0)
    return XO_FOOT;

  if (!(fpw = f_new(folder, fnew)))
  {
    close(fdr);
    return XO_FOOT;
  }

  fsize = 0;
  mgets(-1);
  while (hdr = mread(fdr, sizeof(HDR)))
  {
    xmode = hdr->xmode;

    if (!(xmode & POST_DELETE))
    {
      if ((fwrite(hdr, sizeof(HDR), 1, fpw) != 1))
      {
	close(fdr);
	fclose(fpw);
	unlink(fnew);
	return XO_FOOT;
      }
      fsize++;
    }
    else
    {
      /* �s�u��H */
      cancel_post(hdr);

      hdr_fpath(fold, folder, hdr);
      unlink(fold);
      if (xmode & POST_RESTRICT)
	RefusePal_kill(currboard, hdr);

    }
  }
  close(fdr);
  fclose(fpw);

  sprintf(fold, "%s.o", folder);
  rename(folder, fold);
  if (fsize)
    rename(fnew, folder);
  else
    unlink(fnew);

  btime_update(currbno);

  return post_load(xo);
}
#endif


static void
backup_post_log(fpath, brdname, hdr, mode)
  char *fpath;		/* ��� */
  char *brdname;	/* �ؼЬݪO */
  HDR *hdr;		/* ��l hdr */
  int mode;		/* 0: �O�� hdr ��@��  1: �O�� cuser */
{
  HDR bhdr;
  char folder[64];

  brd_fpath(folder, brdname, FN_DIR);

  hdr_stamp(folder, HDR_COPY | 'A', &bhdr, fpath);
  strcpy(bhdr.title, hdr->title);
  strcpy(bhdr.owner, mode ? cuser.userid : hdr->owner);
  strcpy(bhdr.nick, mode ? cuser.username : hdr->nick);
  /* bhdr.xmode = POST_OUTGO; */
  rec_bot(folder, &bhdr, sizeof(HDR));
  btime_update(brd_bno(brdname));
}


static int
post_delete(xo)	/* ��@�R�� */
  XO *xo;
{
  ACCT x, acct;
  int pos, cur, by_BM;
  HDR *hdr;
  char buf[256];
  char fpath[64], reason[55], title[70];

  /* smiler.1111: �O�@Deletelog �H�� Editlog �� �O�����Q���� */
  if (!strcmp(currboard,BN_DELLOG) || !strcmp(currboard,BN_EDITLOG))
    return XO_NONE;

  if (!cuser.userlevel || !strcmp(currboard, BN_DELETED) || !strcmp(currboard, BN_JUNK))
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;

  if (hdr->xmode & POST_BOTTOM)
    return post_bottom(xo);

  if ((hdr->xmode & POST_MARKED) ||
    (!(bbstate & STAT_BOARD) && (strcmp(hdr->owner, cuser.userid) || (currbattr & BRD_NODELETE))))
    return XO_NONE;

  by_BM = bbstate & STAT_BOARD;

  if (vans(msg_del_ny) == 'y')
  {
    /* smiler.080827: �D�ۤv���ɡA�i�]�w�O�_�n�H�h */
    if (strcmp(hdr->owner, cuser.userid) && (!strchr(hdr->owner, '.')) && (acct_load(&acct, hdr->owner) >= 0))
    {
      switch (vans("�峹�w�R���A�B�~�ʧ@ 1)�H�H�q����@�� 2)�H�h N)�K�F,�����R���H[N]"))
      {
      case '1':
	if (!vget(b_lines, 0, "�п�J�z�ѡG", reason, 55, DOECHO))
	{
	  vmsg("����");
	  return XO_BODY;
	}

	hdr_fpath(fpath, xo->dir, hdr);
	sprintf(title, "[�峹�R��] �z��: %s", reason);
	mail_him(fpath, acct.userid, title, 0);
	break;

      case '2':
	if (vans("�n�H�h���g�峹(y/N)�H[N]") == 'y')
	{
	  if (!vget(b_lines, 0, "�п�J�z�ѡG", reason, 55, DOECHO))
	  {
	    vmsg("����");
	    return XO_BODY;
	  }

	  memcpy(&x, &acct, sizeof(ACCT));
	  x.poor_article = acct.poor_article;

	  usr_fpath(fpath, acct.userid, FN_POOR_ARTICLE);
	  x.poor_article++;
	  sprintf(buf, "%s %-13s �H�h�W %s %s %s %s\n",
	    Now(), cuser.userid, currboard, hdr->xname, hdr->title, reason);
	  /* smiler.080827: �ҥ� acct_setup() ���������ϥΪ̸�ƼҦ�
	  ----------------------------------------------------------------------------------
	     itoc.010811: �ʺA�]�w�u�W�ϥΪ�
	     �Q������L��ƪ��u�W�ϥΪ�(�]�A�����ۤv)�A�� cutmp->status �|�Q�[�W STATUS_DATALOCK
	     �o�ӺX�СA�N�L�k acct_save()�A��O�����K�i�H�ק�u�W�ϥΪ̸��
	     �b�����ק�L�~�W�u�� ID �]���� cutmp->status �S�� STATUS_DATALOCK ���X�СA
	     �ҥH�N�i�H�~��s���A�ҥH�u�W�p�G�P�ɦ��ק�e�B�ק�᪺�P�@�� ID multi-login�A�]�O�L���C
	  ---------------------------------------------------------------------------------- */
	  utmp_admset(x.userno, STATUS_DATALOCK | STATUS_COINLOCK);
	  memcpy(&acct, &x, sizeof(ACCT));
	  acct_save(&acct);
	  f_cat(fpath, buf);

	  hdr_fpath(fpath, xo->dir, hdr);
	  sprintf(title, "[�H�h] �z��: %s", reason);
	  mail_him(fpath, acct.userid, title, 0);
	}
	break;

      default:
	break;
      }
    }

    currchrono = hdr->chrono;

    if (!rec_del(xo->dir, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono))
    {
      pos = move_post(hdr, xo->dir, by_BM);

      if (!by_BM && !(currbattr & BRD_NOCOUNT))
      {
	/* itoc.010711: ��峹�n�����A���ɮפj�p */
	pos = pos >> 3;	/* �۹�� post �� wordsnum / 10 */

	/* itoc.010830.����: �|�}: �Y multi-login �夣��t�@������ */
	if (cuser.money > pos)
	  cuser.money -= pos;
	else
	  cuser.money = 0;

	if (cuser.numposts > 0)
	  cuser.numposts--;
	sprintf(buf, "%s�A�z���峹� %d �g", MSG_DEL_OK, cuser.numposts);
	vmsg(buf);
      }

      if (xo->key == XZ_XPOST)
      {
	vmsg("��C��g�R����V�áA�Э��i�걵�Ҧ��I");
	return XO_QUIT;
      }
      return XO_LOAD;
    }
  }
  return XO_FOOT;
}


static int
chkpost(hdr)
  HDR *hdr;
{
  return ((hdr->xmode & POST_MARKED) || (hdr->xmode & POST_BOTTOM));
}


static int
vfypost(hdr, pos)
  HDR *hdr;
  int pos;
{
  return (Tagger(hdr->chrono, pos, TAG_NIN) || chkpost(hdr));
}


static void
delpost(xo, hdr)
  XO *xo;
  HDR *hdr;
{
  char fpath[64];

  cancel_post(hdr);
  hdr_fpath(fpath, xo->dir, hdr);
  if (!(hdr->xmode & POST_INCOME) && deletelog_use)	/* ���~�H�R�����ƥ�, �q news server �W����Y�i */
    backup_post_log(fpath, BN_DELLOG, hdr, 1);
  unlink(fpath);
  if (hdr->xmode & POST_RESTRICT)
    RefusePal_kill(currboard, hdr);
}


static int
post_rangedel(xo)
  XO *xo;
{
  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  /* smiler.071111: �O�@Deletelog �H�� Editlog �� �O�����Q���� */
  if (!strcmp(currboard,BN_DELLOG) || !strcmp(currboard, BN_EDITLOG))
    return XO_NONE;

  btime_update(currbno);

  return xo_rangedel(xo, sizeof(HDR), chkpost, delpost);
}


static int
post_prune(xo)
  XO *xo;
{
  int ret;

  /* smiler.071111: �O�@ BN_DELLOG �H�� BN_EDITLOG ���O�����Q���� */
  if (!strcmp(currboard, BN_DELLOG) || !strcmp(currboard, BN_EDITLOG))
    return XO_NONE;

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  ret = xo_prune(xo, sizeof(HDR), vfypost, delpost);

  btime_update(currbno);

  if (xo->key == XZ_XPOST && ret == XO_LOAD)
  {
    vmsg("��C��g�妸�R����V�áA�Э��i�걵�Ҧ��I");
    return XO_QUIT;
  }

  return ret;
}


static int
post_copy(xo)	   /* itoc.010924: ���N gem_gather */
  XO *xo;
{
  int tag;

  tag = AskTag("�ݪO�峹����");

  if (tag < 0)
    return XO_FOOT;

  if (!(bbstate & STAT_BM))
    return XO_NONE;

#ifdef HAVE_REFUSEMARK
  gem_buffer(xo->dir, tag ? NULL : (HDR *) xo_pool + (xo->pos - xo->top), chkrescofo, 1);
#else
  gem_buffer(xo->dir, tag ? NULL : (HDR *) xo_pool + (xo->pos - xo->top), chkrescofo, 1);
#endif

  if (bbstate & STAT_BOARD)
  {
#ifdef XZ_XPOST
    if (xo->key == XZ_XPOST)
    {
      zmsg("�ɮ׼аO�����C[�`�N] �z���������}�걵�Ҧ��~��i�J��ذϡC");
      return XO_FOOT;
    }
    else
#endif
    {
      zmsg("���������C[�`�N] �K�W��~��R�����I");
      return post_gem(xo);	/* �����������i��ذ� */
    }
  }

  zmsg("�ɮ׼аO�����C[�`�N] �z�u��b���(�p)�O�D�Ҧb�έӤH��ذ϶K�W�C");
  return XO_FOOT;
}


/* ----------------------------------------------------- */
/* �����\��Gedit / title					 */
/* ----------------------------------------------------- */


#ifdef DO_POST_FILTER
static int
find_xname_by_chrono(chrono, xpath, mode)	/* �t�X�ݪ����A�ק�峹���ƥ��@�֭ק� */
  time4_t chrono;
  char *xpath;
  int mode;	/* 0: �q����m��  1: �q�m������ */
{
  char fpath[64];
  int fsize, find = 0;
  HDR *data, *head, *tail;

  brd_fpath(fpath, currboard, FN_DIR);
  if (data = (HDR *) f_img(fpath, &fsize))
  {
    head = data;
    tail = data + (fsize / sizeof(HDR));
    do
    {
      if (mode)
      {
	if (head->chrono == chrono)
	{
	  find = 1;
	  hdr_fpath(xpath, fpath, head);
	  break;
	}
      }
      else
      {
	if (head->parent_chrono == chrono)
	{
	  find = 1;
	  hdr_fpath(xpath, fpath, head);
	  break;
	}
      }
    } while (++head < tail);
    free(data);
  }

  return find;
}


static void	/* �ק�峹��A�յ۱N���P�m����P�B */
post_bottom_sync(hdr, fpath)
  HDR *hdr;
  char *fpath;
{
  char xpath[64];

  if (hdr->xmode & POST_BOTTOM)	/* �ק�m����A�h����@�_�� */
  {
    if (find_xname_by_chrono(hdr->parent_chrono, xpath, 1))
    {
      unlink(xpath);
      f_ln(fpath, xpath);
    }
  }
  else if (hdr->parent_chrono)	/* ���g�峹���m���ƥ��A�@�֧�蠟 */
  {
    if (find_xname_by_chrono(hdr->chrono, xpath, 0))
    {
      unlink(xpath);
      f_ln(fpath, xpath);	/* ���] link, �H�Ϥ���P�B */
    }
  }
}
#endif


static int
post_edit_merge(hdr, fpath)
  HDR *hdr;
  char *fpath;
{
  char tmp[64], ori[64], patch[64];
  char cmd[128];
  FILE *fp;
  time4_t stamp = 0;
  struct stat st;

  sprintf(ori, "tmp/%s.%s.ori", currboard, hdr->xname);
  if (dashf(ori))
  {
    vmsg("����L�H���b�ק�o�g�峹");
    return (cutmp->mode == M_READA) ? XO_HEAD : XO_NONE;
  }
  f_cp(fpath, ori, O_TRUNC);

  if (!stat(fpath, &st))
    stamp = st.st_size;

  sprintf(tmp, "tmp/%s.%s.tmp", currboard, hdr->xname);
  f_cp(fpath, tmp, O_TRUNC);	/* tmp �s�ƥ��A�åB���Ӱ��s�� */

  /* �� tmp �U�h���s��A�ӫD��l�� */
#ifdef DO_POST_FILTER	/* smiler.080830: �w��峹���D���e�������L�����B */
  if (vedit(tmp, 0) || post_filter(tmpfile, hdr->title))
#else
  if (vedit(tmp, 0))
#endif
  {
    unlink(ori);
    unlink(tmp);
    return 1;
  }
  else	/* �Y�D�����h�[�W�ק��T */
  {
    if (str_cmp(cuser.userid, str_sysop) && (fp = fopen(tmp, "a")))
    {
      ve_banner(fp, 1);
      fclose(fp);
    }
  }

#if 1
  sprintf(patch, "tmp/%s.%s.patch", currboard, hdr->xname);
  sprintf(cmd, "diff -Nau %s %s > %s", ori, tmp, patch);
  system(cmd);
  backup_post_log(patch, BN_EDITLOG, hdr, 0);	/* �O���ק�e��t�� */
#else
//  backup_post_log(fpath, BN_EDITLOG, hdr, 0);	/* �ק�e����l�峹�ƥ� */
//  backup_post_log(tmp, BN_EDITLOG, hdr, 0);	/* ��ק�L�᪺�峹�ƥ� */
#endif

  if (!stat(fpath, &st) && (stamp == st.st_size))	/* ��l�ɮץ��Q�ק�: �S���s���� */
  {
    unlink(fpath);
    f_mv(tmp, fpath);
  }
  else		/* ��l�ɮפw�Q�ק�A�i��X�� */
  {
    sprintf(cmd, "patch -s %s %s", fpath, patch);
    system(cmd);

    unlink(tmp);
    sprintf(tmp, "%s.orig", fpath);
    unlink(tmp);
  }

  unlink(ori);
  unlink(patch);
  post_bottom_sync(hdr, fpath);	/* �ק�峹��A�յ۱N���P�m����P�B */

  return 0;
}


static void change_stamp(char *folder, HDR *hdr);


int
post_edit(xo)
  XO *xo;
{
  char fpath[64];
  HDR *hdr;

  /* smiler.071111 �O�@ Editlog �H�� Deletelog �O���ƥ���� */
  if (!strcmp(currboard, BN_DELLOG) || !strcmp(currboard, BN_EDITLOG))
    return XO_NONE;

  hdr = (HDR *) xo_pool + (xo->pos - xo->top);
  hdr_fpath(fpath, xo->dir, hdr);

  curredit = 0;

  if (!str_cmp(cuser.userid, str_sysop))	/* �����ק� */
  {
#ifdef HAVE_REFUSEMARK
    if (!chkrestrict(hdr))
      return (cutmp->mode == M_READA) ? XO_HEAD : XO_NONE;
#endif

    if (!post_edit_merge(hdr, fpath))
    {
      change_stamp(xo->dir, hdr);
      currchrono = hdr->chrono;
      rec_put(xo->dir, hdr, sizeof(HDR), xo->pos, cmpchrono);
      post_history(xo, hdr);
    }
  }
  else if ((cuser.userlevel && !strcmp(hdr->owner, cuser.userid)) || (bbstate & STAT_BM))	/* �O�D/��@�̭ק� */
  {
    if (!(bbstate & STAT_BM) && (currbattr & BRD_NODELETE))
    {
      vedit(fpath, -1);
      return XO_HEAD;
    }

    if (!post_edit_merge(hdr, fpath))
    {
      change_stamp(xo->dir, hdr);
      currchrono = hdr->chrono;
      rec_put(xo->dir, hdr, sizeof(HDR), xo->pos, cmpchrono);
      post_history(xo, hdr);
    }
  }
  else		/* itoc.010301: ���ѨϥΪ̭ק�(�������x�s)��L�H�o���峹 */
#if 1
  {
    if (!chkrescofo(hdr))
      return (cutmp->mode == M_READA) ? XO_HEAD : XO_NONE;

    vedit(fpath, -1);
  }
#else
    return (cutmp->mode == M_READA) ? XO_HEAD : XO_NONE;
#endif

  /* return post_head(xo); */
  return XO_HEAD;	/* itoc.021226: XZ_POST �M XZ_XPOST �@�� post_edit() */
}


void
header_replace(xo, hdr)		/* itoc.010709: �ק�峹���D���K�ק鷺�媺���D */
  XO *xo;
  HDR *hdr;
{
  FILE *fpr, *fpw;
  char srcfile[64], tmpfile[64], buf[ANSILINELEN];

  hdr_fpath(srcfile, xo->dir, hdr);
  strcpy(tmpfile, "tmp/");
  strcat(tmpfile, hdr->xname);
  f_cp(srcfile, tmpfile, O_TRUNC);

  if (!(fpr = fopen(tmpfile, "r")))
    return;

  if (!(fpw = fopen(srcfile, "w")))
  {
    fclose(fpr);
    return;
  }

  fgets(buf, sizeof(buf), fpr);		/* �[�J�@�� */
  fputs(buf, fpw);

  fgets(buf, sizeof(buf), fpr);		/* �[�J���D */
  if (!str_ncmp(buf, "��", 2))		/* �p�G�� header �~�� */
  {
    strcpy(buf, buf[2] == ' ' ? "��  �D: " : "���D: ");
    strcat(buf, hdr->title);
    strcat(buf, "\n");
  }
  fputs(buf, fpw);

  while (fgets(buf, sizeof(buf), fpr))	/* �[�J��L */
    fputs(buf, fpw);

  fclose(fpr);
  fclose(fpw);
  f_rm(tmpfile);
}


static int
post_title(xo)
  XO *xo;
{
  FILE *fp;
  char tmpfile[64];
  char tmptitle[TTLEN + 1];
  HDR *fhdr, mhdr;
  int pos, cur;

  if (!cuser.userlevel)	/* itoc.000213: �קK guest �b sysop �O����D */
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  fhdr = (HDR *) xo_pool + cur;
  memcpy(&mhdr, fhdr, sizeof(HDR));

  if ((strcmp(cuser.userid, mhdr.owner) && (!(bbstate & STAT_BM))) && !HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;

  strcpy(tmptitle, mhdr.title);

  vget(b_lines, 0, "���D�G", mhdr.title, TTLEN + 1, GCARRY);

  /* smiler.080913: �����ҧ���D�O�_�ŦX�ݪ����W�w */
  sprintf(tmpfile, "tmp/%s_%s_title", cuser.userid, mhdr.xname);
  if (fp = fopen(tmpfile, "w"))
  {
    fprintf(fp, "%s", mhdr.title);
    fclose(fp);
  }

#ifdef DO_POST_FILTER
  if (post_filter(tmpfile, tmptitle))	/* smiler.080830: �w��峹���D���e�������L�����B */
  {
    unlink(tmpfile);
    return XO_HEAD;
  }
  else
    unlink(tmpfile);
#endif

  if ((mhdr.xmode & POST_BOTTOM) && (bbstate & STAT_BM))
  {
    char buf[256], ch;

    vget(b_lines, 0, "���ܤ�r�G", mhdr.mark, 5, GCARRY);
    sprintf(buf, "�������C�� 0) \033[1;30m�i\033[m 1) \033[1;31m�i\033[m 2) \033[1;32m�i\033[m "
      "3) \033[1;33m�i\033[m ""4) \033[1;34m�i\033[m 5) \033[1;35m�i\033[m "
      "6) \033[1;36m�i\033[m 7) \033[1;37m�i\033[m [%c] ", mhdr.color);
    ch = vans(buf);

    if (ch >= '0' && ch <= '7')
      mhdr.color = ch;
  }

  if (HAS_PERM(PERM_ALLBOARD))	/* itoc.000213: ��@�̥u�����D */
  {
    vget(b_lines, 0, "�@�̡G", mhdr.owner, 73 /* sizeof(mhdr.owner) */, GCARRY);
    /* Thor.980727: sizeof(mhdr.owner) = 80 �|�W�L�@�� */
    vget(b_lines, 0, "�ʺ١G", mhdr.nick, sizeof(mhdr.nick), GCARRY);
    vget(b_lines, 0, "����G", mhdr.date, sizeof(mhdr.date), GCARRY);
  }

  if (memcmp(fhdr, &mhdr, sizeof(HDR)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(fhdr, &mhdr, sizeof(HDR));
    currchrono = fhdr->chrono;
    rec_put(xo->dir, fhdr, sizeof(HDR), xo->key == XZ_XPOST ? fhdr->xid : pos, cmpchrono);

    move(3 + cur, 0);
    post_item(++pos, fhdr);

    /* itoc.010709: �ק�峹���D���K�ק鷺�媺���D */
    header_replace(xo, fhdr);
  }
  return XO_FOOT;
}


/* ----------------------------------------------------- */
/* �B�~�\��Gwrite / score				 */
/* ----------------------------------------------------- */


int
post_write(xo)			/* itoc.010328: ��u�W�@�̤��y */
  XO *xo;
{
  if (HAS_PERM(PERM_PAGE))
  {
    HDR *hdr;
    UTMP *up;

    hdr = (HDR *) xo_pool + (xo->pos - xo->top);

    if (!(hdr->xmode & POST_INCOME) && (up = utmp_seek(hdr)))
      do_write(up);
  }
  return XO_NONE;
}


#ifdef HAVE_SCORE

static int curraddscore;


static int
cmpparent(hdr)
  HDR *hdr;
{
  /* �i�H�����ˬd POST_BOTTOM�A�]���@��峹�� parent_chrono = 0 */
  return /* (hdr->xmode & POST_BOTTOM) && */
    (hdr->parent_chrono == currchrono);
}


static void
change_stamp(folder, hdr)
  char *folder;
  HDR *hdr;
{
  HDR buf;

  /* ���F�T�w�s�y�X�Ӫ� stamp �]�O unique (���M�J���� chrono ����)�A
     �N���ͤ@�ӷs���ɮסA���ɮ��H�K link �Y�i�C
     �o�Ӧh���ͥX�Ӫ��U���|�b expire �Q sync �� (�]�����b .DIR ��) */
  hdr_stamp(folder, HDR_LINK | 'A', &buf, "etc/stamp");
  hdr->stamp = buf.chrono;
}


static void
addscore(hdd, ram)
  HDR *hdd, *ram;
{
  hdd->xmode |= POST_SCORE;
  hdd->stamp = ram->stamp;
  if (curraddscore > 0)
  {
    if (hdd->score < 127)
      hdd->score++;
  }
  else if (curraddscore < 0)
  {
    if (hdd->score > -128)
      hdd->score--;
  }
}


int
post_t_score(xo, log, hdr)	/* ����峹�O�� */
  XO *xo;
  char *log;
  HDR *hdr;
{
  int pos, cur, maxlen;
  char *dir, *userid, fpath[64], reason[80];
  int  ip_len;
  FILE *fp;

  pos = xo->pos;
  cur = pos - xo->top;

  if (hdr->xmode & POST_NOSCORE)
    return XO_NONE;

  ip_len = (currbattr & BRD_POST_IP) ? 16 : 5;
  maxlen = 63 - strlen(cuser.userid) - 2 - ip_len;

  userid = cuser.userid;
  str_ncpy(reason, log, maxlen);

  dir = xo->dir;
  hdr_fpath(fpath, dir, hdr);

  if (fp = fopen(fpath, "a"))
  {
    time_t now;
    struct tm *ptime;

    time(&now);
    ptime = localtime(&now);

    fprintf(fp, "\033[1;30m== %s�G%-*s\033[;30m\033*|\033[1m%02d/%02d %02d:%02d\033[m%s%s\n",
      userid, maxlen - 1, reason,
      ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min,
      (currbattr & BRD_POST_IP) ? " " : "\033[30m\033*/",
      (currbattr & BRD_POST_IP) ? fromip : fromipcode);
    fclose(fp);
  }

    /* ���D�ݸ�: �Y�� tag ���, �h���i�� hdr ���b pos �W, �C������ rec_ref �q�Y��_���l�Ĳv*/
    curraddscore = 0;

    currchrono = hdr->chrono;
    rec_ref(dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono, addscore);
    if (hdr->xmode & POST_BOTTOM)	/* �Y�O�����m���峹�A�h�䥿���ӳs�ʤ��� */
    {
      currchrono = hdr->parent_chrono;
      rec_ref(dir, hdr, sizeof(HDR), 0, cmpchrono, addscore);
    }
    else				/* �Y�O�����@��峹�A�h���å��ӳs�ʤ��� */
    {
      rec_ref(dir, hdr, sizeof(HDR), 0, cmpparent, addscore);
    }

    return XO_LOAD;
}


#ifdef HAVE_ANONYMOUS
static void
log_anonyscore(fname, say)
  char *fname, *say;
{
  char buf[512];

  /* Thor.990113: �[�W fromhost ����Ժ� */
  sprintf(buf, "%s %-13s(%s)\n%-13s %s �z��:%s\n",
	Now(), cuser.userid, fromhost, currboard, fname, say);
  f_cat(FN_RUN_ANONYMOUS, buf);
}
#endif


#define	MAX_REASON_RECORD	20


static int
post_append_score(xo, choose)
  XO *xo;
  int choose;	/* 0: �|�����  >0: �w��ܪ��ﶵ */
{
  HDR *hdr;
  int pos, cur, ans, ans2, vtlen, maxlen;
  char *dir, *userid, *verb, fpath[64], reason[80];
  //char *prompt[3] = {"�����u�n�G", "ť�A����G", "�d�@�y�ܡG"};
  char prompt[64];
  int  num_reason_record = 0;
  int  ip_len;
  char ip_str[48];
  FILE *fp;
#ifdef HAVE_ANONYMOUS
  char uid[IDLEN + 1];
#endif
  time_t start;
  static time_t last = 0;

  if ((currbattr & BRD_NOSCORE) || !cuser.userlevel || !(bbstate & STAT_POST) )	/* �������P�o��峹 */
  {
    if (currbattr & BRD_NOSCORE)
      vmsg("�ݪO�w�Q�]�w�T�����");
    else if (!(bbstate & STAT_POST))
      post_no_right();

    return XO_HEAD;
  }

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;

  if (hdr->xmode & POST_NOSCORE)
  {
    vmsg("�o�g�峹�Q�]�w���������@�I");
    return XO_FOOT;
  }

  if (!strcmp(currboard, "newbm"))
  {
    void (*p)();
    switch (vans("(1)�s���O�D��ĳ (2)���� [Q] "))
    {
      case '1':
	p = DL_get("bin/bmtrans.so:bmt_sign");
	(*p)(xo, hdr);
	return XO_LOAD;
      case '2':
	break;
      default:
	return XO_FOOT;
    }
  }

#ifdef HAVE_REFUSEMARK
  if (!chkrestrict(hdr))
    return XO_NONE;
#endif

  time(&start);
  if ((start - last) < 5)
  {
    srand(start);
    vtlen = rand() % 10000;
    sprintf(fpath, "�� ���W������ɶ��L�u�A�п�J�H���Ʀr %4d: ",vtlen);
    vget(b_lines, 0, fpath, reason, 5, DOECHO);
    if (atoi(reason) != vtlen)
    {
      vmsg("��J���~");
      return XO_FOOT;
    }
  }

  if (!(ans = choose))
  {
    sprintf(reason, "�� 1)\033[1;31m���塵\033[m 2)\033[1;32m�N�塾\033[m 3)\033[1;37m����w [%d] \033[m",
      (cuser.ufo & UFO_ADDSCORE) ? 1 : 3);
    ans = vans(reason) - '0';
  }
  if (ans < 1 || ans > 3)
    ans = (cuser.ufo & UFO_ADDSCORE) ? 1 : 3;

  switch (ans)
  {
  case 1:
    verb = "1m��";
    vtlen = 2;
    break;

  case 2:
    verb = "2m��";
    vtlen = 2;
    break;

  case 3:
  default:
    verb = "7m�w";
    vtlen = 2;
    break;
    /* songsongboy.070124:lexel version*/
    /* if (!vget(b_lines, 0, "�п�J�ʵ��G", fpath, 5, DOECHO))
	return XO_FOOT;
      vtlen = strlen(fpath);
      sprintf(verb = vtbuf, "%cm%s", ans - 2, fpath);
      break;*/
  }

  ip_len = (currbattr & BRD_POST_IP) ? 16 : 5;

  if (currbattr & BRD_POST_IP)
    sprintf(ip_str, " %s", fromip);
  else
    sprintf(ip_str, "\033[30m\033*/%s", fromipcode);

#ifdef HAVE_ANONYMOUS
  if (currbattr & BRD_ANONYMOUS)
    maxlen = 63 - IDLEN - vtlen - ip_len;
  else
#endif
    maxlen = 63 - strlen(cuser.userid) - vtlen - ip_len;

#ifdef HAVE_ANONYMOUS
  if (currbattr & BRD_ANONYMOUS)
  {
    userid = uid;
    if (!vget(b_lines, 0, "�п�J�z�Q�Ϊ�ID�A�]�i������[Enter]�A�άO��[r]�ίu�W�G", userid, IDLEN, DOECHO))
      userid = STR_ANONYMOUS;
    else if (userid[0] == 'r' && userid[1] == '\0')
      userid = cuser.userid;
    else
      strcat(userid, ".");		/* �۩w���ܡA�̫�[ '.' */
    maxlen = 63 - strlen(userid) - vtlen - ip_len;

    if (userid[0] != 'r' || userid[1] != '\0')	/* �ΦW�]�n���� IP */
    {
      if (currbattr & BRD_POST_IP)
	strcpy(ip_str, " �~���i�D�A�O");
      else
	strcpy(ip_str, "\033[30m\033*/----");
    }
  }
  else
#endif
    userid = cuser.userid;

  sprintf(prompt, "%s %s�G",
    (ans == 1) ? "��" : (ans == 2) ? "��" : "�w" , userid);

  move(b_lines, 0);
  clrtoeol();
  prints("\033[1;37m�Ъ`�N�G����N�O���z��IP�A�\\Ū�峹�ɫ�\033[33m ! \033[37m�i������ܱ���IP/IP�X\033[m");
  if (!vget(b_lines - 1, 0, prompt, reason, maxlen, DOECHO))
    return XO_HEAD;

  ans2 = vans("�� Y)�T�w N)���� E)�~�� [Y] ");
  if (ans2 == 'n')
    return XO_HEAD;

  dir = xo->dir;
  hdr_fpath(fpath, dir, hdr);

  if (fp = fopen(fpath, "a"))
  {
    time_t now;
    struct tm *ptime;

    time(&now);
    ptime = localtime(&now);

    fprintf(fp, "\033[1;3%s \033[36m%s\033[m�G\033[33m%-*s\033[30m\033*|\033[1m%02d/%02d %02d:%02d\033[m%s\n",
      verb, userid, maxlen - 1, reason,
      ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ip_str);

    move(b_lines - 4, 0);
    prints("\033[1;30m==============================================================================\033[m\n");
    move(b_lines - 2, 0);
    prints("\033[1;3%s \033[36m%s\033[m�G\033[33m%-*s \033[1;30m%02d/%02d %02d:%02d\033[m\n",
      verb, userid, maxlen - 1, reason,
      ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min);

#ifdef HAVE_ANONYMOUS		/* �ΦW����O�� */
  if (currbattr & BRD_ANONYMOUS && strcmp(userid, cuser.userid))
    log_anonyscore(hdr->xname, reason);
#endif

    num_reason_record ++;

    while ((ans2 == 'e') && (num_reason_record < MAX_REASON_RECORD))
    {
      time(&now);
      ptime = localtime(&now);

      move(b_lines - num_reason_record - 2, 0);
      prints("�z��\033[1;3%s��\033[m���e�G        �ثe��J/�Ѿl��ơG%d/%d\n",
	(ans == 1) ? "1m��" : (ans == 2) ? "2m�N" : "7m��",
	num_reason_record + 1, MAX_REASON_RECORD - num_reason_record - 1);
      move(b_lines, 0);
      prints("������J������ Enter �Y�i�C\n");

      if (!vget(b_lines-1, 0, prompt, reason, maxlen, DOECHO))
	break;

      ans2 = vans("�� Y)�������� N)���歫�s��J E)�~����� [E] ");

      if (ans2 == 'n')
      {
	ans2 = 'e';
	continue;
      }

      fprintf(fp, "%-*s\033[33m%-*s\033[m\n", strlen(userid) + 5, "", maxlen - 1, reason);

#ifdef HAVE_ANONYMOUS		/* �ΦW����O�� */
      if (currbattr & BRD_ANONYMOUS && strcmp(userid, cuser.userid))
	log_anonyscore(hdr->xname, reason);
#endif
      num_reason_record++;

      if (ans2 != 'y')
	ans2 = 'e';

      scroll();
      move(b_lines - 2, 0);
      clrtoeol();
      prints("%-*s\033[33m%-*s\033[m\n", strlen(userid) + 5, "", maxlen - 1, reason);
    }

    fclose(fp);
  }

  curraddscore = (ans == 1) ? 1 : (ans == 2) ? -1 : 0;
  currchrono = hdr->chrono;
  change_stamp(dir, hdr);
  rec_ref(dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono, addscore);
  if (hdr->xmode & POST_BOTTOM)	/* �Y�O�����m���峹�A�h�䥿���ӳs�ʤ��� */
  {
    currchrono = hdr->parent_chrono;
    rec_ref(dir, hdr, sizeof(HDR), 0, cmpchrono, addscore);
  }
  else				/* �Y�O�����@��峹�A�h���å��ӳs�ʤ��� */
  {
    rec_ref(dir, hdr, sizeof(HDR), 0, cmpparent, addscore);
  }
  post_history(xo, hdr);
  btime_update(currbno);
  last = start;

  return XO_INIT;
}


int
post_e_score(xo)
  XO *xo;
{
  return post_append_score(xo, (cuser.ufo & UFO_ADDSCORE) ? 1 : 3);
}


int
post_score(xo)
  XO *xo;
{
  return post_append_score(xo, 0);
}


static int
post_noscore(xo)
  XO *xo;
{
  HDR *hdr;
  int pos, cur;

  if (!cuser.userlevel)	/* guest ������L guest ���峹�]�w */
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;

  if ((bbstate & STAT_BOARD) || !strcmp(hdr->owner, cuser.userid))
  {
    hdr->xmode ^= POST_NOSCORE;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono);
    move(3 + cur, 7);
    outs(post_attr(hdr));
  }
  return XO_NONE;
}
#endif	/* HAVE_SCORE */


static int
post_noforward(xo)
  XO *xo;
{
  HDR *hdr;
  int pos, cur;

  if (!cuser.userlevel)	/* guest ������L guest ���峹�]�w */
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;

  if (HAS_PERM(PERM_ALLBOARD) || !strcmp(hdr->owner, cuser.userid))
  {
    hdr->xmode ^= POST_NOFORWARD;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono);
    move(3 + cur, 7);
    outs(post_attr(hdr));
  }
  return XO_NONE;
}


static int
post_addMF(xo)
  XO *xo;
{
  MF mf;
  char fpath[64];

  if (!cuser.userlevel)
    return XO_NONE;

  if (!in_favor(currboard))
  {
    memset(&mf, 0, sizeof(MF));
    time4(&mf.chrono);
    mf.mftype = MF_BOARD;
    strcpy(mf.xname, currboard);

    mf_fpath(fpath, cuser.userid, FN_MF);
    rec_add(fpath, &mf, sizeof(MF));
    vmsg("�w�N���ݪO�[�J�ڪ��̷R");
  }
  else
  {
    vmsg("���ݪO�w�b�̷R���C�Y�n���Х[�J�A�жi�ڪ��̷R�̷s�W");
  }

  return XO_FOOT;
}


static int
post_viewpal()
{
  XO *xt;
  char fpath[64];
  struct stat st;

  if (!cuser.userlevel)
    return 0;

  if ((currbattr & BRD_HIDEPAL) && !(bbstate & STAT_BM))
    return 0;

  brd_fpath(fpath, currboard, FN_PAL);
  if (!stat(fpath, &st) && S_ISREG(st.st_mode) && !st.st_size)	/* ���ǦW�檺 size �� 0 */
    unlink(fpath);

  if (dashf(fpath))
  {
    xz[XZ_FAKE_PAL - XO_ZONE].xo = xt = xo_new(fpath);
    xt->key = PALTYPE_BPAL;
    xover(XZ_FAKE_PAL);
    free(xt);
    return 1;
  }

  vmsg("���O�|���]�w�O�ͦW��");
  return 0;
}


#ifdef DO_POST_FILTER
static int
post_show_dog(perm, prompt)
  BPERM *perm;
  char *prompt;
{
  if (!perm->exist)
    return vmsg("�ثe�L���]�w�I");

  clear();
  move(1, 0);
  prints("\033[1;33m %s \033[m\n\n", prompt);

  prints("%s�~�֭��� >= [%2d��]\033[m\n", IS_BIGGER_AGE(perm->age) ? COLOR_ACP : COLOR_NOT_ACP , perm->age);

  prints("%s�ʧO���� : [%s] \033[m\n",
    (perm->sex == 0) ? COLOR_ACP : (perm->sex == cuser.sex + 1) ? COLOR_ACP : COLOR_NOT_ACP,
    (perm->sex == 0) ? "����" : (perm->sex == 1) ? "����" : (perm->sex == 2) ? "�k��" : "�k��");

  prints("%s�W�u���� >= [%d��] \033[m\n", (cuser.numlogins >= perm->numlogins) ? COLOR_ACP : COLOR_NOT_ACP, perm->numlogins);

  prints("%s�峹�g�� >= [%d�g] \033[m\n", (cuser.numposts >= perm->numposts) ? COLOR_ACP : COLOR_NOT_ACP, perm->numposts);

  prints("%s�u��g�� >= [%d�g] \033[m\n", (cuser.good_article >= perm->good_article) ? COLOR_ACP : COLOR_NOT_ACP, perm->good_article);

  prints("%s�H��g�� <  [%d�g] (0�G����) \033[m\n",
    (perm->poor_article == 0) ? COLOR_ACP :(cuser.poor_article < perm->poor_article) ? COLOR_ACP : COLOR_NOT_ACP, perm->poor_article);

  prints("%s�H�W���� <  [%d��] (0�G����) \033[m\n",
    (perm->violation == 0) ? COLOR_ACP : (cuser.violation < perm->violation) ? COLOR_ACP : COLOR_NOT_ACP, perm->violation);

  prints("%s�ȹ�     >= [%d�T] \033[m\n", (cuser.money >= perm->money) ? COLOR_ACP : COLOR_NOT_ACP, perm->money);

  prints("%s����     >= [%d�T] \033[m\n", (cuser.gold >= perm->gold) ? COLOR_ACP : COLOR_NOT_ACP, perm->gold);

  prints("%s�o�H���� >= [%d��] \033[m\n", (cuser.numemails >= perm->numemails) ? COLOR_ACP : COLOR_NOT_ACP, perm->numemails);

  prints("%s���U�ɶ� >= [%3d��] \033[m\n", IS_BIGGER_1STLG(perm->regmonth) ? COLOR_ACP : COLOR_NOT_ACP, perm->regmonth);

  return vmsg(NULL);
}
#endif


#ifdef HAVE_RSS
static int post_rss();
#endif


static int
post_ishowbm(xo)
  XO *xo;
{
  BRD  *brd;
  brd = bshm->bcache + currbno;
  int isbm;
  int ch, reload = 0;
  char *mark;

  isbm = (bbstate & STAT_BOARD);
  if (isbm)
    mark = "\033[1;33m";
  else
    mark = "\033[0;30m";

  while (1)
  {
    currbattr = brd->battr;	/* post_manage()����|����s, ���s���J�T�O�O�̷s�� */
    clear();
    move(0, 0);
    prints("\033[1;30m============================== "
      "\033[;37;44m[�ݪO�ݩʳ]�w]\033[;1;30m ===============================\033[m\n"
      "\033[32m���� \033[1;30m�ק����O  �ݩ�                         "
      "\033[;32m���� \033[1;30m�ק����O  �ݩ�\033[m\n");
    prints("   - �^��O�W: %-13s                   - �ݪO����: %s\n",
      brd->brdname, brd->class);
    prints(" %s%s - ����O�W: %s\n", mark, "b\033[m", brd->title);
    prints(" %s%s - �O�D�W��: %s\n", mark,
      (currbattr & BRD_PUBLIC) ? " \033[m" : "m\033[m", brd->BM);
    prints(" %s%s - �ݪO�ݩ�: %s", mark,
      (currbattr & BRD_PUBLIC) ? " \033[m" : "i\033[m",
      (brd->readlevel == PERM_SYSOP) ? "���K" :
      (brd->readlevel == PERM_BOARD) ? "�n��" : "���}");
    prints("                          %s%s - �O�ͦW��: %s\n", mark,
      (currbattr & BRD_PUBLIC) ? " \033[m" : "v\033[m",
      (currbattr & BRD_HIDEPAL) ? "�O�D���êO�ͦW��" : "�O�ͥi�ݪO�ͦW��");

    prints(" %s%s - ��孭��: %s", mark,
      (currbattr & BRD_PUBLIC) ? " \033[m" : "x\033[m",
      (currbattr & BRD_NOL) ? "�O�D�w�]�w�O�ͤ��o���" : "�O�D�����O�����]�w  ");
    prints("        %s%s - �i�_����: %s\n", mark, "e\033[m",
      (currbattr & BRD_NOSCORE) ? "�_" : "�i");

    prints(" %s%s - ����]�w: %s���", mark, "f\033[m",
      (currbattr & BRD_NOFORWARD) ? "�T��" : "���\\");
    prints("                      %s%s - ����峹%s��ܰO��\n", mark, "a\033[m",
      (currbattr & BRD_SHOWTURN) ? "" : "��");

    char buf[64];
    sprintf(buf, "%s%s%s",
      (currbattr & BRD_PUBLIC) ? "�����ݪO" : "�D�����O",
      (currbattr & BRD_IAS) ? " | �����]�ݪO" : "",
      (currbattr & BRD_ATOM) ? " | ATOM�ݪO" : "");
    prints(" %s - �ݪO����: %-30s",
      (currbattr & BRD_PUBLIC) ? "\033[1;33mu\033[m" : " ", buf);

    prints("%s%s - �ݪO�������: %s\n", mark, "!\033[m",
      (currbattr & BRD_POST_IP) ? "IP" : "IP �N�X");

    prints("   - �ΦW�ݪO: %s�ΦW%s",
      (currbattr & BRD_ANONYMOUS) ? "�i" : "���i",
      (currbattr & BRD_ANONYMOUS) ? "  " : "");
    prints("                        - �����|��: %s�|��\n",
      (brd->bvote == 0) ? "�L�벼/��L" : (brd->bvote == -1) ? "����L" : "���벼");

    prints("   - �O�_��H: %s��H", (currbattr & BRD_NOTRAN) ? "��" : "��");
    prints("                          - �i�_�Q Z: %s\n",
      (currbattr & BRD_NOZAP) ? "���i" : "�i");

    prints("   - �峹�g��: %s�O��", (currbattr & BRD_NOCOUNT) ? "��" : "��");
    prints("                          - �������D: %s�έp\n\n",
      (currbattr & BRD_NOSTAT) ? "��" : "�ѻP");

    if (isbm)
    {
      prints("\033[1;30m============================== "
	"\033[;37;44m[�s��e���W��]\033[;1;30m ===============================\033[m\n\n");
      prints(
	" %sw%s - �i�O�e��     %so%s - �O�ͦW��     %s%sk%s - �O�ͯS�O�W��"
#ifdef HAVE_TEMPLATE
	"   %st%s - �峹�d��\n"
#else
	"\n"
#endif
#ifdef POST_PREFIX
	" %sp%s - �峹���O    "
#endif
	" %ss%s - �׫H�C��     %sg%s - BBS �ݪ���"
#ifdef HAVE_RSS
	"     %sr%s - RSS �]�w"
#endif
	"\n",
	mark, "\033[m", mark, "\033[m",
#ifndef POST_PREFIX
	"  ",
#else
	"",
#endif
	mark, "\033[m",
#ifdef HAVE_TEMPLATE
	mark, "\033[m",
#endif
#ifdef POST_PREFIX
	mark, "\033[m",
#endif
	mark, "\033[m", mark, "\033[m",
#ifdef HAVE_RSS
	mark, "\033[m"
#endif
	);
    }

    move(b_lines - 5, 0);
    prints("\033[1;30m============================== "
      "\033[;37;44m[�[�ݬݪO�]�w]\033[;1;30m ===============================\033[m\n\n");
    prints(" %s1%s - �\\Ū����  %s2%s - �o�孭��  %s3%s - �C�X����  ",
      "\033[1;36m", "\033[m", "\033[1;36m", "\033[m", "\033[1;36m", "\033[m");
#ifdef HAVE_RSS
    if (!isbm)
      prints("%s4%s - RSS �]�w  %s", "\033[1;36m", "\033[m",
	!(currbattr & BRD_HIDEPAL) ? "\033[1;36m5\033[m - �O�ͦW��" : "");
    else
#endif
      prints("%s4%s - �O�ͦW�� ", "\033[1;36m", "\033[m");

    move(b_lines, 0);
    prints(COLOR1 " �� %-*s " COLOR2 " [�Ϋ���l���N�����}] \033[m", 51 + d_cols, "�п�J�W��U���ܾާ@����");
    ch = vkey();
    if (!isbm && (ch < '1' || ch > '5') && ch != 'u')
      return reload ? XO_INIT : XO_HEAD;

    if (isbm && (ch == '5'))	/* �O�D�����[�ݤ����i RSS �]�w */
      return reload ? XO_INIT : XO_HEAD;

    if (!(currbattr & BRD_PUBLIC) && ch == 'u')	/* �����O�~�ݻ��� */
      return reload ? XO_INIT : XO_HEAD;

    if (!isbm && ch == 't')	/* �O�D�~��ϥΤ峹�d�� */
      return reload ? XO_INIT : XO_HEAD;

    if ((currbattr & BRD_PUBLIC) &&
      (ch == 'm' || ch == 'i' || ch == 'x' || ch == 'v'))
      return reload ? XO_INIT : XO_HEAD;

    if (isbm && ch == '4')	/* �O�D�� 4 �N�O�ݪO�ͦW�� */
       ch = '5';

    move(b_lines, 0);
    clrtoeol();
    switch (ch |= 0x20)
    {
#ifdef DO_POST_FILTER
    case '1':
      post_show_dog(bshm->rperm + currbno, "�i�J�ݪO���󭭨�");
      break;
    case '2':
      post_show_dog(bshm->wperm + currbno, "�o�������󭭨�");
      break;
    case '3':
      post_show_dog(bshm->lperm + currbno, "�ݪO�C����ܥ��O");
      break;
#endif
#ifdef HAVE_RSS
    case '4':
    case 'r':
      reload = post_rss(xo);
      break;
#endif
    case '5':
      reload = post_viewpal() ? 1 : reload;
      break;
    case 'u':
      move(b_lines - 6, 0);
      clrtobot();
      prints("\033[1;30m%s\033[m\n   �����O�O�D,\n"
	"       ���o���N���O�D�W��,�ݪO���}/����/�n�ͳ]�w\n"
	"       �Y�ݧ������]�w,�Ь������毸���ȳ�\n"
	"   �����O�ϥΪ�,\n"
	"       ���o���", MSG_SEPERATOR);
      vmsg(NULL);
      break;

    /* following are BM functions. */
    case 'b':
      DL_func("bin/manage.so:post_brdtitle");
      break;
    case 'm':
      DL_func("bin/manage.so:post_changeBM");
      break;
    case 'i':
      DL_func("bin/manage.so:post_brdlevel");
      break;
    case 'e':
      DL_func("bin/manage.so:post_battr_noscore");
      break;
    case 'x':
      DL_func("bin/manage.so:post_rlock");
      break;
    case 'v':
      reload = DL_func("bin/manage.so:post_vpal") ? 1 : reload;
      break;
    case 'f':
      DL_func("bin/manage.so:post_noforward");
      break;
    case 'a':
      DL_func("bin/manage.so:post_showturn");
      break;
    case '!':
      DL_func("bin/manage.so:post_brd_ip_char");
      break;
    case 'w':
      DL_func("bin/manage.so:post_memo_edit");
      break;
    case 'o':
      reload = DL_func("bin/manage.so:XoBM") ? 1 : reload;
      break;
    case 'k':
      reload = XoBM_add_pal() ? 1 : reload;
      break;
#ifdef HAVE_TEMPLATE
    case 't':
      DL_func("bin/manage.so:post_template_edit");
      break;
#endif
#ifdef POST_PREFIX
    case 'p':
      DL_func("bin/manage.so:post_brd_prefix");
      break;
#endif
    case 's':
      DL_func("bin/manage.so:post_spam_edit");
      break;
    case 'g':
      DL_func("bin/manage.so:post_guard_dog");
      break;
    default:
      return reload ? XO_INIT : XO_HEAD;
    }
  }

  return XO_NONE;	/* �H���U�@ */
}


static int
post_jxname(xo)	/* jump to xname */
  XO *xo;
{
  HDR *head;
  char xname[32], *px, *pb, *ptr;
  char folder[64], *fimage;
  int i, fsize, max;
  int bno = -1;
  static int j_status = 0;

  if (j_status)
    return XO_NONE;

  if (!vget(b_lines, 0, "�ɮצW�١G", px = xname, 12 + BNLEN + 1, DOECHO))
    return XO_FOOT;

  if (xname[0] == '#')
    px++;

  if (pb = strchr(px + 8, '@'))
  {
    *pb++ = '\0';
    while (*pb == ' ')
      pb++;
    if (!*pb)	/* ����u��J @ */
      pb = NULL;
    else if (ptr = strchr(pb, ' '))
      *ptr = '\0';
  }

  if (pb && strcmp(pb, currboard))
  {
    if (((bno = brd_bno(pb)) >= 0) && (brd_bits[bno] & BRD_R_BIT))
      brd_fpath(folder, pb, FN_DIR);
    else	/* �S���v���i�J �� ���~���ݪO�W�� */
      return XO_FOOT;
  }
  else	/* ������{�b�ҳB���ݪO */
    brd_fpath(folder, currboard, FN_DIR);

  if (ptr = strchr(px, ' '))
    *ptr = '\0';

  fimage = f_map(folder, &fsize);
  max = fsize / sizeof(HDR);
  if (fimage == (char *) -1)
  {
    vmsg("�ثe�L�k�}�ү�����");
    return XO_BODY;
  }

  head = (HDR *) fimage;
  for (i = 0; i < max; i++, head++)
  {
    if (!strcmp(head->xname, px))
    {
      munmap(fimage, fsize);
      if (bno >= 0)
      {
	int tmpbno = currbno;
	int tmppos = xo->pos;
	XoPost(bno);
	xz[XZ_POST - XO_ZONE].xo->pos = i;
	j_status++;
	xover(XZ_POST);
	j_status--;
	XoPost(tmpbno);
	xo = xz[XZ_POST - XO_ZONE].xo;
	xo->pos = tmppos;
	return XO_INIT;
      }
      else
      {
	xo->pos = i;
	return XO_LOAD;
      }
    }
  }
  munmap(fimage, fsize);

  vmsg("�L���ɮ�");
  return XO_FOOT;
}


static int
post_info(xo)
  XO *xo;
{
  HDR *hdr;
  ACCT acct;
  int value;
  static int upid = -1;

#if 1
  if (cuser.ufo & BFLAG(6))
    cuser.ufo &= ~(BFLAG(6));
#endif

  hdr = (HDR *) xo_pool + (xo->pos - xo->top);
  value = atoi(hdr->value);

  move(0, 0);
  clrtoeol();
  prints("�ɮצW�١G#%s @ %-*s", hdr->xname, BNLEN + 2, currboard);
  prints("�@�̡G%-19.18s\n", hdr->owner);
#ifdef HAVE_REFUSEMARK
  if (!chkrestrict(hdr))
    prints("          %s\n", "<< �峹�O�K >>");
  else
#endif
  {
    prints("�峹���D�G%s\n", hdr->title);

    if (hdr->xmode & POST_INCOME)
      prints("���g�����~�H�A�L�峹����");
    else if (hdr->xmode & POST_BOTTOM)
      prints("���g���m����A�L�峹����");
    else if (acct_load(&acct, hdr->owner) >= 0)
      prints("�峹���ȡG%d ��", value);
    else if (value)	/* �䤣��ϥΪ̦��S���峹���Ȫ��A�N��ܬ��ΦW�媺 userno */
    {
      if (upid <= 0)
      {
	UTMP *ui;
	if (ui = (UTMP *) utmp_find(cuser.userno))
	  upid = ui->pid;
      }
      if (HAS_PERM(PERM_ALLBOARD))
	prints("�ΦW�޲z�s���G%d", (upid > 0) ? value + upid : 0);
      else
	prints("���g���ΦW�峹�A�L�峹����");
    }
    else
      prints("���g���®榡�峹");

    if (hdr->xmode & POST_GOOD)
      prints(" | \033[1;33m�u��аO\033[m");
    else if (hdr->xmode & POST_MARKED)
      prints(" | \033[1;36mmark�аO\033[m");
    if ((bbstate & STAT_BOARD) && (hdr->xmode & POST_GEM))
      prints(" | \033[1;35m�w����\033[m");
    if (hdr->xmode & POST_DELETE)
      prints(" | \033[1;32m�ݬ�\033[m");
    if (hdr->parent_chrono == 1)
      prints(" | \033\[30;46m�m��\033[m");
    if (hdr->xmode & POST_NOFORWARD)
      prints(" | \033[30;41m�T�����\033[m");
    if (hdr->xmode & POST_NOSCORE)
      prints(" | \033[30;44m�T�����\033[m");
  }

  if (HAS_PERM(PERM_ALLBOARD))
  {
    char fpath[64];
    struct stat st;

    hdr_fpath(fpath, xo->dir, hdr);

    if (!stat(fpath, &st))
    {
      time4_t temp = st.st_mtime;
      prints("\n�ɶ��W�O�G%s  �ɮפj�p�G%d\n", Btime(&temp), st.st_size);
    }

    prints("�o�����G%s", hdr->date);
    prints("%-17s�ʺ�    �G%s\n", "", hdr->nick);
    prints("�ɮצ�m�G%s\n", fpath);

    prints("�n�ͭ��w�G%s", (hdr->xmode & POST_FRIEND) ? "�O" : "�_");

    prints("%-23s�@�̭��w�G%s\n", "", (hdr->xmode & POST_RESTRICT) ? "�O" : "�_");

    prints("�i�௸�~�G%s", (hdr->xmode & POST_OUTGO) ? "�O" : "�_");
    prints("%-23s�w�Q�����G%s",  "", (hdr->xmode & POST_SCORE) ? "�O" : "�_");
  }

  clrtoeol();
  move(xo->pos - xo->top + 3, 0);
  vkey();
  return post_head(xo);
}


#ifdef HAVE_RSS
static int
post_rss()
{
  more("gem/@/@rss.info", NULL);
  DL_func("bin/rss.so:rss_main");
  return 1;
}
#endif


static int
post_whereami(xo)
  XO *xo;
{
  FILE *fp;
  static char *index = "gem/@/@Class.index";
  char cmd[32], buf[256], *ptr;
  int end = 0, line = 6;

  if (!dashf(index))
  {
    vmsg("�������ީ|���إߡA�Ц� sysop �O���������D !!");
    return XO_NONE;
  }

  if ((currboard[0] == 'P') && (currboard[1] == '_'))
    vmsg("�ӤH�ݪO��� (C)lass -> People ���A�H�U�N�j�M��l�i�൲�G !!");

  sprintf(cmd , "*%s ", currboard);

  move(1, 0);
  clrtobot();
  outs("�ڦb���̡H\n\n");
  if (fp = fopen(index, "r"))
  {
    while (fgets(buf, sizeof(buf), fp))
    {
      if (!strncmp(buf, cmd, strlen(cmd)))
      {
	if (line >= 20 && end == 0)
	{
	  line = 6;
	  move(b_lines - 1, 0);
	  outs("\033[1;44m �|����ܧ����A�����N����ܤU��....\033[m");
	  vmsg(NULL);
	  move(3, 0);
	  clrtobot();
	}

	if (ptr = strstr(buf, "C:"))
	{
	  prints("%-*s", end * 2, "");
	  if (!strncmp(ptr + 2, currboard, strlen(currboard)) && *(ptr + 2 + strlen(currboard)) == ' ')
	  {
	    end = -1;
	    prints("%s\n", ptr + 2);
	  }
	  else
	    prints(ptr + 2);
	  end++;
	  line++;
	}
      }
    }
    fclose(fp);
    vmsg(NULL);
  }

  return XO_HEAD;
}


static int
post_help(xo)
  XO *xo;
{
  xo_help("post");
  /* return post_head(xo); */
  return XO_HEAD;		/* itoc.001029: �P xpost_help �@�� */
}


#ifndef NEW_KeyFunc
KeyFunc post_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, post_item_bar,	/* verit.20030129 : �аȥ��N����b�Ĥ@�� */
#endif
  XO_INIT, post_init,
  XO_LOAD, post_load,
  XO_HEAD, post_head,
  XO_BODY, post_body,

  'W', post_whereami,
  'r', post_browse,
  's', post_switch,
  KEY_TAB, post_gem,
  'z', post_gem,
  'y', post_reply,
  'd', post_delete,
  'v', post_visit,
  'x', post_cross,		/* �b post/mbox �����O�p�g x ��ݪO�A�j�g X ��ϥΪ� */
  'X', post_forward,
  't', post_tag,
  'E', post_edit,
  'T', post_title,
  'm', post_mark,
  'M', post_mark_good,
  '_', post_bottom,
  'D', post_rangedel,
  'o', post_noforward,
#ifdef HAVE_SCORE
  '%', post_score,
  'e', post_e_score,
  Ctrl('E'), post_noscore,
#endif

  'w', post_write,

  'b', post_memo,
  'c', post_copy,
  'g', gem_gather,

#ifdef HAVE_REFUSEMARK
  'l', post_refuse,
#endif

#ifdef HAVE_LABELMARK
  'n', post_label,
  Ctrl('N'), post_delabel,
#endif

  '/', XOXpost_search_all,  /* smiler.070201: �j�M�\���X */
  '!', XoRXsearch,
  '~', XoXselect,		/* itoc.001220: �j�M�@��/���D */
  'a', XoXauthor,		/* itoc.001220: �j�M�@�� */
  'S', XoXsearch,		/* itoc.001220: �j�M�ۦP���D�峹 */
  'G', XoXmark,			/* itoc.010325: �j�M mark �峹 */

#if 0
  '~' | XO_DL, (int *)  "bin/dictd.so:main_dictd",
  '/', XoXtitle,		/* itoc.001220: �j�M���D */
  'f', XoXfull,			/* itoc.030608: ����j�M */
  'K', XoXlocal,		/* itoc.010822: �j�M���a�峹 */
#endif

  Ctrl('P'), post_add,
  Ctrl('D'), post_prune,
  Ctrl('Q'), xo_uquery,
  Ctrl('O'), xo_usetup,
  'f', post_info,
  '#', post_jxname,

  Ctrl('F'), post_addMF,
  'i', post_ishowbm,
  'B', post_ishowbm,
  'R' | XO_DL, (void *) "bin/vote.so:vote_result",
  'V' | XO_DL, (void *) "bin/vote.so:XoVote",
  Ctrl('G') | XO_DL, (void *) "bin/xyz.so:post_sibala",

#ifdef HAVE_TERMINATOR
  Ctrl('X') | XO_DL, (void *) "bin/manage.so:post_terminator",
#endif

#ifdef HAVE_XYNEWS
  'u', XoNews,			/* itoc.010822: �s�D�\Ū�Ҧ� */
#endif

  'h', post_help
};
#else
NewKeyFunc post_cb[] =
{
#ifdef HAVE_LIGHTBAR
  /* verit.20030129 : �аȥ��N����b�Ĥ@�� */
  XO_ITEM, post_item_bar,       XO_ITEM,        'n',    "XO_ITEM",      "",
#endif
  XO_INIT, post_init,           XO_INIT,        'n',    "XO_INIT",      "",
  XO_LOAD, post_load,           XO_LOAD,        'n',    "XO_LOAD",      "",
  XO_HEAD, post_head,           XO_HEAD,        'n',    "XO_HEAD",      "",
  XO_BODY, post_body,           XO_BODY,        'n',    "XO_BODY",      "",

  'W', post_whereami,   'W',    'p',    "�ڦb����",     	"",
  'r', post_browse,     'r',    'z',    "�s��",         	"",
  's', post_switch,     's',    'p',    "�����ݪO",     	"",
  KEY_TAB, post_gem,    KEY_TAB,        'p',    "��ذ�",       "",
  'z', post_gem,        'z',    'p',    "��ذ�",       	"",
  'y', post_reply,      'y',    'p',    "�^�Ф峹",     	"",
  'd', post_delete,     'd',    'p',    "�R��",         	"",
  'v', post_visit,      'v',    'p',    "�]�w�峹\�\\Ū�O��",   "",

  'x', post_cross,      'x',    'p',    "����峹�ܬݪO",       "",
  'X', post_forward,    'X',    'p',    "����峹�ܫH�c",       "",
  't', post_tag,        't',    'p',    "��������",     	"",
  'E', post_edit,       'E',    'p',    "�ק�峹���e", 	"",
  'T', post_title,      'T',    'p',    "�ק�峹���D", 	"",
  'm', post_mark,       'm',    'p',    "�峹�аO",     	"",
  'M', post_mark_good,  'M',    'p',    "�u��",         	"",
  '_', post_bottom,     '_',    'p',    "�峹�m��",     	"",
  'D', post_rangedel,   'D',    'p',    "�Ϭq�R���峹", 	"",
  'o', post_noforward,  'o',    'p',    "�峹�T�����", 	"",
#ifdef HAVE_SCORE
  '%', post_score,      '%',    'p',    "����/����/�N��",       "",
  'e', post_e_score,    'e',    'p',    "����",         	"",
  Ctrl('E'), post_noscore,      Ctrl('E'),      'p',    "�峹�T�����", "",
#endif

  'w', post_write,      'w',    'p',    "�ǰe���y���@��",       "",

  'b', post_memo,       'b',    'p',    "�s���i�O�e��", 	"",
  'c', post_copy,       'c',    'p',    "�ƻs�峹(�ܺ�ذ�)",   "",
  'g', gem_gather,      'g',    'p',    "�峹�����ܺ�ذ�",     "",

#ifdef HAVE_REFUSEMARK
  'l', post_refuse,     'l',    'p',    "�峹�[�K/�ѱK",        "",
#endif

#ifdef HAVE_LABELMARK
  'n', post_label,              'n',            'p',    "�峹�аO�ݬ�",		"",
  Ctrl('N'), post_delabel,      Ctrl('N'),      'p',    "�R���аO�ݬ�峹",	"",
#endif

  /* smiler.070201: �j�M�\���X */
  '/', XOXpost_search_all,      '/',    'p',    "�j�M�\\���X��",     "",

  '!', XoRXsearch,              '!',    'p',    "�ư��j�M",             "",

  /* itoc.001220: �j�M�@��/���D */
  '~', XoXselect,               '~',    'p',    "�j�M�@�̤μ��D",       "",

  /* itoc.001220: �j�M�@�� */
  'a', XoXauthor,               'a',    'p',    "�j�M�@��",             "",

  /* itoc.001220: �j�M�ۦP���D�峹 */
  'S', XoXsearch,               'S',    'p',    "�j�M�P���D",           "",

  /* itoc.010325: �j�M mark �峹 */
  'G', XoXmark,                 'G',    'p',    "�j�M�аO",             "",

#if 0
  '~' | XO_DL, (int *)  "bin/dictd.so:main_dictd",
  '/', XoXtitle,                /* itoc.001220: �j�M���D */
  'f', XoXfull,                 /* itoc.030608: ����j�M */
  'K', XoXlocal,                /* itoc.010822: �j�M���a�峹 */
#endif

  Ctrl('P'), post_add,          Ctrl('P'),      'p',    "�o��峹",     	"",
  Ctrl('D'), post_prune,        Ctrl('D'),      'p',    "�R�����ҿ���峹",	"",
  Ctrl('Q'), xo_uquery,         Ctrl('Q'),      'p',    "�d�ߧ@��",     	"",
  Ctrl('O'), xo_usetup,         Ctrl('O'),      's',    "�]�w�@�̭ӤH���",	"",
  'f', post_info,               'f',            'p',    "��ܤ峹��T", 	"",
  '#', post_jxname,             '#',            'p',    "���ɦW��M�峹",	"",

  Ctrl('F'), post_addMF,        Ctrl('F'),      'p',    "�N�ݪO�[�J�ڪ��̷R",	"",
  'i', post_ishowbm,            'i',            'p',    "��ܬݪO��T�κ޲z",	"",
  'B', post_ishowbm,            'B',            'p',    "��ܬݪO��T�κ޲z",	"",

  'R' | XO_DL, (void *) "bin/vote.so:vote_result",      'R' | XO_DL,            'z',    "��ܧ벼���G",         "",
  'V' | XO_DL, (void *) "bin/vote.so:XoVote",           'V' | XO_DL,            'z',    "����벼�����\\��",   "",
  Ctrl('G') | XO_DL, (void *) "bin/xyz.so:post_sibala", Ctrl('G') | XO_DL,      'z',    "���ܬݪO",           "",

#ifdef HAVE_TERMINATOR
  Ctrl('X') | XO_DL, (void *) "bin/manage.so:post_terminator",  Ctrl('X') | XO_DL,      'z',    "�تḨ����",   "",
#endif

#ifdef HAVE_XYNEWS
  /* itoc.010822: �s�D�\Ū�Ҧ� */
  'u', XoNews,                  'u',            'p',    "�����ܷs�D\�\\Ū�Ҧ�",	"",
#endif

  'h', post_help,               'h',            'z',    "�\\�໡��",	""
};
#endif


#ifndef NEW_KeyFunc
KeyFunc xpost_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, post_item_bar,	/* verit.20030129 : �аȥ��N����b�Ĥ@�� */
#endif
  XO_INIT, xpost_init,
  XO_LOAD, xpost_load,
  XO_HEAD, xpost_head,
  XO_BODY, post_body,		/* Thor.980911: �@�ΧY�i */

  'r', xpost_browse,
  'y', post_reply,
  't', post_tag,
  'x', post_cross,
  'X', post_forward,
  KEY_TAB, post_gem,
  'z', post_gem,
  'o', post_noforward,
  'c', post_copy,
  'g', gem_gather,
  'm', post_mark,
  'M', post_mark_good,
  'd', post_delete,		/* Thor.980911: ��K�O�D */
  'E', post_edit,		/* itoc.010716: ���� XPOST ���i�H�s����D�B�峹�A�[�K */
  'T', post_title,
#ifdef HAVE_SCORE
  '%', post_score,
  'e', post_e_score,
  Ctrl('E'), post_noscore,
#endif

  'w', post_write,

#ifdef HAVE_REFUSEMARK
  'l', post_refuse,
#endif

#ifdef HAVE_LABELMARK
  'n', post_label,
#endif

  '/', XOXpost_search_all,  /* smiler.070201: �j�M�\���X */
  '!', XoRXsearch,
  '~', XoXselect,
  'a', XoXauthor,
  'S', XoXsearch,
  'G', XoXmark,

#if 0
  '/', XoXtitle,
  'f', XoXfull,
  'K', XoXlocal,
#endif

  Ctrl('P'), post_add,
  Ctrl('D'), post_prune,
  Ctrl('Q'), xo_uquery,
  Ctrl('O'), xo_usetup,
  'f', post_info,

  'h', post_help		/* itoc.030511: �@�ΧY�i */
};
#else
NewKeyFunc xpost_cb[] =
{
#ifdef HAVE_LIGHTBAR
  /* verit.20030129 : �аȥ��N����b�Ĥ@�� */
  XO_ITEM, post_item_bar,       XO_ITEM,        'n',    "XO_ITEM",      "",
#endif
  XO_INIT, xpost_init,          XO_INIT,        'n',    "XO_INIT",      "",
  XO_LOAD, xpost_load,          XO_LOAD,        'n',    "XO_LOAD",      "",
  XO_HEAD, xpost_head,          XO_HEAD,        'n',    "XO_HEAD",      "",
  /* Thor.980911: �@�ΧY�i */
  XO_BODY, post_body,           XO_BODY,        'n',    "XO_BODY",      "",

  'r', xpost_browse,    'r',    'z',    "�s��",         	"",
  'y', post_reply,      'y',    'p',    "�^�Ф峹",     	"",
  't', post_tag,        't',    'p',    "��������",     	"",
  'x', post_cross,      'x',    'p',    "����峹�ܬݪO",       "",
  'X', post_forward,    'X',    'p',    "����峹�ܫH�c",       "",
  KEY_TAB, post_gem,    KEY_TAB,        'p',    "��ذ�",       "",
  'z', post_gem,        'z',    'p',    "��ذ�",       	"",
  'o', post_noforward,  'o',    'p',    "�峹�T�����", 	"",
  'c', post_copy,       'c',    'p',    "�ƻs�峹(�ܺ�ذ�)",   "",
  'g', gem_gather,      'g',    'p',    "�峹�����ܺ�ذ�",     "",
  'm', post_mark,       'm',    'p',    "�峹�аO",     	"",
  'M', post_mark_good,  'M',    'p',    "�u��",         	"",

  /* Thor.980911: ��K�O�D */
  'd', post_delete,     'd',    'p',    "�R��",         	"",

  /* /* itoc.010716: ���� XPOST ���i�H�s����D�B���e */
  'E', post_edit,       'E',    'p',    "�ק�峹���e", 	"",
  'T', post_title,      'T',    'p',    "�ק�峹���D", 	"",
#ifdef HAVE_SCORE
  '%', post_score,      '%',    'p',    "����/����/�N��",       "",
  'e', post_e_score,    'e',    'p',    "����",         	"",
  Ctrl('E'), post_noscore,      Ctrl('E'),      'p',    "�峹�T�����", "",
#endif

  'w', post_write,      'w',    'p',    "�ǰe���y���@��",       "",

#ifdef HAVE_REFUSEMARK
  'l', post_refuse,     'l',    'p',    "�峹�[�K/�ѱK",        "",
#endif

#ifdef HAVE_LABELMARK
  'n', post_label,              'n',            'p',    "�峹�аO�ݬ�",	"",
#endif

  /* smiler.070201: �j�M�\���X */
  '/', XOXpost_search_all,      '/',    'p',    "�j�M�\\���X��",     "",
  '!', XoRXsearch,              '!',    'p',    "�ư��j�M",             "",
  '~', XoXselect,               '~',    'p',    "�j�M�@�̤μ��D",       "",
  'a', XoXauthor,               'a',    'p',    "�j�M�@��",             "",
  'S', XoXsearch,               'S',    'p',    "�j�M�P���D",           "",
  'G', XoXmark,                 'G',    'p',    "�j�M�аO",             "",

#if 0
//  '/', XoXtitle,
//  'f', XoXfull,
//  'K', XoXlocal,
#endif

  Ctrl('P'), post_add,          Ctrl('P'),      'p',    "�o��峹",     	"",
  Ctrl('D'), post_prune,        Ctrl('D'),      'p',    "�R�����ҿ���峹",	"",
  Ctrl('Q'), xo_uquery,         Ctrl('Q'),      'p',    "�d�ߧ@��",     	"",
  Ctrl('O'), xo_usetup,         Ctrl('O'),      's',    "�]�w�@�̭ӤH���",	"",
  'f', post_info,               'f',            'p',    "��ܤ峹��T", 	"",

  /* itoc.030511: �@�ΧY�i */
  'h', post_help,               'h',            'z',    "�\\�໡��",     ""
};
#endif


#ifdef HAVE_XYNEWS
#ifndef NEW_KeyFunc
KeyFunc news_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, post_item_bar,      /* verit.20030129 : �аȥ��N����b�Ĥ@�� */
#endif
  XO_INIT, news_init,
  XO_LOAD, news_load,
  XO_HEAD, news_head,
  XO_BODY, post_body,

  'r', XoXsearch,

  'h', post_help		/* itoc.030511: �@�ΧY�i */
};
#else
NewKeyFunc news_cb[] =
{
#ifdef HAVE_LIGHTBAR
  /* verit.20030129 : �аȥ��N����b�Ĥ@�� */
  XO_ITEM, post_item_bar,       XO_ITEM,        'n',    "XO_ITEM",      "",
#endif
  XO_INIT, news_init,           XO_INIT,        'n',    "XO_INIT",      "",
  XO_LOAD, news_load,           XO_LOAD,        'n',    "XO_LOAD",      "",
  XO_HEAD, news_head,           XO_HEAD,        'n',    "XO_HEAD",      "",
  XO_BODY, post_body,           XO_BODY,        'n',    "XO_BODY",      "",

  'r', XoXsearch,               'r',    'z',    "�j�M�P���D",           "",

  /* itoc.030511: �@�ΧY�i */
  'h', post_help,               'h',            'z',    "�\\�໡��",   ""
};
#endif
#endif	/* HAVE_XYNEWS */
