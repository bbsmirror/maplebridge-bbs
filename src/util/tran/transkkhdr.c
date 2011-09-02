/*-------------------------------------------------------*/
/* util/transkkhdr.c	( NTHU CS MapleBBS Ver 3.00 )	 */
/*-------------------------------------------------------*/
/* target : KKCity �ݪO .DIR �ഫ			 */
/* create : 10/10/03				 	 */
/* update : 					 	 */
/*-------------------------------------------------------*/


#include "bbs.h"

#if 0

�o��{���ΨӱN KKCity ���ݪO index ���ഫ�� maple(����) �ۮe�Φ��C
�ഫ�� flag ��ӥi�H�b trans_hdr() ���d�ݡA
�ϥΫe�n���}�n�ݪO�A�ñN KKCity ���ݪO�ƥ��ɸm���쥿�T�� ~/brd/<brdname> ��m�A
�M����� transkkhdr <brdname>

#endif

/*-------------------------------------------------------*/
/* �ഫ�{��						 */
/*-------------------------------------------------------*/


#define FN_DIR_TMP	".DIR.tmp"
#define FN_DIR_OLD	".DIR.old"


static void
trans_hdr(board)
  char *board;
{
  int fd;
  HDR old, new;
  char tmp_dir[64], old_dir[64], cur_dir[64];
//  struct stat st;

  brd_fpath(tmp_dir, board, FN_DIR_TMP);
  brd_fpath(old_dir, board, FN_DIR_OLD);
  brd_fpath(cur_dir, board, FN_DIR);
  rename(cur_dir, old_dir);

  if (dashf(tmp_dir))
    unlink(tmp_dir);

  if ((fd = open(old_dir, O_RDONLY)) >= 0)
  {
    while (read(fd, &old, sizeof(HDR)) == sizeof(HDR))
    {
      memset(&new, 0, sizeof(HDR));
      memcpy(&new, &old, sizeof(HDR));

      if (old.xmode & 0x00001000)
      {
	/* �ഫ���ʧ@�b�� */
	new.nick[sizeof(new.nick) - 1] = '\0';
	strcpy(new.mark, "���n");
	new.color = '3';
	new.xmode &= ~0x00001000;
	new.xmode |= POST_BOTTOM;
      }
      else
      {
	new.nick[sizeof(new.nick) - 1] = '\0';
	strcpy(new.mark, "");
	new.color = '\0';
      }

      if (old.xmode & 0x10000000)
      {
	new.xmode &= ~0x10000000;
	new.xmode |= POST_SCORE;
	new.score = *(old.owner + 79);
      }
      else
	new.score = 0;

      if (old.xmode & 0x0004)
      {
	new.xmode &= ~0x0004;
	new.xmode |= POST_GEM;
      }

      if (old.xmode & 0x10000)
      {
	new.xmode &= ~0x10000;
	new.xmode |= POST_RESTRICT;
      }

      if (old.xmode & 0x0200)
      {
	new.xmode &= ~0x0200;
	new.xmode |= POST_INCOME;
      }

      new.owner[75] = '\0';
      new.stamp = 0;
      new.nick[29] = '\0';
      strcpy(new.value, "");
      rec_add(tmp_dir, &new, sizeof(HDR));
    }
    close(fd);
  }

  if (dashf(cur_dir))
    printf("error: %s\n", board);
  else
    rename(tmp_dir, cur_dir);
/*
  printf(" - %s\n", board);
  stat(index, &st);
  printf("      - old: %d\n", st.st_size);
  stat(folder, &st);
  printf("      - new: %d\n\n", st.st_size);
*/
}

/*-------------------------------------------------------*/
/* �D�{��						 */
/*-------------------------------------------------------*/

int
main(argc, argv)
  int argc;
  char *argv[];
{
  int fd;
  BRD brd;

  /* argc == 1 ������O */
  /* argc == 2 ��Y�S�w�O */

  if (argc != 2)
  {
    printf("Usage: %s <target_board>\n", argv[0]);
    exit(-1);
  }

  chdir(BBSHOME);
  if (!dashf(FN_BRD))
  {
    printf("ERROR! Can't open " FN_BRD "\n");
    exit(-1);
  }

  if ((fd = open(FN_BRD, O_RDONLY)) >= 0)
  {
    while (read(fd, &brd, sizeof(BRD)) == sizeof(BRD))
    {
      if (!*brd.brdname)	/* ���O�w�Q�R�� */
	continue;

      if (argc == 1)
      {
	trans_hdr(brd.brdname);
      }
      else if (!str_cmp(brd.brdname, argv[1]))
      {
	trans_hdr(brd.brdname);
	break;
      }
    }
    close(fd);
  }
  return 0;		//  exit(0);
}

