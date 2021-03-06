//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __EQNSOLVE_H
#define  __EQNSOLVE_H

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif

#include "sc_defs.h"
#include "scdarray.h"
#include "tagobj.h"
#include "dataattr.h"
//#include "executiv.h"

#ifdef __EQNSOLVE_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// =========================================================================
//
//
//
// =========================================================================

_FWDDEF(EqnSlvCtrlBlk);
_FWDDEF(TearVarBlk);
_FWDDEF(TearPosBlk);

// =========================================================================
//
//
//
// =========================================================================

const int  MaxTearHistory = 50;

// Tear Type
const byte TT_NoTear        = 0;
const byte TT_SystemTear    = 1;
const byte TT_ManualTear    = 2;
const byte TT_Break         = 3;

// Tear Priority
const byte TP_First         = 0;
const byte TP_Normal        = 1;
const byte TP_Last          = 2;

// Tear Initial Mode
const byte TIW_OnInit       = 1;
const byte TIW_OnStart      = 2;
const byte TIW_Always       = 3;
const byte TIW_Manual       = 4;

const byte TIH_Off          = 0;
const byte TIH_ZeroNIters   = 1;
const byte TIH_HoldNIters   = 2;
const byte TIH_RampNIters   = 3;
const byte TIH_Zeroing      = 4;
const byte TIH_Holding      = 5;
const byte TIH_Ramping      = 6;

const byte TIEU_None        = 0;
const byte TIEU_FullEst     = 1;
const byte TIEU_PartEst     = 2;

const byte TVBTS_Default    = 0;
const byte TVBTS_Maximum    = 1;
const byte TVBTS_Minimum    = 2;
const byte TVBTS_Priority   = 3;

const byte TVT_None         = 0x00;
const byte TVT_Tear         = 0x01;
const byte TVT_Test         = 0x02;

const byte TVU_Inputs       = 0x01;
const byte TVU_Outputs      = 0x02;
const byte TVU_Meas         = 0x04;
const byte TVU_AdvanceVars  = 0x08;
const byte TVU_TestVars     = 0x10;

//Tag Tear Types...
const byte TTT_Unknown      = 0x00;
const byte TTT_Tear         = 0x01;
const byte TTT_FlashTrain   = 0x02;
const byte TTT_PID          = 0x03;

//=========================================================================
//
//
//
//=========================================================================

class DllImportExport TearVar
  {
  friend class TearVarBlk;
  friend class TearVarBlkEdt;
  friend class EqnSolverBlk;
  public:
    TearVar();

    int            TearMethod();
    LPSTR          TearMethodStr(DDEF_Flags * Flags);
    bool           HoldOutput(); 
    LPSTR          HoldOutputStr(DDEF_Flags * Flags); 
  public:
    // Common
    TearVarBlk   * m_pBlk;

    Strng          m_Tag;
    Strng          m_Sym;
    //Strng          m_Name;

    // Tear advance
    flag           m_bTestIsValid;
    byte           m_iConvergedCnt;
    double         m_DampingRqd;
    double         m_DampFactor;
    double         m_EstDampFactor;

    byte           m_iHoldOutput;
    byte           m_iTearMethod;
    
    double         m_YRat;
    CDArray        m_X;
    CDArray        m_Y;

    // Convergence
    double         m_EPS_Abs;
    double         m_EPS_Rel;
    double         m_CurTol;
    CDArray        m_Error;
    CDArray        m_Meas;
    double         m_Initial;

    CnvAttribute * m_pMeasCnv;
    CnvAttribute * m_pDiffCnv;

    FmtAttribute * m_pMeasFmt; //ToGo
    FmtAttribute * m_pDiffFmt; //ToGo


  };

typedef CArray <TearVar, TearVar&> TearVarArray;

//=========================================================================
//
//
//
//=========================================================================

typedef CList <TearPosBlk*, TearPosBlk*> TearPosBlkList;
typedef CSCDPtrListIter<TearPosBlkList, pTearPosBlk> TearPosBlkIter;

