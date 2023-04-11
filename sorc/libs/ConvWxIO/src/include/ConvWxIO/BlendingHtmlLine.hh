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
 * @file   BlendingHtmlLine.hh
 * @brief  One line in a blending HTML file.
 * @class  BlendingHtmlLine
 * @brief  One line in a blending HTML file.  
 * 
 * Used in BlendingHtml class.
 * Controls writing out of HTML content that gives status of several
 * blending apps including blending itself, phase correction, and model 
 * availability
 */

#ifndef BLENDING_HTML_LINE_H
#define BLENDING_HTML_LINE_H
#include <string>
#include <vector>


namespace blendingHtml
{
  /**
   * Bad value for latency (when the latency is not set)
   */
  const int badLatencyHours = -1;

  /**
   * Current HTML file name (fixed)
   */
  const std::string currentFname = "current.html";

  /**
   * Index file name (fixed)
   */
  const std::string indexFname = "index.html";

  /**
   * HTML table start key
   */
  const std::string tableKey = "<TABLE BORDER ALIGN=\"LEFT\" "
    "cellspacing=2 cellpadding=3 style=T1>";

  /** 
   * HTML table end key
   */
  const std::string tableEndKey = "</TABLE>";

  /**
   * HTML table title 
   */
  const std::string tableTitle = "<TR ALIGN=\"LEFT\" "
    "VALIGN=\"TOP\">\n<TH>Archives</TH>\n</TR>";


  /**
   * Tag for beginning of a line
   */
  const std::string tableLineBegin = "<TR ALIGN=\"LEFT\" VALIGN=\"TOP\">\n";
  /**
   * Tag for end of a line
   */
  const std::string tableLineEnd = "</TR>\n";

  const int pcNumColumn = 10;
  const std::string pcHeader[10] = {"R.T.",
				    "T.T.",
				    "Status",
				    "Model Gen",
				    "Model Latency",
				    "Model Missing",
				    "UV Gen",
				    "UV status",
				    "Out Gen",
				    "Out Missing"};

  const int modelNumColumn = 10;
  const std::string modelHeader[10] = {"R.T",
				       "T.T",
				       "Gen",
				       "Missing",
				       "Gen",
				       "Missing",
				       "Gen",
				       "Missing",
				       "Gen",
				       "Missing"};

  const int meanBlendingNumColumn = 14;
  const std::string meanBlendingHeader[14] = {"R.T",
					      "T.T",
					      "Status",
					      "G.T.",
					      "Model Missing",
					      "Extrap Missing",
					      "Weight Gentime",
					      "Model Weight Missing",
					      "Extrap Weight Missing",
					      "Initiation Prob. G.T.",
					      "Initiation Prob. Missing",
					      "Initiation Mask. G.T.",
					      "Initiation Mask. Missing",
					      "Out Missing"};

  const int rapidMeanBlendingNumColumn = 15;
  const std::string rapidMeanBlendingHeader[15] = {"R.T",
						   "T.T",
						   "Status",
						   "Extrap Gentime",
						   "Extrap Missing",
						   "Model Gentime",
						   "Model Missing",
						   "Weight Gentime",
						   "Model Weight Missing",
						   "Extrap Weight Missing",
						   "Initiation Prob. G.T.",
						   "Initiation Prob. Missing",
						   "Initiation Mask. G.T.",
						   "Initiation Mask. Missing",
						   "Out Missing"};

  const int medianBlendingNumColumn = 8;
  const std::string medianBlendingHeader[8] = {"R.T",
					       "T.T",
					       "Status",
					       "G.T.",
					       "Model Missing",
					       "Extrap Missing",
					       "Mask Missing",
					       "Out Missing"};
  const int rapidMedianBlendingNumColumn = 10;
  const std::string rapidMedianBlendingHeader[10] = {"R.T",
						    "T.T",
						    "Status",
						    "Extrap Missing",
						    "Mask Missing",
						    "Model Gentime",
						    "Model Missing",
						    "Initiation Mask. G.T.",
						    "Initiation Mask. Missing",
						    "Out Missing"};
}


class FcstState;
class FcstWithLatencyState;
class UvHtml;
class LpcState;

//----------------------------------------------------------------
class BlendingHtmlLine
{
public:

  /**
   * @enum Format_t
   * @brief The types of HTML that are supported
   */
  typedef enum
  {
    PC=0,                /**< Phase Correction */
    MODEL=1,             /**< Status of model input */
    BLEND_MEAN=2,        /**< Status of mean blending */
    BLEND_MEDIAN=3,      /**< Status of median blending */
    RAPID_BLEND_MEAN=4,
    RAPID_BLEND_MEDIAN=5
  } Format_t;

