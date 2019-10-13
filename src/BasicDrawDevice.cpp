
#define NOMINMAX
#include "BasicDrawDevice.h"
#include "ErrorCode.hpp"

#include <mmsystem.h>
#include <algorithm>

//---------------------------------------------------------------------------
// オプション
//---------------------------------------------------------------------------
tjs_int tTVPBasicDrawDevice::OptionsGeneration = 0;
bool    tTVPBasicDrawDevice::ZoomInterpolation = true;
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::InitDrawDeviceOptions()
{
	tjs_int gen = TVPGetCommandLineArgumentGeneration();
	if (OptionsGeneration != gen) {
		OptionsGeneration  = gen;

		tTJSVariant val;
		ZoomInterpolation = true;
		if(TVPGetCommandLine(TJS_W("-smoothzoom"), &val))
		{
			ttstr str(val);
			ZoomInterpolation = (str != TJS_W("no"));
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTVPBasicDrawDevice::tTVPBasicDrawDevice()
{
	InitDrawDeviceOptions(); // read and initialize options
	TargetWindow = NULL;
	DrawUpdateRectangle = false;
	BackBufferDirty = 1;

	Direct3D = NULL;
	Direct3DDevice = NULL;
	Texture = NULL;
	ShouldShow = false;
	TextureBuffer = NULL;
	TextureWidth = TextureHeight = 0;
	VsyncInterval = 16;
	ZeroMemory( &D3dPP, sizeof(D3dPP) );
	ZeroMemory( &DispMode, sizeof(DispMode) );
}
//---------------------------------------------------------------------------
tTVPBasicDrawDevice::~tTVPBasicDrawDevice()
{
	DestroyD3DDevice();
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::DestroyD3DDevice() {
	DestroyTexture();
	if(Direct3DDevice) Direct3DDevice->Release(), Direct3DDevice = NULL;
	if(Direct3D) Direct3D = NULL;
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::DestroyTexture() {
	if(TextureBuffer && Texture) Texture->UnlockRect(0), TextureBuffer = NULL;
	if(Texture) Texture->Release(), Texture = NULL;
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::InvalidateAll()
{
	// レイヤ演算結果をすべてリクエストする
	// サーフェースが lost した際に内容を再構築する目的で用いる
	RequestInvalidation(tTVPRect(0, 0, DestRect.get_width(), DestRect.get_height()));
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::CheckMonitorMoved() {
	UINT iCurrentMonitor = GetMonitorNumber( TargetWindow );
	if( CurrentMonitor != iCurrentMonitor ) {
		// モニタ移動が発生しているので、デバイスを再生成する
		CreateD3DDevice();
	}
}
//---------------------------------------------------------------------------
bool tTVPBasicDrawDevice::IsTargetWindowActive() const {
	if( TargetWindow == NULL ) return false;
	return ::GetForegroundWindow() == TargetWindow;
}
//---------------------------------------------------------------------------
bool tTVPBasicDrawDevice::GetDirect3D9Device() {
	DestroyD3DDevice();

	EnsureDirect3DObject();

	if( NULL == ( Direct3D = GetDirect3DObjectNoAddRef() ) )
		Logger::ThrowException( ErrorCode::FaildToCreateDirect3D );

	HRESULT hr;
	if( FAILED( hr = DecideD3DPresentParameters() ) ) {
		if( IsTargetWindowActive() ) {
			ErrorToLog( hr );
			Logger::ThrowException( ErrorCode::FaildToDecideBackbufferFormat );
		}
		return false;
	}

	UINT iCurrentMonitor = GetMonitorNumber( TargetWindow );
	DWORD	BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	if( D3D_OK != ( hr = Direct3D->CreateDevice( iCurrentMonitor, D3DDEVTYPE_HAL, TargetWindow, BehaviorFlags, &D3dPP, &Direct3DDevice ) ) ) {
		if( IsTargetWindowActive() ) {
			ErrorToLog( hr );
			Logger::ThrowException( ErrorCode::FaildToCreateDirect3DDevice );
		}
		return false;
	}
	CurrentMonitor = iCurrentMonitor;
	BackBufferDirty |= 1;

	/*
	D3DVIEWPORT9 vp;
	vp.X  = DestLeft;
	vp.Y  = DestTop;
	vp.Width = DestWidth != 0 ? DestWidth : D3dPP.BackBufferWidth;
	vp.Height = DestHeight != 0 ? DestHeight : D3dPP.BackBufferHeight;
	*/
	
	D3DVIEWPORT9 vp;
	vp.X  = 0;
	vp.Y  = 0;
	vp.Width = D3dPP.BackBufferWidth;
	vp.Height = D3dPP.BackBufferHeight;
	vp.MinZ  = 0.0f;
	vp.MaxZ  = 1.0f;
	if( FAILED(hr = Direct3DDevice->SetViewport(&vp)) ) {
		if( IsTargetWindowActive() ) {
			ErrorToLog( hr );
			Logger::ThrowException( ErrorCode::FaildToSetViewport );
		}
		return false;
	}

	if( FAILED( hr = InitializeDirect3DState() ) ) {
		if( IsTargetWindowActive() ) {
			ErrorToLog( hr );
			Logger::ThrowException( ErrorCode::FaildToSetRenderState );
		}
		return false;
	}

	int refreshrate = DispMode.RefreshRate;
	if( refreshrate == 0 ) {
		HDC hdc;
		hdc = ::GetDC(TargetWindow);
		refreshrate = GetDeviceCaps( hdc, VREFRESH );
		::ReleaseDC( TargetWindow, hdc );
	}
	VsyncInterval = 1000 / refreshrate;
	return true;
}
//---------------------------------------------------------------------------
HRESULT tTVPBasicDrawDevice::InitializeDirect3DState() {
	HRESULT	hr;
	D3DCAPS9	d3dcaps;
	if( FAILED( hr = Direct3DDevice->GetDeviceCaps( &d3dcaps ) ) )
		return hr;

	if( d3dcaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR ) {
		if( FAILED( hr = Direct3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, ZoomInterpolation?D3DTEXF_LINEAR:D3DTEXF_POINT ) ) )
			return hr;
	} else {
		if( FAILED( hr = Direct3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) ) )
			return hr;
	}

	if( d3dcaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR ) {
		if( FAILED( hr = Direct3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, ZoomInterpolation?D3DTEXF_LINEAR:D3DTEXF_POINT ) ) )
			return hr;
	} else {
		if( FAILED( hr = Direct3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT ) ) )
		return hr;
	}

	if( FAILED( hr = Direct3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) ) )
		return hr;
	if( FAILED( hr = Direct3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE ) ) )
		return hr;

	return S_OK;
}
//---------------------------------------------------------------------------
UINT tTVPBasicDrawDevice::GetMonitorNumber( HWND window )
{
	if( Direct3D == NULL || window == NULL ) return D3DADAPTER_DEFAULT;
	HMONITOR windowMonitor = ::MonitorFromWindow( window, MONITOR_DEFAULTTOPRIMARY );
	UINT iCurrentMonitor = 0;
	UINT numOfMonitor = Direct3D->GetAdapterCount();
	for( ; iCurrentMonitor < numOfMonitor; ++iCurrentMonitor ) 	{
		if( Direct3D->GetAdapterMonitor(iCurrentMonitor) == windowMonitor )
			break;
	}
	if( iCurrentMonitor == numOfMonitor )
		iCurrentMonitor = D3DADAPTER_DEFAULT;
	return iCurrentMonitor;
}
//---------------------------------------------------------------------------
HRESULT tTVPBasicDrawDevice::DecideD3DPresentParameters() {
	HRESULT			hr;
	UINT iCurrentMonitor = GetMonitorNumber(TargetWindow);
	if( FAILED( hr = Direct3D->GetAdapterDisplayMode( iCurrentMonitor, &DispMode ) ) )
		return hr;

	ZeroMemory( &D3dPP, sizeof(D3dPP) );
	D3dPP.Windowed = TRUE;
	D3dPP.SwapEffect = D3DSWAPEFFECT_COPY;
	D3dPP.BackBufferFormat = D3DFMT_UNKNOWN;
	// for Steam Experimental
	if (BackBufferDirty & 2) {
		D3dPP.BackBufferHeight = DispMode.Height;
		D3dPP.BackBufferWidth = DispMode.Width;
	} else {
		D3dPP.BackBufferHeight = DestRect.get_height();
		D3dPP.BackBufferWidth = DestRect.get_width();
	}
	D3dPP.hDeviceWindow = TargetWindow;
	D3dPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	return S_OK;
}
//---------------------------------------------------------------------------
bool tTVPBasicDrawDevice::CreateD3DDevice()
{
	// Direct3D デバイス、テクスチャなどを作成する
	DestroyD3DDevice();
	if( TargetWindow ) {
		tjs_int w, h;
		GetSrcSize( w, h );
		if( w > 0 && h > 0 ) {
			// get Direct3D9 interface
			if( GetDirect3D9Device() ) {
				return CreateTexture();
			}
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool tTVPBasicDrawDevice::CreateTexture() {
	DestroyTexture();
	tjs_int w, h;
	GetSrcSize( w, h );
	if(TargetWindow && w > 0 && h > 0) {
		HRESULT hr = S_OK;

		D3DCAPS9 d3dcaps;
		Direct3DDevice->GetDeviceCaps( &d3dcaps );

		TextureWidth = w;
		TextureHeight = h;
		if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
			// only square textures are supported
			TextureWidth = std::max(TextureHeight, TextureWidth);
			TextureHeight = TextureWidth;
		}

		DWORD dwWidth = 64;
		DWORD dwHeight = 64;
		if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 ) {
			// 2の累乗のみ許可するかどうか判定
			while( dwWidth < TextureWidth ) dwWidth = dwWidth << 1;
			while( dwHeight < TextureHeight ) dwHeight = dwHeight << 1;
			TextureWidth = dwWidth;
			TextureHeight = dwHeight;

			if( dwWidth > d3dcaps.MaxTextureWidth || dwHeight > d3dcaps.MaxTextureHeight ) {
				Logger::AddLog( ErrorCode::WarningImageSizeTooLargeMayBeCannotCreateTexture );
			}
			Logger::AddLog( ErrorCode::UsePowerOfTwoSurface );
		} else {
			dwWidth = TextureWidth;
			dwHeight = TextureHeight;
		}

		if( D3D_OK != ( hr = Direct3DDevice->CreateTexture( dwWidth, dwHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &Texture, NULL) ) ) {
			if( IsTargetWindowActive() ) {
				ErrorToLog( hr );
				Logger::ThrowException( ErrorCode::CannotAllocateD3DOffScreenSurface, TJSInt32ToHex(hr, 8) );
			}
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::EnsureDevice()
{
	InitDrawDeviceOptions();
	if( TargetWindow ) {
		
		struct TryCatchBlock {
			static void TJS_USERENTRY Try(void * data) {
				selfclass *_this = reinterpret_cast<selfclass *>(data);

				bool recreate = false;
				if( _this->Direct3D == NULL || _this->Direct3DDevice == NULL ) {
					if( _this->GetDirect3D9Device() == false ) {
						return;
					}
					recreate = true;
				}
				if( _this->Texture == NULL ) {
					if( _this->CreateTexture() == false ) {
						return;
					}
					recreate = true;
				}
				if( recreate ) {
					_this->InvalidateAll();
				}
			}
			static bool TJS_USERENTRY Catch(void * data, const tTVPExceptionDesc & desc) {
				selfclass *_this = reinterpret_cast<selfclass *>(data);

				if (desc.type == TJS_W("eTJS")) {
					Logger::AddImportantLog( ErrorCode::FailedToCreateDirect3DDevice, desc.message );
				} else {
					Logger::AddImportantLog( ErrorCode::FailedToCreateDirect3DDeviceUnknownReason );
				}
				_this->DestroyD3DDevice();
				return false; // no rethrow
			}
		};
		TVPDoTryBlock(&TryCatchBlock::Try, &TryCatchBlock::Catch, NULL, (void*)this);
	}
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::TryRecreateWhenDeviceLost()
{
	bool success = false;
	if( Direct3DDevice ) {
		DestroyTexture();
		HRESULT hr = Direct3DDevice->TestCooperativeLevel();
		if( hr == D3DERR_DEVICENOTRESET ) {
			hr = Direct3DDevice->Reset(&D3dPP);
		}
		if( hr == D3DERR_DEVICELOST ) {
			// 復帰できない
			return;
		}
		if( FAILED(hr) ) {
			success = CreateD3DDevice();
		} else {
			if( D3D_OK == InitializeDirect3DState() ) {
				success = true;
			}
		}
	} else {
		success = CreateD3DDevice();
	}
	if( success ) {
		InvalidateAll();	// 画像の再描画(Layer Update)を要求する
	}
}
//---------------------------------------------------------------------------
#define ERR2LOG(HR, MES) \
case HR: Logger::AddLog( ErrorCode:: MES ); break

void tTVPBasicDrawDevice::ErrorToLog( HRESULT hr ) {
	switch( hr ) {
		ERR2LOG( D3DERR_DEVICELOST,                  D3dErrDeviceLost );
		ERR2LOG( D3DERR_DRIVERINTERNALERROR,         D3dErrDriverIinternalError );
		ERR2LOG( D3DERR_INVALIDCALL,                 D3dErrInvalidCall );
		ERR2LOG( D3DERR_OUTOFVIDEOMEMORY,            D3dErrOutOfVideoMemory );
		ERR2LOG( D3DERR_WRONGTEXTUREFORMAT,          D3dErrWrongTextureFormat );
		ERR2LOG( D3DERR_UNSUPPORTEDCOLOROPERATION,   D3dErrUnsuportedColorOperation );
		ERR2LOG( D3DERR_UNSUPPORTEDCOLORARG,         D3dErrUnsuportedColorArg );
		ERR2LOG( D3DERR_UNSUPPORTEDALPHAOPERATION,   D3dErrUnsuportedAalphtOperation );
		ERR2LOG( D3DERR_UNSUPPORTEDALPHAARG,         D3dErrUnsuportedAlphaArg );
		ERR2LOG( D3DERR_TOOMANYOPERATIONS,           D3dErrTooManyOperations );
		ERR2LOG( D3DERR_CONFLICTINGTEXTUREFILTER,    D3dErrConflictioningTextureFilter );
		ERR2LOG( D3DERR_UNSUPPORTEDFACTORVALUE,      D3dErrUnsuportedFactorValue );
		ERR2LOG( D3DERR_CONFLICTINGRENDERSTATE,      D3dErrConflictioningRenderState );
		ERR2LOG( D3DERR_UNSUPPORTEDTEXTUREFILTER,    D3dErrUnsupportedTextureFilter );
		ERR2LOG( D3DERR_CONFLICTINGTEXTUREPALETTE,   D3dErrConflictioningTexturePalette );
		ERR2LOG( D3DERR_NOTFOUND,                    D3dErrNotFound );
		ERR2LOG( D3DERR_MOREDATA,                    D3dErrMoreData );
		ERR2LOG( D3DERR_DEVICENOTRESET,              D3dErrDeviceNotReset );
		ERR2LOG( D3DERR_NOTAVAILABLE,                D3dErrNotAvailable );
		ERR2LOG( D3DERR_INVALIDDEVICE,               D3dErrInvalidDevice );
		ERR2LOG( D3DERR_DRIVERINVALIDCALL,           D3dErrDriverInvalidCall );
		ERR2LOG( D3DERR_WASSTILLDRAWING,             D3dErrWasStillDrawing );
		ERR2LOG( D3DERR_DEVICEHUNG,                  D3dErrDeviceHung );

#ifdef           D3DERR_UNSUPPORTEDOVERLAY
		ERR2LOG( D3DERR_UNSUPPORTEDOVERLAY,          D3dErrUnsupportedOverlay );
#else
#pragma message("D3DERR_UNSUPPORTEDOVERLAY not defined")
#endif
#ifdef           D3DERR_UNSUPPORTEDOVERLAYFORMAT
		ERR2LOG( D3DERR_UNSUPPORTEDOVERLAYFORMAT,    D3dErrUnsupportedOverlayFormat );
#else
#pragma message("D3DERR_UNSUPPORTEDOVERLAYFORMAT not defined")
#endif
#ifdef           D3DERR_CANNOTPROTECTCONTENT
		ERR2LOG( D3DERR_CANNOTPROTECTCONTENT,        D3dErrCannotProtectContent );
#else
#pragma message("D3DERR_CANNOTPROTECTCONTENT not defined")
#endif
#ifdef           D3DERR_UNSUPPORTEDCRYPTO
		ERR2LOG( D3DERR_UNSUPPORTEDCRYPTO,           D3dErrUnsupportedCrypto );
#else
#pragma message("D3DERR_UNSUPPORTEDCRYPTO not defined")
#endif
#ifdef           D3DERR_PRESENT_STATISTICS_DISJOINT
		ERR2LOG( D3DERR_PRESENT_STATISTICS_DISJOINT, D3dErrPresentStatisticsDisJoint );
#else
#pragma message("D3DERR_PRESENT_STATISTICS_DISJOINT not defined")
#endif
		ERR2LOG( D3DERR_DEVICEREMOVED,               D3dErrDeviceRemoved );

		ERR2LOG( D3DOK_NOAUTOGEN,                    D3dOkNoAutoGen );

		ERR2LOG( E_OUTOFMEMORY,                      D3dErrOutOfMemory );
		ERR2LOG( E_FAIL,                             D3dErrFail );
		ERR2LOG( E_INVALIDARG,                       D3dErrInvalidArg );

	case D3D_OK:
		break;
	default:
		Logger::AddLog( ErrorCode::D3dUnknownError );
		break;
	}
}
#undef ERR2LOG

//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::OnManagerSetError(tjs_error r)
{
	switch (r) {
	case TJS_E_ACCESSDENYED:   Logger::ThrowException( ErrorCode::DoesNotSupporteLayerManagerMoreThanOne ); break;
	case TJS_E_MEMBERNOTFOUND: Logger::ThrowException( ErrorCode::UnknownLayerManagerRemoved ); break;
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	inherited::AddLayerManager(manager);

	manager->SetDesiredLayerType(ltOpaque); // ltOpaque な出力を受け取りたい
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::SetTargetWindow(HWND wnd, bool is_main)
{
	InitDrawDeviceOptions();
	DestroyD3DDevice();
	TargetWindow = wnd;
	IsMainWindow = is_main;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::SetDestRectangle(const tTVPRect & rect)
{
	BackBufferDirty |= 1;
	// 位置だけの変更の場合かどうかをチェックする
	if(rect.get_width() == DestRect.get_width() && rect.get_height() == DestRect.get_height()) {
		// 位置だけの変更だ
		inherited::SetDestRectangle(rect);
	} else {
		// サイズも違う
		CheckRecreateOnSizeChanged(rect); // 必要に応じて DestroyD3DDevice() を呼んで recreate する

		bool success = true;
		inherited::SetDestRectangle(rect);

		// TryCatchブロック
		struct SetDestRectangleTryCatchBlock {
			selfclass * Self;
			bool * Success;

			static void TJS_USERENTRY Try(void * data) {
				SetDestRectangleTryCatchBlock *param = reinterpret_cast<SetDestRectangleTryCatchBlock *>(data);
				param->Self->EnsureDevice();
			}
			static bool TJS_USERENTRY Catch(void * data, const tTVPExceptionDesc & desc) {
				SetDestRectangleTryCatchBlock *param = reinterpret_cast<SetDestRectangleTryCatchBlock *>(data);

				if (desc.type == TJS_W("eTJS")) {
					Logger::AddImportantLog( ErrorCode::FailedToCreateDirect3DDevices, desc.message );
				} else {
					Logger::AddImportantLog( ErrorCode::FailedToCreateDirect3DDevicesUnknownReason );
				}
				*(param->Success) = false;
				return false; // no rethrow
			}
		} param = { this, &success };
		TVPDoTryBlock(&SetDestRectangleTryCatchBlock::Try, &SetDestRectangleTryCatchBlock::Catch, NULL, (void*)&param);
		if( success == false ) {
			DestroyD3DDevice();
		}
	}
}
//---------------------------------------------------------------------------
void tTVPBasicDrawDevice::CheckRecreateOnSizeChanged(const tTVPRect & rect)
{
	if( rect.get_width() > (tjs_int)D3dPP.BackBufferWidth || rect.get_height() > (tjs_int)D3dPP.BackBufferHeight ) {
		// バックバッファサイズよりも大きいサイズが指定された場合一度破棄する。後のEnsureDeviceで再生成される。
		DestroyD3DDevice();
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::NotifyLayerResize(iTVPLayerManager * manager)
{
	inherited::NotifyLayerResize(manager);

	BackBufferDirty |= 1;

	// テクスチャを捨てて作り直す。
	CreateTexture();
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::Show()
{
	if(!TargetWindow) return;
	if(!Texture) return;
	if(!Direct3DDevice) return;
	if(!ShouldShow) return;

	ShouldShow = false;

	HRESULT hr = D3D_OK;
	RECT client;
	if( ::GetClientRect( TargetWindow, &client ) ) {
		RECT drect;
		drect.left   = 0;
		drect.top    = 0;
		drect.right  = client.right - client.left;
		drect.bottom = client.bottom - client.top;

		RECT srect = drect;
		hr = Direct3DDevice->Present( &srect, &drect, TargetWindow, NULL );
	} else {
		ShouldShow = true;
	}

	if(hr == D3DERR_DEVICELOST) {
		if( IsTargetWindowActive() ) ErrorToLog( hr );
		TryRecreateWhenDeviceLost();
	} else if(hr != D3D_OK) {
		ErrorToLog( hr );
		Logger::AddImportantLog( ErrorCode::InfDirect3DDevicePresentFailed, TJSInt32ToHex(hr, 8) );
	}
}
//---------------------------------------------------------------------------
bool TJS_INTF_METHOD tTVPBasicDrawDevice::WaitForVBlank( tjs_int* in_vblank, tjs_int* delayed )
{
	if( Direct3DDevice == NULL ) return false;

	bool inVsync = false;
	D3DRASTER_STATUS rs;
	if( D3D_OK == Direct3DDevice->GetRasterStatus(0,&rs) ) {
		inVsync = rs.InVBlank == TRUE;
	}

	// VSync 待ちを行う
	bool isdelayed = false;
	if(!inVsync) {
		// vblank から抜けるまで待つ
		DWORD timeout_target_tick = ::timeGetTime() + 1;
		rs.InVBlank = FALSE;
		HRESULT hr = D3D_OK;
		do {
			hr = Direct3DDevice->GetRasterStatus(0,&rs);
		} while( D3D_OK == hr && rs.InVBlank == TRUE && (long)(::timeGetTime() - timeout_target_tick) <= 0);

		// vblank に入るまで待つ
		rs.InVBlank = TRUE;
		do {
			hr = Direct3DDevice->GetRasterStatus(0,&rs);
		} while( D3D_OK == hr && rs.InVBlank == FALSE && (long)(::timeGetTime() - timeout_target_tick) <= 0);

		if((int)(::timeGetTime() - timeout_target_tick) > 0) {
			// フレームスキップが発生したと考えてよい
			isdelayed  = true;
		}
		inVsync = rs.InVBlank == TRUE;
	}
	*delayed = isdelayed ? 1 : 0;
	*in_vblank = inVsync ? 1 : 0;
	return true;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	EnsureDevice();

	if( Texture && TargetWindow ) {
		if(TextureBuffer) {
			Logger::AddImportantLog( ErrorCode::TextureHasAlreadyBeenLocked );
			Texture->UnlockRect(0), TextureBuffer = NULL;
		}

		D3DLOCKED_RECT rt;
		HRESULT hr = Texture->LockRect( 0, &rt, NULL, D3DLOCK_NO_DIRTY_UPDATE );

		if(hr == D3DERR_INVALIDCALL && IsTargetWindowActive() ) {
			Logger::ThrowException( ErrorCode::InternalErrorResult, TJSInt32ToHex(hr, 8) );
		}

		if(hr != D3D_OK) {
			ErrorToLog( hr );
			TextureBuffer = NULL;
			TryRecreateWhenDeviceLost();
		} else /*if(hr == DD_OK) */ {
			TextureBuffer = rt.pBits;
			TexturePitch = rt.Pitch;
		}
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo で表されるビットマップの cliprect の領域を、x, y に描画
	// する。
	// opacity と type は無視するしかないので無視する
	tjs_int w, h;
	GetSrcSize( w, h );
	if( TextureBuffer && TargetWindow &&
		!(x < 0 || y < 0 ||
			x + cliprect.get_width() > w ||
			y + cliprect.get_height() > h) &&
		!(cliprect.left < 0 || cliprect.top < 0 ||
			cliprect.right > bitmapinfo->bmiHeader.biWidth ||
			cliprect.bottom > bitmapinfo->bmiHeader.biHeight))
	{
		// 範囲外の転送は(一部だけ転送するのではなくて)無視してよい
		ShouldShow = true;

		// bitmapinfo で表された cliprect の領域を x,y にコピーする
		long src_y       = cliprect.top;
		long src_y_limit = cliprect.bottom;
		long src_x       = cliprect.left;
		long width_bytes   = cliprect.get_width() * 4; // 32bit
		long dest_y      = y;
		long dest_x      = x;
		const tjs_uint8 * src_p = (const tjs_uint8 *)bits;
		long src_pitch;

		if(bitmapinfo->bmiHeader.biHeight < 0)
		{
			// bottom-down
			src_pitch = bitmapinfo->bmiHeader.biWidth * 4;
		}
		else
		{
			// bottom-up
			src_pitch = -bitmapinfo->bmiHeader.biWidth * 4;
			src_p += bitmapinfo->bmiHeader.biWidth * 4 * (bitmapinfo->bmiHeader.biHeight - 1);
		}

		for(; src_y < src_y_limit; src_y ++, dest_y ++)
		{
			const void *srcp = src_p + src_pitch * src_y + src_x * 4;
			void *destp = (tjs_uint8*)TextureBuffer + TexturePitch * dest_y + dest_x * 4;
			memcpy(destp, srcp, width_bytes);
		}
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	if(!TargetWindow) return;
	if(!Texture) return;
	if(!Direct3DDevice) return;

	if(!TextureBuffer) return;
	Texture->UnlockRect(0);
	TextureBuffer = NULL;


	//- build vertex list
	struct tVertices
	{
		float x, y, z, rhw;
		float tu, tv;
	};

	// 転送先をクリッピング矩形に基づきクリッピング
	float dl = (float)( DestRect.left < ClipRect.left ? ClipRect.left : DestRect.left );
	float dt = (float)( DestRect.top < ClipRect.top ? ClipRect.top : DestRect.top );
	float dr = (float)( DestRect.right > ClipRect.right ? ClipRect.right : DestRect.right );
	float db = (float)( DestRect.bottom > ClipRect.bottom ? ClipRect.bottom : DestRect.bottom );
	float dw = (float)DestRect.get_width();
	float dh = (float)DestRect.get_height();

	// はみ出している幅を求める
	float cl = dl - (float)DestRect.left;
	float ct = dt - (float)DestRect.top;
	float cr = (float)DestRect.right - dr;
	float cb = (float)DestRect.bottom - db;

	// はみ出している幅を考慮して、転送元画像をクリッピング
	tjs_int w, h;
	GetSrcSize( w, h );
	float sl = (float)(cl * w / dw ) / (float)TextureWidth;
	float st = (float)(ct * h / dh ) / (float)TextureHeight;
	float sr = (float)(w - (cr * w / dw) ) / (float)TextureWidth;
	float sb = (float)(h - (cb * h / dh) ) / (float)TextureHeight;

	tVertices vertices[] =
	{
		{dl - 0.5f, dt - 0.5f, 1.0f, 1.0f, sl, st },
		{dr - 0.5f, dt - 0.5f, 1.0f, 1.0f, sr, st },
		{dl - 0.5f, db - 0.5f, 1.0f, 1.0f, sl, sb },
		{dr - 0.5f, db - 0.5f, 1.0f, 1.0f, sr, sb }
	};

	HRESULT hr;

// フルスクリーン化後、1回は全体消去、それ以降はウィンドウの範囲内のみにした方が効率的。
	D3DVIEWPORT9 vp;
	vp.X  = 0;
	vp.Y  = 0;
	vp.Width = D3dPP.BackBufferWidth;
	vp.Height = D3dPP.BackBufferHeight;
	vp.MinZ  = 0.0f;
	vp.MaxZ  = 1.0f;
	Direct3DDevice->SetViewport(&vp);

	if( SUCCEEDED(Direct3DDevice->BeginScene()) ) {
		struct CAutoEndSceneCall {
			IDirect3DDevice9*	m_Device;
			CAutoEndSceneCall( IDirect3DDevice9* device ) : m_Device(device) {}
			~CAutoEndSceneCall() { m_Device->EndScene(); }
		} autoEnd(Direct3DDevice);

		if( BackBufferDirty ) {
			Direct3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0, 0 );
			if( (BackBufferDirty & 2) == 0 ) BackBufferDirty = 0;
		}

		//- draw as triangles
		if( FAILED(hr = Direct3DDevice->SetTexture(0, Texture)) )
			goto got_error;

		if( FAILED( hr = Direct3DDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 ) ) )
			goto got_error;

		if( FAILED( hr = Direct3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(tVertices) ) ) )
			goto got_error;

		if( FAILED( hr = Direct3DDevice->SetTexture( 0, NULL) ) )
			goto got_error;
	}

got_error:
	if( hr == D3DERR_DEVICELOST ) {
		TryRecreateWhenDeviceLost();
	} else if(hr == D3DERR_DEVICENOTRESET ) {
		hr = Direct3DDevice->Reset(&D3dPP);
		switch( hr ) {
		case D3DERR_DEVICELOST:          Logger::AddLog( ErrorCode::D3dErrDeviceLost           ); TryRecreateWhenDeviceLost(); break;
		case D3DERR_DRIVERINTERNALERROR: Logger::AddLog( ErrorCode::D3dErrDriverIinternalError ); TryRecreateWhenDeviceLost(); break;
		case D3DERR_INVALIDCALL:         Logger::AddLog( ErrorCode::D3dErrInvalidCall          ); TryRecreateWhenDeviceLost(); break;
		case D3DERR_OUTOFVIDEOMEMORY:    Logger::AddLog( ErrorCode::D3dErrOutOfVideoMemory     ); TryRecreateWhenDeviceLost(); break;
		case E_OUTOFMEMORY:              Logger::AddLog( ErrorCode::D3dErrOutOfMemory          ); TryRecreateWhenDeviceLost(); break;

		case D3D_OK:
			if( FAILED( hr = InitializeDirect3DState() ) ) {
				if( IsTargetWindowActive() ) {
					ErrorToLog( hr );
					Logger::ThrowException( ErrorCode::FaildToSetRenderState );
				} else {
					DestroyD3DDevice();
				}
			}
			break;
		}
	} else if(hr != D3D_OK) {
		ErrorToLog( hr );
		Logger::AddImportantLog( ErrorCode::InfPolygonDrawingFailed, TJSInt32ToHex(hr, 8) );
	}
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::SetShowUpdateRect(bool b)
{
	DrawUpdateRectangle = b;
}
//---------------------------------------------------------------------------
bool TJS_INTF_METHOD tTVPBasicDrawDevice::SwitchToFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color, bool changeresolution )
{
	// フルスクリーン化の処理はなにも行わない、互換性のためにウィンドウを全画面化するのみで処理する
	// Direct3D9 でフルスクリーン化するとフォーカスを失うとデバイスをロストするので、そのたびにリセットor作り直しが必要になる。
	// モーダルウィンドウを使用するシステムでは、これは困るので常にウィンドウモードで行う。
	// モーダルウィンドウを使用しないシステムにするのなら、フルスクリーンを使用するDrawDeviceを作ると良い。
	BackBufferDirty |= 2;
	ShouldShow = true;
	CheckMonitorMoved();
	return true;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBasicDrawDevice::RevertFromFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color )
{
	BackBufferDirty = 1;
	ShouldShow = true;
	CheckMonitorMoved();
}
//---------------------------------------------------------------------------