class DllImportExport TearPosBlk //: public TaggedObject
  {
  friend class EqnSolverBlk;

  public:
    TearPosBlk(byte DefaultTearType);
    virtual        ~TearPosBlk();

    virtual TearVarBlk* CreateVarBlk(LPSTR Tag, pTaggedObject pAttach)=0;

    void           ConnectTear(flag CreateIfReqd);
    void           DisConnectTear();
    void           ActivateTear();
    void           DeActivateTear();

    virtual void   SetRqdTearType(byte Type)    { m_iRqdTearType=Range(TT_NoTear, Type, TT_Break); };
    byte           RqdTearType()                { return m_iRqdTearType; };
    virtual void   SetTearType(byte Type)       { m_iTearType = Type; };
    byte           TearType()                   { return m_iTearType; };
    flag           IsTear()                     { return m_iTearType != TT_NoTear; };
    flag           IsSystemTear()               { return m_iTearType == TT_SystemTear; };
    byte           TearPriority()               { return m_iTearPriority; };
    virtual void   SetTearPriority(byte iPri)   { m_iTearPriority=iPri; };
    //byte           TearInitMode()               { return m_iTearInitMode; };
    byte           TearInitHow()                { return m_iTearInitHow; };
    byte           TearInitWhen()               { return m_iTearInitWhen; };
    byte           TearInitEstUsage()           { return m_iTearInitEstUsage; };
    bool           TearInitActive();
    virtual void   SetTearInitWhen(byte iMd)    { m_iTearInitWhen = iMd; };
    virtual void   SetTearInitHow(byte iMd)     { m_iTearInitHow = iMd; };
    virtual void   SetTearInitEstUsage(byte iEu) { m_iTearInitEstUsage= iEu; };
    void           SetTearInitActive(bool On, bool SetCount);
    int            TearHoldRqdCnt()             { return m_iTearHoldRqdCnt; };
    virtual void   SetTearHoldRqdCnt(int iCnt)  { m_iTearHoldRqdCnt=Max(1, iCnt); };
    int            TearHoldCount()              { return m_iTearHoldCount; };
    virtual void   SetTearHoldCount(int iCnt)   { m_iTearHoldCount=Max(0, iCnt); };

    void           SetTearInitCounters(bool AtStart);
    void           BumpTearInitCounters();
    double         EstimatePortion();

    virtual byte   TearVarType()                                  { return TVT_None; };
    virtual void   TearInitialiseOutputs(double EstimatePortion)  {};
    virtual void   TearInputs2Outputs()                           {};
    virtual LPSTR  TearGetTag()                                   { m_sTearTag=""; return NULL; };
    virtual int    TearGetConvergeInfo(TearVarArray & TV)         { return 0; };
    virtual void   TearGetErrors(TearVarArray & TV)               { };
    virtual void   TearGetInputs(TearVarArray & TV)               { };
    virtual void   TearGetOutputs(TearVarArray & TV)              { };
    virtual void   TearSetOutputs(TearVarArray & TV)              { };
    virtual flag   TearAdvance(TearVarArray & TV, EqnSlvCtrlBlk & EqnCB) { return 0; };

  protected:
    POSITION       m_ListPos;

  public:
    static TearPosBlkList sm_List;

  public:
    byte           m_iRqdTearType,
                   m_iTearType,     // Tear Type
                   m_iTearPriority, // Tear Priority
                   m_iTearInitWhen, // Initialise Time
                   m_iTearInitHow,  // Initialise How to Init (Zero,Hold,Ramp)
                   m_iTearInitEstUsage;  // Initialise Estimate usage
    int            m_iTearHoldRqdCnt,
                   m_iTearHoldCount;

    TearVarBlk    *m_pVarBlk;
    Strng          m_sTearTag;

  };


//=========================================================================
//
//
//
//=========================================================================

extern DllImportExport DDBValueLst DDBTearPri[];
extern DllImportExport DDBValueLst DDBTearTypeRqd[];
extern DllImportExport DDBValueLst DDBTearState[];

const byte TCM_Default      = 0;
const byte TCM_DirectSubs   = 1;
const byte TCM_AdaptSubs    = 2;
const byte TCM_Wegstein     = 3;
const byte TCM_Kalman       = 4;
const byte TCM_Last         = 5;

const byte THO_Default      = 0;
const byte THO_Off          = 1;
const byte THO_On           = 2;

const byte DAG_Default      = 0;
const byte DAG_Off          = 1;
const byte DAG_On           = 2;

