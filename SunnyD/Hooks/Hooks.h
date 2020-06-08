#pragma once
#include "framework.h"
#include <ntstatus.h>
#include <winternl.h>

namespace Hooks
{
	void Install();

	FARPROC WINAPI Hooked_GetProcAddr(HMODULE hModule, LPCSTR lpProcName, void* Caller);
	NTSTATUS WINAPI Hooked_Alloc(
		HANDLE	ProcessHandle,
		PVOID*	BaseAddress,
		ULONG_PTR	ZeroBits,
		PSIZE_T	RegionSize,
		ULONG	AllocationType,
		ULONG	Protect
	);
	BOOL WINAPI Hooked_WriteVirtualMemory(
		IN HANDLE	ProcessHandle,
		IN PVOID	BaseAddress,
		IN PVOID	Buffer,
		IN ULONG	NumberOfBytesToWrite,
		OUT PULONG	NumberOfBytesWritten OPTIONAL);
	NTSTATUS NTAPI Hooked_NtCreateThreadEx(
		PHANDLE	hThread,
		ACCESS_MASK	DesiredAccess,
		PVOID	ObjectAttributes,
		HANDLE	ProcessHandle,
		PVOID	lpStartAddress,
		PVOID	lpParameter,
		ULONG	Flags,
		SIZE_T	StackZeroBits,
		SIZE_T	SizeOfStackCommit,
		SIZE_T	SizeOfStackReserve,
		PVOID	lpBytesBuffer);
	NTSTATUS NTAPI Hooked_NtCreateProcess(
		OUT PHANDLE           ProcessHandle,
		IN ACCESS_MASK        DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
		IN HANDLE             ParentProcess,
		IN BOOLEAN            InheritObjectTable,
		IN HANDLE             SectionHandle OPTIONAL,
		IN HANDLE             DebugPort OPTIONAL,
		IN HANDLE             ExceptionPort OPTIONAL);
	BOOL WINAPI Hooked_CreateProcessA(
		_In_opt_ LPCSTR lpApplicationName,
		_Inout_opt_ LPSTR lpCommandLine,
		_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
		_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
		_In_ BOOL bInheritHandles,
		_In_ DWORD dwCreationFlags,
		_In_opt_ LPVOID lpEnvironment,
		_In_opt_ LPCSTR lpCurrentDirectory,
		_In_ LPSTARTUPINFOA lpStartupInfo,
		_Out_ LPPROCESS_INFORMATION lpProcessInformation);
	HANDLE WINAPI Hooked_CreateFileA(
		_In_ LPCSTR lpFileName,
		_In_ DWORD dwDesiredAccess,
		_In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		_In_ DWORD dwCreationDisposition,
		_In_ DWORD dwFlagsAndAttributes,
		_In_opt_ HANDLE hTemplateFile);
	BOOL WINAPI Hooked_Terminate(HANDLE hProc, UINT uExitCode);
	void WINAPI Hooked_ExitProcess(UINT uExitCode);
}