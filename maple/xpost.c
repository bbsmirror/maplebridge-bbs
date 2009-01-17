/*-------------------------------------------------------*/
/* xpost.c      ( NTHU CS MapleBBS Ver 2.39 )		 */
/*-------------------------------------------------------*/
/* target : bulletin boards' routines		 	 */
/* create : 95/03/29				 	 */
/* update : 96/04/05				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"


extern XZ xz[];

extern char xo_pool[];


/*------------------------------------------------------------------------
  Thor.980509:
  �s�� �峹�j�M�Ҧ� �i���w�@ keyword, �C�X�Ҧ�keyword�������峹�C��

  �b tmp/ �U�} xpost.{pid} �@�� folder, �t�ؤ@map�}�C, �Χ@�P�� post �@ map
  �O���Ӥ峹�O�b�� post ����B, �p���i�@ mark, gem, edit, title ���\��,
  �B�����}�ɦ^�ܹ����峹�B
  <�H�W�Q�k obsolete...>

  Thor.980510:
  �إߤ峹�Q�צ�, like tin, �N�峹�� index ��J memory ��,
  ���ϥ� thread, �]�� thread�n�� folder ��...

  ������� Mode, Title & post list

  ���Ҽ{����²�ƪ� �W�U�䲾��..

  O->O->O->...
  |  |  |
  o  o  o
  |  |  |

  index�tfield {next, text} ����int, �t�m�]�� int
  �Ĥ@�h sorted by title, ���J�ɥ� binary search
  �B MMAP only , �Ĥ@�h��� # and +

  �����ѥ���Ϭq�R���ʧ@, �קK�V��
-------------------------------------------------------------------------*/

