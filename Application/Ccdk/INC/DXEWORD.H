//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXEWORD.H ***********************************/
#ifndef     DXEWORD
#define     DXEWORD 
EXTERN  char    DXF_LINE_WORD[] ;
EXTERN  char    DXF_POINT_WORD[] ;
EXTERN  char    DXF_CIRCLE_WORD[] ;
EXTERN  char    DXF_ARC_WORD[] ;
EXTERN  char    DXF_TRACE_WORD[] ; 
EXTERN  char    DXF_SOLID_WORD[] ;
EXTERN  char    DXF_TEXT_WORD[] ;
EXTERN  char    DXF_SHAPE_WORD[] ;
EXTERN  char    DXF_INSERT_WORD[] ;
EXTERN  char    DXF_ATTDEF_WORD[] ;
EXTERN  char    DXF_ATTRIB_WORD[] ;
EXTERN  char    DXF_VERTEX_WORD[] ; 
EXTERN  char    DXF_PLINE_WORD[]  ; 
EXTERN  char    DXF_LINE3D_WORD[] ; 
EXTERN  char    DXF_3DFACE_WORD[] ; 
EXTERN  char    DXF_DIM_WORD[] ; 
EXTERN  char    DXF_NULL_WORD[] ;
#ifdef __ACAD11__
EXTERN  char    DXF_VIEWPORT_WORD[] ;
#endif
#define     DXF_FACE3D_WORD     DXF_3DFACE_WORD 

#define DXF_LINE_NAME       DXF_LINE_WORD 
#define DXF_POINT_NAME      DXF_POINT_WORD 
#define DXF_CIRCLE_NAME     DXF_CIRCLE_WORD 
#define DXF_ARC_NAME        DXF_ARC_WORD 
#define DXF_TRACE_NAME      DXF_TRACE_WORD  
#define DXF_SOLID_NAME      DXF_SOLID_WORD 
#define DXF_TEXT_NAME       DXF_TEXT_WORD 
#define DXF_INSERT_NAME     DXF_INSERT_WORD 
#define DXF_ATTDEF_NAME     DXF_ATTDEF_WORD 
#define DXF_ATTRIB_NAME     DXF_ATTRIB_WORD 
#define DXF_VERTEX_NAME     DXF_VERTEX_WORD  
#define DXF_PLINE_NAME      DXF_PLINE_WORD
#define DXF_LINE3D_NAME     DXF_LINE3D_WORD  
#define DXF_3DFACE_NAME     DXF_3DFACE_WORD  
#define DXF_DIMENSION_NAME  DXF_DIMENSION_WORD  
#define DXF_DIMENSION_WORD  DXF_DIM_WORD  
#define DXF_NULL_NAME       DXF_NULL_WORD 
#endif
