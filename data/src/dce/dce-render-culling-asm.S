!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!void DCE_RenderCullTransformTriangles(float *vert_pos,     ! ireg r4
!                                      float * uv_coord,    ! ireg r5
!                                      int stride,          ! ireg r6
!                                      int count);          ! ireg r7

.globl _DCE_RenderCullTransformTriangles

!r0 = [int] = return clip code for triangle
!r1 = [int] = vert address
!r2 = [int] = uv address
!r3 = [int] = argb address

!r4 = [arg][float *] = vertex position pointer
!r5 = [arg][pvr_vertex_t *] = output vector
!r6 = [arg][pvr_vertex_t *] = uv pointer
!r7 = [arg][uint32 *] = argb pointer

!fr4 = [arg][float] vert stride (in bytes)(passed as float to prevent stack use)
!fr5 = [arg][float] uv stride (in bytes)(passed as float to prevent stack use) 
!fr6 = [arg][float] argb stride (in bytes)(passed as float to prevent stack use) 

!fv0 = vector 1
!fv4 = vector 2
!fv8 = vector 3

!fr12 = u component
!fr13 = v component
!fr15 = z clip threshold
 
.align 4

_DCE_RenderCullTransformTriangles:

    fmov.s fr12, @-r15   ! push fr12 to stack ( callee save ) 
    ftrc fr4, fpul       ! floatToInt(vert stride)
    fmov @r4+, fr0       ! load vertex x
    sts fpul, r1         ! r1 now holds vert stride
    fmov @r4+, fr1        ! load vertex y 
    add #-8, r1          ! adjust vert stride to offset read increment
    fmov @r4, fr2        ! load vertex z
    fldi1 fr3            ! load 1 for w

	ftrc fr6, fpul       ! floatToInt(argb stride)
	sts fpul, r3         ! r3 = argb stride

    ftrv xmtrx, fv0      ! transform first vector
  
    ftrc fr5, fpul       ! floatToInt(uv stride)
    add r1, r4           ! add vertex position stride
    fmov @r4+, fr4       ! load vertex x
    fldi1 fr7            ! load 1 for w
    fmov @r4+, fr5       ! load vertex y
    mov #0, r0           ! set 0 for clip code
    fmov @r4, fr6        ! load vertex z

    ftrv xmtrx, fv4      ! transform second vector

    add r1, r4           ! add vertex position stride
    fmov @r4+, fr8       ! load vertex x
    fldi1 fr11           ! load 1 for w
    fmov @r4+, fr9       ! load vertex y
    sts fpul, r2         ! r2 now holds uv stride
    fmov @r4, fr10       ! load vertex z
    add #-4, r2          ! adjust uv stride to offset read increment

    ftrv xmtrx, fv8      ! transform third vector

    fmov.s fr15, @-r15   ! push fr15 to stack ( callee save )    
    fldi1 fr15           ! load 1 to fr15 for clip threshold
    fneg fr15            ! clip threshold = -1.0f

    fcmp/gt fr15, fr2    ! check 1st vertex for z clipping
    bf .V1IN

    or #1, r0            ! clip code 1st vertex out

.V1IN:

    fcmp/gt fr15, fr6    ! check 2nd vertex for z clipping
    bf .V2IN

    or #2, r0            ! clip code 2nd vertex out

.V2IN:

    fcmp/gt fr15, fr10   ! check 3rd vertex for z clipping
    bf .V3IN

    or #4, r0            ! clip code 3rd vertex out

.V3IN:

    cmp/eq #7, r0        ! clip code (1|2|4)= 7 = all out - dont write output
    bt .RETURN
  
    cmp/eq #0, r0        ! clip code 0 = all in - write with perspective divide
    bf .WRITENODIVIDE    ! otherwise, write output with no perspective divide

!.WRITEWITHDIVIDE:

    fldi1 fr2
    fdiv fr3, fr2        ! perspective divide

	mov.l r8, @-r15      ! push r8 to stack
	mov.l @r7, r8        ! load argb to r8
	add r3, r7           ! add argb sride
    mov.l TA_ADDR, r5    ! set vertex out dest to Tile Accelerator

    fmov.s fr13, @-r15   ! push fr13 to stack ( callee save )
    add #28, r5          ! move output vertex to argb component
    fmov @r6+, fr12      ! load u to fr12
	fmov @r6, fr13       ! load v to fr13
    add r2, r6           ! add uv stride
    
	mov.l r8, @-r5       ! write argb
    fmov fr13, @-r5      ! write v
    mov.l CMD_VERT, r0   ! load PVR_CMD_VERTEX flag to r0
    fmov fr12, @-r5      ! write u
    fmul fr2, fr1        ! 1 / w * y
    fmov fr2, @-r5       ! write z
    fmul fr2, fr0        ! 1 / w * x
    fmov fr1, @-r5       ! write y
    fldi1 fr6            ! load 1 to fr6 for next 1 / w op
    fmov fr0, @-r5       ! write x
	mov.l r0, @-r5       ! write vertex flags

	pref @r5             ! flush vertex via SQ to PVR

    fdiv fr7, fr6        ! perspective divide

	mov.l @r7, r8        ! load argb to r8
	add r3, r7           ! add argb sride

    add #60, r5          ! move output vertex to v component
    fmov @r6+, fr12      ! load u to fr12
    fmov @r6, fr13       ! load v to fr13
    add r2, r6           ! add uv stride

	mov.l r8, @-r5       ! write argb
    fmov fr13, @-r5      ! write v
    fmul fr6, fr5        ! 1 / w * y
    fmov fr12, @-r5      ! write u
    fmul fr6, fr4        ! 1 / w * x
    fmov fr6, @-r5       ! write z
    fldi1 fr10           ! load 1 to fr10 for next 1 / w op
    fmov fr5, @-r5       ! write y
    fmov fr4, @-r5       ! write x 
	mov.l r0, @-r5       ! write vertex flags

	pref @r5             ! flush vertex via SQ to PVR

    fdiv fr11, fr10      ! perspective divide

	mov.l @r7, r8        ! load argb to r8
	add r3, r7           ! add argb sride

    add #60, r5          ! move output vertex to v component
    fmov @r6+, fr12      ! load u to fr12
    fmov @r6, fr13       ! load v to fr13

	mov.l r8, @-r5       ! write argb
    fmov fr13, @-r5      ! write v
    fmul fr10, fr9       ! 1 / w * y
    mov.l CMD_EOS, r0    ! load PVR_CMD_VERTEX_EOL flag to r0
    fmov fr12, @-r5      ! write u
    fmul fr10, fr8       ! 1 / w * x
    fmov fr10, @-r5      ! write z
    fmov fr9, @-r5       ! write y
    fmov fr8, @-r5       ! write x 
	mov.l r0, @-r5       ! write vertex flags

	pref @r5             ! flush vertex via SQ to PVR

    fmov.s @r15+, fr13   ! delay slot = pop stack back to fr13

    mov.l @r15+, r8      ! delay slot = pop stack back to r8

.RETURN:

    fmov.s @r15+, fr15   ! pop stack back to fr15
    rts
    fmov.s @r15+, fr12   ! pop stack back to fr12

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.align 2

VERT_ARGB:
    .long 0xffffffff
TA_ADDR:
    .long 0xe0000000
CMD_VERT:
    .long 0xe0000000
CMD_EOS:
    .long 0xf0000000

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
