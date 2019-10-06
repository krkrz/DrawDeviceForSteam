#include <windows.h>
#include "tp_stub.h"
#include "simplebinder.hpp"

#include "BasicDrawDeviceForSteam.h"

#ifndef USE_K2DD
#define USE_K2DD 0
#endif

#ifndef USE_TJSTEST
#define USE_TJSTEST 0
#endif

// 破棄通知インターフェース
struct iDeviceDestructNotify {
	virtual void onDeviceDestruct() = 0;
};

// このプラグインで提供するDrawDeviceクラス（tTVPBasicDrawDeviceForSteam +α）
class DrawDeviceInterface
	: public tTVPBasicDrawDeviceForSteam {
	typedef  tTVPBasicDrawDeviceForSteam inherited;
	iDeviceDestructNotify *owner;
public:
	DrawDeviceInterface(iDeviceDestructNotify *owner) : owner(owner), inherited() {}
	virtual void TJS_INTF_METHOD Destruct() {
		owner->onDeviceDestruct();
		inherited::Destruct();
	}
	tTVInteger GetInterface() const {
		return reinterpret_cast<tTVInteger>(static_cast<const iTVPDrawDevice*>(this));
	}
};

#if USE_K2DD
#include "Krkr2DrawDeviceWrapper.hpp"
// (あまり意味はないけど)吉里吉里2でも使えるようにする細工(cf. Krkr2DrawDeviceWrapper.hpp)
class DrawDeviceForKrkr2
	: public kz2_tTVPDrawDevice {
	typedef  kz2_tTVPDrawDevice inherited;
public:
	DrawDeviceForKrkr2(iTVPDrawDevice *dd) : inherited(*dd) {}
	virtual void TJS_INTF_METHOD Destruct() {
		inherited::Destruct();
		delete this;
	}
	tTVInteger GetInterface() const {
		return reinterpret_cast<tTVInteger>(static_cast<const k2_iTVPDrawDevice*>(this));
	}
};
#endif


////////////////////////////////////////////////////////////////
// TJS空間にDrawDeviceクラスを登録

extern bool IsKirikiri2(); // cf. v2link.cpp

class DrawDeviceClass : public iDeviceDestructNotify {
	DrawDeviceInterface *ddi;
#if USE_K2DD
	DrawDeviceForKrkr2  *dk2;
#endif
public:
	DrawDeviceClass() : ddi(0)
#if USE_K2DD
		, dk2(0)
#endif
	{
		ddi = new DrawDeviceInterface(this);
		if (IsKirikiri2()) {
#if USE_K2DD
			dk2 = new DrawDeviceForKrkr2(ddi);
#else
			TVPThrowExceptionMessage(TJS_W("BasicDrawDeviceForSteam: Kirikiri2 not supported"));
#endif
		}
	}
	~DrawDeviceClass() {
#if USE_K2DD
		if (dk2) dk2->Destruct(); // ddiも併せてDestructされる
		else
#endif
		if (ddi) ddi->Destruct();
		ClearInterface();
	}
	virtual void onDeviceDestruct() { ClearInterface(); }
	void ClearInterface() {
#if USE_K2DD
		dk2 = 0;
#endif
		ddi = 0;
	}

	static tjs_error CreateNew(iTJSDispatch2 *objthis, DrawDeviceClass* &inst, tjs_int num, tTJSVariant **args) {
		inst = new DrawDeviceClass();
		return TJS_S_OK;
	}
	static bool Link(const tjs_char *name, bool link) {
		return (SimpleBinder::BindUtil(link)
		 .Class(name, &CreateNew)
		 .Property(TJS_W("interface"),   &GetInterface, 0)
		 .Function(TJS_W("recreate"),    &Recreate)
		 .IsValid());
	}

	tjs_error GetInterface(tTJSVariant *result) const {
		if (result) *result = (
#if USE_K2DD
			dk2 ? dk2->GetInterface() :
#endif
			ddi ? ddi->GetInterface() :
			0);
		return TJS_S_OK;
	}

	tjs_error Recreate(tTJSVariant *result) {
		if (ddi) {
			ddi->SetToRecreateDrawer();
			ddi->EnsureDevice();
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
		DrawDeviceClass::Link(TJS_W("DrawDeviceForSteam"), link) &&
#if USE_TJSTEST
		TJSTestCode::Link(link) &&
#endif
		true);
}

bool onV2Link()   { return Entry(true);  }
bool onV2Unlink() { return Entry(false); }
void onV2Detach() {}

