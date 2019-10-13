#pragma once

#include "BasicDrawDevice.h"

class tTVPDrawDeviceForSteam : public tTVPBasicDrawDevice
{
protected:
	virtual void CheckRecreateOnSizeChanged(const tTVPRect & rect) {
		// for Steam Experimental : always DestroyD3DDevice
		SetToRecreateDrawer();
	}
public:
	// çXêVÇ™Ç»Ç©Ç¡ÇΩèÍçáÇ…ShowÇ∑ÇÈ(ContinuousHandlerìoò^óp)
	void Present() {
		if(!GetShouldShow()) {
			SetShouldShow(true);
			Show();
		}
	}
};

