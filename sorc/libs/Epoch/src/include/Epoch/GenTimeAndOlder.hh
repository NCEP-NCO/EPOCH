/**
 * @file GenTimeAndOlder.hh
 * @brief A gen time, and possibly an older actual gen time from a previous
 *        day with the same hour/minute/second as the gen time
 * @class GenTimeAndOlder
 * @brief A gen time, and possibly an older actual gen time from a previous
 *        day with the same hour/minute/second as the gen time
 */

# ifndef    GenTimeAndOlder_hh
# define    GenTimeAndOlder_hh

#include <string>
#include <vector>

//----------------------------------------------------------------
class GenTimeAndOlder
{
public:

  /**
   * Empty
   */
  inline GenTimeAndOlder(void) :
    _ok(false),
    _genTime(-1),
    _olderGenTime(-1),
    _hasOlderGenTime(false)
  {
  }

  /**
   * Members set from inputs, current and older gen time
   * @param[in] genTime
   * @param[in] olderGenTime
   */
  inline GenTimeAndOlder(const time_t &genTime, const time_t olderGenTime) :
    _ok(true),
    _genTime(genTime),
    _olderGenTime(olderGenTime),
    _hasOlderGenTime(true)
  {}

  /**
   * Members set from inputs, only current (no older)
   * @param[in] genTime
   */
  inline GenTimeAndOlder(const time_t &genTime) :
    _ok(true),
    _genTime(genTime),
    _olderGenTime(-1),
    _hasOlderGenTime(false)
  {}

  /**
   * Parse XML to initialize object
   * @param[in] xml
   */
  GenTimeAndOlder(const std::string &xml);
  
  /**
   * Destructor
   */
  inline ~GenTimeAndOlder(void) {}

  /**
   * Check on health
   * @return true if ok
   */
  inline bool isOk(void) const {return _ok;}

  /**
   * Operator < 
   * @param[in] f  Object to compare to
   * @return true if local object is < f
   */
  bool operator<(const GenTimeAndOlder &f) const;

  /**
   * Operator==
   * @return true if the same
   * @param[in] f
   */
  bool operator==(const GenTimeAndOlder &f) const;

  /**
   * @return the local gen time value
   */
  inline time_t getGenTime(void) const {return _genTime;}

  /**
   * @return a debug representation
   */
  std::string sprint(void) const;

  /**
   * @return an XML representation
   */
  std::string getXml(void) const;

  /**
   * Set state from XML parsing
   * @param[in] xml  String to parse
   */
  bool parseXml(const std::string &xml);

  /**
   * @return true if local gentime is older than input
   * @param[in] genTime
   */
  inline bool olderThan(const time_t &genTime) const
  {
    return _genTime < genTime;
  }

  /**
   * @return true if local gentime is equal to input
   * @param[in] genTime
   */
  inline bool genTimeEqual(const time_t &genTime) const
  {
    return genTime == _genTime;
  }

  /**
   * Value of XML tag
   */ 
  static const std::string _tag;

protected:
private:  

  bool _ok;                /**< True if set */
  time_t _genTime;         /**< The gen time */
  time_t _olderGenTime;    /**< Matching older gen time, if set */
  bool _hasOlderGenTime;   /**< True if _olderGenTime is set */
};

# endif
