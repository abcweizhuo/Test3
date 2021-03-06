//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __LoadBasedSeperator_CPP
#include "SysCADSystemHelper.h"
#include "LoadBasedSeperator.h"
//#pragma optimize("", off)

//====================================================================================
//             Load Based Screen
//====================================================================================

CSeperator_LoadBased::CSeperator_LoadBased()
	{
  // Construct the Persistent System Feed Data

  FeedStream  = RioTintoTS::PFlowStream1( new RioTintoTS::FlowStream1 );
  MatInfo     = RioTintoTS::PStreamInfo1( new RioTintoTS::StreamInfo1 );

	//
	// Default Parameters
	//
  m_dTopApperture		= 0.012;
  m_dTopLength			=  5.0;
  m_dTopAngle			=  3.14/9.0;// 20 deg
  m_dTopWidth			=  5.0;
  m_dTopOpenFraction		=  1.0;
  m_dTopBulkDensity		=  2000;//kg/m^3
  m_bTopWetScreening		=  false;
  m_lTopAppetureShape	=  1;
  m_lTopMediaType		=  1;
  m_lTopDeckLocation		=  1;
  m_bTopGravel			=  false;
  m_dTopCustomFactor		=  1.0;
  m_dTopWaterSplitToUS	=  0.0;

  m_dBottomApperture		= 0.0121;
  m_dBottomLength		=  5.0;
  m_dBottomAngle			=  3.14/9.0;// 20 deg
  m_dBottomWidth			=  5.0;
  m_dBottomOpenFraction	=  1.0;
  m_dBottomBulkDensity	=  2000;// kg/m^3
  m_bBottomWetScreening	=  false;
  m_lBottomAppetureShape	=  1;
  m_lBottomMediaType		=  1;
  m_lBottomDeckLocation	=  1;
  m_bBottomGravel		=  false;
  m_dBottomCustomFactor	=  1.0;
  m_dBottomWaterSplitToUS=  0.0;

	 //
	 // Default Outputs
	 //
	 m_dTopQF=0.0;		//  total tph of feed
	 m_dTopFeed_OS=0.0;	//  fraction of feed > apperture
	 m_dTopFeed_US=0.0;	//  fraction of feed < apperture
	 m_dTopFeed_HS=0.0;	//  fraction of feed < half-apperture
	 m_dTopQU=0.0;		//  flowrate of undersize stream
	 m_dTopQO=0.0;		//  flowrate of oversize stream
	 m_dTopS=0.0;		//  apperture size
	 m_dTopT=0.0;		//  screen efficiency (at S)
	 m_dTopAF=0.0;		//  effective screen area
	 m_dTopD50=0.0;		//  fitted d50 of separation
	 m_dTopA=0.0;	    //  basic capacity factor
	 m_dTopB=0.0;		//  over-size factor
	 m_dTopC=0.0;		//  half-size factor
	 m_dTopD=0.0;		//  deck location factor
	 m_dTopE=0.0;		//  wet screening factor
	 m_dTopF=0.0;		//  feed bulk density factor
	 m_dTopG=0.0;		//  load vs efficiency factor
	 m_dTopH=0.0;		//  opening shape factor
	 m_dTopJ=0.0;		//  screen open area factor
	 m_dTopK=0.0;		//  rock/gravel factor
	 m_dTopL=0.0;		//  humidity factor
	 m_dTopX=0.0;		//  user custom factor
	 m_dTopV=0.0;		//  screen load %

	 m_dBottomQF=0.0;		//  total tph of feed
	 m_dBottomFeed_OS=0.0;	//  fraction of feed > apperture
	 m_dBottomFeed_US=0.0;	//  fraction of feed < apperture
	 m_dBottomFeed_HS=0.0;	//  fraction of feed < half-apperture
	 m_dBottomQU=0.0;		//  flowrate of undersize stream
	 m_dBottomQO=0.0;		//  flowrate of oversize stream
	 m_dBottomS=0.0;		//  apperture size
	 m_dBottomT=0.0;		//  screen efficiency (at S)
	 m_dBottomAF=0.0;		//  effective screen area
	 m_dBottomD50=0.0;		//  fitted d50 of separation
	 m_dBottomA=0.0;	    //  basic capacity factor
	 m_dBottomB=0.0;		//  over-size factor
	 m_dBottomC=0.0;		//  half-size factor
	 m_dBottomD=0.0;		//  deck location factor
	 m_dBottomE=0.0;		//  wet screening factor
	 m_dBottomF=0.0;		//  feed bulk density factor
	 m_dBottomG=0.0;		//  load vs efficiency factor
	 m_dBottomH=0.0;		//  opening shape factor
	 m_dBottomJ=0.0;		//  screen open area factor
	 m_dBottomK=0.0;		//  rock/gravel factor
	 m_dBottomL=0.0;		//  humidity factor
	 m_dBottomX=0.0;		//  user custom factor
	 m_dBottomV=0.0;		//  screen load %

//-----------------------------------------------------------------------
  const int nRowsA = 19;
  static double AArray [ nRowsA ] [ 2 ] = 
	{
//          S       A	
//      Screen    Basic
//      Opening Capacity
        { 0    ,   0.0 },
		{ 1    ,   1.1 },
		{ 2    ,   2.0 },
		{ 3    ,   3.0 },
		{ 4    ,   3.9 },
		{ 5    ,   4.8 },
		{ 6.35 ,   5.9 },
		{ 8    ,   7.0 },
		{ 10   ,   8.2 },
		{ 12   ,   9.4 },
		{ 15   ,  10.7 },
		{ 18   ,  12.0 },
		{ 22   ,  13.4 },
		{ 26   ,  14.6 },
		{ 30   ,  15.8 },
		{ 40   ,  18.2 },
		{ 75   ,  25.0 },
		{ 160  ,  41.5 },
		{ 500  , 107.5 }, };
	m_A.InitParams("A", "S", "Screen Opening", "A", "Basic Capacity", nRowsA);
	m_A.SetData((double*)&AArray);


	const int nRowsB = 7;
	static double BArray [ nRowsB ] [ 2 ] = 
	{
//       Fract       B	
//      Oversize  Oversize
//      in Feed    Factor
        { 0.00  ,  1.60 },
		{ 0.50  ,  1.00 },
		{ 0.80  ,  0.64 },
		{ 0.85  ,  0.56 },
		{ 0.90  ,  0.44 },
		{ 0.95  ,  0.24 },
		{ 1.00  ,  0.00 }, };
	m_B.InitParams("B", "Frac", "Oversize in Feed", "B", "Oversize Factor", nRowsB);
	m_B.SetData((double*)&BArray);
	

  const int nRowsC = 18;
  static double CArray [ nRowsC ] [ 2 ] = 
    {
//      Fract of    C	
//     Feed < S/2
//                Factor
        { 0.00 ,  0.70 },
        { 0.25 ,  1.00 },
        { 0.30 ,  1.06 },
        { 0.35 ,  1.13 },
        { 0.40 ,  1.21 },
        { 0.45 ,  1.30 },
        { 0.50 ,  1.40 },
        { 0.55 ,  1.52 },
        { 0.60 ,  1.67 },
        { 0.65 ,  1.85 },
        { 0.70 ,  2.05 },
        { 0.75 ,  2.26 },
        { 0.80 ,  2.50 },
        { 0.85 ,  2.75 },
        { 0.90 ,  3.20 },
        { 0.95 ,  4.00 },
        { 0.98 ,  6.00 },
        { 1.00 , 10.00 }, };
	m_C.InitParams("C", "Frac", "Feed < S/2", "C", "Factor", nRowsC);
	m_C.SetData((double*)&CArray);


	const int nRowsE = 10;
	static double EArray [ nRowsE ] [ 2 ] = 
	{
//        S          E	
//     Screen  Wet Screening
//     Opening    Factor
#ifdef OLD
        { 0    ,   1.0 },
		{ 1    ,   1.5 },
		{ 3    ,   2.0 },
		{ 5    ,   2.0 },
		{ 8    ,   1.6 },
		{ 11   ,   1.4 },
		{ 17   ,   1.2 },
		{ 22   ,   1.1 },
		{ 35   ,   1.0 },
		{ 40   ,   1.0 }, };
#endif
        { 0    ,   2.88 },
		{ 1    ,   3.0 },
		{ 3    ,   3.0 },
		{ 5    ,   2.95 },
		{ 8    ,   2.93 },
		{ 11   ,   2.88 },
		{ 17   ,   2.73 },
		{ 22   ,   2.63 },
		{ 35   ,   1.88 },
		{ 40   ,   1.00 }, };
	m_E.InitParams("E", "S", "Screen Opening", "E", "Wet Screening Factor", nRowsE);
	m_E.SetData((double*)&EArray);


  // Dry Screening Efficiency Curve
  const int nDryRows = 11;
  static double TDryArray [ nDryRows ] [ 2 ] = 
  {
  //          V        T	
  //        Load   Efficiency
      {  000.0 , 0.90 },
      {  062.5 , 0.95 },
      {  075.0 , 0.95 },
      {  100.0 , 0.90 },
      {  450.0 , 0.50 },
      {  500.0 , 0.45 },
      {  562.5 , 0.40 },
      {  642.9 , 0.35 },
      {  750.0 , 0.30 },
      {  900.0 , 0.25 },
      { 1125.0 , 0.20 }, };
	m_Td.InitParams("Td", "V", "Load", "T", "Dry Efficiency", nDryRows);
	m_Td.SetData((double*)&TDryArray);


  // Wet Screening Efficiency Curve
  const int nWetRows = 22;
  static double TWetArray [ nWetRows ] [ 2 ] = 
  {
  //          V        T	
  //        Load   Efficiency
  /*	{ 10	, 	.9917317  },
  { 20	, 	.9927046  },
  { 30	, 	.9910998  },
  { 39.3	, 	.9871216  },
  { 50	, 	.9776228  },
  { 60.5	, 	.963927	  },
  { 66.3	, 	.9553783  },
  { 75	, 	.9416755  },
  { 90	, 	.9167897  },
  { 100	 ,	.9001193  },
  { 110	 ,	.8838962  },
  { 120	 ,	.8684178  },
  { 130	 ,	.853857	  },
  { 138.9	 ,	.8417114  },
  { 144.1	 ,	.8349535  },
  { 150.9	 ,	.8264498  },
  { 157	 ,	.8190873  },
  { 169	 ,	.8050404  },
  { 184.5	 ,	.7866662  },
  { 200	 ,	.7659365  },
  { 210	 ,	.7499636  }};*/

  { 10	, 	.997  },
  { 20	, 	.996  },
  { 30	, 	.99   },
  { 39.3	, 	.982  },
  { 50	, 	.98   },
  { 63	, 	.96	  },
  { 75	, 	.95   },
  { 80	, 	.942  },
  { 90	, 	.923  },
  { 100	 ,	.90   },
  { 110	 ,	.887  },
  { 120	 ,	.873  },
  { 130	 ,	.862  },
  { 140	 ,	.849  },
  { 150	 ,	.837  },
  { 160	 ,	.827  },
  { 170	 ,	.813  },
  { 179.2	 ,	.801  },
  { 190	 ,	.785  },
  { 200	 ,	.77   },
  { 210	 ,	.755  },
  { 300    ,  .6    }};
	m_Tw.InitParams("Tw", "V", "Load", "T", "Wet Efficiency", nWetRows);
	m_Tw.SetData((double*)&TWetArray);
  }

