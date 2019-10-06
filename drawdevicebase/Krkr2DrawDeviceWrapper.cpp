#include <windows.h>
#include "tp_stub.h"
#include <d3d9.h>

static IDirect3D9 *TVPDirect3D=NULL;
static IDirect3D9* (WINAPI * TVPDirect3DCreate)( UINT SDKVersion ) = NULL;
static HMODULE TVPDirect3DDLLHandle=NULL;

#define TVP_MSG_DECL(name, text) static const tjs_char * name = L ## text;
TVP_MSG_DECL(TVPInfoDirect3D,                                                 "(info) Loading Direct3D ...")
TVP_MSG_DECL(TVPFaildToCreateDirect3D,                                        "Faild to create Direct3D9.")
TVP_MSG_DECL(TVPCannotInitDirect3D,                                           "Cannot initialize Direct3D : %1")
TVP_MSG_DECL(TVPCannotLoadD3DDLL,                                             "Cannot load d3d9.dll")
TVP_MSG_DECL(TVPNotFoundDirect3DCreate,                                       "Missing Direct3DCreate9 in d3d9.dll")

static void TVPUninitDirect3D()
{
	// release Direct3D object ( DLL will not be released )
}
static void TVPUnloadDirect3D()
{
	// release Direct3D object and /*release it's DLL */
	TVPUninitDirect3D();
	if(TVPDirect3D) TVPDirect3D->Release(), TVPDirect3D = NULL;
}
static void TJS_USERENTRY TVPInitDirect3D(void *data=NULL)
{
	if(!TVPDirect3DDLLHandle)
	{
		// load d3d9.dll
		TVPAddLog( (const tjs_char*)TVPInfoDirect3D );
		TVPDirect3DDLLHandle = ::LoadLibraryW( L"d3d9.dll" );
		if(!TVPDirect3DDLLHandle)
			TVPThrowExceptionMessage(TVPCannotInitDirect3D, (const tjs_char*)TVPCannotLoadD3DDLL );
	}

	if(!TVPDirect3D)
	{
		try
		{
			// get Direct3DCreaet function
			TVPDirect3DCreate = (IDirect3D9*(WINAPI * )(UINT))GetProcAddress(TVPDirect3DDLLHandle, "Direct3DCreate9");
			if(!TVPDirect3DCreate)
				TVPThrowExceptionMessage(TVPCannotInitDirect3D, (const tjs_char*)TVPNotFoundDirect3DCreate );

			TVPDirect3D = TVPDirect3DCreate( D3D_SDK_VERSION );
			if( NULL == TVPDirect3D )
				TVPThrowExceptionMessage( TVPFaildToCreateDirect3D );
		}
		catch(...)
		{
			TVPUnloadDirect3D();
			throw;
		}
	}
}
static bool TJS_USERENTRY CatchBlockNoRethrow(void * data, const tTVPExceptionDesc & desc) { return false; } // for TVPDoTryBlcok

//---------------------------------------------------------------------------
void Krkr2EnsureDirect3DObject()
{
	TVPDoTryBlock(&TVPInitDirect3D, &CatchBlockNoRethrow, NULL, NULL);
}
IDirect3D9 * Krkr2GetDirect3DObjectNoAddRef()
{
	// retrieves IDirect3D9 interface
	return TVPDirect3D;
}
