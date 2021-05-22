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

void test_PushButtonArray_addEvent(void)
{
    PushButtonArray buttonArray = PushButtonArray();
    uint8_t newEvent0 = buttonArray.addEvent(0, 1, 500, 1500);
    uint8_t newEvent1 = buttonArray.addEvent(1, 0, -500, 1500);
    uint8_t newEvent2 = buttonArray.addEvent(2, 3, 500, 0);
    uint8_t newEvent3 = buttonArray.addEvent(-1, 55, 5000, 1500);
    uint8_t newEvent4 = buttonArray.addEvent(7, 32, 500, 1500);
    TEST_ASSERT_EQUAL(0, newEvent0);
    TEST_ASSERT_EQUAL(1, newEvent1);
    TEST_ASSERT_EQUAL(2, newEvent2);
    TEST_ASSERT_EQUAL(3, newEvent3);
    TEST_ASSERT_EQUAL(4, newEvent4);
}

void test_PushButtonArray_checkSequence(void)
{
    const int STOP_STATES[3] = {1, 0, 2};                     // States to stop
    const int NEXT_STOP_STATES[3] = {0, 2, 0};                // Next states
    const uint32_t STOP_WINDOW_START[3] = {3000, 5000, 7000}; // [ms]
    const uint32_t STOP_WINDOW_END[3] = {4000, 6000, 8000};   // [ms]
    PushButtonArray buttonArray = PushButtonArray();          // create the button array

    
    bool bothLow[2] = {false, false};
    bool zerothHigh[2] = {true, false};
    bool firstHigh[2] = {false, true};
    bool bothhigh[2] = {true, true};

    // setup the events
    uint8_t stopEvent[3] = {
        buttonArray.addEvent(STOP_STATES[0], NEXT_STOP_STATES[0],
                             STOP_WINDOW_START[0], STOP_WINDOW_END[0]),
        buttonArray.addEvent(STOP_STATES[1], NEXT_STOP_STATES[1],
                             STOP_WINDOW_START[1], STOP_WINDOW_END[1]),
        buttonArray.addEvent(STOP_STATES[2], NEXT_STOP_STATES[0],
                             STOP_WINDOW_START[2], STOP_WINDOW_END[2])};
    // deactivate the second and third events for now
    buttonArray.deactivateEvent(stopEvent[1]);
    buttonArray.deactivateEvent(stopEvent[2]);

    eventOutput event;
    event = buttonArray.checkEvents(1000, zerothHigh);
    TEST_ASSERT_EQUAL(0xFF, event.triggeredEvent);
    TEST_ASSERT_EQUAL(NONE, event.triggeredEventType);
    event = buttonArray.checkEvents(4000, zerothHigh);
    TEST_ASSERT_EQUAL(stopEvent[0], event.triggeredEvent);
    TEST_ASSERT_EQUAL(WINDOW_START, event.triggeredEventType);
    event = buttonArray.checkEvents(4999, bothLow);
    TEST_ASSERT_EQUAL(stopEvent[0], event.triggeredEvent);
    TEST_ASSERT_EQUAL(GOOD_TRANSITION, event.triggeredEventType);
    buttonArray.deactivateEvent(stopEvent[0]);
    buttonArray.activateEvent(stopEvent[1]);
    event = buttonArray.checkEvents(5000, bothLow);
    TEST_ASSERT_EQUAL(0xFF, event.triggeredEvent);
    TEST_ASSERT_EQUAL(NONE, event.triggeredEventType);
    event = buttonArray.checkEvents(10000, bothLow);
    TEST_ASSERT_EQUAL(stopEvent[1], event.triggeredEvent);
    TEST_ASSERT_EQUAL(WINDOW_START, event.triggeredEventType);
    event = buttonArray.checkEvents(10999, firstHigh);
    TEST_ASSERT_EQUAL(stopEvent[1], event.triggeredEvent);
    TEST_ASSERT_EQUAL(GOOD_TRANSITION, event.triggeredEventType);
    buttonArray.deactivateEvent(stopEvent[1]);
    buttonArray.activateEvent(stopEvent[2]);
    event = buttonArray.checkEvents(11000, firstHigh);
    TEST_ASSERT_EQUAL(0xFF, event.triggeredEvent);
    TEST_ASSERT_EQUAL(NONE, event.triggeredEventType);
    event = buttonArray.checkEvents(18000, firstHigh);
    TEST_ASSERT_EQUAL(stopEvent[2], event.triggeredEvent);
    TEST_ASSERT_EQUAL(WINDOW_START, event.triggeredEventType);
    event = buttonArray.checkEvents(18999, bothLow);
    TEST_ASSERT_EQUAL(stopEvent[2], event.triggeredEvent);
    TEST_ASSERT_EQUAL(GOOD_TRANSITION, event.triggeredEventType);
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
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(1, 0, 25000));
    TEST_ASSERT_EQUAL(NONE, buttonArrayEvent.checkEvent(0, 1, 25000));
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
    RUN_TEST(test_PushButtonArray_addEvent);
    RUN_TEST(test_PushButtonArray_checkSequence);
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