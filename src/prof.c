#include <3ds.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ProfileEntryCount 512

struct ProfileEntry {
    uint32_t Address;
    uint32_t Calls;
    uint64_t Ticks;
};

static struct ProfileEntry ProfileFnTable[ ProfileEntryCount ];

uint64_t GlobalTicks = 0;
uint64_t EnterTime = 0;
uint64_t ExitTime = 0;

static bool ProfileIsReady = false;

void __cyg_profile_func_enter( void* ThisFn, void* CallSite ) __attribute__( ( no_instrument_function ) );
void __cyg_profile_func_exit( void* ThisFn, void* CallSite ) __attribute__( ( no_instrument_function ) );
struct ProfileEntry* GetFunctionHandle( uint32_t Address ) __attribute__( ( no_instrument_function ) );
const char* GetFunctionName( uint32_t* Address ) __attribute__( ( no_instrument_function ) );
void Profile_Start( void ) __attribute__( ( no_instrument_function ) );
void Profile_Stop( void ) __attribute__( ( no_instrument_function ) );

struct ProfileEntry* GetFunctionHandle( uint32_t Address ) {
    struct ProfileEntry* Entry = NULL;
    int FirstFree = 0;
    int i = 0;

    for ( i = 0; i < ProfileEntryCount; i++ ) {
        Entry = &ProfileFnTable[ i ];

        if ( Entry->Address == Address ) {
            return Entry;
        }

        if ( Entry->Address == 0 && FirstFree == 0 ) {
            FirstFree = i;
        }
    }

    Entry = &ProfileFnTable[ FirstFree ];
    Entry->Address = Address;

    return &ProfileFnTable[ FirstFree ];
}

void __cyg_profile_func_enter( void* ThisFn, void* CallSite ) {
    struct ProfileEntry* Entry = NULL;
    
    if ( ProfileIsReady ) {
        Entry = GetFunctionHandle( ( uint32_t ) ThisFn );

        if ( Entry ) {
            EnterTime = svcGetSystemTick( );
            Entry->Calls++;
        }
    }
}

void __cyg_profile_func_exit( void* ThisFn, void* CallSite ) {
    struct ProfileEntry* Entry = NULL;
    uint64_t Ticks = 0;
    
    if ( ProfileIsReady ) {
        Entry = GetFunctionHandle( ( uint32_t ) ThisFn );

        if ( Entry ) {
            ExitTime = svcGetSystemTick( );
            Ticks = ExitTime - EnterTime;

            GlobalTicks+= Ticks;
            Entry->Ticks+= Ticks;
        }
    }
}

const char* GetFunctionName( uint32_t* Address ) {
    uint8_t* Ptr = ( uint8_t* ) Address;
    uint32_t Length = 0;
    uint32_t Tag = 0;

    if ( Ptr ) {
        Ptr-= 4;

        Tag = *( ( uint32_t* ) Ptr ) & 0xFF000000;
        Length = *( ( uint32_t* ) Ptr ) & 0xFF;

        if ( Tag == 0xFF000000 ) {
            return ( const char* ) ( Ptr - Length );
        }
    }

    return "Unknown";
}

void Profile_Start( void ) {
    memset( ProfileFnTable, 0, sizeof( ProfileFnTable ) );

    GlobalTicks = 0;
    EnterTime = 0;
    ExitTime = 0;

    ProfileIsReady = true;
}

void Profile_Stop( void ) {
    struct ProfileEntry* Entry = NULL;
    FILE* fp = NULL;
    uint32_t i = 0;

    ProfileIsReady = false;

    fp = fopen( "prof.csv", "wt+" );

    if ( fp ) {
        fprintf( fp, "Address,Name,Calls,Ticks,TicksAVG,TimePCT\n" );

        for ( i = 0; i < ProfileEntryCount; i++ ) {
            Entry = &ProfileFnTable[ i ];

            if ( Entry->Address && Entry->Calls ) {
                fprintf(
                    fp,
                    "%p,%s,%" PRIu32 ",%" PRIu64 ",%" PRIu64 ",%.2f\n",
                    ( void* ) Entry->Address,
                    GetFunctionName( ( uint32_t* ) Entry->Address ),
                    Entry->Calls,
                    Entry->Ticks,
                    ( Entry->Ticks / Entry->Calls ),
                    ( ( ( double ) Entry->Ticks ) / ( ( double ) GlobalTicks ) ) * 100.0f
                );
            }
        }

        fclose( fp );
    }
}
