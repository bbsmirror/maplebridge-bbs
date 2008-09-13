/*-------------------------------------------------------*/
/* bank.c	( NTHU CS MapleBBS Ver 3.10 )		 */
/*-------------------------------------------------------*/
/* target : �Ȧ�B�ʶR�v���\��				 */
/* create : 01/07/16					 */
/* update :   /  /  					 */
/* author : itoc.bbs@bbs.tnfsh.tn.edu.tw		 */
/*-------------------------------------------------------*/


#include "bbs.h"

#ifdef HAVE_BUY

static void
x_give()
{
  int way, dollar;
  char userid[IDLEN + 1], buf[80];
  char folder[64], fpath[64], reason[40];
  HDR hdr;
  FILE *fp;
  time_t now;
  PAYCHECK paycheck;

  if (!vget(13, 0, "�z�n����൹�֩O�H", userid, IDLEN + 1, DOECHO))
    return;

  if (acct_userno(userid) <= 0)
  {
    vmsg(err_uid);
    return;
  }

  way = vget(15, 0, "��b 1)��ȹ� 2)������G", buf, 3, DOECHO) - '1';
  if (way < 0 || way > 1)
    return;

  do
  {
    if (!vget(17, 0, "�n��h�ֿ��L�h�H", buf, 9, DOECHO))	/* �̦h�� 99999999 �קK���� */
      return;

    dollar = atoi(buf);

    if (!way)
    {
      if (dollar > cuser.money)
	dollar = cuser.money;	/* ����L�h */
    }
    else
    {
      if (dollar > cuser.gold)
	dollar = cuser.gold;	/* ����L�h */
    }
  } while (dollar <= 1);	/* ����u�� 1�A�|���ܤ���O */

  if (!vget(19, 0, "�п�J�z�ѡG", reason, 40, DOECHO))
    strcpy(reason, "���Ӧh");

  sprintf(buf, "�O�_�n��b�� %s %s�� %d (Y/N)�H[N] ", userid, !way ? "��" : "��", dollar);
  if (vget(21, 0, buf, fpath, 3, LCECHO) == 'y')
  {
    if (!way)
      cuser.money -= dollar;
    else
      cuser.gold -= dollar;

    dollar -= dollar / 10 + ((dollar % 10) ? 1 : 0);	/* 10% ����O */

    /* itoc.020831: �[�J�׿��O�� */
    time(&now);
    sprintf(buf, "%-13s�൹ %-13s�p %d %s (%s)\n",
      cuser.userid, userid, dollar, !way ? "��" : "��", Btime(&now));
    f_cat(FN_RUN_BANK_LOG, buf);

    usr_fpath(folder, userid, fn_dir);
    if (fp = fdopen(hdr_stamp(folder, 0, &hdr, fpath), "w"))
    {
      fprintf(fp, "%s %s (%s)\n���D: ��b�q��\n�ɶ�: %s\n\n", 
	str_author1, cuser.userid, cuser.username, Btime(&now));
      fprintf(fp, "%s\n�L���z�ѬO�G%s\n\n�бz�ܪ��Ĥ��߱N�䲼�I�{", buf, reason);
      fclose(fp);      

      strcpy(hdr.title, "��b�q��");
      strcpy(hdr.owner, cuser.userid);
      rec_add(folder, &hdr, sizeof(HDR));
    }

    memset(&paycheck, 0, sizeof(PAYCHECK));
    time(&paycheck.tissue);
    if (!way)
      paycheck.money = dollar;
    else
      paycheck.gold = dollar;
    sprintf(paycheck.reason, "[��b] %s", cuser.userid);
    usr_fpath(fpath, userid, FN_PAYCHECK);
    rec_add(fpath, &paycheck, sizeof(PAYCHECK));

    sprintf(buf, "�z���W���ȹ� %d ���A���� %d ��", cuser.money, cuser.gold);
    vmsg(buf);
  }
  else
  {
    vmsg("�������");
  }
}


#define GOLD2MONEY	90000	/* �������ȹ� �ײv */
#define MONEY2GOLD	110000	/* �ȹ������� �ײv */

