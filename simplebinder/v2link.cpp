#include <windows.h>
#include "tp_stub.h"

#define EXPORT(hr) extern "C" __declspec(dllexport) hr __stdcall

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:V2Link=_V2Link@4")
#pragma comment(linker, "/EXPORT:V2Unlink=_V2Unlink@0")
#endif
#ifdef __GNUC__
asm (".section .drectve");
asm (".ascii \" -export:V2Link=V2Link@4 -export:V2Unlink=V2Unlink@0\"");
#endif

HINSTANCE V2LinkModule = 0;

int WINAPI
DllMain(HINSTANCE hinst, unsigned long reason, void* /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH) {
		V2LinkModule = hinst;
	}
	return 1;
}

static tjs_int GlobalRefCountAtInit = 0;

static struct KirikiriTypeChecker {
	KirikiriTypeChecker() : exporter(0), type(0) {}
	void setup(iTVPFunctionExporter *exporter) { this->exporter = exporter; }
	int getType() {
		if (type == 0) type = GetType(exporter);
		return type;
	}
private:
	iTVPFunctionExporter *exporter;
	int type; // 0:unchecked, -1:unknown, 1:Z, 2:2
	static int GetType(iTVPFunctionExporter *exporter) {
		if (exporter) {
			const ttstr prefix (TJS_W("bool ::TVPGetFileVersionOf(const "));
			const ttstr postfix(TJS_W(" *,tjs_int &,tjs_int &,tjs_int &,tjs_int &)"));
			if (HasExportedFunction(exporter, ttstr(prefix + TJS_W("wchar_t") + postfix).c_str())) return 1; // KZ
			if (HasExportedFunction(exporter, ttstr(prefix + TJS_W("char")    + postfix).c_str())) return 2; // K2
		}
		return -1;
	}
	static bool HasExportedFunction(iTVPFunctionExporter *exporter, const tjs_char *name, void *ptr = NULL) {
		return exporter->QueryFunctions(&name, &ptr, 1) && ptr;
	}
} KirikiriType;

// extern reference from other soruces
bool IsKirikiriZ() { return KirikiriType.getType() == 1; }
bool IsKirikiri2() { return KirikiriType.getType() == 2; }

extern bool onV2Link();
extern bool onV2Unlink();
extern void onV2Detach();

EXPORT(HRESULT) V2Link(iTVPFunctionExporter *exporter)
{
	TVPInitImportStub(exporter);
	KirikiriType.setup(exporter);

	if (!onV2Link()) return E_FAIL;

	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	return S_OK;
}
EXPORT(HRESULT) V2Unlink()
{
	iTJSDispatch2 *global = TVPGetScriptDispatch();
	if (global) {
		global->Release();
		if (TVPPluginGlobalRefCount > GlobalRefCountAtInit ||
			!onV2Unlink()) return E_FAIL;
	} else {
		onV2Detach();
	}

	TVPUninitImportStub();
	return S_OK;
}

