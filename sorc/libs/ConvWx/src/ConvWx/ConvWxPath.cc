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
 * @file ConvWxPath.cc
 */

#include <cstdio>
#include <sys/stat.h>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ConvWxPath.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//------------------------------------------------------------------
/**
 * Utility routine to create a directory.  If the directory
 * already exists, does nothing.
 *
 * @param[in] path  Directory to create
 *
 * @return true for success
 */

static bool sMakedir(const std::string &path);

//------------------------------------------------------------------
/**
 * Utility routine to create a directory recursively.
 * If the directory already exists, does nothing.
 * Otherwise it recurses through the path, making all
 * needed directories.
 *
 * @param[in] path  The full path to create
 *
 * @return true for success
 */
static bool sMakedirRecurse(const std::string &path);


/**
 * @return true if a path exists
 * @param[in] path
 */
static bool sExists(const std::string &path);

//------------------------------------------------------------------
ConvWxPath::ConvWxPath(const std::string &fullPath) :
  pDelimiter("/"),
  pDot(".")
{
  pSetPath(fullPath);
}
  
//------------------------------------------------------------------
ConvWxPath::~ConvWxPath(void)
{
}

//------------------------------------------------------------------
bool ConvWxPath::makeDirRecurse(void)
{
  if (pDirExists())
  {
    return(true);
  }
  else
  {
    return(sMakedirRecurse(pDirectory));
  }
}

//------------------------------------------------------------------
void ConvWxPath::pSetPath( const std::string &fullPath)
{
  pClear();
  if ( !fullPath.empty() )
  {
    pParse(fullPath);
  }
}

//------------------------------------------------------------------
void ConvWxPath::pClear(void)
{
  pPath = "";
  pDirectory = "";
  pFile = "";
  pBase = "";
  pExt = "";
}

//------------------------------------------------------------------
void ConvWxPath::pParse( const string &newPath )
{
  size_t delimPos = newPath.rfind( pDelimiter );
  size_t length = newPath.length();

  if ( newPath  == ".." ||  newPath  == "." || delimPos == length-1 )
  {
    //
    // The path is just a directory name (no file name)
    //
    pDirectory = newPath;
  }
  else
  {
    //
    // We've got a file name
    //
    if ( delimPos > length )
    {
      //
      // The path is just a file name (current directory implied)
      //
      pFile = newPath;
      pDirectory = "";
    } 
    else
    {
      //
      // This is a composite name (directory and file)
      //
      pFile = newPath.substr(delimPos+1);
      pDirectory = newPath.substr(0, delimPos);
    }
  }

  //
  // Compose the full path from the results of our parsing
   //
  pCompose();
}

//------------------------------------------------------------------
void ConvWxPath::pCompose(void)
{
  pPath = "";

  if (pDirectory.empty())
  {
    pPath = pFile;
  }
  else if (pFile.empty())
  {
    pPath = pDirectory;
  }
  else
  {
    pPath = pDirectory + pDelimiter + pFile;
  }

  // extension

  pExt = "";
  size_t dotPos = pFile.rfind( "." );
  if (dotPos != string::npos)
  {
    pExt.assign(pFile, dotPos + 1, string::npos);
    pBase.assign(pFile, 0, dotPos);
  }
  else
  {
    pBase = pFile;
  }
}

//------------------------------------------------------------------
bool ConvWxPath::pDirExists(void) const
{
  // empty dir must exist - it is the current directory
  if (pDirectory.empty() )
  {
    return true;
  }
  return(sExists(pDirectory));
}

//------------------------------------------------------------------
static bool sMakedir(const std::string &path)
{
  struct stat statBuf;
  
  // Status the directory to see if it already exists.
  if (stat(path.c_str(), &statBuf) == 0)
  {
    return(true);
  }
  
  // Directory doesn't exist, create it.
  if (mkdir(path.c_str(), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) != 0)
  {
    // failed. check if dir has been made by some other process
    // in the mean time, in which case return success.
    if (stat(path.c_str(), &statBuf) == 0)
    {
      return(true);
    }
    else
    {
      return(false);
    }
  }
  else
  {
    return(true);
  }
}

//------------------------------------------------------------------
static bool sMakedirRecurse(const std::string &path)
{
  string upDir;
  string::size_type lastDelim;
  struct stat dirStat;

  string delim = convWx::FILE_DELIMITER;
  
  // Status the directory to see if it already exists.
  // '/' dir will always exist, so this stops the recursion
  // automatically.
  if (stat(path.c_str(), &dirStat) == 0)
  {
    return(true);
  }

  // create up dir - one up the directory tree -
  // by searching for the previous delim and removing it
  // from the string.
  // If no delim, try to make the directory non-recursively.
  upDir = path;
  lastDelim = upDir.rfind(delim);
  if (lastDelim == string::npos)
  {
    return (sMakedir(upDir));
  }

  upDir = upDir.substr(0, lastDelim);
  
  // make the up dir
  if (!sMakedirRecurse(upDir))
  {
    return false;
  }

  // make this dir
  return sMakedir(path);
}

//------------------------------------------------------------------
static bool sExists(const std::string &path)
{
  if (path.empty())
  {
    return false;
  }
  struct stat dirStat;
  if (stat(path.c_str(), &dirStat) == 0)
  {
    return(true);
  }
  else
  {
    return false;
  }
}

