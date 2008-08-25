/*-------------------------------------------------------*/
/* util/autopost_to_board.c	( NTHU CS MapleBBS Ver 3.20 )*/
/*-------------------------------------------------------*/
/* target : MapleBBS �۰� post �t��	 */
/* create : 08/08/25					 */
/* author : smiler.bbs@bbs.cs.nthu.edu.tw */
/* update : //					 */
/*-------------------------------------------------------*/


#include "bbs.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sysexits.h>


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

static void
add_post(brdname, fpath, title, userid, usernick)           /* �o���ݪO */
  char *brdname;        /* �� post ���ݪO */
  char *fpath;          /* �ɮ׸��| */
  char *title;          /* �峹���D */
  char *userid;
  char *usernick;
{
  HDR hdr;
  char folder[64];
  char buf[64];

  sprintf(buf, BBSHOME"/");
  chdir(buf);

  brd_fpath(folder, brdname, ".DIR");


  hdr_stamp(folder, HDR_COPY | 'A', &hdr, fpath);
  strcpy(hdr.owner, userid);
  strcpy(hdr.nick, usernick);
  strcpy(hdr.title, title);
  rec_bot(folder, &hdr, sizeof(HDR));

  //btime_update(brd_bno(brdname));
}


int
main(argc, argv)
  int argc;
  char *argv[];
{

  char filepath[64];
  char owner[76];
  char nick[49];
  char title[73];
  FILE *fp;

  if (argc < 5)
  {
	//                     1        2        3        4        
    printf("Usage:\t%s <brdname> <userid> <usernick> <use>\n", argv[0]);
    exit(-1);
  }

  init_bshm();

  /* <use> �� RSS_POST_TO_BBS �� */
  if(!strcmp(argv[4],"RSS_POST_TO_BBS"))
  {

	  /* smiler.080825: �^�� title from ./RSS_POST_TITLE */
	  sprintf(filepath, BBSHOME"/brd/%s/.RSS_POST_TITLE" ,argv[1]);
	  if(fp = fopen(filepath,"r"))
	  {
		  fgets (title , 70 , fp);
		  fclose(fp);
	  }
	  else
	    strcpy(title,"iMaple RSS Feed");

	  printf("%s\n",argv[4]);
	  sprintf(filepath, BBSHOME"/brd/%s/.RSS_POST" ,argv[1]);
	  //str_ncpy(owner, argv[2], 73-3);
	  sprintf(owner,"%s.",argv[2]);
	  str_ncpy(nick, argv[3], 49-3);
  }

  add_post(argv[1], filepath, title, owner, nick);

  return 0;

}
