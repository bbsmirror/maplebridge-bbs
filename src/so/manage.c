/*-------------------------------------------------------*/
/* manage.c	( NTHU CS MapleBBS Ver 3.10 )		 */
/*-------------------------------------------------------*/
/* target : �ݪO�޲z				 	 */
/* create : 95/03/29				 	 */
/* update : 96/04/05				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"


extern BCACHE *bshm;


#ifdef HAVE_TERMINATOR
/* ----------------------------------------------------- */
/* �����\�� : �ط�������				 */
/* ----------------------------------------------------- */


extern char xo_pool[];


#define MSG_TERMINATOR	"�m�ط������١n"

int
post_terminator(xo)		/* Thor.980521: �׷��峹�R���j�k */
  XO *xo;
{
  int mode, type;
  HDR *hdr;
  char keyOwner[80], keyTitle[TTLEN + 1], buf[80];


  /* smiler.1111 �bDeletelog �H�� Editlog �O ������ post_terminator(xo) */
 if((!strcmp(xo->dir,"brd/Deletelog/.DIR")) || (!strcmp(xo->dir,"brd/Editlog/.DIR")))
   return XO_NONE;


  if (!HAS_PERM(PERM_ALLBOARD))
    return XO_FOOT;

  mode = vans(MSG_TERMINATOR "�R�� (1)����@�� (2)������D (3)�۩w�H[Q] ") - '0';

  if (mode == 1)
  {
    hdr = (HDR *) xo_pool + (xo->pos - xo->top);
    strcpy(keyOwner, hdr->owner);
  }
  else if (mode == 2)
  {
    hdr = (HDR *) xo_pool + (xo->pos - xo->top);
    strcpy(keyTitle, str_ttl(hdr->title));		/* ���� Re: */
  }
  else if (mode == 3)
  {
    if (!vget(b_lines, 0, "�@�̡G", keyOwner, 73, DOECHO))
      mode ^= 1;
    if (!vget(b_lines, 0, "���D�G", keyTitle, TTLEN + 1, DOECHO))
      mode ^= 2;
  }
  else
  {
    return XO_FOOT;
  }

  type = vans(MSG_TERMINATOR "�R�� (1)��H�O (2)�D��H�O (3)�Ҧ��ݪO�H[Q] ");
  if (type < '1' || type > '3')
    return XO_FOOT;

  sprintf(buf, "�R��%s�G%.35s ��%s�O�A�T�w��(Y/N)�H[N] ",
    mode == 1 ? "�@��" : mode == 2 ? "���D" : "����",
    mode == 1 ? keyOwner : mode == 2 ? keyTitle : "�۩w",
    type == '1' ? "��H" : type == '2' ? "�D��H" : "�Ҧ���");

  if (vans(buf) == 'y')
  {
    BRD *bhdr, *head, *tail;
    char tmpboard[BNLEN + 1];

    /* Thor.980616: �O�U currboard�A�H�K�_�� */
    strcpy(tmpboard, currboard);

    head = bhdr = bshm->bcache;
    tail = bhdr + bshm->number;
    do				/* �ܤ֦� note �@�O */
    {
      int fdr, fsize, xmode;
      FILE *fpw;
      char fpath[64], fnew[64], fold[64];
      HDR *hdr;

      /* smiler.1111: �Y���Ұ�editlog_use deletelog_use */
      char Deletelog_folder[64]/*, Deletelog_title[64]*/, copied[64];
      HDR  Deletelog_hdr;	/* �h�N�Q�媺��Ʋ��� Deletelog �O�ƥ� */

      xmode = head->battr;
      if ((type == '1' && (xmode & BRD_NOTRAN)) || (type == '2' && !(xmode & BRD_NOTRAN)))
	continue;

      /* smiler.1111: �O�@Editlog Deletelog����O���ݪO���Q�R����� */
      if ((!strcmp(head->brdname, BN_DELLOG)) || (!strcmp(head->brdname, BN_EDITLOG)))
	continue;

      /* Thor.980616: ��� currboard�A�H cancel post */
      strcpy(currboard, head->brdname);

      sprintf(buf, MSG_TERMINATOR "�ݪO�G%s \033[5m...\033[m", currboard);
      outz(buf);
      refresh();

      brd_fpath(fpath, currboard, fn_dir);

      if ((fdr = open(fpath, O_RDONLY)) < 0)
	continue;

      if (!(fpw = f_new(fpath, fnew)))
      {
	close(fdr);
	continue;
      }

      fsize = 0;
      mgets(-1);
      while (hdr = mread(fdr, sizeof(HDR)))
      {
	xmode = hdr->xmode;

	if ((xmode & POST_MARKED) ||
	  ((mode & 1) && strcmp(keyOwner, hdr->owner)) ||
	  ((mode & 2) && strcmp(keyTitle, str_ttl(hdr->title))))
	{
	  if ((fwrite(hdr, sizeof(HDR), 1, fpw) != 1))
	  {
	    fclose(fpw);
	    close(fdr);
	    goto contWhileOuter;
	  }
	  fsize++;
	}
	else
	{
	  /* ���ós�u��H */

	  cancel_post(hdr);

	  /* smiler.1111: �Ydeletelog_use���Ұ�,�h�N�o���Q�R����Ƴƥ���Deletelog�O */
	  if (deletelog_use)
	  {
	    hdr_fpath(copied, fpath, hdr);
	    brd_fpath(Deletelog_folder,BN_DELLOG, FN_DIR);
	    hdr_stamp(Deletelog_folder, HDR_COPY | 'A', &Deletelog_hdr, copied);
	    strcpy(Deletelog_hdr.title , hdr->title);
	    strcpy(Deletelog_hdr.owner , cuser.userid);
	    strcpy(Deletelog_hdr.nick  , cuser.username);
	    Deletelog_hdr.xmode = POST_OUTGO;
	    rec_bot(Deletelog_folder, &Deletelog_hdr, sizeof(HDR));
	    btime_update(brd_bno(BN_DELLOG));
	  }

	  hdr_fpath(fold, fpath, hdr);
	  unlink(fold);
	  if (xmode & POST_RESTRICT)
	    RefusePal_kill(currboard, hdr);

	}
      }
      close(fdr);
      fclose(fpw);

      sprintf(fold, "%s.o", fpath);
      rename(fpath, fold);
      if (fsize)
	rename(fnew, fpath);
      else
      {
contWhileOuter:
	unlink(fnew);
      }

      btime_update(brd_bno(currboard));
    } while (++head < tail);

    /* �٭� currboard */
    strcpy(currboard, tmpboard);
    return XO_LOAD;
  }

  return XO_FOOT;
}
#endif	/* HAVE_TERMINATOR */