#if 0	/* itoc.060206.���� */

  ��ϥΪ̿�J�j�M�����A�|�i�J XoXpost() �N xo->dir �o�ɮפ��ҰO�����Ҧ� HDR
  �@�@�s���A�M��N�������� HDR �b xo->dir ���ҹ�����m�O���b xpostIndex[]�A
  ���۶i�J xover(XZ_XPOST) �|�I�s xpost_init()�A�A�� xpost_pick() �N xpostIndex[]
  �ҰO������m�q xo->dir �ۨ� xo_pool[]�C

  ��W�[���󰵤G���j�M�ɡA���ɤ��ݭn����� xo->dir �����Ҧ� HDR�A�u�s���O���b
  xpostIndex[] �̭������ǡC�ѩ�G���j�M�ɭn�ݦP�@�� xo->dir�A�ҥH every_Z ��
  �n�T��i�J XZ_XPOST �G���C

  �w�����D�O�G��ϥΪ��٦b XZ_XPOST �̭��ɡA�Y xo->dir �����Ǧ����ʮ� (�Ҧp�R��)�A
  �ӨϥΪ̭n�D xpick_pick() �� (�Ҧp½���B�G���j�M)�A�ѩ� xpostIndex[] �O�����O�b 
  xo->dir ����m�A���ɵ��G�|�X���C

#endif

/* ----------------------------------------------------- */
/* ��C�j�M�D�{��					 */
/* ----------------------------------------------------- */


#ifdef EVERY_Z
#define MSG_XYDENY	"�Х��h�X�ϥ� ^Z �H�e���걵/�s�D�\\��"
extern int z_status;
#endif

extern KeyFunc xpost_cb[];
extern KeyFunc xmbox_cb[];

static int *xpostIndex;		/* Thor: first ypost pos in ypost_xo.key */
static int comebackPos;		/* �O���̫�\Ū���g�峹����m */


static char HintWord[TTLEN + 20];
static char HintAuthor[IDLEN + 20];
static char xs_ch = 0;		/* XSearch ch */

static int ReverseSearch = 0;	/* �����j�M�O�_���ư����� */


static int
XoXpost(xo, hdr, on, off, fchk)		/* Thor: eXtended post : call from post_cb */
  XO *xo;
  HDR *hdr;		/* �j�M������ */
  int on, off;		/* �j�M���d�� (on~off-1) */
  int (*fchk) ();	/* �j�M���禡 */
{
  int *list, fsize, max, locus, count, i;
  char *fimage;
  HDR *head;
  XO *xt;
#ifdef HAVE_XYNEWS
  int returnPos;
#endif

  if (xo->max <= 0)	/* Thor.980911: ����: �H���U�@ */
    return XO_FOOT;

  /* build index according to input condition */

  fimage = f_map(xo->dir, &fsize);

  if (fimage == (char *) -1)
  {
    vmsg("�ثe�L�k�}�ү�����");
    return XO_BODY;
  }

  /* allocate index memory, remember free first */

  /* Thor.990113: �Ȱ�title, author�������S���Hpost */
  max = xpostIndex ?  xo->max : fsize / sizeof(HDR);
  list = (int *) malloc(sizeof(int) * max);

  count = 0;			/* �`�@���X�g�������� */

  if (max > off)
    max = off;

  for (i = on; i < max; i++)
  {
    if (xpostIndex)		/* �W�[����A���j�M�ɡA�u�ݭn��b xpostIndex[] �̭��� */
      locus = xpostIndex[i];
    else			/* ��� xo->dir �����@�� */
      locus = i;

    head = (HDR *) fimage + locus;

#ifdef HAVE_REFUSEMARK
    if (!chkrestrict(head))
      continue;
#endif

    /* check condition */
    if (!(* fchk) (head, hdr))
      continue;

    list[count++] = locus;
  }

  munmap(fimage, fsize);

  ReverseSearch = 0;	/* �C���j�M���N���m */

  if (count <= 0)
  {
    free(list);
    vmsg(MSG_XY_NONE);
    return XO_BODY;
  }

  /* �W�[����A���j�M */
  if (xpostIndex)
  {
    free(xpostIndex);
    xpostIndex = list;

    xo->pos = 0;
    xo->max = count;

    return xpost_init(xo);
  }

  /* �����j�M */
  xpostIndex = list;

  /* build XO for xpost_xo */

  comebackPos = xo->pos;	/* Thor: record pos, future use */
#ifdef HAVE_XYNEWS
  returnPos = comebackPos;
#endif

  xz[XZ_XPOST - XO_ZONE].xo = xt = xo_new(xo->dir);
  xz[XZ_XPOST - XO_ZONE].cb = (xo->dir[0] == 'b') ? xpost_cb : xmbox_cb;
  xt->pos = 0;
  xt->max = count;
  xt->xyz = xo->xyz;
  xt->key = XZ_XPOST;

  xover(XZ_XPOST);

  /* set xo->pos for new location */

#ifdef HAVE_XYNEWS
  if (xz[XZ_NEWS - XO_ZONE].xo)
    xo->pos = returnPos;	/* �q XZ_XPOST �^�� XZ_NEWS ��в��h��Ӫ��a�� */
  else
#endif
    xo->pos = comebackPos;	/* �q XZ_XPOST �^�� XZ_POST ��в��h��Ӫ��a��ΩҾ\Ū�峹���u����m */

  /* free xpost_xo */

  if (xt = xz[XZ_XPOST - XO_ZONE].xo)
  {
    free(xt);
    xz[XZ_XPOST - XO_ZONE].xo = NULL;
  }

  /* free index memory, remember check free pointer */

  if (xpostIndex)
  {
    free(xpostIndex);
    xpostIndex = NULL;
  }

  return XO_INIT;
}


  /* --------------------------------------------------- */
  /* �j�M�@��/���D					 */
  /* --------------------------------------------------- */


static int			/* 0:����������  !=0:�������� */
filter_select(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  char *title;
  char *xname;
  usint str4;

  /* �ɥ� hdr->xid �� strlen(hdr->owner) */

  /* Thor.981109: �S�O�`�N�A���F���C load�Aauthor �O�q�Y match�A���O substr match */
  if (hdr->xid && str_ncmp(head->owner, hdr->owner, hdr->xid))
    return 0;

  if (hdr->title[0])
  {
    title = head->title;
    str4 = STR4(title);
    if (str4 == STR4(STR_REPLY) || str4 == STR4(STR_FORWARD))	/* Thor.980911: ���� Re:/Fw: ���~ */
      title += 4;
    if (!str_sub(title, hdr->title))
      return 0;
  }

  if (hdr->xname[0])
  {
     xname = head->xname;
     if (strcmp(xname, hdr->xname))
       return 0;
  }

  return 1;
}


static int			/* 0:����������  !=0:�������� */
filter_R_select(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  char *title;
  usint str4;

  /* �ɥ� hdr->xid �� strlen(hdr->owner) */

  /* Thor.981109: �S�O�`�N�A���F���C load�Aauthor �O�q�Y match�A���O substr match */
  if (hdr->xid && !str_ncmp(head->owner, hdr->owner, hdr->xid))
    return 0;

  if (hdr->title[0])
  {
    title = head->title;
    str4 = STR4(title);
    if (str4 == STR4(STR_REPLY) || str4 == STR4(STR_FORWARD))	/* Thor.980911: ���� Re:/Fw: ���~ */
      title += 4;
    if (str_sub(title, hdr->title))
      return 0;
  }

  return 1;
}


int
XoXselect(xo)
  XO *xo;
{
  HDR hdr;
  char *key;

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  /* input condition */

  key = hdr.title;
  if (xs_ch == 1)
  {
    xs_ch = 0;
    goto set_owner;
  }
  sprintf(key, "%c", xs_ch ? xs_ch : '\0');
  xs_ch = 0;
  if (vget(b_lines, 0, MSG_XYPOST1, key, 30, GCARRY))
  {
    strcpy(HintWord, key);
    str_lowest(key, key);
  }
  else
  {
set_owner:
    HintWord[0] = '\0';
  }

  key = hdr.owner;
  if (vget(b_lines, 0, MSG_XYPOST2, key, IDLEN + 1, DOECHO))
  {
    strcpy(HintAuthor, key);
    str_lower(key, key);
    hdr.xid = strlen(key);
  }
  else
  {
    HintAuthor[0] = '\0';
    hdr.xid = 0;
  }

  if (!hdr.title[0] && !hdr.xid)
    return XO_BODY;

  hdr.xname[0] = '\0';

  return XoXpost(xo, &hdr, 0, INT_MAX, filter_select);
}


  /* --------------------------------------------------- */
  /* �j�M�@��						 */
  /* --------------------------------------------------- */


int
XoXauthor(xo)
  XO *xo;
{
  HDR hdr;
  char *author;

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  author = hdr.owner;
  if (!ReverseSearch)
  {
    sprintf(author, "%c", xs_ch ? xs_ch : '\0');
    xs_ch = 0;
    if (!vget(b_lines, 0, MSG_XYPOST2, author, IDLEN + 1, GCARRY))
      return XO_BODY;
  }
  else
  {
    if (!vget(b_lines, 0, "[�ư�] �@������r�G", author, IDLEN + 1, DOECHO))
    {
      ReverseSearch = 0;
      return XO_BODY;
    }
  }

  HintWord[0] = '\0';
  if (!ReverseSearch)
    strcpy(HintAuthor, author);
  else
    sprintf(HintAuthor, "\033[1;33m�ư�\033[m %s", author);

  hdr.title[0] = '\0';
  hdr.xname[0] = '\0';
  str_lower(author, author);
  hdr.xid = strlen(author);

  return XoXpost(xo, &hdr, 0, INT_MAX, ReverseSearch ? filter_R_select : filter_select);
}

  /* --------------------------------------------------- */
  /* �j�M�ɦW						 */
  /* --------------------------------------------------- */

static int
XoXxname(xo)
  XO *xo;
{
  HDR hdr;
  char *xname;

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)    /* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  xname = hdr.xname;
  sprintf(xname, "%c", xs_ch ? xs_ch : '\0');
  xs_ch = 0;
  if (!vget(b_lines, 0, "�ɦW����r�G", xname, 9, GCARRY))
    return XO_BODY;

  strcpy(HintWord, xname);
  HintAuthor[0] = '\0';

  hdr.xid = 0;
  hdr.title[0] = '\0';

  return XoXpost(xo, &hdr, 0, INT_MAX, filter_select);
}


  /* --------------------------------------------------- */
  /* �j�M���D						 */
  /* --------------------------------------------------- */


int
XoXtitle(xo)
  XO *xo;
{
  HDR hdr;
  char *title;

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  title = hdr.title;
  if (!ReverseSearch)
  {
    sprintf(title, "%c", xs_ch ? xs_ch : '\0');
    xs_ch = 0;
    if (!vget(b_lines, 0, MSG_XYPOST1, title, 30, GCARRY))
      return XO_BODY;
  }
  else
  {
    if (!vget(b_lines, 0, "[�ư�] ���D����r�G", title, 30, DOECHO))
    {
      ReverseSearch = 0;
      return XO_BODY;
    }
  }

  if (!ReverseSearch)
    strcpy(HintWord, title);
  else
    sprintf(HintWord, "\033[1;33m�ư�\033[m %s", title);
  HintAuthor[0] = '\0';

  str_lowest(title, title);
  hdr.xid = 0;
  hdr.xname[0] = '\0';

  return XoXpost(xo, &hdr, 0, INT_MAX, ReverseSearch ? filter_R_select : filter_select);
}


  /* --------------------------------------------------- */
  /* �j�M�ۦP���D					 */
  /* --------------------------------------------------- */


static int			/* 0:����������  !=0:�������� */
filter_search(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  char *title, buf[TTLEN + 1];
  usint str4;

  title = head->title;
  str4 = STR4(title);
  if (str4 == STR4(STR_REPLY) || str4 == STR4(STR_FORWARD))	/* Thor.980911: ���� Re:/Fw: ���~ */
    title += 4;
  str_lowest(buf, title);
  return !strcmp(buf, hdr->title);
}


int
XoXsearch(xo)
  XO *xo;
{
  HDR hdr, *mhdr;
  char *title;
  usint str4;

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  mhdr = (HDR *) xo_pool + (xo->pos - xo->top);

  title = mhdr->title;
  str4 = STR4(title);
  if (str4 == STR4(STR_REPLY) || str4 == STR4(STR_FORWARD))	/* Thor.980911: ���� Re:/Fw: ���~ */
    title += 4;

  strcpy(HintWord, title);
  HintAuthor[0] = '\0';

  str_lowest(hdr.title, title);

  return XoXpost(xo, &hdr, 0, INT_MAX, filter_search);
}


  /* --------------------------------------------------- */
  /* ����j�M						 */
  /* --------------------------------------------------- */


static char *search_folder;
static int search_fit;		/* >=0:���X�g -1:���_�j�M */
static int search_all;		/* �w�j�M�X�g */

static int			/* 0:����������  !=0:�������� */
filter_full(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  char buf[80], *fimage;
  int rc, fsize;
  struct timeval tv = {0, 10};

  if (search_fit < 0)		/* ���_�j�M */
    return 0;

  if (search_all % 100 == 0)	/* �C 100 �g�~���i�@���i�� */
  {
    sprintf(buf, "�ثe��� \033[1;33m%d / %d\033[m �g�A����j�M��\033[5m...\033[m�����N�䤤�_", 
      search_fit, search_all);
    outz(buf);
    refresh();
  }
  search_all++;

  hdr_fpath(buf, search_folder, head);

  fimage = f_map(buf, &fsize);
  if (fimage == (char *) -1)
    return 0;

  rc = 0;
  if (str_sub(fimage, hdr->title))
  {
    rc = 1;
    search_fit++;
  }

  munmap(fimage, fsize);

  /* �ϥΪ̥i�H���_�j�M */
  fsize = 1;
  if (select(1, (fd_set *) &fsize, NULL, NULL, &tv) > 0)
  {
    vkey();
    search_fit = -1;
  }

  return rc;
}


int
XoXfull(xo)
  XO *xo;
{
  HDR hdr;
  char *key, ans[8];
  int head, tail;

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  /* input condition */

  key = hdr.title;
  sprintf(key, "%c", xs_ch ? xs_ch : '\0');
  xs_ch = 0;
  if (!vget(b_lines, 0, "��������r�G", key, 30, GCARRY))
    return XO_BODY;

  vget(b_lines, 0, "[�]�w�j�M�d��] �_�I�G(Enter)�q�Y�}�l ", ans, 6, DOECHO);
  if ((head = atoi(ans)) <= 0)
    head = 1; 

  vget(b_lines, 44, "���I�G(Enter)���̫� ", ans, 6, DOECHO);
  if ((tail = atoi(ans)) < head)
    tail = INT_MAX;

  head--;

  sprintf(HintWord, "[����j�M] %s", key);
  HintAuthor[0] = '\0';
  str_lowest(key, key);

  search_folder = xo->dir;
  search_fit = 0;
  search_all = 0;

  return XoXpost(xo, &hdr, head, tail, filter_full);
}


  /* --------------------------------------------------- */
  /* �j�M mark						 */
  /* --------------------------------------------------- */


static int			/* 0:����������  !=0:�������� */
filter_mark(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  return (head->xmode & POST_MARKED);
}


static int			/* 0:����������  !=0:�������� */
filter_R_mark(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  return !(head->xmode & POST_MARKED);
}


int
XoXmark(xo)
  XO *xo;
{
#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif
  strcpy(HintWord, ReverseSearch ? "\033[1;33m�Ҧ��L mark �峹\033[m" : "\033[1;33m�Ҧ� mark �峹\033[m");
  HintAuthor[0] = '\0';
  return XoXpost(xo, NULL, 0, INT_MAX, ReverseSearch ? filter_R_mark : filter_mark);
}


  /* --------------------------------------------------- */
  /* �j�M���a						 */
  /* --------------------------------------------------- */


static int			/* 0:����������  !=0:�������� */
filter_local(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  return !(head->xmode & POST_INCOME);
}


static int			/* 0:����������  !=0:�������� */
filter_R_local(head, hdr)
  HDR *head;	/* �ݴ��� */
  HDR *hdr;	/* ���� */
{
  return (head->xmode & POST_INCOME);
}


int
XoXlocal(xo)
  XO *xo;
{
#if 0
  if (currbattr & BRD_NOTRAN)
  {
    vmsg("���O������H�O�A�������O���a�峹");
    return XO_BODY;
  }
#endif

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  strcpy(HintWord, ReverseSearch ? "\033[1;33m�Ҧ���i�峹\033[m" : "\033[1;33m�Ҧ��D��i�峹\033[m");
  HintAuthor[0] = '\0';

  return XoXpost(xo, NULL, 0, INT_MAX, ReverseSearch ? filter_R_local : filter_local);
}


#ifdef HAVE_SCORE
  /* --------------------------------------------------- */
  /* �j�M ����						 */
  /* --------------------------------------------------- */

static int
filter_score(head, hdr)
 HDR *head;	/* �ݴ��� */
 HDR *hdr;	/* ���� */
{
  int score = (head->xmode & POST_SCORE);
  if (!hdr->xmode)
  {
    return hdr->score == 0 ? score :
	hdr->score > 0 ? score && (head->score >= hdr->score) :
	score && (head->score <= hdr->score);
  }
  else
  {
    return hdr->score == 0 ? !score :
	hdr->score > 0 ? score && (head->score <= hdr->score) :
	score && (head->score >= hdr->score);
  }
}


static int
XoXscore(xo)
  XO *xo;
{
  HDR hdr;
  char score[6];

#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)	/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  sprintf(score, "%c", xs_ch ? xs_ch : '\0');
  xs_ch = 0;
  vget(b_lines, 0, "�j�M������ >= + / <= - �h�֪��峹�H (�ϦV�j�M�Ыe�m ! �Ÿ�) ", score, 5, GCARRY);
  if (score[0] == '!')
  {
    hdr.xmode = 1;
    hdr.score = atoi(score + 1);
  }
  else
  {
    hdr.xmode = 0;
    hdr.score = atoi(score);
  }

  HintAuthor[0] = '\0';
  if (!hdr.score)
    strcpy(HintWord, hdr.xmode ? "\033[1;33m�Ҧ����Q�����L�峹\033[m" : "\033[1;33m�Ҧ��Q�����L�峹\033[m");
  else if (hdr.score > 0)
    sprintf(HintWord, "\033[1;33m������%s�󵥩� %d ���峹\033[m", hdr.xmode ? "�p": "�j", hdr.score);
  else  /* (hdr.score < 0) */
    sprintf(HintWord, "\033[1;33m������%s�󵥩� %d ���峹\033[m", hdr.xmode ? "�j" : "�p", hdr.score);

  return XoXpost(xo, &hdr, 0, INT_MAX, filter_score);
}
#endif


#ifdef HAVE_REFUSEMARK
  /* --------------------------------------------------- */
  /* �j�M �[�K/�n�ͤ�					 */
  /* --------------------------------------------------- */

static int
filter_refuse(head, hdr)
 HDR *head;	/* �ݴ��� */
 HDR *hdr;	/* ���� */
{
  return (head->xmode & POST_RESTRICT) && RefusePal_belong(currboard, head);
}


static int
XoXrefuse(xo)
  XO *xo;
{
#ifdef EVERY_Z
  if (z_status && xz[XZ_XPOST - XO_ZONE].xo)    /* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_BODY;
  }
#endif

  strcpy(HintWord, "\033[1;33m�Ҧ��[�K/�n�ͤ�\033[m");
  HintAuthor[0] = '\0';

  return XoXpost(xo, NULL, 0, INT_MAX, filter_refuse);
}
#endif


int
XOXpost_search_all(xo)
  XO *xo;
{
  static char *smenu[] = {"�@�̼��D", "�@��", "���D", "����", "�ɦW", NULL};
  static char *lmenu[] = {"�ۦP���D", "����", "�[�K", "mark", "local", NULL};
  static char *prompt[] = {"���D", "�@��", "���D", "����", "�ɦW", NULL};
  static int plen[] = {30, IDLEN + 1, 30, 30, 9};

  char buf[64];
  int i, ch = 2;

  move(b_lines - 2, 0);
  clrtobot();
  memset(buf, 0, sizeof(buf));

  while (1)
  {
    move(b_lines - 2, 0);
    clrtoeol();
    outs(" ���j�M ");
    for (i = 0; i < 5; i++)
      prints("/%s %s %s", (ch == i) ? "\033[1;44m" : "", smenu[i], (ch == i) ? "\033[m" : "");

    move(b_lines - 1, 0);
    clrtoeol();
    outs("   �걵 ");
    for (i = 5; i < 10; i++)
      prints("/%s %s %s", (ch == i) ? "\033[1;44m" : "", lmenu[i - 5], (ch == i) ? "\033[m" : "");

    move(b_lines, 0);
    clrtoeol();
    if (ch < 5)
    {
      prints("%s����r�G \033[7m%-*s\033[m", prompt[ch], plen[ch] - 1, "");
      move(b_lines, 12 + 1);
    }
    else if (ch == 6)
    {
      prints("�j�M������ >= + / <= - �h�֪��峹�H (�ϦV�j�M�Ыe�m ! �Ÿ�) \033[7m     \033[m");
      move(b_lines, 60);
    }
    else
      move(b_lines - 2, 0);

    switch (i = vkey())
    {
      case KEY_UP:
	ch -= 5;
	break;
      case KEY_DOWN:
	ch += 5;
	break;
      case KEY_LEFT:
	ch--;
	break;
      case KEY_RIGHT:
	ch++;
	break;
      case '\n':
	if (ch == 1 || ch == 2 || ch == 3 || ch == 4 || ch == 6)
          return XO_BODY;
	xs_ch = 1;
	break;
      default:
	if (isprint2(i))
	  xs_ch = i;
	break;
    }

    if (xs_ch)
      break;

    if (ch < 0)
      ch += 10;
    else if (ch > 9)
      ch -= 10;
  }
  
  if (xs_ch == 1 && ch != 0)
    xs_ch = 0;

  switch (ch)
  {
    case 0:
      return XoXselect(xo);  /* itoc.001220: �j�M�@��/���D */
    case 1:
      return XoXauthor(xo);  /* itoc.001220: �j�M�@�� */
    case 3:
      return XoXfull(xo);    /* itoc.030608: ����j�M */
    case 4:
      return XoXxname(xo);   /* smiler.080201: �j�M�ɦW */
    case 5:
      return XoXsearch(xo);  /* itoc.001220: �j�M�ۦP���D�峹 */
#ifdef HAVE_SCORE
    case 6:
      return XoXscore(xo);
#endif
#ifdef HAVE_REFUSEMARK
    case 7:
      return XoXrefuse(xo);
#endif
    case 8:
      return XoXmark(xo);    /* itoc.010325: �j�M mark �峹 */
    case 9:
      return XoXlocal(xo);   /* itoc.010822: �j�M���a�峹 */
    case 2:
    default:
      return XoXtitle(xo);   /* itoc.001220: �j�M���D */
  }
  return XO_BODY;
}


