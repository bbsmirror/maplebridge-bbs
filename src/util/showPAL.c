/*-------------------------------------------------------*/
/* util/showPAL.c	( NTHU CS MapleBBS Ver 3.10 )	 */
/*-------------------------------------------------------*/
/* target : ��� friend/pal ���			 */
/* create : 03/05/24					 */
/* update :   /  /  					 */
/* author : itoc.bbs@bbs.tnfsh.tn.edu.tw                 */
/*-------------------------------------------------------*/


#include "bbs.h"


static inline void
showPAL(pal)
  PAL *pal;
{
  printf("> ------------------------------------------------------------------------------------------ \n"
    " ID : %s\n no : %d\n���� %s\n���Y: %s\n", 
    pal->userid, pal->userno, pal->ship,
    (pal->ftype & PAL_BAD) ? "�a�H" : (pal->ftype & PAL_MATE) ? "�S��n��" : "�@��");
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  int pos;
  char *fname;
  PAL pal;

  if (argc < 2)
  {
    printf("Usage: %s .PAL_path\n", argv[0]);
    exit(1);
  }

  fname = argv[1];
#if 0
  if (strcmp(fname + strlen(fname) - 4, FN_DIR))
  {
    printf("This is not a .DIR file.\n");
    exit(1);
  }
#endif

  pos = 0;
  while (!rec_get(fname, &pal, sizeof(PAL), pos))
  {
    showPAL(&pal);
    pos++;
  }

  printf("> ------------------------------------------------------------------------------------------ \n");
}
