/*-------------------------------------------------------*/
/* xyz.c	( NTHU CS MapleBBS Ver 3.10 )		 */
/*-------------------------------------------------------*/
/* target : ���C���K���~��				 */
/* create : 01/03/01					 */
/* update :   /  /  					 */
/*-------------------------------------------------------*/


#include "bbs.h"

extern UCACHE *ushm;
extern char * const compile_time;

#ifdef HAVE_TIP

/* ----------------------------------------------------- */
/* �C��p���Z						 */
/* ----------------------------------------------------- */

int
x_tip()
{
  int i, j;
  char msg[128];
  FILE *fp;

  if (!(fp = fopen(FN_ETC_TIP, "r")))
    return XEASY;

  fgets(msg, 128, fp);
  j = atoi(msg);		/* �Ĥ@��O���`�g�� */
  i = time(0) % j + 1;
  j = 0;

  while (j < i)			/* ���� i �� tip */
  {
    fgets(msg, 128, fp);
    if (msg[0] == '#')
      j++;
  }

  move(12, 0);
  clrtobot();
  fgets(msg, 128, fp);
  prints("\033[1;36m�C��p���Z�G\033[m\n");
  prints("            %s", msg);
  fgets(msg, 128, fp);
  prints("            %s", msg);
  vmsg(NULL);
  fclose(fp);
  return 0;
}
#endif	/* HAVE_TIP */


#ifdef HAVE_LOVELETTER 

/* ----------------------------------------------------- */
/* ���Ѳ��;�						 */
/* ----------------------------------------------------- */

int
x_loveletter()
{
  FILE *fp;
  int start_show;	/* 1:�}�l�q */
  int style;		/* 0:�}�Y 1:���� 2:���� */
  int line;
  char buf[128];
  char header[3][5] = {"head", "body", "foot"};	/* �}�Y�B����B���� */
  int num[3];

  /* etc/loveletter �e�q�O#head ���q�O#body ��q�O#foot */
  /* ��ƤW���G#head����  #body�K��  #foot���� */

  if (!(fp = fopen(FN_ETC_LOVELETTER, "r")))
    return XEASY;

  /* �e�T��O���g�� */
  fgets(buf, 128, fp);
  num[0] = atoi(buf + 5);
  num[1] = atoi(buf + 5);
  num[2] = atoi(buf + 5);

  /* �M�w�n��ĴX�g */
  line = time(0);
  num[0] = line % num[0];
  num[1] = (line >> 1) % num[1];
  num[2] = (line >> 2) % num[2];

  vs_bar("���Ѳ��;�");

  start_show = style = line = 0;

  while (fgets(buf, 128, fp))
  {
    if (*buf == '#')
    {
      if (!strncmp(buf + 1, header[style], 4))  /* header[] ���׳��O 5 bytes */
	num[style]--;

      if (num[style] < 0)	/* �w�g fget ��n�諸�o�g�F */
      {
	outc('\n');
	start_show = 1;
	style++;
      }
      else
      {
	start_show = 0;
      }
      continue;
    }

    if (start_show)
    {
      if (line >= (b_lines - 5))	/* �W�L�ù��j�p�F */
	break;

      outs(buf);
      line++;
    }
  }

  fclose(fp);
  vmsg(NULL);

  return 0;
}
#endif	/* HAVE_LOVELETTER */

/* ----------------------------------------------------- */
/* ��ܨt�θ�T                                          */
/* ----------------------------------------------------- */

int
x_sysinfo()
{
  vs_bar("�t�θ�T");
  move(2, 0);
  prints("�z�{�b��� \033[1;37;41m�i\033[1;33m " BBSNAME " \033[1;37;41m�j\033[m (" MYIPADDR ")\n"
         "�u�W�A�ȤH��: %d/%d\n"
	 "�sĶ�ɶ�:     %s\n",
	 ushm->count, MAXACTIVE,
	 compile_time);

  vmsg(NULL);
  return 0;
}

/* ----------------------------------------------------- */
/* �K�X�ѰO�A���]�K�X					 */
/* ----------------------------------------------------- */


