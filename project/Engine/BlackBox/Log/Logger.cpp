#include "Logger.h"
#include "StringUtility.h"
#include <dxgidebug.h>

using namespace StringUtility;

namespace Logger {
void Log(const std::string& message) { OutputDebugStringA(message.c_str()); }
void Log(const std::wstring& message) { OutputDebugStringA(ConvertString(message).c_str()); }
} // namespace Logger