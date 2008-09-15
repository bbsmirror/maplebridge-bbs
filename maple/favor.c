/*-------------------------------------------------------*/
/* favorite.c	( NTHU CS MapleBBS Ver 3.10 )		 */
/*-------------------------------------------------------*/
/* target : �ڪ��̷R					 */
/* create : 00/06/16					 */
/* update :   /  /  					 */
/* author : weichung.bbs@bbs.ntit.edu.tw		 */
/*-------------------------------------------------------*/


#include "bbs.h"


#ifdef MY_FAVORITE

static int mf_add();
static int mf_paste();
static int mf_load();
static void XoMF();


extern XZ xz[];
extern char xo_pool[];
extern char brd_bits[];		/* itoc.010821: �P�_�O�_���\Ū�ݪO���v�� */
extern BCACHE *bshm;
extern int class_flag;

#ifndef ENHANCED_VISIT
extern time_t brd_visit[];
#endif

#ifdef AUTO_JUMPBRD
static int mf_jumpnext = 0;     /* itoc.020615: �O�_���h�U�@�ӥ�Ū�O 1:�n 0:���n */
#endif


static MF mftmp;		/* for copy & paste */

static int mf_depth;


void
mf_fpath(fpath, userid, fname)
  char *fpath;
  char *userid;
  char *fname;
{
  char buf[64];

  sprintf(buf, "MF/%s", fname);
  usr_fpath(fpath, userid, buf);
}


static void
mf_item(num, mf)
  int num;
  MF *mf;
{
  char folder[64];
  int mftype, brdpost, bno, label;

  mftype = mf->mftype;
  brdpost = cuser.ufo & UFO_BRDPOST;
  label = mftype & MF_LABEL;

  if (mftype & MF_FOLDER)
  {
    if (brdpost)
    {
      mf_fpath(folder, cuser.userid, mf->xname);
      num = rec_num(folder, sizeof(MF));
    }
    prints("%6d%c  %s %s\n", num, mftype & MF_MARK ? ')' : label ? 'T' : ' ', "��", mf->title);
  }
  else if (mftype & MF_BOARD)
  {
    if ((bno = brd_bno(mf->xname)) >= 0)
      class_item(num, bno, brdpost, 1, label);
    else
      /* itoc.010821: �������ݪO�A�� user �ۤv�M���A�p�� user �~���D���ǬݪO�Q��F */
      prints("       %c \033[36m<%s �w��W�γQ�R���A�бN�����|�R��>\033[m\n", label ? 'T' : ' ', mf->xname);
  }
  else if (mftype & MF_GEM)
  {
    prints("%6d%c  %s %s\n",
      brdpost ? 0 : num, 
      mftype & MF_MARK ? ')' : label ? 'T' : ' ', "��", mf->title);
  }
  else  if (mftype & MF_LINE)		/* qazq.040721: ���j�u */
  {
    prints("%6d%c  %s\n", 
      brdpost ? 0 : num, label ? 'T' : ' ', mf->title);
  }
  else /* if (mftype & MF_CLASS) */	/* LHD.051007: �����s�� */
  {
    char cname[BNLEN + 2];

    sprintf(cname, "%s/", mf->xname);
    prints("%6d%c  %-13.13s\033[1;3%dm%-5.5s\033[m%s\n",
      num, label ? 'T' : ' ', cname, mf->class[3] & 7, mf->class, mf->title);
  }
}