  /**
   * Constructor: A generic output line (one message).
   *
   * @param[in] t  Trigger time
   * @param[in] msg  The message
   * @param[in] color  The color for the message, empty for none
   * @param[in] format  The message type
   */
  BlendingHtmlLine(const time_t &t, const std::string &msg,
		   const std::string &color, Format_t format);

  /**
   * Constructor: A generic output line (one message).
   *
   * @param[in] t trigger time
   * @param[in] msg  The message
   * @param[in] format  The message type
   */
  BlendingHtmlLine(const time_t &t, const std::string &msg,
		   Format_t format);

  /**
   * Constructor: output line for BlendingHtmlLine::BLEND_MEAN
   * with the initiation probability, and no initiation mask
   *
   * @param[in] t   The data gen time
   * @param[in] model   The state of the model data used for blending
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] wtime  The weights data gen time
   * @param[in] mweight  The state of the model weight data used for blending
   * @param[in] eweight  The state of the extrapolation weight data used
   *                     for blending
   * @param[in] itime  Gen time for initiation probability data
   * @param[in] iprob  The state of the initiation probability data used
   *                   for blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing   The informational string for output missing data
   */
  BlendingHtmlLine(const time_t &t, const FcstState &model,
		   const FcstState &extrap, const time_t &wtime,
		   const FcstState &mweight, const FcstState &eweight,
		   const time_t &itime, const FcstState &iprob,
		   const std::string &msg, const std::string &msgColor,
		   const std::string &missing);

  /**
   * Constructor: output line for BlendingHtmlLine::BLEND_MEAN
   * with the initiation mask, and no initiation prob
   *
   * @param[in] t   The data gen time
   * @param[in] itime  Gen time for initiation probability data
   * @param[in] wtime  The weights data gen time
   * @param[in] model   The state of the model data used for blending
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] mweight  The state of the model weight data used for blending
   * @param[in] eweight  The state of the extrapolation weight data used
   *                     for blending
   * @param[in] imask  The state of the initiation mask data used
   *                   for blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing   The informational string for output missing data
   */
  BlendingHtmlLine(const time_t &t, const time_t &itime, const time_t &wtime,
		   const FcstState &model, const FcstState &extrap, 
		   const FcstState &mweight, const FcstState &eweight,
		   const FcstState &imask,
		   const std::string &msg, const std::string &msgColor,
		   const std::string &missing);


  
  /**
   * Constructor: output line for BlendingHtmlLine::BLEND_MEAN
   * with the initiation mask, and the initiation prob
   *
   * @param[in] t   The data gen time
   * @param[in] wtime  The weights data gen time
   * @param[in] probtime  The initiation probability data gen time
   * @param[in] masktime  The initiation mask data gen time
   * @param[in] model   The state of the model data used for blending
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] mweight  The state of the model weight data used for blending
   * @param[in] eweight  The state of the extrapolation weight data used
   *                     for blending
   * @param[in] iprob  The state of the initiation probability data used
   *                   for blending
   * @param[in] imask  The state of the initiation mask data used
   *                   for blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing   The informational string for output missing data
   */
  BlendingHtmlLine(const time_t &t, const time_t &wtime, const time_t &probtime,
		   const time_t &masktime,
		   const FcstState &model, const FcstState &extrap, 
		   const FcstState &mweight, const FcstState &eweight,
		   const FcstState &iprob, const FcstState &imask,
		   const std::string &msg, const std::string &msgColor,
		   const std::string &missing);


  /**
   * Constructor mean blending, initiation probability
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
  BlendingHtmlLine(const time_t &t, const FcstState &extrap, const time_t &mt,
		   const FcstState &model, 
		   const time_t &wt, const FcstState &eweight,
		   const FcstState &mweight, const time_t &ipt, 
		   const FcstState &iprob, const time_t &imt,
		   const FcstState &imask,
		   const std::string &msg, const std::string &msgColor,
		   const std::string &missing);



  /**
   * Constructor: output line for BlendingHtmlLine::BLEND_MEAN, but without 
   * the initiation probability input or initiation mask.
   *
   * @param[in] t   The data gen time
   * @param[in] model   The state of the model data used for blending
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] wt  The weights data gen time
   * @param[in] mweight  The state of the model weight data used for blending
   * @param[in] eweight  The state of the extrapolation weight data used
   *                     for blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing   The informational string for output missing data
   */
  BlendingHtmlLine(const time_t &t, const FcstState &model,
  		   const FcstState &extrap, const time_t &wt,
  		   const FcstState &mweight, const FcstState &eweight,
  		   const std::string &msg,
		   const std::string &msgColor, const std::string &missing);

