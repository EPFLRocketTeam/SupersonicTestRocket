#include <Arduino.h>
#include <unity.h>

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void setup()
{
    // NOTE!!! Wait for >5 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(5000);
    while (!Serial)
    {
    }

    UNITY_BEGIN(); // IMPORTANT LINE!

    // tests to run once
    //RUN_TEST(test_PushButtonArray_translateState);
}

// tests to run multiple times
void loop()
{
    for (size_t i = 0; i < 5; i++)
    {
    }
    UNITY_END();
}