extern DllImportExport LPSTR TearMethodStrings[];
extern DllImportExport LPSTR TearMethodDefStrings[];
extern DllImportExport DDBValueLst DDBTearMethod[];
extern DllImportExport DDBValueLst DDBTearMethodDef[];

extern DllImportExport LPSTR EPSStrategyStrings[];
extern DllImportExport LPSTR EPSStrategyDefStrings[];
extern DllImportExport DDBValueLst DDBEPSStrategy[];
extern DllImportExport DDBValueLst DDBEPSStrategyDef[];

extern DllImportExport LPSTR HoldOutputStrings[];
extern DllImportExport LPSTR HoldOutputDefStrings[];
extern DllImportExport DDBValueLst DDBHoldOutput[];
extern DllImportExport DDBValueLst DDBHoldOutputDef[];

extern DllImportExport LPSTR DampAsGroupStrings[];
extern DllImportExport LPSTR DampAsGroupDefStrings[];
extern DllImportExport LPSTR DampAsGroupStringsShort[];
extern DllImportExport LPSTR DampAsGroupDefStringsShort[];
extern DllImportExport DDBValueLst DDBDampAsGroup[];
extern DllImportExport DDBValueLst DDBDampAsGroupDef[];

extern DllImportExport  int TearConvergeMethod(int i, int j=-1, int k=-1);
extern DllImportExport  LPSTR TearConvergeMethodStr(int i, int j=-1, int k=-1, DDEF_Flags * Flags=NULL);
extern DllImportExport  int FindTearConvergeMethod(LPCSTR Str);

extern DllImportExport  int HoldOutput(int i, int j=-1, int k=-1);
extern DllImportExport  LPSTR HoldOutputStr(int i, int j=-1, int k=-1, DDEF_Flags * Flags=NULL);
extern DllImportExport  int FindHoldOutput(LPCSTR Str);

extern DllImportExport  int DampAsGroup(int i, int j=-1, int k=-1);
extern DllImportExport  LPSTR DampAsGroupStr(int i, int j=-1, int k=-1, DDEF_Flags * Flags=NULL);
extern DllImportExport  LPSTR DampAsGroupStrShort(int i, int j=-1, int k=-1);
extern DllImportExport  int FindDampAsGroup(LPCSTR Str);

extern DllImportExport  int EPSStrategy(int i, int j=-1, int k=-1);
extern DllImportExport  LPSTR EPSStrategyStr(int i, int j=-1, int k=-1, DDEF_Flags * Flags=NULL);
extern DllImportExport  int FindEPSStrategy(LPCSTR Str);

//=========================================================================
//
//
//
//=========================================================================

#if WithTearVarBlkEdit
DEFINE_TAGOBJEDT(TearVarBlk);
#else
DEFINE_TAGOBJ(TearVarBlk);
#endif

typedef CList <TearVarBlk*, TearVarBlk*> TearVarBlkList;
typedef CSCDPtrListIter<TearVarBlkList, TearVarBlk*> TearVarBlkIter;

