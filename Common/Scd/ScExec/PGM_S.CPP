//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __PGM_S_CPP
#include "sc_defs.h"
#include "pgm_e.h"
#include "pgm_s.h"
#include "sp_db.h"
#include "scdver.h"
 
//===========================================================================
//
//
//
//===========================================================================

#if WITHEXTENDPGMSDB

class CLclEDB
  {
  public:
    CLclEDB()
      {
      m_iFidelity=1;
      }

  public:
    long m_iFidelity;
  };

//===========================================================================

enum 
  {
  Idf_EDB_EleCount = 1,
  Idf_EDB_Name,
  Idf_EDB_AtmWeight,
  Idf_EDB_AtmNumberS,
  Idf_EDB_AtmWeightS,
  };

//---------------------------------------------------------------------------

GCEDB::GCEDB(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, &IB.m_VarMap, "GlblSDB", VarClassDefn)
  {
  AddFunct(IB, "Count",              0, 0x0000, Idf_EDB_EleCount);              // Count        ()
  AddFunct(IB, "Name",               1, 0x0000, Idf_EDB_Name,       true);      // Name         (AtmNo)  : returns name of element
  AddFunct(IB, "AtmWeight",          1, 0x0000, Idf_EDB_AtmWeight);             // AtmWeight    (AtmNo)  : returns AtomicWeight of element
                               
  AddFunct(IB, "AtmNumberS",         1, 0x0001, Idf_EDB_AtmNumberS);            // AtmNumberS   (EleName)
  AddFunct(IB, "AtmWeightS",         1, 0x0001, Idf_EDB_AtmWeightS);            // AtmWeightS   (EleName)
  }

//---------------------------------------------------------------------------

void GCEDB::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (pvoid)new CLclEDB;
  }
  
//---------------------------------------------------------------------------

