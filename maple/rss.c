/*-------------------------------------------------------*/
/* rss.c	( NTHU CS MapleBBS Ver 3.20 )		 */
/*-------------------------------------------------------*/
/* target : RSS						 */
/* create : 08/08/16					 */
/* update : // 					 */
/* author : smiler.bbs@bbs.cs.nthu.edu.tw		 */
/*-------------------------------------------------------*/

#include "bbs.h"

extern XZ xz[];
extern char xo_pool[];


#define FN_RSS		".RSS"	/* RSS Reader */
//VAR char *fn_rss	INI(FN_RSS);

/* ----------------------------------------------------- */
/* RSS �\���						 */
/* ----------------------------------------------------- */


static int rss_body();
static int rss_head();
static int rss_add();
static rss_currchrono;

static int
rss_cmpchrono(rss)
  RSS *rss;
{
  return rss->chrono == rss_currchrono;
}

static int
rss_init(xo)
  XO *xo;
{
  xo_load(xo, sizeof(RSS));
  return rss_head(xo);
}

static int
rss_load(xo)
  XO *xo;
{
  xo_load(xo, sizeof(RSS));
  return rss_body(xo);
}

static int
rss_item(num, rss)
  int num;
  RSS *rss;
{
	prints("%6d ", num);                                                   //7
	prints("%c",   (rss->xmode & RSS_RESTRICT) ? ')' : ' ');               //1
	if( (rss->xmode & RSS_RESTRICT) && (!(bbstate & STAT_BOARD))  &&  (strcmp(rss->owner,cuser.userid)) )
	{
		prints("%-70.70s","<<��ƫO�K>>");
		prints("\n");
		return 0;
	}
	prints("\033[1;3%dm%s\033[m ", cal_day(rss->date) + 1, rss->date + 3); //6
	prints("%-12.12s ",rss->owner);                                        //13
	prints("%s %s %s %s ",(rss->xmode & RSS_ATOM) ? "ATOM" : "RSS ",       //5*3+3=18
		                 (rss->xmode & RSS_UTF8)  ? "UTF8" : "Big5",
						 (rss->xmode & RSS_TXT)   ? "TXT " : "Html",
						 (rss->xmode & RSS_START) ? "��"   : "��"
		);
	prints("%-33.30s",rss->bookmark);                                      //33
	prints("\n");
	return 0;

}

#ifdef HAVE_LIGHTBAR
static int
rss_item_bar(xo, mode)
  XO *xo;
  int mode;
{
	RSS *rss;
	int num;

	rss = (RSS *) xo_pool + xo->pos - xo->top;
	num = xo->pos + 1;

	prints("%s",mode ? USR_COLORBAR_RSS : "");
    prints("%6d ", num);                                                   //7
	prints("%c",   (rss->xmode & RSS_RESTRICT) ? ')' : ' ');               //1
	if( (rss->xmode & RSS_RESTRICT) && (!(bbstate & STAT_BOARD))  &&  (strcmp(rss->owner,cuser.userid)) )
	{
		prints("%-70.70s","<<��ƫO�K>>");
		prints("%s",mode ? "\033[m" : "");
		return XO_NONE;
	}
	prints("\033[1;3%dm%s\033[m", cal_day(rss->date) + 1, rss->date + 3);  //5
	prints("%s ",mode ? USR_COLORBAR_RSS : "");                            //1
	prints("%-12.12s ",rss->owner);                                        //13
	prints("%s %s %s %s ",(rss->xmode & RSS_ATOM) ? "ATOM" : "RSS ",       //5*3+3=18
		                 (rss->xmode & RSS_UTF8)  ? "UTF8" : "Big5",
						 (rss->xmode & RSS_TXT)   ? "TXT " : "Html",
						 (rss->xmode & RSS_START) ? "��"   : "��"
		);
	prints("%-33.30s",rss->bookmark);                                      //33
    prints("%s",mode ? "\033[m" : "");

	return XO_NONE;

}
#endif

static int
rss_body(xo)
  XO *xo;
{
  RSS *rss;
  int num, max, tail;

  max = xo->max;
  if (max <= 0)
  {
    if (bbstate & STAT_BOARD)
    {
      if (vans("�n�s�W��ƶ�(Y/N)�H[N] ") == 'y')
	     return rss_add(xo);
    }
    else
    {
      vmsg("���ݪO�|�LRSS");
    }
    return XO_QUIT;
  }

  rss = (RSS *) xo_pool;
  num = xo->top;
  tail = num + XO_TALL;
  if (max > tail)
    max = tail;

  move(3, 0);
  do
  {
    rss_item(++num, rss++);
  } while (num < max);
  clrtobot();
  /* return XO_NONE; */
  return XO_FOOT;	/* itoc.010403: �� b_lines ��W feeter */
}

