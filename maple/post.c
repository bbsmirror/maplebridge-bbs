/*-------------------------------------------------------*/
/* post.c       ( NTHU CS MapleBBS Ver 2.39 )		 */
/*-------------------------------------------------------*/
/* target : bulletin boards' routines		 	 */
/* create : 95/03/29				 	 */
/* update : 96/04/05				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"

#define DO_POST_FILTER

extern BCACHE *bshm;
extern XZ xz[];


extern int wordsnum;		/* itoc.010408: �p��峹�r�� */
extern int TagNum;
extern char xo_pool[];
extern char brd_bits[];


#ifdef HAVE_ANONYMOUS
extern char anonymousid[];	/* itoc.010717: �۩w�ΦW ID */
#endif

void
str_lower_tmp(dst, src)
  char *dst, *src;
{
  int ch;

  do
  {
    ch = *src++;
    if (ch >= 'A' && ch <= 'Z')
      ch |= 0x20;
    *dst++ = ch;
  } while (ch);
}

int
cmpchrono(hdr)
  HDR *hdr;
{
  return hdr->chrono == currchrono;
}

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
RefusePal_fpath(fpath, board, mode, hdr)
  char *fpath;
  char *board;
  char mode;       /* 'C':Cansee  'R':Refimage */
  HDR *hdr;
{
  sprintf(fpath, "brd/%s/RefusePal_DIR/%s_%s",
    board, mode == 'C' ? "Cansee" : "Refimage", hdr->xname);
}
                                                                                
                                                                                
void
RefusePal_kill(board, hdr)   /* amaki.030322: �ΨӬ�W��p�� */
  char *board;
  HDR *hdr;
{
  char fpath[64];
                                                                                
  RefusePal_fpath(fpath, board, 'C', hdr);
  unlink(fpath);
  RefusePal_fpath(fpath, board, 'R', hdr);
  unlink(fpath);
}

int          /* 1:F��  0:���q�� -1:L�� */
RefusePal_level(board, hdr)       //smiler 1108
  char *board;
  HDR *hdr;
{
  int fsize;
  char fpath[64];
  int *fimage;
  FILE *fp;
                                                                                
  if(! hdr->xmode & POST_RESTRICT)
     return 0;
  else
  {
    RefusePal_fpath(fpath, board, 'C', hdr);
    if(fp=fopen(fpath,"r"))
    {
       fclose(fp);
       if (!strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM))
            return 1;
       if (fimage = f_img(fpath, &fsize))
       {
        fsize = belong_pal(fimage, fsize / sizeof(int), cuser.userno);
        free(fimage);
        return fsize;
       }
    }
    else
     return -1;
  }
}

int          /* 1:�b�i���W��W 0:���b�i���W��W */
RefusePal_belong(board, hdr)
  char *board;
  HDR *hdr;
{
  int fsize;
  char fpath[64];
  int *fimage;
                                                                                
  if (!strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM))
    return 1;
                                                                                
  RefusePal_fpath(fpath, board, 'C', hdr);  //smiler 1109
  if (fimage = f_img(fpath, &fsize))
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
XoBM_add_pal(xo)
  XO *xo;
{
  int ans;
  char fpath[64];
  XO *xt;

  if(! (bbstate & STAT_BM))
	  return XO_NONE;

  ans = vans("����ܪO�ͯS�O�W��1~8�H[Q] ");
  if(ans<57 && ans >48)
  {
     sprintf(fpath, "brd/%s/friend_%c", currboard,ans);
     xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath);
     xt->key = PALTYPE_BPAL;        //smiler 1106
     xover(XZ_PAL);                                                                            
     free(xt);
     return XO_INIT;
  }
  else    
     return XO_FOOT;

}

static int
XoBM_Refuse_pal(xo)
  XO *xo;
{
  char fpath[64];
  HDR *hdr;
  XO *xt;
  int ans,ans2,ans3;
  char fpath_friend[64];
  XO *xr;
  char tmp[64],tmp2[64];
  FILE *fp;
  int pos, cur;              //smiler 1108
  pos = xo->pos;             //smiler 1108
  cur = pos - xo->top;       //smiler 1108
                                                                                
  hdr = (HDR *) xo_pool + (xo->pos - xo->top);
                                                                                
  if (!(hdr->xmode & POST_RESTRICT))
    return XO_NONE;
                                                                                
  if (strcmp(hdr->owner, cuser.userid) && !(bbstate & STAT_BM))
    return XO_NONE;

  sprintf(fpath, "brd/%s/RefusePal_DIR", currboard);
  if (!dashd(fpath))
    mkdir(fpath, 0700);
  RefusePal_fpath(fpath, currboard, 'C', hdr);

  ans=ans2=ans3=0;

  if((bbstate & STAT_BM))
      ans3 = vans("����� �n��:1 �O��:2 ���N�s��W��:3 [Q] ");
  else
	  ans3 = 49;
  
  switch (ans3)
  {
  case 49:
    ans2 = vans("����� �n�͸s�զW��:1~5 ���N�s��W��:6 [Q] ");
	if(ans2==54)
		ans=57;
	else if(ans2> 54 || ans2<49)
		ans=-1;
    break;

  case 50:
    ans = vans("����� �O�ͦW��:0 �O�ͯS�O�W��:1~8 ���N�s��W��:9 [Q] ");
	if(ans>57 || ans<48)
		ans=-1;
    break;

  case 51:
    ans = 57;
    break;

  default:
    return XO_NONE;
  }

  if(ans==-1)
	  return XO_NONE;

  if(ans==0) //�n�ͦW��
  {
	if(ans2 < 54 && ans2 >48) //1~5
    {
		str_lower_tmp(tmp2,cuser.userid);
		sprintf(fpath_friend,"usr/%c/%s/list.%d",tmp2[0],tmp2,ans2-48);
	}
  }
  else      //�O�ͦW��
  {
    if(ans==48)
		sprintf(fpath_friend, "brd/%s/friend",currboard);
    else if(ans<57 && ans >48) //1~8
       sprintf(fpath_friend, "brd/%s/friend_%c", currboard,ans);
  }

    if(ans==57)
	{
         xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath);
         xt->key = PALTYPE_BPAL;        //smiler 1106
         xover(XZ_PAL);
         refusepal_cache(hdr, currboard);
         free(xt);
	}
	else
	{
         if(fp=fopen(fpath_friend,"r"))
            fclose(fp);
         else
		 {
            xz[XZ_PAL - XO_ZONE].xo = xr = xo_new(fpath_friend);
			 if(ans2==48)
				 xr->key = PALTYPE_PAL;
			 else if(ans2 > 48)
                 xr->key = PALTYPE_LIST;        //smiler 1106
			 else
				 xr->key = PALTYPE_BPAL;
            xover(XZ_PAL);
            free(xr);
		 }
         sprintf(tmp,"cp %s %s",fpath_friend,fpath);
         system(tmp);
         xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath);
         xt->key = PALTYPE_BPAL;        //smiler 1106
         xover(XZ_PAL);
         refusepal_cache(hdr, currboard);
         free(xt);
	}

    move(3 + cur, 7);              //smiler 1108
    outc('F');                     //smiler 1108
    return XO_INIT;
}

static int
post_viewpal(xo)
  XO *xo;
{
	XO *xt;
	FILE *fp;
	char fpath_org[64];
    char fpath_new[64];
	char tmp[64];

  if (!cuser.userlevel)
    return XO_NONE;
  
  if((!(bbstate & STAT_BM)) && (currbattr & BRD_SHOWPAL))
    return XO_NONE;

  sprintf(fpath_org,"brd/%s/friend",currboard);
  sprintf(fpath_new,"brd/%s/friend_%s",currboard,cuser.userid);
  sprintf(tmp,"cp %s %s",fpath_org,fpath_new);
  if(fp=fopen(fpath_org,"r"))
  {
	  fclose(fp);
	  system(tmp);
      xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath_new);
      xt->key = PALTYPE_BPAL;
      xover(XZ_PAL);
      free(xt);
	  unlink(fpath_new);
	  return XO_INIT;
  }
  else
  {
	  vmsg("���O�|���]�w�O�ͦW�� !!");
	  return XO_NONE;
  }
}

