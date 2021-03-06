//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXDMSM.H ***********************************/ 
#ifndef     DXDMSM 
#define     DXDMSM 
#ifdef __ACAD11__
#include    "tststr.h"


#define     DXF_DIMSTYLE_NAME_PTR(E)        (&(DX_TE_DIMSTYLE(E)->dimstyle_name))
#define     DXF_DIMSTYLE_DIMPOST_PTR(E)     (&(DX_TE_DIMSTYLE(E)->dimstyle_dimpost))
#define     DXF_DIMSTYLE_DIMAPOST_PTR(E)    (&(DX_TE_DIMSTYLE(E)->dimstyle_dimapost))
#define     DXF_DIMSTYLE_DIMBLK1_PTR(E)     (&(DX_TE_DIMSTYLE(E)->dimstyle_dimblk1))
#define     DXF_DIMSTYLE_DIMBLK2_PTR(E)     (&(DX_TE_DIMSTYLE(E)->dimstyle_dimblk2))
#define     DXF_DIMSTYLE_DIMBLK_PTR(E)      (&(DX_TE_DIMSTYLE(E)->dimstyle_dimblk))

#define     DXF_DIMSTYLE_NAME_GET(E)        STRTEST(DX_TE_DIMSTYLE(E)->dimstyle_name.p)
#define     DXF_DIMSTYLE_DIMPOST_GET(E)     STRTEST(DX_TE_DIMSTYLE(E)->dimstyle_dimpost.p)
#define     DXF_DIMSTYLE_DIMAPOST_GET(E)    STRTEST(DX_TE_DIMSTYLE(E)->dimstyle_dimapost.p)
#define     DXF_DIMSTYLE_DIMBLK1_GET(E)     STRTEST(DX_TE_DIMSTYLE(E)->dimstyle_dimblk1.p)
#define     DXF_DIMSTYLE_DIMBLK2_GET(E)     STRTEST(DX_TE_DIMSTYLE(E)->dimstyle_dimblk2.p)
#define     DXF_DIMSTYLE_DIMBLK_GET(E)      STRTEST(DX_TE_DIMSTYLE(E)->dimstyle_dimblk.p)

#define     DXF_DIMSTYLE_NAME_SET(E,value)        (dx0_strass(&DX_TE_DIMSTYLE(E)->dimstyle_name,value))
#define     DXF_DIMSTYLE_DIMPOST_SET(E,value)     (dx0_strass(&DX_TE_DIMSTYLE(E)->dimstyle_dimpost,value))
#define     DXF_DIMSTYLE_DIMAPOST_SET(E,value)    (dx0_strass(&DX_TE_DIMSTYLE(E)->dimstyle_dimapost,value))
#define     DXF_DIMSTYLE_DIMBLK1_SET(E,value)     (dx0_strass(&DX_TE_DIMSTYLE(E)->dimstyle_dimblk1,value))
#define     DXF_DIMSTYLE_DIMBLK2_SET(E,value)     (dx0_strass(&DX_TE_DIMSTYLE(E)->dimstyle_dimblk2,value))
#define     DXF_DIMSTYLE_DIMBLK_SET(E,value)      (dx0_strass(&DX_TE_DIMSTYLE(E)->dimstyle_dimblk,value))


//#define     DXF_DIMSTYLE_NAME(E)        (DX_TE_DIMSTYLE(E)->dimstyle_name)
//#define     DXF_DIMSTYLE_DIMPOST(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimpost)
//#define     DXF_DIMSTYLE_DIMAPOST(E)    (DX_TE_DIMSTYLE(E)->dimstyle_dimapost)
//#define     DXF_DIMSTYLE_DIMBLK1(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimblk1)
//#define     DXF_DIMSTYLE_DIMBLK2(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimblk2)
//#define     DXF_DIMSTYLE_DIMBLK(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimblk)

#define     DXF_DIMSTYLE_DIMSCALE(E)    (DX_TE_DIMSTYLE(E)->dimstyle_dimscale)
#define     DXF_DIMSTYLE_DIMASZ(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimasz)
#define     DXF_DIMSTYLE_DIMEXO(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimexo)
#define     DXF_DIMSTYLE_DIMDLI(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimdli)
#define     DXF_DIMSTYLE_DIMEXE(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimexe)
#define     DXF_DIMSTYLE_DIMRND(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimrnd)
#define     DXF_DIMSTYLE_DIMDLE(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimdle)
#define     DXF_DIMSTYLE_DIMTP(E)       (DX_TE_DIMSTYLE(E)->dimstyle_dimtp)
#define     DXF_DIMSTYLE_DIMTM(E)       (DX_TE_DIMSTYLE(E)->dimstyle_dimtm)
#define     DXF_DIMSTYLE_DIMCLRD(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimclrd)
#define     DXF_DIMSTYLE_DIMCLRT(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimclrt)
#define     DXF_DIMSTYLE_DIMTOL(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtol)
#define     DXF_DIMSTYLE_DIMLIM(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimlim)
#define     DXF_DIMSTYLE_DIMTIH(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtih)
#define     DXF_DIMSTYLE_DIMTOH(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtoh)
#define     DXF_DIMSTYLE_DIMSE1(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimse1)
#define     DXF_DIMSTYLE_DIMSE2(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimse2)
#define     DXF_DIMSTYLE_DIMTAD(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtad)
#define     DXF_DIMSTYLE_DIMZIN(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimzin)
#define     DXF_DIMSTYLE_DIMTXT(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtxt)
#define     DXF_DIMSTYLE_DIMCEN(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimcen)
#define     DXF_DIMSTYLE_DIMTSZ(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtsz)
#define     DXF_DIMSTYLE_DIMALTF(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimaltf)
#define     DXF_DIMSTYLE_DIMLFAC(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimlfac)
#define     DXF_DIMSTYLE_DIMTVP(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtvp)
#define     DXF_DIMSTYLE_DIMALT(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimalt)
#define     DXF_DIMSTYLE_DIMALTD(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimaltd)
#define     DXF_DIMSTYLE_DIMTOFL(E)     (DX_TE_DIMSTYLE(E)->dimstyle_dimtofl)
#define     DXF_DIMSTYLE_DIMSAH(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimsah)
#define     DXF_DIMSTYLE_DIMTIX(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimtix)
#define     DXF_DIMSTYLE_DIMOXD(E)      (DX_TE_DIMSTYLE(E)->dimstyle_dimoxd)
#endif
#endif

