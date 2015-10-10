// setup output map
.alias outpos  o0 as position
.alias outtex0 o1 as texcoord0
.alias outcol  o2 as color

// setup uniform map, for use with SHDR_GetUniformRegister
.alias projection c0-c3

main:
	// outpos = projMtx * in.pos
	dp4 outpos.x, projection[0], v0
	dp4 outpos.y, projection[1], v0
	dp4 outpos.z, projection[2], v0
	dp4 outpos.w, projection[3], v0

	// outtex0 = in.texcoord
	mov outtex0, v1

	// outcolor = in.color
	mov outcol, v1

	nop
	end
