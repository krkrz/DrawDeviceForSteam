#include "ErrorCode.hpp"

#include "PluginName.h"

struct KeyText {
	const char * key;
	const char * text;
} static ErrorCodeMap[] = {
	{ "", "" }, // NONE
#define TVP_MSG_DECL( name, deftext ) { "TVP" #name, deftext },
#define BDD_MSG_DECL( name, deftext ) { "TVPBasicDrawDevice" #name, deftext },
#include "Messages.inc"
#undef  TVP_MSG_DECL
#undef  BDD_MSG_DECL
};

static bool GetErrorCodeText(int code, ttstr &text) {
	if (code < 0 || code >= ErrorCode::MAX) return false;
	text.Clear();
	if (!code) return true;
	ttstr key(ErrorCodeMap[code].key);
	text = TJSGetMessageMapMessage(key.c_str());
	if (text.IsEmpty()) {
		// [MEMO] TJSGetMessageMapMessage ÇÕãgó¢ãgó¢ZÇ≈ÇÕTVPånÇÕìoò^Ç≥ÇÍÇƒÇ¢Ç»Ç¢ÅiÉoÉOÇ¡Ç€Ç¢Åj
		text = ttstr(ErrorCodeMap[code].text);
	}
	if (text.StartsWith(TJS_W("BasicDrawDevice"))) {
		text = ttstr(PLUGIN_NAME) + ttstr(text.c_str() + 15); // 15="BasicDrawDevice".length
	}
	return true;
}


namespace Logger
{
	static ttstr GetMessage(int code) {
		ttstr text;
		if (!GetErrorCodeText(code, text)) {
			text = TVPFormatMessage(TJS_W("%1 UnknownInternalErrorCode: %2"), ttstr(PLUGIN_NAME), ttstr(code));
		}
		return text;
	}
	static ttstr FormatMessage(int code, const ttstr &p1) {
		ttstr msg(GetMessage(code));
		return TVPFormatMessage(msg.c_str(), p1);
	}
	static ttstr FormatMessage(int code, const ttstr &p1, const ttstr &p2) {
		ttstr msg(GetMessage(code));
		return TVPFormatMessage(msg.c_str(), p1, p2);
	}

	void AddLog(int code)                                             { TVPAddLog(GetMessage   (code));         }
	void AddLog(int code, const ttstr &p1)                            { TVPAddLog(FormatMessage(code, p1    )); }
	void AddLog(int code, const ttstr &p1, const ttstr &p2)           { TVPAddLog(FormatMessage(code, p1, p2)); }

	void AddImportantLog(int code)                                    { TVPAddImportantLog(GetMessage   (code));         }
	void AddImportantLog(int code, const ttstr &p1)                   { TVPAddImportantLog(FormatMessage(code, p1    )); }
	void AddImportantLog(int code, const ttstr &p1, const ttstr &p2)  { TVPAddImportantLog(FormatMessage(code, p1, p2)); }

	void ThrowException(int code)                                     { TVPThrowExceptionMessage(GetMessage(code).c_str());         }
	void ThrowException(int code, const ttstr & p1)                   { TVPThrowExceptionMessage(GetMessage(code).c_str(), p1    ); }
	void ThrowException(int code, const ttstr & p1, const ttstr & p2) { TVPThrowExceptionMessage(GetMessage(code).c_str(), p1, p2); }
}