void GCEDB::Done(pGCClassVar pClassVar)
  {
  delete (CLclEDB*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCEDB::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CLclEDB * pLclSDB = (CLclEDB*)pSubClass;
  //long Index = Range(0L, IB.GetLParm(), SDB.Count()-1L);
  switch (FunctId)
    {
    case Idf_EDB_EleCount: 
      return EDB.Count();

    case Idf_EDB_Name:   
      {
      int iEle=IB.GetLParm();
      if (iEle>0 && iEle<=EDB.Count())
        {
        ASSERT_ALWAYS(EDB[iEle-1].AtmNo==iEle, "Bad Atomic No Index", __FILE__, __LINE__);
        IB.PushStringValue(EDB[iEle-1].Name);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_EDB_AtmWeight:
      {
      int iEle=IB.GetLParm();
      if (iEle>0 && iEle<=EDB.Count())
        {
        ASSERT_ALWAYS(EDB[iEle-1].AtmNo==iEle, "Bad Atomic Weight Index", __FILE__, __LINE__);
        return EDB[iEle-1].AtmWt;
        }
      return 0;
      }

    case Idf_EDB_AtmNumberS: 
      {
      int iEle=EDB.Find(IB.GetSParm());
      if (iEle>=0)
        return EDB[iEle].AtmNo;
      return -1;
      }

    case Idf_EDB_AtmWeightS: 
      {
      int iEle= EDB.Find(IB.GetSParm());
      if (iEle>=0 && iEle<EDB.Count())
        return EDB[iEle].AtmWt;
      return 0.0;
      }

    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCEDB::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  CLclEDB * pLcl = (CLclEDB*)pSubClass;
  GCFCBAppendRec(FCB, '{', "GlblEDB", NULL, 0); // Start Internal Vars
  GCFCBAppendRec(FCB, '}', "GlblEDB", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCEDB::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  CLclEDB * pLcl = (CLclEDB*)pSubClass;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        {
        break;
        }
      }
    }
  return True;
  }
#endif

//===========================================================================
//
//
//
//===========================================================================

class CLclSDB
  {
  public:
    CLclSDB()
      {
      m_iFidelity=1;
      }

  public:
    long m_iFidelity;
  };

//===========================================================================

enum 
  {
  Idf_SDB_SpCount = 1,
  Idf_SDB_PhaseCount,
  Idf_SDB_IPhaseCount,
  Idf_SDB_SolidPhase, 
  Idf_SDB_LiquidPhase,
  Idf_SDB_GasPhase,   
  Idf_SDB_PhaseTag,
  Idf_SDB_IPhaseTag,
  Idf_SDB_PhaseSymbol,
  Idf_SDB_IPhaseSymbol,
  Idf_SDB_FindSpecie,
  Idf_SDB_FindPhase,
  Idf_SDB_FindIPhase,
  Idf_SDB_Symbol,
  Idf_SDB_Tag,
  Idf_SDB_SpPhaseNo,
  Idf_SDB_SpIPhaseNo,
  Idf_SDB_SpPhaseTag,
  Idf_SDB_SpIPhaseTag,
  Idf_SDB_SpPhaseSymbol,
  Idf_SDB_SpIPhaseSymbol,
  Idf_SDB_IsSolid,
  Idf_SDB_IsLiquid,
  Idf_SDB_IsGas,
  Idf_SDB_MolecularWt,
  Idf_SDB_Density,
  Idf_SDB_mlHf,
  Idf_SDB_msHf,
  Idf_SDB_mlHz,
  Idf_SDB_msHz,
  Idf_SDB_mlHs,
  Idf_SDB_msHs,
  Idf_SDB_VapourP,
  Idf_SDB_VapourT,
  Idf_SDB_SpElemCount,
  Idf_SDB_SpElemName,      
  Idf_SDB_SpElemUnits,
  Idf_SDB_SpElemAtmNumber,
  Idf_SDB_SpElemAtmWeight,     
  Idf_SDB_SpElemIndexS,        
  Idf_SDB_SpElemUnitsS,        
  Idf_SDB_ElemAtmWeight,        
  };

//---------------------------------------------------------------------------

GCSDB::GCSDB(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, &IB.m_VarMap, "GlblSDB", VarClassDefn)
  {
  AddVar  (IB, "Fidelity",      &GCLong);        
  
  AddFunct(IB, "SpecieCount",         0, 0x0000, Idf_SDB_SpCount);               // SpecieCount        ()
  AddFunct(IB, "PhaseCount",          0, 0x0000, Idf_SDB_PhaseCount);            // PhaseCount         ()
  AddFunct(IB, "IPhaseCount",         0, 0x0000, Idf_SDB_IPhaseCount);           // IPhaseCount        ()

  AddFunct(IB, "SolidPhase",          0, 0x0000, Idf_SDB_SolidPhase);            // SolidPhase         ()
  AddFunct(IB, "LiquidPhase",         0, 0x0000, Idf_SDB_LiquidPhase);           // LiquidPhase         ()
  AddFunct(IB, "GasPhase",            0, 0x0000, Idf_SDB_GasPhase);              // GasPhase           ()

  AddFunct(IB, "PhaseTag",            1, 0x0000, Idf_SDB_PhaseTag,     true);    // PhaseTag           (PhIndex)
  AddFunct(IB, "IPhaseTag",           1, 0x0000, Idf_SDB_IPhaseTag,    true);    // IPhaseTag          (IPhIndex)
  AddFunct(IB, "PhaseSymbol",         1, 0x0000, Idf_SDB_PhaseSymbol,  true);    // PhaseSymbol        (PhIndex)
  AddFunct(IB, "IPhaseSymbol",        1, 0x0000, Idf_SDB_IPhaseSymbol, true);    // IPhaseSymbol       (IPhIndex)
                                                                                  
  AddFunct(IB, "FindSpecie",          1, 0x0001,  Idf_SDB_FindSpecie);            // FindSpecie         (Symbol/Tag)
  AddFunct(IB, "FindPhase",           1, 0x0001,  Idf_SDB_FindPhase);             // FindPhase          (PhaseName)
  AddFunct(IB, "FindIPhase",          1, 0x0001,  Idf_SDB_FindIPhase);            // FindIPhase         (IPhaseName)

  AddFunct(IB, "SpSymbol",            1, 0x0000, Idf_SDB_Symbol,       true);    // Symbol             (SpIndex)
  AddFunct(IB, "SpTag",               1, 0x0000, Idf_SDB_Tag,          true);    // Tag                (SpIndex)

  AddFunct(IB, "SpPhaseNo",           1, 0x0000, Idf_SDB_SpPhaseNo);             // SpPhaseNo          (SpIndex)
  AddFunct(IB, "SpIPhaseNo",          1, 0x0000, Idf_SDB_SpIPhaseNo);            // SpIPhaseNo         (SpIndex)
  AddFunct(IB, "SpPhaseTag",          1, 0x0000, Idf_SDB_SpPhaseTag,     true);  // SpPhaseTag         (SpIndex)
  AddFunct(IB, "SpIPhaseTag",         1, 0x0000, Idf_SDB_SpIPhaseTag,    true);  // SpIPhaseTag        (SpIndex)
  AddFunct(IB, "SpPhaseSymbol",       1, 0x0000, Idf_SDB_SpPhaseSymbol,  true);  // SpPhaseSymbol      (SpIndex)
  AddFunct(IB, "SpIPhaseSymbol",      1, 0x0000, Idf_SDB_SpIPhaseSymbol, true);  // SpIPhaseSymbol     (SpIndex)
                                                                                   
  AddFunct(IB, "SpIsSolid",           1, 0x0000, Idf_SDB_IsSolid);               // SpIsSolid          (SpIndex)
  AddFunct(IB, "SpIsLiquid",          1, 0x0000, Idf_SDB_IsLiquid);              // SpIsLiquid         (SpIndex)
  AddFunct(IB, "SpIsGas",             1, 0x0000, Idf_SDB_IsGas);                 // SpIsGas            (SpIndex)

  AddFunct(IB, "SpMolecularWt",       1, 0x0000, Idf_SDB_MolecularWt);           // SpMolecularWt        (SpIndex)
  AddFunct(IB, "SpDensity",           3, 0x0000, Idf_SDB_Density);               // SpDensity            (SpIndex, temp, press)
  AddFunct(IB, "SpmlHf",              3, 0x0000, Idf_SDB_mlHf);                  // SpmlHf               (SpIndex, temp, press)
  AddFunct(IB, "SpmsHf",              3, 0x0000, Idf_SDB_msHf);                  // SpmsHf               (SpIndex, temp, press)
  AddFunct(IB, "SpmlHz",              3, 0x0000, Idf_SDB_mlHz);                  // SpmlHz               (SpIndex, temp, press)
  AddFunct(IB, "SpmsHz",              3, 0x0000, Idf_SDB_msHz);                  // SpmsHz               (SpIndex, temp, press)
  AddFunct(IB, "SpmlHs",              3, 0x0000, Idf_SDB_mlHs);                  // SpmlHs               (SpIndex, temp, press)
  AddFunct(IB, "SpmsHs",              3, 0x0000, Idf_SDB_msHs);                  // SpmsHs               (SpIndex, temp, press)
  AddFunct(IB, "SpVapourP",           2, 0x0000, Idf_SDB_VapourP);               // SpVapourP            (SpIndex, temp)
  AddFunct(IB, "SpVapourT",           2, 0x0000, Idf_SDB_VapourT);               // SpVapourT            (SpIndex, temp)

#if WITHEXTENDPGMSDB 
  // examples for H20
  AddFunct(IB, "SpElemCount",         1, 0x0000, Idf_SDB_SpElemCount);           // SpElemCount        (SpIndex)            - No of elements in Sp : () returns 2)
  AddFunct(IB, "SpElemName",          2, 0x0000, Idf_SDB_SpElemName,     true);  // SpElemTag          (SpIndex, EleIndex)  - Name of element N in Sp : (0) returns 'H', (1) returns 'O')
  AddFunct(IB, "SpElemUnits",         2, 0x0000, Idf_SDB_SpElemUnits);           // SpElemUnits        (SpIndex, EleIndex)  - Moles of element N in Sp : (0) returns 2.0, (1) returns 1.0)
  AddFunct(IB, "SpElemAtmNumber",     2, 0x0000, Idf_SDB_SpElemAtmNumber);       // SpElemAtmNumber    (SpIndex, EleIndex)  - Atomic No of element N in Sp : (0) returns 1, (1) returns 8)
  AddFunct(IB, "SpElemAtmWeight",     2, 0x0000, Idf_SDB_SpElemAtmWeight);       // SpElemAtmWeight    (SpIndex, EleIndex)  - Atomic Wt of element N in Sp : (0) returns 1.00794, (1) returns 15.9994)
  AddFunct(IB, "SpElemIndexS",        2, 0x0002, Idf_SDB_SpElemIndexS);          // SpElemIndexS       (SpIndex, EleName)   - Find element in Sp           : ("H") returns 0, ("O") returns 1, ("N") returns -1;
#endif
  AddFunct(IB, "SpElemMoles",         2, 0x0002, Idf_SDB_SpElemUnitsS);          // SpElemMoles        (SpIndex, EleName)   - return element moles in Sp  : ("H") returns 2, ("O") returns 1, ("N") returns 0;

  AddFunct(IB, "ElemAtmWeight",       1, 0x0001, Idf_SDB_ElemAtmWeight);         // ElemAtmWeight      (EleName)            - return element atomic weight, 0 if invalid


  //AddFunct(IB, "SpElemUnitsS",        2, 0x0002, Idf_SDB_SpElemUnitsS);          // SpElemUnitsS       (SpIndex, EleName)   - Find element moles in Sp  : ("H") returns 2, ("O") returns 1, ("N") returns 0;
  }

//---------------------------------------------------------------------------

void GCSDB::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (pvoid)new CLclSDB;
  }
  
//---------------------------------------------------------------------------

void GCSDB::Done(pGCClassVar pClassVar)
  {
  delete (CLclSDB*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

const int OccCount=3;
static struct CPhNames {LPSTR m_Tag; LPSTR m_Sym; int m_BOT; } s_PhNames[OccCount]=
  {
    {"Solids", "S", BOT_Solid},
    {"Liquids", "L", BOT_Liquid},
    {"Gas", "G", BOT_Gas}
  };

double GCSDB::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CLclSDB * pLclSDB = (CLclSDB*)pSubClass;
  //long Index = Range(0L, IB.GetLParm(), SDB.Count()-1L);
  switch (FunctId)
    {
    case Idf_SDB_SpCount:       return SDB.Count();
    case Idf_SDB_PhaseCount:    return OccCount;
    case Idf_SDB_IPhaseCount:   return CDB.PhaseCount();

    case Idf_SDB_SolidPhase:    return BOT_Solid;
    case Idf_SDB_LiquidPhase:   return BOT_Liquid;
    case Idf_SDB_GasPhase:      return BOT_Gas;

    case Idf_SDB_FindSpecie:    return SDB.Find(IB.GetSParm());
    case Idf_SDB_FindPhase:     
      {
      Strng S(IB.GetSParm());
      for (int i=0; i<OccCount; i++)
        {
        if (S.XStrICmp(s_PhNames[i].m_Sym)==0 || S.XStrICmp(s_PhNames[i].m_Tag)==0)
          {
          ASSERT(i==s_PhNames[i].m_BOT);
          return i;
          }
        }
      return -1;
      }
    case Idf_SDB_FindIPhase:    
      {
      int i0=CDB.PhaseFirst();
      int iN=CDB.PhaseLast();
      Strng S(IB.GetSParm());
      for (int i=i0; i<iN; i++)
        {
        CPhaseInfo &I=CDB.m_PhInfo[i];
        if (S.XStrICmp(I.m_Sym())==0 || S.XStrICmp(I.m_Tag())==0)
          return i;
        }
      return -1;
      }
    case Idf_SDB_PhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<OccCount)
        {
        IB.PushStringValue(s_PhNames[i].m_Sym);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_PhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<OccCount)
        {
        IB.PushStringValue(s_PhNames[i].m_Tag);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_IPhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<CDB.PhaseCount())
        {
        IB.PushStringValue(CDB.m_PhInfo[i].m_Sym());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_IPhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<CDB.PhaseCount())
        {
        IB.PushStringValue(CDB.m_PhInfo[i].m_Tag());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpPhaseNo:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].m_eOcc;
      return -1;
      }
    case Idf_SDB_SpIPhaseNo:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].m_PhInx;
      return -1;
      }

    case Idf_SDB_SpPhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(s_PhNames[SDB[i].m_eOcc].m_Sym);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpPhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(s_PhNames[SDB[i].m_eOcc].m_Tag);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpIPhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(CDB.m_PhInfo[SDB[i].m_PhInx].m_Sym());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpIPhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(CDB.m_PhInfo[SDB[i].m_PhInx].m_Tag());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }

    case Idf_SDB_IsSolid:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].IsSol();
      return 0;
      }
    case Idf_SDB_IsLiquid:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].IsLiq();
      return 0;
      }
    case Idf_SDB_IsGas:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].IsGas();
      return 0;
      }

    case Idf_SDB_Symbol:        
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(SDB.Sym(i));      
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_Tag:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(SDB.Tag(i));      
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }

    case Idf_SDB_MolecularWt:   
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].MoleWt();
      return 0;
      }
    case Idf_SDB_Density: 
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        CDensityInfo C(pLclSDB->m_iFidelity, CDB.DefDensityMethod(), T, P, NULL, NULL);
        return SDB[i].DensityXZero(C);
        }
      return 0;
      }

    case Idf_SDB_mlHf:    
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].mlHf(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_msHf:    
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].msHf(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_mlHz:      
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].mlHz(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_msHz:   
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].msHz(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_mlHs:     
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].mlHs(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_msHs:    
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].msHs(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }

    case Idf_SDB_VapourP:   
      {
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].VapourP(pLclSDB->m_iFidelity, T);
      return 0;
      }
    case Idf_SDB_VapourT:  
      {
      double P=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].VapourT(pLclSDB->m_iFidelity, P);
      return 0;
      }