static int
rss_add(xo)
  XO *xo;
{
  RSS rss;
  char buf[2];
  int ans;
  int i;

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  move(3,0);
  clrtobot();

  while(1)
  {
	move(i = 4, 0);
	clrtobot();

	if(!vget(i, 0, "�п�JRSS���ҦW�١G", rss.bookmark, 30 + 1, DOECHO))
	{
		strcpy(rss.bookmark," ");
	}

	i+=2;
    prints("\n�п�J���}�G");
	i++;
    if(!vget(i, 0, "", rss.url, 73 + 1, DOECHO))
	{
		return XO_BODY;
	}
	if( strlen(rss.url)<7  || rss.url[0] != 'h' || rss.url[1] != 't' || rss.url[2] != 't' || rss.url[3] != 'p' ||
		rss.url[4]!=':' || rss.url[5] != '/' || rss.url[6] != '/')
	{
		vmsg("���}�榡�ݬ� http:// �A�Э��s��J !!");
		continue;
	}

	i+=2;

    if(!vget(i, 0, "�ORSS�榡��Atom�榡(�Y���M���п�Atom)�G A)Atom  R)Rss  [A] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"a");
	}

	if((buf[0] == 'R') || (buf[0]=='r'))
		rss.xmode &= (~RSS_ATOM);
	else
		rss.xmode |= RSS_ATOM;

	i+=2;

    if(!vget(i, 0, "�OUTF8�s�X��Big5�s�X�G U)UTF8  B)Big5  [U] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"u");
	}

	if((buf[0] == 'B') || (buf[0]=='b'))
		rss.xmode &= (~RSS_UTF8);
	else
		rss.xmode |= RSS_UTF8;

	i+=2;

	if(!vget(i, 0, "�O�_�n�Nhtml�榡��Ķ��txt�榡�GY)�n N)�_ [Y] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"y");
	}
	if((buf[0] == 'N') || (buf[0]=='n'))
		rss.xmode &= (~RSS_TXT);
	else
		rss.xmode |= RSS_TXT;

	i+=2;

    prints("\n�п�J���������G");
	i++;
    if(!vget(i, 0, "", rss.info, 73 + 1, DOECHO))
	{
		strcpy(rss.info," ");
	}

	i+=2;

	if(!vget(i, 0, "�O�_���æ�RSS�ﶵ�GY)�O N)�_ [N] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"n");
	}
	if((buf[0] == 'Y') || (buf[0]=='y'))
		rss.xmode |= RSS_RESTRICT;
	else
		rss.xmode &= (~RSS_RESTRICT);

	i+=2;

	if(!vget(i, 0, "�O�_�ߧY�ҥ�RSS�GY)�O N)�_ [Y] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"y");
	}
	if((buf[0] == 'N') || (buf[0]=='n'))
		rss.xmode &= (~RSS_START);
	else
		rss.xmode |= RSS_START;

    ans = vans("��ܡG S)�x�s  E)�~��  Q)���} [Q] ");

    if (ans != 'S' && ans != 's' && ans != 'E' && ans != 'e')
      return XO_BODY;
    else if((ans == 'S') || (ans == 's'))
	  break;
  }

  time(&(rss.chrono));
  struct tm *ptime;

  ptime = localtime(&(rss.chrono));
  sprintf(rss.date,"%02d/%02d/%02d",ptime->tm_year % 100, ptime->tm_mon + 1, ptime->tm_mday);

  sprintf(rss.owner,cuser.userid);

  rss.xid=0;                                  /* smiler.080819: �O�d�A�ثe�Ȯɥ��Ψ� */

  ans = vans("�s���m A)�s�W I)���J N)�U�@�� Q)���} [A] ");
  switch (ans)
  {
  case 'q':  
    break;

  case 'i':
  case 'n':

    rec_ins(xo->dir, &rss, sizeof(RSS), xo->pos + (ans == 'n'), 1);
    break;

  default:

    rec_add(xo->dir, &rss, sizeof(RSS));
    break;
  }

  return rss_init(xo);
}




static int
rss_delete(xo)
  XO *xo;
{
  RSS *rss;

  rss = (RSS *) xo_pool + (xo->pos - xo->top);

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  if (vans(msg_del_ny) == 'y')
  {
   
    if (!rec_del(xo->dir, sizeof(RSS), xo->pos, NULL))
      return rss_load(xo);
  }

  return XO_FOOT;
}



static int
rss_head(xo)
  XO *xo;
{
  vs_head(currBM, currboard);
  prints(NECKER_RSS, d_cols, "");
  return rss_body(xo);
}


static int
rss_edit(xo)
  XO *xo;
{
  RSS *frss, mrss;
  int pos,cur,ans,i;
  char buf[2];

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;

  frss = (RSS *) xo_pool + xo->pos - xo->top;
  memcpy(&mrss, frss, sizeof(RSS));

  move(3,0);
  clrtobot();

  while(1)
  {
	move(i = 4, 0);
	clrtobot();

	if(!vget(i, 0, "�п�JRSS���ҦW�١G", mrss.bookmark, 30 + 1, GCARRY))
	{
		strcpy(mrss.bookmark," ");
	}

	i+=2;
    prints("\n�п�J���}�G");
	i++;
    if(!vget(i, 0, "", mrss.url, 73 + 1, GCARRY))
	{
		return XO_BODY;
	}
	if( strlen(mrss.url)<7  || mrss.url[0] != 'h' || mrss.url[1] != 't' || mrss.url[2] != 't' || mrss.url[3] != 'p' ||
		mrss.url[4]!=':' || mrss.url[5] != '/' || mrss.url[6] != '/')
	{
		vmsg("���}�榡�ݬ� http:// �A�Э��s��J !!");
		continue;
	}

	i+=2;

    if(!vget(i, 0, "�ORSS�榡��Atom�榡(�Y���M���п�Atom)�G A)Atom  R)Rss  [A] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"a");
	}

	if((buf[0] == 'R') || (buf[0]=='r'))
		mrss.xmode &= (~RSS_ATOM);
	else
		mrss.xmode |= RSS_ATOM;

	i+=2;

    if(!vget(i, 0, "�OUTF8�s�X��Big5�s�X�G U)UTF8  B)Big5  [U] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"u");
	}

	if((buf[0] == 'B') || (buf[0]=='b'))
		mrss.xmode &= (~RSS_UTF8);
	else
		mrss.xmode |= RSS_UTF8;

	i+=2;

	if(!vget(i, 0, "�O�_�n�Nhtml�榡��Ķ��txt�榡�GY)�n N)�_ [Y] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"y");
	}
	if((buf[0] == 'N') || (buf[0]=='n'))
		mrss.xmode &= (~RSS_TXT);
	else
		mrss.xmode |= RSS_TXT;

	i+=2;

    prints("\n�п�J���������G");
	i++;
    if(!vget(i, 0, "", mrss.info, 73 + 1, GCARRY))
	{
		strcpy(mrss.info," ");
	}

	i+=2;

	if(!vget(i, 0, "�O�_���æ�RSS�ﶵ�GY)�O N)�_ [N] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"n");
	}
	if((buf[0] == 'Y') || (buf[0]=='y'))
		mrss.xmode |= RSS_RESTRICT;
	else
		mrss.xmode &= (~RSS_RESTRICT);

	i+=2;

	if(!vget(i, 0, "�O�_�ߧY�ҥ�RSS�GY)�O N)�_ [Y] ", buf, 1 + 1, DOECHO))
	{
		strcpy(buf,"y");
	}
	if((buf[0] == 'N') || (buf[0]=='n'))
		mrss.xmode &= (~RSS_START);
	else
		mrss.xmode |= RSS_START;

    ans = vans("��ܡG S)�x�s  E)�~��  Q)���} [Q] ");

    if (ans != 'S' && ans != 's' && ans != 'E' && ans != 'e')
      return XO_BODY;
    else if((ans == 'S') || (ans == 's'))
	  break;
  }

  struct tm *ptime;
  ptime = localtime(&(mrss.chrono));
  sprintf(mrss.date,"%02d/%02d/%02d",ptime->tm_year % 100, ptime->tm_mon + 1, ptime->tm_mday);

  if (memcmp(frss, &mrss, sizeof(RSS)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(frss, &mrss, sizeof(RSS));
	rss_currchrono = frss->chrono;
    rec_put(xo->dir, frss, sizeof(RSS), pos, rss_cmpchrono);

    move(3 + cur, 0);
    rss_item(++pos, frss);
  }
  return XO_BODY;
}

static int
rss_url(xo)
  XO *xo;
{
  RSS *frss, mrss;
  int pos,cur;

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;

  frss = (RSS *) xo_pool + xo->pos - xo->top;
  memcpy(&mrss, frss, sizeof(RSS));

  move(b_lines-1, 0);
  clrtoeol();
  prints("�п�J���}�G");
  char buf_url[73];
  strcpy(buf_url,mrss.url);
  if(!vget(b_lines, 0, "", mrss.url, 73 + 1, GCARRY))
	 strcpy(mrss.url,buf_url);


  if( strlen(mrss.url)<7  || mrss.url[0] != 'h' || mrss.url[1] != 't' || mrss.url[2] != 't' || mrss.url[3] != 'p' ||
		mrss.url[4]!=':' || mrss.url[5] != '/' || mrss.url[6] != '/')
  {
		strcpy(mrss.url,buf_url);
  }

  struct tm *ptime;
  ptime = localtime(&(mrss.chrono));
  sprintf(mrss.date,"%02d/%02d/%02d",ptime->tm_year % 100, ptime->tm_mon + 1, ptime->tm_mday);

  if (memcmp(frss, &mrss, sizeof(RSS)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(frss, &mrss, sizeof(RSS));
	rss_currchrono = frss->chrono;
    rec_put(xo->dir, frss, sizeof(RSS), pos, rss_cmpchrono);

    move(3 + cur, 0);
    rss_item(++pos, frss);
  }
  return XO_BODY;
}


static int
rss_title(xo)
  XO *xo;
{
  RSS *frss, mrss;
  int pos,cur;

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  pos = xo->pos;
  cur = pos - xo->top;

  frss = (RSS *) xo_pool + xo->pos - xo->top;
  memcpy(&mrss, frss, sizeof(RSS));

  if(!vget(b_lines, 0, "�п�JRSS���ҦW�١G", mrss.bookmark, 30 + 1, GCARRY))
	 strcpy(mrss.bookmark," ");

  struct tm *ptime;
  ptime = localtime(&(mrss.chrono));
  sprintf(mrss.date,"%02d/%02d/%02d",ptime->tm_year % 100, ptime->tm_mon + 1, ptime->tm_mday);

  if (memcmp(frss, &mrss, sizeof(RSS)) && vans(msg_sure_ny) == 'y')
  {
    memcpy(frss, &mrss, sizeof(RSS));
	rss_currchrono = frss->chrono;
    rec_put(xo->dir, frss, sizeof(RSS), pos, rss_cmpchrono);

    move(3 + cur, 0);
    rss_item(++pos, frss);
  }
  return XO_FOOT;
}

static rss_move(xo)
  XO *xo;
{
  RSS *rss;
  char *dir, buf[40];
  int pos, newOrder;

  if (!(bbstate & STAT_BOARD))
    return XO_NONE;

  pos = xo->pos;
  rss = (RSS *) xo_pool + (pos - xo->top);

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
    if (!rec_del(dir, sizeof(RSS), pos, NULL))
    {
      rec_ins(dir, rss, sizeof(RSS), newOrder, 1);
      xo->pos = newOrder;
      return rss_load(xo);
    }
  }

  return XO_FOOT;
}

static int
rss_mark(xo)
  XO *xo;
{
	RSS *rss;
    int pos, cur, xmode;

	if (!(bbstate & STAT_BOARD))
       return XO_NONE;

    pos = xo->pos;
    cur = pos - xo->top;
    rss = (RSS *) xo_pool + cur;
    xmode = rss->xmode;

    rss->xmode = xmode ^ RSS_RESTRICT;
    rss_currchrono = rss->chrono;
    rec_put(xo->dir, rss, sizeof(RSS), pos, rss_cmpchrono);
    
	return XO_BODY;
}

static int
rss_rss_atom(xo)
  XO *xo;
{
	RSS *rss;
    int pos, cur, xmode;

	if (!(bbstate & STAT_BOARD))
       return XO_NONE;

    pos = xo->pos;
    cur = pos - xo->top;
    rss = (RSS *) xo_pool + cur;
    xmode = rss->xmode;

    rss->xmode = xmode ^ RSS_ATOM;
    rss_currchrono = rss->chrono;
    rec_put(xo->dir, rss, sizeof(RSS), pos, rss_cmpchrono);
    
	return XO_BODY;
}

static int
rss_utf8_big5(xo)
  XO *xo;
{
	RSS *rss;
    int pos, cur, xmode;

	if (!(bbstate & STAT_BOARD))
       return XO_NONE;

    pos = xo->pos;
    cur = pos - xo->top;
    rss = (RSS *) xo_pool + cur;
    xmode = rss->xmode;

    rss->xmode = xmode ^ RSS_UTF8;
    rss_currchrono = rss->chrono;
    rec_put(xo->dir, rss, sizeof(RSS), pos, rss_cmpchrono);
    
	return XO_BODY;
}

static int
rss_html_txt(xo)
  XO *xo;
{
	RSS *rss;
    int pos, cur, xmode;

	if (!(bbstate & STAT_BOARD))
       return XO_NONE;

    pos = xo->pos;
    cur = pos - xo->top;
    rss = (RSS *) xo_pool + cur;
    xmode = rss->xmode;

    rss->xmode = xmode ^ RSS_TXT;
    rss_currchrono = rss->chrono;
    rec_put(xo->dir, rss, sizeof(RSS), pos, rss_cmpchrono);
    
	return XO_BODY;
}

static int
rss_start_idle(xo)
  XO *xo;
{
	RSS *rss;
    int pos, cur, xmode;

	if (!(bbstate & STAT_BOARD))
       return XO_NONE;

    pos = xo->pos;
    cur = pos - xo->top;
    rss = (RSS *) xo_pool + cur;
    xmode = rss->xmode;

    rss->xmode = xmode ^ RSS_START;
    rss_currchrono = rss->chrono;
    rec_put(xo->dir, rss, sizeof(RSS), pos, rss_cmpchrono);
    
	return XO_BODY;
}

static int
rss_browse(xo)
  XO *xo;
{

	RSS *rss;
    rss = (RSS *) xo_pool + (xo->pos - xo->top);

	if((!(bbstate & STAT_BOARD)) && (rss->xmode & RSS_RESTRICT))
	  return XO_NONE;

	move(3,0);
	clrtobot();

	struct tm *ptime;
	ptime = localtime(&(rss->chrono));

	prints("\n���ҦW�١G%s\n"  ,rss->bookmark);
	prints("�K�[��  �G%s\n"  ,rss->owner);
	prints("��l����G%02d/%02d/%2d %2d:%2d:%2d\n"  ,ptime->tm_year % 100, ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);
	prints("��ʤ���G%s\n\n",rss->date);
	prints("RSS ���}�G\n%s\n\n",rss->url);
	prints("���������G\n%s\n\n",rss->info);
	prints("RSS �ݩʡG\n");
	prints("        %s\n",(rss->xmode & RSS_START)    ? "RSS �Ұʤ�"         : "RSS �Ȱ��ϥ�"      );
	prints("        %s\n",(rss->xmode & RSS_ATOM )    ? "Atom �榡"          : "RSS �榡"          );
	prints("        %s\n",(rss->xmode & RSS_UTF8 )    ? "���ɮ׬� UTF8 �s�X" : "���ɮ׬� Big5 �s�X");
	prints("        %s\n",(rss->xmode & RSS_TXT  )    ? "�ର TXT �榡"      : "�O�d��Html�榡"    );
	prints("        %s\n",(rss->xmode & RSS_RESTRICT) ? "���ê��A"           : "���}���A"          );

	vmsg(NULL);
	return XO_BODY;
}


static int
rss_help(xo)
  XO *xo;
{
	return XO_NONE;
}

int rss_main()
{
	XO *xo;
    char rss_fpath[64];
    brd_fpath(rss_fpath,currboard,".rss");
    xz[XZ_RSS - XO_ZONE].xo = xo = xo_new(rss_fpath);
    xo->key = XZ_RSS;
    xo->pos = 0;
    xover(XZ_RSS);
    free(xo);
	return XO_INIT;
}

KeyFunc rss_cb[] =
{
#ifdef HAVE_LIGHTBAR
  XO_ITEM, rss_item_bar,
#endif
  XO_INIT, rss_init,
  XO_LOAD, rss_load,
  XO_HEAD, rss_head,
  XO_BODY, rss_body,

  'a', rss_add,
  'd', rss_delete,
  'm', rss_move,

  'o', rss_mark,
  'p', rss_rss_atom,
  'u', rss_utf8_big5,
  'n', rss_html_txt,
  's', rss_start_idle,

  'r', rss_browse,
  'E', rss_edit,
  'H', rss_url,
  'T', rss_title,
  Ctrl('P'), rss_add,

  'h', rss_help
};
