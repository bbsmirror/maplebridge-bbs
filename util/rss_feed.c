/*-------------------------------------------------------*/
/* util/rss_fed.c	( NTHU CS MapleBBS Ver �����毸)	 */
/*-------------------------------------------------------*/
/* target : M3 ���� rss ���e�{��			 */
/* create : 08/08/23					 */
/* author : smiler.bbs@bbs.cs.nthu.edu.tw		 */
/*-------------------------------------------------------*/
/* syntax : rss_feed				 */
/*-------------------------------------------------------*/


#include "bbs.h"
#include <netinet/in.h>

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
/* ���e�D�{��						 */
/* ----------------------------------------------------- */

static void
send_rss(old, new, brd_name)
  RSS *old;
  RSS *new;
  char *brd_name;
{
  FILE *fp;
  char file_path[64];
  char com_buf[512];

  memset(new, 0, sizeof(RSS));
  strcpy(new->bookmark,old->bookmark);
  new->chrono = old->chrono;
  new->xmode  = old->xmode;
  new->xid    = old->xid;
  strcpy(new->owner,old->owner);
  strcpy(new->date,old->date);
  strcpy(new->url,old->url);
  strcpy(new->info,old->info);
  strcpy(new->code_type,old->code_type);
  strcpy(new->modified,old->modified);

  /* smiler.080823: ���e�e�A���N���e�Ȧs�� BBS<->RSS message �M�� */
  sprintf(file_path, BBSHOME"/brd/%s/"FN_BBS_TO_RSS, brd_name);
  unlink(file_path);
  sprintf(file_path, BBSHOME"/brd/%s/"FN_RSS_TO_BBS, brd_name);
  unlink(file_path);

  if (new->xmode & RSS_RESTART)
    strcpy(new->modified,"start");	/* smiler.080823: �]�w���e�ɡA�Nmodified�M��"start" */

  if (new->xmode & RSS_START)
  {
    /* smiler.080823: �e�X BBS_TO_RSS message  */
    sprintf(file_path, BBSHOME"/brd/%s/"FN_BBS_TO_RSS, brd_name);
    fp = fopen(file_path,"w");

    fprintf(fp,"%s\n",new->owner);

    if (new->bookmark[0]!='\0')
      fprintf(fp,"%s\n",new->bookmark);
    else
      fprintf(fp,"%s\n","RSS");

    fprintf(fp,"%s\n",new->url);

    fprintf(fp,"%s\n",new->modified);

    if (new->xmode & RSS_TXT)
      fprintf(fp,"%c\n",'y');
    else
      fprintf(fp,"%c\n",'n');

    if (new->xmode & RSS_RESTART)
      fprintf(fp,"%c\n",'r');
    else
      fprintf(fp,"%c\n",'n');

    fclose(fp);

    /* smiler.080823: �e�X command */
    sprintf(com_buf, BBSHOME"/bin/rss_b003 %s %s;", BBSHOME, brd_name);
    system(com_buf);

    /* smiler.080823: ���^ RSS_TO_BBS message */
    sprintf(file_path, BBSHOME"/brd/%s/"FN_RSS_TO_BBS, brd_name);
    if (fp = fopen(file_path, "r"))
    {
      fgets(new->modified , 64 , fp);
      fclose(fp);
    }
    else
      strcpy(new->modified, "start");	/* �Y feed->modified ���^�ǡA�hmodified���]��start */
  }

  if (new->xmode & RSS_RESTART)
     new->xmode &= (~RSS_RESTART);	/* �̫�N RSS_RESTART �k0 */
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *fp;
  RSS rss;
  //if (argc > 2)
  //{
  //  printf("Usage: %s [board]\n", argv[0]);
  //  return -1;
  //}

  char buf[64];
  char buf2[64];
  char buf3[64];

  struct dirent *de;
  DIR *dirp;

  sprintf(buf, BBSHOME"/brd");
  chdir(buf);

  dirp = opendir(".");

  while (de = readdir(dirp))
  {
    RSS old;
    int fd;
    char *str;

    str = de->d_name;
    if (*str <= ' ' ||  *str == '.')
      continue;

    if ((argc == 2) && str_cmp(str, argv[1]))
      continue;

    sprintf(buf, "%s/" FN_RSS, str);
    sprintf(buf2, "%s/" FN_RSS_TMP, str);
    sprintf(buf3, "%s/" FN_RSS_BAK, str);

    if ((fp = fopen(buf, "r")))
    {

      char command[64];
      sprintf(command, "cp %s %s\n", buf, buf3);
      system(command);
      fclose(fp);

      fp = fopen(buf3, "r");

      /* smiler.080823: �Y .RSS.ENTRY_NUM ���s�b�A�N���}�ɡA�üg�J0 */
      FILE *fp_entry_num;
      char file_path[64];
      sprintf(file_path, BBSHOME"/brd/%s/"FN_RSS_ENTRY_NUM, str);
      if (!dashf(file_path))
      {
	fp_entry_num = fopen (file_path,"w");
	fprintf(fp_entry_num,"%c",'0');
        fclose(fp_entry_num);
      }

      while (fread(&old, sizeof(old), 1, fp) == 1)
      {
        send_rss(&old, &rss, str);
        rec_add(buf2, &rss, sizeof(RSS));
      }

      fclose(fp);

      /* �R���ª��A��s����W */
      unlink(buf3);
      rename(buf2, buf3);

      /* �N�ƥ��ɼg�^ .RSS */
      unlink(buf);
      rename(buf3,buf);
    }
  }

  closedir(dirp);

  return 0;
}
