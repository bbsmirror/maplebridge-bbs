#!/bin/sh
# �Yshare memory �Qroot���F�A�hreboot

# for freebsd only
for i in `ipcs | grep root | awk '{print $3}'`
do
  if [ $OSTYPE = "FreeBSD" ]; then
         reboot
  fi
done

# Linux �Х� ipcs �� ipcrm shm
