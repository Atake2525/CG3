#pragma once
#include <string>

// ログ出力
namespace Logger {
void Log(const std::string& message);

void Log(const std::wstring& message);
}; // namespace Logger