#ifdef HAVE_LIGHTBAR
static int
mf_item_bar(xo, mode)
  XO *xo;
  int mode;
{
  int num;
  MF *mf;
  int mftype, brdpost, invalid, label;
                                                                                
  num = xo->pos + 1;
  mf = (MF *) xo_pool + xo->pos - xo->top;
  mftype = mf->mftype;
  brdpost = cuser.ufo & UFO_BRDPOST;
  label = mftype & MF_LABEL;
                                                                                
  if (mftype & MF_FOLDER)
  {
    if (brdpost)
    {
      struct stat st;
      char fpath[64];
                                                                                
      mf_fpath(fpath, cuser.userid, mf->xname);
      stat(fpath, &st);
      num = st.st_size / sizeof(MF);
    }
    prints("%s%6d%c  %s %-66.54s\033[m", mode ? USR_COLORBAR_BRD : "",
      num, mftype & MF_MARK ? ')' : label ? 'T' : ' ', "��", mf->title);
  }
  else if (mftype & MF_BOARD)
  {
    BRD *bhead, *btail;
    int chn;
    int pbno;
                                                                                
    chn = 0;
    invalid = 1;
    bhead = bshm->bcache;
    btail = bhead + bshm->number;
                                                                                
    do
    {
      if (!strcmp(mf->xname, bhead->brdname))
      {
	pbno = bshm->mantime[chn];
        if (mode)
          class_item_bar(bhead, num, chn, brdpost, pbno, 1, label);
        else
          class_item(num, chn, brdpost, 1, label);
        invalid = 0;
        break;
      }
      chn++;
    } while (++bhead < btail);
                                                                                
    if (invalid)        /* itoc.010821: �Q�媺�ݪO�n�t�~�L */
    {
      prints("%s       %c \033[36m%-13s%-56s\033[m", mode ?
        USR_COLORBAR_BRD : "", label ? 'T' : ' ', mf->xname, "<�w��W�γQ�R���A�бN�����|�R��>");
    }  /* ���צn���� ���ܦۤv��F= =*/
  }
  else if (mftype & MF_GEM)
  {
    prints("%s%6d%c  %s %-66.54s\033[m",
      mode ? USR_COLORBAR_BRD : "",
      brdpost ? 0 : num,
      mftype & MF_MARK ? ')' : label ? 'T' : ' ', "��", mf->title);
  }
  else  if (mftype & MF_LINE)		/* qazq.040721: ���j�u */
  {
    prints("%s%6d%c  %-69.54s\033[m",
      mode ? USR_COLORBAR_BRD : "",
      brdpost ? 0 : num,
      label ? 'T' : ' ', mf->title);
  }
  else /* if (mftype & MF_CLASS) */	/* LHD.051007: �����s�� */
  {
    char cname[BNLEN + 2];

    sprintf(cname, "%s/", mf->xname);
    prints("%s%6d%c  %-13.13s\033[1;3%dm%-5.5s\033[m%s%-51s\033[m",
      mode ? USR_COLORBAR_BRD : "",num, label ? 'T' : ' ', cname, mf->class[3] & 7,
      mf->class, mode ? USR_COLORBAR_BRD : "", mf->title);
  }
  return XO_NONE;
}
#endif


static int
mf_body(xo)
  XO *xo;
{
  MF *mf;
  int max, num, tail;
#ifdef AUTO_JUMPBRD
  int nextpos;
  static int originpos = -1;
#endif

  max = xo->max;
  if (max <= 0)
  {
    max = vans("�ڪ��̷R (A)�s�W (P)�K�W (Q)���} [Q] ");
    switch (max)
    {
    case 'a':

      max = mf_add(xo);
      if (xo->max > 0)
	return max;
      break;

    case 'p':

      mf_paste(xo);
      return mf_load(xo);
    }
 
    return XO_QUIT;
  }

  num = xo->top;
  tail = num + XO_TALL;
  if (max > tail)
    max = tail;

#ifdef AUTO_JUMPBRD
  nextpos = 0;

  /* itoc.020615: �j�M�U�@�ӥ�Ū�ݪO */
  if (mf_jumpnext)
  {
    BRD *bcache;

    tail = xo->pos;	/* �ɥ� tail */
    if (originpos < 0)	/* �b�h�U���j�M��Ū�ݪO�e�A�O���@�}�l��ЩҦb */
      originpos = tail;
    mf = (MF *) xo_pool + tail - num;
    bcache = bshm->bcache;

    while (tail < max)	/* �u��䥻��������Ū�ݪO�A�]���U���٨S���J */
    {
      if (mf->mftype & MF_BOARD)
      {
	int chn;
	BRD *brd;

	chn = brd_bno(mf->xname);
	brd = bcache + chn;

#ifdef ENHANCED_VISIT
	/* itoc.010407: ��γ̫�@�g�wŪ/��Ū�ӧP�_ */
	brh_get(brd->bstamp, chn);
	if (brh_unread(brd->blast))
#else
	if (brd->blast > brd_visit[chn])
#endif
	{
	  nextpos = tail;
	  mf_jumpnext = 0;
	  originpos = -1;
	  break;
	}
      }
      tail++;
      mf++;
    }

    if (mf_jumpnext)	/* �p�G�b�����S����쥼Ū�ݪO */
    {
      if (max < rec_num(xo->dir, sizeof(MF)))	/* �A�h�U���� */
	return num + XO_TALL + XO_MOVE;
 
      /* �w�g�O�̫�@���F�٬O�䤣�쥼Ū�ݪO */
      mf_jumpnext = 0;
      tail = originpos;
      originpos = -1;
      if (tail < num)		/* �^���Ө��� */
	return tail + XO_MOVE;
    }
  }
#endif

  mf = (MF *) xo_pool;

  move(3, 0);
  do
  {
    mf_item(++num, mf++);
  } while (num < max);
  clrtobot();

#ifdef AUTO_JUMPBRD
  /* itoc.020615: �U�@�ӥ�Ū�O�b�����A�n���в��L�h */
  outf(FEETER_MF);
  return nextpos ? nextpos + XO_MOVE : XO_NONE;
#else
  /* return XO_NONE; */
  return XO_FOOT;	/* itoc.010403: �� b_lines ��W feeter */
#endif
}


