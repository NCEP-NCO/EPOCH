/**
 * @file AdditionalInput.hh
 * @brief The specification of one addtional input that gets thresholded
 * @class AdditionalInput
 * @brief The specification of one addtional input that gets thresholded
 *
 * Can be grid based thresholds, or a fixed threshold everywhere
 */

#ifndef AdditionalInput_HH
#define AdditionalInput_HH

#include "AdditionalInput.hh"
#include "ParmsPbarCompute.hh"
#include <vector>
#include <ctime>

class AdditionalInput
{
public:

  /**
   * Empty constructor
   */
  AdditionalInput(void);

  /**
   * Constructor  - fixed threshold additional input
   *
   * @param[in] ensembleFieldName  Name of field in input ensemble model data
   * @param[in] fixedThreshold  The thresh 
   * @param[in] comparisonType  >= or <= 
   */
  AdditionalInput(const std::string &ensembleFieldName,
		  double fixedThreshold,
		  Params::Comparison_t comparisonType);

  /**
   * Destructor
   */
  inline ~AdditionalInput(void) {}

  /**
   * Try to point to the ensemble data grid locally based on local field name
   * using input
   *
   * @param[in] ensembleInputs All the input model grids
   *
   * @return true if pointer set locally successfully
   */
  bool setEnsembleGridPtr(const MultiGrid &ensembleInputs);

  /**
   * Read local ensemble model data (if set) at a grid index and 
   * store the value internally
   * @param[in] gridIndex
   * @return true if could access the data and it was not missing, value stored
   */
  bool getAndStoreEnsembleValueAtPoint(int gridIndex);

  /**
   * @return true if the data value is set, and 
   *         the data passes the comparison test, false otherwise
   */
  bool passesTest(void) const;

protected:
private:

  std::string _ensembleFieldName;          /**< Name of field in input ensemble model data */
  Params::Comparison_t _comparisonType;    /**< Comparision >= or <= */
  const Grid *_ensembleGridPtr;            /**< Pointer to the ensemble data */
  double _threshold;              /**< The fixed threshold */
  bool _currentEnsembleValueSet;  /**< True if both the current ensemble value is set */
  double _currentEnsembleValue;   /**< The current ensemble value when _currentEnsembleValueSet */
};

#endif
