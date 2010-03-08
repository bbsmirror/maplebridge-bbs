/*

smiler.090416: ���{���w��֦� ansi code �� string �@�����p���m�γB�z

0. color  => �Ǧ^ strlen_ansi �� trace �L�� ansi ��X
             �Y�� NULL�A�h����


1. encode => ��M data ���S�w�ѽX���m�A�æ^�Ǹ� data �ѽX�e��m
                                                                                
   ex:
      strlen_ansi("\033[1;33m�o�u�O����\033[m", 10, "DECODE", color);
      return 17
      (�n��Mscreen�W��m10�b��string������m)
                                                                                
2. decode => ��M data ���ѽX�e��m�A�æ^�ǸӦ�m�ѽX�����
                                                                                
   ex:
      strlen_ansi("\033[1;33m�o�u�O����\033[m", 10, "DECODE", color);
      return 3
      (�n�p�⦹string�W��10�Ӧ�m�bscreen�W����m)

*/

#include "bbs.h"

int
strlen_ansi(data, c, coder, color)
  char *data;
  int c;
  char *coder;
  char *color;
{
  int type;          /* �B�@�Ҧ�: ENCODE or DECODE */
  int count;         /* �p�� */
  int x;             /* �ثeŪ���� data+x */
  int ch, ansi;
  int len;
  char *str;
  
  if (!strcmp(coder, "ENCODE"))
    type = 0;        /* ENCODE */
  else
    type = 1;        /* DECODE */


  if (!type)
  {
	count = c;
	len = ANSILINELEN;  /* �̤j�e�\���� */
  }
  else
  {
	count = 0;
	len = c;
  }


  ansi = 0;
  x = 0;

  str = data - 1;
  //ch = *str;
  len ++;

  while (len)   /* �̤j len ���� */
  {

    if ((type == 0) && (count <= 0 /*|| ch == '\0'*/) )  /* �w�g�ƨ�Ӧ�m�F�Areturn �� */
       break;
    else if((type == 1) && (x == c))
    {
       x = count;
       break;
    }

    str++;
    len--;
    x++;
    ch = *str;

    if (ch == '\0')                    /* �wŪ�����A�ɨ��Ѿl���� */
    {
      if (!type)
	x = x + count - 1;
      else
        x = count + c - x + 1;
      break;
    }

    if (ansi)
    {
       if (ch == 'm')
	 ansi = 0;

       if (color)
       {
         *color = ch;
         color++;
       }
      
       continue;
    }
    if (ch == KEY_ESC)
    {
       ansi = 1;
       
       if (color)
       {
         *color = ch;
         color++;
       }
       
       continue;
    }

    if (!type)
      count--;                       /* ENCODE �ɤ��� ansi �� */
    else
      count++;                       /* DECODE �ɭp�� strlen - ansi ���� */

  }

  return x;

}