static int
mf_head(xo)
  XO *xo;
{
  vs_head("�ڪ��̷R", str_site);
  prints(NECKER_MF, 
    class_flag & UFO_BRDPOST ? "�`��" : "�s��", 
    d_cols >> 1, "", d_cols - (d_cols >> 1), "");
  return mf_body(xo);
}


static int
mf_init(xo)
  XO *xo;
{
  xo_load(xo, sizeof(MF));
  return mf_head(xo);
}


static int
mf_load(xo)
  XO *xo;
{
  xo_load(xo, sizeof(MF));
  return mf_body(xo);
}


static int
mf_stamp(mf)
  MF *mf;
{
  char fpath[64];
  int fd;

  do
  {
    mf->xname[0] = 'F';
    archiv32(mf->chrono, mf->xname + 1);

    mf_fpath(fpath, cuser.userid, mf->xname);

    if ((fd = open(fpath, O_WRONLY | O_CREAT | O_EXCL, 0600)) >= 0)
      close(fd);
  } while (fd < 0 && mf->chrono++);

  return fd;
}


static int
mf_add(xo)
  XO *xo;
{
  MF mf;
  int ans;

  ans = vans("�s�W (B)�ݪO���| (F)���v (G)��ذϱ��| (L)���j�u [Q] ");

  if (ans != 'b' && ans != 'f' && ans != 'g' && ans != 'l')
    return XO_FOOT;

  time(&mf.chrono);

  if (ans == 'b' || ans == 'g')
  {
    BRD *brd;
    char bname[BNLEN + 1];

    if (!(brd = ask_board(bname, BRD_R_BIT, NULL)))
      return mf_head(xo);

    mf.mftype = (ans == 'b') ? MF_BOARD : MF_GEM;
    strcpy(mf.xname, brd->brdname);
    if (ans == 'g')
      sprintf(mf.title, "%s �O ��ذϱ��|", mf.xname);
  }
  else /* if (ans == 'f' || ans == 'l') */
  {
    if (!vget(b_lines, 0, "���D�G", mf.title, BTLEN + 1, DOECHO))
      return XO_FOOT;

    mf.mftype = (ans == 'f') ? MF_FOLDER : MF_LINE;
    if (ans == 'f')
    {
      if (mf_stamp(&mf) < 0)
	return XO_FOOT;
    }
  }

  ans = vans("�s���m A)�s�W I)���J N)�U�@�� Q)���} [A] ");
  switch (ans)
  {
  case 'q':  
    break;

  case 'i':
  case 'n':

    rec_ins(xo->dir, &mf, sizeof(MF), xo->pos + (ans == 'n'), 1);
    break;

  default:

    rec_add(xo->dir, &mf, sizeof(MF));
    break;
  }

  return mf_init(xo);
}


static void
mf_do_delete(folder)
  char *folder;
{
  MF mf;
  char fpath[64];
  FILE *fp;

  if (!(fp = fopen(folder, "r")))
    return;

  while (fread(&mf, sizeof(MF), 1, fp) == 1)
  {
    if (mf.mftype & MF_FOLDER)
    {
      mf_fpath(fpath, cuser.userid, mf.xname);
      mf_do_delete(fpath);
    }
  }

  fclose(fp);
  unlink(folder);
}


static int
mf_delete(xo)
  XO *xo;
{
  MF *mf;
  int mftype;
  char fpath[64];

  mf = (MF *) xo_pool + (xo->pos - xo->top);
  mftype = mf->mftype;

  if (mftype & MF_MARK)
    return XO_NONE;

  if (vans(msg_del_ny) == 'y')
  {
    if (mftype & MF_FOLDER)
    {
      mf_fpath(fpath, cuser.userid, mf->xname);
      mf_do_delete(fpath);
    }
    if (!rec_del(xo->dir, sizeof(MF), xo->pos, NULL))
      return mf_load(xo);
  }

  return XO_FOOT;
}