class DllImportExport TearVarBlk : public TaggedObject
  {
#if WithTearVarBlkEdit
  friend class TearVarBlkEdt;
#endif
  friend class EqnSolverBlk;
  friend class TearVar;
  friend class TearObjectEdt;
  friend class CFlange;

  public:
    static TearVarBlk *Find(LPSTR Tag);
    static TearVarBlk *staticFindObjTag(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen);
    static TearVarBlk *Add(TearPosBlk *Pos, LPSTR Tag);
    static TearVarBlk *Add(TearVarBlk *Blk);
    static void Delete(TearVarBlk *Var);
    static void CleanUp();

    TearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    TearVarBlk(pchar Tag_, pTaggedObject pAttach);
    virtual ~TearVarBlk();
    void           CommonConstruct();

    virtual void   OnEditConstruct() {};
    virtual void   OnEditDestroy() {};

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    void           BuildDataDefnFile(DataDefnBlk & DDB);
    void           BuildDataDefnTearValues(DataDefnBlk & DDB);
    void           BuildDataDefnTearMethod(DataDefnBlk & DDB);
    virtual void   BuildDataDefnInitialVars(DataDefnBlk & DDB);
    virtual void   BuildCommonSEDefn(DataDefnBlk &DDB)           {}; // Hide 'Info Page' for TearVarBlks
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };

    static void    BuildDataDefnGlblTears(DataDefnBlk & DDB, TaggedObject * pOwner);
    static flag    DataXchgGlblTears(DataChangeBlk & DCB);

    void           Connect(TearPosBlk *PosBlk);//, flag CreateIfReqd);
    void           DisConnect();
    int            Active() { return TaggedObject::GetActive() && (m_pPosBlk !=NULL); };
    int            InUse() { return m_bInUse; };
    void           SetInUse(int InUse) { m_bInUse=InUse; };
    void           SetActive(int On);

    byte           ConvergeMethod()             { return m_iTearMethod; };
    LPCTSTR        ConvergeMethodStr()          { return ::TearConvergeMethodStr(m_iTearMethod); }
    void           SetConvergeMethod(byte i)    { m_iTearMethod=i; };
    bool           SetConvergeMethod(LPCTSTR Str)
      {
      int i=::FindTearConvergeMethod(Str);
      if (i>=0)
        {
        m_iTearMethod=i;
        return true;
        }
      return false;
      };
    byte           EPSStrategy()                { return m_iEPSStrategy; };
    LPCTSTR        EPSStrategyStr()             { return ::EPSStrategyStr(m_iEPSStrategy); };
    void           SetEPSStrategy(byte i)       { m_iEPSStrategy=i; };
    bool           SetEPSStrategy(LPCTSTR Str)
      {
      int i=::FindEPSStrategy(Str);
      if (i>=0)
        {
        m_iEPSStrategy=i;
        return true;
        }
      return false;
      //for (int i=0; i<TCM_Last; i++)
      //  {
      //  if (_stricmp(Str, EPSStrategyStrings[i])==0)
      //    {
      //    m_iEPSStrategy=i;
      //    return true;
      //    }
      //  }
      //return false;
      };

    void           SetNVariables(int Variables, byte Used);

    int            NVariables() { return m_nVariables; };
    void           SetNHistory(int History);
    int            NHistory() { return m_nHistory; };
    long           SeqConvergedCnt() { return m_lSeqConvergedCnt; };
    long           ConvergeLoopCnt() { return m_lConvergeLoopCnt; };
    byte           TearTagTyp() { return iTearTagTyp; };
    char*          LHSTag(Strng & Tg);
    char*          RHSTag(Strng & Tg);

    TearPosBlk    *PosBlk() { return m_pPosBlk; };

    void           SetRqdTearType(byte Type)  { m_pPosBlk->SetRqdTearType(Type); };
    byte           RqdTearType()              { return m_pPosBlk->RqdTearType(); };
    void           SetTearType(byte Type)     { m_pPosBlk->SetTearType(Type); };
    byte           TearType()                 { return m_pPosBlk->TearType(); };
    flag           IsTear()                   { return m_pPosBlk->IsTear(); };
    flag           IsSystemTear()             { return m_pPosBlk->IsSystemTear(); };
    byte           TearPriority()             { return m_pPosBlk->TearPriority(); };
    void           SetTearPriority(byte iPri) { m_pPosBlk->SetTearPriority(iPri); };
    //byte           TearInitMode()             { return m_pPosBlk->TearInitMode(); };
    byte           TearInitWhen()             { return m_pPosBlk->TearInitWhen(); };
    byte           TearInitHow()              { return m_pPosBlk->TearInitHow(); };
    byte           TearInitEstUsage()         { return m_pPosBlk->TearInitEstUsage(); };
    bool           TearInitActive()           { return m_pPosBlk->TearInitActive(); };
    //void           SetTearInitMode(byte iMd)  { m_pPosBlk->SetTearInitMode(iMd); };
    void           SetTearInitWhen(byte iMd)  { m_pPosBlk->SetTearInitWhen(iMd); };
    void           SetTearInitHow(byte iMd)   { m_pPosBlk->SetTearInitHow(iMd); };
    void           SetTearInitEstUsage(byte iEu) { m_pPosBlk->SetTearInitEstUsage(iEu); };
    void           SetTearInitActive(bool On, bool SetCount) { m_pPosBlk->SetTearInitActive(On, SetCount); };
    void           SetTearInitCounters(bool AtStart) { m_pPosBlk->SetTearInitCounters(AtStart); };
    int            TearHoldRqdCnt()             { return m_pPosBlk->TearHoldRqdCnt(); };
    void           SetTearHoldRqdCnt(int iCnt) { m_pPosBlk->SetTearHoldRqdCnt(iCnt); };
    int            TearHoldCount()             { return m_pPosBlk->TearHoldCount(); };
    void           SetTearHoldCount(int iCnt) { m_pPosBlk->SetTearHoldCount(iCnt); };
    void           BumpTearInitCounters()     { m_pPosBlk->BumpTearInitCounters(); };
    double         EstimatePortion()          { return m_pPosBlk->EstimatePortion(); };

    bool           TearAdvanceReqd()          { return m_pPosBlk && (m_pPosBlk->TearVarType() & TVT_Tear) !=0; };
    bool           TearTestReqd()             { return m_pPosBlk && (m_pPosBlk->TearVarType() & (TVT_Test))!=0; };

    void           TearInitialiseOutputs(double EstimatePortion) { m_pPosBlk->TearInitialiseOutputs(EstimatePortion); };
    void           TearInputs2Outputs()       { m_pPosBlk->TearInputs2Outputs(); };
    LPSTR         TearGetTag()               { return m_pPosBlk->TearGetTag(); };
    int            TearGetConvergeInfo()      { return m_pPosBlk->TearGetConvergeInfo(TV); };
    void           TearGetErrors()            { m_pPosBlk->TearGetErrors(TV); };
    void           TearGetInputs()            { m_pPosBlk->TearGetInputs(TV); };
    void           TearGetOutputs()           { m_pPosBlk->TearGetOutputs(TV); };
    void           TearSetOutputs()           { m_pPosBlk->TearSetOutputs(TV); };
    flag           TearAdvance(EqnSlvCtrlBlk & EqnCB) { return m_pPosBlk->TearAdvance(TV, EqnCB); };

    int            IsGroupBlk() { return fGroupBlk; };

    double         DampingRqd(EqnSlvCtrlBlk & EqnCB, int iVar);
    double         EPS_Abs(EqnSlvCtrlBlk & EqnCB, int iVar);
    double         EPS_Rel(EqnSlvCtrlBlk & EqnCB, int iVar);

    byte           DampAsGroupCfg() { return m_iDampAsGroup; };
    double         DampingRqdCfg()  { return m_DampingRqd; };
    double         EPS_AbsCfg()     { return m_EPS_Abs; };
    double         EPS_RelCfg()     { return m_EPS_Rel; };
    void           SetDampAsGroupCfg(byte On) { m_iDampAsGroup=On; };
    void           SetDampAsGroupCfg(LPSTR Str)
      {
      for (int i=0; i<TCM_Last; i++)
        {
        if (_stricmp(Str, DampAsGroupStrings[i])==0)
          {
          m_iDampAsGroup=i;
          return;
          }
        }
      };
    void           SetDampingRqdCfg(double Val) { m_DampingRqd=Val; };
    void           SetEPS_AbsCfg(double Val)    { m_EPS_Abs=Val; };
    void           SetEPS_RelCfg(double Val)    { m_EPS_Rel=Val; };

    void           SetHoldDamping(bool On)   { m_bHoldDamping=On; };
    flag           HoldDamping()             { return m_bHoldDamping; };

  protected:
    void           Initialise(bool ForceIt);
    void           BeginStep(EqnSlvCtrlBlk & TCB);
    void           EndStep(EqnSlvCtrlBlk & TCB);
    flag           TestConverged(EqnSlvCtrlBlk & TCB, double &Error, Strng &ErrTag);
    void           RotateInputs();
    void           RotateErrors();
    void           RotateOutputs();

    void           AdvWegstein(EqnSlvCtrlBlk & TCB);
    void           AdvDirect(EqnSlvCtrlBlk & TCB, double Damping);
    void           AdvAdaptDirect(EqnSlvCtrlBlk & TCB, double Damping);
    void           Advance(EqnSlvCtrlBlk & TCB);

    LPSTR          SymOrTag(int i) { return TV[i].m_Sym.Length()>0 ? TV[i].m_Sym() : TV[i].m_Tag(); };

    void           GetGroupGlblTag(Strng & Tag) { Tag.Set("%s_%s", sGroupTag(), "Tear"); };

  protected:
  public:
    static TearVarBlkList sm_List;

  protected:
    POSITION       m_ListPos;

    byte           bUsedByBlk;

    byte           m_iHoldOutput;
    byte           m_iTearMethod;
    flag           m_bInUse;
    flag           fGroupBlk;

    TearPosBlk    *m_pPosBlk;
    long           m_nVariables;
    long           m_nHistory;

    byte           m_iDampAsGroup;
    double         m_DampingRqd;
    flag           m_bHoldDamping;
    double         m_DampFctGrowth;
    double         m_DampFctDecay;

    byte           m_iEPSStrategy;
    double         m_EPS_Abs;
    double         m_EPS_Rel;

    int            m_iTagWidth;
    int            m_iSymWidth;

    //byte           m_iDampAsGroup;
    //double         m_DampingRqd;
    //double         m_DampFctGrowth;
    //double         m_DampFctDecay;

    //short          m_iWA_Delay;
    //double         m_WA_Bound;
    //double         m_WA_Clamping;

    byte           iTearTagTyp;
    Strng          sGroupTag;
    TearVarBlk *   pGroupInfo;

    double         m_GrpDampFactor;
    long           m_lStageCnt;
    //Strng          RootTag;

    TearVarArray   TV;

    long           m_lSeqConvergedCnt;
    long           m_lConvergeLoopCnt;

  };

