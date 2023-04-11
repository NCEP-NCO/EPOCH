/**
 * @file ConvWxThreadMgr.cc
 */

#include <ConvWxIO/ConvWxThreadMgr.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <toolsa/TaThreadDoubleQue.hh>
#include <toolsa/TaThread.hh>
#include <toolsa/TaThreadSimple.hh>

/**
 * @class ConvWxThreadQue
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue(void) {}
  TaThread *clone(const int index);

};

class ConvWxSpecialThreadQue : public TaThreadDoubleQue
{
public:
  inline ConvWxSpecialThreadQue(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxSpecialThreadQue(void) {}
  TaThread *clone(const int index);
};


/**
 * Static threading object
 */
static ConvWxThreadQue sThread;

/**
 * Static special threading object
 */
static ConvWxSpecialThreadQue sSpecialThread;

/**
 * Static context pointer
 */
static void *sContext = NULL;

/**
 * Static method pointer
 */
static TaThread::ThreadMethod_t sMethod = NULL;
static TaThread::ThreadMethod_t sSpecialMethod = NULL;

/**
 * Flag for 'special' or not
 */
static bool sIsSpecial = false;

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod);
  t->setThreadContext(sContext);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxSpecialThreadQue::clone(const int index)
{
  if (sSpecialMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sSpecialMethod);
  t->setThreadContext(sContext);
  return (TaThread *)t;
}

//----------------------------------------------------------------
void ConvWxThreadMgr::cleanup(void)
{
  // re-initialize with no threads
  sThread.reinit(0, false);
  sMethod = NULL;
  if (sIsSpecial)
  {
    sSpecialThread.reinit(0, false);
    sSpecialMethod = NULL;
  }
  sContext = NULL;
}

//----------------------------------------------------------------
void ConvWxThreadMgr::init(const int numThread, const bool debug,
			   void *context, void (*method)(void *))
{
  sMethod = method;
  sSpecialMethod = NULL;
  sContext = context;
  sThread.init(numThread, debug);
  sIsSpecial = false;
}

//----------------------------------------------------------------
void ConvWxThreadMgr::init(const int numThread, const bool debug,
			   void *context, void (*method)(void *),
			   void (*specialMethod)(void *))
{
  sMethod = method;
  sSpecialMethod = specialMethod;
  sContext = context;
  sThread.init(numThread, debug);
  sSpecialThread.init(numThread, debug);
  sIsSpecial = true;
}


//----------------------------------------------------------------
void ConvWxThreadMgr::reinit(const int numThread, const bool debug)
{
  sThread.reinit(numThread, debug);
  if (sIsSpecial)
  {
    sSpecialThread.reinit(numThread, debug);
  }
}
  
//----------------------------------------------------------------
void ConvWxThreadMgr::thread(int index, void *info)
{
  sThread.thread(index, info);
}
  
//----------------------------------------------------------------
void ConvWxThreadMgr::specialThread(int index, void *info)
{
  if (sIsSpecial)
  {
    sSpecialThread.thread(index, info);
  }
  else
  {
    ILOG(ERROR, "Cannot start a thread wrong state");
  }
}
  
//----------------------------------------------------------------
void ConvWxThreadMgr::lock(void)
{
  sThread.lockForIO();
}

//----------------------------------------------------------------
void ConvWxThreadMgr::unlock(void)
{
  sThread.unlockAfterIO();
}

//----------------------------------------------------------------
void ConvWxThreadMgr::waitForThreads(void)
{
  sThread.waitForThreads();
  if (sIsSpecial)
  {
    sSpecialThread.waitForThreads();
  }
}