/* ----------------------------------------------------- */
/* �O�D�\�� : �ק�O�W					 */
/* ----------------------------------------------------- */


static int
vkans(msg)	/* vans | vkey: ²�ƨϥΪ̫��䦸�� */
  char *msg;
{
  int ch;

  move(b_lines, 0);
  clrtoeol();
  outs(msg);
  ch = vkey();
  if (ch >= 'A' && ch <= 'Z')	/* ���p�g */
    ch |= 0x20;
  return ch;
}


int
post_brdtitle()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;
  memcpy(&newbrd, oldbrd, sizeof(BRD));

  /* itoc.����: ���I�s brd_title(bno) �N�i�H�F�A�S�t�A�Z�F�@�U�n�F :p */
  if (vkans("�O�_�ק襤��O�W�ԭz(Y/N)�H[N] ") == 'y')
  {
    vget(b_lines, 0, "�ݪO�D�D�G", newbrd.title, BTLEN + 1, GCARRY);

    if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
    {
      memcpy(oldbrd, &newbrd, sizeof(BRD));
      currchrono = newbrd.bstamp;
      rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
    }
  }

  return 0;
}


/* ----------------------------------------------------- */
/* �O�D�\�� : �ק�i�O�e��				 */
/* ----------------------------------------------------- */


int
post_memo_edit()
{
  int mode;
  char fpath[64];

  mode = vans("�i�O�e�� (D)�R�� (E)�ק� (Q)�����H[E] ");

  if (mode != 'q')
  {
    brd_fpath(fpath, currboard, fn_note);

    if (mode == 'd')
    {
      unlink(fpath);
      return 0;
    }

    if (vedit(fpath, 0))	/* Thor.981020: �`�N�Qtalk�����D */
      vmsg(msg_cancel);
  }

  return 0;
}


/* smiler.080203: �U�O�ۭq�׫H���� */
int
post_spam_edit()
{
  int mode;
  char fpath[64];

  mode = vans("�׫H�C�� (D)�R�� (E)�ק� (Q)�����H[E] ");

  if (mode != 'q')
  {
    brd_fpath(fpath, currboard, "spam");

    if (mode == 'd')
    {
      unlink(fpath);
    }
    else
    {
      more("etc/help/post/post_spam_ed",NULL);
      if (vedit(fpath, 0))	/* Thor.981020: �`�N�Qtalk�����D */
	vmsg(msg_cancel);
    }
  }

  return 0;
}

#ifdef POST_PREFIX
/* ----------------------------------------------------- */
/* �O�D�\�� : �ק�o�����O				 */
/* ----------------------------------------------------- */


