#define IN
#define OUT

kernel
void
Regression(	IN global const float *dX,
	   	IN global const float *dY,
		OUT global float *dSumx4,
		OUT global float *dSumx3,
		OUT global float *dSumx2,
		OUT global float *dSumx,
		OUT global float *dSumx2y,
		OUT global float *dSumxy,
		OUT global float *dSumy )
{
	int gid = get_global_id( 0 );

	float x = dX[gid];
	float y = dY[gid];
	
	float x2 = x * x;
	float x3 = x2 * x;
	float x4 = x3 * x;

	dSumx4[ gid ]  = x4;
	dSumx3[ gid ]  = x3;
	dSumx2[ gid ]  = x2;
	dSumx[ gid ]   = x;
	dSumx2y[ gid ] = x2 * y;
	dSumxy[ gid ]  = x * y;
	dSumy[ gid ]   = y;
}
