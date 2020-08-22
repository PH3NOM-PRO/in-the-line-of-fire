!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!void DCE_CollisionRayTrace(float * V1, float * V2, float * V3, float * D, float * E1, float * E2);

.globl _DCE_CollisionRayTrace


!r4 = [arg][float *] = V1
!r5 = [arg][float *] = V2
!r6 = [arg][float *] = V3
!r7 = [arg][float *] = D
!r8 = [arg][float *] = E1 OUT
!r9 = [arg][float *] = E2 OUT

!fv0 = V1 = E1
!fv1 = V2 = E1
!fv2 = V3 = E2

.align 4

_DCE_CollisionRayTrace:

    fmov.s fr12, @-r15   ! push fr12 to stack ( callee save ) 
    fmov.s fr13, @-r15   ! push fr13 to stack ( callee save )
    fmov.s fr14, @-r15   ! push fr14 to stack ( callee save ) 
    fmov.s fr15, @-r15   ! push fr15 to stack ( callee save )
    
.LOADVERTEX1:

    fmov @r4+, fr0       ! load v1 x
    fmov @r4+, fr1       ! load v1 y 
    fmov @r4+, fr2       ! load v1 z 

    fmov fr0, fr3        ! copy v1 to fv1
	fmov fr1, fr4
	fmov fr2, fr5
	
    fmov @r5+, fr12      ! load v2 x
    fmov @r5+, fr13      ! load v2 y 
    fmov @r5+, fr14      ! load v2 z 	   

    fsub fr12, fr0       ! E1.x = fr0
    fsub fr13, fr1       ! E1.y = fr1
    fsub fr14, fr2       ! E1.z = fr2
    
    fmov @r6+, fr12      ! load v3 x
    fmov @r6+, fr13      ! load v3 y 
    fmov @r6+, fr14      ! load v3 z 	   

    fsub fr12, fr4       ! E2.x = fr4
    fsub fr13, fr5       ! E2.y = fr5
    fsub fr14, fr6       ! E2.z = fr6    

! at this point, fv0 = V1, fv1 = E1, fv2 = E2
    
    add #8, r7
    fmov fr0, @-r7       ! write E1.x
    fmov fr1, @-r7       ! write E1.y
    fmov fr2, @r7        ! write E1.z    
	
    add #8, r8
    fmov fr4, @-r8       ! write E2.x
    fmov fr5, @-r8       ! write E2.y
    fmov fr6, @r8        ! write E2.z  	
			    
.RETURN:    
    
    fmov.s @r15+, fr15   ! pop stack back to fr15
    fmov.s @r15+, fr14   ! pop stack back to fr14
    fmov.s @r15+, fr13   ! pop stack back to fr13
    
    rts
    fmov.s @r15+, fr12   ! delay slot = pop stack back to fr12


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! void DCE_Vec3fSub(float * V1, float * V2, float * V3, float * E);

.globl _DCE_Vec3fSub


!r4 = [arg][float *] = V1
!r5 = [arg][float *] = V2
!r6 = [arg][float *] = Dst

.align 4

_DCE_Vec3fSub:

    fmov.s fr12, @-r15   ! push fr12 to stack ( callee save ) 
    fmov.s fr13, @-r15   ! push fr13 to stack ( callee save )
    fmov.s fr14, @-r15   ! push fr14 to stack ( callee save ) 
    fmov.s fr15, @-r15   ! push fr15 to stack ( callee save )

    fmov @r4+, fr0       ! load v1 x
    fmov @r4+, fr1       ! load v1 y 
    fmov @r4+, fr2       ! load v1 z 
	
    fmov @r5+, fr4       ! load v2 x
    fmov @r5+, fr5       ! load v2 y 
    fmov @r5+, fr6       ! load v2 z 	   

    fmov @r6+, fr8       ! load v3 x
    fmov @r6+, fr9       ! load v3 y 
    fmov @r6+, fr10      ! load v3 z 

    fmov fr0, fr12       ! copy v1->fv3 ! E2
    fmov fr1, fr13
    fmov fr2, fr14

    fsub fr4, fr0        ! E1.x = fr0
    fsub fr5, fr1        ! E1.y = fr1
    fsub fr6, fr2        ! E1.z = fr2  

    fsub fr8, fr12       ! E2.x = fr0
    fsub fr9, fr13       ! E2.y = fr1
    fsub fr10, fr14      ! E2.z = fr2  

! at this point, fv0 = V1, fv1 = E1, fv2 = E2
    
    fmov fr0, @r7        ! write E1.x
    add #4, r7
	fmov fr1, @r7        ! write E1.y
	add #4, r7
    fmov fr2, @r7        ! write E1.z    
    add #4, r7
    fmov fr12, @r7       ! write E2.x
    add #4, r7
	fmov fr13, @r7       ! write E2.y
	add #4, r7
    fmov fr14, @r7       ! write E2.z         
    
    fmov.s @r15+, fr15   ! pop stack back to fr15
    fmov.s @r15+, fr14   ! pop stack back to fr14
    fmov.s @r15+, fr13   ! pop stack back to fr13
    
    rts
    fmov.s @r15+, fr12   ! delay slot = pop stack back to fr12
    
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
