/* smiler.090416: ���{���Ω󰻴������� bshm �B�@�欰�ð��A���ɱ� */

#include "bbs.h"


int
bshm_work(userid, funcname)
  char *userid;
  char *funcname;
{

#ifdef	DEBUG_BSHM_WORK 

  FILE *fp;
  char cmd[64];
  time_t now;
  struct tm *ptime;

  BCACHE *bshm_debug;  
  int i;
  bshm_debug = shm_new(BRDSHM_KEY, sizeof(BCACHE));
  
  if (bshm_debug->number > 0)    /* smiler.090416: �Ȯɩw�q bshm_debug->number ==0 �ɬ������`�{�H�A�h���U�~����� */
    return 0;
  
  if (fp = fopen("BSHM_WORK", "a"))
  {
     time(&now);
     ptime = localtime(&now);
     
     fprintf(fp, "%02d/%02d/%02d %02d:%02d:%02d  pid = %-8d  %-13s  %s\n", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec,
             getpid(), userid, funcname);
     fclose(fp);
  }
  
  sprintf(cmd, "kill -15 %d", getpid());
  system(cmd);
  return 0;
  
#endif

}
