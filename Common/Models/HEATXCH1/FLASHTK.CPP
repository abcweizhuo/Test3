//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __FLASHTK_CPP
#include "flashtk.h"
//#include "optoff.h"

//==========================================================================

//SPECIEBLK_LV(Water	           ,"H2O(l)"    , false);

//==========================================================================

#define dbgModels         WITHDEBUG
#if dbgModels   
#include "dbgmngr.h"
static CDbgMngr dbgFlashTrain    ("HeatXch1", "FlashTrain");
static CDbgMngr dbgDumpChanges   ("ProBal",   "DumpChanges");
static CDbgMngr dbgMSProgress    ("ProBal",   "MS-FT-Progress");
#endif

//==========================================================================
/*#D:#T:Flash Tank(1)
#X:#n#n#h<General Description>#n
The Flash Tank is used to 'flash' a feed stream to produce a liquid and vapour 
product at a set pressure.  The liquids and vapours leave the unit separately. 

#nThere are two operational modes for the flash tank:
#na) as a stand-alone unit or 
#nb) as part of a Flash Train.
#nThe operational mode is decided by the overall configuration of the flowsheet 
in which the Flash Tank is located.  For example, if the Flash Tank is connected 
to other flash tanks, Barometric Condensers and / or heat exchangers, the model 
will become part of the entire Flash Train.  The user does not have to specify 
that the unit is part of the Flash Train, SysCAD will do this automatically.  
If the Flash Tank is connected to units other than the previous three, i.e. Feeders, 
tanks, etc. then the Flash Tank is a stand alone unit and can be configured as such.

#nIf the Flash Tank is inserted as part of a Flash Train, see Flash Train for a 
description of the theory and variables.  This documentation will only discuss the 
variables for a 'stand alone' Flash Tank.

#nAny combination of Vapour Liquid Equilibrium, Environmental Heat Loss or Reaction 
Block can be configured, if required.  Detailed descriptions of these sub-models are 
supplied separately, but the method of inserting them into the tank, and descriptions 
of the required variables are given here.

#nThe Vapour Liquid Equilibrium section allows the user to configure which species to 
flash and / or the flashing method.

#nEnvironmental Heat Loss allows the user to specify a heat loss, or gain, between the 
unit and the environment.

#nThe Reaction Block (RB) allows the user to configure any number of reactions within 
the flash tank, fully described in RB Section.

#n#n
#n#u<#l<#b<Inputs and Outputs>>>#n#w
Label   Input/Output  Minimum number  Maximum number#n
feed       Input           1              20#n
liquor     Output          1              1#n
vapour     Output          1              1#n
#a
To function the flash tank must have at least one feed, the vapour and liquor streams specified.
Up to 20 feed streams are allowed.

#n#n#h<Variables to be supplied by the user>#n
#i<VapSpModel>: The user may choose the specie model for the vapour line from a pull down menu.  
The default is Steam/Water.
#i<VLEquilibrium>: This should be checked and the vapour:liquid equilibrium section must be 
configured.#n
#i<EnvironHX>: The user may specify an environmental heat loss.#n
#i<Reactions>: If this block is checked, the user may specify a reaction file in the unit.#n

#n#n#h<Variables calculated by the model>#n
#i<Flash_Temp>: The equilibrium temperature in the Flash Tank.#n
#i<Flash_Press>: The equilibrium pressure in the Flash Tank.#n
#i<QVap>: The mass of vapour flashed in the unit.#n
#i<PRqd>: This is only visible if the unit is part of a flash train.  The flash pressure required 
by the flash train.#n

#n#n#b<Assumptions & Limitations>#n
The vapour stream is entirely steam. Although liquor carry over can occur,
the initial assumption is that there is no contamination with other compounds.

#n#h<Other>#n
Default model prefix:FT#n
Short name:FlshTank#n
Model type:Unit#n
#G:Units
*/

//---------------------------------------------------------------------------
/* not used any more
class VapFinder : public MRootFinderBase
{
protected:
FlashTank &m_FT;
SpConduit &m_Ql, &m_Qv;
static CToleranceBlock s_Tol;

public:
VapFinder(FlashTank & FT, SpConduit &Ql, SpConduit &Qv);
~VapFinder();
double Function(double x);
};

CToleranceBlock VapFinder::s_Tol(TBF_Both, "FlashTank:Vapour", 0.0, 1.0e-8);

//---------------------------------------------------------------------------

VapFinder::VapFinder(FlashTank & FT, SpConduit &Ql, SpConduit &Qv) :
m_FT(FT), m_Ql(Ql), m_Qv(Qv), MRootFinderBase("FlashTank:Vapour", s_Tol)//1.0e-8)
{ 
};

//---------------------------------------------------------------------------

VapFinder::~VapFinder()
{ 
};

//---------------------------------------------------------------------------

double VapFinder::Function(double P) 
{ 
m_FT.m_VLE.PFlash(m_Ql, m_Qv, P, 0.0, VLEF_SaturationLimit);
return m_Qv.QMass();
};
*/
//==========================================================================

#define WITHNONCONDENSABLES  (0)
#define WITHOTHERVAPSPLIT    (1)
#define FORCESTEAM           (0)

