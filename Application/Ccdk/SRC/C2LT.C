//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/********************************** C2LT.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ldefs.h>
#include <c2tdefs.h>
#include <c2vdefs.h>
#include <c2lmcrs.h>
#include <c2vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2l_translate ( line0, shift, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line0 ;
PT2 shift ;
C2_LINE line1 ;
{
    C2V_ADD ( C2_LINE_PT0(line0), shift, C2_LINE_PT0(line1) ) ;
    C2V_ADD ( C2_LINE_PT1(line0), shift, C2_LINE_PT1(line1) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2l_scale ( line0, pt, factor, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line0 ;
PT2 pt ;
REAL factor ;
C2_LINE line1 ;
{
    C2V_ADDT ( pt, C2_LINE_PT0(line0), factor, C2_LINE_PT0(line1) ) ;
    C2V_ADDT ( pt, C2_LINE_PT1(line0), factor, C2_LINE_PT1(line1) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2l_mirror ( line0, origin, normal, line1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line0 ;
PT2 origin, normal ;
C2_LINE line1 ;
{
    c2v_mirror_pt ( C2_LINE_PT0(line0), origin, normal, C2_LINE_PT0(line1) ) ;
    c2v_mirror_pt ( C2_LINE_PT1(line0), origin, normal, C2_LINE_PT1(line1) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2l_rotate_cs ( line0, origin, c, s, line1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line0, line1 ;
PT2 origin ;
REAL c, s ;
{
    c2v_rotate_pt_cs ( C2_LINE_PT0(line0), origin, c, s, C2_LINE_PT0(line1) ) ;
    c2v_rotate_pt_cs ( C2_LINE_PT1(line0), origin, c, s, C2_LINE_PT1(line1) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2l_transform ( line0, t, line1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line0, line1 ;
C2_TRANSFORM t ;
{
    c2t_eval_pt ( C2_LINE_PT0(line0), t, C2_LINE_PT0(line1) ) ;
    c2t_eval_pt ( C2_LINE_PT1(line0), t, C2_LINE_PT1(line1) ) ;
}