  /**
   * Constructor: output line from BlendingHtmlLine::BLEND_MEDIAN
   * @param[in] t   The data time
   * @param[in] model   The state of the model data used for blending
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] mask  The state of the mask data used for blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing   The informational string for output missing data
   */
  BlendingHtmlLine(const time_t &t, const FcstState &model,
		   const FcstState &extrap, const FcstState &mask,
		   const std::string &msg, const std::string &msgColor,
		   const std::string &missing);

  /**
   * Constructor: output line from BlendingHtmlLine::RAPID_BLEND_MEDIAN
   * @param[in] t   The Extrap data generation time
   * @param[in] extrap  The state of the extrapolation data used for blending
   * @param[in] mask  The state of the mask data used for blending
   * @param[in] modelGenTime  The model data generation time
   * @param[in] model   The state of the model data used for blending
   * @param[in] ciFlagTime  The CI Flag data generation time
   * @param[in] ciFlag  The state of the CI Flag data used for blending
   * @param[in] msg   The main message to be logged
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] missing   The informational string for output missing data
   */
  BlendingHtmlLine(const time_t &t, const FcstState &extrap,
		   const FcstState &mask, const time_t &modelGenTime,
		   const FcstState &model, const time_t &ciFlagTime,
		   const FcstState &ciFlag,
		   const std::string &msg, const std::string &msgColor,
		   const std::string &missing);

  /**
   * Constructor: Output line from BlendingHtmlLine::PC
   * @param[in] t  The data time
   * @param[in] msg   The main message
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] model  State of model input data 
   * @param[in] uv  State of phase correction UV input data
   * @param[in] warningLatencyHours   Minimum latency hours for 'yellow'
   * @param[in] errorLatencyHours   Minimum latency hours for 'red'
   */
  BlendingHtmlLine(const time_t &t, const std::string &msg,
		   const std::string &msgColor,
		   const FcstWithLatencyState &model,
		   const UvHtml &uv, double warningLatencyHours,
		   double errorLatencyHours);

  /**
   * Constructor: Output line from BlendingHtmlLine::PC
   * @param[in] t  The data time
   * @param[in] msg   The main message
   * @param[in] msgColor  Color for the main message, empty for none
   * @param[in] model   State of model input data 
   * @param[in] uv  State of phase correction UV input data
   * @param[in] isRestart  True if the app just restarted
   * @param[in] warningLatencyHours   Minimum latency hours for 'yellow'
   * @param[in] errorLatencyHours   Minimum latency hours for 'red'
   */
  BlendingHtmlLine(const time_t &t, const std::string &msg,
		   const std::string &msgColor, const LpcState &model,
		   const UvHtml &uv, bool isRestart,
		   double warningLatencyHours, double errorLatencyHours);

  /**
   * Constructor: Create a message from pairs of strings
   *
   * @param[in] v  Vector of pairs, first=message, second=color
   * @param[in] format  Expected format of the line
   */
  BlendingHtmlLine(const std::vector<std::pair<std::string,std::string> > &v,
		   Format_t format);

  /**
   * Destructor
   */
  virtual ~BlendingHtmlLine(void);

  /**
   * @return True for well formed class
   */
  inline bool isGood(void) const {return pGood;}
  
  /**
   * @return Current real time value
   */
  inline time_t getRealtime(void) const {return pRealtime;}

  /**
   * Debug print to stdout
   */
  void print(void) const;

  /**
   * @return The HTML content for the line
   */
  std::string getLine(void) const;

  /**
   * @return The day file name for the current real time
   *
   * @param[in] path  Path to prepend to the name
   *
   * The returned string is path + dayfileName()
   */
  std::string dayfileName(const std::string &path) const;

  /**
   * @return The day file name for the current real time
   *
   * @note format is 'yyyymmdd.html'
   */
  std::string dayfileName(void) const;

  /**
   * @return  Current real time 
   *
   * @param[out] year  Current year
   * @param[out] month  Current month
   * @param[out] day  Current day
   */
  void dayfileYmd(int &year, int &month, int &day) const;

