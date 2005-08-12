/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to frankj@ifi.uio.no.
*/

#include <global.h>

#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <mmsystem.h>

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};


struct itimerval {
	struct timeval it_interval;	/* next value */
	struct timeval it_value;	/* current value */
};

#define ITIMER_REAL    0		/*generates sigalrm */
#define ITIMER_VIRTUAL 1		/*generates sigvtalrm */
#define ITIMER_VIRT    1		/*generates sigvtalrm */
#define ITIMER_PROF    2		/*generates sigprof */


/* Functions to capsule or serve linux style function
 * for Windows Visual C++ 
*/
int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
	/* Get the time, if they want it */
	if (time_Info != NULL) {
		time_Info->tv_sec = time(NULL);
		time_Info->tv_usec = timeGetTime()*1000;
	}
	/* Get the timezone, if they want it */
	if (timezone_Info != NULL) {
		_tzset();
		timezone_Info->tz_minuteswest = _timezone;
		timezone_Info->tz_dsttime = _daylight;
	}
	/* And return */
	return 0;
}

DIR *opendir(const char *dir)
{
	DIR *dp;
	char *filespec;
	long handle;
	int index;

	filespec = malloc(strlen(dir) + 2 + 1);
	strcpy(filespec, dir);
	index = strlen(filespec) - 1;
	if (index >= 0 && (filespec[index] == '/' || filespec[index] == '\\'))
		filespec[index] = '\0';
	strcat(filespec, "/*");

	dp = (DIR *) malloc(sizeof(DIR));
	dp->offset = 0;
	dp->finished = 0;
	dp->dir = strdup(dir);

	if ((handle = _findfirst(filespec, &(dp->fileinfo))) < 0) {
		free(filespec); free(dp);
		return NULL;
	}
	dp->handle = handle;
	free(filespec);

	return dp;
}

struct dirent *readdir(DIR * dp)
{
	if (!dp || dp->finished)
		return NULL;

	if (dp->offset != 0) {
		if (_findnext(dp->handle, &(dp->fileinfo)) < 0) {
			dp->finished = 1;
      if (ENOENT == errno)
        /* Clear error set to mean no more files else that breaks things */
        errno = 0;
			return NULL;
		}
	}
	dp->offset++;

	strncpy(dp->dent.d_name, dp->fileinfo.name, _MAX_FNAME);
	dp->dent.d_name[_MAX_FNAME] = '\0';
	dp->dent.d_ino = 1;
  /* reclen is used as meaning the length of the whole record */
	dp->dent.d_reclen = strlen(dp->dent.d_name) + sizeof(char) + sizeof(dp->dent.d_ino) + sizeof(dp->dent.d_reclen) + sizeof(dp->dent.d_off);
	dp->dent.d_off = dp->offset;

	return &(dp->dent);
}

int closedir(DIR * dp)
{
	if (!dp)
		return 0;
	_findclose(dp->handle);
	if (dp->dir)
		free(dp->dir);
	if (dp)
		free(dp);

	return 0;
}

void rewinddir(DIR *dir_Info)
{
	/* Re-set to the beginning */
	char *filespec;
	long handle;
	int index;

	dir_Info->handle = 0;
	dir_Info->offset = 0;
	dir_Info->finished = 0;

	filespec = malloc(strlen(dir_Info->dir) + 2 + 1);
	strcpy(filespec, dir_Info->dir);
	index = strlen(filespec) - 1;
	if (index >= 0 && (filespec[index] == '/' || filespec[index] == '\\'))
		filespec[index] = '\0';
	strcat(filespec, "/*");

	if ((handle = _findfirst(filespec, &(dir_Info->fileinfo))) < 0) {
		if (errno == ENOENT) {
			dir_Info->finished = 1;
		}
	}
	dir_Info->handle = handle;
	free(filespec);
}

/* Service-related stuff

  Those functions are called while init is still being done, so no logging available.

  Not useful for plugins, though.

 */

int bRunning;

#ifndef PYTHON_PLUGIN_EXPORTS