int
XoRXsearch(xo)
  XO *xo;
{
  ReverseSearch = 1;
  outz("�ư�����j�M 1)�@�� 2)���D 3)mark 4)local�G");
  switch(vkey())
  {
  case '1':
  case 'a':
    return XoXauthor(xo);
    break;

  case '2':
  case '/':
    return XoXtitle(xo);
    break;

  case '3':
  case 'G':
  case 'g':
  case 'm':
    return XoXmark(xo);
    break;

  case '4':
  case 'L':
  case 'l':
    return XoXlocal(xo);

  default:
    ReverseSearch = 0;
    break;
  }

  return XO_FOOT;
}


/* ----------------------------------------------------- */
/* ��C�j�M�ɭ�						 */
/* ----------------------------------------------------- */


int
xpost_head(xo)
  XO *xo;
{
  vs_head("�D�D��C", xo->xyz);

  /* itoc.010323: �P�ɴ��ܧ@��/�D�D */
  outs("[�걵�t�C] ");
  if (*HintAuthor)
    prints("�@�̡G%-13s   ", HintAuthor);
  if (*HintWord)
    prints("���D�G%.40s", HintWord);

  prints(NECKER_XPOST, d_cols, "", currbattr & BRD_NOSCORE ? "��" : "��");

  return XO_BODY;
}


