#include "mbed.h"
#include "ultrasonic.h"

Serial pc(USBTX, USBRX);

// Store last measured distance and whether a change is pending
int lastDistance = 0;
bool changePending = false;

void dist(int distance)
{
    // Only if the difference is more than 10mm, trigger a pending change
    if (abs(distance - lastDistance) > 10) {
        printf("Distance changed to %dmm\r\n", distance);
        lastDistance = distance;
        changePending = true;
    }
}

ultrasonic mu(PB_0, PB_2, .1, 1, &dist);

int main()
{
    pc.baud(115200);
    mu.startUpdates();

    while (true)
    {
        mu.checkDistance();

        if (changePending)
        {
            // The distance changed, send a message
            printf("Sending data %dmm\r\n", lastDistance);
            changePending = false;

            // Wait 10 seconds after sending a message to reduce channel utilization
            wait(10);
        }
    }
}
