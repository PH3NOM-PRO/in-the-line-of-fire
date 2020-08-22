!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!void DCE_RenderTransformTriangles(float *vert_pos,
!                            float vert_stride,
!                            float * uv_coord,
!                            float uv_stride,
!                            unsigned int * argb,
!                            float argb_stride,
!                            float count);

.globl _DCE_RenderTransformTriangles

!r0 = [int] = triangle count
!r1 = [int] = vert stride
!r2 = [int] = uv stride
!r3 = [int] = argb and pvr vertex flags
!r4 = [arg][float *] = vertex position pointer
!r5 = [arg][float *] = uv pointer
!r6 = [arg][uint32 *] = color (32bit int argb) pointer
!r7 = [arg][pvr_vertex_t *] = output vector
!r14 = [int] = argb stride

!fr4 = [arg][float] vert stride (in bytes)(passed as float to prevent stack use)
!fr5 = [arg][float] uv stride (in bytes)(passed as float to prevent stack use) 
!fr6 = [arg][float] argb stride
!fr7 = [arg][float] count
!fv0 = vector 1
!fv4 = vector 2
!fv8 = vector 3
!fr12 = u component
!fr13 = v component

.align 4

_DCE_RenderTransformTriangles:

    ftrc fr7, fpul       ! floatToInt(triangle count)
    sts fpul, r0         ! r0 now holds triangle count
    fmov.s fr12, @-r15   ! push fr12 to stack ( callee save ) 
    ftrc fr4, fpul       ! floatToInt(vert stride)
    sts fpul, r1         ! r1 now holds vert stride
    ftrc fr5, fpul       ! floatToInt(uv stride)
    sts fpul, r2         ! r2 now holds uv stride
    add #-8, r1          ! adjust vert stride to offset read increment
    fmov.s fr13, @-r15   ! push fr13 to stack ( callee save )
    add #-4, r2          ! adjust uv stride to offset read increment
    mov.l r14, @-r15     ! push r14 to stack ( callee save )
    mov.l TA_ADDR, r7    ! load PVR_CMD_VERTEX flag to r3
    
.LOADVERTEX:

    fmov @r4+, fr0       ! load vertex x
    fmov @r4+, fr1       ! load vertex y 
    fmov @r4, fr2        ! load vertex z
    fldi1 fr3            ! load 1 for w

    ftrv xmtrx, fv0      ! transform first vector
  
    add r1, r4           ! add vertex position stride
    fmov @r4+, fr4       ! load vertex x
    add #28, r7          ! move output vertex to argb component
    fldi1 fr7            ! load 1 for w
    fmov @r4+, fr5       ! load vertex y
    fmov @r4, fr6        ! load vertex z

    ftrv xmtrx, fv4      ! transform second vector

    add r1, r4           ! add vertex position stride
    fmov @r4+, fr8       ! load vertex x
    fldi1 fr11           ! load 1 for w
    fmov @r4+, fr9       ! load vertex y
    fmov @r4, fr10       ! load vertex z

    ftrv xmtrx, fv8      ! transform third vector

    add r1, r4           ! add vertex position stride

    fldi1 fr2
    fdiv fr3, fr2        ! perspective divide
    
    fmov @r5+, fr12      ! load u to fr12
    fmov @r5, fr13       ! load v to fr13
    add r2, r5           ! add uv stride
    
    mov.l r6, @-r7       ! write argb
    fmov fr13, @-r7      ! write v
    mov.l CMD_VERT, r3   ! load PVR_CMD_VERTEX flag to r3
    fmov fr12, @-r7      ! write u
    fmul fr2, fr1        ! 1 / w * y
    fmov fr2, @-r7       ! write z
    fmul fr2, fr0        ! 1 / w * x
    fmov fr1, @-r7       ! write y
    fldi1 fr6            ! load 1 to fr6 for next 1 / w op
    fmov fr0, @-r7       ! write x
    mov.l r3, @-r7       ! write first vertex flag

    pref @r7             ! flush vertex via SQ to PVR

    fdiv fr7, fr6        ! perspective divide

    add #60, r7          ! move output vertex to argb component
    fmov @r5+, fr12      ! load u to fr12
    fmov @r5, fr13       ! load v to fr13
    add r2, r5           ! add uv stride

    mov.l r6, @-r7       ! write argb
    fmov fr13, @-r7      ! write v
    mov.l CMD_VERT, r3   ! load PVR_CMD_VERTEX flag to r3
    fmul fr6, fr5        ! 1 / w * y
    fmov fr12, @-r7      ! write u
    fmul fr6, fr4        ! 1 / w * x
    fmov fr6, @-r7       ! write z
    fldi1 fr10           ! load 1 to fr10 for next 1 / w op
    fmov fr5, @-r7       ! write y
    fmov fr4, @-r7       ! write x 
    mov.l r3, @-r7       ! write second vertex flag

    pref @r7             ! flush vertex via SQ to PVR

    fdiv fr11, fr10      ! perspective divide

    add #60, r7          ! move output vertex to v component
    fmov @r5+, fr12      ! load u to fr12
    fmov @r5, fr13       ! load v to fr13
    add r2, r5           ! add uv stride

    mov.l r6, @-r7       ! write argb
    fmov fr13, @-r7      ! write v
    fmul fr10, fr9       ! 1 / w * y
    fmov fr12, @-r7      ! write u
    fmul fr10, fr8       ! 1 / w * x
    fmov fr10, @-r7      ! write z
    mov.l CMD_EOS, r3    ! load PVR_CMD_VERTEX_EOL flag to r3
    fmov fr9, @-r7       ! write y
    fmov fr8, @-r7       ! write x 
    mov.l r3, @-r7       ! write last vertex flag 

    pref @r7             ! flush vertex via SQ to PVR

    add #32, r7          ! move forward to next output vertex

    dt r0                ! decrement count, check for loop
    bf .LOADVERTEX       ! more triangles, run next loop
    
    mov.l @r15+, r14     ! pop stack back to r14
    fmov.s @r15+, fr13   ! pop stack back to fr13

    rts
    fmov.s @r15+, fr12   ! delay slot = pop stack back to fr12

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.align 2
TA_ADDR:
    .long 0xe0000000
CMD_VERT:
    .long 0xe0000000
CMD_EOS:
    .long 0xf0000000
CMD_CLR:
    .long 0xffffffff
    
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