SERVICE_STATUS m_ServiceStatus;
SERVICE_STATUS_HANDLE m_ServiceStatusHandle;
#define SERVICE_NAME        "Crossfire"
#define SERVICE_DISPLAY     "Crossfire server"
#define SERVICE_DESCRIPTION "Crossfire is a multiplayer online RPG game."

#include <winsvc.h>

void service_register( )
    {
	char strDir[ 1024 ];
	HANDLE schSCManager,schService;
    char* strDescription = SERVICE_DESCRIPTION;

	GetModuleFileName( NULL, strDir, 1024 );
    strcat( strDir, " -srv" );

	schSCManager = OpenSCManager( NULL,NULL,SC_MANAGER_ALL_ACCESS );
 
	if (schSCManager == NULL)
        {
        printf( "openscmanager failed" );
		exit( 1 );
        }

    schService = CreateService(schSCManager, SERVICE_NAME, SERVICE_DISPLAY,           // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        strDir,        // service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 
 
    if (schService == NULL) 
        {
        printf( "createservice failed" );
        exit( 1 );
        }

    ChangeServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, &strDescription );

    CloseServiceHandle(schService);
    CloseServiceHandle( schSCManager );
    exit( 0 );
    }

void service_unregister( )
    {
	HANDLE schSCManager;
	SC_HANDLE hService;

	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
 
	if (schSCManager == NULL)
        {
        printf( "open failed" );
		exit( 1 );
        }

	hService=OpenService(schSCManager, SERVICE_NAME, SERVICE_ALL_ACCESS);

	if (hService == NULL) 
        {
        printf( "openservice failed" );
		exit( 1 );
        }

	if(DeleteService(hService)==0)
        {
        printf( "Delete failed" );
		exit( 1 );
        }

	if(CloseServiceHandle(hService)==0)
        {
        printf( "close failed" );
		exit( 1 );
        }

    if ( !CloseServiceHandle( schSCManager ) )
        {
        printf( "close schSCManager failed" );
        exit( 1 );
        }

    exit( 0 );
    }

void WINAPI ServiceCtrlHandler(DWORD Opcode)
    {
    switch(Opcode) 
        {
        case SERVICE_CONTROL_PAUSE: 
            m_ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
            m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
            break; 

        case SERVICE_CONTROL_STOP: 
            m_ServiceStatus.dwWin32ExitCode = 0; 
            m_ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
            m_ServiceStatus.dwCheckPoint    = 0; 
            m_ServiceStatus.dwWaitHint      = 0; 
 
            SetServiceStatus (m_ServiceStatusHandle,&m_ServiceStatus);

			bRunning = 0;

            LOG( llevInfo, "Service stopped.\n" );

			break;
 
        case SERVICE_CONTROL_INTERROGATE: 
            break; 
        }      
    return; 
    }

extern int main( int argc, char** argv );

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
    {
	char strDir[ 1024 ];
    char* strSlash;

	GetModuleFileName( NULL, strDir, 1024 );
    strSlash = strrchr( strDir, '\\' );
    if ( strSlash )
        *strSlash = '\0';
    chdir( strDir );

    m_ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    m_ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    m_ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP; 
    m_ServiceStatus.dwWin32ExitCode      = 0; 
    m_ServiceStatus.dwServiceSpecificExitCode = 0; 
    m_ServiceStatus.dwCheckPoint         = 0; 
    m_ServiceStatus.dwWaitHint           = 0; 
 
    m_ServiceStatusHandle = RegisterServiceCtrlHandler( SERVICE_NAME, ServiceCtrlHandler );
    if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
        { 
        return; 
        }     

    m_ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    m_ServiceStatus.dwCheckPoint         = 0; 
    m_ServiceStatus.dwWaitHint           = 0;  
    SetServiceStatus (m_ServiceStatusHandle, &m_ServiceStatus);

    bRunning = 1;
    main( 0, NULL );

    return;
    }

void service_handle( )
    {
    SERVICE_TABLE_ENTRY DispatchTable[ ] = { { SERVICE_NAME, ServiceMain },{ NULL, NULL } };  
	StartServiceCtrlDispatcher( DispatchTable );
    exit( 0 );
    }
#endif /* PYTHON_PLUGIN_EXPORTS */
