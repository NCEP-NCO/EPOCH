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
 * @file ConvWxPath.hh
 * @brief  Class to create all directories in a path
 * @class ConvWxPath
 * @brief  Class to create all directories in a path
 *
 * The full path is typically top/dir_1/dir_2/.../dir_n/file_base.file_extension
 *
 * All the directories are created as needed, recursively, by the call to
 * makeDirRecurse
 *
 */

#ifndef CONVWX_PATH_HH
#define CONVWX_PATH_HH

#include <string>

class ConvWxPath
{
public:

  /**
   * Constructor - store information needed about the path
   * @param[in] fullPath  Path for a file, where the very last thing
   *                      is a file name.
   */
  ConvWxPath(const std::string &fullPath);
  
  /**
   * Destructor
   */
  virtual ~ConvWxPath(void);

  /**
   * Make the directory recursively.
   * All parent directories will be recursively created.
   *
   * @return true for success and the path exists
   */
  bool  makeDirRecurse(void);

protected:
private:
  
  std::string pDelimiter;  /**< path delimiter = "/" */
  std::string pDot;        /**< The delimiter between base and extension "." */

  std::string pPath;       /**< The full path */
  std::string pDirectory;  /**< The path except for the file */
  std::string pFile;       /**< The part of the path that is the file */
  std::string pBase;       /**< The base part of the file name */
  std::string pExt;        /**< The extension part of the file name */

  /**
   * Assign values to pPath, pDirectory, pFile, pBase and pExt from an input
   *
   * @param[in] fullPath the string to parse.
   *
   * If the input is empty, these members are all assigned the empty string
   */
  void pSetPath(const std::string &fullPath);

  /**
   * Assign empty string values to pPath, pDirectory, pFile, pBase and pExt
   */
  void pClear(void);

  /**
   * Parse a string to assign values to pPath, pDirectory, pFile, pBase and
   * pExt from an input.
   *
   * @param[in] newPath the string to parse, non-empty on input
   */
  void pParse(const std::string &newPath);

  /**
   * Finish up assignments to pPath, pDirectory, pFile, pBase and pExt by
   * deriving some values from others
   */
  void pCompose(void);

  /**
   * @return true if pDirectory exists
   */
  bool pDirExists(void) const;

};

#endif