//=========================================================================
//
//
//
//=========================================================================

#if WithTearVarBlkEdit
class DllImportExport TearVarBlkEdt : public FxdEdtBookRef
  {
  public :
    TearVarBlk &   rTVB;
    CRect          GraphR;
    int            iDragPtNo;  //the index of the point being moved (-1 if none)
    int            iNameWidth; //
    int            iPg1;
    CnvAttribute   DampCnv, MeasCnv, DiffCnv, EPSCnv;//, *pWrkCnv;
    FmtAttribute   DampFmt, MeasFmt, DiffFmt, EPSFmt;//, *pWrkFmt;

    TearVarBlkEdt(FxdEdtView *pView_, TearVarBlk *pTVB_);//, rStrng Tag, rStrng Name);
    virtual ~TearVarBlkEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();

  private:

  };
#endif

//=========================================================================
//
//
//
//=========================================================================

const int MAX_EQNSLV_WORST = 5;
const int MAX_EQNSLV_MSGLEN = 96;
class DllImportExport WorstEqn
  {
  public:
    double         dErr;
    byte           iTearTagTyp;
    char           cStr[MAX_EQNSLV_MSGLEN];

    WorstEqn()     { dErr=0.0; cStr[0]=0; iTearTagTyp=TTT_Unknown; };
  };

