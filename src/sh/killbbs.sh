#!/bin/sh
# �M�����W�ϥΪ̻Pshared memory

BBSUSER="maple"

kill `ps -auxw | grep bbsd | awk '{print $2}'`

# for freebsd only
for i in `ipcs | grep ${BBSUSER} | awk '{print $3}'`
do
  if [ $OSTYPE = "FreeBSD" ]; then
         ipcrm -M $i
  fi
done

# Linux �Х� ipcs �� ipcrm shm
