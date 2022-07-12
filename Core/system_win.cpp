// Defines the windows implementation of system
#include "system_win.hpp"

#include <iostream>

#define _WIN32_DCOM
#include <comdef.h>
#include <WbemIdl.h>

#pragma comment(lib, "wbemuuid.lib")

bool system_win::start()
{
	// Check if admin
	bool is_admin = false;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION tkne;
		DWORD cbSize = sizeof(tkne);
		if (GetTokenInformation(hToken, TokenElevation, &tkne, sizeof(tkne), &cbSize)) {
			is_admin = tkne.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	
	if (!is_admin) {
		std::cout << "Not running as admin" << std::endl;

		// Restart with admin permissions
		ShellExecuteA(NULL,
			"runas",
			"x64\\Release\\Core.exe",
			NULL,
			NULL,                        // default dir 
			SW_SHOWNORMAL
		);
		std::exit(0);
	}
	else {
		std::cout << "Running as admin" << std::endl;
	}

	// Init COM library
	HRESULT hres;
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		std::cout << "A" << std::endl;
		return false;
	}

	hres = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL
	);
	if (FAILED(hres)) {
		std::cout << "Couldn't init security" << std::endl;
		CoUninitialize();
		return false;
	}

	if (!connect(L"ROOT\\CIMV2")) {
		std::cout << "Connection 0 failed" << std::endl;
		return false;
	}

	// Need admin for this connection to work
	if (!connect(L"ROOT\\WMI")) {
		std::cout << "Connection 1 failed" << std::endl;
		return false;
	}

	return true;
}

void system_win::stop()
{
}

std::wstring system_win::get_property(const std::string& db, const std::wstring& name, int i)
{
	IWbemLocator* pLoc = static_cast<IWbemLocator*>(this->connections[i].first);
	IWbemServices* pSvc = static_cast<IWbemServices*>(this->connections[i].second);

	HRESULT hres;
	IEnumWbemClassObject* pEnumerator = NULL;

	std::string query_text = "SELECT * FROM " + db;

	hres = static_cast<IWbemServices*>(pSvc)->ExecQuery(
		bstr_t("WQL"),
		bstr_t(query_text.c_str()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator
	);
	if (FAILED(hres)) {
		std::cout << "Query failed" << std::endl;
		static_cast<IWbemServices*>(pSvc)->Release();
		static_cast<IWbemLocator*>(pLoc)->Release();
		CoUninitialize();
		return L"err"; // sketchy at best
	}

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	std::wstring rvalue; // Return value
	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		VariantInit(&vtProp);
		// Get the value of the Name property
		hr = pclsObj->Get(name.c_str(), 0, &vtProp, 0, 0);
		rvalue = std::to_wstring(vtProp.uintVal); // eww
		VariantClear(&vtProp);

		pclsObj->Release();
	}
	return rvalue;
}

bool system_win::connect(const std::wstring& svr_name)
{
	IWbemLocator* pLoc = NULL;
	IWbemServices* pSvc = NULL;

	HRESULT hres;

	// Connect to server
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID*)&pLoc
	);
	if (FAILED(hres)) {
		std::cout << "C" << std::endl;
		CoUninitialize();
		return false;
	}

	hres = pLoc->ConnectServer(
		_bstr_t(svr_name.c_str()),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
	);
	if (FAILED(hres)) {
		std::cout << "Couldn't establish connection to windows server " << std::hex << hres << std::endl;
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);
	if (FAILED(hres)) {
		std::cout << "Could not set proxy blanket" << std::endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	std::pair<void*, void*> builder; // pair builder
	builder.first = pLoc;
	builder.second = pSvc;
	this->connections.push_back(builder);

	return true;
}

void system_win::disconnect(int i)
{

}