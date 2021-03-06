//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** ALLDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef  SPLINE
#ifndef ALLDEFS
#define ALLDEFS
#include <aldefs.h>

extern  BOOLEAN all_3diag __(( REAL* DUMMY0 , REAL* DUMMY1 , REAL* DUMMY2 ,
            REAL* DUMMY3 , INT DUMMY4 , INT DUMMY5 , REAL* DUMMY6 )) ;
extern  BOOLEAN all_3diag_0 __(( REAL* DUMMY0 , REAL* DUMMY1 , REAL* DUMMY2 ,
            INT DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 )) ;
extern  void    all_3diag_1 __(( REAL* DUMMY0 , REAL* DUMMY1 , REAL* DUMMY2 ,
            REAL* DUMMY3 , INT DUMMY4 , INT DUMMY5 , REAL* DUMMY6 )) ;
extern  BOOLEAN all_3diag_per __(( REAL* DUMMY0 , REAL* DUMMY1 , REAL* DUMMY2 ,
            REAL* DUMMY3 , INT DUMMY4 , INT DUMMY5 , REAL* DUMMY6 )) ;
extern  BOOLEAN all_3diag_per_0 __(( REAL* DUMMY0 , REAL* DUMMY1 ,
            REAL* DUMMY2 , INT DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 ,
             REAL* DUMMY6 , REAL* DUMMY7 )) ;
extern  BOOLEAN all_3diag_per_1 __(( REAL* DUMMY0 , REAL* DUMMY1 ,
            REAL* DUMMY2 , REAL* DUMMY3 ,  REAL* DUMMY4 , REAL* DUMMY5 ,
            INT DUMMY6 , INT DUMMY7 , REAL* DUMMY8 )) ;
extern  BOOLEAN all_3diag_per_2 __(( REAL* DUMMY0 , REAL* DUMMY1 ,
            REAL* DUMMY2 , REAL DUMMY3 ,  REAL DUMMY4 , REAL* DUMMY5 ,
            INT DUMMY6 , INT DUMMY7 , REAL* DUMMY8 )) ;
extern  BOOLEAN all_lin_sys __(( REAL* DUMMY0 , REAL* DUMMY1 , INT DUMMY2 ,
            INT DUMMY3 , REAL* DUMMY4 )) ;
extern  void    all_product_mtx __(( REAL* DUMMY0 , REAL* DUMMY1 ,
            INT DUMMY2 , INT DUMMY3 , INT DUMMY4 , REAL* DUMMY5 )) ;
#endif /* ALLDEFS */
#endif /* SPLINE */