//=========================================================================
//
//  EqnSlv Control .. Seperate because it is needed across dll interface
//
//=========================================================================
#define ALLOWMASSSMOOTH 0
//NB: MassSmoothing in ProBal solver matrix does not work correctly. Until fixed disable this!!!

class DllImportExport EqnSlvCfgBlk
  {
  public:
    byte           m_iConvergeMeth;
    flag           m_bMassSmooth;
    flag           m_bForceConverged:1,
                   m_bIdleWhenDone:1;
    short          m_iRqdCnvrgdIters;
    long           m_iMaxIters;

    byte           m_iHoldOutput;

    byte           m_iEPSStrategy;
    double         m_EPS_Rel;
    double         m_EPS_Abs;

    byte           m_iDampAsGroup;
    double         m_DampingRqd;
    double         m_DampFctGrowth;
    double         m_DampFctDecay;

    short          m_iWA_Delay;
    double         m_WA_Bound;
    double         m_WA_Clamping;

    EqnSlvCfgBlk() { InitProBal(); };

    void           InitProBal();
    void           InitDynamic();
  };

//=========================================================================
//
//
//
//=========================================================================

const byte ESR_Snks    = 0x01;
const byte ESR_Srcs    = 0x02;
const byte ESR_Errors  = 0x04;
const byte ESR_Species = 0x08;