static void
delmf(xo, mf)
  XO *xo;
  MF *mf;
{
  if (mf->mftype & MF_FOLDER)
  {
    char fpath[64];

    mf_fpath(fpath, cuser.userid, mf->xname);
    mf_do_delete(fpath);
  }
}


static int
mf_rangedel(xo)		/* amaki.030910: ���ѧڪ��̷R�Ϭq�R�� */
  XO *xo;
{
  return xo_rangedel(xo, sizeof(MF), NULL, delmf);
}


static int
mf_title(xo)
  XO *xo;
{
  MF *mf, xmf;

  mf = (MF *) xo_pool + (xo->pos - xo->top);
  xmf = *mf;

  if (!(mf->mftype & (MF_FOLDER | MF_LINE)))
    return XO_NONE;

  vget(b_lines, 0, "���D�G", xmf.title, BTLEN + 1, GCARRY);

  if (memcmp(mf, &xmf, sizeof(MF)) && vans(msg_sure_ny) == 'y')
  {
    int num;

    *mf = xmf;
    num = xo->pos;
    rec_put(xo->dir, mf, sizeof(MF), xo->pos, NULL);
    num++;
    move(num - xo->top + 2, 0);
    mf_item(num, mf);
  }

  return XO_FOOT;
}


static int
mf_move(xo)
  XO *xo;
{
  MF *mf;
  char *dir, buf[40];
  int pos, newOrder;

  pos = xo->pos;
  mf = (MF *) xo_pool + (pos - xo->top);

  sprintf(buf, "�п�J�� %d �ﶵ���s��m�G", pos + 1);
  if (!vget(b_lines, 0, buf, buf, 5, DOECHO))
    return XO_FOOT;

  newOrder = atoi(buf) - 1;
  if (newOrder < 0)
    newOrder = 0;
  else if (newOrder >= xo->max)
    newOrder = xo->max - 1;

  if (newOrder != pos)
  {
    dir = xo->dir;
    if (!rec_del(dir, sizeof(MF), pos, NULL))
    {
      rec_ins(dir, mf, sizeof(MF), newOrder, 1);
      xo->pos = newOrder;
      return mf_load(xo);
    }
  }

  return XO_FOOT;
}


static int
mf_mark(xo)
  XO *xo;
{
  MF *mf;

  mf = (MF *) xo_pool + (xo->pos - xo->top);

  if (mf->mftype & MF_FOLDER)
  {
    int num;

    mf->mftype ^= MF_MARK;
    num = xo->pos;
    rec_put(xo->dir, mf, sizeof(MF), num, NULL);
    num++;
    move(num - xo->top + 2, 0);
    mf_item(num, mf);
  }

  return XO_NONE;
}


static int
mf_label(xo)
  XO *xo;
{
  MF *mf;
  int pos, cur, type;

  pos = xo->pos;
  cur = pos - xo->top;
  mf = (MF *) xo_pool + cur;
  type = mf->mftype;

  if (type & MF_MARK)	/* mark �����v�����ಾ�� */
    return XO_NONE;

  mf->mftype = type ^ MF_LABEL;
  rec_put(xo->dir, mf, sizeof(MF), pos, NULL);

  move(3 + cur, 0);
  mf_item(pos, mf);

  return (pos + 1 == xo->max ? 0 : pos + 1) + XO_MOVE;   /* ���ܤU�@�� */
}


static int
mf_has_label(src, depth)
  char *src;
  int depth;
{
  static int has_label;

  int fd;
  char fpath[64];
  MF fmf;

  if (!depth)
    has_label = 0;
  else
  {
    if (has_label)		/* �b�Y�@�ӻ��j���䨧�?MF_LABEL �N����j�M */
      return 1;
  }

  mf_fpath(fpath, cuser.userid, src);
  if ((fd = open(fpath, O_RDONLY)) >= 0)
  {
    while (read(fd, &fmf, sizeof(MF)) == sizeof(MF))
    {  
      if (fmf.mftype & MF_LABEL)
      {
	has_label = 1;
	break;
      }
      if (fmf.mftype & MF_FOLDER)
	mf_has_label(fmf.xname, 1);
    }
    close(fd);
  }

  return has_label;
}


