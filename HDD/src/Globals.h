#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

    #define DEBUG
    #define ASCII 128

    #define KB 1024
    #define MB 1048576
    #define GB 1073741824

    #define MS 0.001
    #define US 0.000001
    #define NS 0.000000001
    #define m_MS 1000
    #define m_US 1000000
    #define m_NS 1000000000

    #include <string>
/**
\033[22;30m - black             1
\033[22;31m - red               2
\033[22;32m - green             3
\033[22;33m - brown             4
\033[22;34m - blue              5
\033[22;35m - magenta           6
\033[22;36m - cyan              7
\033[22;37m - gray              8
\033[01;30m - dark gray         9
\033[01;31m - light red         10
\033[01;32m - light green       11
\033[01;33m - yellow            12
\033[01;34m - light blue        13
\033[01;35m - light magenta     14
\033[01;36m - light cyan        15
\033[01;37m - white             16
*/
    #define MYCOLORS 16
namespace myColors{
    extern std::string ConsoleColors_[MYCOLORS];
}
#endif // GLOBALS_H_INCLUDED
