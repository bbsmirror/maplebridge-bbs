/*-------------------------------------------------------*/
/* util/mergebrd.c	( NTHU CS MapleBBS Ver �����毸) */
/*-------------------------------------------------------*/
/* target : M3 ���� �ݪO�X�ֵ{��			 */
/* create : 08/05/09					 */
/* author : smiler.bbs@lexel.twbbs.org			 */
/*-------------------------------------------------------*/
/* syntax : mergebrd board_src1 board_src2 board_dst	 */
/*-------------------------------------------------------*/


#include "bbs.h"
#include <netinet/in.h>

#define FN_DIR_TMP	"./.DIR.tmp"
#define FN_DIR_TMP2

/* ----------------------------------------------------- */
/* �ഫ�D�{��						 */
/* ----------------------------------------------------- */


static void
trans_hdr(old, new, src, dst)
  HDR *old;
  HDR *new;
  char *src;
  char *dst;
{
  memset(new, 0, sizeof(HDR));

  new->chrono = old->chrono;
  new->xmode  = old->xmode;
  new->xid    = old->xid;
  str_ncpy(new->xname, old->xname, sizeof(new->xname));
  new->parent_chrono = old->parent_chrono;
  str_ncpy(new->owner, old->owner, sizeof(new->owner));
  new->stamp = old->stamp;
  str_ncpy(new->nick, old->nick, sizeof(new->nick));
  new->score = old->score;
  str_ncpy(new->date, old->date, sizeof(new->date));
  str_ncpy(new->title, old->title, sizeof(new->title));

//  printf("%s %s %d\n",src,new->title,new->chrono);

  FILE *fd;
  char dst_f[64];
  char cmd[64];
  sprintf(dst_f,BBSHOME"/brd/%s/%c/%s",dst,new->xname[7],new->xname);
  if(fd = fopen(dst_f,"r"))
  {
	  new->xname[0]='X';
	  fclose(fd);
  }
  sprintf(cmd,"cp "BBSHOME"/brd/%s/%c/%s "BBSHOME"/brd/%s/%c/%s",src,old->xname[7],old->xname,dst,new->xname[7],new->xname);
  system(cmd);
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *fp1, *fp2;
  char buf[64];
  char buf1[64];
  char buf2[64];
  char buf3[64];

  char src1[15];
  char src2[15];
  char dst[15];

  if (argc != 4)
  {
    printf("syntax: ./mergebrd �ӷ��ݪO1 �ӷ��ݪO2 �ت��ݪO \n");
    return 0;
  }

  strcpy(src1,argv[1]);
  strcpy(src2,argv[2]);
  strcpy(dst,argv[3]);


  if ((!strcmp(src1,dst)) || (!strcmp(src2,dst)))
  {
    printf("�ӷ��ݪO�ݻP�ت��ݪO�۲� !!\n");
    return 0;
  }

  char start;
  printf("�� Y ��}�l�ݪO merge�A��l���N�����} \n");
  scanf("%c",&start);
  if (start != 'Y')
    return 0;

  sprintf(buf, BBSHOME"/brd");
  chdir(buf);

  HDR hdr1,hdr2,hdr3;

  sprintf(buf1, BBSHOME"/brd/%s/" FN_DIR, src1);
  sprintf(buf2, BBSHOME"/brd/%s/" FN_DIR, src2);
  sprintf(buf3, BBSHOME"/brd/%s/" FN_DIR, dst);

  if (!(fp1 = fopen(buf1, "r")))
  {
    printf("%s �}�ɥ���.\n", buf1);
    return 1;
  }
  if (!(fp2 = fopen(buf2, "r")))
  {
    fclose(fp1);
    printf("%s �}�ɥ���.\n", buf2);
    return 1;
  }

  fread(&hdr1, sizeof(hdr1), 1, fp1);
  fread(&hdr2, sizeof(hdr2), 1, fp2);

  int empty = 0;

  while (!empty)
  {
    while ((hdr1.chrono <= hdr2.chrono) && (!empty))
    {
//    printf("1.%s %d %s %d\n",hdr1.title,hdr1.chrono,hdr2.title,hdr2.chrono);
      trans_hdr(&hdr1, &hdr3, src1, dst);
      rec_add(buf3, &hdr3, sizeof(HDR));
      if (fread(&hdr1, sizeof(hdr1), 1, fp1) != 1)
      {
	empty = 1;
	break;
      }
    }
    while ((hdr1.chrono > hdr2.chrono) && (!empty))
    {
//    printf("2.%s %d %s %d\n",hdr1.title,hdr1.chrono,hdr2.title,hdr2.chrono);
      trans_hdr(&hdr2, &hdr3, src2, dst);
      rec_add(buf3, &hdr3, sizeof(HDR));
      if (fread(&hdr2, sizeof(hdr2), 1, fp2) != 1)
      {
	empty = 2;
	break;
      }
    }
    if (!empty)
    {
      trans_hdr(&hdr1, &hdr3, src1, dst);
      rec_add(buf3, &hdr3, sizeof(HDR));
      if (fread(&hdr1, sizeof(hdr1), 1, fp1) != 1)
      {
	empty = 1;
	break;
      }
    }
  }

  if (empty == 2)
  {
    trans_hdr(&hdr1, &hdr3, src1, dst);
    rec_add(buf3, &hdr3, sizeof(HDR));
    while (fread(&hdr1, sizeof(hdr1), 1, fp1) == 1)
    {
      trans_hdr(&hdr1, &hdr3, src1, dst);
      rec_add(buf3, &hdr3, sizeof(HDR));
    }
  }
  else if (empty == 1)
  {
    trans_hdr(&hdr2, &hdr3, src2, dst);
    rec_add(buf3, &hdr3, sizeof(HDR));
    while (fread(&hdr2, sizeof(hdr2), 1, fp2) == 1)
    {
      trans_hdr(&hdr2, &hdr3, src2, dst);
      rec_add(buf3, &hdr3, sizeof(HDR));
    }
  }

  fclose(fp1);
  fclose(fp2);

  return 0;
}
