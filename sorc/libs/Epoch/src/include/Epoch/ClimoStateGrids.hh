/**
 * @file ClimoStateGrids.hh
 * @brief State grids for a particular field within the state
 *        Contains pointers to the actual count and sum grids which are
 *        elsewhere
 * @class ClimoStateGrids
 * @brief State grids for a particular field within the state
 *        Contains pointers to the actual count and sum grids which are
 *        elsewhere
 */

#ifndef CLIMO_STATE_GRIDS_HH
#define CLIMO_STATE_GRIDS_HH

#include <string>

class Grid;
class MultiGrid;

class ClimoStateGrids
{
public:

  /**
   * Constructor
   * Create local members for inputs
   *
   * @param[in] name  Name of input field, expect matching counts and
   *                  sums fields in input data
   * @param[in] data  The data that should have sums and counts for
   *                  all input fields, local pointers are set from these
   */
  ClimoStateGrids(const std::string &name, MultiGrid &data);

  /**
   *  Destructor
   */
  ~ClimoStateGrids(void);

  /**
   * @return true if local state is good
   */
  inline bool good(void) const {return _good;}

  /**
   * Add input data to the state by incrementing counts/sums grids
   *
   * @param[in] input  Input data that should have a field with _name
   *
   * @return true if successful
   */
  bool increment(const Grid &input);

  /**
   * Subtract input data from the state by decrementing counts/sums grids
   *
   * @param[in] input  Input data that should have a field with _name
   *
   * @return true if successful
   */
  bool decrement(const Grid &input);

  /**
   * Normalize the state into the input Grid by dividing sums by counts
   * at each point.

   * @param[in,out] climoGrid  Where to put the normalized results
   *
   * @return true if successful
   */
  bool normalize(Grid &climoGrid) const;

protected:
private:

  bool _good;           /**< True if counts/sums pointers are set */
  std::string _name;    /**< name of field */
  Grid *_counts;        /**< Pointer to externally defined counts grid */
  Grid *_sums;          /**< Pointer to externally defined sums grid */

  /**
   * Decrement sums/counts at a point
   *
   * @param[in] index  Index to point
   * @param[in] value  Value to use in sums
   */
  void _decrement(int index, double value);

  /**
   * Increment sums/counts at a point
   *
   * @param[in] index  Index to point
   * @param[in] value  Value to use in sums
   */
  void _increment(int index, double value);

  /**
   * Normalize sums/counts at a point
   *
   * @param[in] index  Index to point
   * @param[in,out] climoGrid  Grid in which to store normalized value
   */
  void _normalize(int index, Grid &climoGrid) const;
};

#endif
