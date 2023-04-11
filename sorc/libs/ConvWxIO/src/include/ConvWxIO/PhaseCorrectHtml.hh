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
 * @file PhaseCorrectHtml.hh
 * @brief Methods that allow HTML output when doing Phase Correction
 * @class PhaseCorrectHtml
 * @brief Methods that allow HTML output when doing Phase Correction
 *
 * This is a derived class of BlendingHtml that outputs additional 
 * information when phase correcting is not done (why it was not done)
 */

# ifndef    PHASE_CORRECT_HTML_HH
# define    PHASE_CORRECT_HTML_HH

#include <ConvWxIO/BlendingHtml.hh>

class ParmHtml;
class FcstWithLatencyState;
class UvHtml;
class LpcState;

//----------------------------------------------------------------
class PhaseCorrectHtml : public  BlendingHtml 
{
public:

  /**
   * @enum NoPc_t
   * @brief The reasons for not phase correcting 
   */
  typedef enum
  {
    TERMINATE = 0,        /**< the app terminated */
    RESTART = 1,          /**< the app restarted */
    NO_PC_NO_MODEL = 2,   /**< no model data */
    NO_PC_NO_INPUT_PC = 3 /**< no input phase correction vectors */
  } NoPc_t;

  /**
   * Constructor
   * @param[in] html  The parameters used to construct with
   */
  PhaseCorrectHtml(const ParmHtml &html);

  /**
   * Destructor
   */
  virtual ~PhaseCorrectHtml(void);

  /**
   * Log inputs as HTML
   *
   * @param[in] t  The time
   * @param[in] type  What has happened that generates HTML output
   */
  void log(const time_t &t, const NoPc_t &type) const;

  /**
   * Log inputs as HTML in a red color
   *
   * @param[in] t  The time
   * @param[in] type  What has happened that generates HTML output
   */
  void logRed(const time_t &t, const NoPc_t &type) const;

  /**
   * Log inputs as HTML
   *
   * @param[in] t  The time
   * @param[in] s  The main message to log
   * @param[in] scolor   Color to use
   * @param[in] f  The state of model data
   * @param[in] uv  The state of UV (phase correction) data
   */
  void log(const time_t & t, const std::string &s, const std::string &scolor,
	   const FcstWithLatencyState &f, const UvHtml &uv) const;

  /**
   * Log inputs as HTML
   *
   * @param[in] t  The time
   * @param[in] s  The main message to log
   * @param[in] scolor   Color to use
   * @param[in] state  The state of phase correction 
   * @param[in] uv  The state of UV (phase correction) data
   * @param[in] isRestart  True if the app was just restarted
   */
  void log(const time_t & t, const std::string &s, const std::string &scolor,
	   const LpcState &state, const UvHtml &uv, const bool isRestart) const;

protected:
private:  

  /**
   * @return a string to go with a type
   *
   * @param[in] type  Type to map to a string
   */
  std::string pMsg(const NoPc_t &type) const;

};

# endif
