/*-------------------------------------------------------*/
/* bmtrans.c   ( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* target : �O�D����					 */
/* create : 08/12/07					 */
/* update : //						 */
/*-------------------------------------------------------*/


#include "bbs.h"

#if 0
  PAL �ɮ׳W��
                  PAL_BAD        PAL_MATE     pal.ship[0]
    old master                       o            'o'
    old slave        o                            'o'

    new master                       o            'c'
    new slave        o                            'c'

    not change
    have completed                                'b'

  �ݭn��ĳ
    *. �s�����O�D	(PALMATE | c) && !(PAL_MATE | o)

  ���ݪ�ĳ
    *. �s�¥��Ҭ��ƪO�D
    *. �����O�D

  �w�����һݪ�ĳ����
    *. �W�椤�w�S������ (PALMATE | c)

  �]�w�s�O�D�v��
    *. �W�[ (PALMATE | b) �O�D�v��
    *. �W�[ (PAL_BAD | c) �O�D�v��
    *. ��s .BRD �P bcache �����O�D�W��
    *. �尣 PAL ��
#endif

extern BCACHE *bshm;


#define	MSG_SEPERATE	"\033[36m"MSG_SEPERATOR"\033[m\n"
#define BRD_NEWBM	"newbm"		/* ���ѥӽв��ʪO�D���O�W */


static int
bmt_setperm(xo, hdr)
  XO *xo;
  HDR *hdr;
{
  ACCT acct;
  PAL pal;
  BRD *brd;
  int pos, num;
  char fpath[64], buf[256];
  time_t now;
  struct tm *ptime;

  sprintf(fpath, "brd/%s/bmt/%s", BRD_NEWBM, hdr->xname);
  num = rec_num(fpath, sizeof(PAL));
  pos = 0;

  while (pos < num)
  {
    rec_get(fpath, &pal, sizeof(PAL), pos);

    if (acct_load(&acct, pal.userid) >= 0)
      acct_setperm(&acct, PERM_BM, 0);
    pos++;
  }

  if (!strcmp(pal.userid, "--------"))
  {
    brd = bshm->bcache + pal.userno;
    strcpy(brd->BM, pal.ship);
    rec_put(FN_BRD, brd, sizeof(BRD), pal.userno, NULL);
    unlink(fpath);

    time(&now);
    ptime = localtime(&now);
    sprintf(buf, "\033[1;30m== sysop\033[m�G\033[1;33m%-51s\033[1;30m%02d/%02d %02d:%02d:%02d\033[m\n",
      "�ӽЮ׳q�L",
      ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);
    hdr_fpath(fpath, xo->dir, hdr);
    f_cat(fpath, buf);

    hdr->xmode |= (POST_MARKED | POST_SCORE);
    currchrono = hdr->chrono;
    rec_put(xo->dir, hdr, sizeof(HDR),
      xo->key == XZ_XPOST ? hdr->xid : xo->pos, cmpchrono);

    vmsg("�ӽЮ׳q�L");
    return 1;
  }

  vmsg("�ӽЮ׳]�w���ѡA�Ц� sysop �O�^��");
  return 0;
}


int
bmt_sign(xo, hdr)
  XO *xo;
  HDR *hdr;
{
  PAL pal;
  int pos, num, mode;
  char fpath[64];

  sprintf(fpath, "brd/%s/bmt/%s", BRD_NEWBM, hdr->xname);
  num = rec_num(fpath, sizeof(PAL)) - 1;
  pos = mode = 0;

  while (pos < num)
  {
    rec_get(fpath, &pal, sizeof(PAL), pos);

    if (!strcmp(pal.userid, cuser.userid))
    {
      if (!pal.ftype)
	vmsg("�z�w��ĳ�L�F");
      else if (vans("�T�w�n��ĳ(y/n)�H[Q] ") == 'y')
      {
	pal.ftype = 0;
	rec_put(fpath, &pal, sizeof(PAL), pos, NULL);
        post_t_score(xo, "��ĳ���g�ӽЮ�", hdr);
	vmsg("��ĳ���\\");
      }
      if (!mode)
        mode = 1;
    }

    if (mode && pal.ftype)
      mode = -1;	/* not completed */
    pos++;
  }

  if (!mode)
    vmsg("�z���ݭn��ĳ���ӽЮ�");
  else if (mode > 0)	/* completed */
    bmt_setperm(xo, hdr);

  return 0;
}


int
bmt_add(xo)
  XO *xo;
{
  BRD *brd;
  ACCT acct;
  PAL pal;
  HDR hdr;
  char fpath[64], buf[80];
  char BMlist[BMLEN + 1];
  char *blist, *ptr;
  int bno, BMlen, len;
  FILE *fp;

  move(b_lines - 20, 0);
  clrtobot();
  prints("%s\n", MSG_SEPERATE);
  prints("\033[1;33m�����毸 " BRD_NEWBM " �O�D���ʨt��\033[m\n"
        "���t�Υ\\�Φb��U�U�����ݪO�i��O�D�W�沧�ʡA\033[1;31m�Ф��ݥ�\033[m\n"
        "�Y�o�{������ϥα��ΡA�N�H���W�B�z�C\n"
        "������ϥΤW�����D�A�·Ц� \033[1;33msysop\033[m �O�^���A���¡I\n\n");
  prints("%s\n", MSG_SEPERATE);

  if (!vget(b_lines, 0, "�^��O�W�G", buf, BNLEN + 1, DOECHO))
    return 0;

  if ((bno = brd_bno(buf)) < 0)
  {
    vmsg("�O�W���~�I");
    return 0;
  }

// get old BM list
// check if master BM!
  brd = bshm->bcache + bno;

  blist = brd->BM;
  if (!(cuser.userlevel & PERM_BM) || blist[0] <= ' ' ||
    (is_bm(blist, cuser.userid) != 1))
  {
    vmsg("�z�������O�D�A�L�k�ӽСI");
    return 0;
  }

#if 0
  if (!(brd->battr & BRD_PUBLIC))	/* �u�������~�ݭn�ӽ� */
  {
    vmsg("���ݪO�������O�A�i�ѥ��O�D�ۦ�]�w�W��");
    return 0;
  }
#endif

  move(10, 0);
  prints("�^��O�W�G%s\n����O�W�G%s\n\n��O�D�W��G%s\n�s�O�D�W��G%s\n%s\n",
    brd->brdname, brd->title, blist, blist, MSG_SEPERATE);

// while(vget())
//   get new bm list
//   check if has BM basic request
//   check if less than BNLEN
  strcpy(BMlist, brd->BM);
  BMlen = strlen(BMlist);

  while(1)
  {
    if (!vget(b_lines, 0, "�п�J�s���O�D�W��A�β����ªO�D�A�����Ы� Enter�G", buf, IDLEN + 1, DOECHO))
    {
      if (vans("������J 1)�O���A�W���J�L�~ 2)�_�A�ڭn���s��J �H[1] ") == '2')
      {
	strcpy(BMlist, brd->BM);
	BMlen = strlen(BMlist);
	continue;
      }
      else
	break;
    }

    if (is_bm(BMlist, buf))	/* �R���¦����O�D */
    {
      len = strlen(buf);
      if (BMlen == len)
      {
	BMlist[0] = '\0';
      }
      else if (!str_cmp(BMlist + BMlen - len, buf))	/* �W��W�̫�@��AID �᭱���� '/' */
      {
	BMlist[BMlen - len - 1] = '\0';			/* �R�� ID �Ϋe���� '/' */
	len++;
      }
      else						/* ID �᭱�|�� '/' */
      {
	str_lower(buf, buf);
	strcat(buf, "/");
	len++;
	ptr = str_str(BMlist, buf);
        strcpy(ptr, ptr + len);
      }
      BMlen -= len;
    }
    else if (acct_load(&acct, buf) >= 0 && !is_bm(BMlist, buf))	/* ��J�s�O�D */
    {
      if (!(acct.userlevel & PERM_VALID))
      {
	vmsg("�ӨϥΪ̩|���q�L�{��");
	continue;
      }
#if 0
      if (acct.numlogins < 300)
      {
	vmsg("�ݤW�� 300 ���H�W�~�����O�D");
	continue;
      }
      if (acct.staytime < 100 * 3600)
      {
	vmsg("�W���ɼƻݹF 100 �p�ɥH�W�~�����O�D");
	continue;
      }
#endif

      len = strlen(buf);
      if (BMlen)
      {
	len++;		/* '/' + userid */
	if (BMlen + len > BMLEN)
	{
	  vmsg("�O�D�W��L���A�L�k�N�o ID �]���O�D");
	  continue;
	}
	sprintf(BMlist + BMlen, "/%s", acct.userid);
	BMlen += len;
      }
      else
      {
	strcpy(BMlist, acct.userid);
	BMlen = len;
      }
    }
    else
      continue;

    move(14, 0);
    prints("�s�O�D�W��G%s\n", BMlist);
  }

  if (!strcmp(BMlist, blist))
  {
    vmsg("�L������");
    return 0;
  }
  if (!strlen(BMlist))
  {
    vmsg("�ߤ@���O�D����ݥѯ��Ȥ�ʼf�֡A�Хt��ӽ�");
    return 0;
  }

  if (vans("�Юֹ��ƬO�_���T 1)���T�A���X�ӽ� 2)���� �H[1] ") == '2')
  {
    unlink("pal_file");
    vmsg("�����ӽ�");
    return 0;
  }

// auto post to board BM
  sprintf(fpath, "tmp/%s.bmt", cuser.userid);
  sprintf(buf, "[�ӽЪO�D����] %s", brd->brdname);
  if (fp = fopen(fpath, "w"))
  {
    fprintf(fp, "�@��: %s (%s) ����: %s\n", cuser.userid, cuser.username, BRD_NEWBM);
    fprintf(fp, "���D: %s\n", buf);
    fprintf(fp, "�ɶ�: %s\n\n\n", Now());
    fprintf(fp, "�^��O�W�G%s\n", brd->brdname);
    fprintf(fp, "����O�W�G%s\n", brd->title);
    fprintf(fp, "��O�D�W��G%s\n", blist);
    fprintf(fp, "�s�O�D�W��G%s\n", BMlist);
    fprintf(fp, "\n\n\n--\n�� �����毸 - �O�D�汵�t��\n");
    fclose(fp);
    add_post(BRD_NEWBM, fpath, buf, cuser.userid, cuser.username, 0, &hdr);
    unlink(fpath);
  }

// add PAL file that contains BM changes
// set file mode ?
// add function key to let BM changes
// add detect function if all new BM got sign

  sprintf(fpath, "brd/%s/bmt/%s", BRD_NEWBM, hdr.xname);
  blist = BMlist;

  do
  {
    if (ptr = strchr(blist, '/'))
    {
      str_ncpy(buf, blist, ptr - blist + 1);
      blist = ptr + 1;
    }
    else	/* last one */
      strcpy(buf, blist);

    memset(&pal, 0, sizeof(PAL));
    acct_load(&acct, buf);
    strcpy(pal.userid, acct.userid);
    pal.userno = acct.userno;
    if (is_bm(BMlist, acct.userid) == 1 && is_bm(brd->BM, acct.userid) != 1)
      pal.ftype = PAL_MATE;	/* new master */
    else if (is_bm(BMlist, acct.userid) && !is_bm(brd->BM, acct.userid))
      pal.ftype = PAL_BAD;	/* new slave */

    if (pal.ftype)
      rec_add(fpath, &pal, sizeof(PAL));
  } while (ptr);

  memset(&pal, 0, sizeof(PAL));
  strcpy(pal.userid, "--------");
  pal.userno = bno;
  strcpy(pal.ship, BMlist);
  rec_add(fpath, &pal, sizeof(PAL));

  if (rec_num(fpath, sizeof(PAL)) == 1)
    bmt_setperm(xo, &hdr);
  else
    vmsg("�ӽЧ����A�зs���O�D��ĳ");

  return 0;
}
