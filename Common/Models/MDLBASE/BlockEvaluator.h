#pragma once

#include "vlebase.h"
#include "sp_react.h"
#include "m_hxbase.h"
#include "evapblock.h"
#include "makeupblock.h"
#include "bleedblock.h"

#ifdef __BLOCKEVALUATOR_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

const DWORD BEO_StateSemantics  = 0x00000001;
const DWORD BEO_WithMakeups     = 0x00000002;
const DWORD BEO_WithBleeds      = 0x00000004;

class DllImportExport CBlockEvaluator : public CBlockEvalStateSemantics
  {
  friend class CBlockEvalBase;

  public:
    //                                     RB, HX, EHX, VLE
    static const int MaxBEBlocks           =   1  +1   +1   +1   +MaxNdMakeups+MaxNdBleeds;

    CBlockEvaluator(FlwNode * pNd,
                    DWORD Options,
                    CReactionBase * pRB = NULL,
                    CHXBase *pHX = NULL,
                    CEnvironHXBase * pEHX = NULL,
                    CVLEBase * pVLE = NULL,
                    CEvapBase * pEvap = NULL);
    ~CBlockEvaluator(void);

    void Attach(bool AllowStateSemantics,
                CReactionBase * pRB = NULL,
                CHXBase *pHX = NULL,
                CEnvironHXBase * pEHX = NULL,
                CVLEBase * pVLE = NULL,
                CEvapBase * pEvap = NULL);

    void              SetEnable(bool On);

    void              AddBlk(CBlockEvalBase *p, int DefSeqNo);
    void              RemBlk(CBlockEvalBase *p);

    //CFlange         * GetFlange(int IOId);

    void              Add_OnOff(DataDefnBlk &DDB, DDBPages PageIs=DDB_NoPage);
    void              BuildDataDefn(DataDefnBlk &DDB, DDBPages PageIs=DDB_RqdPage);
    flag              DataXchg(DataChangeBlk & DCB);
    flag              ValidateData(ValidateDataBlk & VDB, bool AllowStateSemantics);
    void              BuildOnOffValLst(DDBValueLstMem  * ValLst, int NInSequence, LPCSTR StateName);

    virtual bool      StateSemanticsOn(); // override of CBlockEvalStateSemantics::StateSemanticsOn

    void              SortBlocks();

    double            Duty();

    void              EvalProducts(int iJoinNo, SpConduit & Fo, double Po, CFlwThermalBlk * pFTB, double FinalTEst=dNAN);
    void              EvalProductsInline(int iJoinNo, SpConduit & Fo, double Len, double Diam, double Po, CFlwThermalBlk * pFTB, double FinalTEst=dNAN);

    int               ChangeTag(char * pOldTag, char * pNewTag);
    int               DeleteTag(char * pDelTag);

    int               MakeupCount() { return m_pMakeups.GetCount(); };
    int               BleedCount()  { return m_pBleeds.GetCount(); };

    void              SetMakeupCount(int N, bool MakeConnects);
    void              SetBleedCount(int N, bool MakeConnects);

    void              CheckFlowsConsistent(int iJoinNo, bool ForMakeup);

    int               EnabledCount();

    bool              RBAttached()      { return m_pRB!=NULL; };
    bool              HXAttached()      { return m_pHX!=NULL; };
    bool              EHXAttached()     { return m_pEHX!=NULL; };
    bool              VLEAttached()     { return m_pVLE!=NULL; };
    bool              EvapAttached()    { return m_pEvap!=NULL; };

  protected:
    FlwNode         * m_pNd;
    int               m_nBlocks;
    CBlockEvalBase  * m_Blks[MaxBEBlocks+1];


    int               m_nMaxNdMakeups;
    CReactionBase   * m_pRB;
    CHXBase         * m_pHX;
    CEnvironHXBase  * m_pEHX;
    CVLEBase        * m_pVLE;
    CEvapBase       * m_pEvap;
    CArray <CMakeupBase*, CMakeupBase*> m_pMakeups;
    CArray <CBleedBase*, CBleedBase*> m_pBleeds;

    DWORD             m_Options;

    Strng             m_sBlkSeqFeed;
    Strng             m_sBlkSeqState;
    DDBValueLstMem    m_OnOffValLst;  
    DDBValueLstMem    m_OnOffStateValLst;  
  };


#undef DllImportExport