static void
xpost_pick(xo)
  XO *xo;
{
  int *list, fsize, pos, max, top, num;
  HDR *fimage, *hdr;

  fimage = (HDR *) f_map(xo->dir, &fsize);
  if (fimage == (HDR *) - 1)
    return;

  hdr = (HDR *) xo_pool;
  list = xpostIndex;

  pos = xo->pos;
  xo->top = top = (pos / XO_TALL) * XO_TALL;
  max = xo->max;
  pos = top + XO_TALL;
  if (max > pos)
    max = pos;
  num = fsize / sizeof(HDR);

  do
  {
    pos = list[top++];
    if (pos >= num)	/* hightman.030528: �קK .DIR �Q�R��ɡA�|�S���峹�i�H��� */
      continue;
    memcpy(hdr, fimage + pos, sizeof(HDR));
    hdr->xid = pos;		/* �� hdr->xid �ӰO�������b�ݪO���� pos */
    hdr++;
  } while (top < max);

  munmap(fimage, fsize);
}


int
xpost_init(xo)
  XO *xo;
{
  /* load into pool */

  xpost_pick(xo);

  return xpost_head(xo);
}


int
xpost_load(xo)
  XO *xo;
{
  /* load into pool */

  xpost_pick(xo);

  return XO_BODY;
}