void CSeperator_LoadBased::BuildDataFields(MDataDefn &DB)
	{ 
	static MDDValueLst DDAppetureShape[]={
		{1,   "Round"},
		{2,   "Square"},
		{3,   "Slot 2:1"},
		{4,   "Slot 3:1"},
		{5,   "Slot 4:1"},
		{0}};
	static MDDValueLst DDMediaType[]={
		{1,   "Wire"},
		{2,   "Plate"},
		{3,   "Rubber"},
		{4,   "Poly"},
		{0}};
	static MDDValueLst DDDeckLocation[]={
		{1,   "Top"},
		{2,   "Bottom"},
		{0}};

	DB.ObjectBegin("TS_LoadSep", "LoadBasedScreen");

  // SysCAD Tag Name convention ==> no spaces between words
  // Use MF_VISIBLE to make parameters invisible

  DB.Text("");
  DB.Text("Load Based Screen");

  //
  // Parameters
  //
  DB.StructBegin("Top" );
  DB.Double("Apperture"    , "", &m_dTopApperture      , MF_PARAMETER ,  MC_L("mm"));
  DB.Double("Length"       , "", &m_dTopLength         , MF_PARAM_STOPPED ,  MC_L("mm"));
  DB.Double("Width"        , "", &m_dTopWidth          , MF_PARAM_STOPPED ,  MC_L("mm"));
  DB.Double("Angle"        , "", &m_dTopAngle          , MF_PARAM_STOPPED | MF_INIT_HIDDEN ,  MC_Ang("deg"));
  DB.Double("OpenFraction" , "", &m_dTopOpenFraction   , MF_PARAM_STOPPED ,  MC_Frac("%"));
  DB.Double("BulkDensity"  , "", &m_dTopBulkDensity    , MF_PARAM_STOPPED ,  MC_Rho("t/m^3"));
  DB.Bool  ("WetScreening" , "", &m_bTopWetScreening   , MF_PARAM_STOPPED  );
  DB.Long  ("AppetureShape", "", &m_lTopAppetureShape  , MF_PARAM_STOPPED, DDAppetureShape);
  DB.Long  ("MediaType"    , "", &m_lTopMediaType      , MF_PARAM_STOPPED, DDMediaType);
  DB.Long  ("DeckLocation" , "", &m_lTopDeckLocation   , MF_PARAM_STOPPED, DDDeckLocation);
  DB.Bool  ("Gravel"       , "", &m_bTopGravel         , MF_PARAM_STOPPED  );
  DB.Double("CustomFactor" , "", &m_dTopCustomFactor   , MF_PARAM_STOPPED ,  MC_None);
  DB.Double("WaterSplitToUS","", &m_dTopWaterSplitToUS , MF_PARAMETER ,  MC_Frac("%"));
  DB.StructEnd();

  DB.Text("");
  DB.StructBegin("Bottom" );
  DB.Double("Apperture"    , "", &m_dBottomApperture      , MF_PARAMETER ,  MC_L("mm"));
  DB.Double("Length"       , "", &m_dBottomLength         , MF_PARAM_STOPPED ,  MC_L("mm"));
  DB.Double("Width"        , "", &m_dBottomWidth          , MF_PARAM_STOPPED ,  MC_L("mm"));
  DB.Double("Angle"        , "", &m_dBottomAngle          , MF_PARAM_STOPPED | MF_INIT_HIDDEN,  MC_Ang("deg"));
  DB.Double("OpenFraction" , "", &m_dBottomOpenFraction   , MF_PARAM_STOPPED ,  MC_Frac("%"));
  DB.Double("BulkDensity"  , "", &m_dBottomBulkDensity    , MF_PARAM_STOPPED ,  MC_Rho("t/m^3"));
  DB.Bool  ("WetScreening" , "", &m_bBottomWetScreening   , MF_PARAM_STOPPED  );
  DB.Long  ("AppetureShape", "", &m_lBottomAppetureShape  , MF_PARAM_STOPPED, DDAppetureShape);
  DB.Long  ("MediaType"    , "", &m_lBottomMediaType      , MF_PARAM_STOPPED, DDMediaType);
  DB.Long  ("DeckLocation" , "", &m_lBottomDeckLocation   , MF_PARAM_STOPPED, DDDeckLocation);
  DB.Bool  ("Gravel"       , "", &m_bBottomGravel         , MF_PARAM_STOPPED  );
  DB.Double("CustomFactor" , "", &m_dBottomCustomFactor   , MF_PARAM_STOPPED ,  MC_None);
  DB.Double("WaterSplitToUS","", &m_dBottomWaterSplitToUS , MF_PARAMETER ,  MC_Frac("%"));
  DB.StructEnd();

  DB.Page("Regr1");
	DB.ObjectBegin("TS_LoadSep_Regr", "Regr"); //force this into separate data table in database
  m_A.BuildDataFields(DB);
  m_B.BuildDataFields(DB);
  DB.Page("Regr2");
  m_C.BuildDataFields(DB);
  m_E.BuildDataFields(DB);
  DB.Page("Regr3");
  m_Td.BuildDataFields(DB);
  m_Tw.BuildDataFields(DB);
  DB.ObjectEnd();

  //
  // Outputs
  //
  DB.Page("Results");

  DB.StructBegin("TopRes" );
  DB.Double("QF"			, "", &m_dTopQF			, MF_RESULT ,  MC_None);
  DB.Double("Feed_OS"	, "", &m_dTopFeed_OS, MF_RESULT ,  MC_None);
  DB.Double("Feed_US"	, "", &m_dTopFeed_US, MF_RESULT ,  MC_None);
  DB.Double("Feed_HSperture"	, "", &m_dTopFeed_HS	, MF_RESULT ,  MC_None);
  DB.Double("QU"			, "", &m_dTopQU			, MF_RESULT ,  MC_None);
  DB.Double("QO"			, "", &m_dTopQO			, MF_RESULT ,  MC_None);
  DB.Double("S"				, "", &m_dTopS			, MF_RESULT ,  MC_None);
  DB.Double("T"				, "", &m_dTopT			, MF_RESULT ,  MC_None);
  DB.Double("AF"			, "", &m_dTopAF			, MF_RESULT ,  MC_None);
  DB.Double("D5"			, "", &m_dTopD50		, MF_RESULT ,  MC_None);
  DB.Double("A"				, "", &m_dTopA			, MF_RESULT ,  MC_None);
  DB.Double("B"				, "", &m_dTopB			, MF_RESULT ,  MC_None);
  DB.Double("C"				, "", &m_dTopC			, MF_RESULT ,  MC_None);
  DB.Double("D"				, "", &m_dTopD			, MF_RESULT ,  MC_None);
  DB.Double("E"				, "", &m_dTopE			, MF_RESULT ,  MC_None);
  DB.Double("F"				, "", &m_dTopF			, MF_RESULT ,  MC_None);
  DB.Double("G"				, "", &m_dTopG			, MF_RESULT ,  MC_None);
  DB.Double("H"				, "", &m_dTopH			, MF_RESULT ,  MC_None);
  DB.Double("J"				, "", &m_dTopJ			, MF_RESULT ,  MC_None);
  DB.Double("K"				, "", &m_dTopK			, MF_RESULT ,  MC_None);
  DB.Double("L"				, "", &m_dTopL			, MF_RESULT ,  MC_None);
  DB.Double("X"				, "", &m_dTopX			, MF_RESULT ,  MC_None);
  DB.Double("V"				, "", &m_dTopV			, MF_RESULT ,  MC_None);
  DB.StructEnd();

  DB.Text("");
  DB.StructBegin("BottomRes"  );
  DB.Double("QF"			, "", &m_dBottomQF		, MF_RESULT ,  MC_None);
  DB.Double("Feed_OS"	, "", &m_dBottomFeed_OS, MF_RESULT ,  MC_None);
  DB.Double("Feed_US"	, "", &m_dBottomFeed_US, MF_RESULT ,  MC_None);
  DB.Double("Feed_HSperture"	, "", &m_dBottomFeed_HS		, MF_RESULT ,  MC_None);
  DB.Double("QU"			, "", &m_dBottomQU		, MF_RESULT ,  MC_None);
  DB.Double("QO"			, "", &m_dBottomQO		, MF_RESULT ,  MC_None);
  DB.Double("S"				, "", &m_dBottomS			, MF_RESULT ,  MC_None);
  DB.Double("T"				, "", &m_dBottomT			, MF_RESULT ,  MC_None);
  DB.Double("AF"			, "", &m_dBottomAF		, MF_RESULT ,  MC_None);
  DB.Double("D5"			, "", &m_dBottomD50		, MF_RESULT ,  MC_None);
  DB.Double("A"				, "", &m_dBottomA			, MF_RESULT ,  MC_None);
  DB.Double("B"				, "", &m_dBottomB			, MF_RESULT ,  MC_None);
  DB.Double("C"				, "", &m_dBottomC			, MF_RESULT ,  MC_None);
  DB.Double("D"				, "", &m_dBottomD			, MF_RESULT ,  MC_None);
  DB.Double("E"				, "", &m_dBottomE			, MF_RESULT ,  MC_None);
  DB.Double("F"				, "", &m_dBottomF			, MF_RESULT ,  MC_None);
  DB.Double("G"				, "", &m_dBottomG			, MF_RESULT ,  MC_None);
  DB.Double("H"				, "", &m_dBottomH			, MF_RESULT ,  MC_None);
  DB.Double("J"				, "", &m_dBottomJ			, MF_RESULT ,  MC_None);
  DB.Double("K"				, "", &m_dBottomK			, MF_RESULT ,  MC_None);
  DB.Double("L"				, "", &m_dBottomL			, MF_RESULT ,  MC_None);
  DB.Double("X"				, "", &m_dBottomX			, MF_RESULT ,  MC_None);
  DB.Double("V"				, "", &m_dBottomV			, MF_RESULT ,  MC_None);
  DB.StructEnd();

  DB.ObjectEnd();
	}

