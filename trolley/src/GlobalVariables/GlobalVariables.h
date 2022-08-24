#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include "Arduino.h"

class GlobalVariables
{
public:
    static unsigned short bleClients;
    static bool trolleyEnabled;
    static char *trolleyName;
    static char *trolleyId;
    static String coordinates;
};

#endif