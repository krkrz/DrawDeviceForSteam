#pragma once

#include "tTVPDrawDevice.hpp"
#include <d3d9.h>

//---------------------------------------------------------------------------
//! @brief		DrawDeviceSteam(BasicDrawDevice����̎኱�̊g��)
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
	void*	TextureBuffer; //!< �e�N�X�`���̃T�[�t�F�[�X�ւ̃������|�C���^
	long	TexturePitch; //!< �e�N�X�`���̃s�b�`

	tjs_uint TextureWidth; //!< �e�N�X�`���̉���
	tjs_uint TextureHeight; //!< �e�N�X�`���̏c��

	bool ShouldShow; //!< show �Ŏ��ۂɉ�ʂɉ摜��]�����ׂ���

	tjs_uint VsyncInterval;

public:
	tTVPBasicDrawDeviceForSteam(); //!< �R���X�g���N�^

protected: // [MODIFIED]
	~tTVPBasicDrawDeviceForSteam(); //!< �f�X�g���N�^

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
	// Z��p���\�b�h�̂���2�ł̌݊��������K�v�ȏꍇ�͓K�X�I�[�o�[���C�h���邱��
	virtual void        EnsureDirect3DObject()      const {        TVPEnsureDirect3DObject();      }
	virtual IDirect3D9* GetDirect3DObjectNoAddRef() const { return TVPGetDirect3DObjectNoAddRef(); }

public:
	void SetToRecreateDrawer() { DestroyD3DDevice(); }

public:
	void EnsureDevice();

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);

//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);

//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD Show();
	virtual bool TJS_INTF_METHOD WaitForVBlank( tjs_int* in_vblank, tjs_int* delayed );

//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

//---- �f�o�b�O�x��
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b);

//---- �t���X�N���[��
	virtual bool TJS_INTF_METHOD SwitchToFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color, bool changeresolution );
	virtual void TJS_INTF_METHOD RevertFromFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color );

};
//---------------------------------------------------------------------------

