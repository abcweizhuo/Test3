//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __MATHLIB_H
#define  __MATHLIB_H

#include "..\..\..\SMDK\Include\md_share1.h"

#if defined(__MATHLIB_CPP)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#ifndef __AFXTEMPL_H__
//#include <afxtempl.h> // "xafxtempl.h"  
#endif

//===========================================================================
//
//  
//
//===========================================================================

#ifndef _MANAGED

inline DWORD FPP_EnableExceptions()                               
  { 
  DWORD CW=_controlfp(0,0);                                                 // Get Old fpControl
  _clearfp();                                                               // clear any outstanding exceptions
  _controlfp(/*_EM_ZERODIVIDE|_EM_OVERFLOW|*/_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);	//Set New fpControl
  return CW;
  };

inline DWORD FPP_DisableExceptions()                               
  { 
  DWORD CW=_controlfp(0,0);     // Get Old fpControl
  _clearfp();                   // clear any outstanding exceptions
  _controlfp(_MCW_EM, _MCW_EM);	// Disable Exceptions;
  return CW;
  };

inline DWORD FPP_RestoreExceptions(DWORD CW)                               
  { 
  _clearfp();                       // clear any outstanding exceptions
  return _controlfp(CW, _MCW_EM);	  // Restore fpControl
  };

#endif

//#endif

//extern double Normal (int size, double *vector);
DllExport double Pow(double base, int exponent);

DllExport UINT FindNextPrimeNumber(UINT n);
DllExport int FindNextPrimeNumber(int n);
DllExport long FindNextPrimeNumber(long n);

/*#F:Find the cube roots of a Cubic equation*/
typedef double CubeRoots[3];
DllExport long FindNCubeRoots(double b, double c, double d, CubeRoots x);
DllExport double FindLargestCubeRoot(double a1, double a2, double a3);

//===========================================================================
//
//  
//
//===========================================================================

#if MOVEDTO_MD_SHARE_3
//Ascii Octal to int 
inline int aotoi(const char* p)
  {
  DWORD O=0;
  if (p)
    while (*p>='0' && *p<='7')
      {
      O*=8;
      O+=(*p)-(char)'0';
      p++;
      }
  return O;
  }

// -----------------------------------------------------------

inline int SafeAOtoI(const char * Str, int Default=0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (isdigit(*Str))// || *Str=='-' || *Str=='+')
      return aotoi(Str);
    }
  return Default;
  }

// -----------------------------------------------------------

DllExport double ParseNANFlag(const char * Str, double Val=dNAN);

inline double SafeAtoF(const char * Str, double Default=0.0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (*Str=='*')
      return ParseNANFlag(Str);
    if (isdigit(*Str) || *Str=='-' || *Str=='+' || *Str=='.')
      return atof(Str);
    }
  return Default;
  }

// -----------------------------------------------------------

inline long SafeAtoL(const char * Str, long Default=0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (isdigit(*Str) || *Str=='-' || *Str=='+')
      return atol(Str);
    }
  return Default;
  }

// -----------------------------------------------------------

inline int SafeAtoI(const char * Str, int Default=0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (isdigit(*Str) || *Str=='-' || *Str=='+')
      return atoi(Str);
    }
  return Default;
  }

//---------------------------------------------------------------------------

inline int SafeAlphatoI(const char * Str, int Default=0, int Base=26)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (Str && isalpha(*Str) && (toupper(*Str)-(char)'A')<Base)
      {
      const char* p = Str;
      int O = 0;
      while (isalpha(*Str) && (toupper(*Str)-(char)'A')<Base)
        {
        O *= Base;
        O += (char)toupper(*Str)-(char)'A'+1;
        Str++;
        }
      return O;
      }
    }
  return Default;
  }
#endif

//===========================================================================
//
//  
//
//===========================================================================

inline flag ConvergedDV(double D, double V, double ATol, double RTol, double & Err)
  {
  const double Vt = Max(fabs(V), fabs(V + D));
  Err = fabs(D) / (ATol + Vt * RTol);
  return (Err < 1.0);
  }

inline flag ConvergedDV(double D, double V, double ATol, double RTol)
  {
  const double Vt = Max(fabs(V), fabs(V + D));
  const double Err = fabs(D) / (ATol + Vt * RTol);
  return (Err < 1.0);
  }

inline flag ConvergedVV(double V1, double V2, double ATol, double RTol, double & Err)
  {
  const double Vt = Max(fabs(V1), fabs(V2));
  Err = fabs(V2-V1) / (ATol + Vt * RTol);
  return (Err < 1.0);
  }

