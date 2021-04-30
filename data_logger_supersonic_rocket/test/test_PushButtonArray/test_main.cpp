#include <Arduino.h>
#include <unity.h>

#include "PushButtonArray.h"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

// PushButtonArray tests -------------------------------------------------------
void test_PushButtonArray_translateState(void)
{
    PushButtonArray buttonArray = PushButtonArray();
    bool input[2] = {false, false};
    TEST_ASSERT_EQUAL(0, buttonArray.translateState(input));
    input[0] = true;
    TEST_ASSERT_EQUAL(1, buttonArray.translateState(input));
    input[1] = true;
    TEST_ASSERT_EQUAL(3, buttonArray.translateState(input));
    input[0] = false;
    TEST_ASSERT_EQUAL(2, buttonArray.translateState(input));
}

// PushButtonArrayEvent tests --------------------------------------------------
void test_PushButtonArrayEvent_constructor(void)
{
    PushButtonArrayEvent buttonArrayEvent = PushButtonArrayEvent();
    TEST_ASSERT_EQUAL(1, buttonArrayEvent.deletedStatus());
}

void test_PushButtonArrayEvent_activateEvent(void)
{
    PushButtonArrayEvent buttonArrayEvent = PushButtonArrayEvent();
    buttonArrayEvent.activateEvent();
    TEST_ASSERT_EQUAL(1, buttonArrayEvent.activatedStatus());
}

void test_PushButtonArrayEvent_deactivateEvent(void)
{
    PushButtonArrayEvent buttonArrayEvent = PushButtonArrayEvent();
    buttonArrayEvent.activateEvent();
    TEST_ASSERT_EQUAL(1, buttonArrayEvent.activatedStatus());
    buttonArrayEvent.deactivateEvent();
    TEST_ASSERT_EQUAL(0, buttonArrayEvent.activatedStatus());
}

void test_PushButtonArrayEvent_createEvent(void)
{
    PushButtonArrayEvent buttonArrayEvent = PushButtonArrayEvent();
    buttonArrayEvent.createEvent(0, 1, 500, 1000);
    TEST_ASSERT_EQUAL(0, buttonArrayEvent.deletedStatus());
    TEST_ASSERT_EQUAL(1, buttonArrayEvent.activatedStatus());
}

void test_PushButtonArrayEvent_deleteEvent(void)
{
    PushButtonArrayEvent buttonArrayEvent = PushButtonArrayEvent();
    buttonArrayEvent.createEvent(0, 1, 500, 1000);
    buttonArrayEvent.deleteEvent();
    TEST_ASSERT_EQUAL(1, buttonArrayEvent.deletedStatus());
    TEST_ASSERT_EQUAL(0, buttonArrayEvent.activatedStatus());
}

void test_PushButtonArrayEvent_checkEvent(void)
{
    PushButtonArrayEvent buttonArrayEvent = PushButtonArrayEvent();
    buttonArrayEvent.createEvent(0, 1, 500, 1000);
    TEST_ASSERT_EQUAL(WINDOW_START, buttonArrayEvent.checkEvent(0, 0, 500));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 0, 500));
    TEST_ASSERT_EQUAL(GOOD_TRANSITION, buttonArrayEvent.checkEvent(0, 1, 500));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 1, 500));
    TEST_ASSERT_EQUAL(BAD_TRANSITION, buttonArrayEvent.checkEvent(1, 0, 500));
    TEST_ASSERT_EQUAL(GOOD_TRANSITION, buttonArrayEvent.checkEvent(0, 1, 750));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 1, 750));
    TEST_ASSERT_EQUAL(WINDOW_END, buttonArrayEvent.checkEvent(0, 0, 1000));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 0, 1000));
    TEST_ASSERT_EQUAL(GOOD_TRANSITION, buttonArrayEvent.checkEvent(0, 1, 1000));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 1, 1000));
    TEST_ASSERT_EQUAL(BAD_TRANSITION, buttonArrayEvent.checkEvent(1, 0, 1000));
    TEST_ASSERT_EQUAL(NONE,
                      buttonArrayEvent.checkEvent(0, 0,
                                                  250U - (UINT_MAX - 248U)));
    TEST_ASSERT_EQUAL(WINDOW_START,
                      buttonArrayEvent.checkEvent(0, 0,
                                                  250U - (UINT_MAX - 249U)));
    TEST_ASSERT_EQUAL(GOOD_TRANSITION,
                      buttonArrayEvent.checkEvent(0, 1,
                                                  250U - (UINT_MAX - 500U)));
    TEST_ASSERT_EQUAL(NONE,
                      buttonArrayEvent.checkEvent(0, 0,
                                                  500U - (UINT_MAX - 498U)));
    TEST_ASSERT_EQUAL(WINDOW_END,
                      buttonArrayEvent.checkEvent(0, 0,
                                                  500U - (UINT_MAX - 499U)));
    buttonArrayEvent.deactivateEvent();
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 0, 500));
    buttonArrayEvent.activateEvent();
    TEST_ASSERT_EQUAL(WINDOW_START, buttonArrayEvent.checkEvent(0, 0, 500));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 0, 500));
    buttonArrayEvent.createEvent(1, 0, 50000, 4500);
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 0, 5000));
    TEST_ASSERT_EQUAL(BAD_TRANSITION, buttonArrayEvent.checkEvent(1, 0, 25000));
    TEST_ASSERT_EQUAL(BAD_TRANSITION, buttonArrayEvent.checkEvent(0, 1, 25000));
    buttonArrayEvent.createEvent(0, 1, 500, 1000);
    TEST_ASSERT_EQUAL(WINDOW_START, buttonArrayEvent.checkEvent(0, 0, 500));
    TEST_ASSERT_EQUAL(GOOD_TRANSITION, buttonArrayEvent.checkEvent(0, 1, 1000));
    buttonArrayEvent.deleteEvent();
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 0, 500));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(1, 0, 1000));
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
    RUN_TEST(test_PushButtonArray_translateState);
    RUN_TEST(test_PushButtonArrayEvent_constructor);
    RUN_TEST(test_PushButtonArrayEvent_createEvent);
    RUN_TEST(test_PushButtonArrayEvent_deleteEvent);
    RUN_TEST(test_PushButtonArrayEvent_checkEvent);
}

// tests to run multiple times
void loop()
{
    for (size_t i = 0; i < 5; i++)
    {
    }
    UNITY_END();
}