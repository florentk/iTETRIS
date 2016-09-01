// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <time.h>
#include "utilities.h"
#include "sync-manager.h"
#include "utils/common/TplConvert.h"

namespace utils
{

time_t Conversion::m_startTime;
time_t Conversion::m_endTime;

std::string
Conversion::int2String(int intValue)
{
    std::stringstream out;
    out << intValue;
    return out.str();
}

int
Conversion::string2Int(std::string stringValue)
{
    int rValue;

    rValue = TplConvert<char>::_2int(stringValue.c_str());

    return rValue;
}

bool
Conversion::Wait(std::string message, int timeStep)
{
    if (ics::SyncManager::m_simStep == timeStep) {
        std::cout << message;
        std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
        return true;
    }

    return false;
}

void
Conversion::Wait(std::string message)
{
    std::cout << message;
    std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
}

double
Conversion::GetElapsedTime()
{
    return difftime(m_endTime, m_startTime);
}

}
