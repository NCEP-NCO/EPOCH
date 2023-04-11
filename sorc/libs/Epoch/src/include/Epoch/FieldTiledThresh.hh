/**
 * @file FieldTiledThresh.hh
 * @brief Simple class to store a field name and 
 *        a vector of thresholds, one threshold per tile
 * @class FieldTiledThresh
 * @brief Simple class to store a field name and 
 *        a vector of thresholds, one threshold per tile
 */

# ifndef    FieldTiledThresh_hh
# define    FieldTiledThresh_hh

#include <string>
#include <vector>

//----------------------------------------------------------------
class FieldTiledThresh
{
public:

  /**
   * Empty
   */
  inline FieldTiledThresh(void) :
    _ok(false),
    _fieldName("Unknown")
  {
  }

  /**
   * Members set from inputs
   * @param[in] field
   * @param[in] thresh
   */
  inline FieldTiledThresh(const std::string &field,
		     std::vector<double> thresh) :
    _ok(true),
    _fieldName(field),
    _thresh(thresh)
  {}

  /**
   * Name set from input, with threshold set to a bad value, for all tiles
   *
   * @param[in] field
   * @param[in] numTile
   */
  inline FieldTiledThresh(const std::string &field, int numTile) :
    _ok(true),
    _fieldName(field)
  {
    for (int i=0; i<numTile; ++i)
    {
      _thresh.push_back(-999.99);
    }
  }

  /**
   * Destructor
   */
  inline ~FieldTiledThresh(void) {}

  /**
   * @return true if names match
   *
   * @param[in] fieldName  
   */
  inline bool nameMatch(const std::string &fieldName) const
  {
    return _fieldName == fieldName;
  }

  /**
   * @return threshold for a tile
   * @param[in] tileIndex
   */
  inline
  double getThresh(int tileIndex) const {return _thresh[tileIndex];}

  /**
   * Print to stdout
   * @param[in] verbose Flag
   */
  void print(bool verbose=false) const;

protected:
private:  

  bool _ok;                /**< True if set */
  std::string _fieldName;  /**< Name */
  std::vector<double> _thresh;   /**< Thresholds */


};

# endif
