#pragma once
#include <windows.h>
#include "tp_stub.h"

// 吉里吉里Z の tp_stub でないとコンパイルできない（判定はとりあえず片方にしか存在しないdefineで判定）
#ifndef TJS_CII_SET_SUPRECLASS
#error invalid tp_stub.h: kirikiri-z version required.
#endif

#include <vector>
#include <algorithm>

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
// kz へのインターフェース変換 (吉里吉里2側提供→プラグイン側参照)

// k2z_tTVPLayerManager : 吉里吉里2->Z版 iTVPLayerManager ラッパー
// k2z_tTVPWindow       : 吉里吉里2->Z版 iTVPWindow ラッパー

class k2z_tTVPLayerManager : public iTVPLayerManager {
	k2_iTVPLayerManager &k2; // 実体を常に伴うので参照で問題ない
public:
	k2z_tTVPLayerManager(k2_iTVPLayerManager &lm) : k2(lm) {}
	k2z_tTVPLayerManager(const k2z_tTVPLayerManager &k2z) : k2(k2z.k2) {}
	void operator =  (const k2z_tTVPLayerManager &k2z) { k2 = k2z.k2; }
	bool operator == (const k2z_tTVPLayerManager &compare) const { return &k2 == &compare.k2; } // for std::find
	bool operator == (const k2_iTVPLayerManager  *compare) const { return &k2 ==  compare;    } // for std::find

//-- object lifetime management
	virtual void TJS_INTF_METHOD AddRef()  { k2.AddRef(); }
	virtual void TJS_INTF_METHOD Release() { k2.Release(); }

//-- draw device specific information
	virtual void TJS_INTF_METHOD SetDrawDeviceData(void * data) { k2.SetDrawDeviceData(data); }
	virtual void * TJS_INTF_METHOD GetDrawDeviceData() const { return k2.GetDrawDeviceData(); }

//-- layer metrics
	virtual bool TJS_INTF_METHOD GetPrimaryLayerSize(tjs_int &w, tjs_int &h) const { return k2.GetPrimaryLayerSize(w, h); }

//-- layer structure information
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() const { return k2.GetPrimaryLayer(); }
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() const { return k2.GetFocusedLayer(); }
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) { k2.SetFocusedLayer(layer); }