int
x_password()
{
  int i;
  ACCT acct;
  FILE *fp;
  char fpath[80], email[60], passwd[PSWDLEN + 1];
  time_t now;

  vmsg("���L�ϥΪ̧ѰO�K�X�ɡA���e�s�K�X�ܸӨϥΪ̪��H�c");

  if (acct_get(msg_uid, &acct) > 0)
  {
    time(&now);

    if (acct.lastlogin > now - 86400 * 10)
    {
      vmsg("�ӨϥΪ̥����Q�ѥH�W���W����i���e�K�X");
      return 0;
    }

    vget(b_lines - 2, 0, "�п�J�{�Үɪ� Email�G", email, 40, DOECHO);

    if (str_cmp(acct.email, email))
    {
      vmsg("�o���O�ӨϥΪ̻{�ҮɥΪ� Email");
      return 0;
    }

    if (not_addr(email) || !mail_external(email))
    {
      vmsg(err_email);
      return 0;
    }

    vget(b_lines - 1, 0, "�п�J�u��m�W�G", fpath, RNLEN + 1, DOECHO);
    if (strcmp(acct.realname, fpath))
    {
      vmsg("�o���O�ӨϥΪ̪��u��m�W");
      return 0;
    }

    if (vans("��ƥ��T�A�нT�{�O�_���ͷs�K�X(Y/N)�H[N] ") != 'y')
      return 0;

    sprintf(fpath, "%s ��F %s ���K�X", cuser.userid, acct.userid);
    blog("PASSWD", fpath);

    /* �üƲ��� A~Z �զX���K�X�K�X */
    for (i = 0; i < PSWDLEN; i++)
      passwd[i] = rnd(26) + 'A';
    passwd[PSWDLEN] = '\0';

    /* ���s acct_load ���J�@���A�קK���b vans() �ɵn�J�|���~�����ĪG */
    if (acct_load(&acct, acct.userid) >= 0)
    {
      str_ncpy(acct.passwd, genpasswd(passwd), PASSLEN + 1);
      acct_save(&acct);
    }

    sprintf(fpath, "tmp/sendpass.%s", cuser.userid);
    if (fp = fopen(fpath, "w"))
    {
      fprintf(fp, "%s ���z�ӽФF�s�K�X\n\n", cuser.userid);
      fprintf(fp, BBSNAME "ID : %s\n\n", acct.userid);
      fprintf(fp, BBSNAME "�s�K�X : %s\n", passwd);
      fclose(fp);

      bsmtp(fpath, BBSNAME "�s�K�X", email, 0);
      unlink(fpath);

      vmsg("�s�K�X�w�H��ӻ{�ҫH�c");
    }
  }

  return 0;
}


