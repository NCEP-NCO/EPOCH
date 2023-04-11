/**
 * @file Info.hh
 * @brief Information passed in and out of the algorithm
 * @class Info
 * @brief Information passed in and out of the algorithm
 */

# ifndef   INFO_HH
# define   INFO_HH
#include <string>
class EnsLookupGenMgr;

//------------------------------------------------------------------
class Info 
{
public:

  /**
   * constructor
   * @param[in] t  Gen time
   * @param[in] lt  Lead seconds
   * @param[in] alg  Pointer to Mgr
   */
  Info(const time_t &t, const int lt, EnsLookupGenMgr *alg);

  /**
   *  destructor
   */
  virtual ~Info(void);


  time_t _genTime;   /**< Gen or obs time */
  int _lt;     /**< Lead seconds (when forecast, ignored if not) */
  EnsLookupGenMgr *_alg; /**< Pointer to context */

protected:
private:  
  
};

# endif
