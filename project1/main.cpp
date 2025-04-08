#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#ifndef F_PI
#define F_PI (float)M_PI
#endif

#ifndef DEBUG
#define DEBUG false
#endif

#ifndef NUMT
#define NUMT 2
#endif

#ifndef NUMTRIALS
#define NUMTRIALS 50000
#endif

#ifndef NUMTRIES
#define NUMTRIES 30
#endif

const float GMIN = 10.0;
const float GMAX = 20.0;
const float HMIN = 20.0;
const float HMAX = 30.0;
const float DMIN = 10.0;
const float DMAX = 20.0;
const float VMIN = 20.0;
const float VMAX = 30.0;
const float THMIN = 70.0;
const float THMAX = 80.0;

const float GRAVITY = -9.8;
const float TOL = 5.0;

float Ranf(float low, float high) {
    float r = (float)rand();
    float t = r / (float)RAND_MAX;
    return low + t * (high - low);
}

int Ranf( int ilow, int ihigh )
{
	float low = (float)ilow;
	float high = ceil( (float)ihigh );
	return (int) Ranf(low,high);
}

void TimeOfDaySeed() {
    time_t now;
    time(&now);
    struct tm n;
    n = *localtime(&now);
    srand((unsigned int)(1000. * difftime(now, mktime(&n))));
}

inline float Radians(float degrees) {
    return (F_PI / 180.f) * degrees;
}

int main(int argc, char* argv[]) {
#ifndef _OPENMP
    fprintf(stderr, "No OpenMP support!\n");
    return 1;
#endif

    TimeOfDaySeed();

    omp_set_num_threads(NUMT);

    float* vs = new float[NUMTRIALS];
    float* ths = new float[NUMTRIALS];
    float* gs = new float[NUMTRIALS];
    float* hs = new float[NUMTRIALS];
    float* ds = new float[NUMTRIALS];

    for (int n = 0; n < NUMTRIALS; n++) {
        vs[n] = Ranf(VMIN, VMAX);
        ths[n] = Ranf(THMIN, THMAX);
        gs[n] = Ranf(GMIN, GMAX);
        hs[n] = Ranf(HMIN, HMAX);
        ds[n] = Ranf(DMIN, DMAX);
    }

    double maxPerformance = 0.;
    int numHits;

    for (int tries = 0; tries < NUMTRIES; tries++) {
        double time0 = omp_get_wtime();
        numHits = 0;

		#pragma omp parallel for default(none) shared(vs, ths, gs, hs, ds) reduction(+:numHits)
        for (int n = 0; n < NUMTRIALS; n++) {
            float v = vs[n];
            float thr = Radians(ths[n]);
            float vx = v * cos(thr);
            float vy = v * sin(thr);
            float g = gs[n];
            float h = hs[n];
            float d = ds[n];

            float t = -vy / (0.5f * GRAVITY);
            float x = vx * t;

            if (x <= g) {
                continue;
            }

            t = g / vx;
            float y = vy * t + 0.5f * GRAVITY * t * t;

            if (y <= h) {
                continue;
            }

            float A = 0.5f * GRAVITY;
            float B = vy;
            float C = -h;
            float disc = B * B - 4.f * A * C;

            if (disc < 0.) {
                continue;
            }

            float sqrtdisc = sqrtf(disc);
            float t1 = (-B + sqrtdisc) / (2.f * A);
            float t2 = (-B - sqrtdisc) / (2.f * A);
            float tmax = (t1 > t2) ? t1 : t2;
            float upperDist = vx * tmax - g;

            if (fabs(upperDist - d) <= TOL) {
                numHits++;
            }
        }

        double time1 = omp_get_wtime();
        double megaTrialsPerSecond = (double)NUMTRIALS / (time1 - time0) / 1000000.;
        if (megaTrialsPerSecond > maxPerformance)
            maxPerformance = megaTrialsPerSecond;
    }

    float probability = (float)numHits / (float)NUMTRIALS;

#ifdef CSV
    printf("%2d , %8d , %6.2lf\n", NUMT, NUMTRIALS, maxPerformance);
#else
    printf("%2d threads : %8d trials ; probability = %6.2f%% ; megatrials/sec = %6.2lf\n",
        NUMT, NUMTRIALS, 100. * probability, maxPerformance);
#endif

    delete[] vs;
    delete[] ths;
    delete[] gs;
    delete[] hs;
    delete[] ds;

    return 0;
}