static void
x_exchange()
{
  int way, gold, money;
  char buf[80], ans[8];

  move(13, 0);
  prints("�ȹ������� = %d�G1  �������ȹ� = 1�G%d", MONEY2GOLD, GOLD2MONEY);

  way = vget(15, 0, "�קI 1)�ȹ������� 2)�������ȹ��G", ans, 3, DOECHO) - '1';

  if (!way)
    money = cuser.money / MONEY2GOLD;
  else if (way == 1)
    money = cuser.gold;
  else
    return;

  if (!way)
    sprintf(buf, "�z�n�N�ȹ��I�����h�֭Ӫ����O�H[1 - %d] ", money);
  else
    sprintf(buf, "�z�n�I���h�֭Ӫ��������ȹ��O�H[1 - %d] ", money);
    
  if (!vget(17, 0, buf, ans, 4, DOECHO))	/* ���פ���u�A�קK���� */
    return;

  gold = atoi(ans);
  if (gold <= 0 || gold > money)
    return;

  if (!way)
  {
    if (gold > (INT_MAX - cuser.gold))
    {
      vmsg("�z���Ӧh���o��|���쪺�I");
      return;
    }
    money = gold * MONEY2GOLD;
    sprintf(buf, "�O�_�n�I���ȹ� %d �� ������ %d (Y/N)�H[N] ", money, gold);
  }
  else
  {
    money = gold * GOLD2MONEY;
    if (money > (INT_MAX - cuser.money))
    {
      vmsg("�z���Ӧh���o��|���쪺�I");
      return;
    }
    sprintf(buf, "�O�_�n�I������ %d �� ���ȹ� %d (Y/N)�H[N] ", gold, money);
  }

  if (vget(19, 0, buf, ans, 3, LCECHO) == 'y')
  {
    if (!way)
    {
      cuser.money -= money;
      addgold(gold);
    }
    else
    {
      cuser.gold -= gold;
      addmoney(money);
    }
    sprintf(buf, "�z���W���ȹ� %d ���A���� %d ��", cuser.money, cuser.gold);
    vmsg(buf);
  }
  else
  {
    vmsg("�������");
  }
}


static void
x_cash()
{
  int fd, money, gold;
  char fpath[64], buf[64];
  FILE *fp;
  PAYCHECK paycheck;

  usr_fpath(fpath, cuser.userid, FN_PAYCHECK);
  if ((fd = open(fpath, O_RDONLY)) < 0)
  {
    vmsg("�z�ثe�S���䲼���I�{");
    return;
  }

  usr_fpath(buf, cuser.userid, "cashed");
  fp = fopen(buf, "w");
  fputs("�H�U�O�z���䲼�I���M��G\n\n", fp);

  money = gold = 0;
  while (read(fd, &paycheck, sizeof(PAYCHECK)) == sizeof(PAYCHECK))
  {
    if (paycheck.money < (INT_MAX - money))	/* �קK���� */
      money += paycheck.money;
    else
      money = INT_MAX;
    if (paycheck.gold < (INT_MAX - gold))	/* �קK���� */
      gold += paycheck.gold;
    else
      gold = INT_MAX;

    fprintf(fp, "%s %s %d �� %d ��\n", 
      Btime(&paycheck.tissue), paycheck.reason, paycheck.money, paycheck.gold);
  }
  close(fd);
  unlink(fpath);

  fprintf(fp, "\n�z�@�I�{ %d �� %d ��\n", money, gold);
  fclose(fp);

  addmoney(money);
  addgold(gold);

  more(buf, NULL);
  unlink(buf);
}


int
x_bank()
{
  char ans[3];

  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  vs_bar("�H�U�Ȧ�");
  move(2, 0);

  /* itoc.011208: �H���U�@ */
  if (cuser.money < 0)
    cuser.money = 0;
  if (cuser.gold < 0)
    cuser.gold = 0;

  outs("\033[1;36m  �~������������������������������������������������������������\n");
  prints("  ��\033[32m�z�{�b���ȹ� \033[33m%12d\033[32m ���A���� \033[33m%12d\033[32m ��\033[36m        ��\n", 
    cuser.money, cuser.gold);
  outs("  �ࢤ����������������������������������������������������������\n"
    "  �� �ثe�Ȧ洣�ѤU�C�X���A�ȡG                               ��\n"
    "  ��\033[33m1.\033[37m ��b -- ��b����L�H   (��� 10% ����O) \033[36m              ��\n"
    "  ��\033[33m2.\033[37m �קI -- �ȹ�/���� �I�� (��� 10% ����O) \033[36m              ��\n"
    "  ��\033[33m3.\033[37m �I�{ -- �䲼�I�{                         \033[36m              ��\n"
    "  ��������������������������������������������������������������\033[m");

  vget(11, 0, "�п�J�z�ݭn���A�ȡG", ans, 3, DOECHO);
  if (ans[0] == '1')
    x_give();
  else if (ans[0] == '2')
    x_exchange();
  else if (ans[0] == '3')
    x_cash();

  return 0;
}