//====================================================================================

void CSeperator_LoadBased::EvalProducts(MStream &Feed ,
               MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit)
	
  { 
  // Get info on Size Distribution
  MIPSD & l_PSD=*Feed.FindIF<MIPSD>();
  if (IsNothing(l_PSD))
    {
    return; //Why do we get here!!!
    }
  long    l_SizeCount      = l_PSD.getSizeCount();
  long    l_PSDVectorCount = l_PSD.getPSDVectorCount();

  if ( bInit && !IsNothing(l_PSD))
    {
    bInit = false;

    // Copy the Sieve Data Sizing Info
    SysCADSystemHelper::SysCADSizeDataToSystem(l_PSD, MatInfo);

    // Copy Material Information from database
    // For now we just set up n components
    // The MineralInfo data will come from a database
    MatInfo->SetNumberOfMinerals( l_PSD.PSDVectorCount );

    // Initialize Top Deck
    m_TopDeckParams[0] =  m_dTopApperture*1000.0;
    m_TopDeckParams[1] =  m_dTopLength;
    m_TopDeckParams[2] =  m_dTopWidth;
    m_TopDeckParams[3] =  m_dTopAngle*3.14/9.0;
    m_TopDeckParams[4] =  m_dTopOpenFraction;
    m_TopDeckParams[5] =  m_dTopBulkDensity/1000.0;
    m_TopDeckParams[6] =  m_bTopWetScreening;
    m_TopDeckParams[7] =  m_lTopAppetureShape;
    m_TopDeckParams[8] =  m_lTopMediaType;
    m_TopDeckParams[9] =  m_lTopDeckLocation;
    m_TopDeckParams[10] =  m_bTopGravel;
    m_TopDeckParams[11] =  m_dTopCustomFactor;
    m_TopDeckParams[12] =  m_dTopWaterSplitToUS;
    RioTintoTS::VectorView ParamVec(m_TopDeckParams,13,1);
    if (TopDeckScreen.Initialize(MatInfo, ParamVec) == false)
      {
      }

    if ( bTwoDecks )
      {
      // TO DO
      m_BottomDeckParams[0]  =  m_dBottomApperture*1000.0; // m to mm
      m_BottomDeckParams[1]  =  m_dBottomLength;
      m_BottomDeckParams[2]  =  m_dBottomWidth;
      m_BottomDeckParams[3]  =  m_dBottomAngle*3.14/9.0; // rads to deg
      m_BottomDeckParams[4]  =  m_dBottomOpenFraction;
      m_BottomDeckParams[5]  =  m_dBottomBulkDensity/1000.0; // kg to t
      m_BottomDeckParams[6]  =  m_bBottomWetScreening;
      m_BottomDeckParams[7]  =  m_lBottomAppetureShape;
      m_BottomDeckParams[8]  =  m_lBottomMediaType;
      m_BottomDeckParams[9]  =  m_lBottomDeckLocation;
      m_BottomDeckParams[10] =  m_bBottomGravel;
      m_BottomDeckParams[11] =  m_dBottomCustomFactor;
      m_BottomDeckParams[12] =  m_dBottomWaterSplitToUS;
      RioTintoTS::VectorView ParamVec(m_BottomDeckParams,13,1);
      if ( BottomDeckScreen.Initialize(MatInfo, ParamVec) == false )
        {
        }
      }

    TopDeckScreen.InitializeRegr(m_A.Val(), m_B.Val(), m_C.Val(), m_E.Val(), m_Td.Val(), m_Tw.Val());
    if ( bTwoDecks )
      {
      BottomDeckScreen.InitializeRegr(m_A.Val(), m_B.Val(), m_C.Val(), m_E.Val(), m_Td.Val(), m_Tw.Val());
      }

    if ( !bTwoDecks )
      {//clear the bottom deck results
	    m_dBottomQF=0.0;		//  total tph of feed
	    m_dBottomFeed_OS=0.0;	//  fraction of feed > apperture
	    m_dBottomFeed_US=0.0;	//  fraction of feed < apperture
	    m_dBottomFeed_HS=0.0;	//  fraction of feed < half-apperture
	    m_dBottomQU=0.0;		//  flowrate of undersize stream
	    m_dBottomQO=0.0;		//  flowrate of oversize stream
	    m_dBottomS=0.0;		//  apperture size
	    m_dBottomT=0.0;		//  screen efficiency (at S)
	    m_dBottomAF=0.0;		//  effective screen area
	    m_dBottomD50=0.0;		//  fitted d50 of separation
	    m_dBottomA=0.0;	    //  basic capacity factor
	    m_dBottomB=0.0;		//  over-size factor
	    m_dBottomC=0.0;		//  half-size factor
	    m_dBottomD=0.0;		//  deck location factor
	    m_dBottomE=0.0;		//  wet screening factor
	    m_dBottomF=0.0;		//  feed bulk density factor
	    m_dBottomG=0.0;		//  load vs efficiency factor
	    m_dBottomH=0.0;		//  opening shape factor
	    m_dBottomJ=0.0;		//  screen open area factor
	    m_dBottomK=0.0;		//  rock/gravel factor
	    m_dBottomL=0.0;		//  humidity factor
	    m_dBottomX=0.0;		//  user custom factor
	    m_dBottomV=0.0;		//  screen load %
      }

    // Create the Feed Stream. Use SetConfig after initialisation
    // if we need to change anything. We need to initialise here
    // because we get the size information from the Feed stream
    FeedStream = RioTintoTS::FlowStream1::Create( MatInfo  );
    }


  if (!IsNothing(l_PSD))
    {
    // Copy the input size data to the system feed stream solids
    SysCADSystemHelper::SysCADSolidsToSystem(Feed,FeedStream);
    SysCADSystemHelper::SysCADLiquidToSystem(Feed,FeedStream);

    //set some parameters every iteration:
    TopDeckScreen.SetWaterSplitToUS(m_dTopWaterSplitToUS);
    TopDeckScreen.SetApperture(m_dTopApperture*1000.0);
    if ( bTwoDecks )
      {
      BottomDeckScreen.SetWaterSplitToUS(m_dBottomWaterSplitToUS);
      BottomDeckScreen.SetApperture(m_dBottomApperture*1000.0);
      }

    // Execute the Top Deck Model
    TopDeckScreen.CalculateModel( FeedStream );

    if ( bTwoDecks )
      {
      // Execute the Top Deck Model
      BottomDeckScreen.CalculateModel( TopDeckScreen.Undersize );
      }

    // Copy the output size data to the SysCAD output streams
    if (!bTwoDecks)
      {
      SysCADSystemHelper::SystemSolidsToSysCAD(Product1,TopDeckScreen.Oversize);
      SysCADSystemHelper::SystemSolidsToSysCAD(Product3,TopDeckScreen.Undersize);
      SysCADSystemHelper::SystemLiquidToSysCAD(Product1,TopDeckScreen.Oversize);
      SysCADSystemHelper::SystemLiquidToSysCAD(Product3,TopDeckScreen.Undersize);
      }
    else
      {
      SysCADSystemHelper::SystemSolidsToSysCAD(Product1,TopDeckScreen.Oversize);
      SysCADSystemHelper::SystemSolidsToSysCAD(Product2,BottomDeckScreen.Oversize);
      SysCADSystemHelper::SystemSolidsToSysCAD(Product3,BottomDeckScreen.Undersize);
      SysCADSystemHelper::SystemLiquidToSysCAD(Product1,TopDeckScreen.Oversize);
      SysCADSystemHelper::SystemLiquidToSysCAD(Product2,BottomDeckScreen.Oversize);
      SysCADSystemHelper::SystemLiquidToSysCAD(Product3,BottomDeckScreen.Undersize);
      }

    // Copy outputs
    m_dTopQF		= TopDeckScreen.ModelOutput[0];
    m_dTopFeed_OS	= TopDeckScreen.ModelOutput[1];
    m_dTopFeed_US	= TopDeckScreen.ModelOutput[2];
    m_dTopFeed_HS	= TopDeckScreen.ModelOutput[3];
    m_dTopQU		= TopDeckScreen.ModelOutput[4];
    m_dTopQO		= TopDeckScreen.ModelOutput[5];
    m_dTopS			= TopDeckScreen.ModelOutput[6];
    m_dTopT			= TopDeckScreen.ModelOutput[7];
    m_dTopAF		= TopDeckScreen.ModelOutput[8];
    m_dTopD50		= TopDeckScreen.ModelOutput[9];
    m_dTopA			= TopDeckScreen.ModelOutput[10];
    m_dTopB			= TopDeckScreen.ModelOutput[11];
    m_dTopC			= TopDeckScreen.ModelOutput[12];
    m_dTopD			= TopDeckScreen.ModelOutput[13];
    m_dTopE			= TopDeckScreen.ModelOutput[14];
    m_dTopF			= TopDeckScreen.ModelOutput[15];
    m_dTopG			= TopDeckScreen.ModelOutput[16];
    m_dTopH			= TopDeckScreen.ModelOutput[17];
    m_dTopJ			= TopDeckScreen.ModelOutput[18];
    m_dTopK			= TopDeckScreen.ModelOutput[19];
    m_dTopL			= TopDeckScreen.ModelOutput[20];
    m_dTopX			= TopDeckScreen.ModelOutput[21];
    m_dTopV			= TopDeckScreen.ModelOutput[22];

    if ( bTwoDecks )
      {
      m_dBottomQF			= BottomDeckScreen.ModelOutput[0];
      m_dBottomFeed_OS	= BottomDeckScreen.ModelOutput[1];
      m_dBottomFeed_US	= BottomDeckScreen.ModelOutput[2];
      m_dBottomFeed_HS	= BottomDeckScreen.ModelOutput[3];
      m_dBottomQU			= BottomDeckScreen.ModelOutput[4];
      m_dBottomQO			= BottomDeckScreen.ModelOutput[5];
      m_dBottomS			= BottomDeckScreen.ModelOutput[6];
      m_dBottomT			= BottomDeckScreen.ModelOutput[7];
      m_dBottomAF			= BottomDeckScreen.ModelOutput[8];
      m_dBottomD50		= BottomDeckScreen.ModelOutput[9];
      m_dBottomA			= BottomDeckScreen.ModelOutput[10];
      m_dBottomB			= BottomDeckScreen.ModelOutput[11];
      m_dBottomC			= BottomDeckScreen.ModelOutput[12];
      m_dBottomD			= BottomDeckScreen.ModelOutput[13];
      m_dBottomE			= BottomDeckScreen.ModelOutput[14];
      m_dBottomF			= BottomDeckScreen.ModelOutput[15];
      m_dBottomG			= BottomDeckScreen.ModelOutput[16];
      m_dBottomH			= BottomDeckScreen.ModelOutput[17];
      m_dBottomJ			= BottomDeckScreen.ModelOutput[18];
      m_dBottomK			= BottomDeckScreen.ModelOutput[19];
      m_dBottomL			= BottomDeckScreen.ModelOutput[20];
      m_dBottomX			= BottomDeckScreen.ModelOutput[21];
      m_dBottomV			= BottomDeckScreen.ModelOutput[22];
      }
    }
  }

//====================================================================================