static int
post_showbm(xo)
  XO *xo;
{
    BRD  *brd;
    FILE *fp;
	char file_path[64];
    brd = bshm->bcache + currbno;
    sprintf(file_path,"brd/%s/BM_%s",currboard,cuser.userid);
	fp=fopen(file_path,"w");    
	fprintf(fp,"�ݪO�^��O�W: %s\n",brd->brdname);
	fprintf(fp,"�ݪO����    : %s\n",brd->class);
    fprintf(fp,"�ݪO����O�W: %s\n",brd->title);
    fprintf(fp,"�ݪO�O�D�W��: %s\n",brd->BM);
	if(brd->bvote ==0)
	  fprintf(fp,"�ݪO�����|��: �L�벼�|��\n");
	else if(brd->bvote == -1)
	  fprintf(fp,"�ݪO�����|��: ����L�|��\n");
	else
	  fprintf(fp,"�ݪO�����|��: ���벼�|��\n");

//#define BRD_NOZAP   0x01    /* ���i zap */
	fprintf(fp,"�ݪO�i�_�Qz : ");
	if(currbattr & BRD_NOZAP)
		fprintf(fp,"���i\n");
	else
		fprintf(fp,"�i\n");
//#define BRD_NOTRAN  0x02    /* ����H */
	fprintf(fp,"�ݪO�i�_��H: ");
	if(currbattr & BRD_NOTRAN)
		fprintf(fp,"����H\n");
	else
		fprintf(fp,"�i��H\n");
//#define BRD_NOCOUNT 0x04    /* ���p�峹�o��g�� */
	fprintf(fp,"�峹�o��g��: ");
	if(currbattr & BRD_NOCOUNT)
		fprintf(fp,"���O��\n");
	else
		fprintf(fp,"�O��\n");
//#define BRD_NOSTAT  0x08    /* ���ǤJ�������D�έp */
	fprintf(fp,"�������D�έp: ");
	if(currbattr & BRD_NOSTAT)
		fprintf(fp,"���O��\n");
	else
		fprintf(fp,"�O��\n");
//#define BRD_NOVOTE  0x10    /* �����G�벼���G�� [record] �O */
	fprintf(fp,"���ᵲ�G���G: ");
	if(currbattr & BRD_NOVOTE)
		fprintf(fp,"�����G����G�� [record] �O\n");
	else
		fprintf(fp,"���G�벼���G�� [record] �O\n");
//#define BRD_ANONYMOUS   0x20    /* �ΦW�ݪO */
	fprintf(fp,"�ΦW�ݪO   ?: ");
	if(currbattr & BRD_ANONYMOUS)
		fprintf(fp,"�O\n");
	else
		fprintf(fp,"�_\n");
//#define BRD_NOSCORE 0x40    /* �������ݪO */
	fprintf(fp,"�ݪO�i�_����: ");
	if(currbattr & BRD_NOSCORE)
		fprintf(fp,"�_\n");
	else
		fprintf(fp,"�i\n");
//#define BRD_NOL     0x100   /* ���i��� */
	fprintf(fp,"�ݪO��孭��: ");
	if(currbattr & BRD_NOL)
		fprintf(fp,"�O�D�w�]�w�O�ͤ��o���\n");
	else
		fprintf(fp,"�O�D�����O�����]�w\n");
//#define BRD_SHOWPAL 0x200   /* ��ܪO�ͦW�� */
	fprintf(fp,"��ܪO�ͦW��: ");
	if(currbattr & BRD_SHOWPAL)
		fprintf(fp,"�O�D���êO�ͦW��\n");
	else
		fprintf(fp,"�O�ͥi�ݪO�ͦW��(ctrl^g)\n");
//#define BRD_PUBLIC  0x80    /* �����O */
	fprintf(fp,"�O�_�������O: ");
	if(currbattr & BRD_PUBLIC)
	{
		fprintf(fp,"�O�����O\n");
		fprintf(fp,"\n===>\n");
		fprintf(fp,"   �����O�O�D,\n");
		fprintf(fp,"     ���o���N���O�D�W��,�ݪO���}/����/�n�ͳ]�w\n");
		fprintf(fp,"     �Y�ݧ������]�w,�Ь������毸���ȳ�\n");
		fprintf(fp,"   �����O�ϥΪ�,\n");
		fprintf(fp,"     ���o���\n");
	}
	else
		fprintf(fp,"�D�����O\n");
	fclose(fp);
	more(file_path, NULL);
	unlink(file_path);
	return XO_HEAD;
}

/* smiler 0914*/
int
post_filter(acl)
  char *acl;			/* file name of access control list */
{
  FILE *fp;
  char filter[256];

  if (!(fp = fopen(acl, "r")))
    return -1;

  while (fgets(filter, sizeof(filter), fp))
  {
     if(strstr(filter,"paperdo@gmail.com") || strstr(filter,"http://paperdo.googlepages.com/index.htm")
		 || strstr(filter,"�פ�N�g") || strstr(filter,"�N�g�פ�"))
		 return 1;
  }

  fclose(fp);
  return 0;
}

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
static 			/* �� enews.c �� */
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
	  return 1;
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
  ACCT acct;
  HDR hdr;

  if (HAS_PERM(PERM_ALLADMIN))
    return 0;

  /* �O�D�b�ۤv�޲z���ݪO���C�J��K�ˬd */
  blist = (bshm->bcache + bno)->BM;
  if (HAS_PERM(PERM_BM) && blist[0] > ' ' && is_bm(blist, cuser.userid))
    return 0;
  
  else if (acl_has(FN_ETC_NOCROSSPOST, "", cuser.userid) > 0)
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

    bbstate &= ~STAT_POST;
    cuser.userlevel &= ~PERM_POST;
    cuser.userlevel |= PERM_DENYPOST;
    if (acct_load(&acct, cuser.userid) >= 0)
    {
      acct.tvalid = time(NULL) + CROSSPOST_DENY_DAY * 86400;
      acct_setperm(&acct, PERM_DENYPOST, PERM_POST);
    }
    board_main();
    mail_self(FN_ETC_CROSSPOST, str_sysop, "Cross-Post ���v", 0);
    vmsg("�z�]���L�� Cross-Post �w�Q���v");
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


