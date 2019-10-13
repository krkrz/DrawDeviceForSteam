#pragma once
#include <windows.h>
#include "tp_stub.h"

//---------------------------------------------------------------------------
// 吉里吉里2の各種インターフェースを定義

// k2_iTVPLayerManager : 吉里吉里2版 iTVPLayerManager
// k2_iTVPWindow       : 吉里吉里2版 iTVPWindow
// k2_iTVPDrawDevice   : 吉里吉里2版 iTVPDrawDevice

// その他の互換情報
// - tTJSNI_BaseLayer : プラグイン側で扱うのはポインタだけなので問題なし
// - tTVPMouseButton  : mbX1, mbX2 が吉里吉里Zで追加されているが問題なし
// - tTVPLayerType    : 互換あり
// - tTVPRect         : 互換あり
// - tTVPImeMode      : 互換あり

/*******************************/
struct k2_iTVPLayerManager {
//-- object lifetime management
	virtual void TJS_INTF_METHOD AddRef() = 0;
	virtual void TJS_INTF_METHOD Release() = 0;

//-- draw device specific information
	virtual void TJS_INTF_METHOD SetDrawDeviceData(void * data) = 0;
	virtual void * TJS_INTF_METHOD GetDrawDeviceData() const = 0;

//-- layer metrics
	virtual bool TJS_INTF_METHOD GetPrimaryLayerSize(tjs_int &w, tjs_int &h) const = 0;

//-- layer structure information
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() const = 0;
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() const = 0;
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;

//-- HID releted
	virtual void TJS_INTF_METHOD NotifyClick(tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD NotifyDoubleClick(tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD NotifyMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD NotifyMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD NotifyMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD ReleaseCapture() = 0;
	virtual void TJS_INTF_METHOD NotifyMouseOutOfWindow() = 0;
	virtual void TJS_INTF_METHOD NotifyKeyDown(tjs_uint key, tjs_uint32 shift) = 0;
	virtual void TJS_INTF_METHOD NotifyKeyUp(tjs_uint key, tjs_uint32 shift) = 0;
	virtual void TJS_INTF_METHOD NotifyKeyPress(tjs_char key) = 0;
	virtual void TJS_INTF_METHOD NotifyMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;
	// [INCOMPATIBLE] no NotifyTouch*～NotifyMultiTouch methods
	virtual void TJS_INTF_METHOD RecheckInputState() = 0;

//-- invalidation/update
	virtual void TJS_INTF_METHOD SetDesiredLayerType(tTVPLayerType type) = 0;
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect &r) = 0; // draw device -> layer
	virtual void TJS_INTF_METHOD UpdateToDrawDevice() = 0;

//-- debug assist
	virtual void TJS_INTF_METHOD DumpLayerStructure() = 0;
};

/*******************************/
struct k2_iTVPWindow
{
	virtual void TJS_INTF_METHOD NotifySrcResize() = 0;
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor() = 0;
	virtual void TJS_INTF_METHOD SetMouseCursor(tjs_int cursor) = 0;
	virtual void TJS_INTF_METHOD GetCursorPos(tjs_int &x, tjs_int &y) = 0;
	virtual void TJS_INTF_METHOD SetCursorPos(tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD WindowReleaseCapture() = 0;
	virtual void TJS_INTF_METHOD SetHintText(const ttstr & text) = 0; // [INCOMPATIBLE] no sender argument
	virtual void TJS_INTF_METHOD SetAttentionPoint(tTJSNI_BaseLayer *layer, tjs_int l, tjs_int t) = 0;
	virtual void TJS_INTF_METHOD DisableAttentionPoint() = 0;
	virtual void TJS_INTF_METHOD SetImeMode(tTVPImeMode mode) = 0;
	virtual void TJS_INTF_METHOD ResetImeMode() = 0;
	virtual void TJS_INTF_METHOD RequestUpdate() = 0;
	virtual iTJSDispatch2 * GetWindowDispatch() = 0;
};

/*******************************/
struct k2_iTVPDrawDevice
{
//---- オブジェクト生存期間制御
	virtual void TJS_INTF_METHOD Destruct() = 0;

//---- window interface 関連
	virtual void TJS_INTF_METHOD SetWindowInterface(k2_iTVPWindow * window) = 0;

//---- LayerManager の管理関連
	virtual void TJS_INTF_METHOD AddLayerManager(k2_iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD RemoveLayerManager(k2_iTVPLayerManager * manager) = 0;

//---- 描画位置・サイズ関連
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main) = 0;
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect) = 0;
	// [INCOMPATIBLE]         no SetClipRectangle method
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h) = 0;
	virtual void TJS_INTF_METHOD NotifyLayerResize(k2_iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(k2_iTVPLayerManager * manager) = 0;

//---- ユーザーインターフェース関連
	virtual void TJS_INTF_METHOD OnClick(tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD OnDoubleClick(tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;
	virtual void TJS_INTF_METHOD OnReleaseCapture() = 0;
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow() = 0;
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift) = 0;
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift) = 0;
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key) = 0;
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD RecheckInputState() = 0;
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor(k2_iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD SetMouseCursor(k2_iTVPLayerManager * manager, tjs_int cursor) = 0;
	virtual void TJS_INTF_METHOD GetCursorPos(k2_iTVPLayerManager * manager, tjs_int &x, tjs_int &y) = 0;
	virtual void TJS_INTF_METHOD SetCursorPos(k2_iTVPLayerManager * manager, tjs_int x, tjs_int y) = 0;
	virtual void TJS_INTF_METHOD WindowReleaseCapture(k2_iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD SetHintText(k2_iTVPLayerManager * manager, const ttstr & text) = 0; // [INCOMPATIBLE] no sender argument
	virtual void TJS_INTF_METHOD SetAttentionPoint(k2_iTVPLayerManager * manager, tTJSNI_BaseLayer *layer, tjs_int l, tjs_int t) = 0;
	virtual void TJS_INTF_METHOD DisableAttentionPoint(k2_iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD SetImeMode(k2_iTVPLayerManager * manager, tTVPImeMode mode) = 0;
	virtual void TJS_INTF_METHOD ResetImeMode(k2_iTVPLayerManager * manager) = 0;

//---- プライマリレイヤ関連
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() = 0;
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() = 0;
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;

//---- 再描画関連
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect) = 0;
	virtual void TJS_INTF_METHOD Update() = 0;
	virtual void TJS_INTF_METHOD Show() = 0;

//---- LayerManager からの画像受け渡し関連
	virtual void TJS_INTF_METHOD StartBitmapCompletion(k2_iTVPLayerManager * manager) = 0;
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(k2_iTVPLayerManager * manager, tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo, const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity) = 0;
	virtual void TJS_INTF_METHOD EndBitmapCompletion(k2_iTVPLayerManager * manager) = 0;

//---- デバッグ支援
	virtual void TJS_INTF_METHOD DumpLayerStructure() = 0;
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b) = 0;

	// [INCOMPATIBLE] no {SwtichTo,RevertFrom}FullScreen, WaitForVBlank methods
};


//---------------------------------------------------------------------------
// k2 へのインターフェース変換 (プラグイン側提供→吉里吉里2側参照)

// kz2_tTVPDrawDevice : 吉里吉里Z->2版 iTVPDrawDevice ラッパー

class kz2_tTVPDrawDevice;
class Krkr2DrawDeviceWrapper {
	iTVPDrawDevice *owner;
	kz2_tTVPDrawDevice *k2dd;
public:
	Krkr2DrawDeviceWrapper(iTVPDrawDevice *kzdd);
	~Krkr2DrawDeviceWrapper();
	tTVInteger GetInterface() const { return reinterpret_cast<tTVInteger>(k2dd); }

	// 吉里吉里2/Zで処理を分ける
	tTVInteger  SwitchGetInterface() const;
	void        SwitchEnsureDirect3DObject() const;
	IDirect3D9* SwitchGetDirect3DObjectNoAddRef() const;

	// 吉里吉里2向け D3D9 初期化処理 
	static void        EnsureDirect3DObject();
	static IDirect3D9* GetDirect3DObjectNoAddRef();

	// プラグイン解放時に呼ぶ（ハードウェア例外を抑制するため）
	static void DetachAll();
};