class DllImportExport EqnSlvCtrlBlk
  {
  public:
    EqnSlvCfgBlk  Cfg;

  protected:
    long           nIters;
    long           nConvergedIters;
    long           nBadError;
    long           nBadLimit;
    flag           fConfigOK;
    flag           fConverged;
    //ag           fSolutionOK;

    flag           bHoldTearUpdate;
    long           nUnknowns;
    byte           iCollectWorst;
    byte           iReport;
    double         dReportTol;
    double         dWorstErr;
    double         dWorstOtherErr;
    double         dTheWorstErr;

  public:
    WorstEqn       Worst[MAX_EQNSLV_WORST];
    WorstEqn       WorstOther;

    EqnSlvCtrlBlk();
    void   Init();
    void   BeginStep();
    void   EndStep();

    flag   NoteBadEqnError(double Err, double RelTol);
    void   CollectBadEqnError(double Err, double Val, double AbsTol, double RelTol, double Damping, LPSTR Tag, byte TearTagTyp);
    flag   NoteBadEqnLimit();
    void   CollectBadEqnLimit(double Val, LPSTR Tag, byte TearTagTyp);
    flag   NoteWorstError(double Err, double RelTol);
    void   CollectWorstError(double Err, double Val, double AbsTol, double RelTol, double Damping, LPSTR Tag, byte TearTagTyp);
    void   SetCollectWorst(byte C);

    long   NIters() const { return nIters; };
    long   NConvergedIters() const { return nConvergedIters; };
    long   NBadError() const { return nBadError; };
    long   NBadLimit() const { return nBadLimit; };
    flag   ConfigOK() const { return fConfigOK; };
    flag   Converged() const { return fConverged; };

    flag   HoldTearUpdate() const { return bHoldTearUpdate; };
    long   NUnknowns() const { return nUnknowns; };
    byte   Report() const { return iReport; };
    double ReportTol() const { return dReportTol; };
    double WorstErr() const { return dWorstErr; };
    double WorstOtherErr() const { return dWorstOtherErr; };
    double TheWorstErr() const { return dTheWorstErr; };
    byte   CollectWorst() const { return iCollectWorst; };

    void   BumpNIters() { nIters++; if (fConverged) nConvergedIters++; };
    void   BumpNBadError() { nBadError++; };
    void   BumpNBadLimit() { nBadLimit++; };
    void   BumpNUnknowns() { nUnknowns++; };

    void   SetNBadError(long E) { nBadError=E; };
    void   SetNBadLimit(long L) { nBadLimit=L; };
    void   SetNIters(long I) { nIters=I; };
    void   SetConfigOK(flag C) { fConfigOK=C; };
    void   SetConverged(flag C) { fConverged=C; if (!fConverged) nConvergedIters=0; };
    void   SetHoldTearUpdate(flag H) { bHoldTearUpdate=H; };
    void   SetNUnknowns(long U) { nUnknowns=U; };
    void   SetReport(byte R) { iReport=R; };
    void   SetReportTol(double T) { dReportTol=T; };

  };
typedef CArray <TearVarBlk*, TearVarBlk*> TearVarPtrArray;

//=========================================================================
//
//
//
//=========================================================================

class DllImportExport EqnSolverBlk
  {
  public:

    EqnSolverBlk();
    ~EqnSolverBlk();


    void           SetTearsInUse(int InUse);
    void           ActivateAllReqdTears();
    void           DeActivateAllTears();
    void           ConnectAllTears(flag CreateIfReqd);
    void           DisConnectAllTears();
    void           FindAllTearsInUse();
    void           RemoveAllTears();
    void           RemoveUnusedTears();
    void           RemoveInActiveTears();
    void           InitialiseActiveTears(EqnSlvCtrlBlk & EqnCB);
    void           RestartAllActiveTears(EqnSlvCtrlBlk & EqnCB);

    //void           InitialiseAllTearOutputs(EqnSlvCtrlBlk & EqnCB);
    void           TransferAllTears(EqnSlvCtrlBlk & EqnCB);
    flag           TestTearConvergence(long ConvergeLoopCnt, EqnSlvCtrlBlk & EqnCB, Strng_List &BadTearTags, Strng_List & BadTearInfo);

    void           BeginStep(EqnSlvCtrlBlk & TCB);
    void           EndStep(EqnSlvCtrlBlk & TCB);

  protected:

  };

//=========================================================================
//
//
//
//=========================================================================

#undef DllImportExport

#endif