static int
do_post(xo, title)
  XO *xo;
  char *title;
{
  /* Thor.981105: �i�J�e�ݳ]�n curredit �� quote_file */
  HDR hdr;
  char fpath[64], *folder, *nick, *rcpt;
  int mode;
  time_t spendtime;
  char buf_filepath[50];                       /* smiler.070914: for post_filter */
  HDR  hdr2;                                   /* smiler.070916: for ���� nthu.forsale */
  char fpath2[64], folder2[64];                // smiler.070916
  char board_from[30];                         // smiler.070916

  if (!(bbstate & STAT_POST))
  {
#ifdef NEWUSER_LIMIT
    if (cuser.lastlogin - cuser.firstlogin < 3 * 86400)
      vmsg("�s��W���A�T���l�i�i�K�峹");
    else
#endif      
      vmsg("�藍�_�A�z�S���b���o��峹���v��");
    return XO_FOOT;
  }

  film_out(FILM_POST, 0);

  prints("�o��峹��i %s �j�ݪO", currboard);

#ifdef POST_PREFIX
  /* �ɥ� mode�Brcpt�Bfpath */

  if (title)
  {
    rcpt = NULL;
  }
  else		/* itoc.020113: �s�峹��ܼ��D���� */
  {
#define NUM_PREFIX 6
    char *prefix[NUM_PREFIX] = {"[���i] ", "[�s�D] ", "[����] ", "[���] ", "[���D] ", "[����] "};

    move(21, 0);
    outs("���O�G");
    for (mode = 0; mode < NUM_PREFIX; mode++)
      prints("%d.%s", mode + 1, prefix[mode]);

    mode = vget(20, 0, "�п�ܤ峹���O�]�� Enter ���L�^�G", fpath, 3, DOECHO) - '1';
    if (mode >= 0 && mode < NUM_PREFIX)		/* ��J�Ʀr�ﶵ */
      rcpt = prefix[mode];
    else					/* �ťո��L */
      rcpt = NULL;
  }

  if (!ve_subject(21, title, rcpt))
#else
  if (!ve_subject(21, title, NULL))
#endif
      return XO_HEAD;

  /* ����� Internet �v���̡A�u��b�����o��峹 */
  /* Thor.990111: �S��H�X�h���ݪO, �]�u��b�����o��峹 */

  if (!HAS_PERM(PERM_INTERNET) || (currbattr & BRD_NOTRAN))
    curredit &= ~EDIT_OUTGO;

  utmp_mode(M_POST);
  fpath[0] = '\0';
  time(&spendtime);
  if (vedit(fpath, 1) < 0)
  {
    unlink(fpath);
    vmsg(msg_cancel);
    return XO_HEAD;
  }
  spendtime = time(0) - spendtime;	/* itoc.010712: �`�@�᪺�ɶ�(���) */

  /* build filename */

  folder = xo->dir;
  hdr_stamp(folder, HDR_LINK | 'A', &hdr, fpath);

  strcpy(buf_filepath,folder);                        // smiler.070914
  buf_filepath[strlen(buf_filepath)-4]=hdr.xname[7];  // smiler.070914
  buf_filepath[strlen(buf_filepath)-3]='/';           // smiler.070914
  buf_filepath[strlen(buf_filepath)-2]='\0';          // smiler.070914
  strcat(buf_filepath,hdr.xname);                     // smiler.070914

  strcpy(fpath2,fpath);                               // smiler.070916
  strcpy(folder2,folder);                             // smiler.070916
  folder2[4]='\0';                                    // smiler.070916
  strcat(folder2,"nthu.forsale/.DIR");                // smiler.070916 
  hdr_stamp(folder2, HDR_LINK | 'A', &hdr2, fpath2);  // smiler.070916
  strcpy(board_from,folder+4);                        // smiler.070916
  board_from[strlen(board_from)-5]='\0';              // smiler.070916

  /* set owner to anonymous for anonymous board */

#ifdef HAVE_ANONYMOUS
  /* Thor.980727: lkchu�s�W��[²�檺��ܩʰΦW�\��] */
  if (curredit & EDIT_ANONYMOUS)
  {
    rcpt = anonymousid;	/* itoc.010717: �۩w�ΦW ID */
    nick = STR_ANONYMOUS;

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
  strcpy(hdr.owner, rcpt);
  strcpy(hdr.nick, nick);
  strcpy(hdr.title, title);

  hdr2.xmode = mode;                     // smiler.070916
  strcpy(hdr2.owner, rcpt);              // smiler.070916
  strcpy(hdr2.nick, nick);               // smiler.070916
  strcpy(hdr2.title, title);             // smiler.070916

#ifdef DO_POST_FILTER
  if(post_filter(buf_filepath) ==1)       /* smiler 0914 */
  {
    unlink(buf_filepath);
    vmsg("�ФŤj�q�~�O,�~��,�s�i !!");
    return XO_HEAD;
  }
#endif
  rec_bot(folder, &hdr, sizeof(HDR));
  btime_update(currbno);

  /* smiler 0916 */
  if(strstr(hdr2.title,"��") || strstr(hdr2.title,"��") || strstr(hdr2.title,"�X�M"))
  {
	 if( (!strstr(board_from,"P_")) && (!strstr(board_from,"R_")) && 
	   (!strstr(board_from,"LAB_")) && (!strstr(board_from,"G_")) )
	 {
          rec_bot(folder2, &hdr2, sizeof(HDR));
          btime_update(brd_bno("nthu.forsale"));
	 }
  }


  if (mode & POST_OUTGO)
    outgo_post(&hdr, currboard);

  post_history(xo, &hdr);

  clear();
  outs("���Q�K�X�峹�A");

  if (currbattr & BRD_NOCOUNT || wordsnum < 30)
  {				/* itoc.010408: �H���������{�H */
    outs("�峹���C�J�����A�q�Х]�[�C");
  }
  else
  {
    /* itoc.010408: �̤峹����/�ҶO�ɶ��ӨM�w�n���h�ֿ��F����~�|���N�q */
    mode = BMIN(wordsnum, spendtime) / 10;	/* �C�Q�r/�� �@�� */
    prints("�o�O�z���� %d �g�峹�A�o %d �ȡC", ++cuser.numposts, mode);
    addmoney(mode);
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

  unlink(fpath);

  vmsg(NULL);

  return XO_INIT;
}


int
do_reply(xo, hdr)
  XO *xo;
  HDR *hdr;
{
  curredit = 0;

  switch (vans("�� �^���� (F)�ݪO (M)�@�̫H�c (B)�G�̬ҬO (Q)�����H[F] "))
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
  }

  /* Thor.981105: ���׬O��i��, �άO�n��X��, ���O�O���i�ݨ쪺, �ҥH�^�H�]��������X */
  if (hdr->xmode & (POST_INCOME | POST_OUTGO))
    curredit |= EDIT_OUTGO;

  hdr_fpath(quote_file, xo->dir, hdr);
  strcpy(quote_user, hdr->owner);
  strcpy(quote_nick, hdr->nick);
  return do_post(xo, hdr->title);
}


static int
post_reply(xo)
  XO *xo;
{
  if (bbstate & STAT_POST)
  {
    HDR *hdr;

    hdr = (HDR *) xo_pool + (xo->pos - xo->top);

#ifdef HAVE_REFUSEMARK
    if ((hdr->xmode & POST_RESTRICT) && !RefusePal_belong(currboard, hdr))
      return XO_NONE;
#endif

    return do_reply(xo, hdr);
  }
  return XO_NONE;
}


static int
post_add(xo)
  XO *xo;
{
  curredit = EDIT_OUTGO;
  *quote_file = '\0';
  return do_post(xo, NULL);
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
static inline int
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
  static char *type[6] = {"Re", "Fw", "��", "\033[1;33m=>", "\033[1;33m=>", "\033[1;32m��"};
  uschar *title, *mark;
  int ch, len;
  int in_chi;		/* 1: �b����r�� */
  char title_tmp[64];  //smiler 1108
  int chi_detect;      //smiler 071114
  int i;               //smiler 071114
#ifdef HAVE_DECLARE
  int square;		/* 1: �n�B�z��A */
#endif
#ifdef CHECK_ONLINE
  UTMP *online;
#endif

   cc=cc-1; /*smiler.070724: ���D�ֻݦL�@�� */
  /* --------------------------------------------------- */
  /* �L�X���						 */
  /* --------------------------------------------------- */

#ifdef HAVE_DECLARE
  /* itoc.010217: ��άP���X�ӤW�� */
  /*smiler.070724: ����L�X�e,�h�Ť@��*/
  prints(" \033[1;3%dm%s\033[m ", cal_day(hdr->date) + 1, hdr->date + 3);

#else
  prints(" \033[m%s\033[m ",hdr->date + 3);
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
  if((hdr->xmode & POST_RESTRICT)) //smiler 1108
  {
      strcpy(title_tmp,"<< �峹�O�K >>"); //smiler 1108
      title = str_ttl(mark = title_tmp);
  }
  else
     title = str_ttl(mark = hdr->title);

  len = (title == mark) ? 2 : (*mark == 'R') ? 0 : 1;
  if (!strcmp(currtitle, title))
    len += 3;
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
  while ((ch = *title++) && (title < mark-3))
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

  if(title == mark-3){

    if(strlen(title) < 4){
      outc(ch);
      while (ch = *title++)
	outc(ch);
    }
    else
      outs(" ...");
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
hdr_outs_bar(hdr, cc)           /* print HDR's subject */
  HDR *hdr;
  int cc;               /* �L�X�̦h cc - 1 �Ӧr */
{

  /* �^��/���/���/�\Ū�����P�D�D�^��/�\Ū�����P�D�D���/�\Ū�����P�D�D��� */
  static char *type[6] = {"Re", "Fw", "��", "\033[1;33m=>", "\033[1;33m=>", "\033[1;32m��"};
  uschar *title, *mark;
  int ch, len;
  int in_chi;		/* 1: �b����r�� */
  char title_tmp[64];  //smiler 1108
  int chi_detect;      //smiler 071114
  int i;               //smiler 071114
#ifdef HAVE_DECLARE
  int square;		/* 1: �n�B�z��A */
#endif
#ifdef CHECK_ONLINE
  UTMP *online;
#endif

   cc=cc-1; /*smiler.070724: ���D�ֻݦL�@�� */
  /* --------------------------------------------------- */
  /* �L�X���						 */
  /* --------------------------------------------------- */

#ifdef HAVE_DECLARE
  /* itoc.010217: ��άP���X�ӤW�� */
  /*smiler.070724: ����L�X�e,�h�Ť@��*/
  prints(COLORBAR_POST" \033[1;3%dm%s\033[m"COLORBAR_POST" ", cal_day(hdr->date) + 1, hdr->date + 3);

#else
  prints(COLORBAR_POST" \033[m%s\033[m"COLORBAR_POST" ",hdr->date + 3);
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
  if((hdr->xmode & POST_RESTRICT)) //smiler 1108
  {
      strcpy(title_tmp,"<< �峹�O�K >>"); //smiler 1108
      title = str_ttl(mark = title_tmp);
  }
  else
     title = str_ttl(mark = hdr->title);

  len = (title == mark) ? 2 : (*mark == 'R') ? 0 : 1;
  if (!strcmp(currtitle, title))
    len += 3;
  prints(COLORBAR_POST);
  outs(type[len]);
  prints(COLORBAR_POST);
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

  prints(COLORBAR_POST);

  /* ��W�L cc ���ת������������� */
  /* itoc.060604.����: �p�G��n���b����r���@�b�N�|�X�{�ýX�A���L�o���p�ܤֵo�͡A�ҥH�N���ޤF */
  while ((ch = *title++) && (title < mark-3))
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
	outs("]\033[m"COLORBAR_POST);
	square = 0;			/* �u�B�z�@�դ�A�A��A�w�g�B�z���F */
	continue;
      }
    }
#endif

    outc(ch);
  }

  if(title == mark-3){

    if(strlen(title) < 4){
      outc(ch);
      while (ch = *title++)
        outc(ch);
     }  
     else{
        outs(" ...");
        title += 4;
	    }
  }
#if 1
  while(title <= mark)
  {
	  prints(" ");
	  title++;
  }
#endif

#ifdef HAVE_DECLARE
  if (square || len >= 3)	/* Thor.980508: �ܦ��٭�� */
#else
  if (len >= 3)
#endif
    outs("\033[m");

  prints("\033[m");
  //outc('\n');
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


static int
post_attr(hdr)
  HDR *hdr;
{
  int mode, attr;

  mode = hdr->xmode;

  /* �w�\Ū���p�g�A���\Ū���j�g */
  /* �ѩ�m����S���\Ū�O���A�ҥH�����wŪ */
  /* �[�K�峹�����wŪ */
#ifdef HAVE_REFUSEMARK
  attr = ((mode & POST_BOTTOM) || !brh_unread(BMAX(hdr->chrono, hdr->stamp)) || 
    ((mode & POST_RESTRICT) && strcmp(hdr->owner, cuser.userid) && !(bbstate & STAT_BM))) ? 0x20 : 0;
#else
  attr = ((mode & POST_BOTTOM) || !brh_unread(BMAX(hdr->chrono, hdr->stamp)) ? 0x20 : 0;
#endif

#ifdef HAVE_REFUSEMARK
  if (mode & POST_RESTRICT)
  {
   if (RefusePal_level(currboard, hdr)==1)
       attr |= 'F';
   else
    attr |= 'L';
  }
  else
#endif
#ifdef HAVE_LABELMARK
  if (mode & POST_DELETE)
    attr |= 'T';
  else
#endif
  if (mode & POST_NOFORWARD)
    attr |= 'X';
  else if (mode & POST_NOSCORE)
    attr |= 'N';
  else if (mode & POST_MARKED)
    attr |= 'M';
  else if (!attr)
    attr = '+';

  return attr;
}


static void
post_item(num, hdr)
  int num;
  HDR *hdr;
{
#ifdef HAVE_SCORE
  if(!(cuser.ufo & UFO_FILENAME))
  {  
   if(hdr->xmode & POST_BOTTOM)
     prints("  \033[1;33m���n  \033[m");
   else
     prints("%6d%c%c", (hdr->xmode & POST_BOTTOM) ? -1 : num, tag_char(hdr->chrono), post_attr(hdr));
   if (hdr->xmode & POST_SCORE)
   {
     num = hdr->score;
     if (num <= 99 && num >= -99)
       prints("\033[%c;3%cm%2d\033[m", num == 0 ? '0' : '1', num > 0 ? '1' : num < 0 ? '2' : '7' , abs(num));
     else
       prints("\033[1;3%s\033[m", num >= 0 ? "1m�z" : "2m�N");
   }
   else
   {
     outs("  ");
   }
  }
  else
   prints("%10s",hdr->xname);

  hdr_outs(hdr, d_cols + 46);	/* �֤@��ө���� */
#else
 if(!(cuser.ufo & UFO_FILENAME))
 {
   /*ckm.070325: �m����S���s��*/
   if(hdr->xmode & POST_BOTTOM)
     prints("  \033[1;33m���n\033[m  ");
   else
     prints("%6d%c%c ", (hdr->xmode & POST_BOTTOM) ? -1 : num, tag_char(hdr->chrono), post_attr(hdr));
 }
 else
   prints("%10s",hdr->xname);    
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
                                                                                
#ifdef HAVE_SCORE
  /*static char scorelist[36] =
  {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
  };*/
                                                                                
  hdr = (HDR *) xo_pool + xo->pos - xo->top;
  num = xo->pos + 1;

 if(!(cuser.ufo & UFO_FILENAME))
 {
  
   if(hdr->xmode & POST_BOTTOM)
   {
   prints("%s%s%s%c%c",
     mode ? COLORBAR_POST : "",
     "  \033[1;33m���n\033[m",mode ? COLORBAR_POST : "",
     tag_char(hdr->chrono), post_attr(hdr));
   }
   else
   {
   prints("%s%6d%c%c",
     mode ? COLORBAR_POST : "",
     num,
     tag_char(hdr->chrono), post_attr(hdr));
   } 


   if (hdr->xmode & POST_SCORE)
   {
     //num = hdr->score;
     num = hdr->score;
     if (num <= 99 && num >= -99)
         prints("%s\033[%c;3%cm%s%2d\033[m%s",mode ? COLORBAR_POST : "", '1', num > 0 ? '1' : num < 0 ? '2' : '7' ,mode ? COLORBAR_POST : num > 0 ? "\033[m\033[1;31m" : num < 0 ? "\033[m\033[1;32m" : "\033[m\033[1;37m" , abs(num),mode ? COLORBAR_POST : "");
     else
       prints("%s\033[1;3%s\033[m%s",mode ? COLORBAR_POST : "", num >= 0 ? "1m�z" : "2m�N",mode ? COLORBAR_POST : "");
   }
   else
   {
     outs("  ");
   }
 }
 else
   prints("%s%10s",mode ? COLORBAR_POST : "",hdr->xname);
                                                                                
  if (mode)
    hdr_outs_bar(hdr, 46);    /* �֤@��ө���� */
  else
    hdr_outs(hdr, 46);
#else
  hdr = (HDR *) xo_pool + xo->pos - xo->top;
  num = xo->pos + 1;
  if(!(cuser.ufo & UFO_FILENAME))
  {
   if(hdr->xmode & POST_BOTTOM)
   {
     prints("%s%s%s%c%c ",
     mode ? COLORBAR_POST : "",
     "  \033[1;33m���n\033[m",mode ? COLORBAR_POST : "",
     tag_char(hdr->chrono), post_attr(hdr));
   }
   else
   {
     prints("%s%6d%c%c ",
     mode ? COLORBAR_POST : "",
     num,
     tag_char(hdr->chrono), post_attr(hdr));
   }
  }
  else
    prints("%s%10s",mode ? COLORBAR_POST : "",hdr->xname);
                                                                                
  if (mode)
    hdr_outs_bar(hdr, 47);
  else
    hdr_outs(hdr, 47);
#endif
                                                                                
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
  if(!(cuser.ufo & UFO_FILENAME))
    prints(NECKER_POST, d_cols, "", currbattr & BRD_NOSCORE ? "��" : "��", bshm->mantime[currbno]);
  else
    prints(NECKER_POST_FILE, d_cols, "", currbattr & BRD_NOSCORE ? "��" : "��", bshm->mantime[currbno]);
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

    hdr = (HDR *) xo_pool;
    row = 3;
    do
    {
      move(row, 7);
      outc(post_attr(hdr++));
    } while (++row < max);
  }
  return XO_FOOT;
}


void
post_history(xo, hdr)          /* �N hdr �o�g�[�J brh */
  XO *xo;
  HDR *hdr;
{
  int fd;
  time_t prev, chrono, next, this;
  HDR buf;
                                                                                
  chrono = BMAX(hdr->chrono, hdr->stamp);
  if (!brh_unread(chrono))      /* �p�G�w�b brh ���A�N�L�ݰʧ@ */
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
                                                                                
    if (prev > chrono)      /* �S���U�@�g */
      prev = chrono;
    if (next < chrono)      /* �S���W�@�g */
      next = chrono;
                                                                                
    brh_add(prev, chrono, next);
  }
}