/* ----------------------------------------------------- */
/* �Y��l�{��						 */
/* ----------------------------------------------------- */
/* ryanlei.081123: ��g�A��ܤ覡�i���G
   1)�X�{���� 2)�v����� 3)��̬ҭn�A
   �æC�X�����`�M�A�����A�Ψ����ȧ@���ѦҡC
   �t��ʱƪ��Ϩ��Y�Ƥ@�P
*/
int
post_sibala(xo)
	XO *xo;
{
	FILE *fp, *fp2, *fp3;
	HDR hdr;
	struct tm *ptime;
	time_t now;
	char fpath[64], buf[64], title[64], folder[64], FP3[64];
	char line[40] = "�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w\n";
	unsigned short i, j, k, num_sibala_face, num_sibala;
	unsigned short *count, mode;  // ryanlei.081123: ���Ʋέp�B��ܼҦ�
	unsigned total = 0;  // �I���`�M
	float mean, Ex;  // ryanlei.081123: �����ơB�����

	sprintf(fpath, "tmp/%s.sibala", cuser.userid);

	for (j = 0; j != 'y';)
	{
    	j = 0;
    	num_sibala = 0;
    	num_sibala_face = 0;

    	fp = fopen(fpath, "w");
		fprintf(fp, "���H   : %s\n", cuser.userid);
		fprintf(fp, "�ӷ�     : %s\n", fromhost);
		fprintf(fp, "IP��}   : %s\n", get_my_ip());

		time(&now);
		ptime = localtime(&now);
		fprintf(fp, "���ɶ� : %02d/%02d/%02d %02d:%02d:%02d\n", 
    		ptime->tm_year % 100, ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min, ptime->tm_sec);

    	clear();
    	move(i = 3, 0);
    	if (!vget(i, 0, "��J���D�W�� : ", title, 60, DOECHO))
			break;
		fprintf(fp, "���D�W�� : %s\n", title);

    	i += 2;
		// ryanlei.081123: ���F����malloc�t���L���A�u���\���ƻ�l����
    	if (!vget(i, 0, "��J��l���� : ", buf, 3, DOECHO) || !(num_sibala_face = atoi(buf)))
      		break;
		fprintf(fp, "��뭱�� : %d\n", num_sibala_face);
		// ryanlei.081123: �έp�U�I�ƥX�{������
		count = malloc( sizeof(unsigned short) * num_sibala_face );

    	i += 2;
    	if (!vget(i, 0, "��J��릸�� : ", buf, 5, DOECHO) || !(num_sibala = atoi(buf)))
      		break;
    	fprintf(fp, "��릸�� : %d\n", num_sibala);

		// ryanlei.081123: 
		// ��ܤ覡 : 1)�X�{���� 2)�v�����G 3)��̬ҭn
		i += 2;
    	vget(i, 0, "��ܤ覡 : 1)�X�{���� 2)�v�����G 3)��̬ҭn [1] ", buf, 2, DOECHO);
		mode = atoi( buf );
		if ( mode != 2 && mode != 3 )
			mode = 1;  // default: 1
		fprintf( fp, "��ܤ覡 : %s\n", 
			mode == 2 ? "�v�����G" :
			mode == 3 ? "��̬ҭn" :
			"�X�{����" );
		fprintf( fp, "\n��뵲�G : \n%s", line );
		fclose( fp );

	    j = vans("Y)��X E)���� Q)�����H[Q] ");

	    if (j != 'y')
    	{
	    	fclose(fp);
    	  	unlink(fpath);
    	}

		if (!j || j == 'q')
      		return XO_HEAD;
	}

	if (!j)	/* �B�z���_���X */
  	{
    	fclose(fp);
    	unlink(fpath);
    	return XO_HEAD;
	}

  	sprintf(folder, "%s.tmp1", fpath);	/* �ɥ� folder */
  	fp2 = fopen( folder, "w" );
  	sprintf( FP3, "%s.tmp2", fpath );
  	fp3 = fopen( FP3, "w" );

  	if ( mode == 3 )  /* ��̬ҭn=>�[���j�u */
	  fprintf( fp3, "%s", line );

	for ( i = 0; i <= num_sibala_face; i++ ) count[ i ] = 0;  // ��l��
	/* ���l�}�l */
	for ( i = 0, srand(time(0)); i < num_sibala; i++ )
  	{
		k = rand() % num_sibala_face + 1;
		count[ k-1 ]++;
		total += k;
		if ( mode != 1 )
    		fprintf(fp3, "�� %4d ����뵲�G : %4d\n", i + 1, k );
  	}
  	fclose( fp3 );

  	if ( mode != 2 )
	{
		fprintf( fp2, "    �I��      �X�{����      ���\n" );
		for ( i = 0; i < num_sibala_face; i++ )
  		{	
			if ( count[ i ] != 0 )  // �X�{1���H�W�~���
			{
	  			fprintf( fp2, "    %4d      %4d       %6.3f%%\n",
	  			i+1, count[ i ], (float)count[ i ]/(float)num_sibala*100.f ); 
	  		}
		}
		fprintf( fp2, "%s�������榸��: %7.2f    %6.3f%%\n", line,
			(float)num_sibala / (float)num_sibala_face, 
			100.f / (float)num_sibala_face );
  	}
	/* �p�⥭���M����� */
  	mean = (float)total / (float)num_sibala;
  	Ex = (float)( num_sibala_face + 1 ) / 2.f;
  	
	fclose(fp2);
  	sprintf(buf, "��맹���I\n");
  	vmsg(buf);

	if ( mode != 2 ) 
	{
		sprintf( buf, "/bin/cat %s >> %s", folder, fpath );
		system(buf);
  	}
  	if ( mode != 1 )
	{
		sprintf( buf, "/bin/cat %s >> %s", FP3, fpath );
		system(buf);
  	}
  
  	/* �`�p */
  	fp = fopen( fpath, "a" );  // �A�}�@����
  	fprintf( fp, "\n�I���`�M : %6d     ����� : %9.2f\n", total, Ex * num_sibala );
  	fprintf( fp, "�I�ƥ��� : %6.2f     ����� : %9.2f\n", mean, Ex );
	fclose( fp );

  	unlink(folder);

	if (!(bbstate & STAT_POST))	/* �b currboard �S���o���v���A�G�H�^�H�c */
  	{
    	usr_fpath(folder, cuser.userid, fn_dir);
    	hdr_stamp(folder, HDR_COPY, &hdr, fpath);
    	strcpy(hdr.owner, "��뵲�G");
    	strcpy(hdr.nick,  "�䯫");
    	hdr.xmode = MAIL_READ | MAIL_NOREPLY;
    	sprintf(hdr.title, "%s : %s", cuser.userid, title);
    	rec_add(folder, &hdr, sizeof(HDR));
    	vmsg("�z�S���b���o��峹���v���A��뵲�G�w�H�^�z���H�c");
  	}
  	else		/* �K�^��ݪO�W */
  	{
    	hdr_stamp(xo->dir, HDR_COPY | 'A', &hdr, fpath);
    	strcpy(hdr.owner, "��뵲�G");
    	strcpy(hdr.nick,  "�䯫");
    	sprintf(hdr.title, "%s : %s", cuser.userid, title);
    	rec_bot(xo->dir, &hdr, sizeof(HDR));
    	btime_update(brd_bno(currboard));
  	}

  	/* �K�� log �O���O�� */
  	brd_fpath(folder, "log", ".DIR");

  	hdr_stamp(folder, HDR_COPY | 'A', &hdr, fpath);
  	strcpy(hdr.owner, "��뵲�G");
  	strcpy(hdr.nick,  "�䯫");
  	sprintf(hdr.title, "%s : %s", cuser.userid, title);
  	rec_bot(folder, &hdr, sizeof(HDR));
  	btime_update(brd_bno("log"));

  	free( count );
  	unlink(fpath);

  	return XO_INIT;
}