//-- HID releted
	virtual void TJS_INTF_METHOD NotifyClick      (tjs_int x, tjs_int y)                                       { k2.NotifyClick      (x, y); }
	virtual void TJS_INTF_METHOD NotifyDoubleClick(tjs_int x, tjs_int y)                                       { k2.NotifyDoubleClick(x, y); }
	virtual void TJS_INTF_METHOD NotifyMouseDown  (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) { k2.NotifyMouseDown  (x, y, mb, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseUp    (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) { k2.NotifyMouseUp    (x, y, mb, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseMove  (tjs_int x, tjs_int y, tjs_uint32 flags)                     { k2.NotifyMouseMove  (x, y, flags); }
	virtual void TJS_INTF_METHOD ReleaseCapture()                                                              { k2.ReleaseCapture(); }
	virtual void TJS_INTF_METHOD NotifyMouseOutOfWindow()                                                      { k2.NotifyMouseOutOfWindow(); }

	virtual void TJS_INTF_METHOD NotifyKeyDown   (tjs_uint key, tjs_uint32 shift)                              { k2.NotifyKeyDown (key, shift); }
	virtual void TJS_INTF_METHOD NotifyKeyUp     (tjs_uint key, tjs_uint32 shift)                              { k2.NotifyKeyUp   (key, shift); }
	virtual void TJS_INTF_METHOD NotifyKeyPress  (tjs_char key)                                                { k2.NotifyKeyPress(key); }

	virtual void TJS_INTF_METHOD NotifyMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)       { k2.NotifyMouseWheel(shift, delta, x, y); }

	// ※タッチ系は吉里吉里2には無いのでなにもしない
	virtual void TJS_INTF_METHOD NotifyTouchDown   ( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) {}
	virtual void TJS_INTF_METHOD NotifyTouchUp     ( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) {}
	virtual void TJS_INTF_METHOD NotifyTouchMove   ( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) {}
	virtual void TJS_INTF_METHOD NotifyTouchScaling( tjs_real startdist, tjs_real curdist, tjs_real cx, tjs_real cy, tjs_int flag ) {}
	virtual void TJS_INTF_METHOD NotifyTouchRotate ( tjs_real startangle, tjs_real curangle, tjs_real dist, tjs_real cx, tjs_real cy, tjs_int flag ) {}
	virtual void TJS_INTF_METHOD NotifyMultiTouch  () {}

	virtual void TJS_INTF_METHOD RecheckInputState()                     { k2.RecheckInputState(); }

//-- invalidation/update
	virtual void TJS_INTF_METHOD SetDesiredLayerType(tTVPLayerType type) { k2.SetDesiredLayerType(type); }
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect &r)  { k2.RequestInvalidation(r); }
	virtual void TJS_INTF_METHOD UpdateToDrawDevice()                    { k2.UpdateToDrawDevice(); }

//-- debug assist
	virtual void TJS_INTF_METHOD DumpLayerStructure()                    { k2.DumpLayerStructure(); }
};

/*******************************/
class k2z_tTVPWindow : public iTVPWindow
{
	k2_iTVPWindow *k2;
public:
	k2z_tTVPWindow() : k2(0) {}
	void setInterface(k2_iTVPWindow *k2win) { k2 = k2win; }
	virtual void TJS_INTF_METHOD NotifySrcResize()                    { if (k2) k2->NotifySrcResize(); }
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor()              { if (k2) k2->SetDefaultMouseCursor(); }
	virtual void TJS_INTF_METHOD SetMouseCursor(tjs_int cursor)       { if (k2) k2->SetMouseCursor(cursor); }
	virtual void TJS_INTF_METHOD GetCursorPos(tjs_int &x, tjs_int &y) { if (k2) k2->GetCursorPos(x, y); }
	virtual void TJS_INTF_METHOD SetCursorPos(tjs_int  x, tjs_int  y) { if (k2) k2->SetCursorPos(x, y); }
	virtual void TJS_INTF_METHOD WindowReleaseCapture()               { if (k2) k2->WindowReleaseCapture(); }
	virtual void TJS_INTF_METHOD SetHintText
		(iTJSDispatch2* sender, const ttstr & text)                   { if (k2) k2->SetHintText(text); } // ※引数が違う
	virtual void TJS_INTF_METHOD SetAttentionPoint
		(tTJSNI_BaseLayer *layer, tjs_int l, tjs_int t)               { if (k2) k2->SetAttentionPoint(layer, l, t); }
	virtual void TJS_INTF_METHOD DisableAttentionPoint()              { if (k2) k2->DisableAttentionPoint(); }
	virtual void TJS_INTF_METHOD SetImeMode(tTVPImeMode mode)         { if (k2) k2->SetImeMode(mode); }
	virtual void TJS_INTF_METHOD ResetImeMode()                       { if (k2) k2->ResetImeMode(); }
	virtual void TJS_INTF_METHOD RequestUpdate()                      { if (k2) k2->RequestUpdate(); }
	virtual iTJSDispatch2 * GetWindowDispatch()                       { return k2 ? k2->GetWindowDispatch() : NULL; } // TJS_INTF_METHOD じゃない?
};


//---------------------------------------------------------------------------
// k2 へのインターフェース変換 (プラグイン側提供→吉里吉里2側参照)

// kz2_tTVPDrawDevice : 吉里吉里Z->2版 iTVPDrawDevice ラッパー

// LayerManager変換メソッド呼び出しマクロ
#define FINDLM(k2lm, manager, method, args) do { \
	iTVPLayerManager *manager = findLayerManager(k2lm); \
	if (manager) dd. ## method ## args; \
	} while(0)

class kz2_tTVPDrawDevice : public k2_iTVPDrawDevice
{
	iTVPDrawDevice &dd;
	k2z_tTVPWindow window;
	std::vector<k2z_tTVPLayerManager> k2managers;
protected:
	iTVPLayerManager * findLayerManager(k2_iTVPLayerManager *k2lm) {
		std::vector<k2z_tTVPLayerManager>::iterator find = std::find(k2managers.begin(), k2managers.end(), k2lm);
		return (find == k2managers.end()) ? static_cast<iTVPLayerManager*>(&(*find)) : NULL;
	}
public:
	// 表記短縮用typedef
	typedef k2_iTVPLayerManager K2LM;
	typedef tTJSNI_BaseLayer    NLay;
	typedef tTVPMouseButton     MBtn;