#ifdef HAVE_TEMPLATE
int
post_template_edit()
{
  FILE *fp;
  int i, ans, pnum;
  char buf[64], fpath[64];
  char prefix[NUM_PREFIX][16];
  char *prefix_def[NUM_PREFIX] = DEFAULT_PREFIX;

  if (!(bbstate & STAT_BOARD))
    return 0;

  move(b_lines - 1, 0);
  clrtoeol();
  prints("���O: ");

  i = 0;
  ans = 6;
  brd_fpath(fpath, currboard, "prefix.new");
  if (fp = fopen(fpath, "r"))
  {
    for (; i < NUM_PREFIX; i++)
    {
      if (!fgets(buf, 14, fp))
	break;
      if (strlen(buf) == 1)
	break;
      buf[strlen(buf) - 1] = '\0';
      sprintf(prefix[i], "%s", buf);
      prints("%d.%s ", i + 1, buf);
      pnum = i + 1;
      ans += (3 + strlen(buf));
    }
    fclose(fp);
  }
  else		/* ���S���~ initialize */
  {
    pnum = NUM_PREFIX;
    for (; i < NUM_PREFIX; i++)
    {
      sprintf(prefix[i], "%s", prefix_def[i]);
      prints("%d.%s ", i + 1, prefix[i]);
      ans += (3 + strlen(prefix[i]));
    }
  }
  move(b_lines, 0);
  clrtoeol();

  i = vget(b_lines - 1, ans, "", fpath, 3, DOECHO) - '1';
  if (i < 0 || i >= pnum)
    return 0;

  sprintf(buf, "�d�� \033[1m%d.[%s]\033[m D)�R�� E)�ק� Q)�����H[Q] ", i + 1, prefix[i]);
  ans = vans(buf);

  if (ans == 'd')
  {
    unlink(fpath);
    return 0;
  }

  if (ans != 'e')
    return 0;

  brd_fpath(fpath, currboard, "prefix");
  if (!dashd(fpath))
    mkdir(fpath, 0700);

  sprintf(buf, "prefix/template_%d", i + 1);
  brd_fpath(fpath, currboard, buf);

  if (vedit(fpath, 0))	/* Thor.981020: �`�N�Qtalk�����D */
    vmsg(msg_cancel);

  return 0;
}
#endif	/* HAVE_TEMPLATE */


static int
post_prefix_edit()
{
  FILE *fp;
  int i;
  char fpath[64], buf[20], prefix[NUM_PREFIX][16], *menu[NUM_PREFIX + 3];
  char *prefix_def[NUM_PREFIX] = DEFAULT_PREFIX;

  if (!(bbstate & STAT_BOARD))
    return 0;

  brd_fpath(fpath, currboard, "prefix.new");
  i = vans("���O (D)�R�� (E)�ק� (Q)�����H[Q] ");

  if (i == 'd')
  {
    unlink(fpath);
    return 0;
  }

  if (i != 'e')
    return 0;

  i = 0;
  if (fp = fopen(fpath, "r"))
  {
    for (; i < NUM_PREFIX; i++)
    {
      if (!fgets(buf, 14, fp))
	break;
      if (strlen(buf) == 1)
	break;
      buf[strlen(buf) - 1] = '\0';
      sprintf(prefix[i], "%d.%s", i + 1, buf);
    }
    fclose(fp);
  }

  /* �񺡦� NUM_PREFIX �� */
  if (!i)	/* ���S���~ initialize */
  {
    for (; i < NUM_PREFIX; i++)
      sprintf(prefix[i], "%d.%s", i + 1, prefix_def[i]);
  }
  else
  {
    for (; i < NUM_PREFIX; i++)
      sprintf(prefix[i], "%d.%s", i + 1, "");
  }

#ifdef POPUP_ANSWER
  menu[0] = "10";
  for (i = 1; i <= NUM_PREFIX; i++)
    menu[i] = prefix[i - 1];
  menu[NUM_PREFIX + 1] = "0.���}";
  menu[NUM_PREFIX + 2] = NULL;

  do
  {
    /* �b popupmenu �̭��� ���� ���} */
    i = pans(3, 20, "�峹���O", menu) - '0';
    if (i >= 1 && i <= NUM_PREFIX)
    {
      strcpy(buf, prefix[i - 1] + 2);
      vget(b_lines, 0, "���O�G", buf, 13, GCARRY);	/* �d�մN�M�� */
	strcpy(prefix[i - 1] + 2, buf);
    }
  } while (i);
#else
  for (i = 0; i < NUM_PREFIX; i++)
    menu[i] = prefix[i];

  do
  {
    move(b_lines - 2, 0);
    clrtobot();
    for (i = 0; i < NUM_PREFIX; i++)
      prints("%s ", menu[i]);
    vget(b_lines - 1, 0, "�п�ܱz�n��諸����(1 ~ 8�A�ο�J 0 ���})", buf, 3, DOECHO);
    i = atoi(buf);
    if (i >= 1 && i <= NUM_PREFIX)
    {
      strcpy(buf, prefix[i - 1] + 2);
      vget(b_lines, 0, "���O�G", buf, 13, GCARRY);	/* �d�մN�M�� */
	strcpy(prefix[i - 1] + 2, buf);
    }
  } while (i);
#endif

  if (fp = fopen(fpath, "w"))
  {
    for (i = 0; i < NUM_PREFIX; i++)
      fprintf(fp, "%s\n", prefix[i] + 2);
    fclose(fp);
  }

  return 0;
}
#endif	/* POST_PREFIX */


