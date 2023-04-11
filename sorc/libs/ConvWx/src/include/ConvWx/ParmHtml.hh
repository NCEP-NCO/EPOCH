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
 * @file ParmHtml.hh
 * @brief Parameters for HTML logging output
 * @class ParmHtml
 * @brief Parameters for HTML logging output
 */

# ifndef    PARMHTML_HH
# define    PARMHTML_HH

//----------------------------------------------------------------
class ParmHtml
{
public:

  /**
   * Empty (invalid) constructor
   */
  ParmHtml(void);

  /**
   * Constructor with all parameters passed in.
   *
   * @param[in] path    Path to top level HTML output 
   * @param[in] subdir    Subdirectory of path in which to write HTML
   * @param[in] hoursBack  Number of hours to put into 'current' HTML file
   *
   * @note the HTML is written to 'path/subdir'
   */
  ParmHtml(const std::string &path, const std::string &subdir,
	   const int hoursBack);

  /**
   * Destructor
   */
  virtual ~ParmHtml(void);


  std::string pPath;    /**< Top level path to HTML content */
  std::string pSubdir;  /**< subdirectory for this particular HTML*/
  int pHoursBack;       /**< hours to put into 'current' HTML file. */
  
protected:
private:  
};

# endif     // ParmHtml_HH
