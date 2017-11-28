#include "mbed.h"
#include "mDot.h"
#include "ChannelPlans.h"
#include "ultrasonic.h"
#include "CayenneLPP.h"
#include "ttn_config.h"

Serial pc(USBTX, USBRX);

// Store last measured distance and whether a change is pending
int lastDistance = 0;
bool changePending = false;

// Callback function when the measured distance has changed
void distanceChanged(int distance)
{
    // Only if the difference is more than 20 mm, trigger a pending change
    if (abs(distance - lastDistance) > 20)
    {
        printf("Distance changed to %d cm\r\n", distance / 10);
        lastDistance = distance;
        changePending = true;
    }
}

// Measure every 1/10th second
ultrasonic mu(PB_0, PB_2, .1, 1, &distanceChanged);

int main()
{
    pc.baud(115200);

    // Initialize xDot with EU868 channel plan
    lora::ChannelPlan *plan = new lora::ChannelPlan_EU868();
    mDot *dot = mDot::getInstance(plan);

    // Configure the xDot: AppEUI, AppKey, enable public network and disable acknowledgements
    printf("Configuring xDot...\r\n");
    dot->resetConfig();
    dot->resetNetworkSession();
    if (dot->setAppEUI(appEUI) != mDot::MDOT_OK)
    {
        printf("Failed to set AppEUI\r\n");
    }
    if (dot->setAppKey(appKey) != mDot::MDOT_OK)
    {
        printf("Failed to set AppKey\r\n");
    }
    if (dot->setPublicNetwork(true) != mDot::MDOT_OK)
    {
        printf("Failed to set public network\r\n");
    }
    if (dot->setAck(false) != mDot::MDOT_OK)
    {
        printf("Failed to disable acknowledgements\r\n");
    }

    // Join the network
    printf("Joining The Things Network...\r\n");
    int32_t ret;
    while ((ret = dot->joinNetwork()) != mDot::MDOT_OK)
    {
        uint32_t waitTime = (dot->getNextTxMs() / 1000) + 1;
        printf("Failed to join network: %ld, retrying in %lu seconds...\r\n", ret, waitTime);
        wait(waitTime);
    }
    printf("Joined The Things Network\r\n");

    // Allocate 50 bytes for sending payload
    CayenneLPP payload(50);

    // Start measuring and sending data
    mu.startUpdates();
    while (true)
    {
        // See if there are changes pending
        mu.checkDistance();
        if (changePending)
        {
            printf("Sending data...\r\n");

            // Encode the distance
            payload.reset();
            payload.addAnalogInput(1, (float)lastDistance / 10);

            // Send the data to the network
            std::vector<uint8_t> data(payload.getBuffer(), payload.getBuffer() + payload.getSize());
            if ((ret = dot->send(data)) != mDot::MDOT_OK) {
                printf("Failed to send data: %ld\r\n", ret);
            } else {
                printf("Sent data\r\n");
            }

            // Wait 10 seconds after sending a message to reduce channel utilization
            wait(10);
            changePending = false;
        }
    }
}
