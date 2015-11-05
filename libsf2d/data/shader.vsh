; Outputs
.out outpos position
.out outtc0 texcoord0
.out outclr color

; Inputs
.alias inpos v0
.alias inarg v1

; Uniforms, for use with SHDR_GetUniformRegister
.fvec mvpMtx[4]

.proc main
	; outpos = mvpMtx * in.pos
	dp4 outpos.x, mvpMtx[0], inpos
	dp4 outpos.y, mvpMtx[1], inpos
	dp4 outpos.z, mvpMtx[2], inpos
	dp4 outpos.w, mvpMtx[3], inpos

	; outtc0 = in.texcoord
	mov outtc0, inarg

	; outclr = in.color
	mov outclr, inarg

	end
.end
