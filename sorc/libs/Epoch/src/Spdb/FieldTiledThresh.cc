/**
 * @file FieldTiledThresh.cc
 */

#include <Epoch/FieldTiledThresh.hh>
#include <toolsa/LogStream.hh>
#include <cstdio>

//-------------------------------------------------------------
void FieldTiledThresh::print(bool verbose) const
{
  printf("Field:%s", _fieldName.c_str());
  if (verbose)
  {
    printf(" [");
    for (size_t i=0; i<_thresh.size(); ++i)
    {
      printf("%lf ", _thresh[i]);
    }
    printf("]\n");
  }
  else
  {
    printf(" Nthresh:%d\n", (int)_thresh.size());
  }
}