  /**
   * @return  A string for the day preceded by an input 
   *
   * @param[in] name  Name to prepend 
   *
   * @note The returned string is name + " " + dayString()
   */
  std::string dayString(const std::string &name) const;

  /**
   * @return A string for the real time day.
   *
   * @note  The string has format 'yyyy/mm/dd'
   */
  std::string dayString(void) const;

  /**
   * @return  The fixed HTML blending header that includes a name
   *
   * @param[in] name  Name to include in the fixed header string
   *
   * @note  This is put at the top of HTML files
   */
  static std::string head(const std::string &name);

  /**
   * @return The fixed HTML blending tail 
   *
   * @note  This is put at the end of HTML files
   */
  static std::string tail(void);

  /**
   * @return  The fixed HTML blending table header line for each format
   *
   * @param[in] format  The format
   */
  static std::string getHeaderLine(Format_t format);

  /**
   * @return  A descriptive string for each format
   * @param[in] format  The format
   */
  static std::string getDescriptor(Format_t format);

  /**
   * @return  The number of table columns for each format
   * @param[in] format  The format
   */
  static int numColumn(Format_t format);

  /**
   * @return  A fixed help file name
   */
  static std::string helpFileName(void);

  /**
   * Create and write out a help file.
   * 
   * @param[in] indexFile  Name of the index file
   * @param[in] filePath  Where the help file is to be written
   * @param[in] name  Name that goes into HTML here and there
   * @param[in] format  The format of the help file
   * @param[in] helpHeader  Header strings for app help (first, second)
   * @param[in] helpContent  App help strings (pairs of first, second)
   *
   * @return  Name of the help file, which was created from inputs,
   *          empty string if no file was created.
   */
  static std::string 
  createHelpFile(const std::string &indexFile, const std::string &filePath,
		 const std::string &name, Format_t format,
		 const std::pair<std::string,std::string> &helpHeader,
		 const std::vector<std::pair<std::string,std::string> >
		 &helpContent);

  /**
   * Parse a string into a returned vector of BlendingHtmlLine
   *
   * @param[in] parseString  String to parse
   * @param[in] format  Expected format of s
   *
   * @return  The vector
   */
  static std::vector<BlendingHtmlLine> parse(const std::string &parseString,
					     Format_t format);

protected:
private:

  /** 
   * Each line is some number of columns, each with a color.
   * The first string is content, second string is the optional color
   */
  std::vector<std::pair<std::string,std::string> > pContent;
  
  time_t pRealtime;    /**<  Real time */
  int pTriggerHour;    /**<  Trigger hour */
  int  pTriggerMinute; /**<  Trigger minute */
  Format_t pFormat;    /**<  Format of line */
  bool pGood;          /**<  Well formed object */

  /**
   * Method called from all constructors, Put hour and minute of input time
   * into state, put current real time into state. 
   * initialize pContent with these fields for these two columns (shared
   * by all BlendingHtml).
   *
   * @param[in] t  Data time
   */
  void pInit(const time_t &t);

  /**
   * @return The number of HTML columns for the local state (pFormat)
   */
  int pNumColumn(void) const;

  /**
   * Add column for latency to pContent (in color if latency is too large)
   *
   * @param[in] msg  String to write that represents latency
   * @param[in] latencyHours  The latency in hours
   * @param[in] warningLatencyHours  Write in yellow if latencyHours
   *                                 exceeds this 
   * @param[in] errorLatencyHours  Write in red if latencyHours
   *                                 exceeds this 
   */
  void pAddLatency(const std::string &msg, double latencyHours,
		   double warningLatencyHours, double errorLatencyHours);

  /**
   * Add column for missing data to pContent, with color determined by
   * content of input string  (red, yellow or no color)
   *
   * @param[in] missingString  Contains missing data information, with
   *                           "none" if nothing is missing, or "all" if
   *                           everything is missing, or a specification of
   *                           exactly what is missing.
   */
  void pAddMissingToContent(const std::string &missingString);
  
  /**
   * Add input string as a column in pContent. If input msg = "none"
   * give it color yellow, otherwise no color.
   *
   * @param[in] msg  The string to add
   */
  void pAddToContentYellowIfNone(const std::string &msg);

  /**
   * Add input string to pContent, no color
   *
   * @param[in] msg  The string to add
   */
  void pAddToContent(const std::string &msg);

  /**
   * Add input string to pContent with input color
   *
   * @param[in] msg  The string to add
   * @param[in] color  The color to use
   */
  void pAddToContent(const std::string &msg, const std::string &color);

};

#endif
