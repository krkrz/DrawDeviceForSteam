#include "Krkr2DrawDeviceWrapper.hpp"

#include <vector>
#include <algorithm>

// 吉里吉里Z の tp_stub でないとコンパイルできない（判定はとりあえず片方にしか存在しないdefineで判定）
#ifndef TJS_CII_SET_SUPRECLASS
#error invalid tp_stub.h: kirikiri-z version required.
#endif

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
	virtual iTJSDispatch2 * GetWindowDispatch()                       { return k2 ? GetWindowDispatch(k2) : NULL; }
private:
	static iTJSDispatch2 * GetWindowDispatch(k2_iTVPWindow *);  // TJS_INTF_METHOD じゃないと直接呼べないので規約吸収の細工を入れて呼ぶ必要がある
};

//---------------------------------------------------------------------------
// k2 へのインターフェース変換 (プラグイン側提供→吉里吉里2側参照)

class       kz2_tTVPDrawDevice : public k2_iTVPDrawDevice {
	typedef kz2_tTVPDrawDevice selfclass;

	iTVPDrawDevice &dd;
	k2z_tTVPWindow window;
	std::vector<k2z_tTVPLayerManager> k2managers;
	static std::vector<selfclass*> InstanceList;

	// Constructor
	kz2_tTVPDrawDevice(iTVPDrawDevice &kzdd) : dd(kzdd) {}
	~kz2_tTVPDrawDevice() {}

public:
	// factory method
	static selfclass *Create(iTVPDrawDevice *kzdd) {
		if (!kzdd) return NULL;
		void *ptr = TVP_malloc(sizeof(selfclass));
		selfclass *r = new(ptr) selfclass(*kzdd); // placement new
		AddInstance(r);
		return r;
	}
	// delete instance
	static void Delete(selfclass *inst) {
		if (!inst) return;
		inst->~selfclass();
		TVP_free(inst);
	}

	// check instance
	static bool IsExistInstance() { return !InstanceList.empty(); }

	// instance foreach
	template <class T>
	static void Foreach(T &op, bool withclear = false) {
		for (std::vector<selfclass*>::iterator it = InstanceList.begin(); it != InstanceList.end(); ++it) {
			op(*it);
		}
		if (withclear) InstanceList.clear();
	}
protected:
	// [XXX] thread unsafe : TJSがシングルスレッド動作なので問題ない
	static void    AddInstance(selfclass *inst) { InstanceList.push_back(inst); } 
	static void RemoveInstance(selfclass *inst) { std::remove(InstanceList.begin(), InstanceList.end(), inst); }

	iTVPLayerManager * findLayerManager(k2_iTVPLayerManager *k2lm) {
		std::vector<k2z_tTVPLayerManager>::iterator find = std::find(k2managers.begin(), k2managers.end(), k2lm);
		return (find != k2managers.end()) ? static_cast<iTVPLayerManager*>(&(*find)) : NULL;
	}
// LayerManager変換メソッド呼び出しマクロ
#define FINDLM(k2lm, manager, method, args) do { \
	iTVPLayerManager *manager = findLayerManager(k2lm); \
	if (manager) dd.method args ; \
	} while(0)
public:
	// 表記短縮用typedef
	typedef k2_iTVPLayerManager K2LM;
	typedef tTJSNI_BaseLayer    NLay;
	typedef tTVPMouseButton     MBtn;