static int
mf_do_clear_label(src, curDir)
  char *src, *curDir;
{
  int fsize;
  char fpath[64];
  MF mf, *data, *head, *tail;

  if (data = (MF *) f_img(src, &fsize))
  {
    head = data;
    tail = data + (fsize / sizeof(MF));
    do
    {
      if (head->mftype & MF_FOLDER)
      {
        mf_fpath(fpath, cuser.userid, head->xname);
        mf_do_clear_label(fpath, curDir);
      }

      if ((!curDir || strcmp(curDir, src)) && (head->mftype & MF_LABEL))
      {
        memcpy(&mf, head, sizeof(MF));
        mf.mftype &= ~MF_LABEL;
	rec_put(src, &mf, sizeof(MF), head - data, NULL);
      }
    } while (++head < tail);

    free(data);
  }
}


static int
mf_clear_label(xo)
  XO *xo;
{
  char folder[64];

  if (!mf_has_label(FN_MF, 0))
    return XO_NONE;

  if (vans("�n�M���Ҧ����аO�ܡH[y/N]") != 'y')
    return XO_FOOT;

  mf_fpath(folder, cuser.userid, FN_MF);
  mf_do_clear_label(folder, NULL);

  return mf_load(xo);
}


static int
mf_browse(xo)
  XO *xo;
{
  int type, bno;
  char *xname, fpath[64];
  BRD *brd;
  MF *mf;

  mf = (MF *) xo_pool + (xo->pos - xo->top);
  type = mf->mftype;
  xname = mf->xname;

  if (type & MF_BOARD)		/* �ݪO���| */
  {
    /* itoc.010726: �Y�O�ݪO�w�g�Q����v���S���F�A�h�n�������| */
    if ((bno = brd_bno(xname)) < 0 || !(brd_bits[bno] & BRD_R_BIT))
    {
      //rec_del(xo->dir, sizeof(MF), xo->pos, NULL);
      //vmsg("���ݪO�w�Q�R���αz�S���v���\\Ū���ݪO�A�t�αN�۰ʲ������|");
	  vmsg("���ݪO�w�Q�R���αz�S���v���\\Ū���ݪO !!");
      return mf_load(xo);
    }

    brd = bshm->bcache + bno;
    XoPost(bno);
    xover(XZ_POST);
#ifndef ENHANCED_VISIT
    time(&brd_visit[bno]);
#endif

#ifdef AUTO_JUMPBRD
    if (cuser.ufo & UFO_JUMPBRD)
      mf_jumpnext = 1;	/* itoc.010910: �u���b���}�ݪO�^��ݪO�C��ɤ~�ݭn���h�U�@�ӥ�Ū�ݪO */
#endif

    return mf_init(xo);
  }
  else if (type & MF_GEM)	/* ��ذϱ��| */
  {
    /* itoc.010726: �Y�O�ݪO�w�g�Q����v���S���F�A�h�n�������| */
    if ((type = gem_link(xname)) < 0)
    {
      //rec_del(xo->dir, sizeof(MF), xo->pos, NULL);
      //vmsg("���ݪO�w�Q�R���αz�S���v���\\Ū���ݪO�A�t�αN�۰ʲ������|");
	  vmsg("���ݪO�w�Q�R���αz�S���v���\\Ū���ݪO !!");
      return mf_load(xo);
    }

    gem_fpath(fpath, xname, fn_dir);
    XoGem(fpath, "��ذ�", type);
    return mf_init(xo);
  }
  else if (type & MF_FOLDER)	/* ���v */
  {
    mf_depth++;
    mf_fpath(fpath, cuser.userid, xname);
    XoMF(fpath);
    mf_depth--;
    return mf_load(xo);
  }
  else if (type & MF_CLASS)	/* �����s�� */
  {
    if (!MFclass_browse(xname))
    {
      rec_del(xo->dir, sizeof(MF), xo->pos, NULL);
      vmsg("�������s�դw�Q�R���A�t�αN�۰ʲ������|");
      return mf_load(xo);
    }
    return mf_init(xo);
  }

  return XO_NONE;
}


static int
mf_copy(xo)
  XO *xo;
{
  MF *mf;

  mf = (MF *) xo_pool + (xo->pos - xo->top);

  memcpy(&mftmp, mf, sizeof(MF));
  zmsg("���������A���O���v������Ƥ��|�Q����");

  return XO_FOOT;
}


