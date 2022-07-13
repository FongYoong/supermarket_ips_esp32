#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

class GlobalVariables
{
public:
    static unsigned short bleClients;
    static bool trolleyEnabled;
    static unsigned long locationUpdateInterval; // Interval between logs // Milliseconds
    static char *trolleyName;
    static char *trolleyId;
};

#endif