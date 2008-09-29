/*-------------------------------------------------------*/
/* util/usies-sort.c	( NTHU CS MapleBBS Ver 3.10 )	 */
/*-------------------------------------------------------*/
/* target : brd_usies �g acl-sort �H��A�A�]���{���i�`�p */
/*          �U�ݪO�ϥΤH��                               */
/* create : 01/02/24				 	 */
/* update :   /  /				 	 */
/* author : itoc.bbs@bbs.tnfsh.tn.edu.tw                 */
/*-------------------------------------------------------*/
/* syntax : usies-sort <file>				 */
/*-------------------------------------------------------*/


//#include <stdio.h>
#include "bbs.h"

#define NUM_BADBOARD	100000	/* �C�� 100000 �H�����O�C�J�C�ϥβv */
#define NUM_DROPBOARD	20	/* �C�� 20 �H�����O���C�J�O�� */
#define BRD_TIMES	(BBSHOME "/run/brd_times.log")	/*qazq:�[�J�ݪO�\Ū���Ƭ���*/


static void
usies_sort(fpath)
  char *fpath;
{
  int num;
  char brdname[60], buf[60];
  char *str;
  FILE *fp, *ftimes;

  if (!(fp = fopen(fpath, "r")))
    return;

  ftimes = fopen(BRD_TIMES, "w");

  if (!fgets(brdname, sizeof(buf), fp))	/* �Ĥ@�ӪO */
  {
    fclose(fp);
    return;
  }

  printf("�ݪO           �H��  (�u�έp�֩� %d �H�A�� 0 �H�������b�U�C)\n", NUM_BADBOARD);

  str = brdname;
  while (*++str != ' ')
    ;
  str_ncpy(brdname, brdname, str - brdname + 1);
  num = 1;

  while (fgets(buf, sizeof(buf), fp))
  {
    str = buf;

    while (*++str != ' ')
      ;

    str_ncpy(buf, buf, str - buf + 1);

    if (!strcmp(brdname, buf))	/* �w�g�X�{���O�A�\Ū�H���[�@ */
    {
      num++;
    }
    else			/* �s�X�{���O */
    {
      if ((num >= NUM_DROPBOARD) && (num < NUM_BADBOARD))      	/* �έp�e�@�ӪO�ϥδX�� */
      {
        printf("%-15s%d\n", brdname, num);
        fprintf(ftimes, "%s %d\n", brdname, num);
      }
      strcpy(brdname, buf);
      num = 1;
    }
  }
  if ((num >= NUM_DROPBOARD) && (num < NUM_BADBOARD))		/* �έp�̫�@�ӪO�ϥδX�� */
    printf("%-15s%d\n", brdname, num);
  fclose(fp);
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  if (argc != 2)
  {
    printf("Usage:\t%s file\n", argv[0]);
  }
  else
  {
    usies_sort(argv[1]);	/* "brd_usies.log" */
  }
  exit(0);
}