static void
mf_do_paste(dstDir, mf, pos)
  char *dstDir;		/* destination folder */
  MF *mf;		/* source hdr */
  int pos;		/* -1: ���[�b�̫�  >=0: �K�W����m */
{
  int xmode, fsize;
  char folder[64], fpath[64];
  MF fmf, *data, *head, *tail;

  xmode = mf->mftype;
  memcpy(&fmf, mf, sizeof(MF));

  if (xmode & MF_FOLDER)	/* ���v/���� */
  {
    /* �b�ƻs/�K�W��@���ܦ����v�A�]�������O�����M�ίS��γ~�� */
    time(&fmf.chrono);			/* �y�@�ӷs�� chrono */
    if ((fsize = mf_stamp(&fmf)) < 0)
      return;
    close(fsize);
  }
  fmf.mftype &= ~MF_LABEL;	/* �аO���ƻs */

  if (pos < 0)
    rec_add(dstDir, &fmf, sizeof(MF));
  else
    rec_ins(dstDir, &fmf, sizeof(MF), pos, 1);

  if (xmode & MF_FOLDER)	/* ���v/���� */
  {
    /* �إߧ��ۤv�o�Ө��v�H��A�A recursive �a�@�h�@�h�ؿ��i�h�@�g�@�g�t�s�s�� */
    mf_fpath(folder, cuser.userid, mf->xname);
    mf_fpath(fpath, cuser.userid, fmf.xname);
    if (data = (MF *) f_img(folder, &fsize))
    {
      head = data;
      tail = data + (fsize / sizeof(MF));
      do
      {
	mf_do_paste(fpath, head, -1);
      } while (++head < tail);

      free(data);
    }
  }
}


static int
mf_invalid_loop(dst, src, depth)
  char *dst, *src;
  int depth;
{
  static int valid;

  int fd;
  char fpath[64];
  MF fmf;

  if (!depth)
  {
    if (!strcmp(dst, src))	/* ��ۤv����ۤv�̭� */
      return 1;
    valid = 0;
  }
  else
  {
    if (valid)		/* �b�Y�@�ӻ��j�����D�k�ҾڴN����j�Ҥu�@ */
      return 1;
  }

  mf_fpath(fpath, cuser.userid, src);
  if ((fd = open(fpath, O_RDONLY)) >= 0)
  {
    while (read(fd, &fmf, sizeof(MF)) == sizeof(MF))
    {  
      if (fmf.mftype & MF_FOLDER)	/* MF_FOLDER �H�~���|�y���L�a�j�� */
      {
	if (!strcmp(dst, fmf.xname))
	{
	  valid = 1;
	  return 1;
	}

	/* recursive �a�@�h�@�h���v�i�h�ˬd�O�_�|�y���L�a�j�� */
	mf_invalid_loop(dst, fmf.xname, 1);
      }
    }
    close(fd);
  }

  return valid;
}


static int
mf_paste(xo)
  XO *xo;
{
  MF mf;
  int ans;

  if (!mftmp.chrono)
  {
    zmsg("�Х����� copy �R�O��A paste");
    return XO_FOOT;
  }

  memcpy(&mf, &mftmp, sizeof(MF));
  time(&mf.chrono);			/* �y�@�ӷs�� chrono */

  /* itoc.010726.����: �Y�O MF_FOLDER�A�h�����ɦW�A�K�W�A�@�Ө��v�@���ɮ� */
  /* itoc.010726.����: ���v�ƻs�K�W�A�̭����F��èS���K�W�A�i�o�g recursive ���{�� :p */
  if (mf.mftype & MF_FOLDER)
  {
    if (mf_invalid_loop(xo->dir + 10 + strlen(cuser.userid), mftmp.xname, 0))	/* strlen("usr/u/userid/MF/") */
    {
      vmsg("�y���j�骺���v�N�L�k����");
      return XO_FOOT;
    }
  }

  ans = vans("�s���m A)�s�W I)���J N)�U�@�� Q)���} [A] ");
  switch (ans)
  {
  case 'q':
    return XO_FOOT;

  case 'i':
  case 'n':

    if (mf.mftype & MF_FOLDER)
      mf_do_paste(xo->dir, &mftmp, xo->pos + (ans == 'n'));
    else
      rec_ins(xo->dir, &mf, sizeof(MF), xo->pos + (ans == 'n'), 1);
    break;

  default:

    if (mf.mftype & MF_FOLDER)
      mf_do_paste(xo->dir, &mftmp, -1);
    else
      rec_add(xo->dir, &mf, sizeof(MF));
    break;
  }

  return mf_load(xo);
}


