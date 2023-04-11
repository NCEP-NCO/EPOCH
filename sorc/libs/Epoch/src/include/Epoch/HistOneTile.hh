/**
 * @file HistOneTile.hh
 * @brief One tiles counts and sums when building up history
 * @class HistOneTile
 * @brief One tiles counts and sums when building up history
 */

#ifndef HistOneTile_hh
#define HistOneTile_hh

class TileThreshInfoGenBased;

class HistOneTile
{
public:
  /**
   * Constructor
   * @param[in] index  Tile index
   */
  inline HistOneTile(int index) : _index(index), _count(0.0), _sum(0.0) {}

  /**
   * Destructor
   */
  inline ~HistOneTile(void) {}

  /**
   * Add to count/sum with input value
   * @param[in] threshold
   */
  inline void add(double threshold) {_count+=1.0; _sum+=threshold;}

  /**
   * return average of all the thresholds.  If nothing in average return -1
   *
   * @param[out] value  The average
   * @return true if average was computed, false if not
   */
  inline bool average(double &value) const
  {
    if (_count > 0)
    {
      value = _sum/_count;
      return true;
    }
    else
    {
      value = -1;
      return false;
    }
  }

  /**
   * Update local count/sum using input info
   * @param[in] info
   * 
   * If the info says it was coldstart, do not update anything
   */
  void update(const TileThreshInfoGenBased &info);
  
protected:
private:
  int _index;    /**< Tile index */
  double _count; /**< Count of number of things in the sum */
  double _sum;   /**< number of things */

};

#endif