int
b_invis()
{
  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  if (cuser.ufo & UFO_CLOAK)
  {
    if (vans("�O�_�{��(Y/N)�H[N] ") != 'y')
      return XEASY; 
    /* �{���K�O */
  }
  else
  {
    if (HAS_PERM(PERM_CLOAK))
    {
      if (vans("�O�_����(Y/N)�H[N] ") != 'y')
	return XEASY;
      /* ���L�������v���̧K�O */
    }
    else
    {
      //if (cuser.gold < 10)    /* smiler.080613: �N��O10���אּ5000�� */
		if(cuser.money < 5000)
		{
			vmsg("�n 5000 �ȹ��~�����γ�");
			return XEASY;
		}
        if (vans("�O�_�� 5000 �ȹ�����(Y/N)�H[N] ") != 'y')
			return XEASY;
		cuser.money -= 5000;
    }
  }

  cuser.ufo ^= UFO_CLOAK;
  cutmp->ufo ^= UFO_CLOAK;	/* ufo �n�P�B */

  return XEASY;
}


static void
buy_level(userlevel)		/* itoc.010830: �u�s level ���A�H�K�ܰʨ�b�u�W��ʪ��{����� */
  usint userlevel;
{
  if (!HAS_STATUS(STATUS_DATALOCK))	/* itoc.010811: �n�S���Q������w�A�~��g�J */
  {
    int fd;
    char fpath[80];
    ACCT tuser;

    usr_fpath(fpath, cuser.userid, fn_acct);
    fd = open(fpath, O_RDWR);
    if (fd >= 0)
    {
      if (read(fd, &tuser, sizeof(ACCT)) == sizeof(ACCT))
      {
	tuser.userlevel |= userlevel;
	lseek(fd, (off_t) 0, SEEK_SET);
	write(fd, &tuser, sizeof(ACCT));
	vmsg("�z�w�g��o�v���A�Э��s�W��");
      }
      close(fd);
    }
  }
}


int
b_cloak()
{
  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  if (HAS_PERM(PERM_CLOAK))
  {
    vmsg("�z�w�g��L�����ΤF");
  }
  else
  {
    if (cuser.gold < 60)               /* smiler.080613:�쥻1000���אּ60�� */
    {
      vmsg("�n 60 �����~���ʶR�L�������v����");
    }
    else if (vans("�O�_�� 60 �����ʶR�L�������v��(Y/N)�H[N] ") == 'y')
    {
      cuser.gold -= 60;
      buy_level(PERM_CLOAK);
    }
  }

  return XEASY;
}


int
b_mbox()
{
  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  if (HAS_PERM(PERM_MBOX))
  {
    vmsg("�z���H�c�w�g�S���W���F");
  }
  else
  {
    if (cuser.gold < 99)                /* smiler.080613: �ק�쥻��O1000����99�� */
    {
      vmsg("�n 99 �����~���ʶR�H�c�L���v����");
    }
    else if (vans("�O�_�� 99 �����ʶR�H�c�L���v��(Y/N)�H[N] ") == 'y')
    {
      cuser.gold -= 99;
      buy_level(PERM_MBOX);
    }
  }

  return XEASY;
}


int
b_xempt()
{
  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  if (HAS_PERM(PERM_XEMPT))
  {
    vmsg("�z���b���w�g�ä[�O�d�F");
  }
  else
  {
    /* songsongboy.070404: ���~ && 3000 hour */
    if ((cuser.gold < 100) && ((ap_start - cuser.firstlogin < 1825 * 86400) || (cuser.staytime < 3000*60)))
      vmsg("�n�� 100 �����ε��U�����~�B�W���W�L�T�d�p�ɤ~��b���ä[�O�d��");
    else if ((ap_start - cuser.firstlogin >= 1825 * 86400) && (cuser.staytime >= 3000*60))
    {
      buy_level(PERM_XEMPT);
      vmsg("�A���b���w�g��o�ä[�O�d�F�I");
    }
    else if (vans("�O�_�� 100 �����ʶR�b���ä[�O�d�v��(Y/N)�H[N] ") == 'y')
    {
      cuser.gold -= 100;
      buy_level(PERM_XEMPT);
      vmsg("�A���b���w�g��o�ä[�O�d�F�I");
    }
  }

  return XEASY;
}


