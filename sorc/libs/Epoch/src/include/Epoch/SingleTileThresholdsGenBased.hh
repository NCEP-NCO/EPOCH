/**
 * @file SingleTileThresholdsGenBased.hh
 * @brief Storage for thresholds in one tile
 * @class SingleTileThresholdsGenBased
 * @brief Storage for thresholds in one tile
 *
 *
 * Additional information is a bias value,
 * a flag as to whether it is 'coldstart' (i.e. fixed) threshold,
 * and a flag as to whether the values are those of the 'mother tile' or not.
 *
 * Within the XML that is read/written for this object
 */

# ifndef    SingleTileThresholdsGenBased_hh
# define    SingleTileThresholdsGenBased_hh

#include <vector>
#include <string>

class TileThreshInfoGenBased;
class TileInfo;

//----------------------------------------------------------------
class SingleTileThresholdsGenBased
{
public:

  /**
   * Empty
   */
  SingleTileThresholdsGenBased(void);

  /**
   * constructor in which an XML string is parsed, as would be created by
   * toXml()
   *
   * @param[in] xml  The string to parse
   */
  SingleTileThresholdsGenBased(const std::string &xml);

  /**
   * 'Coldstart' constructor in which threshold passed in
   *
   * @param[in] precipThresh  The coldstart threshold
   * @param[in] fromMother  True if this is the mother tile
   *
   */
  SingleTileThresholdsGenBased(double precipThresh, bool fromMother);

  /**
   * Destructor
   */
  ~SingleTileThresholdsGenBased(void);

  /**
   * Simple get methods
   */
  inline bool getColdstart(void) const {return _coldstart;}
  inline bool getMotherTile(void) const {return _motherTile;}
  inline double getBias(void) const {return _bias;}
  inline double getThresh(void) const {return _thresh;}

  /**
   * Simple set methods
   */
  inline void setThreshold(double value) { _thresh = value;}
  inline void setColdstart(bool status) { _coldstart = status;}


  /**
   * Update local state using input
   *
   * @param[in] item
   */
  bool update(const TileThreshInfoGenBased &item);

  /**
   * Convert local state to an XML string, including the tiling index
   *
   * @param[in] tileIndex  The tile index value
   * @param[in] indent   Number of tabs to indent the XML
   *
   * @return the XML string
   */
  std::string toXml(int tileIndex, int indent=0) const;

  /**
   * Debug print of local state along with inputs
   * @param[in] lt  Lead secvonds
   * @param[in] obar  Obar value
   * @param[in] tileIndex  
   * @param[in] info  Information object to pull print stuff out of
   * @param[in] verbose  True for more debug output
   */
  void print(int lt, double obar, int tileIndex,
	     const TileInfo &info,bool verbose) const;

  /**
   * @return true if object memeber values are set
   */
  inline bool ok(void) const {return _ok;}

  /**
   * Value of XML tag
   */ 
  static const std::string _tag;

protected:
private:  

  bool _ok;           /**< True if object is set */
  double _thresh;     /**< threshold for tile */
  double _bias;       /**< Bias value */
  bool _coldstart;    /**< True if the threshold is coldstart*/
  bool _motherTile;   /**< True if the threshold is from the 'mother tile' */
};

# endif
