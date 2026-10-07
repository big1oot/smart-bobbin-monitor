#include <stdio.h>
#include <time.h>

enum SystemState {
    IDLE,
    MONITORING,
    CONFIRMING,
    ALERT
};

void printState(enum SystemState state)
{
    switch (state)
    {
        case IDLE:
            printf("State: IDLE\n");
            break;

        case MONITORING:
            printf("State: MONITORING\n");
            break;

        case CONFIRMING:
            printf("State: CONFIRMING\n");
            break;

        case ALERT:
            printf("State: ALERT\n");
            break;
    }
}

int main()
{
    enum SystemState currentState = IDLE;
    enum SystemState previousState = currentState;

    int machineRunning = 1;
    int threadActivity = 1;
    int resetEvent = 0;
    int alertIssued = 0;

    clock_t confirmationStart;

    printState(currentState);

    while (1)
    {
        switch (currentState)
        {
            case IDLE:
                if (machineRunning == 1)
                {
                    currentState = MONITORING;
                }
                break;


            case MONITORING:
                if (machineRunning == 0)
                {
                    currentState = IDLE;
                }
                else if (machineRunning == 1 && threadActivity == 0)
                {
                    confirmationStart = clock();
                    currentState = CONFIRMING;
                }
                break;


            case CONFIRMING:
            {
                double elapsedTime =
                    (double)(clock() - confirmationStart) / CLOCKS_PER_SEC;

                if (machineRunning == 0)
                {
                    currentState = IDLE;
                }
                else if (threadActivity == 1)
                {
                    currentState = MONITORING;
                }
                else if (elapsedTime >= 2.0)
                {
                    currentState = ALERT;
                }

                break;
            }


            case ALERT:
                if (alertIssued == 0)
                {
                    printf("CHECK YOUR BOBBIN!\n");
                    alertIssued = 1;
                }

                if (resetEvent == 1)
                {
                    alertIssued = 0;

                    if (machineRunning == 1)
                    {
                        currentState = MONITORING;
                    }
                    else
                    {
                        currentState = IDLE;
                    }
                }

                break;
        }


        /* Print only when the state changes */
        if (currentState != previousState)
        {
            printState(currentState);
            previousState = currentState;
        }
    }

    return 0;
}
