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
	// �X�V���Ȃ������ꍇ��Show����(ContinuousHandler�o�^�p)
	void Present() {
		if(!GetShouldShow()) {
			SetShouldShow(true);
			Show();
		}
	}
};

