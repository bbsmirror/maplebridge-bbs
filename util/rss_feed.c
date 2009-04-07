/*-------------------------------------------------------*/
/* util/rss_fed.c	( NTHU CS MapleBBS Ver �����毸) */
/*-------------------------------------------------------*/
/* target : M3 ���� rss ���e�{��			 */
/* create : 08/08/23					 */
/* author : smiler.bbs@bbs.cs.nthu.edu.tw		 */
/*-------------------------------------------------------*/
/* syntax : rss_feed					 */
/*-------------------------------------------------------*/


#include "bbs.h"
#include <netinet/in.h>

#ifdef HAVE_RSS

#define FN_RSS_BAK		"./.RSS.bak"	
#define FN_RSS_TMP		"./.RSS.tmp"

/* smiler.080823: store entry->id */
#define FN_RSS_ENTRY		"./.RSS.ENTRY"

/* smiler.080823: store total entry->id number */
#define FN_RSS_ENTRY_NUM	"./.RSS.ENTRY_NUM"

/* smiler.080823: BBS<->RSS conversation */
#define FN_BBS_TO_RSS		"./.RSS.BBS_TO_RSS"
#define FN_RSS_TO_BBS		"./.RSS.RSS_TO_BBS"


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
  
  system(FN_BIN_ACCOUNT);       /* smiler.090408: �ǳƦn bshm */

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
  char file_path[64];
  char cmd[256];

  /* smiler.080823: ���e�e�A���N���e�Ȧs�� BBS<->RSS message �M�� */
  sprintf(file_path, "%s/" FN_BBS_TO_RSS, brdname);
  unlink(file_path);
  sprintf(file_path, "%s/" FN_RSS_TO_BBS, brdname);
  unlink(file_path);

  if (rss->xmode & RSS_RESTART)
    strcpy(rss->modified, "start");	/* smiler.080823: �]�w���e�ɡA�N modified �M�� "start" */

  if (rss->xmode & RSS_START)
  {
    /* smiler.080823: �e�X BBS_TO_RSS message  */
    sprintf(file_path, "%s/" FN_BBS_TO_RSS, brdname);
    fp = fopen(file_path, "w");

    fprintf(fp, "%s\n", rss->owner);
    fprintf(fp, "%s\n", (rss->bookmark[0] != '\0') ? rss->bookmark : "RSS");
    fprintf(fp, "%s\n", rss->url);
    fprintf(fp, "%s\n", rss->modified);
    fprintf(fp, "%c\n", (rss->xmode & RSS_TXT) ? 'y' : 'n');
    fprintf(fp, "%c\n", (rss->xmode & RSS_RESTART) ? 'r' : 'n');

    fclose(fp);

    /* smiler.080823: �e�X command */
    sprintf(cmd, BBSHOME "/bin/rss_b004 %s %s;", BBSHOME, brdname);
    system(cmd);

    /* smiler.080823: ���^ RSS_TO_BBS message */
    sprintf(file_path, "%s/" FN_RSS_TO_BBS, brdname);
    if (fp = fopen(file_path, "r"))
    {
      fgets(rss->modified , 64, fp);
      fclose(fp);
    }
    else
      strcpy(rss->modified, "start");	/* �Y feed->modified ���^�ǡA�h modified ���]�� "start" */
  }

  if (rss->xmode & RSS_RESTART)
     rss->xmode ^= RSS_RESTART;		/* �̫�N RSS_RESTART �k 0 */
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *fp;

#if 0
  if (argc > 2)
  {
    printf("Usage: %s [board]\n", argv[0]);
    return -1;
  }
#endif

  char buf[64];
  char buf2[64];
  char buf3[64];

  struct dirent *de;
  DIR *dirp;

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

    sprintf(buf, "%s/" FN_RSS, str);
    sprintf(buf2, "%s/" FN_RSS_TMP, str);
    sprintf(buf3, "%s/" FN_RSS_BAK, str);

    if (dashf(buf))
    {
      char cmd[64];
      char file_path[64];

      /* smiler.080823: �Y .RSS.ENTRY_NUM ���s�b�A�N���}�ɡA�üg�J0 */
      sprintf(file_path, "%s/" FN_RSS_ENTRY_NUM, str);
      if (!dashf(file_path))
      {
	sprintf(cmd, "echo '0' > %s", file_path);
	system(cmd);
      }

      sprintf(cmd, "cp %s %s", buf, buf3);
      system(cmd);

      fp = fopen(buf3, "r");
      while (fread(&rss, sizeof(RSS), 1, fp) == 1)
      {
	send_rss(&rss, str);
	rec_add(buf2, &rss, sizeof(RSS));
      }

      fclose(fp);

      /* �R���ª��A�N�ƥ��ɼg�^ .RSS */
      unlink(buf);
      unlink(buf3);
      rename(buf2, buf);

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
