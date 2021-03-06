//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3V.C **********************************/
/*************************** 3d vector geometry ****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3vdefs.h>
#include <c2vmcrs.h>
#include <c3vmcrs.h>
#include <c3vpriv.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_set_zero(a) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
{
    a[0] = 0.0 ;
    a[1] = 0.0 ;
    a[2] = 0.0 ;
    RETURN ( a ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_set(x,y,z,a) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z ;
PT3 a ;
{
    a[0] = x ;
    a[1] = y ;
    a[2] = z ;
    RETURN ( a ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_copy(a,b)
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    b[0] = a[0] ;
    b[1] = a[1] ;
    b[2] = a[2] ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3v_unit_normal ( a, b, c )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
{
    C3V_CROSS ( a, b, c ) ;
    RETURN ( c3v_normalize ( c, c ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3v_normalize ( a, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    REAL norm ;
    norm = C3V_NORML1 ( a ) ;
    if ( norm <= BBS_ZERO ) 
        RETURN ( FALSE ) ;
    b[0] = a[0] / norm ;
    b[1] = a[1] / norm ;
    b[2] = a[2] / norm ;
    norm = C3V_NORM ( b ) ;
    b[0] = b[0] / norm ;
    b[1] = b[1] / norm ;
    b[2] = b[2] / norm ;
    RETURN ( TRUE ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3v_normalize_l1 ( a, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    REAL norm ;
    norm = C3V_NORML1 ( a ) ;
    if ( norm <= BBS_TOL ) 
        RETURN ( FALSE ) ;
    b[0] = a[0] / norm ;
    b[1] = a[1] / norm ;
    b[2] = a[2] / norm ;
    RETURN ( TRUE ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_vecs_parallel ( a, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    REAL eps ;

    eps = BBS_ZERO * C3V_NORML1(a) * C3V_NORML1(b) ;
    RETURN ( fabs ( a[0] * b[1] - a[1] * b[0] ) <= eps && 
             fabs ( a[1] * b[2] - a[2] * b[1] ) <= eps && 
             fabs ( a[2] * b[0] - a[0] * b[2] ) <= eps ) ;
}                                                         


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL    c3v_vecs_sin ( a, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    PT3 c ;
    C3V_CROSS ( a, b, c ) ;
    RETURN ( C3V_NORM ( c ) / ( C3V_NORM ( a ) * C3V_NORM ( b ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL    c3v_vecs_cos ( a, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    RETURN ( C3V_DOT ( a, b ) / ( C3V_NORM ( a ) * C3V_NORM ( b ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL    c3v_vecs_angle ( a, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    RETURN ( qgl_atan2 ( c3v_vecs_sin ( a, b ) , c3v_vecs_cos ( a, b ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL    c3v_vec_angle_z ( b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 b ;
{
    RETURN ( acos ( b[2] / C3V_NORM ( b ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL    c3v_vec_angle_xy ( b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 b ;
{
    RETURN ( C2V_IS_SMALL(b) ? 0.0 : qgl_atan2 ( b[1], b[0] ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BBS_PUBLIC void c3v_spherical ( a, r_ptr, theta_ptr, phi_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
REAL *r_ptr, *theta_ptr, *phi_ptr ;
{
    if ( r_ptr != NULL ) 
        *r_ptr = C3V_NORM ( a ) ;
    if ( theta_ptr != NULL ) 
        *theta_ptr = c3v_vec_angle_xy ( a ) ;
    if ( phi_ptr != NULL ) 
        *phi_ptr = c3v_vec_angle_z ( a ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BBS_PUBLIC REAL *c3v_set_spherical ( r, theta, phi, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
REAL r, theta, phi ;
{
    a[2] = cos ( phi ) ;
    r *= sin ( phi ) ;
    a[0] = r * cos ( theta ) ;
    a[1] = r * sin ( theta ) ;
    RETURN ( a ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL c3v_project_line ( a, b, c, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c, p ;
{
    PT3 u, v ;
    REAL t ;
    C3V_SUB ( b, a, u ) ;
    C3V_SUB ( c, a, v ) ;
    t = C3V_DOT ( u, v ) / C3V_DOT ( u, u ) ;
    if ( p!= NULL ) 
        C3V_ADDT ( a, u, t, p ) ;
    RETURN ( t ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*   c3v_project_pt_normal ( a, n, d, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, n, b ;
REAL d ;
{
    REAL t ;

    t = d - C3V_DOT ( a, n ) ;
    C3V_ADDT ( a, n, t, b ) ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_project_pt_oblique ( a, normal, dist, direction, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, normal, direction, b ;
REAL dist ;
{
    REAL dot, t ;

    dot = C3V_DOT ( direction, normal ) ;
    if ( IS_SMALL(dot) ) 
        RETURN ( FALSE ) ;
    t = ( dist - C3V_DOT ( a, normal ) ) / dot ;
    C3V_ADDT ( a, direction, t, b ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*   c3v_project_vec_normal ( a, n, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, n, b ;
{
    REAL t ;

    t = - C3V_DOT ( a, n ) ;
    C3V_ADDT ( a, n, t, b ) ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_project_vec_oblique ( a, normal, direction, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, normal, direction, b ;
{
    REAL dot, t ;

    dot = C3V_DOT ( direction, normal ) ;
    if ( IS_SMALL(dot) ) 
        RETURN ( FALSE ) ;
    t = - C3V_DOT ( a, normal ) / dot ;
    C3V_ADDT ( a, direction, t, b ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*   c3v_mirror_pt ( a, n, d, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, n, b ;
REAL d ;
{
    REAL t ;

    t = 2.0 * ( d - C3V_DOT ( a, n ) ) ;
    C3V_ADDT ( a, n, t, b ) ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*   c3v_mirror_vec ( a, n, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, n, b ;
{
    REAL t ;

    t = - 2.0 * C3V_DOT ( a, n ) ;
    C3V_ADDT ( a, n, t, b ) ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_basis ( n, u, v ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 n, u, v ;
{
    REAL t ;
    if ( !c3v_normalize ( n, n ) ) 
        RETURN ( FALSE ) ;
    t = n[0] / ( 1.0 + fabs(n[2]) ) ;
    u[0] = 1.0 - n[0] * t ;
    u[1] = - n[1] * t ;
    u[2] = n[2] >= 0.0 ? -n[0] : n[0] ; 
    if ( v != NULL ) 
        C3V_CROSS ( n, u, v ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_basis_orthogonal ( u, v ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 u, v ;
{
    REAL nu, nv, tol ;
    nu = C3V_DOT ( u, u ) ;
    nv = C3V_DOT ( v, v ) ;
    tol = BBS_ZERO * ( nu + nv ) ;
    RETURN ( fabs(nu-nv) <= tol && fabs ( C3V_DOT ( u, v ) ) <= tol ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_coord_sys ( normal, dist, origin, x_axis, y_axis ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 normal, origin, x_axis, y_axis ;
REAL dist ;
{
    REAL t ;
    if ( C3V_IS_ZERO ( normal ) )
        RETURN ( FALSE ) ;
    t = dist / C3V_DOT ( normal, normal ) ;
    C3V_SCALE ( normal, t, origin ) ;
    RETURN ( c3v_basis ( normal, x_axis, y_axis ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_rotate_pt ( a, origin, axis, angle, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, origin, axis, b ;
REAL angle ;
{
    REAL c, s ;
    c = cos ( angle ) ;
    s = sin ( angle ) ;
    c3v_rotate_pt_cs ( a, origin, axis, c, s, b ) ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_rotate_pt_cs ( a, origin, axis, c, s, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, origin, axis, b ;
REAL c, s ;
{
    PT3 n, a1, a2 ;
    REAL w ;

    if ( !c3v_normalize ( axis, n ) )
        RETURN ( NULL ) ;
    C3V_SUB ( a, origin, a1 ) ;
    C3V_CROSS ( n, a1, a2 ) ;
    w = C3V_DOT ( a1, n ) * ( 1.0 - c ) ;
    b[0] = origin[0] + c * a1[0] + w * n[0] + s * a2[0] ;
    b[1] = origin[1] + c * a1[1] + w * n[1] + s * a2[1] ;
    b[2] = origin[2] + c * a1[2] + w * n[2] + s * a2[2] ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_rotate_vec ( a, axis, angle, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, axis, b ;
REAL angle ;
{
    REAL c, s ;
    c = cos ( angle ) ;
    s = sin ( angle ) ;
    c3v_rotate_vec_cs ( a, axis, c, s, b ) ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_rotate_vec_cs ( a, axis, c, s, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, axis, b ;
REAL c, s ;
{
    PT3 n, q ;
    REAL w ;
    if ( !c3v_normalize ( axis, n ) )
        RETURN ( NULL ) ;
    C3V_CROSS ( n, a, q ) ; 
    w = C3V_DOT ( a, n ) * ( 1.0 - c ) ;
    b[0] = c * a[0] + w * n[0] + s * q[0] ;
    b[1] = c * a[1] + w * n[1] + s * q[1] ;
    b[2] = c * a[2] + w * n[2] + s * q[2] ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3v_coord ( a0, a1, b, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a0, a1, b, c ;
{
    REAL u ;
    u = c3v_project_line ( a0, a1, b, c ) ;
    C3V_SUB ( b, c, c ) ;
    RETURN ( u ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_plane_3pts ( p0, p1, p2, normal, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 p0, p1, p2, normal ;
REAL *dist_ptr ;
{
    PT3 v1, v2 ;
    C3V_SUB ( p1, p0, v1 ) ;
    C3V_SUB ( p2, p0, v2 ) ;
    if ( !c3v_unit_normal ( v1, v2, normal ) )
        RETURN ( FALSE ) ;
    *dist_ptr = C3V_DOT ( p0, normal ) ;
    if ( *dist_ptr < 0.0 ) {
        C3V_NEGATE ( normal, normal ) ;
        *dist_ptr = - *dist_ptr ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_pt_on_plane ( p, normal, dist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 p, normal ;
REAL dist ;
{
    RETURN ( IS_SMALL( C3V_DOT ( p, normal ) - dist ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3v_vec_on_plane ( v, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 v, normal ;
{
    RETURN ( fabs ( C3V_DOT ( v, normal ) ) <= C3V_NORML1 ( v ) * BBS_ZERO ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3v_plane_def ( a, p, normal, dist_ptr, deg_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, p[2], normal ;
REAL *dist_ptr ;
INT *deg_ptr ;
{
    if ( *deg_ptr == 0 ) {
        C3V_COPY ( a, p[0] ) ;
        *deg_ptr = 1 ;
    }
    else if ( *deg_ptr == 1 ) {
        if ( !C3V_IDENT_PTS ( a, p[0] ) ) {
            C3V_COPY ( a, p[1] ) ;
            *deg_ptr = 2 ;
        }
    }
    else if ( *deg_ptr == 2 ) {
        if ( c3v_plane_3pts ( p[0], p[1], a, normal, dist_ptr ) )
            *deg_ptr = 3 ;
    }
    else {
        if ( !c3v_pt_on_plane ( a, normal, *dist_ptr ) )
            (*deg_ptr)++ ;
    }
}

