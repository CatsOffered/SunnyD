#include "Hooks.h"
#include "jumphook.h"

#include "Forms/mainform.h"
#include "sunnyd.h"
#include "sig.h"

CJumpHook hk_terminate, hk_exit, hk_getproc, hk_alloc, hk_write, hk_thred, hk_creatproc;

void Hooks::Install()
{
	MessageBoxA(0, "Nice", "Info", MB_ICONINFORMATION);
	hk_write.Hook("ntdll.dll", "NtWriteVirtualMemory", Hooked_WriteVirtualMemory, 0);
	hk_terminate.Hook("kernel32.dll", "TerminateProcess", Hooked_Terminate, 0);
	hk_exit.Hook("kernel32.dll", "ExitProcess", Hooked_ExitProcess, 0);
	hk_thred.Hook("ntdll.dll", "NtCreateThreadEx", Hooked_NtCreateThreadEx, 0);
	//hk_creatproc.Hook("ntdll.dll", "NtCreateProcess", Hooked_NtCreateProcess, 0);
	//hk_creatproc.Hook("kernel32.dll", "CreateProcessA", Hooked_CreateProcessA, 0);
	//hk_creatfil.Hook("kernel32.dll", "CreateFileA", Hooked_CreateFileA, 0);
	hk_alloc.Hook("ntdll.dll", "NtAllocateVirtualMemory", Hooked_Alloc, 0);
	hk_getproc.Hook("kernelbase.dll", "GetProcAddressForCaller", Hooked_GetProcAddr, 0);
}

FARPROC WINAPI Hooks::Hooked_GetProcAddr(HMODULE hModule, LPCSTR lpProcName, void* Caller)
{
    if (GetCurrentThread() != SunnyD::hWin && HIWORD(lpProcName))
    {
        char name[MAX_PATH];
        if (GetModuleFileNameA(hModule, name, sizeof(name)))
        {
            char* base = strrchr(name, '\\') + 1;
            SunnyD::AddProcInfo(new ProcAddr(base, lpProcName));
        }
    }
    return ((decltype(Hooked_GetProcAddr)*)hk_getproc.GetOldCode())(hModule, lpProcName, Caller);
}

NTSTATUS WINAPI Hooks::Hooked_Alloc(
    HANDLE    ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T   RegionSize,
    ULONG     AllocationType,
    ULONG     Protect
)
{
    static int recursed = 0;
    SIZE_T size = *RegionSize;
    NTSTATUS result = ((decltype(Hooked_Alloc)*)hk_alloc.GetOldCode())(
        ProcessHandle, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);
    if (recursed > 0)
        return result;
    recursed++;

    if (ProcessHandle != GetCurrentProcess() && GetCurrentThread() != SunnyD::hWin)
    {
        HANDLE handle;
        if (DuplicateHandle(GetCurrentProcess(), ProcessHandle, GetCurrentProcess(), &handle, 0, true, DUPLICATE_SAME_ACCESS))
        {
            MemInfo* info = MemInfo_Alloc(*BaseAddress, size, handle);
            SunnyD::Update([info] { SunnyD::form->Dumps()->AddAlloc(info); });
        }
    }
    recursed--;
    return result;
}
BOOL WINAPI Hooks::Hooked_WriteVirtualMemory(
    IN HANDLE               ProcessHandle,
    IN PVOID                BaseAddress,
    IN PVOID                Buffer,
    IN ULONG                NumberOfBytesToWrite,
    OUT PULONG              NumberOfBytesWritten OPTIONAL)
{
    BOOL result = ((decltype(Hooked_WriteVirtualMemory)*)hk_write.GetOldCode())(
        ProcessHandle, BaseAddress, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten);
    if (GetCurrentThread() != SunnyD::hWin)
    {
        HANDLE handle;
        if (DuplicateHandle(GetCurrentProcess(), ProcessHandle, GetCurrentProcess(), &handle, 0, true, DUPLICATE_SAME_ACCESS))
        {
            MemInfo* info = MemInfo_Write(BaseAddress, Buffer, NumberOfBytesToWrite, handle);
            SunnyD::Update([info] { SunnyD::form->Dumps()->AddWrite(info); });
        }
    }
    return result;
}