static int
post_browse(xo)
  XO *xo;
{
  HDR *hdr;
  int xmode, pos, key;
  char *dir, fpath[64];

  dir = xo->dir;

  for (;;)
  {
    pos = xo->pos;
    hdr = (HDR *) xo_pool + (pos - xo->top);
    xmode = hdr->xmode;

#ifdef HAVE_REFUSEMARK
    if ((xmode & POST_RESTRICT) && !RefusePal_belong(currboard, hdr))
      break;
#endif

    hdr_fpath(fpath, dir, hdr);

    /* Thor.990204: ���Ҽ{more �Ǧ^�� */   
    if ((key = more(fpath, FOOTER_POST)) < 0)
      break;

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
    case '%':
      post_score(xo);
      return post_init(xo);
#endif

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
  return post_init(xo);
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

  /*  �ѨM�H�c��H���D */
  int comefrom;          // 0: �q�H�c��H 1: �q�ݪO��H
  char mail_path_tmp[64];
  char userid_tmp[15];

  str_lower_tmp(userid_tmp,cuser.userid);
  sprintf(mail_path_tmp,"usr/%c/%s/.DIR",userid_tmp[0],userid_tmp);
  if(!strcmp(mail_path_tmp,xo->dir))
	  comefrom=0;
  else
	  comefrom=1;


  if (!cuser.userlevel)	/* itoc.000213: �קK guest ����h sysop �O */
    return XO_NONE;

#ifdef HAVE_REFUSEMARK
  pos = xo->pos;             //smiler 1108
  cur = pos - xo->top;       //smiler 1108
  hdr_org = (HDR *) xo_pool + (xo->pos - xo->top);
  //if(RefusePal_level(currboard, hdr_org)!=0)  //�Y��L���F��,�ȪO�D�Χ@�̥i���
  if(hdr_org->xmode & POST_RESTRICT)
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
    (vget(2, 0, "(1)������ (2)����峹�H[2] ", buf, 3, DOECHO) == '1') ? 0 : 1; /* smiler.070602: ��w�]��2 */

  if (!tag)	/* lkchu.981201: �������N���n�@�@�߰� */
  {
    if (method)
      sprintf(ve_title, "[���] %.65s", hdr->title); /* smiler.070602: �אּ�����,���D��[���]�}�Y */
    else
      strcpy(ve_title, hdr->title);

    if (!vget(2, 0, "���D�G", ve_title, TTLEN + 1, GCARRY))
      return XO_HEAD;
  }

#ifdef HAVE_REFUSEMARK    
  rc = vget(2, 0, "(S)�s�� (L)���� (X)�K�� (Q)�����H[Q] ", buf, 3, LCECHO);
  if (rc != 'l' && rc != 's' && rc != 'x')
#else
  rc = vget(2, 0, "(S)�s�� (L)���� (Q)�����H[Q] ", buf, 3, LCECHO);
  if (rc != 'l' && rc != 's')
#endif
    return XO_HEAD;

  if (method && *dir == 'b')	/* �q�ݪO��X�A���ˬd���ݪO�O�_�����K�O */
  {
    /* �ɥ� tmpbattr */
    tmpbattr = (bshm->bcache + currbno)->readlevel;
    if (tmpbattr == PERM_SYSOP || tmpbattr == PERM_BOARD)
      method = 2;
  }

  xbno = brd_bno(xboard);
  xbattr = (bshm->bcache + xbno)->battr;

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

	if(comefrom)                    /* smiler.071114: �ݬ��B�b�ݪO,�U���X��~�ݧ@�P�_ */
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

#ifdef HAVE_DETECT_CROSSPOST
    if (check_crosspost(fpath, xbno))
      break;
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

  vmsg("�������");
  return XO_HEAD;
}


int
post_forward(xo)
  XO *xo;
{
  ACCT muser;
  int pos;
  HDR *hdr;

  if (!HAS_PERM(PERM_LOCAL))
    return XO_NONE;

  pos = xo->pos;
  hdr = (HDR *) xo_pool + (pos - xo->top);

  if (hdr->xmode & GEM_FOLDER)	/* �D plain text ������ */
    return XO_NONE;

#ifdef HAVE_REFUSEMARK
//  if ((hdr->xmode & POST_RESTRICT) && !RefusePal_belong(currboard, hdr))
//    return XO_NONE;
  //if(RefusePal_level(currboard, hdr)!=0)  //�Y��L���F��,�ȪO�D�Χ@�̥i���
  if (hdr->xmode & POST_RESTRICT)
  {
  if (strcmp(hdr->owner, cuser.userid) && !(bbstate & STAT_BM))
    return XO_NONE;
  }
#endif

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

    hdr->xmode = xmode ^ POST_MARKED;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono);

    move(3 + cur, 7);
    outc(post_attr(hdr));
  }
  return XO_NONE;
}