int
xpost_browse(xo)
  XO *xo;
{
  HDR *hdr;
  int key;
  char *dir, fpath[64];

  dir = xo->dir;

  for (;;)
  {
    hdr = (HDR *) xo_pool + (xo->pos - xo->top);

#if 0	/* itoc.010822: ���ݭn�A�b XoXpost() ���w�Q�簣 */
#ifdef HAVE_REFUSEMARK
    if (!chkrestrict(hdr))
      continue;
#endif
#endif

    hdr_fpath(fpath, dir, hdr);

    /* Thor.990204: ���Ҽ{more �Ǧ^�� */   
    if ((key = more(fpath, FOOTER_POST)) < 0)
      break;

    comebackPos = hdr->xid; 
    /* Thor.980911: �q�걵�Ҧ��^�Ӯɭn�^��ݹL�����g�峹��m */

    post_history(xo, hdr);
    strcpy(currtitle, str_ttl(hdr->title));

re_key:
    /* Thor.990204: ���Ҽ{more �Ǧ^�� */   
    if (!key)
      key = vkey();

    switch (key)
    {
    case KEY_UP:
    case KEY_PGUP:
    case '[':	/* itoc.000227: ��C�j�M���A���ɷQ�� [ �ݤW�@�g�峹 */
    case 'k':	/* itoc.000227: ��C�j�M���A���ɷQ�� k �ݤW�@�g�峹 */
      {
	int pos = xo->pos - 1;

	/* itoc.000227: �קK�ݹL�Y */
	if (pos < 0)
	  return xpost_head(xo);

	xo->pos = pos;

	if (pos <= xo->top)
	  xpost_pick(xo);
  
	continue;
      }

    case KEY_DOWN:
    case KEY_PGDN:
    case ']':	/* Thor.990204: ��C�j�M���A���ɷQ�� ] �ݤU�@�g�峹 */
    case 'j':	/* Thor.990204: ��C�j�M���A���ɷQ�� j �ݤU�@�g�峹 */
    case ' ':
      {
	int pos = xo->pos + 1;

	/* Thor.980727: �ץ��ݹL�Y��bug */

	if (pos >= xo->max)
    	  return xpost_head(xo);

	xo->pos = pos;

	if (pos >= xo->top + XO_TALL)
  	  xpost_pick(xo);

	continue;
      }

    case 'y':
    case 'r':
      if (bbstate & STAT_POST)
      {
	if (do_reply(xo, hdr) == XO_INIT)	/* �����\�a post �X�h�F */
	  return xpost_init(xo);
      }
      break;

    case 'm': 
      if ((bbstate & STAT_BOARD) && !(hdr->xmode & POST_MARKED))
      {
	/* �b xpost_browse �ɬݤ��� m �O���A�ҥH����u�� mark */
	hdr->xmode ^= POST_MARKED;
	currchrono = hdr->chrono;
	rec_put(dir, hdr, sizeof(HDR), hdr->xid, cmpchrono);
      } 
      break;

#ifdef HAVE_SCORE
	case 'e':
	  post_e_score(xo);
	  return xpost_init(xo);
    case '%': 
      post_score(xo);
      return xpost_init(xo);
#endif

    case '/':
      if (vget(b_lines, 0, "�j�M�G", hunt, sizeof(hunt), DOECHO))
      {
	key = more(fpath, FOOTER_POST);
	goto re_key;
      }
      continue;

    case 'E':
      return post_edit(xo);

    case 'C':	/* itoc.000515: xpost_browse �ɥi�s�J�Ȧs�� */
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
    }
    break;
  }

  return xpost_head(xo);
}


