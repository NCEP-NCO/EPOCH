/**
 * @file ConvWxThreadMgr.hh
 * @brief Threading, for "Mgr" classes that do threading
 * @class ConvWxThreadMgr
 * @brief Threading, for "Mgr" classes that do threading
 *
 * These static functions are in place for threaded applications, no state
 *
 * Each thread has three things:
 *  1. A pointer to some kind of information that is passed in and out of
 *     algorithms (info in the thread() method)
 *  2. A pointer to the managing  class (the context pointer)
 *  3. A pointer to a method that takes one argument which is pointer to
 *     information.
 * and optionally
 *  4. A pointer to a second method that takes one argument which is pointer to
 *     information.
 *
 */

# ifndef    CONVWX_THREAD_MGR_HH
# define    CONVWX_THREAD_MGR_HH

//----------------------------------------------------------------
class ConvWxThreadMgr
{
public:

  /**
   *  Cleans up at end
   */
  static void cleanup(void);

  /**
   * Initialize
   *
   * @param[in] numThread  Number of threads, set to <=1 for no threading
   * @param[in] debug  True for thread debugging
   * @param[in] context  The managing class (pointer)
   * @param[in] method  The method to run, argument assumed pointer to info
   */
  static void init(const int numThread, const bool debug,
		   void *context, void (*method)(void *));

  /**
   * Initialize
   *
   * @param[in] numThread  Number of threads, set to <=1 for no threading
   * @param[in] debug  True for thread debugging
   * @param[in] context  The managing class (pointer)
   * @param[in] method  The method to run, argument assumed pointer to info
   * @param[in] specialMethod  The special method to run, argument assumed pointer to special info
   */
  static void init(const int numThread, const bool debug,
		   void *context, void (*method)(void *),
		   void (*specialMethod)(void *));

  /**
   * Reinitialize after having already initialized
   *
   * @param[in] numThread  Number of threads, <=1 for no threading
   * @param[in] debug  True for thread debugging
   */
  static void reinit(const int numThread, const bool debug);

  /**
   * Start a thread, or call the 'method' directly if the number of
   * threads is less than two.
   *
   * @param[in] index  Index to assign to thread
   * @param[in] info   Pointer to the information specific to this app,
   *                   cast to void
   */
  static void thread(int index, void *info);
  
  /**
   * Start a special thread, or call the 'method' directly if the number of
   * threads is less than two. This should only be called with the
   * init function that has specialMethod passed in
   *
   * @param[in] index  Index to assign to thread
   * @param[in] info   Pointer to the information specific to this app,
   *                   cast to void
   */
  static void specialThread(int index, void *info);
  
  /**
   * if threading is enabled, lock a mutex
   */
  static void lock(void);

  /**
   * if threading is enabled, unlock mutex that was locked by a call to lock()
   */
  static void unlock(void);

  /**
   * Wait for threads to complete, then return
   */
  static void waitForThreads(void);

protected:
private:  

};

# endif 