inline flag ConvergedVV(double V1, double V2, double ATol, double RTol)
  {
  const double Vt = Max(fabs(V1), fabs(V2));
  const double Err = fabs(V2-V1) / (ATol + Vt * RTol);
  return (Err < 1.0);
  }

inline flag ConvergedDV_NANOK(double D, double V, double ATol, double RTol, double & Err)
  {
  bool f1=Valid(D);
  bool f2=Valid(V);
  if (f1 && f2)
    {
    const double Vt = Max(fabs(V), fabs(V + D));
    Err = fabs(D) / (ATol + Vt * RTol);
    return (Err < 1.0);
    }
  if (f1==f2)
    {
    Err=0.1;
    return true;
    }
  Err=10;
  return true;
  }

inline flag ConvergedVV_NANOK(double V1, double V2, double ATol, double RTol, double & Err)
  {
  bool f1=Valid(V1);
  bool f2=Valid(V2);
  if (f1 && f2)
    {
    const double Vt = Max(fabs(V1), fabs(V2));
    Err = fabs(V2-V1) / (ATol + Vt * RTol);
    return (Err < 1.0);
    }
  if (f1==f2)
    {
    Err=0.1;
    return true;
    }
  Err=10;
  return true;
  }

//===========================================================================
//
//  
//
//===========================================================================

const int MaxSigmaXn=3;
const int MaxSigmaXnY=2;

class DllImportExport LeastSquareFit
  {
  protected:
    int       m_N;
    double    m_Xn[MaxSigmaXn];
    double    m_XnY[MaxSigmaXnY];
  public:

    LeastSquareFit() { Initialise(); }
    void    Initialise();
    void    AddPt(double X, double Y);
    void    RemovePt(double X, double Y);
    double  Param(int iParam, int iOrder=1);

  };

// ===========================================================================

const DWORD   TD_Time               = 0x0001;
const DWORD   TD_DateLeft           = 0x0002;
const DWORD   TD_DateRight          = 0x0004;
const DWORD   TD_Days               = 0x0008;
const DWORD   TD_DaysJoined         = 0x0010;
const DWORD   TD_IntegralSecs       = 0x0020;
const DWORD   TD_TrimTime           = 0x0040;
const DWORD   TD_Seconds            = 0x0080;
const DWORD   TD_DateStrFormat      = 0x0100;
const DWORD   TD_TimeOnly           = 0x0200;

const DWORD   TD_DateTime           = TD_Time|TD_DateLeft;
const DWORD   TD_TimeDate           = TD_Time|TD_DateRight;

