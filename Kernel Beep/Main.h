#ifndef MAIN_H
#define MAIN_H

#include <ntddk.h>

#pragma warning(disable:4055) // Cast PVOID to func ptr

#define DRIVER_DEVICE_NAME		L"\\Device\\BeepDriver"
#define DRIVER_DOS_DEVICE_NAME	L"\\DosDevices\\BeepDriver"

#define DRV_TYPE		0x8000 
#define IOCTL_MAKEBEEP	CTL_CODE(DRV_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

NTSTATUS DriverEntry( DRIVER_OBJECT *DriverObject, UNICODE_STRING *RegistryPath );
NTSTATUS DriverDispatch( DEVICE_OBJECT *DeviceObject, IRP *Irp );
void DriverUnload( DRIVER_OBJECT *DriverObject );
BOOLEAN DoBeep( ULONG Frequency, ULONG Duration );

typedef BOOLEAN( NTAPI *tHalMakeBeep )( ULONG Frequency );
extern tHalMakeBeep _HalMakeBeep;

typedef struct
{
    unsigned long m_Frequency;
    unsigned long m_Duration;
} BeepRequest;

#endif