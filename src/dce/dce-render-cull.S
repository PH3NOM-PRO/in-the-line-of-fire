!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! DCE_RenderCullTriangles( float * vert_src, float * uv_src, unsigned int argb, unsigned int count );
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
!r0 = [var] = PVR TA SQ ADDR
!r1 = [var] = PVR VERTEX CMD
!r2 = [var] = PVR VERTEX EOL
!
!r4 = [arg][float*] = Vertex Pointer
!r5 = [arg][float*] = UV Pointer
!r6 = [arg][uint32] = Color (32bit int argb)
!r7 = [arg][uint32] = Count ( triangles )
!
!fv0 =  [var] = 1st Vertex of Triangle
!fv4 =  [var] = 2nd Vertex of Triangle
!fv8 =  [var] = 3rd Vertex of Triangle
!fr12 = [var] = Vertex U Component
!fr13 = [var] = Vertex V Component
!fr14 = [var] = Near-Z Clipping Plane
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.globl _DCE_RenderCullTriangles

.align 4

_DCE_RenderCullTriangles:

    mov.l CMD_VERT, r1   ! load PVR_CMD_VERTEX flag to r1
    fmov.s fr14, @-r15   ! push fr14 to stack ( callee save )
    mov.l TA_ADDR, r0    ! load TA SQ ADDR to r0
    fmov.s fr13, @-r15   ! push fr13 to stack ( callee save )
    add #28, r0          ! move output vertex to argb component
    fmov.s fr12, @-r15   ! push fr12 to stack ( callee save ) 
    mov.l CMD_EOS, r2    ! load PVR_CMD_VERTEX flag to r2
    
    fldi1 fr14           ! load 1 for Near Z Clipping Plane
    fneg fr14            ! negate so -1 = Near Z Clipping Plane
    
.LOADXV1:

    fmov @r4+, fr0       ! load 1st vertex x
    fmov @r4+, fr1       ! load 1st vertex y 
    fmov @r4+, fr2       ! load 1st vertex z
    fldi1      fr3       ! load 1 for w

    ftrv xmtrx, fv0      ! transform 1st vector
    
.LOADXV2:
    
    add #20, r4          ! move vertex pointer to next x component
    fmov @r4+, fr4       ! load 2nd vertex x
    fmov @r4+, fr5       ! load 2nd vertex y 
    fmov @r4+, fr6       ! load 2nd vertex z
    fldi1      fr7       ! load 1 for w

    fcmp/gt fr14, fr2    ! check 1st vertex for z clipping
    bf .LOADXV3          ! vertex is insinde clipping plane - load and transform next vertex

    bra .NEXT_TRI        ! vertex is out of clipping plane - skip forward to next triangle
    add #52, r4          ! delay slot add vertex stride to next triangle
    
.LOADXV3:

    ftrv xmtrx, fv4      ! transform 2nd vector

    add #20, r4           ! move vertex pointer past u/v components
    fmov @r4+, fr4       ! load 3rd vertex x
    fmov @r4+, fr5       ! load 3rd vertex y 
    fmov @r4+, fr6       ! load 3rd vertex z
    fldi1      fr7       ! load 1 for w

    fcmp/gt fr14, fr6    ! check 2nd vertex for z clipping
    bf .XV3              ! vertex is insinde clipping plane - load and transform next vertex

    bra .NEXT_TRI        ! vertex is out of clipping plane - skip forward to next triangle
    add #20, r4           ! delay slot add vertex stride to next triangle

.XV3:

    ftrv xmtrx, fv8      ! transform  3rd vector

.LOADUV1:
  
    fmov @r5+, fr12      ! load u to fr12 V1
    fmov @r5, fr13       ! load v to fr13 V1
    add #16, r5          ! add uv stride  V1

    mov.l r6, @-r0       ! write argb V1
    fmov fr13, @-r0      ! write v    V1
    fmov fr12, @-r0      ! write u    V1
    
    fcmp/gt fr14, fr10   ! check 2nd vertex for z clipping
    bf .PDIV_WRITE       ! vertex is insinde clipping plane - do perspective division on vertices and write out

    bra .NEXT_TRI        ! vertex is out of clipping plane - skip forward to next triangle
    add #56, r5          ! delay slot add uv stride to next triangle    
 
 .PDIV_WRITE:
    
    fldi1 fr2            ! V1->z = 1
    fdiv fr3, fr2        ! perspective divide  V1->z = 1.0f / V1->w
    
    fldi1 fr6            ! V2->z = 1
    fldi1 fr10           ! V3->z = 1
    
    fmul fr2, fr1        ! 1 / w * y V1
    fmov fr2, @-r0       ! write z   V1
    fmul fr2, fr0        ! 1 / w * x V1
    fmov fr1, @-r0       ! write y   V1
    fmov fr0, @-r0       ! write x   V1
    mov.l r1, @-r0       ! write 1st vertex flag
    
    pref @r0             ! flush 1st vertex via SQ to PVR

    fdiv fr7, fr6        ! perspective divide  V2->z = 1.0f / V2->w    

    add #60, r0          ! move to argb component    

    fmov @r5+, fr12      ! load u to fr12 V2
    fmov @r5, fr13       ! load v to fr13 V2
    add #16, r5          ! add uv stride  V2

    mov.l r6, @-r0       ! write argb V2
    fmov fr13, @-r0      ! write v    V2
    fmov fr12, @-r0      ! write u    V2

    fmul fr6, fr5        ! 1 / w * y V2
    fmov fr6, @-r0       ! write z   V2
    fmul fr6, fr4        ! 1 / w * x V2
    fmov fr5, @-r0       ! write y   V2
    fmov fr4, @-r0       ! write x   V2
    mov.l r1, @-r0       ! write 2nd vertex flag    

    pref @r0             ! flush 2nd vertex via SQ to PVR

    fdiv fr11, fr10      ! perspective divide  V3->z = 1.0f / V3->w    

    add #60, r0          ! move to argb component    

    fmov @r5+, fr12      ! load u to fr12 V3
    fmov @r5, fr13       ! load v to fr13 V3
    add #16, r5          ! add uv stride  V3

    mov.l r6, @-r0       ! write argb V3
    fmov fr13, @-r0      ! write v    V3
    fmov fr12, @-r0      ! write u    V3

    fmul fr10, fr5       ! 1 / w * y V3
    fmov fr10, @-r0      ! write z   V3
    fmul fr10, fr4       ! 1 / w * x V3
    fmov fr9, @-r0       ! write y   V3
    fmov fr8, @-r0       ! write x   V3
    mov.l r2, @-r0       ! write 3rd vertex eol flag    

    pref @r0             ! flush 3rd vertex via SQ to PVR

.NEXT_TRI:

    dt r7                ! decrement count, check for loop
    bf .LOADXV1          ! more triangles, run next loop

.RETURN:

    fmov.s @r15+, fr12   ! pop stack back to fr12
    fmov.s @r15+, fr13   ! pop stack back to fr13
    
    mov r7, r0
    
    rts
    fmov.s @r15+, fr14   ! delay slot = pop stack back to fr14

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.align 2
TA_ADDR:
    .long 0xe0000000
CMD_VERT:
    .long 0xe0000000
CMD_EOS:
    .long 0xf0000000
    
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