static void
mf_do_append(dst, src)
  char *dst, *src;
{
  int fsize, offset;
  char fpath[64];
  MF mf, *data, *head, *tail;

  if (data = (MF *) f_img(src, &fsize))
  {
    offset = 0;
    head = data;
    tail = data + (fsize / sizeof(MF));
    do
    {
      if (head->mftype & MF_FOLDER)
      {
	mf_fpath(fpath, cuser.userid, head->xname);
	mf_do_append(dst, fpath);

	if (mf_invalid_loop(dst + 10 + strlen(cuser.userid), head->xname, 0))	/* strlen("usr/u/userid/MF/") */
	  continue;
      }

      if (strcmp(dst, src) && (head->mftype & MF_LABEL))	/* �����\�P�@�Ө��v�Uappend, �Ч�� move */
      {
	memcpy(&mf, head, sizeof(MF));
	mf.mftype &= ~MF_LABEL;
	rec_add(dst, &mf, sizeof(MF));
	rec_del(src, sizeof(MF), head - data - offset, NULL);
	++offset;
      }
    } while (++head < tail);

    free(data);
  }

}


static int
mf_append(xo)	/* �⦳ MF_LABEL �аO�������e�ؿ� */
  XO *xo;
{
  char folder[64];

  if (!mf_has_label(FN_MF, 0))
    return XO_NONE;

  if (vans("�n�N�Ҧ��аO����{�b�o�Ө��v�ܡH[y/N]") != 'y')
    return XO_FOOT;

  mf_fpath(folder, cuser.userid, FN_MF);

  mf_do_append(xo->dir, folder);
  mf_do_clear_label(folder, xo->dir);

  return mf_load(xo);
}


static void
mf_do_del_label(src)
  char *src;
{
  int fsize, offset;
  char fpath[64];
  MF *data, *head, *tail;

  if (data = (MF *) f_img(src, &fsize))
  {
    offset = 0;
    head = data;
    tail = data + (fsize / sizeof(MF));
    do
    {
      if (head->mftype & MF_LABEL)
      {
	if (head->mftype & MF_FOLDER)
	{
	  mf_fpath(fpath, cuser.userid, head->xname);
	  mf_do_delete(fpath);
	}
	rec_del(src, sizeof(MF), head - data - offset, NULL);

	++offset;
	continue;
      }

      if (head->mftype & MF_FOLDER)
      {
        mf_fpath(fpath, cuser.userid, head->xname);
        mf_do_del_label(fpath);
      }
    } while (++head < tail);

    free(data);
  }
}


static int
mf_del_label(xo)
  XO *xo;
{
  char folder[64];

  if (!mf_has_label(FN_MF, 0))
    return XO_NONE;

  if (vans("�n�R���Ҧ��аO�ܡH[y/N]") != 'y')
    return XO_FOOT;

  mf_fpath(folder, cuser.userid, FN_MF);
  mf_do_del_label(folder);

  return mf_load(xo);
}


static int
mf_namemode(xo)
  XO *xo;
{
  cuser.ufo ^= UFO_BRDPOST;
  cutmp->ufo = cuser.ufo;
  class_flag ^= UFO_BRDPOST;	/* �M class �P�B */
  return mf_head(xo);
}


static int
mf_edit(xo)		/* itoc.010110: �ڪ��̷R���ݪO�ק� */
  XO *xo;
{
  MF *mf;

  mf = (MF *) xo_pool + (xo->pos - xo->top);

  if ((mf->mftype & MF_BOARD) && (HAS_PERM(PERM_ALLBOARD | PERM_BM)))
  {
    int bno;

    bno = brd_bno(mf->xname);
    if (bno >= 0)
    {
      if (!HAS_PERM(PERM_ALLBOARD))
	brd_title(bno);
      else
	brd_edit(bno);  
      return mf_init(xo);
    }
  }
  return XO_NONE;
}


static int
mf_switch(xo)
  XO *xo;
{
  Select();
  return mf_init(xo);
}


static int
mf_visit(xo)		/* itoc.010402: �ݪO�C��]�w�ݪO�wŪ */
  XO *xo;
{
  int bno;
  MF *mf;

  mf = (MF *) xo_pool + (xo->pos - xo->top);
  bno = brd_bno(mf->xname);

  if (bno >= 0)		/* itoc.010110: ���Ӥ��| < 0 ? */
  {
    BRD *brd;
    brd = bshm->bcache + bno;
    brh_get(brd->bstamp, bno);
    brh_visit(0);
#ifndef ENHANCED_VISIT
    time(&brd_visit[bno]);
#endif
  }
  return mf_body(xo);
}