#if FORCESTEAM
static MInitialiseTest InitTest(&FlashTankClass);
SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);
#endif

const byte ioidFT_Feed         = 0;
const byte ioidFT_Liq          = 1;
const byte ioidFT_Vap          = 2;
const byte ioidFT_NCVent       = 3;

XID   xidFlashDesc             = MdlBsXID(10000);

static IOAreaRec FlashTankIOAreaList[] =
  {{"Feeds",        "Feed"    , ioidFT_Feed , LIO_In0 ,    nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz, 0.0f},
  {"Liquor",       "Liquor"  , ioidFT_Liq  , LIO_Out0,    nc_MLnk, 1,  1, IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz, 0.0f},
  {"Vapour",       "Vapour"  , ioidFT_Vap  , LIO_Out1,    nc_MLnk, 1,  1, IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz, 1.0f},
#if WITHNONCONDENSABLES
  {"NonCond Vent", "NonCondVent",ioidFT_NCVent, LIO_Out, nc_MLnk, 0,  1, IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz, 1.0f},
#endif
  SPILL2AREA,
  VENT2AREA(-1),
  {NULL}}; //This lists the areas of the model where links can be attached.


/*static double Drw_FlashTank[] = { DD_Poly, -5,8.5, -5,-6.5, 0,-11.5, 5,-6.5, 5,8.5,
                                  DD_Poly, -5,-3.5, 5,-3.5,
                                  DD_Arc3, -5,8.5, 0,11.5, 5,8.5,
                                  DD_TagPos, 0, -15.0,
                                  DD_End }; */
static double Drw_FlashTank[] = { DD_Poly, -5.5,9.4, -5.5,-7.2, 0,-12.7, 5.5,-7.2, 5.5,9.4,
                                  DD_Poly, -5.5,-3.8, 5.5,-3.8,
                                  DD_Arc3, -5.5,9.4, 0,12.7, 5.5,9.4,
                                  DD_TagPos, 0, -16.2,
                                  DD_End }; 

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(FlashTank, "FlshTank", "1", Drw_FlashTank, "Tank", "FT", TOC_ALL|TOC_GRP_ENERGY|TOC_HEATBAL,
                    //                    "HX - Flash Tank(1)",
                    "Heat Transfer:Flash Tank(1)",
                    "The flash tank flashes water vapour from the liquor feed stream.  "
                    "The amount vaporised depends on the condition of the heaters and "
                    "pipework (not on the conditions in the tank). "
                    "The model maintains equilibrium between the vapour and liquid streams. "
                    "The vapour and liquid phases exits separately via the vapour and "
                    "liquor streams, respectively. "
                    "Chemical reactions can occur in the flash tank. "
                    "The model operates in one of two modes."
                    "1) The user specifies only the heat loss to atmosphere from the flash tank. "
                    "The model will calculate the flash pressure and temperature, "
                    "and the flow rate of the vapour and liquor phases."
                    "2) The user specifies the heat loss and the tank pressure. "
                    "The model calculates the flash temperature, and the flow rate of the vapour "
                    "and liquor phases, and the necessary resistance number in the outlet pipes "
                    "to give the required pressure drops to obtain a pressure balance.")

FlashTank::FlashTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
MN_Surge(pClass_, TagIn, pAttach, eAttach),
m_QProd("QProd", this, TOA_Embedded),
m_QWrk("Wrk", this, TOA_Embedded),
m_FTF(this)
  {
  AttachIOAreas(FlashTankIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);
  m_PCtrl0.SetMethod(PC_SatP);

  m_QWrk.SetVLEBlk(&m_VLE);
  m_QProd.SetVLEBlk(&m_VLE);

  SetAllowedModes(true, NM_All|SM_Direct/*|SM_Inline*/|SM_Buffered|HM_All);

  sVapSpModel="Steam/Water";//SMSteamClass.?
  fOnLine=True;

  m_VLE.Open(true);
  //CNM This is commented out - Approach Temp does not work correctly
  //m_VLE.SetFlags(VLEF_ApproachTempOn);

#if DOPBSMOOTHING  
  m_fSolnVapQmValid=false;
  m_dSolnVapQm=0.0;
#endif
  m_dSatPVapFracToLiq = 0.0;
  m_dOtherVapFracToLiq = 0.0;
  m_bTrackVapFdStatus  = 1;

  m_nRmtScl=0;
  for (int j=0; j<sizeof(m_RmtScl)/sizeof(m_RmtScl[0]); j++)        
    m_RmtScl[j]=0.0;

  dTempIn=Std_T;
  dBPEIn=0.0;
  dBPE=0.0;
  dWaterSatT=Std_T;

  InitAssocGrfShowPB(eAGS_Hide, eAGS_Hide, eAGS_Hide, eAGS_Hide, eAGS_Hide, eAGS_Show);
  InitAssocGrfShowDyn(eAGS_Hide, eAGS_Hide, eAGS_Hide, eAGS_Hide, eAGS_Hide, eAGS_Show);

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(FlashTank), ioidFT_Vap, mmio_MODEL, &typeid(CFT_Flash));
  };

// -------------------------------------------------------------------------

