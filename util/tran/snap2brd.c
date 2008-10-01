/*-------------------------------------------------------*/
/* util/snap2brd.c	( NTHU CS MapleBBS Ver 3.10 )	 */
/*-------------------------------------------------------*/
/* target : M3 BRD �ഫ�{��				 */
/* create : 03/07/09					 */
/* update :   /  /  					 */
/* author : itoc.bbs@bbs.tnfsh.tn.edu.tw		 */
/*-------------------------------------------------------*/


#include "snap.h"
#include <netinet/in.h>

#define FN_BRD_TMP	"./.BRD.tmp"
//#define FN_BRD		"./.BRD"
//char FN_BRD_TMP[]=".BRD.tmp";
//char FN_BRD[]=".BRD";

usint transbattr(usint originbattr);

int main()
{
  boardheader bh;
  BRD brd;
  FILE *fp;
  /*printf("%d %d", sizeof(bh), sizeof(brd));
  return 0;*/
  
  chdir(BBSHOME);/*/�O�o�N�o��^�_*/
  if (!(fp = fopen(FN_BRD, "r")))
  {
    return -1;
  }
  while (fread(&bh, sizeof(bh), 1, fp) == 1)
  {
    if (!*bh.brdname)	/* ���O�w�Q�R�� */
      continue;

    memset(&brd, 0, sizeof(BRD));

    /* �ഫ���ʧ@�b�� */
    str_ncpy(brd.brdname, bh.brdname, sizeof(brd.brdname));
    str_ncpy(brd.class, bh.bclass, sizeof(brd.class));		/* �������έ��] */
    str_ncpy(brd.title, bh.title /*+ 3*/, sizeof(brd.title));	/* ���L "�� " */
    str_ncpy(brd.BM, bh.BM, sizeof(brd.BM));
    brd.bvote = bh.bvote;
    brd.bstamp = ntohl(bh.bstamp);//�t�έY�@�ˤ��Υ[ntohl
    brd.battr = transbattr(ntohl(bh.battr));
    brd.readlevel = ntohl(bh.readlevel);
    brd.postlevel = ntohl(bh.postlevel);
    if( brd.readlevel == 0x80000000)
    {
      printf("%s\n",brd.brdname);	/*debug*/
      brd.readlevel = brd.readlevel | 0xF8000000;
      brd.postlevel = brd.postlevel | 0xF8000000;
    }
    brd.btime = ntohl(bh.btime);
    brd.bpost = ntohl(bh.bpost);
    brd.blast = 0;

    rec_add(FN_BRD_TMP, &brd, sizeof(BRD));
  }

  fclose(fp);

  /* �R���ª��A��s����W */
  unlink(FN_BRD);
  rename(FN_BRD_TMP, FN_BRD);

  return 0;
}

usint transbattr(usint originbattr)
{
  usint newbattr=originbattr;
  if(originbattr & 0x40 != 0)
  {
    newbattr &= (~0x40);/*BRD_VIP�w�L;*/
  }
  newbattr |= 0x80;
}