int
post_brd_prefix()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;

  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("�ϥΤ峹���O (1)�ϥ� (2)���ϥ� (3)�]�w���O (Q)�����H[Q] "))
  {
  case '1':
    newbrd.battr &= ~BRD_NOPREFIX;
    break;
  case '2':
    newbrd.battr |= BRD_NOPREFIX;
    break;
  case '3':
    post_prefix_edit();
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}


/* ----------------------------------------------------- */
/* �O�D�\�� : �ݪO�ݩ�					 */
/* ----------------------------------------------------- */


/* smiler.090206: �]�w�ݪO���� IP ��� */
int
post_brd_ip_char()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;
  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("�ݪO������� (1)IP (2)IP �N�X (Q)�����]�w�H[Q] "))
  {
  case '1':
    newbrd.battr |= BRD_POST_IP;
    break;
  case '2':
    newbrd.battr &= ~BRD_POST_IP;
    break;
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}

#ifdef HAVE_SCORE
int
post_battr_noscore()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;
  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("�}����� (1)���\\ (2)�����\\ (Q)�����H[Q] "))
  {
  case '1':
    newbrd.battr &= ~BRD_NOSCORE;
    break;
  case '2':
    newbrd.battr |= BRD_NOSCORE;
    break;
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}
#endif	/* HAVE_SCORE */


int
post_rlock()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;

  if (oldbrd->battr & BRD_PUBLIC)  /* �����O�����\�H�N��� */
    return 0;

  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("�}����� (1)���\\ (2)�����\\ (Q)�����H[Q] "))
  {
  case '1':
    newbrd.battr &= ~BRD_NOL;
    break;
  case '2':
    newbrd.battr |= BRD_NOL;
    break;
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}


int
post_vpal()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;

  if (oldbrd->battr & BRD_PUBLIC)  /* �����O�����\�H�N��� */
    return 0;

  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("�}���[�ݪO�ͦW�� (1)���\\ (2)�����\\ (Q)�����H[Q] "))
  {
  case '1':
    newbrd.battr &= ~BRD_HIDEPAL;
    break;
  case '2':
    newbrd.battr |= BRD_HIDEPAL;
    break;
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 1;
}


int
post_noforward()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;
  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("����峹 (1)���\\ (2)�T�� (Q)�����H[Q] "))
  {
  case '1':
    newbrd.battr &= ~BRD_NOFORWARD;
    break;
  case '2':
    newbrd.battr |= BRD_NOFORWARD;
    break;
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}


int
post_showturn()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;
  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("����O�� (1)���} (2)���� (Q)�����H[Q] "))
  {
  case '1':
    newbrd.battr |= BRD_SHOWTURN;
    break;
  case '2':
    newbrd.battr &= ~BRD_SHOWTURN;
    break;
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}

/* ----------------------------------------------------- */
/* �O�D�\�� : �ק�O�D�W��				 */
/* ----------------------------------------------------- */


