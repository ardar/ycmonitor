// PcNTService.h : PcNTService DLL ����ͷ�ļ�
//

#pragma once

//����ȫ�ֺ�������
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCTSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);