void FlashTank::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  BuildDataDefnElevation(DDB);
  if (SolveSurgeMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_DynBoth|HM_All);
    DDB.Text    ("");
    DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
    DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|0);
    DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|0);
    DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|0);
    DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,   "kg/s",   &m_QmAcc,       this, isResult|0);
    DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,   "L/s",    &m_QvAcc,       this, isResult|0);
    }

  DDB.Visibility(NM_Probal|SM_Direct|SM_Inline|HM_All);
  DDB.Text    ("");
  DDB.Text    ("Results");
  DDB.String  ("VapourCalcs",   "",       DC_,     "",      xidFlashDesc,       this, isResult|noFileAtAll);
  DDB.Double  ("TemperatureIn", "Ti",     DC_T,    "C",     &dTempIn,           this, isResult|noFile|noSnap);
  DDB.Double  ("BoilPtElevIn",  "BPEIn",  DC_dT,   "C",     &dBPEIn,            this, isResult|noFile|noSnap);
  DDB.Double  ("BoilPtElev",    "BPE",    DC_dT,   "C",     &dBPE,              this, isResult|noFile|noSnap);
  DDB.Double  ("WaterSatT",     "",       DC_T,    "C",     &dWaterSatT,        this, isResult|noFile|noSnap|InitHidden);

  m_FTF.BuildDataDefn(DDB);

  DDB.Visibility();
  DDB.Text    ("");
  DDB.Text  (StdSepUnderbar);
  DDB.Text    ("Requirements");
  DDB.Bool    ("On_Line",      "",      DC_,     "",       &fOnLine,       this, isParmStopped, DDBYesNo);
  DDB.Visibility(NM_Probal|SM_Direct|SM_Inline|HM_All);
#if WITHOTHERVAPSPLIT
  DDB.Double ("SatPVapSplitToLiq",   "", DC_Frac, "%",      &m_dSatPVapFracToLiq, this, isParm);
  DDB.Double ("OtherVapSplitToLiq",   "", DC_Frac, "%",      &m_dOtherVapFracToLiq, this, isParm);
#endif
  long PBAllowed  = PC_Atmos|PC_Reqd|PC_ReqdSatT|PC_SatP;
  long DynAllowed = PC_Atmos|PC_Reqd|PC_ReqdSatT;
  m_PCtrl0.BuildDataDefn(DDB, this, NetProbalMethod(), PBAllowed, DynAllowed);
  DDB.Visibility();

  DDBValueLstMem DDB0;
  TagObjClass::GetSDescValueLst(SpModel::GroupName, DDB0);
  DDB.String("VapSpModel",     "",        DC_,    "",      &sVapSpModel,  this, isParm, DDB0());

  DDB.Visibility(NM_All|SM_All|HM_All);
  DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &m_bShowQFeed,         this, isParm|SetOnChange);
#if WITHOTHERVAPSPLIT
  DDB.CheckBox("TrackVapFd",        "",  DC_,     "",      &m_bTrackVapFdStatus,  this, isParm);
#endif
  DDB.Visibility();
  m_RB.Add_OnOff(DDB);
  if (SolveBufferedMethod())
    m_EHX.Add_OnOff(DDB);
  m_VLE.Add_OnOff(DDB);

  DDB.Visibility(SM_Buffered|HM_All);
  //  DDB.CheckBoxBtn("Closed",    "",      DC_     , "",      xidClosed,      this, isParm, DDBYesNo);
  AddMdlClosed(DDB);
  AddMdlNetworked(DDB);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  if (SolveBufferedMethod())
    BuildDataDefnIOOpts(DDB);

  m_RB.BuildDataDefn(DDB);
  if (SolveBufferedMethod())
    m_EHX.BuildDataDefn(DDB);

  m_VLE.BuildDataDefn(DDB);

  if (1)//NetProbalMethod())
    {
    DDB.Visibility(NM_All|SM_All|HM_All, m_bShowQFeed);
    DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);
    DDB.Visibility();
    }
  if (1)//NetProbalMethod())
    {
    DDB.Visibility(NM_All|SM_All|HM_All);
    DDB.Object(&m_QProd, this, NULL, NULL, DDB_RqdPage);
    DDB.Visibility();
    }
  if (SolveBufferedMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag FlashTank::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;
  if (m_VLE.DataXchg(DCB))
    return 1;
  if (m_FTF.DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidFlashDesc: DCB.pC = m_VLE.FlashDescription();   return 1;
    }

  return 0;
  }

//---------------------------------------------------------------------------

flag FlashTank::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
  if (sVapSpModel.XStrICmp(SteamWaterSpModelName)==0)
    sVapSpModel=BaseSpModelName;

  m_dSatPVapFracToLiq = Range(0.0, m_dSatPVapFracToLiq, 1.0);
  m_dOtherVapFracToLiq = Range(0.0, m_dOtherVapFracToLiq, 1.0);

  DDBValueLstMem DDB0;
  TagObjClass::GetSDescValueLst(SpModel::GroupName, DDB0);
  if (sVapSpModel() && DDB0.Find(sVapSpModel())<0)
    {
    OK=false;
    SetCI(3);
    }
  else
    ClrCI(3);

  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
      {
      const bool UsrSetP = ((m_PCtrl0.Method()&(PC_Atmos|PC_Reqd|PC_ReqdSatT))!=0);
      SetCI(2, fOnLine && ((!m_FTF.Active() && !UsrSetP) || (m_FTF.Active() && UsrSetP)) );
      break;
      }
    default:
      ClrCI(2);
      if (Contents.Closed())
        m_VLE.VFlash(Contents, 0.0, VLEF_Null);//, NULL, NULL, NULL);
      break;
    }

  return OK;
  }