static int
mf_unvisit(xo)		/* itoc.010402: �ݪO�C��]�w�ݪO��Ū */
  XO *xo;
{
  int bno;
  MF *mf;

  mf = (MF *) xo_pool + (xo->pos - xo->top);
  bno = brd_bno(mf->xname);

  if (bno >= 0)		/* itoc.010110: ���Ӥ��| < 0 ? */
  {
    BRD *brd;
    brd = bshm->bcache + bno;
    brh_get(brd->bstamp, bno);
    brh_visit(1);
#ifndef ENHANCED_VISIT
    brd_visit[bno] = 0;	/* itoc.010402: �̪��s���ɶ��k�s�A�ϬݪO�C����ܥ�Ū */
#endif
  }
  return mf_body(xo);
}


static int
mf_nextunread(xo)
  XO *xo;
{
  int max, pos, bno;
  MF *mf;

  max = xo->max;
  pos = xo->pos;
  mf = (MF *) xo_pool + (xo->pos - xo->top);

  while (++pos < max)
  {
    bno = brd_bno((++mf)->xname);
    if (bno >= 0 && !(brd_bits[bno] & BRD_Z_BIT))	/* ���L������ zap �����ݪO */
    {
      BRD *brd;
      brd = bshm->bcache + bno;

#ifdef ENHANCED_VISIT
      /* itoc.010407: ��γ̫�@�g�wŪ/��Ū�ӧP�_ */
      brh_get(brd->bstamp, bno);

      if (brh_unread(brd->blast))
#else
      if (brd->blast > brd_visit[bno])
#endif
	return pos + XO_MOVE;
    }
  }

  return XO_NONE;
}


static int
mf_help(xo)
  XO *xo;
{
  xo_help("mf");
  return mf_head(xo);
}


static KeyFunc mf_cb[] =
{
#ifdef  HAVE_LIGHTBAR
  XO_ITEM, mf_item_bar,
#endif
  XO_INIT, mf_init,
  XO_LOAD, mf_load,
  XO_HEAD, mf_head,
  XO_BODY, mf_body,

  'r', mf_browse,
  'd', mf_delete,
  'D', mf_rangedel,
  'o', mf_mark,
  't', mf_label,
  'm', mf_move,
  'T', mf_title,
  'E', mf_edit,
  's', mf_switch,
  'c', mf_namemode,
  'v', mf_visit,
  'V', mf_unvisit,
  '`', mf_nextunread,

  Ctrl('P'), mf_add,
  'C', mf_copy,
  'g', mf_copy,
  'p', mf_paste,
  Ctrl('V'), mf_paste,
  Ctrl('A'), mf_append,
  Ctrl('T'), mf_clear_label,
  Ctrl('D'), mf_del_label,

  'h', mf_help
};


static void
XoMF(folder)
  char *folder;
{
  XO *xo, *last;

  /* itoc.060706: �� mf_depth �� 0 �ɡA��ܭn�i FN_MF�A�b mf_main() �w�B�z�L�A
     ���ݭn���s xo_new()�A�p���i�O�d������ xo->pos */

  if (mf_depth)
  {
    last = xz[XZ_MF - XO_ZONE].xo;	/* record */

    xz[XZ_MF - XO_ZONE].xo = xo = xo_new(folder);
    xo->pos = 0;
  }

#ifdef AUTO_JUMPBRD
  if (cuser.ufo & UFO_JUMPBRD)
    mf_jumpnext = 1;	/* itoc.020615: �D�ʸ��h�U�@�ӥ�Ū�ݪO */
#endif
  xover(XZ_MF);

  if (mf_depth)
  {
    free(xo);
    xz[XZ_MF - XO_ZONE].xo = last;	/* restore */
  }
}


int
MyFavorite()
{
  /* �q�D���i�J�ڪ��̷R�Amf_depth �@�w�O 0 */
  XoMF(NULL);

  return 0;
}


void
mf_main()
{
  char fpath[64];
  XO *xo;

  /* itoc.060706.����: �i���ɴN�n��l�ơA�]���ϥΪ̥i��@�W���N every_Z ���h�ڪ��̷R */

  mf_fpath(fpath, cuser.userid, FN_MF);
  xz[XZ_MF - XO_ZONE].xo = xo = xo_new(fpath);
  xz[XZ_MF - XO_ZONE].cb = mf_cb;

  xo->pos = 0;

#ifdef AUTO_JUMPBRD
  if (cuser.ufo & UFO_JUMPBRD)
    mf_jumpnext = 1;	/* itoc.020615: �D�ʸ��h�U�@�ӥ�Ū�ݪO */
#endif
}
#endif				/* MY_FAVORITE */
