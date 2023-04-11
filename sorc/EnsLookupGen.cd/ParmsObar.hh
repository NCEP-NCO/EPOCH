
/**
 * @file ParmsObar.hh
 * @brief params for each obar threshold
 * @class ParmsObar
 * @brief params for each obar threshold
 */

# ifndef    ParmsObar_HH
# define    ParmsObar_HH

#include <string>
#include <Epoch/FieldThresh.hh>
#include "Params.hh"

class ParmsObar
{

public:

  /**
   * Constructor
   * @param[in] p  Obar parameters
   * @param[in] outsideThresh  The thresholds for outside the domain all fields
   */
  inline ParmsObar(const Params::ObsThresh_t &p,
		   const std::vector<FieldThresh> &outsideThresh) :
    _obarThresh(p.obsThresh),
    _griddedFieldName(p.griddedFieldName),
    _outsideThresh(outsideThresh)
  {}

  /**
   * Destructor
   */
  inline ~ParmsObar(void) {}

  /**
   * @return true if input field name has an outside the domain threshold
   * @param[in] name
   */
  inline bool hasOutsideThresh(const std::string &name) const
  {
    for (size_t i=0; i<_outsideThresh.size(); ++i)
    {
      if (_outsideThresh[i].nameMatch(name))
      {
	return true;
      }
    }
    return false;
  }

  /**
   * @return true if input field name has an outside the domain threshold
   *         and return that threshold
   * @param[in] fieldName
   * @param[out] thresh
   */
  inline bool getOutsideThresh(const std::string &fieldName, double &thresh) const
  {
    for (size_t i=0; i<_outsideThresh.size(); ++i)
    {
      if (_outsideThresh[i].nameMatch(fieldName))
      {
	thresh = _outsideThresh[i].getThresh();
	return true;
      }
    }
    return false;
  }


  double _obarThresh;                       /**< Obar threshold */
  std::string _griddedFieldName;            /**< field name to use for gridded data */
  std::vector<FieldThresh> _outsideThresh;  /**< the outside thresholds for
					     *  each field */

protected:
private:

};

# endif     // PARMS_ENSFCSTGEN_HH 
