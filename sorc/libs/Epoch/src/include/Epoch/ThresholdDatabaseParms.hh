
/**
 * @file ThresholdDatabaseParms.hh
 * @brief All user defined algorithm parameters that specify thresholds
 *        used to compute probabilities
 * @class ThresholdDatabaseParms
 * @brief All user defined algorithm parameters that specify thresholds
 *        used to compute probabilities
 *
 * Thresholds come from either a database, or are fixed.
 * Within a database, thresholds exist for one field at one or more obar thresholds
 * Each database has it's own obar thresholds based on that field, for example
 * a APCP database might have one threshold, 2.0, whereas a ULWRF database might have
 * 3 obar thresholds, 30, 35, and 40.
 *
 * In use, more than one of these objects exists for thresholding, one per field.  Exactly
 * one of the objects is used to generate output probabilities, for the obar threshold set
 * that exists for that field.
 */

# ifndef    ThresholdDatabaseParms_hh
# define    ThresholdDatabaseParms_hh

#include <string>
#include <vector>
#include <Epoch/ThresholdDatabaseParams.hh>

class ObarThreshParms;

class ThresholdDatabaseParms
{

public:

  bool _fromDatabase; /**< True if thresholds come from a database, false if fixed */
  std::string _fieldName;   /**< database and model field name */
  ThresholdDatabaseParams::Compare_t  _compare;  /**< comparison type */
  /**
   * fixed threshold (when _fromDatabase=false), or the coldstart threshold
   * (when _fromDatabase=true)
   */
  double _coldstartThreshold;

  /**
   * The database (when _fromDatabase = true)
   */
  std::string _databaseUrl;

  /**
   * True if this is the particular object from which output probability fields are
   * specified  (should be unique)
   */
  bool _isOutputObar;  

  /**
   * True fo output the gridded threshold values that are used for this field
   */
  bool _doOutputThresholdsGrid; 

  /**
   * The output probability field names and outside thresholds for all the fields
   * at each obar thresholds (only used for that unique object _isOutputObar=true)
   */
  std::vector<ObarThreshParms>  _obarThresh;


  /**
   * @return true if input vector of obar thresholds equals local threshold members
   * @param[in] obarThresh  The values
   */
  bool obarMatch(const std::vector<double> &obarThresh) const;

  
  /**
   * @return true if all ObarThreshParms members have outside thresholds all the fields
   * that are input
   * @param[in] fields
   */
  bool hasAllOutsideThresh(const std::vector<std::string> &fields) const;

  /**
   * @return true if this is set as the unique output obar object, and the thresholds come
   * from a database, and at least one obar threshold is configured.
   * IF all this is true, this object can be used to generate output probabilities.
   */
  bool isGoodOutputObar(void) const;

  /**
   * @return true if this is NOT set as the the unique output obar object, and one of
   * the following is true:  1. It is a database with at least one obar threshold, or
   * 2. It is a fixed threshold.
   *
   * IF all this true, the object is well formed for use
   */
  bool isGoodNonoutputObar(void) const;
  /**
   * Use the local comparison type to compare the value to the threshold
   * @return true if the value passes the test compared to the threshold
   * @param[in] val
   * @param[in] thresh
   */
  bool threshTest(double val, double thresh) const;

  /**
   * Use the input  comparison type to compare the value to the threshold
   * @return true if the value passes the test compared to the threshold
   * @param[in] val
   * @param[in] thresh
   * @param[in] c  Comparison type
   */
  static bool threshTest(double val, double thresh, ThresholdDatabaseParams::Compare_t c);


protected:
private:
};

/**
 * @class ObarThreshParms
 * @brief The information for one obar threshold in a database
 *
 */
class ObarThreshParms
{
public:

  /**
   * The obar threshold expected in database
   */
  double _obarThresh;  

  /**
   * The output probability field name, when this obar is used to generate
   * probabilities. Not used otherwise.  (see how things are set up in
   * ThresholdDatabaseParms)
   */
  std::string _griddedFieldName; 

  /**
   *  The outside threshold for all the fields, used when this obar is used
   * to generate probabilities. Not used otherwise.
   * First = field name, second =outside threshold
   */
  std::vector<std::pair<std::string, double> >  _outsideThresh; 

  /**
   * @return true if outside threshold is configured for  all the fields
   * that are input
   * @param[in] fields
   */
  bool hasAllOutsideThresh(const std::vector<std::string> &fields) const;

  /**
   * Retrieve the outside threshold for the field indicated by fieldName
   * @param[in] fieldName
   * @param[out] thresh
   * @return true if the fieldName exists locally and thresh was set
   */
  bool getOutsideThresh(const std::string &fieldName, double &thresh) const;
  
protected:
private:
};

/**
 * @class MultiThrehsoldDatabaseParms
 * @brief Informaton for each thresholded field.
 *
 * Field names and comparison types, coldstart thresh and database
 * information (including fixed fields that do not have a database)
 */
class MultiThresholdDatabaseParms
{
public:

  /**
   * Empty constructor, values not set
   */
  MultiThresholdDatabaseParms();

  /**
   * Constructor that reads in param files to set local state
   * @param[in] num  Number of parm files
   * @param[in] parmfiles  The param file names
   *
   * Sets _ok to false if problems
   */
  MultiThresholdDatabaseParms(int num, const char **parmfiles);

  /**
   * @return true if values are all set and consistent
   */
  inline bool isOk(void) const {return _ok;}

  /**
   * @return the names of all the fields with thresholds
   *
   * These names are expected in the input data
   */
  std::vector<std::string> fieldNames(void) const;

  /**
   * @return the number of different fields have threshold params set 
   *         internally
   */
  int numFieldParms(void) const;

  /**
   * @return the i'th set of threshold params 
   * @param[in] i
   */
  ThresholdDatabaseParms ithFieldParms(int i) const;

  /**
   * @return the number of obar thresholds that generate output fields
   */
  int numOutputObarFields(void) const;

  /**
   * @return the ith obar threshold params that is for an output field
   * @param[in] i
   */
  ObarThreshParms ithOutputObarFieldParms(int i) const;
  
  
protected:
private:

  /**
   * True if object is well formed with all values filled in
   */
  bool _ok;

  /**
   * The param file names that have been read in to set local state
   */
  std::vector<std::string> _parmFileName;

  /**
   * The threshold parameters for each field
   */
  std::vector<ThresholdDatabaseParms> _thresh;

  /**
   * The actual fields names, one per _thresh
   */
  std::vector<std::string> _fieldNames;

  /**
   * The index into the unique _thresh element for which outputs are generated
   * for each obar threshold in that _thresh element
   */
  int _outputObarFieldIndex;
  
};

# endif     // PARMS_ENSFCSTGEN_HH 