int
post_changeBM()
{
  char buf[80], userid[IDLEN + 2], *blist;
  BRD *oldbrd, newbrd;
  ACCT acct;
  int BMlen, len;

  oldbrd = bshm->bcache + currbno;

  blist = oldbrd->BM;
  if (is_bm(blist, cuser.userid) != 1)	/* �u�����O�D�i�H�]�w�O�D�W�� */
    return 0;

  if (oldbrd->battr & BRD_PUBLIC)	/* �����O�����\�H�N��� */
    return 0;

  if (strncmp(oldbrd->brdname, "P_", 2) && strncmp(oldbrd->brdname, "L_", 2) &&
    strncmp(oldbrd->brdname, "R_", 2) && strncmp(oldbrd->brdname, "G_", 2) &&
    strncmp(oldbrd->brdname, "LAB_", 4) && !(oldbrd->battr & BRD_IAS))
    return 0;

  memcpy(&newbrd, oldbrd, sizeof(BRD));

  move(5, 0);
  clrtobot();

  move(8, 0);
  prints("�ثe�O�D�� %s\n�п�J�s���O�D�W��A�Ϋ� [Return] ����", oldbrd->BM);

  strcpy(buf, oldbrd->BM);
  BMlen = strlen(buf);

  while (vget(10, 0, "�п�J�ƪO�D�A�����Ы� Enter�A�M���Ҧ��ƪO�D�Х��u�L�v�G", userid, IDLEN + 1, DOECHO))
  {
    if (!strcmp(userid, "�L"))
    {
      strcpy(buf, cuser.userid);
      BMlen = strlen(buf);
    }
    else if (is_bm(buf, userid))	/* �R���¦����O�D */
    {
      len = strlen(userid);
      if (!str_cmp(cuser.userid, userid))
      {
	vmsg("���i�H�N�ۤv���X�O�D�W��");
	continue;
      }
      else if (!str_cmp(buf + BMlen - len, userid))	/* �W��W�̫�@��AID �᭱���� '/' */
      {
	buf[BMlen - len - 1] = '\0';			/* �R�� ID �Ϋe���� '/' */
	len++;
      }
      else						/* ID �᭱�|�� '/' */
      {
	str_lower(userid, userid);
	strcat(userid, "/");
	len++;
	blist = str_str(buf, userid);
	strcpy(blist, blist + len);
      }
      BMlen -= len;
    }
    else if (acct_load(&acct, userid) >= 0 && !is_bm(buf, userid))	/* ��J�s�O�D */
    {
      len = strlen(userid) + 1;	/* '/' + userid */
      if (BMlen + len > BMLEN)
      {
	vmsg("�O�D�W��L���A�L�k�N�o ID �]���O�D");
	continue;
      }
      sprintf(buf + BMlen, "/%s", acct.userid);
      BMlen += len;

      acct_setperm(&acct, PERM_BM, 0);
    }
    else
      continue;

    move(8, 0);
    prints("�ثe�O�D�� %s", buf);
    clrtoeol();
  }
  strcpy(newbrd.BM, buf);

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);

    sprintf(currBM, "�O�D�G%s", newbrd.BM);
  }

  return 0;
}


#ifdef HAVE_MODERATED_BOARD
/* ----------------------------------------------------- */
/* �O�D�\�� : �ݪO�v��					 */
/* ----------------------------------------------------- */


int
post_brdlevel()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;
  memcpy(&newbrd, oldbrd, sizeof(BRD));

  if (oldbrd->battr & BRD_PUBLIC)  /* �����O�����\�H�N��� */
    return 0;

  if (oldbrd->battr & BRD_IAS)	/* �����]�ݪO�����\�H�N��� */
  {
    vmsg("�����]�ݪO�p�ݧ���ݩʽЦV�]�ȥӽ�!!");
    return 0;
  }

  switch (vkans("1)���}�ݪO 2)���K�ݪO 3)�n�ͬݪO�H[Q] "))
  {
  case '1':				/* ���}�ݪO */
    newbrd.readlevel = 0;
    newbrd.postlevel = PERM_POST;
    newbrd.battr &= ~(BRD_NOSTAT | BRD_NOVOTE);
    break;

  case '2':				/* ���K�ݪO */
    newbrd.readlevel = PERM_SYSOP;
    newbrd.postlevel = 0;
    newbrd.battr |= (BRD_NOSTAT | BRD_NOVOTE);
    break;

  case '3':				/* �n�ͬݪO */
    newbrd.readlevel = PERM_BOARD;
    newbrd.postlevel = 0;
    newbrd.battr |= (BRD_NOSTAT | BRD_NOVOTE);
    break;

  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}
#endif	/* HAVE_MODERATED_BOARD */


#ifdef HAVE_MODERATED_BOARD
/* ----------------------------------------------------- */
/* �O�ͦW��Gmoderated board				 */
/* ----------------------------------------------------- */


static void
bpal_cache(fpath)
  char *fpath;
{
  BPAL *bpal;

  bpal = bshm->pcache + currbno;
  bpal->pal_max = image_pal(fpath, bpal->pal_spool);
}


extern XZ xz[];


int
XoBM()
{
  BRD *oldbrd;
  oldbrd = bshm->bcache + currbno;

  XO *xt;
  char fpath[64];

  more("etc/help/post/post_pal_ed",NULL);

  brd_fpath(fpath, currboard, fn_pal);
  xz[XZ_PAL - XO_ZONE].xo = xt = xo_new(fpath);
  xt->key = PALTYPE_BPAL;
  xover(XZ_PAL);		/* Thor: �ixover�e, pal_xo �@�w�n ready */

  /* build userno image to speed up, maybe upgreade to shm */

  bpal_cache(fpath);

  free(xt);

  return 1;
}
#endif	/* HAVE_MODERATED_BOARD */


#ifdef DO_POST_FILTER
/* ----------------------------------------------------- */
/* �ݪ���						 */
/* ----------------------------------------------------- */


