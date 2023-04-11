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
 * @file  BlendingHtml.hh
 * @brief  HTML log messages from blending
 * @class  BlendingHtml
 * @brief  HTML log messages from blending
 *
 * Controls writing out of HTML content that gives status of several
 * blending apps including blending itself, phase correction, and model 
 * availability
 */

#ifndef BLENDING_HTML_H
#define BLENDING_HTML_H
#include <ConvWxIO/BlendingHtmlLine.hh>

class ParmHtml;

//----------------------------------------------------------------
class BlendingHtml
{
public:

  /**
   * Empty constructor does nothing
   */
  BlendingHtml(void);

  /**
   * Constructor
   *
   * @param[in] parms  HTML parameters
   * @param[in] name  Name that shows up in HTML
   * @param[in] format  HTML formats are for for specific supported types
   * @param[in] helpHeader  Header for app help (columns)
   * @param[in] help  Content of app help (rows and columns)
   */
  BlendingHtml(const ParmHtml &parms, const std::string &name,
	       BlendingHtmlLine::Format_t format,
	       const std::pair<std::string,std::string> &helpHeader,
	       const std::vector<std::pair<std::string,std::string> > &help);
    
  /**
   * Destructor
   */
  virtual ~BlendingHtml(void);

  /**
   * @return  The format 
   */
  inline BlendingHtmlLine::Format_t getFormat(void) const {return pFormat;}

  /**
   * Set minimum latency values for 'yellow' and 'red' HTML colors.
   *
   * @param[in] warningLatencyHours  Minimum latency hours for 'yellow'
   * @param[in] errorLatencyHours    Minimum latency hours for 'red'
   */
  void setLatencyThresholds(double warningLatencyHours,
			    double errorLatencyHours);

  /**
   * Log an arbitrary string message
   *
   * @param[in] t  Message time
   * @param[in] msg  The message
   */
  void log(const time_t &t, const std::string &msg) const;

  /**
   * Log an arbitrary string message
   *
   * @param[in] t  Message time
   * @param[in] msg  The message
   * @param[in] color  Color for that message
   */
  void log(const time_t &t, const std::string &msg,
	   const std::string &color) const;

  /**
   * Log the blending algorithm state, mean blending, initiation mask
   * included
   *
   * @param[in] t  Data trigger time
   * @param[in] model  The state of the model data used for blending
   * @param[in] extrap The state of the extrapolation data used for blending
   * @param[in] wt  Extrapolation/model weights gen time
   * @param[in] imaskt  Initiation mask gen time.
   * @param[in] mweight The state of the model weights data used for blending
   * @param[in] eweight The state of the extrapolation weights data used for
   *                    blending
   * @param[in] imask  The state of the initiation mask data used for
   *                   blending
   * @param[in] msg  The main message to be logged
   * @param[in] msgColor Color for the main message, empty for none
   * @param[in] missing  The information for output missing data
   */
  void log(const time_t &t, const FcstState &model, const FcstState &extrap,
	   const time_t &wt, const time_t &imaskt, const FcstState &mweight,
	   const FcstState &eweight, const FcstState &imask,
	   const std::string &msg, const std::string &msgColor,
	   const std::string &missing) const;

  /**
   * Log the blending algorithm state, mean blending, initiation probability
   * included
   *
   * @param[in] t  Data trigger time
   * @param[in] model  The state of the model data used for blending
   * @param[in] extrap The state of the extrapolation data used for blending
   * @param[in] wt  Extrapolation/model weights gen time
   * @param[in] mweight The state of the model weights data used for blending
   * @param[in] eweight The state of the extrapolation weights data used for
   *                    blending
   * @param[in] it  Initiation probability gen time.
   * @param[in] iprob  The state of the initiation probability data used for
   *                   blending
   * @param[in] msg  The main message to be logged
   * @param[in] msgColor Color for the main message, empty for none
   * @param[in] missing  The information for output missing data
   */
  void log(const time_t &t, const FcstState &model, const FcstState &extrap,
	   const time_t &wt, const FcstState &mweight,
	   const FcstState &eweight, const time_t &it, const FcstState &iprob,
	   const std::string &msg, const std::string &msgColor,
	   const std::string &missing) const;

