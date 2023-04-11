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
 * @file  PathRead.hh
 * @brief  static method to return the files in a directory
 *         
 * @class  PathRead
 * @brief  static method to return the files in a directory
 */

#ifndef PATH_READ_H
#define PATH_READ_H

#include <vector>
#include <string>

//----------------------------------------------------------------
class PathRead
{
public:

  /**
   * @return the files that are in a path
   *
   * @param[in] path  Path to look in
   * 
   * The returned strings are the file names, without the full path.
   * Can be either directories or files
   */
  static std::vector<std::string> filesInPath(const std::string &path);

protected:
private:

  
};

#endif
