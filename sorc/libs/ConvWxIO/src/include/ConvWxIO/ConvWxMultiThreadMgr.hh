/**
 * @file ConvWxMultiThreadMgr.hh
 * @brief Threading, for "Mgr" classes that do threading
 * @class ConvWxMultiThreadMgr
 * @brief Threading, for "Mgr" classes that do threading
 *
 * These static functions are in place for threaded applications, no state,
 * where there can be multiple 'threaded actions' happening simultaniously.
 * 
 * Each threaded action is specified by these things:
 *
 *  0. A unique name
 *  1. A pointer to some kind of information that is passed in and out of
 *     algorithms (info in the thread() method)
 *  2. A pointer to the managing  class (the context pointer)
 *  3. A pointer to a static method that takes one argument which is pointer to
 *     information.
 *
 */

# ifndef    CONVWX_MULTI_THREAD_MGR_HH
# define    CONVWX_MULTI_THREAD_MGR_HH

#include <string>

//----------------------------------------------------------------
class ConvWxMultiThreadMgr
{
public:

  /**
   *  Cleans up at end, deletes ALL threading actions
   */
  static void cleanup(void);

  /**
   * Cleans up at end, deletes a single thread action
   * @param[in] name thread action
   */
  static void cleanup(const std::string &name);

  /**
   * Initialize
   *
   * @param[in] name for thread action
   * @param[in] numThread  Number of threads, set to <= 1 for no threading
   * @param[in] debug  True for thread debugging
   * @param[in] context  The managing class (pointer)
   * @param[in] method  The method to run, argument assumed pointer to info
   *
   * @return true if succeful, false if name already in use
   */
  static bool init(const std::string &name, int numThread, bool debug,
		   void *context, void (*method)(void *));

  /**
   * Reinitialize after having already initialized
   *
   * @param[in] name for thread action
   * @param[in] numThread  Number of threads, set to <= 1 for no threading
   * @param[in] debug  True for thread debugging
   *
   * @return true if succeful, false if name not in use
   */
  static bool reinit(const std::string &name, int numThread, bool debug);


  /**
   * Start a thread within a thread action, or call the 'method' directly
   * if the number of threads for that action is less than two.
   *
   * @param[in] name for thread action
   * @param[in] index  Index to assign to thread
   * @param[in] info   Pointer to the information specific to this thread,
   *                   cast to void
   *
   * @return true if succeful, false if name not in use
   */
  static bool thread(const std::string &name, int index, void *info);
  
  /**
   * if threading is enabled for the named action, lock it's mutex
   *
   * @param[in] name for thread action
   * @return true if succeful, false if name not in use
   */
  static bool lock(const std::string &name);

  /**
   * if threading is enabled for the named action, unlock mutex that was
   * locked by a call to lock()
   *
   * @param[in] name for thread action
   * @return true if succeful, false if name not in use
   */
  static bool unlock(const std::string &name);

  /**
   * Wait for threads to complete for one thread action, then return
   *
   * @param[in] name for thread action
   * @return true if succeful, false if name not in use, in which case return
   * immediately.
   */
  static bool waitForThreads(const std::string &name);

protected:
private:  

};

# endif 