	// Constructor
	kz2_tTVPDrawDevice(iTVPDrawDevice &kzdd) : dd(kzdd) {}

//---- オブジェクト生存期間制御
	virtual void TJS_INTF_METHOD Destruct() {
		window.setInterface(NULL);
		k2managers.clear();
		dd.Destruct();
	}

//---- window interface 関連
	virtual void TJS_INTF_METHOD SetWindowInterface(k2_iTVPWindow * k2win) {
		window.setInterface(k2win);
		dd.SetWindowInterface(&window);
	}

//---- LayerManager の管理関連
	virtual void TJS_INTF_METHOD AddLayerManager(K2LM* k2lm) {
		if (k2lm) {
			k2managers.push_back(*k2lm);
			dd.AddLayerManager(static_cast<iTVPLayerManager*>(& k2managers.back()));
		}
	}
	virtual void TJS_INTF_METHOD RemoveLayerManager(K2LM* k2lm) {
		iTVPLayerManager *kzlm = findLayerManager(k2lm);
		if (kzlm) {
			dd.RemoveLayerManager(kzlm);
			std::remove(k2managers.begin(), k2managers.end(), k2lm);
		}
	}

//---- 描画位置・サイズ関連
	virtual void TJS_INTF_METHOD SetTargetWindow       (HWND wnd, bool is_main)                                { dd.SetTargetWindow(wnd, is_main); }
	virtual void TJS_INTF_METHOD SetDestRectangle      (const tTVPRect & rect)                                 { dd.SetDestRectangle(rect); }
//	virtual void TJS_INTF_METHOD SetClipRectangle      (const tTVPRect & rect) = 0;
	virtual void TJS_INTF_METHOD GetSrcSize            (tjs_int &w, tjs_int &h)                                { dd.GetSrcSize(w, h); }
	virtual void TJS_INTF_METHOD NotifyLayerResize     (K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, NotifyLayerResize,      (kzlm)); }
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, NotifyLayerImageChange, (kzlm)); }

//---- ユーザーインターフェース関連
	virtual void TJS_INTF_METHOD OnClick               (tjs_int x, tjs_int y)                                  { dd.OnClick           (x, y); }
	virtual void TJS_INTF_METHOD OnDoubleClick         (tjs_int x, tjs_int y)                                  { dd.OnDoubleClick     (x, y); }
	virtual void TJS_INTF_METHOD OnMouseDown           (tjs_int x, tjs_int y, MBtn mb, tjs_uint32 flags)       { dd.OnMouseDown       (x, y, mb, flags); }
	virtual void TJS_INTF_METHOD OnMouseUp             (tjs_int x, tjs_int y, MBtn mb, tjs_uint32 flags)       { dd.OnMouseUp         (x, y, mb, flags); }
	virtual void TJS_INTF_METHOD OnMouseMove           (tjs_int x, tjs_int y, tjs_uint32 flags)                { dd.OnMouseMove       (x, y, flags); }
	virtual void TJS_INTF_METHOD OnReleaseCapture      ()                                                      { dd.OnReleaseCapture  (); }
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow    ()                                                      { dd.OnMouseOutOfWindow(); }
	virtual void TJS_INTF_METHOD OnKeyDown             (tjs_uint key, tjs_uint32 shift)                        { dd.OnKeyDown         (key, shift); }
	virtual void TJS_INTF_METHOD OnKeyUp               (tjs_uint key, tjs_uint32 shift)                        { dd.OnKeyUp           (key, shift); }
	virtual void TJS_INTF_METHOD OnKeyPress            (tjs_char key)                                          { dd.OnKeyPress        (key); }
	virtual void TJS_INTF_METHOD OnMouseWheel          (tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) { dd.OnMouseWheel      (shift, delta, x, y); }
