/**
 * @file ParmsField.hh
 * @brief params for one field that is input to the algorithm
 * @class ParmsField
 * @brief params for one field that is input to the algorithm
 */

# ifndef    ParmsField_H
# define    ParmsField_H

#include "Params.hh"
#include "ParmsObar.hh"
#include <string>

class ParmsField
{

public:

  /**
   * Constructor
   * @param[in] p  Params used to fill members, tiled thresholds
   */
  ParmsField(const Params::FieldFromDatabaseSpec_t &p,
	     const std::vector<ParmsObar> &op);

  /**
   * Constructor
   * @param[in] p  Params used to fill members, fixedthresholds (no db, no obar)
   */
  ParmsField(const Params::FieldFixedSpec_t &p);

  /**
   * Destructor
   */
  inline ~ParmsField(void) {}

  /**
   * Compare value to threshold using local comparison type
   *
   * @param[in] val  Value to compare to threshold
   * @param[in] thresh  The threshold
   * @return true if passed the test
   */
  bool threshTest(double val, double thresh) const;

  /**
   * @return the field name member
   */
  inline std::string getFieldName(void) const {return _fieldName;}

  /**
   * @return the comparison type member
   */
  inline Params::Compare_t getComparison(void) const {return _comparison;}

  /**
   * @return the string for this comparison type
   */
  inline std::string comparisionString(void) const
  {
    return sprintCompare(_comparison);
  }

  /**
   * @return a string for the comparison type
   * @param[in] c
   */
  static std::string sprintCompare(Params::Compare_t c);

  /**
   * Compare value to threshold using input comparison type
   *
   * @param[in] val  Value to compare to threshold
   * @param[in] thresh  The threshold
   * @param[in] c  Comparison
   * @return true if passed the test
   */
  static bool threshTest(double val, double thresh, Params::Compare_t c);

  std::string _fieldName;        /**< Name of input field */
  Params::Compare_t _comparison; /**< Type of comparison */
  bool _isDatabase;              /**< True if thresholds come from a database */
  std::string _databaseUrl;      /**< The DB url if _isDatabase */
  int _obarInfoIndex;            /**< Index into obar information in params */
  bool _isOutputObar;            /**< True if the obar thresholds are used for output */
  bool _doOutputThresholds;      /**< True to output thresholds grid for this field */
  std::vector<ParmsObar> _obarInfo; /**< The params for each obar associated with this field */

  /**
   * Threshold for cold starts, or for fixed field  the actual fixed threshold
   */
  double _coldstartThresh;       

protected:

private:

};

# endif     // PARMS_ENSFCSTGEN_HH 
