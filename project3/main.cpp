#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <omp.h>

// Setting the number of threads and capitals to try:
#ifndef NUMT
#define NUMT 2
#endif

#ifndef NUMCAPITALS
#define NUMCAPITALS 5
#endif

// Maximum iterations for convergence and number of tries for performance benchmarking
#define MAXITERATIONS 100
#define NUMTIMES 20

struct City {
    std::string name;
    float longitude;
    float latitude;
    int capitalNumber;
    float minDistance;
};

#include "UsCities.data"

// Setting the number of cities to try:
#define NUMCITIES (sizeof(Cities) / sizeof(struct City))

struct Capital {
    std::string name;
    float longitude;
    float latitude;
    float longSum;
    float latSum;
    int numSum;
};

std::vector<Capital> Capitals(NUMCAPITALS);

// Function to calculate the distance between city and capital
float Distance(int city, int capital) {
    float dx = Cities[city].longitude - Capitals[capital].longitude;
    float dy = Cities[city].latitude - Capitals[capital].latitude;
    return std::sqrt(dx * dx + dy * dy);
}

int main(int argc, char *argv[]) {
    // Check for OpenMP support
    #ifdef _OPENMP
    //fprintf(stderr, "OpenMP is supported -- version = %d\n", _OPENMP);
    #else
    std::cerr << "No OpenMP support!" << std::endl;
    return 1;
    #endif

    // Set number of OpenMP threads
    omp_set_num_threads(NUMT);

    // Initialize capitals at uniform intervals (start with cities spread across the region)
    for (int k = 0; k < NUMCAPITALS; k++) {
        int cityIndex = k * (NUMCITIES - 1) / (NUMCAPITALS - 1);
        Capitals[k].longitude = Cities[cityIndex].longitude;
        Capitals[k].latitude = Cities[cityIndex].latitude;
    }

    double time0, time1;
    for (int n = 0; n < MAXITERATIONS; n++) {
        // Reset capital summations
        for (int k = 0; k < NUMCAPITALS; k++) {
            Capitals[k].longSum = 0.;
            Capitals[k].latSum = 0.;
            Capitals[k].numSum = 0;
        }

        time0 = omp_get_wtime();

        #pragma omp parallel for default(none) shared(Capitals, Cities)
        for (int i = 0; i < NUMCITIES; i++) {
            int capitalNumber = -1;
            float minDistance = 1.e+37;

            for (int k = 0; k < NUMCAPITALS; k++) {
                float dist = Distance(i, k);
                if (dist < minDistance) {
                    minDistance = dist;
                    capitalNumber = k;
                    Cities[i].capitalNumber = capitalNumber;
                }
            }

            int k = Cities[i].capitalNumber;
            // Use a critical section to update capital data
            #pragma omp critical
            {
                Capitals[k].longSum += Cities[i].longitude;
                Capitals[k].latSum += Cities[i].latitude;
                Capitals[k].numSum++;
            }
        }

        time1 = omp_get_wtime();

        // Update each capital's longitude and latitude
        for (int k = 0; k < NUMCAPITALS; k++) {
            Capitals[k].longitude = Capitals[k].longSum / Capitals[k].numSum;
            Capitals[k].latitude = Capitals[k].latSum / Capitals[k].numSum;
        }
    }

    double megaCityCapitalsPerSecond = (double)NUMCITIES * (double)NUMCAPITALS / (time1 - time0) / 1000000.;

    // Find the actual city closest to each capital
    for (int k = 0; k < NUMCAPITALS; k++) {
        int closestCityIndex = -1;
        float minDistance = 1.e+37;

        for (int i = 0; i < NUMCITIES; i++) {
            float dist = Distance(i, k);
            if (dist < minDistance) {
                minDistance = dist;
                closestCityIndex = i;
            }
        }
        Capitals[k].name = Cities[closestCityIndex].name;
    }

    // Print out the results if using a single thread
    if (NUMT == 1) {
        for (int k = 0; k < NUMCAPITALS; k++) {
            std::cout << "\t" << k << ": " 
                      << Capitals[k].longitude << ", " 
                      << Capitals[k].latitude << " -> " 
                      << Capitals[k].name << std::endl;
        }
    }

    #ifdef CSV
    std::cerr << NUMT << " , " << NUMCITIES << " , " << NUMCAPITALS << " , " << megaCityCapitalsPerSecond << std::endl;
    #else
    std::cerr << NUMT << " threads : " << NUMCITIES << " cities ; " << NUMCAPITALS 
              << " capitals; megatrials/sec = " << megaCityCapitalsPerSecond << std::endl;
    #endif

    return 0;
}