int
xmbox_browse(xo)
  XO *xo;
{
  HDR *hdr;
  char *dir, fpath[64];

  int key;

  dir = xo->dir;

  for (;;)
  {
    hdr = (HDR *) xo_pool + (xo->pos - xo->top);

    hdr_fpath(fpath, dir, hdr);

    /* Thor.990204: ���Ҽ{more �Ǧ^�� */
    if ((key = more(fpath, FOOTER_MAILER)) < 0)
      break;

    comebackPos = hdr->xid; 
    /* Thor.980911: �q�걵�Ҧ��^�Ӯɭn�^��ݹL�����g�峹��m */

    strcpy(currtitle, str_ttl(hdr->title));

re_key:
    /* Thor.990204: ���Ҽ{more �Ǧ^�� */
    if (!key)
      key = vkey();

    switch (key)
    {
    case KEY_UP:
    case KEY_PGUP:
    case '[':	/* itoc.000227: ��C�j�M���A���ɷQ�� [ �ݤW�@�g�峹 */
    case 'k':	/* itoc.000227: ��C�j�M���A���ɷQ�� k �ݤW�@�g�峹 */
      {
	int pos = xo->pos - 1;

	/* itoc.000227: �קK�ݹL�Y */
	if (pos < 0)
	  return xpost_head(xo);

	xo->pos = pos;

	if (pos <= xo->top)
	  xpost_pick(xo);
  
	continue;
      }

    case KEY_DOWN:
    case KEY_PGDN:
    case ']':	/* Thor.990204: ��C�j�M���A���ɷQ�� ] �ݤU�@�g�峹 */
    case 'j':	/* Thor.990204: ��C�j�M���A���ɷQ�� j �ݤU�@�g�峹 */
    case ' ':
      {
	int pos = xo->pos + 1;

	/* Thor.980727: �ץ��ݹL�Y��bug */

	if (pos >= xo->max)
    	  return xpost_head(xo);

	xo->pos = pos;

	if (pos >= xo->top + XO_TALL)
  	  xpost_pick(xo);

	continue;
      }

    case 'y':
    case 'r':
      strcpy(quote_file, fpath);
      do_mreply(hdr, 1);
      break;

    case 'm': 
      if (!(hdr->xmode & POST_MARKED))
      {
	/* �b xmbox_browse �ɬݤ��� m �O���A�ҥH����u�� mark */
	hdr->xmode ^= POST_MARKED;
	currchrono = hdr->chrono;
	rec_put(dir, hdr, sizeof(HDR), hdr->xid, cmpchrono);
      } 
      break;

    case '/':
      if (vget(b_lines, 0, "�j�M�G", hunt, sizeof(hunt), DOECHO))
      {
	key = more(fpath, FOOTER_MAILER);
	goto re_key;
      }
      continue;

    case 'E':
      return mbox_edit(xo);

    case 'C':	/* itoc.000515: xmbox_browse �ɥi�s�J�Ȧs�� */
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
      xo_help("mbox");
      break;
    }
    break;
  }

  return xpost_head(xo);
}