class DllImportExport CTimeValue
  {
  public:
    static const long   Divisor;
    static const double MinResolution;
    static const __int64 NANMask;

    CTimeValue()                        { m_mSecs = 0; };
    CTimeValue(double T)                { if (Valid(T)) Seconds = T; else m_mSecs = NANMask; };
    CTimeValue(__int64 T)               { m_mSecs = T; };
    CTimeValue(const CTimeValue & T)    { m_mSecs = T.m_mSecs; };
    CTimeValue(const time_t & T)        { m_mSecs = T*Divisor; };
    CTimeValue(LPCTSTR Str)             { m_mSecs = 0; Parse(Str); };

    __int64    getmSeconds() const      { return m_mSecs; };
    void       putmSeconds(__int64 T)   { m_mSecs = T; };
    double     getSeconds() const       { return m_mSecs / (double)Divisor; };
    void       putSeconds(double T);
    __int64    getRaw() const           { return m_mSecs; };
    void       putRaw(__int64 T)        { m_mSecs = T; };
    __int64    getSecondsI64() const    { return m_mSecs / Divisor; };
    void       putSecondsI64(__int64 T) { m_mSecs = T*Divisor; };


    __declspec(property(get=getSeconds,put=putSeconds))             double  Seconds;
    __declspec(property(get=getSecondsI64,put=putSecondsI64))       __int64 SecondsI64;
    __declspec(property(get=getmSeconds,put=putmSeconds))           __int64 mSeconds;
    __declspec(property(get=getRaw,put=putRaw))                     __int64 Raw;

    int getTimemSeconds();
    int getTimeSeconds();
    int getTimeMinutes();
    int getTimeHours();
    int getTimeDays();       
    int getDateDay();
    int getDateMonth();
    int getDateYear();   
    int getDaysInMonth();
    int getDaysInYear();
    int getDayOfWeek(); //1=Sunday
    int getDayOfYear();
    LPCTSTR getDayOfWeekDesc();
    LPCTSTR getDateMonthDesc();  

    __declspec(property(get=getTimemSeconds))                       int     TimemSeconds;
    __declspec(property(get=getTimeSeconds))                        int     TimeSeconds;
    __declspec(property(get=getTimeMinutes))                        int     TimeMinutes;
    __declspec(property(get=getTimeHours))                          int     TimeHours;
    __declspec(property(get=getTimeDays))                           int     TimeDays;
    __declspec(property(get=getDateDay))                            int     DateDay;
    __declspec(property(get=getDateMonth))                          int     DateMonth;
    __declspec(property(get=getDateYear))                           int     DateYear;
    __declspec(property(get=getDaysInMonth))                        int     DaysInMonth;
    __declspec(property(get=getDaysInYear))                         int     DaysInYear;
    __declspec(property(get=getDayOfWeek))                          int     DayOfWeek;
    __declspec(property(get=getDayOfYear))                          int     DayOfYear;
    __declspec(property(get=getDayOfWeekDesc))                      LPCTSTR DayOfWeekDesc;
    __declspec(property(get=getDateMonthDesc))                      LPCTSTR DateMonthDesc;

    CTimeValue & operator =(const CTimeValue & T1)      { m_mSecs=T1.m_mSecs; return *this; };
    //CTimeValue & operator =(const double T1)            { *this=CTimeValue(T1); return *this; };
    
    CTimeValue operator +(const CTimeValue & T1) const  { CTimeValue X; X.m_mSecs = m_mSecs+T1.m_mSecs; return X; };
    CTimeValue operator -(const CTimeValue & T1) const  { CTimeValue X; X.m_mSecs = m_mSecs-T1.m_mSecs; return X; };
    double     operator *(const CTimeValue & T1) const  { return Seconds*T1.Seconds; };
    double     operator /(const CTimeValue & T1) const  { return Seconds/T1.Seconds; };
    CTimeValue operator +(const double v) const         { CTimeValue X; X.m_mSecs = m_mSecs+__int64(v*Divisor); return X; };
    CTimeValue operator -(const double v) const         { CTimeValue X; X.m_mSecs = m_mSecs-__int64(v*Divisor); return X; };
    CTimeValue operator *(const double v) const         { CTimeValue X; X.m_mSecs = __int64(m_mSecs*v); return X; };
    CTimeValue operator /(const double v) const         { CTimeValue X; X.m_mSecs = __int64(m_mSecs/v); return X; };
    CTimeValue operator +=(const CTimeValue & T1)       { m_mSecs = m_mSecs+T1.m_mSecs; return *this; };
    CTimeValue operator -=(const CTimeValue & T1)       { m_mSecs = m_mSecs-T1.m_mSecs; return *this; };

    CTimeValue Round(CTimeValue & T1)                   { CTimeValue T2=((m_mSecs + T1.m_mSecs/2)/T1.m_mSecs) * T1.m_mSecs; return T2; };
    CTimeValue GreaterThan(CTimeValue & T1)             { CTimeValue T2=Max(m_mSecs, T1.m_mSecs+1); return T2; };

    bool       operator >=(const CTimeValue & T1) const { return m_mSecs>=T1.m_mSecs; };
    bool       operator > (const CTimeValue & T1) const { return m_mSecs> T1.m_mSecs; };
    bool       operator ==(const CTimeValue & T1) const { return m_mSecs==T1.m_mSecs; };
    bool       operator !=(const CTimeValue & T1) const { return m_mSecs!=T1.m_mSecs; };
    bool       operator < (const CTimeValue & T1) const { return m_mSecs< T1.m_mSecs; };
    bool       operator <=(const CTimeValue & T1) const { return m_mSecs<=T1.m_mSecs; };

    bool       operator >=(const double T1) const { return m_mSecs>=T1*Divisor; };
    bool       operator > (const double T1) const { return m_mSecs> T1*Divisor; };
    bool       operator ==(const double T1) const { return m_mSecs==T1*Divisor; };
    bool       operator !=(const double T1) const { return m_mSecs!=T1*Divisor; };
    bool       operator < (const double T1) const { return m_mSecs< T1*Divisor; };
    bool       operator <=(const double T1) const { return m_mSecs<=T1*Divisor; };

    operator double() { return getSeconds(); }

    LPCTSTR Format(DWORD Options) const;
    LPCTSTR Format(LPCTSTR CnvStr) const;
    bool    Parse(LPCTSTR Str);

  protected:
    __int64    m_mSecs; // in milliSeconds
  };

