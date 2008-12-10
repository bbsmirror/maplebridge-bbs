/*-------------------------------------------------------*/
/* dictd.c    ( YZU WindTOPBBS Ver 3.10 )                */
/*-------------------------------------------------------*/
/* author : statue.bbs@bbs.yzu.edu.tw                    */
/* target : �r��                                         */
/* create : 01/11/18                                     */
/* update : 08/12/11                                     */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "iconv.h"

#if 0
/*
smiler.080203:
1.�r������\��A�Ш� http://dict.tw/doc/Dict_BSD.htm
  �H�� http://netlab.cse.yzu.edu.tw/~statue/freebsd/zh-tut/dict.html

2.iconv.h �����s�X���D�A�Ш� http://netlab.cse.yzu.edu.tw/~statue/freebsd/zh-tut/converter.html
  �S�O�`�N: gcc -I/usr/local/include -L/usr/local/lib -liconv -o my_iconv my_iconv.c
*/
#endif

//�N�X�ഫ:�q�@�ؽs�X�ର�t�@�ؽs�X
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
  iconv_t cd;
  char **pin = &inbuf;
  char **pout = &outbuf;

  if (!(cd = iconv_open(to_charset, from_charset)))
    return -1;

  memset(outbuf,0,outlen);
  if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
    return -1;
  iconv_close(cd);
  return 0;
}


//big5�ରutf-8�X
int b2u(char *inbuf,int inlen,char *outbuf,int outlen)
{
  return code_convert("big5","utf-8",inbuf,inlen,outbuf,outlen);
}


int
main_dictd()
{
  char word[73];
  char word2[128];
//  char word_big5[256];     //smiler.0202
  char word_utf8[256];
  char tmp[256];
  char fname[64];
  char fname_tmp[64];          //smiler.0202
  uschar *str, *ptr, *pend;

  int is_ch=0;
  int is_eng=0;

  while(1)
  {
    clear();
    move(0, 30);
    outs("\033[1;37;44m�� �h�γ~�r�� ��\033[m");
    move(2, 0);
    outs("���r��ӷ��� FreeBSD �� dict-database �H�� cdict5 �r��C����J�����r�Τ��y�C\n");
    move(4, 0);
    outs("�r��N��   �r�廡��\n");
    outs("============================================================================\n");
    outs("moecomp    Taiwan MOE computer dictionary\n");
    outs("netterm    Network Terminology\n");
    outs("pydict     pydict data\n");
    outs("cedict     Chinese to English dictionary\n");
    outs("web1913    Webster's Revised Unabridged Dictionary (1913)\n");
    outs("wn         WordNet (r) 2.0\n");
    outs("gazetteer  U.S. Gazetteer (1990)\n");
    outs("jargon     Jargon File (4.3.1, 29 Jun 2001)\n");
    outs("foldoc     The Free On-line Dictionary of Computing (27 SEP 03)\n");
    outs("elements   Elements database 20001107\n");
    outs("easton     Easton's 1897 Bible Dictionary\n");
    outs("hitchcock  Hitchcock's Bible Names Dictionary (late 1800's)\n");
    outs("vera       Virtual Entity of Relevant Acronyms (Version 1.9, June 2002)\n");
    outs("devils     THE DEVIL'S DICTIONARY ((C)1911 Released April 15 1993)\n");
    outs("world95    The CIA World Factbook (1995)\n");
    outs("cdict5     Chinese to English and English to Chinese Dictionary\n");
    memset(word, 0, sizeof(word));
    memset(word2, 0, sizeof(word2));

    if (!vget(b_lines, 0, "word: ", word, sizeof(word) - 1, DOECHO))
      return 0;

    str = word;
    ptr = word2;
    pend = str + strlen(word);

    while (str < pend)
    {
      if (*str >= 0x81 && *str < 0xFE && *(str + 1) >= 0x40
	&& *(str + 1) <= 0xFE && *(str + 1) != 0x7F)    /* ����r BIG5+ */
      {
      
        is_ch=1;
      
	if (*(str + 1) == 0x5C)
	{
	  *ptr = *str;
	  *(ptr + 1) = *(str + 1);
	  *(ptr + 2) = 0x5C;
	  ptr++;
	}
	else
	{
	  *ptr = *str;
	  *(ptr + 1) = *(str + 1);
	}
	ptr++;
	str++;
      }
      else
      {
        /* smiler.081211: security reason�A �ȱ�������A�^��A�Ů�Ÿ���J !! */
	if((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || (*str == ' '))
	{
	  *ptr = *str;
	  if(*ptr != ' ')
	    is_eng=1;
	}
	else
	{
	  vmsg("�r��ȱ�������A�^��A�H�ΪŮ�Ÿ��T�ؿ�J !!");
	    return 0;
	}
      }

      if(is_ch == is_eng)
      {
        vmsg("���i�P�ɿ�J���^�� !!");
          return 0;
      }
      
      str++;
      ptr++;
    }

    sprintf(fname_tmp, "tmp/%s.dictd.tmp", cuser.userid);
    sprintf(fname, "tmp/%s.dictd", cuser.userid);


    /* smiler.080203: �ഫ��J �� big5 �ର utf8 */
    b2u(word,strlen(word),word_utf8,256);

    /* dict */
    sprintf(tmp, "/usr/local/bin/dict -h localhost \"%s\" > %s", word_utf8, fname_tmp);
    system(tmp);

    /* UTF-8 �� BIG5 */
    sprintf(tmp, "/usr/local/bin/iconv -f UTF-8 -t big5 %s > %s", fname_tmp, fname);
    system(tmp);

    /* cdict5 */
    sprintf(tmp, BBSHOME"/bin/cdict5/cdict5 \"%s\" >> %s", word, fname);
    system(tmp);

    more(fname, NULL);
    sprintf(tmp, "�O�_�� %s ���d�ߵ��G�H�^�ۤv�H�c�H (y/N) ", word);
    if (cuser.userlevel && vans(tmp) == 'y')
    {
      sprintf(tmp, "[�� �� ��] %s ���r��d�ߵ��G", word);
      mail_self(fname, cuser.userid, tmp, 0);
    }
    unlink(fname_tmp);
    unlink(fname);
  }

  return 0;
}