#ifdef HAVE_XYNEWS	/* itoc.010822: news �\Ū�Ҧ� */

#if 0	/* �c�Q */

�b�j�ݪO(�s�u�ݪO)���`�`���ܦh����峹�A��� news ���\Ū�覡�A
��Ҧ� reply ���峹�������áA�u��ܲĤ@�ʵo��C

�Ĥ@���Q�� XoNews �ӭ簣 reply �峹
�ĤG���Q�έ즳�� XoXsearch �j�M�P�D�D�峹
�p���N�i�H�F��s�D�\Ū�Ҧ����ĪG

#endif


static int *newsIndex;

extern KeyFunc news_cb[];


int
news_head(xo)
  XO *xo;
{
  vs_head("�s�D�\\Ū", xo->xyz);
  prints(NECKER_NEWS, d_cols, "");
  return XO_BODY;
}


static void
news_pick(xo)
  XO *xo;
{
  int *list, fsize, pos, max, top;
  HDR *fimage, *hdr;

  fimage = (HDR *) f_map(xo->dir, &fsize);
  if (fimage == (HDR *) - 1)
    return;

  hdr = (HDR *) xo_pool;
  list = newsIndex;

  pos = xo->pos;
  xo->top = top = (pos / XO_TALL) * XO_TALL;
  max = xo->max;
  pos = top + XO_TALL;
  if (max > pos)
    max = pos;

  do
  {
    pos = list[top++];
    memcpy(hdr, fimage + pos, sizeof(HDR));
    /* hdr->xid = pos; */	/* �b XZ_NEWS �S�Ψ� xid�A�i�H�Ҽ{�O�d�� reply �g�� */
    hdr++;
  } while (top < max);

  munmap(fimage, fsize);
}