/*ryancid: reset parent_chrono*/
static void
reset_parent_chrono(hdd, ram)
  HDR *hdd, *ram;
{
  hdd->parent_chrono=0;
}

/*ryancid: set parent_chrono*/
static void
set_parent_chrono(hdd, ram)
  HDR *hdd, *ram;
{
  hdd->parent_chrono=1;
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
  
  if(!(hdr->xmode & POST_BOTTOM))
  {
    /*Allow only one bottom post per article*/	
    if(hdr->parent_chrono!=0)
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
    if(hdr->xmode & POST_SCORE)
    {
      post.xmode |= POST_SCORE;
      post.score = hdr->score;
    }
    /**/

    strcpy(post.owner, hdr->owner);
    strcpy(post.nick, hdr->nick);
    strcpy(post.title, hdr->title);

    rec_add(xo->dir, &post, sizeof(HDR));
    /**/
    currchrono = hdr -> chrono;
    rec_ref(xo->dir, hdr, sizeof(HDR), \
	xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono ,\
	set_parent_chrono);/*ryancid: set the parent_chrono*/
  }
  else
  {
    /**/
    currchrono = hdr->parent_chrono;
    rec_ref(xo->dir, hdr, sizeof(HDR), \
	xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono ,\
	reset_parent_chrono);/*reset the original chrono*/

    currchrono = hdr->chrono;
    
    if (!rec_del(xo->dir, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono))
    {
      pos = move_post(hdr, xo->dir, bbstate & STAT_BOARD);
    }
  }
  
  return post_load(xo); /* ckm.070325: ���s���J�C�� */         
}


#ifdef HAVE_REFUSEMARK
static int
post_friend(xo)
  XO *xo;
{
  HDR *hdr;
  int pos, cur;
                                                                                
  if (!cuser.userlevel) /* itoc.020114: guest ������L guest ���峹�[�K */
    return XO_NONE;
 
  if((currbattr & BRD_PUBLIC))
	  return XO_NONE;
  if((!(bbstate & STAT_BM)) && (currbattr & BRD_NOL))
    return XO_NONE;
  
  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;
                                                                                
  if (!strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM))
  {
   if (hdr->xmode & POST_RESTRICT)
   {
     if(RefusePal_level(currboard, hdr)== (-1))  //L�夣�i��l����
         return XO_NONE;
#if 0
     if (vans("�Ѱ��峹�O�K�|�R�������i���W��A�z�T�w��(Y/N)�H[N] ") != 'y')
     {
       move(3 + cur, 7);
       outc(post_attr(hdr));
       return XO_FOOT;
     }
#endif
     RefusePal_kill(currboard, hdr);
                                                                                
   }
                                                                                
                                                                                
    hdr->xmode ^= POST_RESTRICT;
	hdr->xmode ^= POST_FRIEND;      /* smiler 1108 */
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ?
hdr->xid : pos, cmpchrono);
                                                                                
   if (hdr->xmode & POST_RESTRICT)
     return XoBM_Refuse_pal(xo);
                                                                                
    move(3 + cur, 7);
    outc(post_attr(hdr));
  }
  return XO_NONE;
                                                                                
}
                                                                                
                                                                                
static int
post_refuse(xo)     /* itoc.010602: �峹�[�K */
  XO *xo;
{
  HDR *hdr;
  int pos, cur;
                                                                                
  if (!cuser.userlevel) /* itoc.020114: guest ������L guest ���峹�[�K */
    return XO_NONE;
  
  if((currbattr & BRD_PUBLIC))
	  return XO_NONE;
  if((!(bbstate & STAT_BM)) && (currbattr & BRD_NOL) )
    return XO_NONE;
  
  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;
                                                                                
  if (!strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM))
  {
   if (hdr->xmode & POST_RESTRICT)
   {
       if(RefusePal_level(currboard, hdr)==1)  //F�夣�i��L����
           return XO_NONE;
   }
                                                                                
    hdr->xmode ^= POST_RESTRICT;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ?
hdr->xid : pos, cmpchrono);
                                                                                
    move(3 + cur, 7);
    outc(post_attr(hdr));                                                                                
  }
                                                                                
  return XO_NONE;
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
    outc(post_attr(hdr));

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