//	virtual void TJS_INTF_METHOD OnTouchDown           ( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) = 0;
//	virtual void TJS_INTF_METHOD OnTouchUp             ( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) = 0;
//	virtual void TJS_INTF_METHOD OnTouchMove           ( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) = 0;
//	virtual void TJS_INTF_METHOD OnTouchScaling        ( tjs_real startdist, tjs_real curdist, tjs_real cx, tjs_real cy, tjs_int flag ) = 0;
//	virtual void TJS_INTF_METHOD OnTouchRotate         ( tjs_real startangle, tjs_real curangle, tjs_real dist, tjs_real cx, tjs_real cy, tjs_int flag ) = 0;
//	virtual void TJS_INTF_METHOD OnMultiTouch          () = 0;
//	virtual void TJS_INTF_METHOD OnDisplayRotate       ( tjs_int orientation, tjs_int rotate, tjs_int bpp, tjs_int width, tjs_int height ) = 0;
	virtual void TJS_INTF_METHOD RecheckInputState     ()                                                      { dd.RecheckInputState (); }
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor (K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, SetDefaultMouseCursor,(kzlm)); }
	virtual void TJS_INTF_METHOD SetMouseCursor        (K2LM* k2lm, tjs_int cursor)                            { FINDLM(k2lm, kzlm, SetMouseCursor,       (kzlm, cursor)); }
	virtual void TJS_INTF_METHOD GetCursorPos          (K2LM* k2lm, tjs_int &x, tjs_int &y)                    { FINDLM(k2lm, kzlm, GetCursorPos,         (kzlm, x, y)); }
	virtual void TJS_INTF_METHOD SetCursorPos          (K2LM* k2lm, tjs_int  x, tjs_int  y)                    { FINDLM(k2lm, kzlm, SetCursorPos,         (kzlm, x, y)); }
	virtual void TJS_INTF_METHOD WindowReleaseCapture  (K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, WindowReleaseCapture, (kzlm)); }
//	virtual void TJS_INTF_METHOD SetHintText           (iTVPLayerManager * manager, iTJSDispatch2* sender, const ttstr & text) = 0;
	virtual void TJS_INTF_METHOD SetHintText           (K2LM* k2lm, const ttstr & text)                        { FINDLM(k2lm, kzlm, SetHintText,          (kzlm, NULL, text)); } // sender=NULL
	virtual void TJS_INTF_METHOD SetAttentionPoint     (K2LM* k2lm, NLay*layer, tjs_int l, tjs_int t)          { FINDLM(k2lm, kzlm, SetAttentionPoint,    (kzlm, layer, l, t)); }
	virtual void TJS_INTF_METHOD DisableAttentionPoint (K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, DisableAttentionPoint,(kzlm)); }
	virtual void TJS_INTF_METHOD SetImeMode            (K2LM* k2lm, tTVPImeMode mode)                          { FINDLM(k2lm, kzlm, SetImeMode,           (kzlm, mode)); }
	virtual void TJS_INTF_METHOD ResetImeMode          (K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, ResetImeMode,         (kzlm)); }

//---- プライマリレイヤ関連
	virtual NLay* TJS_INTF_METHOD GetPrimaryLayer      ()                                                      { return dd.GetPrimaryLayer(); }
	virtual NLay* TJS_INTF_METHOD GetFocusedLayer      ()                                                      { return dd.GetFocusedLayer(); }
	virtual void  TJS_INTF_METHOD SetFocusedLayer      (NLay* layer)                                           { dd.SetFocusedLayer(layer); }

//---- 再描画関連
	virtual void TJS_INTF_METHOD RequestInvalidation   (const tTVPRect & rect)                                 { dd.RequestInvalidation(rect); }
	virtual void TJS_INTF_METHOD Update()                                                                      { dd.Update(); }
	virtual void TJS_INTF_METHOD Show()                                                                        { dd.Show(); }

//---- LayerManager からの画像受け渡し関連
	virtual void TJS_INTF_METHOD StartBitmapCompletion (K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, StartBitmapCompletion, (kzlm)); }
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted (K2LM* k2lm, tjs_int x, tjs_int y, const void * bits,
		const BITMAPINFO * bitmapinfo, const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)          { FINDLM(k2lm, kzlm, NotifyBitmapCompleted, (kzlm, x, y, bits, bitmapinfo, cliprect, type, opacity)); }
	virtual void TJS_INTF_METHOD EndBitmapCompletion   (K2LM* k2lm)                                            { FINDLM(k2lm, kzlm, EndBitmapCompletion,   (kzlm)); }

//---- デバッグ支援
	virtual void TJS_INTF_METHOD DumpLayerStructure    ()                                                      { dd.DumpLayerStructure(); }
	virtual void TJS_INTF_METHOD SetShowUpdateRect     (bool b)                                                { dd.SetShowUpdateRect(b); }
//	virtual bool TJS_INTF_METHOD SwitchToFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color, bool changeresolution ) = 0;
//	virtual void TJS_INTF_METHOD RevertFromFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color ) = 0;
//	virtual bool TJS_INTF_METHOD WaitForVBlank( tjs_int* in_vblank, tjs_int* delayed ) = 0;
};