//--------------------------------------------------------------------------

void FlashTank::ConfigureJoins()
  {
  if (NetMethod()==NM_Probal || SolveMethod()!=SM_Buffered)
    {
    for (int i=0; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0);
#if DOPBSMOOTHING  
    m_fSolnVapQmValid=false;
    m_dSolnVapQm=0.0;
#endif
    if (!DoMassSmoothing())
      m_nRmtScl=0;
    }
  else
    MN_Surge::ConfigureJoins();
  }

//--------------------------------------------------------------------------

void FlashTank::EvalPBMakeUpReqd(long JoinMask) 
  { 
  if (m_FTF.Active())
    return ;

  MakeUpNodeTransferReqd(0); 
  };

//--------------------------------------------------------------------------

void FlashTank::EvalPBMakeUpAvail(long JoinMask) 
  { 
  if (m_FTF.Active())
    {
    int ioVap=IOWithId_Self(ioidFT_Vap);
    IOFlange(ioVap)->SetMakeUpAvail(Max(1.0e-6, m_FTF.m_dQVap)); //steam
    return ;
    }

  MakeUpNodeTransferAvail(0); 
  };

//--------------------------------------------------------------------------

double FlashTank::GetFlashPressure(SpConduit &Q)
  {
  double FlashP=0;
  if (m_FTF.Active())
    {
    if (m_PCtrl0.Method()==PC_Reqd)
      FlashP=m_PCtrl0.PRqd();
    else if (m_PCtrl0.Method()==PC_ReqdSatT)
      {
      if (m_VLE.SatPMethod()==SMO_SatPPartialP)
        FlashP=Q.SaturationTotalP(m_PCtrl0.TRqd()-m_VLE.ApproachTemp());
      else
        FlashP=Q.SaturationP(m_PCtrl0.TRqd()-m_VLE.ApproachTemp());
      }
    else if (m_PCtrl0.Method()==PC_Atmos)
      FlashP=AtmosPress();
    else if (!MassSmoothing())
      FlashP=m_FTF.m_dPRqd;
    }
  else if (fOnLine)
    {
    if (m_PCtrl0.Method()==PC_Reqd)
      FlashP=m_PCtrl0.PRqd();
    else if (m_PCtrl0.Method()==PC_ReqdSatT)
      {
      if (m_VLE.SatPMethod()==SMO_SatPPartialP)
        FlashP=Q.SaturationTotalP(m_PCtrl0.TRqd()-m_VLE.ApproachTemp());
      else
        FlashP=Q.SaturationP(m_PCtrl0.TRqd()-m_VLE.ApproachTemp());
      }
    else
      FlashP=AtmosPress();
    }
  else
    FlashP=m_FTF.m_dFeedP;

  FlashP = Max(MinFlshTrainP, FlashP);
  m_PCtrl0.SetP2Set(FlashP, true, true);
  return FlashP;
  }

//--------------------------------------------------------------------------

void FlashTank::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    {
    if (NetMethod()==NM_Probal || SolveMethod()!=SM_Buffered)
      {
      m_FTF.m_dFeedP=MeasureJoinPressure(0, &m_PCtrl0);
      m_FTF.m_dFlashP=GetFlashPressure(m_QProd);
      SetJoinPressure(0, &m_PCtrl0);
      }
    else
      MN_Surge::EvalJoinPressures(JoinMask);
    }
  };

//--------------------------------------------------------------------------

void FlashTank::SetState(eScdMdlStateActs RqdState)
  {
  MN_Surge::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      m_FTF.SetState(RqdState);
#if DOPBSMOOTHING  
      m_fSolnVapQmValid=false;
      m_dSolnVapQm=0.0;
#endif
      m_nRmtScl=0;
      break;
    default:
      break;

      //    case MSA_Empty:
      //    case MSA_PreSet:
      //      break;
      //    case MSA_SteadyState:
      //      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      //      break;
    }
  };

//--------------------------------------------------------------------------

void FlashTank::EvalSteadyState()
  {
  };

//--------------------------------------------------------------------------

