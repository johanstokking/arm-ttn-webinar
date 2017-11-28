#include "mbed.h"
#include "mDot.h"
#include "ChannelPlans.h"
#include "ultrasonic.h"
#include "ttn_config.h"

Serial pc(USBTX, USBRX);
mDot *dot = NULL;
lora::ChannelPlan *plan = NULL;

// Store last measured distance and whether a change is pending
int lastDistance = 0;
bool changePending = false;

void distanceChanged(int distance)
{
    // Only if the difference is more than 10mm, trigger a pending change
    if (abs(distance - lastDistance) > 10)
    {
        printf("Distance changed to %dmm\r\n", distance);
        lastDistance = distance;
        changePending = true;
    }
}

ultrasonic mu(PB_0, PB_2, .1, 1, &distanceChanged);

int main()
{
    pc.baud(115200);

    // Initialize xDot with EU868 channel plan
    plan = new lora::ChannelPlan_EU868();
    dot = mDot::getInstance(plan);

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

    // Start measuring distance updates
    mu.startUpdates();

    while (true)
    {
        // See if there are changes pending
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
