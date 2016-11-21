#include "Main.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, DriverDispatch)
#pragma alloc_text (PAGE, DriverUnload)
#pragma alloc_text (PAGE, DoBeep)
#endif

tHalMakeBeep _HalMakeBeep = NULL;

BOOLEAN DoBeep( ULONG Frequency, ULONG Duration )
{
    BOOLEAN result = FALSE;
    LARGE_INTEGER delay;

    delay.QuadPart = Int32x32To64( Duration, -10000 );

    result = _HalMakeBeep( Frequency );

    KeDelayExecutionThread( KernelMode, FALSE, &delay );

    _HalMakeBeep( 0 );

    return result;
}

NTSTATUS DriverEntry( DRIVER_OBJECT *DriverObject, UNICODE_STRING *RegistryPath )
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING deviceName = { 0 };
    UNICODE_STRING dosDeviceName = { 0 };
    UNICODE_STRING routineName = { 0 };
    DEVICE_OBJECT *deviceObject = NULL;

    UNREFERENCED_PARAMETER( RegistryPath );

    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDispatch;

    RtlInitUnicodeString( &deviceName, DRIVER_DEVICE_NAME );
    RtlInitUnicodeString( &dosDeviceName, DRIVER_DOS_DEVICE_NAME );

    status = IoCreateDevice( DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject );
    if ( !NT_SUCCESS( status ) )
        return status;

    deviceObject->Flags |= DO_BUFFERED_IO;
    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    status = IoCreateSymbolicLink( &dosDeviceName, &deviceName );
    if ( !NT_SUCCESS( status ) )
        return status;

    RtlInitUnicodeString( &routineName, L"HalMakeBeep" );

    PVOID pHalMakeBeepAddress = MmGetSystemRoutineAddress( &routineName );	// I experienced a system crash on some systems if we link this in the IAT, so we have to do this manually
    if ( pHalMakeBeepAddress == NULL )
        return STATUS_NOT_SUPPORTED;

    _HalMakeBeep = ( tHalMakeBeep )pHalMakeBeepAddress;

    return status;
}

void DriverUnload( DRIVER_OBJECT *DriverObject )
{
    UNICODE_STRING dosDeviceName;

    if ( DriverObject )
    {
        RtlInitUnicodeString( &dosDeviceName, DRIVER_DOS_DEVICE_NAME );
        IoDeleteSymbolicLink( &dosDeviceName );
        IoDeleteDevice( DriverObject->DeviceObject );
    }
}


NTSTATUS DriverDispatch( DEVICE_OBJECT *DeviceObject, IRP *Irp )
{
    NTSTATUS status = STATUS_SUCCESS;
    IO_STACK_LOCATION *stackLocation = NULL;

    UNREFERENCED_PARAMETER( DeviceObject );

    __try
    {
        stackLocation = IoGetCurrentIrpStackLocation( Irp );

        if ( stackLocation->MajorFunction == IRP_MJ_DEVICE_CONTROL )
        {
            Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;

            if ( stackLocation->Parameters.DeviceIoControl.IoControlCode == IOCTL_MAKEBEEP )
            {
                BeepRequest *pRequest = ( BeepRequest* )( Irp->AssociatedIrp.SystemBuffer );

                if ( pRequest && stackLocation->Parameters.DeviceIoControl.InputBufferLength == sizeof( BeepRequest ) )
                {
                    if ( DoBeep( pRequest->m_Frequency, pRequest->m_Duration ) )
                    {
                        Irp->IoStatus.Status = STATUS_SUCCESS;
                    }
                }
            }
        }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
    }

    status = Irp->IoStatus.Status;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );

    return status;
}


