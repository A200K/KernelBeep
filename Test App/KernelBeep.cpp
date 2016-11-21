#include "KernelBeep.h"

#define DRIVER_HANDLE_NAME	L"\\\\.\\BeepDriver"
#define FILE_DRV_BASE		0x8000 
#define IOCTL_MAKEBEEP		CTL_CODE(FILE_DRV_BASE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    unsigned long m_Frequency;
    unsigned long m_Duration;
} BeepRequest;

KernelBeep::KernelBeep( )
{
    this->m_hDriverHandle = CreateFileW( DRIVER_HANDLE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL );
}

KernelBeep::~KernelBeep( )
{
    if ( this->m_hDriverHandle != INVALID_HANDLE_VALUE )
    {
        CloseHandle( this->m_hDriverHandle );
    }
}

KernelBeep &KernelBeep::GetInstance( )
{
    static KernelBeep instance;
    return instance;
}

bool KernelBeep::CanBeUsed( )
{
    return this->m_hDriverHandle != INVALID_HANDLE_VALUE;
}

bool KernelBeep::Beep( DWORD dwFrequency, DWORD dwDuration )
{
    if ( !this->CanBeUsed( ) )
        return ::Beep( dwFrequency, dwDuration ); // Native Windows Beep

    BeepRequest req;
    req.m_Frequency = dwFrequency;
    req.m_Duration = dwDuration;

    return DeviceIoControl( this->m_hDriverHandle, IOCTL_MAKEBEEP, &req, sizeof( BeepRequest ), NULL, 0, NULL, NULL );;
}