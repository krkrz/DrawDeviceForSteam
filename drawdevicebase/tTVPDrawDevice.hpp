#pragma once
#include <windows.h>
#include "tp_stub.h"

// �g���g��Z �� tp_stub �łȂ��ƃR���p�C���ł��Ȃ��i����͂Ƃ肠�����Е��ɂ������݂��Ȃ�define�Ŕ���j
#ifndef TJS_CII_SET_SUPRECLASS
#error invalid tp_stub.h: kirikiri-z version required.
#endif

//---------------------------------------------------------------------------

// �����̃��C���}�l�[�W���Q���Ǘ����邽�߂̃C���^�[�t�F�[�X(tTVPDrawDeviceTmpl�e���v���[�g�����p)
struct iTVPLayerManagerSet
{
	inline tjs_error Add    (iTVPLayerManager * manager) { return TJS_S_OK; }
	inline tjs_error Remove (iTVPLayerManager * manager) { return TJS_S_OK; }

	inline void SetRect(const tTVPRect & rect)           {}
	inline bool GetSrcSize(tjs_int &w, tjs_int &h) const { return false; }

	inline bool FromPrimary(iTVPLayerManager * manager, tjs_int &x, tjs_int &y) const { return false; }
	inline bool ToPrimary  (iTVPLayerManager * manager, tjs_int &x, tjs_int &y) const { return false; }