#if WITHEXTENDPGMSDB 
    case Idf_SDB_SpElemCount:
      {
      int iSp=IB.GetLParm();
      if (iSp>=0 && iSp<SDB.Count())
        return CDB[SDB[iSp].iCId].ElComp.GetCount();
      return 0;
      }
    case Idf_SDB_SpElemName:
      {
      int iEle=IB.GetLParm();
      int iSp=IB.GetLParm();
      if (iSp>=0 && iSp<SDB.Count())
        {
        IB.PushStringValue(EDB[CDB[SDB[iSp].iCId].ElComp[iEle].iENo].Name);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpElemUnits:
      {
      int iEle=IB.GetLParm();
      int iSp=IB.GetLParm();
      if (iSp>=0 && iSp<SDB.Count())
        return CDB[SDB[iSp].iCId].ElComp[iEle].dMoles;
      return 0;
      }
    case Idf_SDB_SpElemAtmNumber:
      {
      int iEle=IB.GetLParm();
      int iSp=IB.GetLParm();
      if (iSp>=0 && iSp<SDB.Count())
        return EDB[CDB[SDB[iSp].iCId].ElComp[iEle].iENo].AtmNo;
      return -1;
      }
    case Idf_SDB_SpElemAtmWeight:
      {
      int iEle=IB.GetLParm();
      int iSp=IB.GetLParm();
      if (iSp>=0 && iSp<SDB.Count())
        return EDB[CDB[SDB[iSp].iCId].ElComp[iEle].iENo].AtmWt;
      return -1;
      }

    case Idf_SDB_SpElemIndexS:
      {
      CString Ele=IB.GetSParm();
      int iSp=IB.GetLParm();
      if (iSp>=0 && iSp<SDB.Count())
        {
        CComponent &C=CDB[SDB[iSp].iCId];
        for (int e=0; e<C.ElComp.GetCount(); e++)
          {
          if (_stricmp(EDB[C.ElComp[e].iENo].Name, Ele)==0)
            return e; 
          }
        }
      return -1;
      }
#endif
    case Idf_SDB_SpElemUnitsS:
      {
      CString Ele=IB.GetSParm();
      int iSp=IB.GetLParm();
      if (iSp>=0 && iSp<SDB.Count())
        {
        CComponent &C=CDB[SDB[iSp].iCId];
        for (int e=0; e<C.m_ElComp.GetCount(); e++)
          {
          if (_stricmp(EDB[C.m_ElComp[e].iENo].m_Name, Ele)==0)
            return C.m_ElComp[e].dMoles; 
          }
        }
      return 0;
      }

    case Idf_SDB_ElemAtmWeight:
      {
      int iEle= EDB.Find(IB.GetSParm());
      if (iEle>=0 && iEle<EDB.Count())
        return EDB[iEle].m_AtmWt;
      return 0.0;
      }

    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCSDB::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  CLclSDB * pLcl = (CLclSDB*)pSubClass;
  GCFCBAppendRec(FCB, '{', "GlblSDB", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Fidelity", pLcl->m_iFidelity);
  GCFCBAppendRec(FCB, '}', "GlblSDB", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCSDB::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  CLclSDB * pLcl = (CLclSDB*)pSubClass;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        {
        long iF;
        if (Buff.Try("Fidelity", iF)) 
          {
          pLcl->m_iFidelity=iF;
          break;
          }
        break;
        }
      }
    }
  return True;
  }
  
//===========================================================================
