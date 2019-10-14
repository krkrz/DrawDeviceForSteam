#include <windows.h>
#include "tp_stub.h"

#include "DrawDeviceForSteam.hpp"
#include "DrawDeviceTmpl.hpp"
#include "PluginName.h"

// 吉里吉里2上での動作が不要であれば0にする
#ifndef USE_K2DD
#define USE_K2DD 1
#endif

#ifndef USE_TJSTEST
#define USE_TJSTEST 0
#endif

// このプラグインで提供するDrawDeviceクラス（tTVPBasicDrawDeviceForSteam +α）
#if USE_K2DD
class DrawDeviceInterface : public DrawDeviceIntfTmpl<tTVPDrawDeviceForSteam>
{
public:
	// 吉里吉里Z通常処理/吉里吉里2互換処理の切り替え
	virtual void        EnsureDirect3DObject()      const {        k2dd.SwitchEnsureDirect3DObject();      }
	virtual IDirect3D9* GetDirect3DObjectNoAddRef() const { return k2dd.SwitchGetDirect3DObjectNoAddRef(); }
};
#else
class DrawDeviceInterface : public DrawDeviceIntfTmpl<tTVPDrawDeviceForSteam, false> {};
#endif

////////////////////////////////////////////////////////////////
// TJS空間にDrawDeviceクラスを登録

class DrawDeviceClass : public iDeviceDestructNotify
{
	// 定型マクロ展開
	DRAWDEVICECLASS_ENTRY(DrawDeviceClass, DrawDeviceInterface, ddi);

private:
	static const tjs_char* GetName() { return PLUGIN_NAME; }

	static DrawDeviceClass* Create(tjs_int num, tTJSVariant **args) {
		return new DrawDeviceClass();
	}

	static bool LinkOthers(SimpleBinder::BindUtil &bind) {
		return (bind
				.Function(TJS_W("recreate"),    &Recreate)
				.Function(TJS_W("present"),     &Present)
				.IsValid());
	}

	tjs_error Recreate(tTJSVariant *result) {
		if (ddi) {
			ddi->SetToRecreateDrawer();
			ddi->EnsureDevice();
		}
		return TJS_S_OK;
	}

	tjs_error Present(tTJSVariant *result) {
		if (ddi) {
			ddi->Present();
		}
		return TJS_S_OK;
	}
};

#if USE_TJSTEST
#include "ErrorCode.hpp"
struct TJSTestCode {
	static tjs_error CreateMessageMapFile(tTJSVariant *result, tTJSVariant *file) {
		TVPCreateMessageMapFile(file->AsStringNoAddRef());
		return TJS_S_OK;
	}
	static tjs_error AddLog         (tTJSVariant *result, tTJSVariant *code, tjs_int optnum, tTJSVariant **opt) {
		if      (optnum == 0) Logger::AddLog((tjs_int)*code);
		else if (optnum >= 1) Logger::AddLog((tjs_int)*code, opt[0]->AsStringNoAddRef());
		else if (optnum >= 2) Logger::AddLog((tjs_int)*code, opt[0]->AsStringNoAddRef(), opt[1]->AsStringNoAddRef());
		return TJS_S_OK;
	}
	static tjs_error AddImportantLog(tTJSVariant *result, tTJSVariant *code, tjs_int optnum, tTJSVariant **opt) {
		if      (optnum == 0) Logger::AddImportantLog((tjs_int)*code);
		else if (optnum >= 1) Logger::AddImportantLog((tjs_int)*code, opt[0]->AsStringNoAddRef());
		else if (optnum >= 2) Logger::AddImportantLog((tjs_int)*code, opt[0]->AsStringNoAddRef(), opt[1]->AsStringNoAddRef());
		return TJS_S_OK;
	}
	static tjs_error ThrowException (tTJSVariant *result, tTJSVariant *code, tjs_int optnum, tTJSVariant **opt) {
		if      (optnum == 0) Logger::ThrowException((tjs_int)*code);
		else if (optnum >= 1) Logger::ThrowException((tjs_int)*code, opt[0]->AsStringNoAddRef());
		else if (optnum >= 2) Logger::ThrowException((tjs_int)*code, opt[0]->AsStringNoAddRef(), opt[1]->AsStringNoAddRef());
		return TJS_S_OK;
	}
	static bool Link(bool link) {
		return (
			SimpleBinder::BindUtil(TJS_W("Debug"), link)
			.Function(TJS_W("CreateMessageMapFile"),  &CreateMessageMapFile)
			.Function(TJS_W("TestDDAddLog"),          &AddLog)
			.Function(TJS_W("TestDDAddImportantLog"), &AddImportantLog)
			.Function(TJS_W("TestDDThrowException"),  &ThrowException)
			.IsValid());
	}
};
#endif

bool Entry(bool link) {
	return (
		DrawDeviceClass::Link(link) &&
#if USE_TJSTEST
		TJSTestCode::Link(link) &&
#endif
		true);
}

static void InitPlugin() {
#if (! USE_K2DD)
	extern bool IsKirikiri2();
	if (IsKirikiri2()) TVPThrowExceptionMessage(TJS_W("%1: krkr2 not supported"), PLUGIN_NAME);
#endif
}
static void QuitPlugin() {
#if USE_K2DD
	Krkr2DrawDeviceWrapper::DetachAll();
#endif
}
bool onV2Link()   { InitPlugin(); return Entry(true);  }
bool onV2Unlink() { QuitPlugin(); return Entry(false); }
void onV2Detach() { QuitPlugin(); }

