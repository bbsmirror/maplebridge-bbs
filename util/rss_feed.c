/*-------------------------------------------------------*/
/* util/rss_feed.c	( NTHU CS MapleBBS Ver �����毸) */
/*-------------------------------------------------------*/
/* target : M3 ���� rss ���e�{��			 */
/* author : smiler.bbs@bbs.cs.nthu.edu.tw		 */
/* create : 08/08/23					 */
/* update : 09/05/23					 */
/*-------------------------------------------------------*/
/* syntax : rss_feed [brdname]				 */
/*-------------------------------------------------------*/


#include "bbs.h"
#include <netinet/in.h>

#ifdef HAVE_RSS

#define FN_RSS_BAK		".RSS.bak"
#define FN_RSS_TMP		".RSS.tmp"

/* smiler.080823: store entry->id */
#define FN_RSS_ENTRY		".RSS.ENTRY"

/* smiler.080823: store total entry->id number */
#define FN_RSS_ENTRY_NUM	".RSS.ENTRY_NUM"

/* smiler.080823: BBS<->RSS conversation */
#define FN_BBS_TO_RSS		".RSS.BBS_TO_RSS"
#define FN_RSS_TO_BBS		".RSS.RSS_TO_BBS"

#define BIN_FETCH_RSS		"bin/rss_b004"


/* ----------------------------------------------------- */
/* board�Gshm �������P cache.c �ۮe			 */
/* ----------------------------------------------------- */


static BCACHE *bshm;


static void
init_bshm()
{
  /* itoc.030727: �b�}�� bbsd ���e�A���ӴN�n����L account�A
     �ҥH bshm ���Ӥw�]�w�n */

  bshm = shm_new(BRDSHM_KEY, sizeof(BCACHE));

  if (bshm->uptime <= 0)	/* bshm ���]�w���� */
    exit(0);
}


static BRD *
brd_get(bname)
  char *bname;
{
  BRD *bhdr, *tail;

  bhdr = bshm->bcache;
  tail = bhdr + bshm->number;
  do
  {
    if (!str_cmp(bname, bhdr->brdname))
      return bhdr;
  } while (++bhdr < tail);
  return NULL;
}


/* ----------------------------------------------------- */
/* ���e�D�{��						 */
/* ----------------------------------------------------- */

static void
send_rss(rss, brdname)
  RSS *rss;
  char *brdname;
{
  FILE *fp;
  char cmd[256];

  if (rss->xmode & RSS_RESTART)
    strcpy(rss->modified, "start");	/* smiler.080823: �]�w���e�ɡA�N modified �M�� "start" */

  if (rss->xmode & RSS_START)
  {
    /* smiler.080823: �e�X BBS_TO_RSS message  */
    fp = fopen(FN_BBS_TO_RSS, "w");

    fprintf(fp, "%s\n", rss->owner);
    fprintf(fp, "%s\n", (rss->bookmark[0] != '\0') ? rss->bookmark : "RSS");
    fprintf(fp, "%s\n", rss->url);
    fprintf(fp, "%s\n", rss->modified);
    fprintf(fp, "%c\n", (rss->xmode & RSS_TXT) ? 'y' : 'n');
    fprintf(fp, "%c\n", (rss->xmode & RSS_RESTART) ? 'r' : 'n');

    fclose(fp);

    /* smiler.080823: �e�X command */
    sprintf(cmd, BBSHOME "/%s '%s' '%s'", BIN_FETCH_RSS, BBSHOME, brdname);
    system(cmd);

    /* smiler.080823: ���^ RSS_TO_BBS message */
    if (fp = fopen(FN_RSS_TO_BBS, "r"))
    {
      char *ptr;

      fgets(cmd, sizeof(cmd), fp);
      fclose(fp);
      unlink(FN_RSS_TO_BBS);

      if (ptr = strchr(cmd, '\n'))
        *ptr = '\0';
      str_ncpy(rss->modified, cmd, sizeof(rss->modified));
    }
    else
      strcpy(rss->modified, "start");	/* �Y feed->modified ���^�ǡA�h modified ���]�� "start" */
  }

  rss->xmode &= ~RSS_RESTART;		/* �̫�N RSS_RESTART �k 0 */
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *fp;
  char buf[64];
  struct dirent *de;
  DIR *dirp;

#if 0
  if (argc > 2)
  {
    printf("Usage: %s [board]\n", argv[0]);
    return -1;
  }
#endif

  chdir(BBSHOME "/brd");

  init_bshm();

  dirp = opendir(".");

  while (de = readdir(dirp))
  {
    RSS rss;
    char *str;

    str = de->d_name;
    if (*str <= ' ' ||  *str == '.')
      continue;

    if ((argc == 2) && str_cmp(str, argv[1]))
      continue;

    sprintf(buf, BBSHOME "/brd/%s", str);
    chdir(buf);

    if (dashf(FN_RSS))
    {
      if (!dashf(FN_RSS_ENTRY_NUM))
      {		/* smiler.080823: �Y FN_RSS_ENTRY_NUM ���s�b�A�N���}�ɡA�üg�J0 */
	sprintf(buf, "echo '0' > %s", FN_RSS_ENTRY_NUM);
	system(buf);
      }

      f_cp(FN_RSS, FN_RSS_BAK, O_TRUNC);
      fp = fopen(FN_RSS_BAK, "r");

      while (fread(&rss, sizeof(RSS), 1, fp) == 1)
      {
	send_rss(&rss, str);
	rec_add(FN_RSS_TMP, &rss, sizeof(RSS));
      }

      fclose(fp);

      /* �R���ª��A�N�ƥ��ɼg�^ .RSS */
      unlink(FN_RSS);
      unlink(FN_RSS_BAK);
      rename(FN_RSS_TMP, FN_RSS);

      /* ��s�ݪO btime */
      BRD *brd;
      brd = brd_get(str);
      brd->btime = -1;
    }
  }

  closedir(dirp);

  return 0;
}
#endif	/* HAVE_RSS */