  /**
   * Log the blending algorithm state, mean blending, initiation probability
   * included and initiation mask included
   *
   * @param[in] t  Data trigger time
   * @param[in] model  The state of the model data used for blending
   * @param[in] extrap The state of the extrapolation data used for blending
   * @param[in] wt  Extrapolation/model weights gen time
   * @param[in] mweight The state of the model weights data used for blending
   * @param[in] eweight The state of the extrapolation weights data used for
   *                    blending
   * @param[in] ipt  Initiation probability gen time.
   * @param[in] imt  Initiation mask gen time.
   * @param[in] iprob  The state of the initiation probability data used for
   *                   blending
   * @param[in] imask  The state of the initiation mask data used for
   *                   blending
   * @param[in] msg  The main message to be logged
   * @param[in] msgColor Color for the main message, empty for none
   * @param[in] missing  The information for output missing data
   */
  void log(const time_t &t, const FcstState &model, const FcstState &extrap,
	   const time_t &wt, const FcstState &mweight,
	   const FcstState &eweight, const time_t &ipt, const time_t &imt,
	   const FcstState &iprob, const FcstState &imask,
	   const std::string &msg, const std::string &msgColor,
	   const std::string &missing) const;
  /**
   * Log the blending algorithm state, mean blending, initiation probability
   * included and initiation mask included, rapidblending with independent
   * model gen/leads
   *
   * @param[in] t  Data trigger time
   * @param[in] extrap The state of the extrapolation data used for blending
   * @param[in] mt  Model gen time
   * @param[in] model  The state of the model data used for blending
   * @param[in] wt  Extrapolation/model weights gen time
   * @param[in] eweight The state of the extrapolation weights data used for
   *                    blending
   * @param[in] mweight The state of the model weights data used for blending
   * @param[in] ipt  Initiation probability gen time.
   * @param[in] iprob  The state of the initiation probability data used for
   *                   blending
   * @param[in] imt  Initiation mask gen time.
   * @param[in] imask  The state of the initiation mask data used for
   *                   blending
   * @param[in] msg  The main message to be logged
   * @param[in] msgColor Color for the main message, empty for none
   * @param[in] missing  The information for output missing data
   */
  void log(const time_t &t, const FcstState &extrap, const time_t &mt,
	   const FcstState &model, 
	   const time_t &wt, const FcstState &eweight,
	   const FcstState &mweight, const time_t &ipt, 
	   const FcstState &iprob, const time_t &imt, const FcstState &imask,
	   const std::string &msg, const std::string &msgColor,
	   const std::string &missing) const;

  /**
   * Log the blending algorithm state, mean blending, but without initiation
   * probability input.
   *
   * @param[in] t  Data trigger time
   * @param[in] model  The state of the model data used for blending
   * @param[in] extrap The state of the extrapolation data used for blending
   * @param[in] wt  Weights gen time
   * @param[in] mweight The state of the model weights data used for blending
   * @param[in] eweight The state of the extrapolation weights data used for
   *                    blending
   * @param[in] msg  The main message to be logged
   * @param[in] msgColor Color for the main message, empty for none
   * @param[in] missing  The information for output missing data
   */
  void log(const time_t &t, const FcstState &model, const FcstState &extrap,
  	   const time_t &wt, const FcstState &mweight,
  	   const FcstState &eweight, const std::string &msg,
  	   const std::string &msgColor, const std::string &missing) const;

  /**
   * Log the blending algorithm state, median blending
   *
   * @param[in] t  Data trigger time
   * @param[in] model  The state of the model data used for blending
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] mask  The state of the mask data used in blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing  The information for output missing data
   */
  void log(const time_t & t, const FcstState &model, const FcstState &extrap,
	   const FcstState &mask, const std::string &msg,
	   const std::string &msgColor, const std::string &missing) const;