static int
post_bbs_dog()
{
  BRD *oldbrd, newbrd;

  oldbrd = bshm->bcache + currbno;
  memcpy(&newbrd, oldbrd, sizeof(BRD));

  switch (vkans("�[�JBBS�ݪ����p�e 1)�O 2)�_ Q)�����H[Q] "))
  {
  case '1':
    newbrd.battr |= BRD_BBS_DOG;
    break;
  case '2':
    newbrd.battr &= (~BRD_BBS_DOG);
    break;
  default:
    return 0;
  }

  if (memcmp(&newbrd, oldbrd, sizeof(BRD)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(oldbrd, &newbrd, sizeof(BRD));
    currchrono = newbrd.bstamp;
    rec_put(FN_BRD, &newbrd, sizeof(BRD), currbno, cmpbstamp);
  }

  return 0;
}


static int
post_article_filter()
{
#define	NUM_DOG		10
#define	LEN_DOG_NAME	70

  FILE *fp;
  char fpath[64], buf[LEN_DOG_NAME], input[LEN_DOG_NAME], dog[NUM_DOG][LEN_DOG_NAME];
  int i, choose;

  brd_fpath(fpath, currboard, FN_BBSDOG);
  if (!dashf(fpath))
  {
    fp = fopen(fpath, "w");
    i = 0;
    while (i < 10)
    {
      fprintf(fp, "%c\n",'\0');
      i++;
    }
    fclose(fp);
  }

  choose = 0;
  do
  {
    if (choose)
    {
      move(b_lines - 1, 0);
      prints("�ק�� %d �� : ", choose);

      if (!vget(b_lines, 0, "", input, LEN_DOG_NAME, DOECHO))
	input[0] = '\0';

      while (strstr(input," "))
      {
	move(b_lines - 2, 0);
	prints("\033[1;33m��J���r�ꤤ���o���Ů�A�Э��s��J !!\033[m\n");
	prints("�ק�� %d �� : ", choose);

	if (!vget(b_lines, 0, "", input, LEN_DOG_NAME, DOECHO))
	{
	  input[0] = '\0';
	  break;
	}
      }
      if (input[0] != '\0')
	strcpy(dog[choose - 1], input);
      else
	dog[choose - 1][0] = '\0';

      fp = fopen(fpath, "w");
      for (i = 0 ; i < NUM_DOG ; i++ )
      {
	 if (dog[i][0] == '\0')
	 {
	   fprintf(fp, "%c\n", dog[i][0]);
	 }
	 else
	   fprintf(fp, "%s\n", dog[i]);
      }
      fclose(fp);
    }

    fp = fopen(fpath, "r");
    clear();
    for (i = 0; i < NUM_DOG; i++)
    {
      prints("%d.\n", i+1);

      fscanf(fp, "%s", &buf);

      if ((buf[0] == '\0') || (buf[0] == '\n'))
      {
	prints("\n");
	dog[i][0] = '\0';
      }
      else
      {
	prints("\033[0;30;47m%s\033[m\n", buf);
	strcpy(dog[i], buf);
      }
    }
    fclose(fp);

    switch(vans("��� (D)�R�� (E)�ק� (Q)���}�H[E] "))
    {
    case 'd':
      unlink(fpath);
    case 'q':
      return 0;
    }

    if (!vget(b_lines, 0, "�� �W�h�ק� 1~10) (Q)���} [Q]", input, 3, DOECHO))
      break;
    choose = atoi(input);

    if (choose > 10 || choose < 0)
      break;
  } while(choose);

  return 0;
}


/* smiler.080831: �O�D�۩w �iŪ/�i�g/�i�C
   �ثe�P�_���� :
   1. �ͤ�     (�~�֭���@: 18�T�@or any level)
   2. �ʧO     (�k�ʱM�O�A�k�ʱM�O....balabala)
   3. �W�u���� (���o�p��h�֦�)
   4. �峹�g�� (���o�p��h�ֽg)
   5. �u��g�� (���o�p��h�ֽg)
   6. �H��g�� (���o����h�ֽg)
   7. �H�W���� (���o����h�֦�)
   8. �ȹ��@�@ (���o�֩�h�֪T)
   9. �����@�@ (���o�֩�h�֪T)
  10. �o�H���� (���o�p��h�֦�)
  11. ���U�ɶ� (���o�C��h�֤�)
*/
static int
post_my_level(mode)
  int mode;
{
  FILE *fw;
  BPERM newperm, *perm;
  char fpath_r[64], fpath_w[64], wd[64], buf[16];
  char *title, *list;
  int i;

  switch (mode)
  {
    case 0:
      perm = bshm->rperm + currbno;
      title = "�\\Ū����";
      list = FN_NO_READ;
      break;
    case 1:
      perm = bshm->wperm + currbno;
      title = "�o�孭��";
      list = FN_NO_WRITE;
      break;
    case 2:
    default:
      perm = bshm->lperm + currbno;
      title = "�C�X����";
      list = FN_NO_LIST;
      break;
  }
  memset(&newperm, 0, sizeof(BPERM));
  if (perm->exist)
    memcpy(&newperm, perm, sizeof(BPERM));

  brd_fpath(fpath_r, currboard, list);
  sprintf(fpath_w, "%s.tmp", fpath_r);

  sprintf(wd, "%s E)�s�� D)�R�� Q)���� [E] ", title);
  switch (vans(wd))
  {
  case 'd':
    if (dashf(fpath_r))
      unlink(fpath_r);
    perm->exist = 0;
  case 'q':
    return 0;
  }

  while (1)
  {
    clear();

    if (!(fw = fopen(fpath_w, "w")))
     return 0;

    i = 3;
    move(1, 0);
    prints(" - \033[1;33m%s\033[m", title);

    sprintf(wd, "�~�֭��� >= [%2d��]�G", newperm.age);
    if (vget(i++, 0, wd, buf, 3, DOECHO))
      newperm.age = atoi(buf);
    if (newperm.age >= 0)
      fprintf(fw, "%d\n", newperm.age);
    else
    {
      newperm.age = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�ʧO���� (0)����(1)���� (2)�k�� (3)�k�ʡG[%d] ", newperm.sex);
    if (vget(i++, 0, wd, buf, 2, DOECHO))
      newperm.sex = (atoi(buf) % 4);
    if (newperm.sex >= 0)
      fprintf(fw, "%d\n", newperm.sex);
    else
    {
      newperm.sex = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�W�u���� >= [%d��] ", newperm.numlogins);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.numlogins = atoi(buf);
    if (newperm.numlogins >= 0)
      fprintf(fw, "%d\n", newperm.numlogins);
    else
    {
      newperm.numlogins = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�峹�g�� >= [%d�g] ", newperm.numposts);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.numposts = atoi(buf);
    if (newperm.numposts >= 0)
      fprintf(fw, "%d\n", newperm.numposts);
    else
    {
      newperm.numposts = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�u��g�� >= [%d�g] ", newperm.good_article);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.good_article = atoi(buf);
    if (newperm.good_article >= 0)
      fprintf(fw, "%d\n", newperm.good_article);
    else
    {
      newperm.good_article = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�H��g�� <  [%d�g] (0�G����) ", newperm.poor_article);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.poor_article = atoi(buf);
    if (newperm.poor_article >= 0)
      fprintf(fw, "%d\n", newperm.poor_article);
    else
    {
      newperm.poor_article = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�H�W���� <  [%d��] (0�G����) ", newperm.violation);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.violation = atoi(buf);
    if (newperm.violation >= 0)
      fprintf(fw, "%d\n", newperm.violation);
    else
    {
      newperm.violation = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�ȹ�     >= [%d�T] ", newperm.money);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.money = atoi(buf);
    if (newperm.money >= 0)
      fprintf(fw, "%d\n", newperm.money);
    else
    {
      newperm.money = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "����     >= [%d�T] ", newperm.gold);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.gold = atoi(buf);
    if (newperm.gold >= 0)
      fprintf(fw, "%d\n", newperm.gold);
    else
    {
      newperm.gold = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "�o�H���� >= [%d��] ", newperm.numemails);
    if (vget(i++, 0, wd, buf, 10, DOECHO))
      newperm.numemails = atoi(buf);
    if (newperm.numemails >= 0)
      fprintf(fw, "%d\n", newperm.numemails);
    else
    {
      newperm.numemails = 0;
      fprintf(fw, "0\n");
    }

    sprintf(wd, "���U�ɶ� >= [%2d��] ", newperm.regmonth);
    if (vget(i++, 0, wd, buf, 3, DOECHO))
      newperm.regmonth = atoi(buf);
    if (newperm.regmonth >= 0)
      fprintf(fw, "%d\n", newperm.regmonth);
    else
    {
      newperm.regmonth = 0;
      fprintf(fw, "0\n");
    }

    fclose(fw);

    switch (vkans("�T�w����v���]�w (S)�s�� (E)�~�� (Q)�����H[Q] "))
    {
    case 's':
      unlink(fpath_r);
      rename(fpath_w, fpath_r);
      newperm.exist = 1;
      memcpy(perm, &newperm, sizeof(BPERM));
      return 0;

    case 'e':
      continue;

    default:
      unlink(fpath_w);
      return 0;
    }
 }

 return 0;
}


static int
post_view_bbs_dog_log()
{
  char fpath[64],warn[64];

  brd_fpath(fpath, currboard, FN_BBSDOG_LOG);
  more(fpath, NULL);

  switch (vans("�פ�O�� M)�H�^�ۤv�H�c D)�R�� Q)���}�H[Q] "))
  {
  case 'm':
    sprintf(warn, "%s �O�פ�O����", currboard);
    mail_self(fpath, cuser.userid, warn, 0);
    break;

  case 'd':
    unlink(fpath);
    break;

  default:
    break;
  }

  return 0;
}

/* ----------------------------------------------------- */
/* �O�D���						 */
/* ----------------------------------------------------- */

int
post_guard_dog()
{
  char fpath[64];
#ifdef POPUP_ANSWER
  char *menu[] =
  {
    "BQ",
    "BBSdog  BBS�ݪ����p�e",
    "Post    �峹���e����",
    "Read    �ݪO�\\Ū����",
    "Write   �ݪO�o�孭��",
    "List    �ݪO�C�X����",
    "Vlog    �פ�log�O��",
    NULL
  };
#else
  char *menu = "�� (B)�ݪ����p�e  ����(P)�峹���e (R)�\\Ū (W)�o�� (L)�C�X (V)�פ�O���H[Q] ";
#endif

  sprintf(fpath, BBSHOME"/gem/@/@BBS_DOG_WARN");
  more(fpath, NULL);

  clear();
#ifdef POPUP_ANSWER
  switch (pans(3, 20, "BBS �ݪ���", menu))
#else
  switch (vans(menu))
#endif
  {
  case 'b':
    return post_bbs_dog();
  case 'p':
    return post_article_filter();
  case 'r':
    return post_my_level(0);
  case 'w':
    return post_my_level(1);
  case 'l':
    return post_my_level(2);
  case 'v':
    return post_view_bbs_dog_log();
  }
  return 0;
}
#endif


#if 0
int
post_manage(xo)
  XO *xo;
{
  BRD *brd;
#ifdef POPUP_ANSWER
  char *menu[] =
  {
    "BQ",
    "BTitle  �ק�ݪO�D�D",
    "WMemo   �s��i�O�e��",
    "Manager �W��ƪO�D",
#  ifdef HAVE_MODERATED_BOARD
    "Level   ���}/�n��/���K",
    "OPal    �O�ͦW��",
#  endif
#  ifdef POST_PREFIX
    "Prefix  �]�w�峹���O",
#  endif
    "ASpam   �ݪO�׫H�C��",
#  ifdef HAVE_MODERATED_BOARD
    "VPal    �i�_�[�ݪO�ͦW��",
#  endif
#  ifdef HAVE_SCORE
    "Score   �]�w�i�_����",
#  endif
    "RLock   �O�ͥi�_���",
    "Nfward  �ݪO�T�����",
    "Fshow   ����O���}��",
#ifdef DO_POST_FILTER
    "Guard   BBS �ݪ���",
#endif
    NULL
  };
#else
  char *menu = "�� �O�D��� (B)�D�D (W)�i�O (S)�׫H (M)�ƪO"
#  ifdef HAVE_MODERATED_BOARD
    " (L)�v�� (O)�O��"
#  endif
#  ifdef POST_PREFIX
    " (P)���O"
#  endif
    " (A)�׫H"
#  ifdef HAVE_SCORE
    " (S)����"
#  endif
#  ifdef HAVE_MODERATED_BOARD
    " (V)�W��"
#  endif
    " (R)��� (N)��� (F)����"
#ifdef DO_POST_FILTER
    " (G)�ݪ���"
#endif
    "�H[Q] ";
#endif

  if (!(bbstate & STAT_BOARD))
    return XO_HEAD;

  vs_bar("�O�D�޲z");
  brd = bshm->bcache + currbno;
  prints("�ݪO�W�١G%s\n�ݪO�����G[%s] %s\n�O�D�W��G%s\n",
    brd->brdname, brd->class, brd->title, brd->BM);
  prints("����ԭz�G%s\n", brd->title);
#ifdef HAVE_MODERATED_BOARD
  prints("�ݪO�v���G%s�ݪO\n", brd->readlevel == PERM_SYSOP ? "���K" : brd->readlevel == PERM_BOARD ? "�n��" : "���}");
#endif

#ifdef POPUP_ANSWER
  switch (pans(3, 20, "�O�D���", menu))
#else
  switch (vans(menu))
#endif
  {
  case 'b':
    return post_brdtitle(xo);

  case 'w':
    return post_memo_edit(xo);

  case 'a':
    return post_spam_edit(xo);

  case 'm':
    return post_changeBM(xo);

#ifdef HAVE_SCORE
  case 's':
    return post_battr_noscore(xo);
#endif

#ifdef HAVE_MODERATED_BOARD
  case 'l':
    return post_brdlevel(xo);

  case 'o':
    return XoBM(xo);

  case 'v':
    return post_vpal(xo);
#endif

#ifdef POST_PREFIX
  case 'p':
    return post_brd_prefix(xo);
#endif
  case 'r':
    return post_rlock(xo);
  case 'n':
    return post_noforward(xo);
  case 'f':
    return post_showturn(xo);
#ifdef DO_POST_FILTER
  case 'g':
    return post_guard_dog(xo);
#endif
  }

  return XO_HEAD;
}
#endif
