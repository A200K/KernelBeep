#include <iostream>
#include "KernelBeep.h"

int main( int argc, char *argv[] )
{
    printf( "Kernel Beep Test\n" );

    if ( KernelBeep::GetInstance( ).CanBeUsed( ) )
    {
        printf( "Press enter to beep\n" );

        while ( true )
        {
            getchar( );

            ULONG frequency = 200 + ( rand( ) % ( 4000 - 200 ) );

            if ( KernelBeep::GetInstance( ).Beep( frequency, 300 ) )
            {
                printf( "Frequency: %i\n", frequency );
            }
            else
            {
                printf( "Something went wrong\n" );
            }
        }
    }
    else
    {
        printf( "Can't connect to beep driver\n" );
        getchar( );
    }

    return EXIT_SUCCESS;
}