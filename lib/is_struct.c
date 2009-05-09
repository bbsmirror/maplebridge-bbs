#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>


int
is_struct(fpath, struct_size)	/* 1:�� file �� struct �զ� 0:�L���ɮ� -1:�_ */
  char *fpath;
  size_t struct_size;
{
  struct stat st;

  if (!stat(fpath, &st))
    return (st.st_size % struct_size) ? -1 : 1;
  return 0;
}
