!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! DCE_RenderCullTris( float * vert_src, float * uv_src, int count, int stride); !
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
!r0 = [int] = Tile Accelerator SQ Destination
!r1 = [int] = PVR Vertex Command
!r2 = [int] = Vertex ARGB
!r3 = [int] = Vertex Stride for U/V
!r4 = [arg][float *] = Vertex Source Pointer
!r5 = [arg][float *] = UV Source Pointer
!r6 = [arg][int32]   = Triangle Count
!r7 = [arg][int32]   = Vertex And UV Source Pointer Stride
!
!               0 1 2 3
!fv0 = Vertex 0 x y z w 0 1 2 3
!fv4 = Vertex 1 x y z w 4 5 6 7
!fv8 = Vertex 2 x y z w 8 9 10 11
!fr12 = U
!fr13 = V
!fr14 = Near Z Clip Plane
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.globl _DCE_RenderCullTris

.align 4

_DCE_RenderCullTris:

    mov.l TA_ADDR,  r0   ! load TA SQ ADDR to r0
    mov.l CMD_VERT, r1   ! load PVR_CMD_VERTEX flag to r1
    mov.l CMD_EOS,  r2   ! load PVR_CMD_VERTEX_EOL flag to r2
    mov.l CMD_ARGB, r3   ! load ARGB to r3
	    
    fmov.s fr14, @-r15   ! push fr14 to stack ( callee save )
    fmov.s fr13, @-r15   ! push fr13 to stack ( callee save )
    fmov.s fr12, @-r15   ! push fr12 to stack ( callee save ) 
    
    fldi1 fr14           ! load 1 for Near Z Clipping Plane
    fneg  fr14           ! negate so -1 = Near Z Clipping Plane

    add #28, r0          ! move output vertex to argb component
    add #-8, r7          ! offset Vertex stride by Vertex read increment
    
.BEGIN_LOOP:

    fmov @r4+, fr0       ! load vertex x
    fmov @r4+, fr1       ! load vertex y 
    fmov @r4,  fr2       ! load vertex z 
    fldi1 fr3            ! load 1 for  w

    add r7, r4           ! add vertex stride

    ftrv xmtrx, fv0      ! transform 1st vector   

    fmov @r4+, fr4       ! load vertex x
    fmov @r4+, fr5       ! load vertex y 
    fmov @r4,  fr6       ! load vertex z 
    fldi1 fr7            ! load 1 for  w

    add r7, r4           ! add vertex stride

    fcmp/gt fr14, fr2    ! check 1st vertex for z culling before we transform 2nd vertex
    bt .SKIP_WRITEV1

    ftrv xmtrx, fv4      ! transform 2nd vector
    
    fmov @r4+, fr8       ! load vertex x
    fmov @r4+, fr9       ! load vertex y 
    fmov @r4,  fr10      ! load vertex z 
    fldi1 fr11           ! load 1 for  w

    add r7, r4           ! add vertex stride

    fcmp/gt fr14, fr6    ! check 2nd vertex for z culling before we transform 3rd vertex
    bt .SKIP_WRITEV2

    ftrv xmtrx, fv8      ! transform 3rd vector

    fcmp/gt fr14, fr10   ! check 3rd vertex for z culling
    bt .SKIP_WRITEV2

.WRITE_TRI:

    fldi1 fr2            ! vertex 0 z = 1
    fdiv fr3, fr2        ! vertex 0 z = 1 / w
    fmul fr2, fr1        ! vertex 0 y *= 1 / w
    fmul fr2, fr0        ! vertex 0 x *= 1 / w
    
    fmov @r5+, fr12      ! load vertex 0 u
    fmov @r5+,  fr13      ! load vertex 0 v    
    add r7, r5           ! increment vertex stride
    
    mov.l r3,  @-r0      ! write argb
    fmov fr13, @-r0      ! write v
    fmov fr12, @-r0      ! write u
    fmov fr2,  @-r0      ! write z
    fmov fr1,  @-r0      ! write y
    fmov fr0,  @-r0      ! write x    
	mov.l r1,  @-r0      ! write vetex flag
    
    pref @r0             ! flush vertex 0 to pvr ta
    
    add #28, r0          ! move output vertex to next argb component
    
    fldi1 fr6            ! vertex 1 z = 1
    fdiv fr7, fr6        ! vertex 1 z = 1 / w
    fmul fr6, fr5        ! vertex 1 y *= 1 / w
    fmul fr6, fr4        ! vertex 1 x *= 1 / w
    
    fmov @r5+, fr12      ! load vertex 1 u
    fmov @r5+,  fr13      ! load vertex 1 v    
    add r7, r5           ! increment vertex stride
    
    mov.l r3,  @-r0      ! write argb
    fmov fr13, @-r0      ! write v
    fmov fr12, @-r0      ! write u
    fmov fr6,  @-r0      ! write z
    fmov fr5,  @-r0      ! write y
    fmov fr4,  @-r0      ! write x
    mov.l r1,  @-r0      ! write vetex flag
    
    pref @r0             ! flush vertex 1 to pvr ta
    
    add #28, r0          ! move output vertex to next argb component

    fldi1 fr10           ! vertex 2 z = 1
    fdiv fr11, fr10      ! vertex 2 z = 1 / w
    fmul fr10, fr9       ! vertex 2 y *= 1 / w
    fmul fr10, fr8       ! vertex 2 x *= 1 / w
    
    fmov @r5+, fr12      ! load vertex 2 u
    fmov @r5+,  fr13      ! load vertex 2 v    
    add r7, r5           ! increment vertex stride
    
    mov.l r3,  @-r0      ! write argb
    fmov fr13, @-r0      ! write v
    fmov fr12, @-r0      ! write u
    fmov fr10, @-r0      ! write z
    fmov fr9,  @-r0      ! write y
    fmov fr8,  @-r0      ! write x  
    mov.l r2,  @-r0      ! write vetex flag
    
    pref @r0             ! flush vertex 2 to pvr ta
    
    add #28, r0          ! move output vertex to next argb component
    
    bra .CHECK_LOOP
    nop
    
.SKIP_WRITEV1:

     add #96, r5
     add #32, r4
     bra .CHECK_LOOP
     nop

.SKIP_WRITEV2:

     add #96, r5
    
.CHECK_LOOP:  
    
    dt r6
    bf .BEGIN_LOOP
    
.RETURN:

    fmov.s @r15+, fr12   ! pop stack back to fr12
    fmov.s @r15+, fr13   ! pop stack back to fr13
    
    rts
    fmov.s @r15+, fr14   ! delay slot = pop stack back to fr14   

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.align 2

TA_ADDR:  .long 0xe0000000

CMD_VERT: .long 0xe0000000

CMD_EOS:  .long 0xf0000000

CMD_ARGB: .long 0xffffffff
    
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
