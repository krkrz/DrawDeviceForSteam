#pragma once

#include <windows.h>
#include "tp_stub.h"

namespace ErrorCode
{
	enum {
		NONE,
#define TVP_MSG_DECL( name, deftext ) name,
#define BDD_MSG_DECL( name, deftext ) name,
#include "Messages.inc"
#undef  TVP_MSG_DECL
#undef  BDD_MSG_DECL
		MAX
	};
}

namespace Logger {
	void AddLog(int errcode);
	void AddLog(int errcode, const ttstr &p1);
	void AddLog(int errcode, const ttstr &p1, const ttstr &p2);

	void AddImportantLog(int errcode);
	void AddImportantLog(int errcode, const ttstr &p1);
	void AddImportantLog(int errcode, const ttstr &p1, const ttstr &p2);

	void ThrowException(int errcode);
	void ThrowException(int errcode, const ttstr & p1);
	void ThrowException(int errcode, const ttstr & p1, const ttstr & p2);
}
