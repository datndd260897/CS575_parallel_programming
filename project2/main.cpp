#include <iostream>
#include <fstream>  // For file output
#include <cmath>
#include <omp.h>
#include <cstdlib>

using namespace std;

// Global Variables
int NowYear = 2025;        // Start year
int NowMonth = 0;          // Start month
float NowPrecip = 0.0;     // Precipitation
float NowTemp = 0.0;       // Temperature
float NowHeight = 5.0;     // Starting grain height (inches)
int NowNumDeer = 2;        // Starting number of deer
int NowNumWolves = 1;      // Starting number of wolves

// Constants
const float GRAIN_GROWS_PER_MONTH = 12.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;
const float AVG_PRECIP_PER_MONTH = 7.0;
const float AMP_PRECIP_PER_MONTH = 6.0;
const float RANDOM_PRECIP = 2.0;
const float AVG_TEMP = 60.0;
const float AMP_TEMP = 20.0;
const float RANDOM_TEMP = 10.0;
const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;
const float WOLF_EATS_DEER_PER_MONTH = 0.2;  // Percentage of deer eaten by wolves each month (per wolf)

unsigned int seed = 0;

// Barrier Variables
omp_lock_t Lock;
volatile int NumInThreadTeam;
volatile int NumAtBarrier;
volatile int NumGone;

void InitBarrier(int n) {
    NumInThreadTeam = n;
    NumAtBarrier = 0;
    omp_init_lock(&Lock);
}

void WaitBarrier() {
    omp_set_lock(&Lock);
    {
        NumAtBarrier++;
        if (NumAtBarrier == NumInThreadTeam) {
            NumGone = 0;
            NumAtBarrier = 0;
            while (NumGone != NumInThreadTeam - 1);
            omp_unset_lock(&Lock);
            return;
        }
    }
    omp_unset_lock(&Lock);

    while (NumAtBarrier != 0);  // Wait for others to finish

    #pragma omp atomic
    NumGone++;
}

// Helper function to generate random float values in a range
float Ranf(float low, float high) {
    return (float)rand() / (RAND_MAX + 1.0) * (high - low) + low;
}

// SQR function
float SQR(float x) {
    return x * x;
}

// Grain Growth Function
void Grain() {
    while (NowYear < 2031) {
        // Compute next grain height
        float ang = (30.0 * (float)NowMonth + 15.0) * (M_PI / 180.0);
        NowTemp = AVG_TEMP - AMP_TEMP * cos(ang) + Ranf(-RANDOM_TEMP, RANDOM_TEMP);
        NowPrecip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang) + Ranf(-RANDOM_PRECIP, RANDOM_PRECIP);
        if (NowPrecip < 0) NowPrecip = 0;

        float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.0));
        float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.0));
        float nextHeight = NowHeight + tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

        if (nextHeight < 0) nextHeight = 0.0;

        // DoneComputing barrier
        WaitBarrier();

        // Assign new global height
        NowHeight = nextHeight;

        // DoneAssigning barrier
        WaitBarrier();
    }
}

// Deer Population Function with wolves affecting deer population
void Deer() {
    while (NowYear < 2031) {
        // Compute next deer population based on grain height (carrying capacity)
        int nextNumDeer = NowNumDeer;
        int carryingCapacity = (int)(NowHeight);

        // Wolves reduce deer population each month
        int wolvesEatingDeer = (int)(NowNumWolves * WOLF_EATS_DEER_PER_MONTH);
        nextNumDeer -= wolvesEatingDeer;

        // Make sure the deer population doesn't go below zero
        if (nextNumDeer < 0) nextNumDeer = 0;

        // If deer population is below the carrying capacity, it can increase
        if (nextNumDeer < carryingCapacity) {
            nextNumDeer++;
        } else if (nextNumDeer > carryingCapacity) {
            nextNumDeer--;
        }

        // DoneComputing barrier
        WaitBarrier();

        // Assign new global deer population
        NowNumDeer = nextNumDeer;

        // DoneAssigning barrier
        WaitBarrier();
    }
}

// Your Own Agent (Control Wolves based on Deer Population with Mortality)
void MyAgent() {
    while (NowYear < 2031) {
        // Control the wolf population based on the number of deer
        if (NowNumDeer >= 4) {
            NowNumWolves++;  // Increase wolves by 1 if deer are >= 4
        } else if (NowNumDeer < 4 && NowNumWolves > 1) {
            NowNumWolves--;  // Decrease wolves by 1 if deer are < 4, but don't go below 1 wolf
        }

        // Implement wolf mortality if wolves outnumber deer
        if (NowNumWolves > NowNumDeer) {
            // Mortality is proportional to the difference between wolves and deer
            float wolfDeathRate = 0.1; // Adjust this value to control the death rate
            int wolvesToDie = (int)((NowNumWolves - NowNumDeer) * wolfDeathRate);
            NowNumWolves -= wolvesToDie;

            // Ensure wolves don't die below 1
            if (NowNumWolves < 1) {
                NowNumWolves = 1;
            }
        }

        // DoneComputing barrier
        WaitBarrier();

        // DoneAssigning barrier
        WaitBarrier();
    }
}

// Watcher Function (to print the state of the system to CSV)
void Watcher() {
    // Create an output file stream and open a CSV file
    ofstream outFile("simulation_output.csv");

    // Write headers to the CSV file
    outFile << "Months,Temp(C),Precip(cm),Height(cm),Deers,Wolves" << endl;

    while (NowYear < 2031) {
        // DoneComputing barrier
        WaitBarrier();

        // Convert temperature from Fahrenheit to Celsius
        float tempInC = (5.0 / 9.0) * (NowTemp - 32);  // °C = (5./9.) * (°F - 32)

        // Convert height from inches to centimeters
        float heightInCm = NowHeight * 2.54;  // Convert inches to cm

        // Convert precipitation from inches to centimeters
        float precipInCm = NowPrecip * 2.54;  // Convert inches to cm

        // Write the data to the CSV file
        outFile << NowMonth << " , " << tempInC << " , " << precipInCm << " , " << heightInCm << " , " << NowNumDeer << " , " << NowNumWolves << endl;

        // Increment month
        NowMonth++;
        if (NowMonth % 12 == 0) {
            NowYear++;
        }

        // DoneAssigning barrier
        WaitBarrier();

        // DonePrinting barrier
        WaitBarrier();
    }

    // Close the file after the simulation ends
    outFile.close();
}

int main() {
    // Initialize random seed
    srand(seed);

    // Set number of OpenMP threads
    omp_set_num_threads(4);  // Change to 4 after adding MyAgent

    // Initialize barrier
    InitBarrier(4);  // We have 4 threads now

    // Start parallel sections
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            Deer();
        }

        #pragma omp section
        {
            Grain();
        }

        #pragma omp section
        {
            Watcher();
        }

        #pragma omp section
        {
            MyAgent();  // Your custom agent (wolves population growth with mortality)
        }
    }

    return 0;
}
