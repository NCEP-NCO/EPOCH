/**
 * @file AdditionalInputs.hh
 * @brief The specification of any number of addtional inputs that gets thresholded
 * @class AdditionalInputs
 * @brief The specification of any number of addtional inputs that gets thresholded
 *
 * Zero or more
 *
 */

#ifndef AdditionalInputs_HH
#define AdditionalInputs_HH

#include "AdditionalInput.hh"
#include "ParmsPbarCompute.hh"
#include <vector>
#include <ctime>

class AdditionalInputs
{
public:

  /**
   * Constructor 
   * @param[in] parms Stored locally
   */
  AdditionalInputs(const ParmsPbarCompute &parms);

  /**
   * Destructor
   */
  inline ~AdditionalInputs(void) {}

  /**
   * Reset state
   * Uses params to set up all additional inputs to an initial state
   */
  void reset(void);

  /**
   * Use input ensemble model data to set pointers to all additional inputs
   * @param[in] mg  Ensemble model data
   * @return true if all pointers set
   */
  bool setGridPointers(const MultiGrid &mg);

  /**
   * At a gridpoint set ensemble data values for all additional inputs
   * @param[in] gridIndex
   * @param[in] which 1 or 2
   * @return  true if able to set everything and no value was missing
   */
  bool setAllValues(int gridIndex, int which);

  /**
   * @return true if all values have been set and every value passes the comparison
   * test compared to the threshold
   * @param[in] which 1 or 2
   */
  bool passesTests(int which) const;


protected:
private:

  ParmsPbarCompute _parms;           /**< params */
  bool _hasInput1;         /**< True if field 1 has additional input */
  bool _hasInput2;         /**< True if field 2 has additional input */
  AdditionalInput _input1;    /**< state for each additional input */
  AdditionalInput _input2;    /**< state for each additional input */
};

#endif
