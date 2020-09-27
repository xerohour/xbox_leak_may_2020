
#define	F_NOTE	1
#define	F_REST	0
#define	F_JUMPTO	2
#define	F_LOOP	3
#define	F_ENDLOOP	4
#define	F_PATCH	5
#define	F_PAN	6
#define	F_MUX	7
#define	F_DEMUX	8
#define	F_VOLUME	9
#define	F_XPOSE	10
#define	F_XSET	11
#define	F_SLUR	12
#define	F_RING	13
#define	F_CLOCKSET	14
#define	F_END	15

#define	F_FILTERINC	16
#define	F_FILTERSET	17



#define	note(pitch,dur)	F_NOTE,pitch,dur
#define	rest(dur)		F_REST,dur
#define	jumpto(label)	F_JUMPTO,label
#define	loop(n)			F_LOOP,n
#define	endloop			F_ENDLOOP
#define	patch(num)		F_PATCH,num
#define	pan(value)		F_PAN,value
#define	mux(label)		F_MUX,label
#define	demux			F_DEMUX
#define	volume(val)		F_VOLUME,val
#define	xpose(val)		F_XPOSE,val
#define	xset(val)		F_XSET,val
#define	slur(pitch,dur)	F_SLUR,pitch,dur
#define	ring(dur)		F_RING,dur
#define	clockset(val)	F_CLOCKSET,val
#define	sosend			F_END
#define	finc(f,res)		F_FILTERINC,f,res
#define	fset(f,res)		F_FILTERSET,f,res
