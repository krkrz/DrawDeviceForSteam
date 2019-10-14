#pragma once

#include "Krkr2DrawDeviceWrapper.hpp"
#include "simplebinder.hpp"

// 破棄通知インターフェース
struct iDeviceDestructNotify {
	virtual void onDeviceDestruct() = 0;
};

// 破棄通知+吉里吉里2ラッパー切り替え付きテンプレート
template <class DDBASE, bool SUPPORTK2 = true>
class DrawDeviceIntfTmpl : public DDBASE {
	typedef  DDBASE inherited;
protected:
	iDeviceDestructNotify *owner;
	Krkr2DrawDeviceWrapper k2dd; // 吉里吉里2向けDrawDeviceインターフェース互換ラッパー
public:
	DrawDeviceIntfTmpl() : owner(0), k2dd(this), inherited() {}
	void setDestructNotify(iDeviceDestructNotify *owner) { this->owner = owner; }
	virtual void TJS_INTF_METHOD Destruct() {
		if (owner) owner->onDeviceDestruct(); // TJSクラスインスタンス側に破棄通知
		inherited::Destruct();
	}
	tTVInteger GetInterface() const { return k2dd.SwitchGetInterface(); } // 吉里吉里Zか2でインターフェースを自動で切り替える
};

// 吉里吉里2をサポートしない版
template <class DDBASE>
class DrawDeviceIntfTmpl<DDBASE, false> : public DDBASE {
	typedef  DDBASE inherited;
protected:
	iDeviceDestructNotify *owner;
public:
	DrawDeviceIntfTmpl() : owner(0), inherited() {}
	void setDestructNotify(iDeviceDestructNotify *owner) { this->owner = owner; }
	virtual void TJS_INTF_METHOD Destruct() {
		if (owner) owner->onDeviceDestruct(); // TJSクラスインスタンス側に破棄通知
		inherited::Destruct();
	}
	tTVInteger GetInterface() const { return reinterpret_cast<tTVInteger>(this); }
};

// TJS登録用(SimpleBinderのクラス登録都合でマクロ)
#define DRAWDEVICECLASS_ENTRY(CLASSNAME, INTERFACE, DDI) \
private: \
	INTERFACE *DDI; \
	CLASSNAME() : DDI(0) { DDI = new INTERFACE(); DDI->setDestructNotify(static_cast<iDeviceDestructNotify*>(this)); } \
	void ClearInterface() { DDI = 0; } \
public: \
	~CLASSNAME() { if (DDI) DDI->Destruct(); ClearInterface(); } \
	virtual void onDeviceDestruct() { ClearInterface(); } \
	static tjs_error CreateNew(iTJSDispatch2 *objthis, CLASSNAME* &inst, tjs_int num, tTJSVariant **args) { inst = Create(num, args); return TJS_S_OK; } \
	static bool Link(bool link) { return LinkOthers(SimpleBinder::BindUtil(link).Class(GetName(), &CreateNew).Property(TJS_W("interface"), &GetInterface, 0)); } \
	tjs_error GetInterface(tTJSVariant *result) const { if (result) *result = DDI ? DDI->GetInterface() : 0; return TJS_S_OK; }