void FlashTank::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoFlwIOs()>0)
    {
    switch (SolveMethod())
      {
      case SM_Direct:
      case SM_Inline:
        {
        if (fDoDbgBrk)
          { int xxx=0; };

        const int ioLiq = IOWithId_Self(ioidFT_Liq);
        const int ioVap = IOWithId_Self(ioidFT_Vap);

        SpConduit & Ql=*IOConduit(ioLiq);
        SpConduit & Qv=*IOConduit(ioVap);

        Ql.SetVLEBlk(&m_VLE);
        Qv.SetVLEBlk(&m_VLE);

#if dbgModels
        double MemTl, MemPl, MemHl, MemMl;
        double MemTv, MemPv, MemHv, MemMv;
        if (NetProbalMethod() && dbgDumpChanges())
          {
          MemTl=Ql.Temp();
          MemPl=Ql.Press();
          MemHl=Ql.totHf();
          MemMl=Ql.QMass();
          MemTv=Qv.Temp();
          MemPv=Qv.Press();
          MemHv=Qv.totHf();
          MemMv=Qv.QMass();
          }
#endif

        double PIn=SigmaQInPMin(m_QFeed, som_ALL, First64IOIds);

        dTempIn = m_QFeed.Temp();
        dBPEIn = m_QFeed.BoilingPtElevation(PIn);

        Qv.SetModel(sVapSpModel());

        m_VLE.SetHfInAtZero(m_QFeed);

        Ql.QZero();
        Qv.QZero();
        Ql.QSetF(m_QFeed, som_ALL, 1.0, m_QFeed.Press());

        flag OK=false;
        if (fOnLine)
          {
          m_RB.EvalProducts(Ql);
          m_EHX.EvalProducts(Ql);

#if WITHNONCONDENSABLES
          const int idNCVent=IOWithId_Self(ioidFT_NCVent);
          if (idNCVent>=0)
            {
            m_QWrk.QSetF(Ql, som_SL, 1.0, PIn);
            SpConduit& NonCondVent = *(IOConduit(idNCVent));
            NonCondVent.QSetF(Ql, som_Gas, 1.0, PIn);
            Ql.QCopy( m_QWrk);
            }
          else
            {
            m_QWrk.QCopy(Ql, PIn); //leave vapours in slurry...
            }
          SetCI(1, bTrackVapFdStatus && idNCVent<0 && Ql.MassFrac(som_Gas)>1.0e-3);//Should this test be before or after RB???  Should this only test for OtherVapours (or steam as well???)
#else
          //Should this test be before or after RB???  Should this only test for OtherVapours (or steam as well???)
          SetCI(1, m_bTrackVapFdStatus && Ql.MassFrac(som_Gas)>1.0e-3);
          m_QWrk.QCopy(Ql, PIn);
#endif

          double QLMass=Ql.QMass();
          if (QLMass>UsableMass)
            {//TODO : CHECK! ... Does all this m_VLE stuff work if (1) steam is present in feed mixture? (2) other vapours are present in feed mixture?
            OK=true;
            m_FTF.m_dFeedVapP=Ql.SaturationTotalP(Ql.Temp());
            double PMargin=0.0001;
            Qv.CopyQualities(Ql);

            // CNM - Why is this here ?
            //m_VLE.PFlash(Ql, Qv, m_FTF.m_dSuctionP, 0.0, VLEF_SaturationLimit);

            int ZeroQvCount=0;
            for (int Iter=0;  ;Iter++)
              {
              double PrevFlashP=m_FTF.m_dFlashP;
              PMargin *= 10.0;
              flag DoQP=true;
              if (m_FTF.Active())
                {
                m_FTF.m_dFlashP=Min(m_FTF.m_dFlashP, m_FTF.m_dFeedVapP-PMargin); // Force a small amount of Vapour
                //dbgpln("xlashP2   %10.2f", m_FTF.m_dFlashP-101.325);
#if DOPBSMOOTHING  
                if (m_fSolnVapQmValid && m_dSolnVapQm>0.0)
                  {
                  m_VLE.MVapFlash(Ql, Qv, m_dSolnVapQm, 0.0, 0);
                  m_FTF.m_dFlashP=Ql.Press(); 
                  //dbgpln("xlashP2   %10.2f", m_FTF.m_dFlashP-101.325);
                  m_FTF.m_dPRqd=m_FTF.m_dFlashP;
                  //m_FTF.dPRqdCalc=m_FTF.dFlashP;
                  DoQP=false;
                  }
#endif
                m_FTF.m_dPRqdCalc=m_FTF.m_dFlashP;
                }
              if (DoQP)
                m_VLE.PFlash(Ql, Qv, m_FTF.m_dFlashP, 0.0, VLEF_SaturationLimit|VLEF_HoldFeedMeasure);

              m_QWrk.QCopy(Ql, m_FTF.m_dFlashP/*PIn*/);
              m_QWrk.QAddF(Qv, som_ALL, 1.0);
              m_FTF.m_dFlashT=Ql.Temp();
#if FORCESTEAM
              m_FTF.m_dQVap=Qv.VMass[H2OVap()]; //steam only!
#else
              m_FTF.m_dQVap=Qv.QMass(som_ALL); //assumes steam!!!
#endif

              m_FTF.m_dFlashP=GetFlashPressure(m_QWrk);

              if (Iter>50)
                {
                LogError(FullObjTag(), 0, "Flash Pressure Not Converged");
                break;
                }
              if (m_FTF.m_dQVap>=1.0e-6*QLMass)
                {
                ZeroQvCount=0;
                if (fabs(PrevFlashP-m_FTF.m_dFlashP)<1e-6)
                  break;
                }
              else if (Iter>=3)
                {
                // Not able to flash any vapour - make sure this is true
                if (ZeroQvCount++>=5)
                  break;
                }
              }
            }
          }

        if (!OK)
          {
          m_FTF.m_dFlashT=Ql.Temp();
          m_FTF.m_dFlashP=PIn;
          dbgpln("xlashP3   %10.2f", m_FTF.m_dFlashP-101.325);
          Qv.QSetF(Ql, som_ALL, 0.0, PIn);
          m_FTF.m_dQVap=0.0;
          }
        m_QProd.QCopy(m_QWrk);
        dWaterSatT = Ql.PureSaturationT(m_FTF.m_dFlashP);
        dBPE = Ql.BoilingPtElevation(m_FTF.m_dFlashP);

#if WITHOTHERVAPSPLIT
        if (1)
          {
          double T=Ql.Temp();
          double P=Ql.Press();
          SpecieIter It(som_Gas);
          for (int g=-1; It.Loop(g); )
            {
            double Splt= (g==m_VLE.SatPVapIndex()) ? m_dSatPVapFracToLiq : m_dOtherVapFracToLiq;
            double G=Qv.VValue[g]+Ql.VValue[g];
            Qv.VValue[g]=G*(1.0-Splt);
            Ql.VValue[g]=G*Splt;
            }

          Qv.SetTempPress(T,P);
          Ql.SetTempPress(T,P);
          }
#endif

        m_VLE.AddHfOutAtZero(Ql);
        m_VLE.AddHfOutAtZero(Qv);

#if dbgModels
        if (NetProbalMethod() && dbgDumpChanges())
          {
          dbgpln("--- %s",FullObjTag());

#define DBGIT(A,B,C) dbgpln("    %15.6f %15.6f %15.6f %25.20f %s", A, B, (B-A), 100*(B-A)/GTZ(Max(fabs(A), fabs(B))), C);

          DBGIT(MemMl, Ql.QMass(), "Qml");
          DBGIT(MemTl, Ql.Temp(),  "Tl");
          DBGIT(MemPl, Ql.Press(), "Pl");
          DBGIT(MemHl, Ql.totHf(), "Hl");
          DBGIT(MemMv, Qv.QMass(), "Qmv");
          DBGIT(MemTv, Qv.Temp(),  "Tv");
          DBGIT(MemPv, Qv.Press(), "Pv");
          DBGIT(MemHv, Qv.totHf(), "Hv");

#undef DBGIT
          }
#endif
        }
        break;
      default:
        MN_Surge::EvalProducts(NEI);
      }
    }
  }