	inline bool NotifyClick           (tjs_int x, tjs_int y)                                                                          { return false; }
	inline bool NotifyDoubleClick     (tjs_int x, tjs_int y)                                                                          { return false; }
	inline bool NotifyMouseDown       (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)                                    { return false; }
	inline bool NotifyMouseUp         (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)                                    { return false; }
	inline bool NotifyMouseMove       (tjs_int x, tjs_int y, tjs_uint32 flags)                                                        { return false; }
	inline bool ReleaseCapture        ()                                                                                              { return false; }
	inline bool NotifyMouseOutOfWindow()                                                                                              { return false; }
	inline bool NotifyKeyDown         (tjs_uint key, tjs_uint32 shift)                                                                { return false; }
	inline bool NotifyKeyUp           (tjs_uint key, tjs_uint32 shift)                                                                { return false; }
	inline bool NotifyKeyPress        (tjs_char key)                                                                                  { return false; }
	inline bool NotifyMouseWheel      (tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)                                         { return false; }
	inline bool NotifyTouchDown       (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { return false; }
	inline bool NotifyTouchUp         (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { return false; }
	inline bool NotifyTouchMove       (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { return false; }
	inline bool NotifyTouchScaling    (tjs_real startdist,  tjs_real curdist,                 tjs_real cx, tjs_real cy, tjs_int flag) { return false; }
	inline bool NotifyTouchRotate     (tjs_real startangle, tjs_real curangle, tjs_real dist, tjs_real cx, tjs_real cy, tjs_int flag) { return false; }
	inline bool NotifyMultiTouch      ()                                                                                              { return false; }
	inline bool RecheckInputState     ()                                                                                              { return false; }

	inline tTJSNI_BaseLayer * GetPrimaryLayer()            { return NULL; }
	inline tTJSNI_BaseLayer * GetFocusedLayer()            { return NULL; }
	inline bool SetFocusedLayer(tTJSNI_BaseLayer * layer)  { return false; }
	inline bool RequestInvalidation(const tTVPRect & rect) { return false; }
	inline bool UpdateToDrawDevice()                       { return false; }
	inline bool DumpLayerStructure()                       { return false; }

	inline bool CanResize     (iTVPLayerManager * manager) { return false; }
	inline bool CanUpdate     (iTVPLayerManager * manager) { return false; }
	inline bool CanMouseCursor(iTVPLayerManager * manager) { return false; }
	inline bool CanCursorPos  (iTVPLayerManager * manager) { return false; }
	inline bool CanHintText   (iTVPLayerManager * manager) { return false; }
	inline bool CanCapture    (iTVPLayerManager * manager) { return false; }
	inline bool CanImeMode    (iTVPLayerManager * manager) { return false; }
};

// �P�̂̃��C���}�l�[�W���݈̂����� LayerManagerSet
class tTVPLayerManagerSet : public iTVPLayerManagerSet
{
protected:
	iTVPLayerManager * Manager;
	tTVPRect DestRect;

	inline bool ManagerExists() const { return Manager != NULL; }

	virtual const tTVPRect& GetRect(iTVPLayerManager * manager) const { return DestRect; } // �����̃��C���}�l�[�W����DestRect��ς���ꍇ�͌p����ŕύX���邱��
public:
	tTVPLayerManagerSet() : Manager(NULL) { DestRect.clear(); }
	virtual ~tTVPLayerManagerSet() {}

	inline tjs_error Add    (iTVPLayerManager * manager) {
		if (manager) {
			if (Manager) return TJS_E_ACCESSDENYED;
			Manager =  manager;
			Manager->AddRef();
		}
		return TJS_S_OK;
	}
	inline tjs_error Remove (iTVPLayerManager * manager) {
		if (manager) {
			if (!Manager || Manager != manager) return TJS_E_MEMBERNOTFOUND;
			Manager = NULL;
			Manager->Release();
		}
		return TJS_S_OK;
	}

	inline void SetRect(const tTVPRect & rect)           { DestRect = rect; }
	inline bool GetSrcSize(tjs_int &w, tjs_int &h) const { return Manager && Manager->GetPrimaryLayerSize(w, h); }

	inline bool FromPrimary(iTVPLayerManager * manager, tjs_int  &x, tjs_int  &y) const { return Transform::FromPrimary(manager, GetRect(manager), x, y); }
	inline bool ToPrimary  (iTVPLayerManager * manager, tjs_int  &x, tjs_int  &y) const { return Transform::ToPrimary  (manager, GetRect(manager), x, y); }
	inline bool ToPrimary  (iTVPLayerManager * manager, tjs_real &x, tjs_real &y) const { return Transform::ToPrimary  (manager, GetRect(manager), x, y); } // iTVPLayerManagerSet �ɂ͊܂܂�Ȃ�
	inline bool ToPrimary  (iTVPLayerManager * manager, tTVPRect &rect)           const { return Transform::ToPrimary  (manager, GetRect(manager), rect); } // iTVPLayerManagerSet �ɂ͊܂܂�Ȃ�

	inline bool NotifyClick           (tjs_int x, tjs_int y)                                                                          { return ToPrimary(Manager, x, y) && (Manager->NotifyClick      (x, y),            true); }
	inline bool NotifyDoubleClick     (tjs_int x, tjs_int y)                                                                          { return ToPrimary(Manager, x, y) && (Manager->NotifyDoubleClick(x, y),            true); }
	inline bool NotifyMouseDown       (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)                                    { return ToPrimary(Manager, x, y) && (Manager->NotifyMouseDown  (x, y, mb, flags), true); }
	inline bool NotifyMouseUp         (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)                                    { return ToPrimary(Manager, x, y) && (Manager->NotifyMouseUp    (x, y, mb, flags), true); }
	inline bool NotifyMouseMove       (tjs_int x, tjs_int y, tjs_uint32 flags)                                                        { return ToPrimary(Manager, x, y) && (Manager->NotifyMouseMove  (x, y,     flags), true); }
	inline bool ReleaseCapture        ()                                                                                              { return Manager && (Manager->ReleaseCapture        (),           true); }
	inline bool NotifyMouseOutOfWindow()                                                                                              { return Manager && (Manager->NotifyMouseOutOfWindow(),           true); }
	inline bool NotifyKeyDown         (tjs_uint key, tjs_uint32 shift)                                                                { return Manager && (Manager->NotifyKeyDown         (key, shift), true); }
	inline bool NotifyKeyUp           (tjs_uint key, tjs_uint32 shift)                                                                { return Manager && (Manager->NotifyKeyUp           (key, shift), true); }
	inline bool NotifyKeyPress        (tjs_char key)                                                                                  { return Manager && (Manager->NotifyKeyPress        (key),        true); }
	inline bool NotifyMouseWheel      (tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)                                         { return ToPrimary(Manager, x, y) && (Manager->NotifyMouseWheel (shift, delta, x, y), true); }
	inline bool NotifyTouchDown       (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { return ToPrimary(Manager, x, y) && (Manager->NotifyTouchDown  (x, y, cx, cy, id),   true); }
	inline bool NotifyTouchUp         (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { return ToPrimary(Manager, x, y) && (Manager->NotifyTouchUp    (x, y, cx, cy, id),   true); }
	inline bool NotifyTouchMove       (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { return ToPrimary(Manager, x, y) && (Manager->NotifyTouchMove  (x, y, cx, cy, id),   true); }
	inline bool NotifyTouchScaling    (tjs_real startdist,  tjs_real curdist,                 tjs_real cx, tjs_real cy, tjs_int flag) { return Manager && (Manager->NotifyTouchScaling    (startdist,  curdist,        cx, cy, flag), true); }
	inline bool NotifyTouchRotate     (tjs_real startangle, tjs_real curangle, tjs_real dist, tjs_real cx, tjs_real cy, tjs_int flag) { return Manager && (Manager->NotifyTouchRotate     (startangle, curangle, dist, cx, cy, flag), true); }
	inline bool NotifyMultiTouch      ()                                                                                              { return Manager && (Manager->NotifyMultiTouch      (), true); }
	inline bool RecheckInputState     ()                                                                                              { return Manager && (Manager->RecheckInputState     (), true); }

	inline tTJSNI_BaseLayer * GetPrimaryLayer()            { return Manager ? Manager->GetPrimaryLayer() : NULL; }
	inline tTJSNI_BaseLayer * GetFocusedLayer()            { return Manager ? Manager->GetFocusedLayer() : NULL; }
	inline bool SetFocusedLayer(tTJSNI_BaseLayer * layer)  { return Manager && (Manager->SetFocusedLayer(layer), true); }
	inline bool RequestInvalidation(const tTVPRect & rect) { tTVPRect trans(rect); return ToPrimary(Manager, trans) && (Manager->RequestInvalidation(trans), true); }
	inline bool UpdateToDrawDevice()                       { return Manager && (Manager->UpdateToDrawDevice(), true); }
	inline bool DumpLayerStructure()                       { return Manager && (Manager->DumpLayerStructure(), true); }

	inline bool CanResize     (iTVPLayerManager * manager) { return ManagerExists(); }
	inline bool CanUpdate     (iTVPLayerManager * manager) { return ManagerExists(); }
	inline bool CanMouseCursor(iTVPLayerManager * manager) { return ManagerExists(); }
	inline bool CanCursorPos  (iTVPLayerManager * manager) { return ManagerExists(); }
	inline bool CanHintText   (iTVPLayerManager * manager) { return ManagerExists(); }
	inline bool CanCapture    (iTVPLayerManager * manager) { return ManagerExists(); }
	inline bool CanImeMode    (iTVPLayerManager * manager) { return ManagerExists(); }

protected:
	// ���W�ϊ��c�[��
	struct Transform {
		tjs_int pl_w, pl_h; // PrimaryLayer size
		tjs_int x, y, w, h; // DestRect

		Transform() : pl_w(-1), pl_h(-1), x(0), y(0), w(0), h(0) {}
		Transform(const iTVPLayerManager *manager) : x(0), y(0), w(0), h(0) { this->manager(manager); if (isValid()) w=pl_w, h=pl_h; }
		Transform(const iTVPLayerManager *manager, const tTVPRect &rect)    { this->manager(manager); this->rect(rect); }

		bool isValid() const { return pl_w >= 0; }
		bool manager(const iTVPLayerManager *manager) {
			pl_w = pl_h = 0;
			if (manager && manager->GetPrimaryLayerSize(pl_w, pl_h)) return true;
			pl_w = pl_h = -1; // invalid
			return false;
		}
		void rect(const tTVPRect &rect) {
			x = rect.left;
			y = rect.top;
			w = rect.get_width();
			h = rect.get_height();
		}
		template <typename P>
		bool toPrimary(P &rx, P &ry) const {
			if (!isValid()) return false;
			rx = (w ? ((rx - x) * pl_w / w) : 0);
			ry = (h ? ((ry - y) * pl_h / h) : 0);
			return true;
		}
		template <typename P>
		bool fromPrimary(P &rx, P &ry) const {
			if (!isValid()) return false;
			rx = (pl_w ? (rx * w / pl_w) : 0) + x;
			ry = (pl_h ? (ry * h / pl_h) : 0) + y;
			return true;
		}

		//! @brief		Device��LayerManager�����̍��W�̕ϊ����s��
		static bool ToPrimary(iTVPLayerManager * manager, const tTVPRect &rect, tjs_int &x, tjs_int &y) {
			Transform trans(manager, rect);
			return trans.toPrimary(x, y);
		}
		//! @brief		Device��LayerManager�����̍��W�̕ϊ����s�� (tjs_real version)
		static bool ToPrimary(iTVPLayerManager * manager, const tTVPRect &rect, tjs_real &x, tjs_real &y) {
			Transform trans(manager, rect);
			return trans.toPrimary(x, y);
		}
		//! @brief		Device��LayerManager�����̍��W�̕ϊ����s�� (tTVPRect version)
		static bool ToPrimary(iTVPLayerManager * manager, const tTVPRect &rect, tTVPRect & fromto) {
			Transform trans(manager, rect);
			if (trans.toPrimary(fromto.left, fromto.top) && trans.toPrimary(fromto.right, fromto.bottom)) {
				++fromto.right, ++fromto.bottom; // �����_�؂�̂ẴP�[�X���t�H���[���邽�߂� +1 ����
				return true;
			}
			return false;
		}

		//! @brief		LayerManager��Device�����̍��W�̕ϊ����s��
		static bool FromPrimary(iTVPLayerManager * manager, const tTVPRect &rect, tjs_int &x, tjs_int &y) {
			Transform trans(manager, rect);
			return trans.fromPrimary(x, y);
		}
	};
};

// �����̃��C���}�l�[�W���������� LayerManagerSet
class tTVPMultiLayerManagerSet : public tTVPLayerManagerSet
{
	// [TODO] ���������\��
	tTVPMultiLayerManagerSet();
};

//---------------------------------------------------------------------------

template <class MANAGERSET>
class tTVPDrawDeviceTmpl : public iTVPDrawDevice
{
protected:
	iTVPWindow * Window;				//!< �E�B���h�E�C���^�[�t�F�[�X
	MANAGERSET ManagerSet;				//!< �}�l�[�W���Q�Ǘ��C���^�[�t�F�[�X

	tTVPRect DestRect;					//!< �`���ʒu
	tTVPRect ClipRect;					//!< �N���b�s���O��`

	//! @brief		ManagerSet�G���[����
	virtual void OnManagerSetError(tjs_error r) {}

public:
	//! @brief		�R���X�g���N�^
	tTVPDrawDeviceTmpl() : Window(NULL) {
		DestRect.clear();
		ClipRect.clear();
	}

	//! @brief		�f�X�g���N�^
	virtual ~tTVPDrawDeviceTmpl() {
		Window = NULL;
	}

//---- �I�u�W�F�N�g�������Ԑ���
	virtual void TJS_INTF_METHOD Destruct()                                                                                                        { delete this; }

//---- window interface �֘A
	virtual void TJS_INTF_METHOD SetWindowInterface(iTVPWindow * window)                                                                           { Window = window; }

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager   (iTVPLayerManager * manager)                                                                    { tjs_error r = ManagerSet.Add   (manager); if (TJS_FAILED(r)) OnManagerSetError(r); }
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager)                                                                    { tjs_error r = ManagerSet.Remove(manager); if (TJS_FAILED(r)) OnManagerSetError(r); }

//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetDestRectangle      (const tTVPRect & rect)                                                                     { DestRect = rect; ManagerSet.SetRect(rect); }
	virtual void TJS_INTF_METHOD SetClipRectangle      (const tTVPRect & rect)                                                                     { ClipRect = rect; }
	virtual void TJS_INTF_METHOD GetSrcSize            (tjs_int &w, tjs_int &h)                                                                    { if(!ManagerSet.GetSrcSize(w, h)) w = h = 0; }
	virtual void TJS_INTF_METHOD NotifyLayerResize     (iTVPLayerManager * manager)                                                                { if (ManagerSet.CanResize(manager)) Window->NotifySrcResize(); }
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(iTVPLayerManager * manager)                                                                { if (ManagerSet.CanUpdate(manager)) Window->RequestUpdate(); }

//---- ���[�U�[�C���^�[�t�F�[�X�֘A
	// window �� drawdevice
	virtual void TJS_INTF_METHOD OnClick           (tjs_int x, tjs_int y)                                                                          { ManagerSet.NotifyClick      (x, y); }
	virtual void TJS_INTF_METHOD OnDoubleClick     (tjs_int x, tjs_int y)                                                                          { ManagerSet.NotifyDoubleClick(x, y); }
	virtual void TJS_INTF_METHOD OnMouseDown       (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)                                    { ManagerSet.NotifyMouseDown  (x, y, mb, flags); }
	virtual void TJS_INTF_METHOD OnMouseUp         (tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)                                    { ManagerSet.NotifyMouseUp    (x, y, mb, flags); }
	virtual void TJS_INTF_METHOD OnMouseMove       (tjs_int x, tjs_int y, tjs_uint32 flags)                                                        { ManagerSet.NotifyMouseMove  (x, y,     flags); }
	virtual void TJS_INTF_METHOD OnReleaseCapture  ()                                                                                              { ManagerSet.ReleaseCapture(); }
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow()                                                                                              { ManagerSet.NotifyMouseOutOfWindow(); }
	virtual void TJS_INTF_METHOD OnKeyDown         (tjs_uint key, tjs_uint32 shift)                                                                { ManagerSet.NotifyKeyDown (key, shift); }
	virtual void TJS_INTF_METHOD OnKeyUp           (tjs_uint key, tjs_uint32 shift)                                                                { ManagerSet.NotifyKeyUp   (key, shift); }
	virtual void TJS_INTF_METHOD OnKeyPress        (tjs_char key)                                                                                  { ManagerSet.NotifyKeyPress(key); }
	virtual void TJS_INTF_METHOD OnMouseWheel      (tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)                                         { ManagerSet.NotifyMouseWheel(shift, delta, x, y); }
	virtual void TJS_INTF_METHOD OnTouchDown       (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { ManagerSet.NotifyTouchDown (x, y, cx, cy, id); }
	virtual void TJS_INTF_METHOD OnTouchUp         (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { ManagerSet.NotifyTouchUp   (x, y, cx, cy, id); }
	virtual void TJS_INTF_METHOD OnTouchMove       (tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id)                               { ManagerSet.NotifyTouchMove (x, y, cx, cy, id); }
	virtual void TJS_INTF_METHOD OnTouchScaling    (tjs_real startdist,  tjs_real curdist,                 tjs_real cx, tjs_real cy, tjs_int flag) { ManagerSet.NotifyTouchScaling(startdist,  curdist,        cx, cy, flag); }
	virtual void TJS_INTF_METHOD OnTouchRotate     (tjs_real startangle, tjs_real curangle, tjs_real dist, tjs_real cx, tjs_real cy, tjs_int flag) { ManagerSet.NotifyTouchRotate (startangle, curangle, dist, cx, cy, flag); }
	virtual void TJS_INTF_METHOD OnMultiTouch      ()                                                                                              { ManagerSet.NotifyMultiTouch(); }
	virtual void TJS_INTF_METHOD RecheckInputState ()                                                                                              { ManagerSet.RecheckInputState(); }

	// layer manager �� drawdevice
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor(iTVPLayerManager * manager)                                                                 { if (ManagerSet.CanMouseCursor(manager)) Window->SetDefaultMouseCursor(); }
	virtual void TJS_INTF_METHOD SetMouseCursor       (iTVPLayerManager * manager, tjs_int cursor)                                                 { if (ManagerSet.CanMouseCursor(manager)) Window->SetMouseCursor(cursor); }
	virtual void TJS_INTF_METHOD GetCursorPos         (iTVPLayerManager * manager, tjs_int &x, tjs_int &y)                                         { if (ManagerSet.CanCursorPos  (manager) && (Window->GetCursorPos(x, y), ManagerSet.ToPrimary(manager, x, y))) return; /*failed*/x = y = 0; }
	virtual void TJS_INTF_METHOD SetCursorPos         (iTVPLayerManager * manager, tjs_int  x, tjs_int  y)                                         { if (ManagerSet.CanCursorPos  (manager) && ManagerSet.FromPrimary(manager, x, y)) Window->SetCursorPos(x, y); }
	virtual void TJS_INTF_METHOD SetHintText          (iTVPLayerManager * manager, iTJSDispatch2* sender, const ttstr & text)                      { if (ManagerSet.CanHintText   (manager)) Window->SetHintText(sender, text); }
	virtual void TJS_INTF_METHOD WindowReleaseCapture (iTVPLayerManager * manager)                                                                 { if (ManagerSet.CanCapture    (manager)) Window->WindowReleaseCapture(); }

	virtual void TJS_INTF_METHOD SetAttentionPoint    (iTVPLayerManager * manager, tTJSNI_BaseLayer *layer, tjs_int l, tjs_int t)                  { if (ManagerSet.CanImeMode    (manager) && ManagerSet.FromPrimary(manager, l, t)) Window->SetAttentionPoint(layer, l, t); }
	virtual void TJS_INTF_METHOD DisableAttentionPoint(iTVPLayerManager * manager)                                                                 { if (ManagerSet.CanImeMode    (manager)) Window->DisableAttentionPoint(); }
	virtual void TJS_INTF_METHOD SetImeMode           (iTVPLayerManager * manager, tTVPImeMode mode)                                               { if (ManagerSet.CanImeMode    (manager)) Window->SetImeMode(mode); }
	virtual void TJS_INTF_METHOD ResetImeMode         (iTVPLayerManager * manager)                                                                 { if (ManagerSet.CanImeMode    (manager)) Window->ResetImeMode(); }

//---- �v���C�}�����C���֘A
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer()                                                                                   { return ManagerSet.GetPrimaryLayer(); } // called from Window.primaryLayer property
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer()                                                                                   { return ManagerSet.GetFocusedLayer(); } // called from Window.focusedLayer property
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer)                                                                         { ManagerSet.SetFocusedLayer(layer); }

//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect)                                                                        { ManagerSet.RequestInvalidation(rect); }
	virtual void TJS_INTF_METHOD Update()                                                                                                          { ManagerSet.UpdateToDrawDevice(); }
	virtual void TJS_INTF_METHOD Show()                                                                                                            {} // [STUB] do nothing...

//---- �f�o�b�O�x��
	virtual void TJS_INTF_METHOD DumpLayerStructure()                                                                                              { ManagerSet.DumpLayerStructure(); }
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b)                                                                                         {} // do nothing...

//---- �f�B�X�v���C�֘A (STUB: �g���g��Z only)
	virtual bool TJS_INTF_METHOD WaitForVBlank( tjs_int* in_vblank, tjs_int* delayed )                                                             { return false; }
	virtual bool TJS_INTF_METHOD SwitchToFullScreen  ( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color, bool changeresolution )  { return false; }
	virtual void TJS_INTF_METHOD RevertFromFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color )                         {}
	virtual void TJS_INTF_METHOD OnDisplayRotate( tjs_int orientation, tjs_int rotate, tjs_int bpp, tjs_int width, tjs_int height )                {}
};


//---------------------------------------------------------------------------
// �e���v���[�g�G�C���A�X

typedef tTVPDrawDeviceTmpl<tTVPLayerManagerSet>      tTVPDrawDevice;
typedef tTVPDrawDeviceTmpl<tTVPMultiLayerManagerSet> tTVPMultiPrimaryDrawDevice; // [TODO] �����̃v���C�}�������Ă�DrawDevice
