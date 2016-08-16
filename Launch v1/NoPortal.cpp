#include <windows.h>

#define GAME_PATH	"NoPortal"

typedef void (*pfnChangeGame)( const char *progname );
typedef int (*pfnInit)( const char *progname, int bChangeGame, pfnChangeGame func );
typedef void (*pfnShutdown)( void );

pfnInit Host_Main;
pfnShutdown Host_Shutdown = NULL;
char szGameDir[128];
HINSTANCE	hEngine;

void Sys_Error( const char *errorstring )
{
	MessageBox( NULL, errorstring, "NoPortal Error", MB_OK|MB_SETFOREGROUND|MB_ICONSTOP );
	exit( 1 );
}

void Sys_LoadEngine( void )
{
	if(( hEngine = LoadLibrary( "xash.dll" )) == NULL )
	{
		Sys_Error( "Unable to load the xash.dll" );
	}

	if(( Host_Main = (pfnInit)GetProcAddress( hEngine, "Host_Main" )) == NULL )
	{
		Sys_Error( "xash.dll missed 'Host_Main' export" );
	}

	Host_Shutdown = (pfnShutdown)GetProcAddress( hEngine, "Host_Shutdown" );
}

void Sys_UnloadEngine( void )
{
	if( Host_Shutdown ) Host_Shutdown( );
	if( hEngine ) FreeLibrary( hEngine );
}

void Sys_ChangeGame( const char *progname )
{
	if( !progname || !progname[0] ) Sys_Error( "Sys_ChangeGame: NULL gamedir" );
	if( Host_Shutdown == NULL ) Sys_Error( "Sys_ChangeGame: missed 'Host_Shutdown' export\n" );
	strncpy( szGameDir, progname, sizeof( szGameDir ) - 1 );

	Sys_UnloadEngine ();
	Sys_LoadEngine ();

	Host_Main( szGameDir, TRUE, ( Host_Shutdown != NULL ) ? Sys_ChangeGame : NULL );
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	Sys_LoadEngine();

	return Host_Main( GAME_PATH, FALSE, ( Host_Shutdown != NULL ) ? Sys_ChangeGame : NULL );
}
