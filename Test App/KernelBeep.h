#ifndef KERNELBEEP_H
#define KERNELBEEP_H

#include <Windows.h>

class KernelBeep
{
public:
    KernelBeep( );
    ~KernelBeep( );

    static KernelBeep &GetInstance( );

    bool CanBeUsed( );
    bool Beep( DWORD dwFrequency, DWORD dwDuration );

private:
    HANDLE m_hDriverHandle;
};

#endif