//--------------------------------------------------------------------------

void FlashTank::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (m_EHX.Enabled())
      CI.m_EHXPowerIn+=m_EHX.HeatFlow();
    if (1)
      CI.m_HfGainAtZero+=m_VLE.HfGainAtZero();
    }
  };

//--------------------------------------------------------------------------

flag FlashTank::MacroMdlValidNd(int iIONo) 
  { 
  return fOnLine;
  };

//--------------------------------------------------------------------------

void FlashTank::MacroMdlEvaluate(eScdMacroMdlEvals Eval)
  { 
  if (m_FTF.Active())
    {
    switch (Eval)
      {
      case MME_Flash2P:
        {
        double P0=m_FTF.m_dPEval;
        m_VLE.PFlash(m_QWrk, P0, 0.0, VLEF_Null);
#if FORCESTEAM
        double Q0= m_QWrk.VMass[H2OVap()];
#else
        double Q0=m_QWrk.QMass(som_Gas);
#endif
        double P1=P0+0.1; // CNM
        m_VLE.PFlash(m_QWrk, P1, 0.0, VLEF_Null);
#if FORCESTEAM
        double Q1= m_QWrk.VMass[H2OVap()];
#else
        double Q1= m_QWrk.QMass(som_Gas);
#endif
        m_FTF.m_dDQvDp=(Q0-Q1)/(P0-P1); // should be negative
        m_FTF.m_fDQvDpGood=( m_QWrk.QMass()>1.0e-10);
#if dbgModels   
        if (dbgFlashTrain())
          dbgpln("FlashMM2P  %12.6f @ %12.6f %s", m_FTF.m_dDQvDp, m_FTF.m_dPEval, Tag());
#endif
        }
        break;
      case MME_Flash2Qv:
        {
        double P0=m_FTF.m_dPEval;
        m_VLE.PFlash( m_QWrk, P0, 0.0, VLEF_Null);
#if FORCESTEAM
        m_FTF.m_dQVapEval= m_QWrk.VMass[H2OVap()]; //steam only!
#else
        m_FTF.m_dQVapEval= m_QWrk.QMass(som_Gas);
#endif
#if dbgModels   
        if (dbgFlashTrain())
          dbgpln("FlashMM2Qv %12.6f @ %12.6f %s", m_FTF.m_dQVapEval, m_FTF.m_dPEval, Tag());
#endif
        }
        break;
      case MME_Flash2QvSens:
        {
        //double P0=m_FTF.dPEval;
        m_VLE.MVapFlash( m_QWrk, m_FTF.m_dQVapEval, 0.0, VLEF_Null);
        m_FTF.m_dPEval= m_QWrk.Press();
        //        #if dbgModels   
        //        if (dbgFlashTrain())
        //          dbgpln("FlashMM2QS %12.6f @ %12.6f %s", m_FTF.dQVapEval, m_FTF.dPEval, Tag());
        //        #endif
        }
        break;
      }
    }
  };

