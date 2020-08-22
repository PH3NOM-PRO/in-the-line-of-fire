!void _glKosCopyDoublePrecision( void *src, void *dst, int stride, int count );
.globl __glKosCopyDoublePrecision
!r4 = [arg] = src
!r5 = [arg] = dst
!r6 = [arg] = stride
!r7 = [arg] = count

__glKosCopyDoublePrecision:
    fschg

.COPYDOUBLEPRECISION:
    fmov @r4+, dr14
    fmov dr14, @r5
    add r6, r5

    dt r7
    bf .COPYDOUBLEPRECISION

    rts
    fschg

!void _glKosCopyVertexPVR( void *src, void *dst );
! Copy a pvr_vertex_t
.globl __glKosCopyVertexPVR
!r4 = [arg] = src
!r5 = [arg] = dst
! Number of cycles: ~11 cycles.
__glKosCopyVertexPVR:
    add #32, r5

    fmov @r4+, dr8   ! read flags, x
    fmov @r4+, dr10  ! read y, z
    fmov @r4+, dr12  ! read u, v
    fmov @r4, dr14   ! read argb, oargb

    fmov dr14, @-r5  ! write argb, oargb
    fmov dr12, @-r5  ! write u, v
    fmov dr10, @-r5  ! write y, z
    fmov dr8, @-r5   ! write argb, oargb

    rts
    nop

!void _glKosCopyVertexUV( void *src, void *dst );
! Copy a u,v set
.globl __glKosCopyVertexUV
!r4 = [arg] = src
!r5 = [arg] = dst
! Number of cycles: ~4 cycles.
__glKosCopyVertexUV:
    fmov @r4, dr14  ! read u, v
    fmov dr14, @r5  ! write u, v

    rts
    nop

!void _glKosCopyVertexPVRUV( void *src, void *dst, void *uvsrc, void *uvdst );
! Copy a pvr_vertex_t and a multi-texture u/v 
.globl __glKosCopyVertexPVRUV
!r4 = [arg] = src
!r5 = [arg] = dst
!r6 = [arg] = uvsrc
!r7 = [arg] = uvdst
! Number of cycles: ~13 cycles.
__glKosCopyVertexPVRUV:
    add #32, r5

    fmov @r4+, dr8   ! read flags, x
    fmov @r4+, dr10  ! read y, z
    fmov @r4+, dr12  ! read u, v
    fmov @r4, dr14   ! read argb, oargb

    fmov @r6, dr6  ! read multi u, v
    fmov dr6, @r7  ! write multi u, v

    fmov dr14, @-r5  ! write argb, oargb
    fmov dr12, @-r5  ! write u, v
    fmov dr10, @-r5  ! write y, z
    fmov dr8, @-r5   ! write argb, oargb

    rts
    nop

.globl __glKosFSCHG

__glKosFSCHG:
    rts
    fschg