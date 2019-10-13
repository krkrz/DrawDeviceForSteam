#pragma once

#include "tTVPDrawDevice.hpp"
#include <d3d9.h>

//---------------------------------------------------------------------------
//! @brief		DrawDeviceSteam(BasicDrawDeviceからの若干の拡張)
//---------------------------------------------------------------------------
class tTVPBasicDrawDeviceForSteam : public tTVPDrawDevice
{
	typedef tTVPDrawDevice inherited;
	typedef tTVPBasicDrawDeviceForSteam selfclass;

	HWND TargetWindow;
	bool IsMainWindow;
	bool DrawUpdateRectangle;
	int  BackBufferDirty; // [MODIFIED] bool->int

	IDirect3D9*				Direct3D;
	IDirect3DDevice9*		Direct3DDevice;
	IDirect3DTexture9*		Texture;
	D3DPRESENT_PARAMETERS	D3dPP;
	D3DDISPLAYMODE			DispMode;

	UINT	CurrentMonitor;
	void*	TextureBuffer; //!< テクスチャのサーフェースへのメモリポインタ
	long	TexturePitch; //!< テクスチャのピッチ

	tjs_uint TextureWidth; //!< テクスチャの横幅
	tjs_uint TextureHeight; //!< テクスチャの縦幅

	bool ShouldShow; //!< show で実際に画面に画像を転送すべきか

	tjs_uint VsyncInterval;

public:
	tTVPBasicDrawDeviceForSteam(); //!< コンストラクタ

protected: // [MODIFIED]
	~tTVPBasicDrawDeviceForSteam(); //!< デストラクタ

private:
	void InvalidateAll();

	UINT GetMonitorNumber( HWND window );

	bool IsTargetWindowActive() const;

	bool GetDirect3D9Device();
	HRESULT InitializeDirect3DState();
	HRESULT DecideD3DPresentParameters();

	bool CreateD3DDevice();
	void DestroyD3DDevice();

	bool CreateTexture();
	void DestroyTexture();

	void TryRecreateWhenDeviceLost();
	void ErrorToLog( HRESULT hr );
	void CheckMonitorMoved();

private:
	// [MODIFIED]
	virtual void OnManagerSetError(tjs_error r);
	static void InitDrawDeviceOptions();
	static tjs_int OptionsGeneration;
	static bool    ZoomInterpolation;
protected:
	// Z専用メソッドのため2での互換処理が必要な場合は適宜オーバーライドすること
	virtual void        EnsureDirect3DObject()      const {        TVPEnsureDirect3DObject();      }
	virtual IDirect3D9* GetDirect3DObjectNoAddRef() const { return TVPGetDirect3DObjectNoAddRef(); }

public:
	void SetToRecreateDrawer() { DestroyD3DDevice(); }

public:
	void EnsureDevice();

//---- LayerManager の管理関連
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);

//---- 描画位置・サイズ関連
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);

//---- 再描画関連
	virtual void TJS_INTF_METHOD Show();
	virtual bool TJS_INTF_METHOD WaitForVBlank( tjs_int* in_vblank, tjs_int* delayed );

//---- LayerManager からの画像受け渡し関連
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

//---- デバッグ支援
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b);

//---- フルスクリーン
	virtual bool TJS_INTF_METHOD SwitchToFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color, bool changeresolution );
	virtual void TJS_INTF_METHOD RevertFromFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color );

};
//---------------------------------------------------------------------------

