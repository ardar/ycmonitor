// PcNTService.h : PcNTService DLL 的主头文件
//

#pragma once

//定义全局函数变量
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCTSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);