//--------------------------------------------------------------------------

CMacroMdlBase* FlashTank::MacroMdlActivate() 
  { 
  return &m_FTF; 
  };

//--------------------------------------------------------------------------

void FlashTank::MacroMdlDeactivate() 
  { 
  };

//===========================================================================
#if DOPBSMOOTHING  
#if DOPB_FILT_ALPHAS
#define M_EST m_Est[0]
#define M_AVG m_Avg
#else
#define M_EST m_Avg
#define M_AVG m_Avg
#endif

void FlashTank::EvalMsAGDatas()
  {
  if (m_FTF.Active())
    {
    CVLEDataArray Cmp;
    if (m_VLE.GetVLEData(Cmp)<1)
      {
      MN_Surge::EvalMsAGDatas();
      return;
      }

    int nDstIOs=0;
    for (int i=0; i<NoFlwIOs(); i++)
      if (IO_Out(i))
        nDstIOs++;

    MsAGDatas.SetSize(nDstIOs);

    //SpMArray MassIn=0;
    //SpMArray MassOut=0;

    //    if (pMIn && pMIn[0])
    //      MassIn=*pMIn[0];
    //    if (pMOut && pMOut[0])
    //      MassOut=*pMOut[0];

    int iDstIO=0;
    SpMArray MTotI=0;
    SpMArray MTotO=0;
    CMSIO MSrcIOs[MaxIOList];
    int nMSrcIOs=0;
    CMSIO MOutIOs[MaxIOList];
    int nMOutIOs=0;
    for (i=0; i<NoFlwIOs(); i++)
      if (IO_In(i))
        {
        MSrcIOs[nMSrcIOs].SetSrc(this, i);
        //        MSrcIOs[nMSrcIOs].pRmtLnk=Nd_Rmt(i);
        nMSrcIOs++;
        MTotI+=IOConduit(i)->MArray();
        }
      else if (IO_Out(i))
        {
        MOutIOs[nMOutIOs].SetSrc(this, i);
        nMOutIOs++;
        MTotO+=IOConduit(i)->MArray();
        }

      double TotI=MTotI.Sum();
      double TotO=MTotO.Sum();
      int ioLiq = IOWithId_Self(ioidFT_Liq);
      int ioVap = IOWithId_Self(ioidFT_Vap);
      if (IO_Out(ioVap))
        { // Add Dst
        double QmTot=IOConduit(ioVap)->QMass();
        CMsAGData & MSA=MsAGDatas[iDstIO++];
#if DOPB_FILT_ALPHAS
        MSA.M_EST.SetSize(1);
#endif

        MSA.m_Op=CMSOp_RmtMult;
        MSA.m_iJoinNo=0;//j;
        MSA.m_pNd=this;
        MSA.m_fMergeSrc=false;
        MSA.m_fMergeDst=false;
        MSA.m_fMergeDstIfUnity=true;

        MSA.m_DstIO.SetDst(this, ioVap, &MSA);
        MSA.m_SrcIOs.SetSize(nMSrcIOs);
        for (int m=0; m<MSA.m_SrcIOs.GetSize(); m++)
          MSA.m_SrcIOs[m]=MSrcIOs[m];

        MSA.m_RmtIOs.SetSize(m_FTF.CoolLnks.GetSize());
        for (m=0; m<MSA.m_RmtIOs.GetSize(); m++)
          MSA.m_RmtIOs[m]=m_FTF.CoolLnks[m];

        MSA.M_EST.m_MConst.Init();
        //MSA.M_EST.m_MConst.SetAll(0);

        double NewRmtScl=m_FTF.dQFrac*(m_FTF.dQRqdTotal+m_FTF.dVapLoss)/GTZ(m_FTF.m_dQCool);
        double AvgRmtScl=0.0;

        int j;
        if (m_nRmtScl==0)
          {
          m_nRmtScl=3;//sizeof(m_RmtScl)/sizeof(m_RmtScl[0]);
          for (j=0; j<m_nRmtScl; j++)        
            m_RmtScl[j]=NewRmtScl;
          }

        for (j=m_nRmtScl-1; j>0; j--)        
          m_RmtScl[j]=m_RmtScl[j-1];
        m_RmtScl[0]=NewRmtScl;

        dbgp("RmtScl ");
        for (j=0; j<m_nRmtScl; j++)        
          {
          AvgRmtScl+=m_RmtScl[j];
          dbgp(" %12.8f ", m_RmtScl[j]);
          }
        AvgRmtScl/=m_nRmtScl;
        NewRmtScl=AvgRmtScl;

        dbgpln(" = %12.8f ", NewRmtScl);

#if dbgModels   
        double xxx=(NewRmtScl-MSA.M_EST.m_RmtScl)/GTZ(Max(fabs(NewRmtScl), fabs(MSA.M_EST.m_RmtScl)));
        if (dbgMSProgress())
          dbgp("RmtScl %12.8f >> ", MSA.M_EST.m_RmtScl);
#endif
        static double Filt=0.90;//0.75;
        if (NewRmtScl<1.0e-12)
          NewRmtScl=1.0e-6;
        if (MSA.M_EST.m_RmtScl>0.0)
          MSA.M_EST.m_RmtScl=Filt*MSA.M_EST.m_RmtScl+(1-Filt)*NewRmtScl;
        else
          MSA.M_EST.m_RmtScl=NewRmtScl;
#if dbgModels   
        if (dbgMSProgress())
          dbgpln("%12.8f [%12.8f] %10.4f%% %s", MSA.M_EST.m_RmtScl, NewRmtScl, xxx, sTag());
#endif

        MSA.M_EST.m_MPhi.Init();
        MSA.M_EST.m_SpReqd.Init();
        for (int s=0; s<SDB.No(); s++)
          {
          MSA.M_EST.m_MPhi[s]=ChkMeaningFull(IOConduit(ioVap)->Qm(s)/GTZ(QmTot));
          if (IOConduit(ioVap)->Qm(s)!=0.0)
            MSA.M_EST.m_SpReqd[s]=true;
          }
        }

      if (IO_Out(ioLiq))
        { // Add Dst
        CMsAGData & MSA=MsAGDatas[iDstIO++];
#if DOPB_FILT_ALPHAS
        MSA.M_EST.SetSize(1);
#endif

        MSA.m_Op=CMSOp_IODiff;
        MSA.m_iJoinNo=0;//j;
        MSA.m_pNd=this;
        MSA.m_fMergeSrc=false;
        MSA.m_fMergeDst=false;
        MSA.m_fMergeDstIfUnity=true;

        MSA.m_DstIO.SetDst(this, ioLiq, &MSA);
        MSA.m_SrcIOs.SetSize(nMSrcIOs);
        for (int m=0; m<MSA.m_SrcIOs.GetSize(); m++)
          MSA.m_SrcIOs[m]=MSrcIOs[m];
        MSA.m_MOutIOs.SetSize(nMOutIOs-1);
        int n=0;
        for (m=0; m<nMOutIOs; m++)
          if (MOutIOs[m].IONo()!=ioLiq)
            MSA.m_MOutIOs[n++]=MOutIOs[m];
        MSA.M_EST.m_MBeta.SetAll(0.0);
        //MSA.M_EST.m_MAlpha.Init();
        MSA.M_EST.m_MGamma.Init(true);
        MSA.M_EST.m_MBeta.Init(false);
        MSA.M_EST.m_SpReqd.Init();
        for (int s=0; s<SDB.No(); s++)
          {
          //        MSA.M_EST.m_MAlpha[s]=(MTotI[s]+MassIn[s])>1.0e-12 ? 1.0 : 0.0;
          //        MSA.M_EST.m_MGamma[s][s]=(MTotI[s]/*+MassIn[s]*/)>1.0e-12 ? 1.0 : 0.0;
          //        MSA.M_EST.m_MGamma[s][s]=1.0;
          if (MTotO[s]!=0.0)
            MSA.M_EST.m_SpReqd[s]=true;
          //MSA.M_EST.m_MBeta.SetSize(SDB.No(), SDB.No());
          for (int j=0; j<Cmp.GetSize(); j++)
            if (s==Cmp[j].li)
              {
              MSA.M_EST.m_MBeta[s][Cmp[j].li]=-1.0;
              MSA.M_EST.m_MBeta[s][Cmp[j].vi]=1.0;
              }
            else if (s==Cmp[j].vi)
              {
              //MSA.MBeta[s][s]=0.0;
              }
            else
              {
              MSA.M_EST.m_MBeta[s][s]=0.0;
              if (SDB[s].IsGas())
                MSA.M_EST.m_MGamma[s][s]=0.0;//Loss*Alpha;
              }
          }
        }
    }
  else
    MN_Surge::EvalMsAGDatas();
  };

