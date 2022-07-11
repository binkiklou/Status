// Defines the windows implementation of system
#include "system_win.hpp"

#include <iostream>

#define _WIN32_DCOM
#include <comdef.h>
#include <WbemIdl.h>

#pragma comment(lib, "wbemuuid.lib")

bool system_win::start()
{
	this->m_pLoc = NULL;
	this->m_pSvc = NULL;

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
		std::cout << "B" << std::endl;
		CoUninitialize();
		return false;
	}

	// Connect to server
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID*) &this->m_pLoc
	);
	if (FAILED(hres)) {
		std::cout << "C" << std::endl;
		CoUninitialize();
		return false;
	}

	// This doesn't need to exist, right?
	IWbemServices* pSvc = static_cast<IWbemServices*>(this->m_pSvc);

	hres = static_cast<IWbemLocator*>(m_pLoc)->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
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
		static_cast<IWbemLocator*>(this->m_pLoc)->Release();
		CoUninitialize();
		return false;
	}

	this->m_pSvc = pSvc;

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
		static_cast<IWbemLocator*>(this->m_pLoc)->Release();
		CoUninitialize();
		return false;
	}

	std::cout << "Windows server connection established" << std::endl;

	return true;
}

void system_win::stop()
{
	if (this->m_pSvc != NULL) 
	{
		static_cast<IWbemServices*>(this->m_pSvc)->Release();
	}

	if (this->m_pLoc != NULL)
	{
		static_cast<IWbemLocator*>(this->m_pLoc)->Release();
	}

	CoUninitialize();
}

std::wstring system_win::get_property(const std::string& txt)
{
	HRESULT hres;
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = static_cast<IWbemServices*>(this->m_pSvc)->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM WIN32_OperatingSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator
	);
	if (FAILED(hres)) {
		std::cout << "Query failed" << std::endl;
		static_cast<IWbemServices*>(this->m_pSvc)->Release();
		static_cast<IWbemLocator*>(this->m_pLoc)->Release();
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
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		rvalue = vtProp.bstrVal;
		VariantClear(&vtProp);

		pclsObj->Release();
	}
	return rvalue;
}