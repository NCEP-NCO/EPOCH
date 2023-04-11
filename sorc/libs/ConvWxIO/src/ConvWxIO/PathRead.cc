// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>

/**
 * @file PathRead.cc
 */
#include <ConvWxIO/PathRead.hh>
#include <toolsa/ReadDir.hh>

//--------------------------------------------------------------------
std::vector<std::string> PathRead::filesInPath(const std::string &path)
{
  ReadDir rdir;
  vector<string> ret;
  if (rdir.open(path.c_str()))
  {
    return ret;
  }
  struct dirent *dp;
  for (dp = rdir.read(); dp != NULL; dp = rdir.read())
  {
    // exclude entries and files beginning with '.'
    if (dp->d_name[0] == '.')
    {
      continue;
    }
    ret.push_back(dp->d_name);
  }
  return ret;
}