//--------------------------------------------------------------------------

void FlashTank::UpdateMsSoln(SparseSolver & SSMS)
  {
  int ioVap = IOWithId_Self(ioidFT_Vap);
  if (IO_Out(ioVap))
    { 
    // The flows have now been updated
    SpConduit & Vap=*IOConduit(ioVap);
    m_fSolnVapQmValid=true;

#if dbgModels   
    if (dbgMSProgress())
      dbgpln("UpdateMsSoln %10.6f >> %10.6f  %s", m_dSolnVapQm,Vap.QMass(), FullObjTag());
#endif
    m_dSolnVapQm=Vap.QMass();
    int xxx=0;
    }

  };
#endif
//===========================================================================

flag FlashTank::InitialiseSolution()
  {
  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Buffered:
      MN_Surge::InitialiseSolution();
      break;
    case SM_Inline:
      Contents.dwDisplay=SPDF_ShapeData|SPDF_PBMode;
      Contents.SetStateAction(IE_Disabled);
      m_EqThermals.SetStateAction(IE_Disabled);
      break;
    }

  return 1;
  };

//--------------------------------------------------------------------------

flag FlashTank::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tExpect no Vapour in feed mixture"; return 1;
    case 2: pS="E\tBad pressure method configuration"; return 1;
    case 3: pS="W\tBad VapSpModel"; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

