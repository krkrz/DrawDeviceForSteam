#include <windows.h>
#include "tp_stub.h"
#include "simplebinder.hpp"

#include "BasicDrawDeviceForSteam.h"
#include "Krkr2DrawDeviceWrapper.hpp"

#ifndef USE_TJSTEST
#define USE_TJSTEST 0
#endif

extern bool        IsKirikiriZ(); // 吉里吉里種別判定 (cf. v2link.cpp)

// 破棄通知インターフェース
struct iDeviceDestructNotify {
	virtual void onDeviceDestruct() = 0;
};

// このプラグインで提供するDrawDeviceクラス（tTVPBasicDrawDeviceForSteam +α）
class DrawDeviceInterface
	: public tTVPBasicDrawDeviceForSteam {
	typedef  tTVPBasicDrawDeviceForSteam inherited;
	iDeviceDestructNotify *owner;
	kz2_tTVPDrawDevice k2dd; // 吉里吉里2向けDrawDeviceインターフェース互換ラッパー
	bool kz;
public:
	DrawDeviceInterface(iDeviceDestructNotify *owner) : owner(owner), k2dd(*this), kz(IsKirikiriZ()), inherited() {}
	virtual void TJS_INTF_METHOD Destruct() {
		owner->onDeviceDestruct();
		inherited::Destruct();
	}
	tTVInteger GetInterface() const {
		return kz ? reinterpret_cast<tTVInteger>(static_cast<const iTVPDrawDevice*>(this))
			:       reinterpret_cast<tTVInteger>(static_cast<const k2_iTVPDrawDevice*>(&k2dd));
	}

	// 吉里吉里2互換処理
	virtual void        EnsureDirect3DObject()      const { if    (kz)  inherited::EnsureDirect3DObject(); else  Krkr2EnsureDirect3DObject();      }
	virtual IDirect3D9* GetDirect3DObjectNoAddRef() const { return kz ? inherited::GetDirect3DObjectNoAddRef() : Krkr2GetDirect3DObjectNoAddRef(); }
};


////////////////////////////////////////////////////////////////
// TJS空間にDrawDeviceクラスを登録

class DrawDeviceClass : public iDeviceDestructNotify {
	DrawDeviceInterface *ddi;
public:
	DrawDeviceClass() : ddi(0)
	{
		ddi = new DrawDeviceInterface(this);
	}
	~DrawDeviceClass() {
		if (ddi) ddi->Destruct();
		ClearInterface();
	}
	virtual void onDeviceDestruct() { ClearInterface(); } // called from iDeviceDestructNotify
	void ClearInterface() {
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
		if (result) *result = ddi ? ddi->GetInterface() : 0;
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

