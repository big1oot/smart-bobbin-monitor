#include <stdio.h>

enum SystemState {
    IDLE,
    MONITORING,
    CONFIRMING,
    ALERT
};

/* Convert a state into readable text */
const char *stateName(enum SystemState state)
{
    switch (state)
    {
        case IDLE:
            return "IDLE";

        case MONITORING:
            return "MONITORING";

        case CONFIRMING:
            return "CONFIRMING";

        case ALERT:
            return "ALERT";

        default:
            return "UNKNOWN";
    }
}


/* Main state transition logic */
void transition(enum SystemState *state,
                int machineRunning,
                int threadActivity,
                int resetEvent,
                double confirmingTime)
{
    switch (*state)
    {
        case IDLE:

            /* Start monitoring when sewing begins */
            if (machineRunning == 1)
            {
                *state = MONITORING;
            }

            break;


        case MONITORING:

            /* Machine stopped */
            if (machineRunning == 0)
            {
                *state = IDLE;
            }

            /* Machine is sewing but bobbin thread activity disappeared */
            else if (threadActivity == 0)
            {
                *state = CONFIRMING;
            }

            break;


        case CONFIRMING:

            /* Sewing stopped during confirmation */
            if (machineRunning == 0)
            {
                *state = IDLE;
            }

            /* Thread activity returned */
            else if (threadActivity == 1)
            {
                *state = MONITORING;
            }

            /* Machine still sewing and no thread for at least 2 seconds */
            else if (confirmingTime >= 2.0)
            {
                *state = ALERT;
            }

            break;


        case ALERT:

            /*
             * Thread activity alone DOES NOT clear ALERT.
             * A deliberate reset is required.
             */
            if (resetEvent == 1)
            {
                if (machineRunning == 1)
                {
                    *state = MONITORING;
                }
                else
                {
                    *state = IDLE;
                }
            }

            break;
    }
}


/* Run one simulated step and print state changes */
void runStep(enum SystemState *state,
             int machineRunning,
             int threadActivity,
             int resetEvent,
             double confirmingTime)
{
    enum SystemState previousState = *state;

    transition(state,
               machineRunning,
               threadActivity,
               resetEvent,
               confirmingTime);

    if (*state != previousState)
    {
        printf("State: %s -> %s\n",
               stateName(previousState),
               stateName(*state));
    }
}


int main()
{
    enum SystemState currentState;


    /* =========================================================
       TEST 1
       Temporary thread interruption should NOT cause an alert
       ========================================================= */

    printf("\n=== TEST 1: TEMPORARY INTERRUPTION ===\n");

    currentState = IDLE;

    /* Machine starts sewing normally */
    runStep(&currentState, 1, 1, 0, 0);

    /* Thread stops */
    runStep(&currentState, 1, 0, 0, 0);

    /* Only 1 second has passed */
    runStep(&currentState, 1, 0, 0, 1.0);

    /* Thread returns before 2 seconds */
    runStep(&currentState, 1, 1, 0, 1.0);

    if (currentState == MONITORING)
    {
        printf("RESULT: PASS - No false alert\n");
    }
    else
    {
        printf("RESULT: FAIL\n");
    }



    /* =========================================================
       TEST 2
       Continuous loss of thread for 2 seconds should cause ALERT
       ========================================================= */

    printf("\n=== TEST 2: BOBBIN FAILURE ===\n");

    currentState = MONITORING;

    /* Thread disappears */
    runStep(&currentState, 1, 0, 0, 0);

    /* Two seconds pass with machine still sewing */
    runStep(&currentState, 1, 0, 0, 2.0);

    if (currentState == ALERT)
    {
        printf("CHECK YOUR BOBBIN!\n");
        printf("RESULT: PASS - Alert issued\n");
    }
    else
    {
        printf("RESULT: FAIL\n");
    }



    /* =========================================================
       TEST 3
       Machine stops during confirmation
       System should cancel the fault and return to IDLE
       ========================================================= */

    printf("\n=== TEST 3: MACHINE STOPS DURING CONFIRMATION ===\n");

    currentState = MONITORING;

    /* Thread disappears while sewing */
    runStep(&currentState, 1, 0, 0, 0);

    /* Machine stops before confirmation finishes */
    runStep(&currentState, 0, 0, 0, 1.0);

    if (currentState == IDLE)
    {
        printf("RESULT: PASS - Confirmation cancelled\n");
    }
    else
    {
        printf("RESULT: FAIL\n");
    }



    /* =========================================================
       TEST 4
       Thread returning AFTER ALERT must NOT clear the alert
       ========================================================= */

    printf("\n=== TEST 4: ALERT REMAINS LATCHED ===\n");

    currentState = MONITORING;

    /* Thread disappears */
    runStep(&currentState, 1, 0, 0, 0);

    /* Two seconds pass */
    runStep(&currentState, 1, 0, 0, 2.0);

    /* Thread starts moving again, but NO reset */
    runStep(&currentState, 1, 1, 0, 3.0);

    if (currentState == ALERT)
    {
        printf("RESULT: PASS - Alert remained active\n");
    }
    else
    {
        printf("RESULT: FAIL - Alert cleared without reset\n");
    }



    /* =========================================================
       TEST 5
       Reset while machine is still running
       Should return to MONITORING
       ========================================================= */

    printf("\n=== TEST 5: RESET WHILE MACHINE RUNNING ===\n");

    /* We deliberately start this test already in ALERT */
    currentState = ALERT;

    /* User acknowledges alert */
    runStep(&currentState, 1, 1, 1, 0);

    if (currentState == MONITORING)
    {
        printf("RESULT: PASS - Returned to monitoring\n");
    }
    else
    {
        printf("RESULT: FAIL\n");
    }



    /* =========================================================
       TEST 6
       Reset while machine is stopped
       Should return to IDLE
       ========================================================= */

    printf("\n=== TEST 6: RESET WHILE MACHINE STOPPED ===\n");

    currentState = ALERT;

    /* User acknowledges alert while machine is stopped */
    runStep(&currentState, 0, 0, 1, 0);

    if (currentState == IDLE)
    {
        printf("RESULT: PASS - Returned to idle\n");
    }
    else
    {
        printf("RESULT: FAIL\n");
    }



    printf("\n=== ALL TESTS COMPLETE ===\n");

    return 0;
}
