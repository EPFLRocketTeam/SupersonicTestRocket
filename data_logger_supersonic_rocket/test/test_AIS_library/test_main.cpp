#include <Arduino.h>
#include <unity.h>

#include "AISx120SX.h"
#include "CRC.h"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }


// test is commented out since the interface is private
// void test_AISx120SX_calculateEvenParity(void)
// {
//     AISx120SX aisObject = AISx120SX(0);

//     uint8_t array[4] = {0};
//     bool parity = true;

//     TEST_ASSERT_EQUAL(parity,
//                       !aisObject.calculateEvenParity(array, sizeof(array)));
//     for (size_t i = 0; i < 8; i++)
//     {
//         for (size_t j = 0; j < 4; j++)
//         {
//             bitSet(array[j], i);
//             parity = !parity;
//             TEST_ASSERT_EQUAL(parity,
//                               !aisObject.calculateEvenParity(array, sizeof(array)));
//         }
//     }
// }

void test_AISx120SX_CRC(void)
{   
    uint8_t test[] = {0x0, 0x0};
    TEST_ASSERT_EQUAL(0x0, Fast_CRC_Cal8Bits(0x0, sizeof(test), test));
    uint8_t test1[] = {0x0, 0x8};
    TEST_ASSERT_EQUAL(0x57, Fast_CRC_Cal8Bits(0x0, sizeof(test1), test1));
    uint8_t test2[] = {0x0, 0x10};
    TEST_ASSERT_EQUAL(0xae, Fast_CRC_Cal8Bits(0x0, sizeof(test2), test2));
    uint8_t test3[] = {0x0, 0xFF};
    TEST_ASSERT_EQUAL(0x42, Fast_CRC_Cal8Bits(0x0, sizeof(test3), test3));

}

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
    //RUN_TEST(test_AISx120SX_calculateEvenParity);
    RUN_TEST(test_AISx120SX_CRC);
}

// tests to run multiple times
void loop()
{
    for (size_t i = 0; i < 5; i++)
    {
    }
    UNITY_END();
}