/**
 * @file ConvWxMultiThreadMgr.cc
 */

#include <ConvWxIO/ConvWxMultiThreadMgr.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <toolsa/TaThreadDoubleQue.hh>
#include <toolsa/TaThread.hh>
#include <toolsa/TaThreadSimple.hh>
#include <vector>
#include <map>

/**
 * Static context pointer
 */
static void *sContext[50];

/**
 * Static method pointer
 */
static TaThread::ThreadMethod_t sMethod[50];

static std::string sName[50];

/**
 * @class ConvWxThreadQue0
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue0 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue0(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue0(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue1
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue1 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue1(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue1(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue2
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue2 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue2(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue2(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue3
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue3 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue3(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue3(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue4
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue4 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue4(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue4(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue5
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue5 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue5(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue5(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue6
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue6 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue6(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue6(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue7
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue7 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue7(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue7(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue8
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue8 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue8(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue8(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue9
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue9 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue9(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue9(void) {}
  TaThread *clone(const int index);
};








/**
 * @class ConvWxThreadQue10
 * @brief instantiates TaThreadDoubleQue1 by implementing clone method
 */
class ConvWxThreadQue10 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue10(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue10(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue11
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue11 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue11(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue11(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue12
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue12 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue12(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue12(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue13
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue13 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue13(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue13(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue14
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue14 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue14(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue14(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue15
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue15 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue15(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue15(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue16
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue16 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue16(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue16(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue17
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue17 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue17(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue17(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue18
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue18 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue18(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue18(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue19
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue19 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue19(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue19(void) {}
  TaThread *clone(const int index);
};










/**
 * @class ConvWxThreadQue20
 * @brief instantiates TaThreadDoubleQue1 by implementing clone method
 */
class ConvWxThreadQue20 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue20(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue20(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue21
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue21 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue21(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue21(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue22
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue22 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue22(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue22(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue23
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue23 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue23(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue23(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue24
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue24 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue24(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue24(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue25
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue25 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue25(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue25(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue26
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue26 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue26(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue26(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue27
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue27 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue27(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue27(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue28
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue28 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue28(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue28(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue29
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue29 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue29(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue29(void) {}
  TaThread *clone(const int index);
};









/**
 * @class ConvWxThreadQue30
 * @brief instantiates TaThreadDoubleQue1 by implementing clone method
 */
class ConvWxThreadQue30 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue30(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue30(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue31
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue31 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue31(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue31(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue32
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue32 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue32(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue32(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue33
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue33 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue33(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue33(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue34
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue34 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue34(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue34(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue35
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue35 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue35(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue35(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue36
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue36 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue36(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue36(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue37
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue37 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue37(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue37(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue38
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue38 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue38(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue38(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue39
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue39 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue39(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue39(void) {}
  TaThread *clone(const int index);
};












/**
 * @class ConvWxThreadQue40
 * @brief instantiates TaThreadDoubleQue1 by implementing clone method
 */
class ConvWxThreadQue40 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue40(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue40(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue41
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue41 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue41(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue41(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue42
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue42 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue42(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue42(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue43
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue43 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue43(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue43(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue44
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue44 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue44(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue44(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue45
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue45 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue45(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue45(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue46
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue46 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue46(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue46(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue47
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue47 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue47(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue47(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue48
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue48 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue48(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue48(void) {}
  TaThread *clone(const int index);
};

/**
 * @class ConvWxThreadQue49
 * @brief instantiates TaThreadDoubleQue by implementing clone method
 */
class ConvWxThreadQue49 : public TaThreadDoubleQue
{
public:
  inline ConvWxThreadQue49(void) : TaThreadDoubleQue() {}
  inline virtual ~ConvWxThreadQue49(void) {}
  TaThread *clone(const int index);
};


static int sNumThread=0;
static ConvWxThreadQue0 sThread0;
static ConvWxThreadQue1 sThread1;
static ConvWxThreadQue2 sThread2;
static ConvWxThreadQue3 sThread3;
static ConvWxThreadQue4 sThread4;
static ConvWxThreadQue5 sThread5;
static ConvWxThreadQue6 sThread6;
static ConvWxThreadQue7 sThread7;
static ConvWxThreadQue8 sThread8;
static ConvWxThreadQue9 sThread9;

static ConvWxThreadQue10 sThread10;
static ConvWxThreadQue11 sThread11;
static ConvWxThreadQue12 sThread12;
static ConvWxThreadQue13 sThread13;
static ConvWxThreadQue14 sThread14;
static ConvWxThreadQue15 sThread15;
static ConvWxThreadQue16 sThread16;
static ConvWxThreadQue17 sThread17;
static ConvWxThreadQue18 sThread18;
static ConvWxThreadQue19 sThread19;

static ConvWxThreadQue20 sThread20;
static ConvWxThreadQue21 sThread21;
static ConvWxThreadQue22 sThread22;
static ConvWxThreadQue23 sThread23;
static ConvWxThreadQue24 sThread24;
static ConvWxThreadQue25 sThread25;
static ConvWxThreadQue26 sThread26;
static ConvWxThreadQue27 sThread27;
static ConvWxThreadQue28 sThread28;
static ConvWxThreadQue29 sThread29;

static ConvWxThreadQue30 sThread30;
static ConvWxThreadQue31 sThread31;
static ConvWxThreadQue32 sThread32;
static ConvWxThreadQue33 sThread33;
static ConvWxThreadQue34 sThread34;
static ConvWxThreadQue35 sThread35;
static ConvWxThreadQue36 sThread36;
static ConvWxThreadQue37 sThread37;
static ConvWxThreadQue38 sThread38;
static ConvWxThreadQue39 sThread39;

static ConvWxThreadQue40 sThread40;
static ConvWxThreadQue41 sThread41;
static ConvWxThreadQue42 sThread42;
static ConvWxThreadQue43 sThread43;
static ConvWxThreadQue44 sThread44;
static ConvWxThreadQue45 sThread45;
static ConvWxThreadQue46 sThread46;
static ConvWxThreadQue47 sThread47;
static ConvWxThreadQue48 sThread48;
static ConvWxThreadQue49 sThread49;

static TaThreadDoubleQue *sThreadPtr(int n)
{
  switch (n)
  {
  case 0:
    return &sThread0;
  case 1:
    return &sThread1;
  case 2:
    return &sThread2;
  case 3:
    return &sThread3;
  case 4:
    return &sThread4;
  case 5:
    return &sThread5;
  case 6:
    return &sThread6;
  case 7:
    return &sThread7;
  case 8:
    return &sThread8;
  case 9:
    return &sThread9;

  case 10:
    return &sThread10;
  case 11:
    return &sThread11;
  case 12:
    return &sThread12;
  case 13:
    return &sThread13;
  case 14:
    return &sThread14;
  case 15:
    return &sThread15;
  case 16:
    return &sThread16;
  case 17:
    return &sThread17;
  case 18:
    return &sThread18;
  case 19:
    return &sThread19;

  case 20:
    return &sThread20;
  case 21:
    return &sThread21;
  case 22:
    return &sThread22;
  case 23:
    return &sThread23;
  case 24:
    return &sThread24;
  case 25:
    return &sThread25;
  case 26:
    return &sThread26;
  case 27:
    return &sThread27;
  case 28:
    return &sThread28;
  case 29:
    return &sThread29;

  case 30:
    return &sThread30;
  case 31:
    return &sThread31;
  case 32:
    return &sThread32;
  case 33:
    return &sThread33;
  case 34:
    return &sThread34;
  case 35:
    return &sThread35;
  case 36:
    return &sThread36;
  case 37:
    return &sThread37;
  case 38:
    return &sThread38;
  case 39:
    return &sThread39;

  case 40:
    return &sThread40;
  case 41:
    return &sThread41;
  case 42:
    return &sThread42;
  case 43:
    return &sThread43;
  case 44:
    return &sThread44;
  case 45:
    return &sThread45;
  case 46:
    return &sThread46;
  case 47:
    return &sThread47;
  case 48:
    return &sThread48;
  case 49:
    return &sThread49;

    


  default:
    return NULL;
  }
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue0::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[0]);
  t->setThreadContext(sContext[0]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue1::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[1]);
  t->setThreadContext(sContext[1]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue2::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[2]);
  t->setThreadContext(sContext[2]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue3::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[3]);
  t->setThreadContext(sContext[3]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue4::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[4]);
  t->setThreadContext(sContext[4]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue5::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[5]);
  t->setThreadContext(sContext[5]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue6::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[6]);
  t->setThreadContext(sContext[6]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue7::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[7]);
  t->setThreadContext(sContext[7]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue8::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[8]);
  t->setThreadContext(sContext[8]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue9::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[9]);
  t->setThreadContext(sContext[9]);
  return (TaThread *)t;
}



//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue10::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[10]);
  t->setThreadContext(sContext[10]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue11::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[11]);
  t->setThreadContext(sContext[11]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue12::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[12]);
  t->setThreadContext(sContext[12]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue13::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[13]);
  t->setThreadContext(sContext[13]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue14::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[14]);
  t->setThreadContext(sContext[14]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue15::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[15]);
  t->setThreadContext(sContext[15]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue16::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[16]);
  t->setThreadContext(sContext[16]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue17::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[17]);
  t->setThreadContext(sContext[17]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue18::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[18]);
  t->setThreadContext(sContext[18]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue19::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[19]);
  t->setThreadContext(sContext[19]);
  return (TaThread *)t;
}



//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue20::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[20]);
  t->setThreadContext(sContext[20]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue21::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[21]);
  t->setThreadContext(sContext[21]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue22::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[22]);
  t->setThreadContext(sContext[22]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue23::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[23]);
  t->setThreadContext(sContext[23]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue24::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[24]);
  t->setThreadContext(sContext[24]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue25::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[25]);
  t->setThreadContext(sContext[25]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue26::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[26]);
  t->setThreadContext(sContext[26]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue27::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[27]);
  t->setThreadContext(sContext[27]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue28::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[28]);
  t->setThreadContext(sContext[28]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue29::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[29]);
  t->setThreadContext(sContext[29]);
  return (TaThread *)t;
}


//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue30::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[30]);
  t->setThreadContext(sContext[30]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue31::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[31]);
  t->setThreadContext(sContext[31]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue32::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[32]);
  t->setThreadContext(sContext[32]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue33::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[33]);
  t->setThreadContext(sContext[33]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue34::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[34]);
  t->setThreadContext(sContext[34]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue35::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[35]);
  t->setThreadContext(sContext[35]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue36::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[36]);
  t->setThreadContext(sContext[36]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue37::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[37]);
  t->setThreadContext(sContext[37]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue38::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[38]);
  t->setThreadContext(sContext[38]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue39::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[39]);
  t->setThreadContext(sContext[39]);
  return (TaThread *)t;
}


//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue40::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[40]);
  t->setThreadContext(sContext[40]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue41::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[41]);
  t->setThreadContext(sContext[41]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue42::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[42]);
  t->setThreadContext(sContext[42]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue43::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[43]);
  t->setThreadContext(sContext[43]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue44::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[44]);
  t->setThreadContext(sContext[44]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue45::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[45]);
  t->setThreadContext(sContext[45]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue46::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[46]);
  t->setThreadContext(sContext[46]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue47::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[47]);
  t->setThreadContext(sContext[47]);
  return (TaThread *)t;
}

//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue48::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[48]);
  t->setThreadContext(sContext[48]);
  return (TaThread *)t;
}
//----------------------------------------------------------------
// The needed clone method
TaThread *ConvWxThreadQue49::clone(const int index)
{
  if (sMethod == NULL || sContext == NULL)
  {
    ILOG(ERROR, "Cannot clone a thread, pointers not set");
    return NULL;
  }

  // simple thread with the method and context pointers as set
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadMethod(sMethod[49]);
  t->setThreadContext(sContext[49]);
  return (TaThread *)t;
}


//----------------------------------------------------------------
void ConvWxMultiThreadMgr::cleanup(void)
{
  for (int i=0; i<sNumThread; ++i)
  {
    TaThreadDoubleQue *thread = sThreadPtr(i);
    thread->reinit(0, false);
  }
}

//----------------------------------------------------------------
void ConvWxMultiThreadMgr::cleanup(const std::string &name)
{
  for (int i=0; i<sNumThread; ++i)
  {
    if (sName[i] == name)
    {
      TaThreadDoubleQue *thread = sThreadPtr(i);
      thread->reinit(0, false);
    }
  }
}

//----------------------------------------------------------------
bool ConvWxMultiThreadMgr::init(const std::string &name,
				int numThread, bool debug,
				void *context, void (*method)(void *))

{
  if (sNumThread >= 49)
  {
    ILOG(ERROR, "No more threads, maximum = 50");
    return false;
  }

  // See if already there
  for (int i=0; i<sNumThread; ++i)
  {
    if (sName[i] == name)
    {
      ILOGF(ERROR, "Already named thread in use %s", name.c_str());
      return false;
    }
  }
  
  sName[sNumThread] = name;
  sMethod[sNumThread] = method;
  sContext[sNumThread] = context;
  TaThreadDoubleQue *thread = sThreadPtr(sNumThread);
  if (thread != NULL)
  {
    thread->init(numThread, debug);
    sNumThread++;
    return true;
  }
  else
  {
    ILOGF(ERROR, "Index out of bounds %d", sNumThread);
    return false;
  }
}

//----------------------------------------------------------------
bool ConvWxMultiThreadMgr::reinit(const std::string &name,
				  int numThread, bool debug)
{
  // find the right one and initialize that one
  for (int i=0; i<sNumThread; ++i)
  {
    if (sName[i] == name)
    {
      TaThreadDoubleQue *thread = sThreadPtr(i);
      thread->reinit(numThread, debug);
      return true;
    }
  }
  ILOGF(ERROR, "No named thread %s", name.c_str());
  return false;
}
  
//----------------------------------------------------------------
bool ConvWxMultiThreadMgr::thread(const std::string &name,
				  int index, void *info)
{
  // find the right one and initialize that one
  for (int i=0; i<sNumThread; ++i)
  {
    if (sName[i] == name)
    {
      TaThreadDoubleQue *thread = sThreadPtr(i);
      thread->thread(index, info);
      return true;
    }
  }
  ILOGF(ERROR, "No named thread %s\n", name.c_str());
  return false;
}
  
//----------------------------------------------------------------
bool ConvWxMultiThreadMgr::lock(const std::string &name)
{
  // find the right one and initialize that one
  for (int i=0; i<sNumThread; ++i)
  {
    if (sName[i] == name)
    {
      TaThreadDoubleQue *thread = sThreadPtr(i);
      thread->lockForIO();
      return true;
    }
  }
  ILOGF(ERROR, "No named thread %s\n", name.c_str());
  return false;
}

//----------------------------------------------------------------
bool ConvWxMultiThreadMgr::unlock(const std::string &name)
{
  // find the right one and initialize that one
  for (int i=0; i<sNumThread; ++i)
  {
    if (sName[i] == name)
    {
      TaThreadDoubleQue *thread = sThreadPtr(i);
      thread->unlockAfterIO();
      return true;
    }
  }
  ILOGF(ERROR, "No named thread %s\n", name.c_str());
  return false;
}

//----------------------------------------------------------------
bool ConvWxMultiThreadMgr::waitForThreads(const std::string &name)
{
  // find the right one and initialize that one
  for (int i=0; i<sNumThread; ++i)
  {
    if (sName[i] == name)
    {
      TaThreadDoubleQue *thread = sThreadPtr(i);
      thread->waitForThreads();
      return true;
    }
  }
  ILOGF(ERROR, "No named thread %s\n", name.c_str());
  return false;
}