  /**
   * Log the blending algorithm state, radid median blending
   *
   * @param[in] t  Data trigger time = extrap/mask gen time
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] mask The state of the mask data used for blending
   * @param[in] modelGenTime  Generation time for model input data
   * @param[in] model  The state of the model data used for blending
   * @param[in] ciMaskGenTime  Generation time for CI Flag input data
   * @param[in] ciMask  The state of the CI mask data used in blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing  The information for output missing data
   */
  void log(const time_t & t, const FcstState &extrap, const FcstState &mask,
	   const time_t & modelGenTime, const FcstState &model,
	   const time_t & ciMaskGenTime, const FcstState &ciMask, 
	   const std::string &msg,
	   const std::string &msgColor, const std::string &missing) const;

  /**
   * Log the phase correction apply algorithm state
   * (the actual phase correction of model data)
   *
   * @param[in] t  Data trigger time
   * @param[in] msg  The main message
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] model  State of the model data that is phase corrected
   * @param[in] uv  State of phase correction UV vector data
   */
  void log(const time_t & t, const std::string &msg,
	   const std::string &msgColor, const FcstWithLatencyState &model,
	   const UvHtml &uv) const;

  /**
   * Log the phase correction creation algorithm state
   * (the determination of phase correction vectors)
   *
   * @param[in] t  Data trigger time
   * @param[in] msg  The main message
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] phaseCorrect  State of phase correction
   * @param[in] uv  State of phase correction vector data
   * @param[in] isRestart  True if the app just restarted
   */
  void log(const time_t & t, const std::string &msg,
	   const std::string &msgColor, const LpcState &phaseCorrect,
	   const UvHtml &uv, const bool isRestart) const;

protected:
private:

  std::string pPath;  /**< Top place to write HTML (should exist) */
  std::string pSubdir;/**< Sub directory to write content to (can create) */
  std::string pName;  /**< Name, from the constructor */
  std::string pWorkingPath;  /**< pPath/pSubdir */
  std::string pIndexFile;    /**< The main HTML file used to refer to other
			      *   files */
  std::string pCurrentFile;  /**< The current file - a fixed special name */
  std::string pHelpFile;     /**< The help file name */
  BlendingHtmlLine::Format_t pFormat;  /**< The format */
  int pHoursBack;  /**< Number of hours of data back from the current time
		    * to keep in the current file. */
  double pWarningLatencyHours;  /**< Minimum hours latency for a warning state*/
  double pErrorLatencyHours;  /**< Minimum hours latency for an error state*/

  /**
   * Write one line of data to output HTML files (current and 'dayfile')
   *
   * @param[in] line  The line
   */
  void pStoreNewLine(const BlendingHtmlLine &line) const;

  /**
   * Write multiple lines of data to an output HTML file
   *
   * @param[in] headerName  The name that goes into the header of the HTML file
   * @param[in] lines  The lines to write to the file
   * @param[in] fileName  The name of the file to write to
   */
  void pWriteTable(const std::string &headerName,
		   const std::vector<BlendingHtmlLine> &lines,
		   const std::string &fileName) const;

  /**
   * Write one line of data to an output HTML file
   *
   * @param[in] headerName  The name that goes into the header of the HTML file
   * @param[in] line  The line to write to the file
   * @param[in] fileName  The name of the file to write to
   */
  void pWriteTable(const std::string &headerName, const BlendingHtmlLine &line,
		   const std::string &fileName) const;

  /**
   * Read HTML data from a file, parse it into lines, and add one more line.
   *
   * @param[in] fileName  File to read
   * @param[in] newLine  The new line to add to what was read in
   * @param[in] removeOld  True to remove lines that are read in which are
   *                       older than pHoursBack
   *
   * @return  The lines that were read in, plus the new line
   */
  std::vector<BlendingHtmlLine> pUpdateContent(const std::string &fileName,
					       const BlendingHtmlLine &newLine,
					       const bool removeOld) const;

  /**
   * Read an index file, update as needed to refer to a new line, and
   * write the index file back out after updates.
   *
   * Updates happen if the input line is for a new day that is not in the
   * index file data that was read in.
   *
   * @param[in] line  The line to use for updating
   */
  void pUpdateIndexFile(const BlendingHtmlLine &line) const;

};

#endif