NTSTATUS NTAPI Hooks::Hooked_NtCreateThreadEx(
    PHANDLE hThread, ACCESS_MASK DesiredAccess, PVOID ObjectAttributes, HANDLE ProcessHandle,
    PVOID lpStartAddress, PVOID lpParameter, ULONG Flags, SIZE_T StackZeroBits, SIZE_T SizeOfStackCommit,
    SIZE_T SizeOfStackReserve, PVOID lpBytesBuffer)
{
    UINT_PTR base = Sig::GetBaseOfCode(SunnyD::hInst);
    UINT_PTR start = (UINT_PTR)lpStartAddress;
    if (GetCurrentProcessId() != GetProcessId(ProcessHandle) ||
        (start < base || start > base + Sig::GetSizeOfCode(SunnyD::hInst)))
    {
        HANDLE handle;
        if (DuplicateHandle(GetCurrentProcess(), ProcessHandle, GetCurrentProcess(), &handle, 0, true, DUPLICATE_SAME_ACCESS))
        {
            MemInfo* info = MemInfo_Thread(lpStartAddress, handle, lpParameter);
            SunnyD::Update([info] { SunnyD::form->Dumps()->AddThread(info); });
        }
    }
    return ((decltype(Hooks::Hooked_NtCreateThreadEx)*)hk_thred.GetOldCode())(
        hThread, DesiredAccess, ObjectAttributes, ProcessHandle, lpStartAddress,
        lpParameter, Flags, StackZeroBits, SizeOfStackCommit, SizeOfStackReserve, lpBytesBuffer);
}

NTSTATUS NTAPI Hooks::Hooked_NtCreateProcess(
    OUT PHANDLE           ProcessHandle,
    IN ACCESS_MASK        DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE             ParentProcess,
    IN BOOLEAN            InheritObjectTable,
    IN HANDLE             SectionHandle OPTIONAL,
    IN HANDLE             DebugPort OPTIONAL,
    IN HANDLE             ExceptionPort OPTIONAL)
{
    MessageBoxA(0, "Creating process...", "Info", MB_ICONINFORMATION);
    TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
    return ((decltype(Hooks::Hooked_NtCreateProcess)*)hk_creatproc.GetOldCode())(
        ProcessHandle, DesiredAccess, ObjectAttributes, ParentProcess,
        InheritObjectTable, SectionHandle, DebugPort, ExceptionPort);
}

BOOL WINAPI Hooks::Hooked_CreateProcessA(
    _In_opt_ LPCSTR lpApplicationName,
    _Inout_opt_ LPSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOA lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation)
{
    char buf[MAX_PATH + 64];
    sprintf_s(buf, __FUNCTION__ ": (%p)", *(void**)_AddressOfReturnAddress());
    MessageBoxA(0, buf, "Info", MB_ICONINFORMATION);
    //TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
    return ((decltype(CreateProcessA)*)hk_creatproc.GetOldCode())(
        lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, 
        dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

CJumpHook hk_creatfil;

HANDLE WINAPI Hooks::Hooked_CreateFileA(
    _In_ LPCSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile)
{
    char buf[MAX_PATH + 64];
    sprintf_s(buf, __FUNCTION__ ": \"%s\" (%p)", lpFileName, *(void**)_AddressOfReturnAddress());
    MessageBoxA(0, buf, "Info", MB_ICONINFORMATION);
    //TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
    return ((decltype(CreateFileA)*)hk_creatfil.GetOldCode())(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL WINAPI Hooks::Hooked_Terminate(HANDLE hProc, UINT uExitCode)
{
    MessageBox(0, __FUNCTION__, "Info", MB_ICONINFORMATION);
    return ((decltype(TerminateProcess)*)hk_terminate.GetOldCode())(hProc, uExitCode);
}
void WINAPI Hooks::Hooked_ExitProcess(UINT uExitCode)
{
    MessageBox(0, __FUNCTION__, "Info", MB_ICONINFORMATION);
    return ((decltype(ExitProcess)*)hk_exit.GetOldCode())(uExitCode);
}