extern DllImportExport CTimeValue TimeNAN;
inline bool ValidTime(CTimeValue T) { return T!=TimeNAN; }

inline CTimeValue Min(CTimeValue T1, CTimeValue T2) { return T1.mSeconds<T2.mSeconds ? T1:T2; };
inline CTimeValue Max(CTimeValue T1, CTimeValue T2) { return T1.mSeconds>T2.mSeconds ? T1:T2; };

//===========================================================================
//
//  
//
//===========================================================================

class CPointF
  {
  public:
    CPointF()
      {
      m_X = 0.0;
      m_Y = 0.0;
      };
    CPointF(double x, double y)
      {
      m_X = float(x);
      m_Y = float(y);
      }
    CPointF(float x, float y)
      {
      m_X = x;
      m_Y = y;
      }

    void Set(double x,double y)
      {
      m_X = float(x);
      m_Y = float(y);
      }

    void Set(float x,float y)
      {
      m_X = x;
      m_Y = y;
      }

    float  X()    const { return m_X;      };
    float  Y()  const { return m_Y;      };

  protected:
    float   m_X;
    float   m_Y;
  };

//---------------------------------------------------------------------------

class CPointFList : public CList<CPointF, CPointF&> 
  {
  };

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

class CRectangleF
  {
  public:
    CRectangleF()
      {
      m_X = 0.0;
      m_Y = 0.0;
      m_W = 0.0;
      m_H = 0.0;
      };
    CRectangleF(double x, double y, double w, double h)
      {
      m_X = float(x);
      m_Y = float(y);
      m_W = float(w);
      m_H = float(h);
      }
    CRectangleF(float x, float y, float w, float h)
      {
      m_X = x;
      m_Y = y;
      m_W = w;
      m_H = h;
      }

    void Set(double x,double y,double w,double h)
      {
      m_X = float(x);
      m_Y = float(y);
      m_W = float(w);
      m_H = float(h);
      }

    void Set(float x,float y,float w,float h)
      {
      m_X = x;
      m_Y = y;
      m_W = w;
      m_H = h;
      }

    void Set(const CPointF & pt)
      {
      m_X = pt.X();
      m_Y = pt.Y();
      m_W = 0;
      m_H = 0;
      }
    void Include(const CPointF & pt, bool PositiveUp)
      {
      if (pt.X()<m_X)
        {
        m_W += m_X-pt.X();
        m_X = pt.X();
        }
      else
        m_W = Max(m_W, pt.X()-m_X);

      if (PositiveUp)
        {
        if (pt.Y()<m_Y)
          {
          m_H += m_Y-pt.Y();
          m_Y = pt.Y();
          }
        else
          m_H = Max(m_H, pt.Y()-m_Y);
        }
      else
        {
        if (pt.Y()<m_Y)
          {
          m_H += m_Y-pt.Y();
          m_Y = pt.Y();
          }
        else
          m_H = Max(m_H, pt.Y()-m_Y);
        }

      }

    float  Left()    const { return m_X;      };
    float  Bottom()  const { return m_Y;      };
    float  Right()   const { return m_X+m_W;  };
    float  Top()     const { return m_Y+m_H;  };

    float  Width()   const { return m_W; };
    float  Height()  const { return m_H; };
    float  MidX()    const { return m_X+0.5f*m_W; };
    float  MidY()    const { return m_Y+0.5f*m_H; };

    bool   Contains(const CRectangleF & rct, bool PositiveUp) 
      {
      if (PositiveUp)
        return rct.Left()>= Left() && rct.Right()<= Right() && rct.Bottom()>= Bottom() && rct.Top()<=Top();
      else
        return rct.Left()>= Left() && rct.Right()<= Right() && rct.Top()>=Top() && rct.Bottom()<= Bottom();
      }
    bool   Contains(const CPointF & pt, bool PositiveUp)
      {
      if (PositiveUp)
        return pt.X()>= Left() && pt.X()<= Right() && pt.Y()>= Bottom() && pt.Y()<=Top();
      else
        return pt.X()>= Left() && pt.X()<= Right() && pt.Y()>=Top() && pt.Y()<= Bottom();
      }
  protected:
    float   m_X;
    float   m_Y;
    float   m_W;
    float   m_H;
  };

//========================================================================
//
//
//
//========================================================================

#undef DllImportExport 

#endif  /* __MATHLIB_H */