//---- オブジェクト生存期間制御
	virtual void TJS_INTF_METHOD Destruct() {
		RemoveInstance(this);
		window.setInterface(NULL);
		k2managers.clear();
		dd.Destruct();
		// ここでは自身を破棄しないので注意（dd.Destruct()の処理先にて消されることを想定）
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
	virtual void TJS_INTF_METHOD SetDestRectangle      (const tTVPRect & rect)                                 { dd.SetClipRectangle(rect); dd.SetDestRectangle(rect); } // [MEMO] Clip=Dest として動作させる
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

std::vector<kz2_tTVPDrawDevice*> kz2_tTVPDrawDevice::InstanceList;

//---------------------------------------------------------------------------
// Krkr2DrawDeviceWrapper 実装

Krkr2DrawDeviceWrapper::Krkr2DrawDeviceWrapper(iTVPDrawDevice *kzdd) : owner(kzdd), k2dd(0)
{
	k2dd = kz2_tTVPDrawDevice::Create(kzdd);
}
Krkr2DrawDeviceWrapper::~Krkr2DrawDeviceWrapper()
{
	kz2_tTVPDrawDevice::Delete(k2dd);
}
extern bool IsKirikiri2(); // cf. v2link.cpp
tTVInteger  Krkr2DrawDeviceWrapper::SwitchGetInterface() const
{
	return IsKirikiri2() ? this->GetInterface() : reinterpret_cast<tTVInteger>(owner);
}
void        Krkr2DrawDeviceWrapper::SwitchEnsureDirect3DObject() const
{
	if (IsKirikiri2()) EnsureDirect3DObject();
	else TVPEnsureDirect3DObject();
}
IDirect3D9* Krkr2DrawDeviceWrapper::SwitchGetDirect3DObjectNoAddRef() const
{
	return IsKirikiri2() ? GetDirect3DObjectNoAddRef() : TVPGetDirect3DObjectNoAddRef();
}
void Krkr2DrawDeviceWrapper::DetachAll()
{
	if (!kz2_tTVPDrawDevice::IsExistInstance()) return;

	class Disabler {
		void ** vfptr;
		// ダミーの仮想関数テーブルを作成
		static void ** CreateDummyTable(size_t count) {
			void **ptr = reinterpret_cast<void**>(TVP_malloc(sizeof (void*) * (count + 4))); // [XXX] TVP_free されない
			if (!ptr) return ptr;
			ptr[0] = ptr[1] = ptr[2] = ptr[3] = 0; // RII用
			void * retasm = SearchRetAsm();
			for (size_t n = 0; n < count; n++) ptr[n + 4] = retasm;
			return ptr + 4;
		}
		// ret(0xc3)を探す
		static void * SearchRetAsm() {
			extern void * TVPGetImportFuncPtr(const char *);
			void * func = TVPGetImportFuncPtr("void ::TJSDoRehash()");
			return func ? memchr(func, 0xc3, 256) : NULL;
		}
	public:
		Disabler(size_t count) { vfptr = CreateDummyTable(count); }
		void operator()(k2_iTVPDrawDevice * dd) const {
			*(reinterpret_cast<void***>(dd)) = vfptr; // [XXX] 仮想関数参照先を書き換える
		}
	} disabler(60); // [XXX] 60 = k2_iTVPDrawDeviceのメソッド個数より大きい数

	kz2_tTVPDrawDevice::Foreach(disabler, true);
}

//---------------------------------------------------------------------------
// 吉里吉里2向け D3D9 関連処理

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
void Krkr2DrawDeviceWrapper::EnsureDirect3DObject()
{
	TVPDoTryBlock(&TVPInitDirect3D, &CatchBlockNoRethrow, NULL, NULL);
}
IDirect3D9 * Krkr2DrawDeviceWrapper::GetDirect3DObjectNoAddRef()
{
	// retrieves IDirect3D9 interface
	return TVPDirect3D;
}

//---------------------------------------------------------------------------
// 吉里吉里2用 GetWindowDispatch 呼び出し規約吸収

#ifdef _MSC_VER

#pragma optimize("", off)
iTJSDispatch2 * k2z_tTVPWindow::GetWindowDispatch(k2_iTVPWindow *window) {
	void *method = (*(reinterpret_cast<void***>(window)))[12]; // __vfptr[12] == GetWindowDispatch
	iTJSDispatch2 * result = 0;
	__asm {
		mov eax, window // eax = this
		call method
		mov result, eax
	}
	return result;
}
#pragma optimize("", on)

#elif defined(__GNUC__)
iTJSDispatch2 * __attribute__((optimize("O0"))) k2z_tTVPWindow::GetWindowDispatch(k2_iTVPWindow *window) {
	void *method = (*(reinterpret_cast<void***>(window)))[12]; // __vfptr[12] == GetWindowDispatch
	iTJSDispatch2 * result = 0;
	__asm__ volatile ("mov %%eax, %1\n\t" // eax = this
		"call *%2\n\t"
		"mov %0, %%eax\n\t"
		: "=r"(result)
		: "r"(window), "r"(method)
		: "%eax"
	);
	return result;
}
#else
#error GetWindowDispatch : compiler not supported.
// do nothing
#endif