int
news_init(xo)
  XO *xo;
{
  /* load into pool */
  news_pick(xo);
  return news_head(xo);
}


int
news_load(xo)
  XO *xo;
{
  /* load into pool */
  news_pick(xo);
  return XO_BODY;
}


int
XoNews(xo)			/* itoc: News reader : call from post_cb */
  XO *xo;
{
  int returnPos;
  int *list, fsize, max, count, i;
  char *fimage;
  HDR *head;
  XO *xt;

  if (xo->max <= 0)		/* Thor.980911: ����: �H���U�@ */
    return XO_FOOT;

#ifdef EVERY_Z		/* itoc.060206: �u���� ^Z �~�i��q���P�ݪO�i�J�s�D�Ҧ� */
  if (xz[XZ_NEWS - XO_ZONE].xo)		/* itoc.020308: ���o�ֿn�i�J�G�� */
  {
    vmsg(MSG_XYDENY);
    return XO_FOOT;
  }
#endif

  /* build index according to input condition */

  fimage = f_map(xo->dir, &fsize);

  if (fimage == (char *) -1)
  {
    vmsg("�ثe�L�k�}�ү�����");
    return XO_FOOT;
  }

  /* allocate index memory, remember free first */

  /* Thor.990113: �Ȱ�title, author�������S���Hpost */
  max = fsize / sizeof(HDR);
  list = (int *) malloc(sizeof(int) * max);

  count = 0;

  for (i = 0; i < max; i++)
  {
    head = (HDR *) fimage + i;

#ifdef HAVE_REFUSEMARK
    if (!chkrestrict(head))
      continue;
#endif

    /* check condition */
    if (STR4(head->title) == STR4(STR_REPLY))	/* reply ���峹���n */
      continue;

    list[count++] = i;
  }

  munmap(fimage, fsize);

  if (count <= 0)
  {
    free(list);
    vmsg(MSG_XY_NONE);
    return XO_FOOT;
  }

  newsIndex = list;

  /* build XO for news_xo */

  returnPos = xo->pos;		/* Thor: record pos, future use */
  xz[XZ_NEWS - XO_ZONE].xo = xt = xo_new(xo->dir);
  xz[XZ_NEWS - XO_ZONE].cb = news_cb;
  xt->pos = 0;
  xt->max = count;
  xt->xyz = xo->xyz;
  xt->key = XZ_NEWS;

  xover(XZ_NEWS);

  /* set xo->pos for new location */

  xo->pos = returnPos;		/* �q XZ_NEWS �^�� XZ_POST ��в��h��Ӫ��a�� */

  /* free news_xo */

  if (xt = xz[XZ_NEWS - XO_ZONE].xo)
  {
    free(xt);
    xz[XZ_NEWS - XO_ZONE].xo = NULL;
  }

  /* free index memory, remember check free pointer */

  if (list)
    free(list);

  return XO_INIT;
}
#endif	/* HAVE_XYNEWS */