int
b_xvalid()
{
  
  if (HAS_PERM(PERM_XVALID))
  {
    vmsg("�z���b���w�g�ä[�K�{�ҤF");
  }
  else
  {
    /* songsongboy.070404: �T�~*/
    if (ap_start - cuser.firstlogin < 1095 * 86400)
      vmsg("�n���U���T�~�~��ä[�K�{�ҳ�");
    else
    {
      buy_level(PERM_XVALID);
      vmsg("�A���b���w�g��o�ä[�K�{�ҤF�I");
    }
  }
  return XEASY;
}  


int
b_nthu()
{
  
  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }            
  
  char foo[56];
  char *domain;
  FILE *fp;
  
  str_lower(foo, cuser.email);
  
  domain = (char *) strchr(foo, '@');
  *domain++ = '\0';
    
  /* songsongboy.070404: �P�_�O�_�� NTHU ����*/
  if (!strncmp(domain,"oz.nthu.edu.tw",14) || !strncmp(domain,"mx.nthu.edu.tw",14) || !strncmp(domain,"alumni.nthu.edu.tw",18) )
  {
    if (!HAS_PERM(PERM_XVALID))
      buy_level(PERM_XVALID);
    vmsg("�A���b����o�ä[�K�{�ҤF�I");    
    
    if (acl_has("run/nthumember", foo, domain) > 0)     /* ���H�c�w�g���L�� */
      vmsg("�A�w�g���L 50 �����F��I");
    else if (vans("�A�n�����b����o 50 �����ܡA�Ъ`�N�C�H�u�ள�@��(Y/N)�H[N] ") == 'y')
    {
      while(!(fp = fopen("run/nthumember", "a")));
      fprintf(fp, "%s %s\n", cuser.email, cuser.userid);
      fclose(fp);
      
      cuser.gold += 50;
      vmsg("���ߧA��o 50 �����I");
    }
  }
  else if (!strncmp(domain,"cs.nthu.edu.tw",14))
  {
    if(!HAS_PERM(PERM_XEMPT))
      buy_level(PERM_XVALID);
    vmsg("�A���b����o�ä[�O�d�F�I");
  }
  else
    vmsg("�A���H�c���ŦX����I");
    
  return XEASY;
  
}


int
b_celebrate()
{
  char foo[56];
  char *domain;  
  int fp;
  
  str_lower(foo, cuser.email);
  
  domain = (char *) strchr(foo, '@');
  *domain++ = '\0';    
  
  if (HAS_PERM(PERM_XVALID) && HAS_PERM(PERM_XEMPT))
    vmsg("���b���w�g�����v��");
  else if (acl_has("run/celebrate", foo, domain) > 0)
    vmsg("�A�w�g�ӽйL�F��I");
  else if (vans("�A�n�����b����o VIP �ܡA�Ъ`�N�C�H�u��Φb�@�b��(Y/N)�H[N] ") == 'y')
  {
    while(!(fp = fopen("run/celebrate", "a")));
    fprintf(fp, "%s %s\n", cuser.email, cuser.userid);
    fclose(fp);
    
    if (!HAS_PERM(PERM_XVALID))
      buy_level(PERM_XVALID);
    vmsg("�A���b����o�ä[�K�{�ҤF�I");
    
    if (!HAS_PERM(PERM_XEMPT))
      buy_level(PERM_XEMPT);
    vmsg("�A���b����o�ä[�O�d�F�I");
                                          
  }
                                        
  return XEASY;  
}                                      


#if 0	/* �������ʶR�۱��\�� */
int
b_purge()
{
  if (HAS_STATUS(STATUS_COINLOCK))
  {
    vmsg(msg_coinlock);
    return XEASY;
  }

  if (HAS_PERM(PERM_PURGE))
  {
    vmsg("�t�Φb�U���w���M�b���ɡA�N�M���� ID");
  }
  else
  {
    if (cuser.gold < 1000)
    {
      vmsg("�n 1000 �����~��۱���");
    }
    else if (vans("�O�_�� 1000 �����۱�(Y/N)�H[N] ") == 'y')
    {
      cuser.gold -= 1000;
      buy_level(PERM_PURGE);
    }
  }

  return XEASY;
}
#endif
#endif	/* HAVE_BUY */