/* ��@�R�� */
static int
post_delete(xo)
  XO *xo;
{
  int pos, cur, by_BM;
  HDR *hdr;
  char buf[80];
  char Deletelog_folder[64],copied[64];
  HDR  Deletelog_hdr;

  /* smiler.1111: �O�@Deletelog �H�� Editlog �� �O�����Q���� */
 if((!strcmp(xo->dir,"brd/Deletelog/.DIR")) || (!strcmp(xo->dir,"brd/Editlog/.DIR")))
   return XO_NONE;

  if (!cuser.userlevel ||
    !strcmp(currboard, BN_DELETED) ||
    !strcmp(currboard, BN_JUNK))
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;

  if ((hdr->xmode & POST_BOTTOM) || (hdr->xmode & POST_MARKED) ||
    (!(bbstate & STAT_BOARD) && strcmp(hdr->owner, cuser.userid)))
    return XO_NONE;

  by_BM = bbstate & STAT_BOARD;

  if (vans(msg_del_ny) == 'y')
  {
    /* smiler 1031 */
    if(deletelog_use)
	{
      hdr_fpath(copied,xo->dir,hdr);
      brd_fpath(Deletelog_folder,"Deletelog", FN_DIR);
      hdr_stamp(Deletelog_folder, HDR_COPY | 'A', &Deletelog_hdr, copied);                                                                                
      strcpy(Deletelog_hdr.title , hdr->title);
      strcpy(Deletelog_hdr.owner , cuser.userid);
      strcpy(Deletelog_hdr.nick  , cuser.username);
      Deletelog_hdr.xmode = POST_OUTGO;
      rec_bot(Deletelog_folder, &Deletelog_hdr, sizeof(HDR));
      btime_update(brd_bno("Deletelog"));
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
  char Deletelog_folder[64],Deletelog_title[64],copied[64];
  HDR  Deletelog_hdr;

//  /* smiler.1111: �O�@Deletelog �H�� Editlog �� �O�����Q���� */
// if((!strcmp(xo->dir,"brd/Deletelog/.DIR")) || (!strcmp(xo->dir,"brd/Editlog/.DIR")))
//   return XO_NONE;

  if(deletelog_use)
  {
   hdr_fpath(copied,xo->dir,hdr);
   brd_fpath(Deletelog_folder,"Deletelog", FN_DIR);
   hdr_stamp(Deletelog_folder, HDR_COPY | 'A', &Deletelog_hdr, copied);
   strcpy(Deletelog_hdr.title , hdr->title);
   strcpy(Deletelog_hdr.owner , cuser.userid);
   strcpy(Deletelog_hdr.nick  , cuser.username);
   Deletelog_hdr.xmode = POST_OUTGO;
   rec_bot(Deletelog_folder, &Deletelog_hdr, sizeof(HDR));
   btime_update(brd_bno("Deletelog"));
  }


  cancel_post(hdr);
  hdr_fpath(fpath, xo->dir, hdr);
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
  if((!strcmp(xo->dir,"brd/Deletelog/.DIR")) || (!strcmp(xo->dir,"brd/Editlog/.DIR")))
    return XO_NONE;

  btime_update(currbno);

  return xo_rangedel(xo, sizeof(HDR), chkpost, delpost);
}


static int
post_prune(xo)
  XO *xo;
{
  int ret;

  /* smiler.071111: �O�@Deletelog �H�� Editlog �� �O�����Q���� */
  if((!strcmp(xo->dir,"brd/Deletelog/.DIR")) || (!strcmp(xo->dir,"brd/Editlog/.DIR")))
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


#ifdef HAVE_REFUSEMARK
static int
chkrestrict(hdr)
  HDR *hdr;
{
  return !(hdr->xmode & POST_RESTRICT) || 
    !strcmp(hdr->owner, cuser.userid) || (bbstate & STAT_BM);
}
#endif


static int
post_copy(xo)	   /* itoc.010924: ���N gem_gather */
  XO *xo;
{
  int tag;

  tag = AskTag("�ݪO�峹����");

  if (tag < 0)
    return XO_FOOT;

#ifdef HAVE_REFUSEMARK
  gem_buffer(xo->dir, tag ? NULL : (HDR *) xo_pool + (xo->pos - xo->top), chkrestrict);
#else
  gem_buffer(xo->dir, tag ? NULL : (HDR *) xo_pool + (xo->pos - xo->top), NULL);
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
/* �����\��Gedit / title				 */
/* ----------------------------------------------------- */


int
post_edit(xo)
  XO *xo;
{
  char fpath[64];
  HDR *hdr;
  FILE *fp;
  /* smiler 1031 */
  char Editlog_folder[64],Editlog_title[64],copied[64];
  HDR  Editlog_hdr;

  /* smiler.071111 �O�@ Editlog �H�� Deletelog �O���ƥ���� */
 if((!strcmp(xo->dir,"brd/Deletelog/.DIR")) || (!strcmp(xo->dir,"brd/Editlog/.DIR")))
   return XO_NONE;

  hdr = (HDR *) xo_pool + (xo->pos - xo->top);

  /* smiler 1031 */
  hdr_fpath(copied,xo->dir,hdr);
  brd_fpath(Editlog_folder,"Editlog", FN_DIR);


  hdr_fpath(fpath, xo->dir, hdr);
 
  if (HAS_PERM(PERM_ALLBOARD))			/* �����ק� */
  {
#ifdef HAVE_REFUSEMARK
    if ((hdr->xmode & POST_RESTRICT) && !RefusePal_belong(currboard, hdr))
      return XO_NONE;
#endif

    /* smiler 1031 */                                                                                
    hdr_stamp(Editlog_folder, HDR_COPY | 'A', &Editlog_hdr, copied);
    strcpy(Editlog_hdr.title , hdr->title);
    strcpy(Editlog_hdr.owner , hdr->owner);
    strcpy(Editlog_hdr.nick  , hdr->nick);
    Editlog_hdr.xmode = POST_OUTGO;
    rec_bot(Editlog_folder, &Editlog_hdr, sizeof(HDR));
    btime_update(brd_bno("Editlog"));

    vedit(fpath, 0);

    /* smiler 1031 */                                                                                
    hdr_stamp(Editlog_folder, HDR_COPY | 'A', &Editlog_hdr, copied);
    strcpy(Editlog_hdr.title , hdr->title);
    strcpy(Editlog_hdr.owner , cuser.userid);
    strcpy(Editlog_hdr.nick  , cuser.username);
    Editlog_hdr.xmode = POST_OUTGO;
    rec_bot(Editlog_folder, &Editlog_hdr, sizeof(HDR));
    btime_update(brd_bno("Editlog"));

  }
  else if ((cuser.userlevel && !strcmp(hdr->owner, cuser.userid)) || (bbstate & STAT_BOARD))	/* �O�D/��@�̭ק� */
  {

	/* smiler 1031 */
    hdr_stamp(Editlog_folder, HDR_COPY | 'A', &Editlog_hdr, copied);
    strcpy(Editlog_hdr.title , hdr->title);
    strcpy(Editlog_hdr.owner , hdr->owner);
    strcpy(Editlog_hdr.nick  , hdr->nick);
    Editlog_hdr.xmode = POST_OUTGO;
    rec_bot(Editlog_folder, &Editlog_hdr, sizeof(HDR));
    btime_update(brd_bno("Editlog"));

    if (!vedit(fpath, 0))	/* �Y�D�����h�[�W�ק��T */
    {
      if (fp = fopen(fpath, "a"))
      {
	ve_banner(fp, 1);
	fclose(fp);
      }
    }
    /* smiler 1031 */                                                                                
    hdr_stamp(Editlog_folder, HDR_COPY | 'A', &Editlog_hdr, copied);
    strcpy(Editlog_hdr.title , hdr->title);
    strcpy(Editlog_hdr.owner , cuser.userid);
    strcpy(Editlog_hdr.nick  , cuser.username);
    Editlog_hdr.xmode = POST_OUTGO;
    rec_bot(Editlog_folder, &Editlog_hdr, sizeof(HDR));
    btime_update(brd_bno("Editlog"));

  }
#if 0
  else		/* itoc.010301: ���ѨϥΪ̭ק�(�������x�s)��L�H�o���峹 */
  {
#ifdef HAVE_REFUSEMARK
    if (hdr->xmode & POST_RESTRICT)
      return XO_NONE;
#endif
    vedit(fpath, -1);
  }
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

  while(fgets(buf, sizeof(buf), fpr))	/* �[�J��L */
    fputs(buf, fpw);

  fclose(fpr);
  fclose(fpw);
  f_rm(tmpfile);
}


static int
post_title(xo)
  XO *xo;
{
  HDR *fhdr, mhdr;
  int pos, cur;

  if (!cuser.userlevel)	/* itoc.000213: �קK guest �b sysop �O����D */
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  fhdr = (HDR *) xo_pool + cur;
  memcpy(&mhdr, fhdr, sizeof(HDR));

  if (strcmp(cuser.userid, mhdr.owner) && !HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;

  vget(b_lines, 0, "���D�G", mhdr.title, TTLEN + 1, GCARRY);

  if (HAS_PERM(PERM_ALLBOARD))  /* itoc.000213: ��@�̥u�����D */
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


static void
addscore(hdd, ram)
  HDR *hdd, *ram;
{
  hdd->xmode |= POST_SCORE;
  hdd->stamp = ram->stamp;
  if (curraddscore < 50 )
  {
    if (hdd->score < 127)
      hdd->score++;
  }
  else if (curraddscore == 50)
  {
    if (hdd->score > -128)
      hdd->score--;
  }
}


int
post_score(xo)
  XO *xo;
{
  HDR *hdr;
  int pos, cur, ans, vtlen, maxlen;
  char *dir, *userid, *verb, fpath[64], reason[80];/*, vtbuf[12];*/
  FILE *fp;
#ifdef HAVE_ANONYMOUS
  char uid[IDLEN + 1];
#endif

  if ((currbattr & BRD_NOSCORE) || !cuser.userlevel || !(bbstate & STAT_POST) )	/* �������P�o��峹 */
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;
  
  if (hdr->xmode & POST_NOSCORE)
    return XO_NONE;

#ifdef HAVE_REFUSEMARK
  if ((hdr->xmode & POST_RESTRICT) && !RefusePal_belong(currboard, hdr))
    return XO_NONE;
#endif

  switch (ans = vans("�� 1)�����u�n 2)ť�A���� 3)��L�N�� [3] "))
  {
  case '1':
    verb = "1m��";
    vtlen = 2;
    break;

  case '2':
    verb = "2m��";
    vtlen = 2;
    break;

  case '3':
    verb = "7m�w";
    vtlen = 2;
    break;
    /* songsongboy.070124:lexel version*/
    /*if (!vget(b_lines, 0, "�п�J�ʵ��G", fpath, 5, DOECHO))
      return XO_FOOT;
    vtlen = strlen(fpath);
    sprintf(verb = vtbuf, "%cm%s", ans - 2, fpath);
    break;*/

  default:
    ans='3';
    verb = "7m�w";
    vtlen = 2;
  }

#ifdef HAVE_ANONYMOUS
  if (currbattr & BRD_ANONYMOUS)
    maxlen = 64 - IDLEN - vtlen;
  else
#endif
    maxlen = 64 - strlen(cuser.userid) - vtlen;

  if (!vget(b_lines, 0, "�п�J�z�ѡG", reason, maxlen, DOECHO))
    return XO_FOOT;

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
    maxlen = 64 - strlen(userid) - vtlen;
  }
  else
#endif
    userid = cuser.userid;

  dir = xo->dir;
  hdr_fpath(fpath, dir, hdr);

  if (fp = fopen(fpath, "a"))
  {
    time_t now;
    struct tm *ptime;

    time(&now);
    ptime = localtime(&now);

    fprintf(fp, "\033[1;3%s\033[m \033[1;36m%s \033[m�G\033[0;33m%-*s\033[1;30m%02d/%02d/%02d\n", 
      verb, userid, maxlen, reason, 
      ptime->tm_year % 100, ptime->tm_mon + 1, ptime->tm_mday);
    fclose(fp);
  }

    curraddscore = ans;
    currchrono = hdr->chrono;
    change_stamp(xo->dir, hdr);
    rec_ref(dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ? hdr->xid : pos, cmpchrono, addscore);
    if (hdr->xmode & POST_BOTTOM)  /* �Y�O�����m���峹�A�h�䥿���ӳs�ʤ��� */
    {
      currchrono = hdr->parent_chrono;
      rec_ref(dir, hdr, sizeof(HDR), 0, cmpchrono, addscore);
    }
    else                           /* �Y�O�����@��峹�A�h���å��ӳs�ʤ��� */
    {
      /* currchrono = hdr->chrono; */ /* �e�����F */
      rec_ref(dir, hdr, sizeof(HDR), 0, cmpparent, addscore);
    }
    post_history(xo, hdr);
    btime_update(currbno);

    return XO_LOAD;

  return XO_FOOT;
}
#endif	/* HAVE_SCORE */

static int
post_noforward(xo)
  XO *xo;
{
  HDR *hdr;
  int pos, cur;
  
  if (!cuser.userlevel) /* guest ������L guest ���峹�[�K */
    return XO_NONE;
    
  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;
  
  if (HAS_PERM(PERM_ALLBOARD) || !strcmp(hdr->owner, cuser.userid))
  {
    hdr->xmode ^= POST_NOFORWARD;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ?
      hdr->xid : pos, cmpchrono);
      move(3 + cur, 7);
      outc(post_attr(hdr));
  }
  return XO_NONE;
}

static int
post_noscore(xo)
  XO *xo;
{
  HDR *hdr;
  int pos, cur;
  
  if (!cuser.userlevel) /* guest ������L guest ���峹�[�K */
      return XO_NONE;
  
  pos = xo->pos;
  cur = pos - xo->top;
  hdr = (HDR *) xo_pool + cur;
  
  if (HAS_PERM(PERM_ALLBOARD) || !strcmp(hdr->owner, cuser.userid))
  {
    hdr->xmode ^= POST_NOSCORE;
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR), xo->key == XZ_XPOST ?
      hdr->xid : pos, cmpchrono);
    move(3 + cur, 7);
    outc(post_attr(hdr));
  }
  return XO_NONE;
}

static int
post_help(xo)
  XO *xo;
{
  xo_help("post");
  /* return post_head(xo); */
  return XO_HEAD;		/* itoc.001029: �P xpost_help �@�� */
}

static int
post_state(xo)
  XO *xo;
{
  HDR *ghdr;
  char fpath[64], *dir;
  struct stat st;
  

  
  if (!HAS_PERM(PERM_ALLBOARD))
    return XO_NONE;
                                                                                
  ghdr = (HDR *) xo_pool + (xo->pos - xo->top);
                                                                                
  dir = xo->dir;

  hdr_fpath(fpath, dir, ghdr);
                                                                                
  move(3, 0);
  clrtobot();

  prints("========  �򥻸�� ========\n");
  if (!stat(fpath, &st))
    prints("\n�ɶ��W�O : %s\n�ɮפj�p : %d\n", Btime(&st.st_mtime), st.st_size);

  prints("DIR ��m : %s\n",dir);
  prints("�ɮצW�� : %s\n",ghdr->xname);
  prints("�ɮצ�m : %s\n",fpath);
  prints("�@��     : %s\n",ghdr->owner);
  prints("�ʺ�     : %s\n",ghdr->nick);
  prints("�o���� : %s\n",ghdr->date);
  prints("�峹���D : %s\n",ghdr->title);
  prints("========  �峹�ݩ� ========\n");

  
  prints("�峹�аO : ");
  if(ghdr->xmode & POST_MARKED)
	  prints("��");
  else
	  prints("�L");

  prints("    �i�_��H : ");
  if(ghdr->xmode & POST_NOFORWARD)
	  prints("�_\n");
  else
	  prints("�i\n");

  prints("�i�_���� : ");
  if(ghdr->xmode & POST_NOSCORE)
	  prints("�_");
  else
	  prints("�i");

  prints("    �m���� ? : ");
  if(ghdr->xmode & POST_BOTTOM)
	  prints("�O\n");
  else
	  prints("�_\n");


  prints("�аO�ݬ� : ");
  if(ghdr->xmode & POST_DELETE)
	  prints("�O");
  else
	  prints("�_");


  prints("    ��H�i�� : ");
  if(ghdr->xmode & POST_INCOME)
	  prints("�O\n");
  else
	  prints("�_\n");


  prints("��ŭ��� : ");
  if(ghdr->xmode & POST_FRIEND)
	  prints("�O");
  else
	  prints("�_");


  prints("    �i�௸�~ : ");
  if(ghdr->xmode & POST_OUTGO)
	  prints("�O\n");
  else
	  prints("�_\n");

  prints("���ŭ��� : ");
  if(ghdr->xmode & POST_RESTRICT)
	  prints("�O");
  else
	  prints("�_");

  prints("    �̰����� : ");
  if(ghdr->xmode & POST_RESERVED)
	  prints("�O\n");
  else
	  prints("�_\n");

  prints("�w�Q���� : ");
  if(ghdr->xmode & POST_SCORE)
	  prints("�O\n");
  else
	  prints("�_\n");
                                                                                
  vmsg(NULL);
                                                                                
  return post_body(xo);
}

/* smiler.080201: �e����� �ɦW<-> �g�� */
static int
post_filename(xo)
  XO *xo;
{
  cuser.ufo ^= UFO_FILENAME;
  cutmp->ufo = cuser.ufo;
  return XO_INIT;
}

static int
post_sub()
{
  char name[IDLEN + 1];
  char url[30], fpath[80];
  int bm;
  FILE *fp;
  
  bm = bbstate & STAT_BOARD;
  
  if (bm)
  {
    if (!vget(b_lines - 1, 0, "*bookmark* : ", name, IDLEN + 1, DOECHO))
      return XO_INIT;
    if (!vget(b_lines - 1, 0, "*url* : ", url, 70, DOECHO))
      return XO_INIT;

    brd_fpath(fpath, currboard, "rss");

    if(fp = fopen(fpath, "r"))
	{
		vmsg("�w��RSS");
		return XO_INIT;
	}
    else
      fp = fopen(fpath, "w");
    fprintf(fp, "%s\n%s", name, url);
    fclose(fp);   
  }  
  return XO_INIT;
}

static int
post_sub_delete()
{
  char fpath[80];
  int bm; 
  bm = bbstate & STAT_BOARD;
  
  if (bm)
  {
    brd_fpath(fpath, currboard, "rss");
    unlink(fpath);
  }  
  return XO_INIT;
}

static int
post_rss(xo)
  XO *xo;
{
  int bm;
  int ans;
  bm = bbstate & STAT_BOARD;  
  if (bm)
  {
     switch (ans = vans("��RSS 1)�s�W 2)�R�� [Q] "))
	 {
       case '1':
		   return post_sub();
		   break;
	   case '2':
		   return post_sub_delete();
		   break;
	   default:
		   return XO_INIT;
	}
  }
  return XO_INIT;
}

KeyFunc post_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, post_item_bar,         /* verit.20030129 : �аȥ��N����b�Ĥ@�� */
#endif
  XO_INIT, post_init,
  XO_LOAD, post_load,
  XO_HEAD, post_head,
  XO_BODY, post_body,

  'r', post_browse,
  's', post_switch,
  KEY_TAB, post_gem,
  'z', post_gem,
  'o', post_noforward,
  'f', post_filename,
  'y', post_reply,
  'd', post_delete,
  'v', post_visit,
  'x', post_cross,		/* �b post/mbox �����O�p�g x ��ݪO�A�j�g X ��ϥΪ� */
  'X', post_forward,
  't', post_tag,
  'E', post_edit,
  'T', post_title,
  'm', post_mark,
  '_', post_bottom,
  'D', post_rangedel,
  'S', post_rss,
#ifdef HAVE_SCORE
  '%', post_score,
#endif

  'w', post_write,

  'b', post_memo,
  'c', post_copy,
  'g', gem_gather,

  Ctrl('P'), post_add,
  Ctrl('D'), post_prune,
  Ctrl('Q'), xo_uquery,
  Ctrl('O'), xo_usetup,
  Ctrl('e'), post_noscore,
  Ctrl('f'), XoBM_add_pal,
  'O', XoBM_Refuse_pal,
  Ctrl('g'), post_viewpal,
  Ctrl('b'), post_showbm,
  Ctrl('S'), post_state,
#ifdef HAVE_REFUSEMARK
  'L', post_refuse,
  'l', post_friend,
#endif

#ifdef HAVE_LABELMARK
  'n', post_label,
  Ctrl('N'), post_delabel,
#endif

  'B' | XO_DL, (void *) "bin/manage.so:post_manage",
  'R' | XO_DL, (void *) "bin/vote.so:vote_result",
  'V' | XO_DL, (void *) "bin/vote.so:XoVote",

#ifdef HAVE_TERMINATOR
  Ctrl('X') | XO_DL, (void *) "bin/manage.so:post_terminator",
#endif

  '/', XOXpost_search_all,  /* smiler.070201: �j�M�\���X */

  //'~' | XO_DL, (int *)  "bin/dictd.so:main_dictd",
  //'~', XoXselect,		/* itoc.001220: �j�M�@��/���D */
  //'S', XoXsearch,		/* itoc.001220: �j�M�ۦP���D�峹 */
  //'a', XoXauthor,		/* itoc.001220: �j�M�@�� */
  //'/', XoXtitle,		/* itoc.001220: �j�M���D */
  //'f', XoXfull,			/* itoc.030608: ����j�M */
  //'G', XoXmark,			/* itoc.010325: �j�M mark �峹 */
  //'K', XoXlocal,		/* itoc.010822: �j�M���a�峹 */

#ifdef HAVE_XYNEWS
  'u', XoNews,			/* itoc.010822: �s�D�\Ū�Ҧ� */
#endif

  'h', post_help
};


KeyFunc xpost_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, post_item_bar,         /* verit.20030129 : �аȥ��N����b�Ĥ@�� */
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
  'o', post_noforward,
  'c', post_copy,
  'g', gem_gather,
  'm', post_mark,
  'd', post_delete,		/* Thor.980911: ��K�O�D */
  'E', post_edit,		/* itoc.010716: ���� XPOST ���i�H�s����D�B�峹�A�[�K */
  'T', post_title,
#ifdef HAVE_SCORE
  '%', post_score,
#endif
  'w', post_write,
#ifdef HAVE_REFUSEMARK
  'L', post_refuse,
#endif
#ifdef HAVE_LABELMARK
  'n', post_label,
#endif

    '/', XOXpost_search_all,  /* smiler.070201: �j�M�\���X */
//  '~', XoXselect,
//  'S', XoXsearch,
//  'a', XoXauthor,
//  '/', XoXtitle,
//  'f', XoXfull,
//  'G', XoXmark,
//  'K', XoXlocal,

  Ctrl('P'), post_add,
  Ctrl('D'), post_prune,
  Ctrl('Q'), xo_uquery,
  Ctrl('O'), xo_usetup,
  Ctrl('e'), post_noscore,

  'h', post_help		/* itoc.030511: �@�ΧY�i */
};


#ifdef HAVE_XYNEWS
KeyFunc news_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, post_item_bar,         /* verit.20030129 : �аȥ��N����b�Ĥ@�� */
#endif
  XO_INIT, news_init,
  XO_LOAD, news_load,
  XO_HEAD, news_head,
  XO_BODY, post_body,

  'r', XoXsearch,

  'h', post_help		/* itoc.030511: �@�ΧY�i */
};
#endif	/* HAVE_XYNEWS */
