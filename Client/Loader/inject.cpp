/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "inject.hpp"

#include <vector>

namespace
{
	InjectResult fail(DWORD err, const wchar_t* msg)
	{
		return { false, err, msg };
	}
}

InjectResult spawnAndInject(const InjectOptions& opt)
{
	if (opt.gameExe.empty() || opt.clientDll.empty())
	{
		return fail(ERROR_INVALID_PARAMETER, L"missing gameExe or clientDll");
	}

	STARTUPINFOW si = {};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};

	std::wstring cmd = L"\"" + opt.gameExe + L"\" " + opt.commandLine;
	std::vector<wchar_t> cmdBuf(cmd.begin(), cmd.end());
	cmdBuf.push_back(L'\0');

	if (!CreateProcessW(
			opt.gameExe.c_str(),
			cmdBuf.data(),
			nullptr, nullptr, FALSE,
			CREATE_SUSPENDED,
			nullptr,
			opt.gameWorkDir.empty() ? nullptr : opt.gameWorkDir.c_str(),
			&si, &pi))
	{
		return fail(GetLastError(), L"CreateProcessW failed");
	}

	const SIZE_T pathBytes = (opt.clientDll.size() + 1) * sizeof(wchar_t);
	void* remotePath = VirtualAllocEx(pi.hProcess, nullptr, pathBytes,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!remotePath)
	{
		TerminateProcess(pi.hProcess, 1);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return fail(GetLastError(), L"VirtualAllocEx failed");
	}

	if (!WriteProcessMemory(pi.hProcess, remotePath, opt.clientDll.c_str(), pathBytes, nullptr))
	{
		VirtualFreeEx(pi.hProcess, remotePath, 0, MEM_RELEASE);
		TerminateProcess(pi.hProcess, 1);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return fail(GetLastError(), L"WriteProcessMemory failed");
	}

	auto loadLib = reinterpret_cast<LPTHREAD_START_ROUTINE>(
		GetProcAddress(GetModuleHandleW(L"kernel32"), "LoadLibraryW"));
	HANDLE thread = CreateRemoteThread(pi.hProcess, nullptr, 0, loadLib, remotePath, 0, nullptr);
	if (!thread)
	{
		VirtualFreeEx(pi.hProcess, remotePath, 0, MEM_RELEASE);
		TerminateProcess(pi.hProcess, 1);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return fail(GetLastError(), L"CreateRemoteThread failed");
	}

	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
	VirtualFreeEx(pi.hProcess, remotePath, 0, MEM_RELEASE);

	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return { true, 0, L"injected" };
}
