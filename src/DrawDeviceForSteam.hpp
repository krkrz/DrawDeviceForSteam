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
	// 更新がなかった場合にShowする(ContinuousHandler登録用)
	void Present() {
		if(!GetShouldShow()) {
			SetShouldShow(true);
			Show();
		}
	}
};

