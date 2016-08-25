#include "CrashHandle.h"
#include <QFile>

CrashHandle::CrashHandle(QObject *parent) : QObject(parent)
{
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注册异常捕获函数
}

LONG CrashHandle::ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    if (pException) {
        //启动外部程序
        QProcess process;
        QStringList list;
        list.append(QString::number(pException->ExceptionRecord->ExceptionCode,16));
        list.append(QString::number((uint)pException->ExceptionRecord->ExceptionAddress,16));
        process.execute("../CrashDialog/bin/CrashDialog.exe", list);

        //生成dump & crash.log
        createDump(pException);
        GetExceptionInfo(pException);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

void CrashHandle::createDump(EXCEPTION_POINTERS *pException)
{
    HANDLE hDumpFile = CreateFile((const wchar_t*)QString("crash.dmp").utf16(), GENERIC_WRITE
                                  , 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if( hDumpFile != INVALID_HANDLE_VALUE){
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }
}

void CrashHandle::GetExceptionInfo(EXCEPTION_POINTERS *pException)
{
    WCHAR Module_Name[MAX_PATH];

    QString sRet;
    char buffer[512] = {0};

    QString sTmp = GetVersionStr();
    sRet.append(sTmp);
    sRet.append("Process:  ");

    GetModuleFileName(NULL, Module_Name, MAX_PATH);
    sRet.append(QString::fromWCharArray(Module_Name));
    sRet.append("\n");

    // If exception occurred.
    if (pException) {
        EXCEPTION_RECORD &	E = *pException->ExceptionRecord;
        CONTEXT &			C = *pException->ContextRecord;

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "Exception Addr:  %08X  ", (int)E.ExceptionAddress);
        sRet.append(buffer);

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "\nException Code:  %08X", (int)E.ExceptionCode);
        sRet.append(buffer);

        sRet.append("\nRegisters: ");
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "\nEAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X",  (unsigned int)C.Eax,(unsigned int) C.Ebx, (unsigned int)C.Ecx, (unsigned int)C.Edx);
        sRet.append(buffer);

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "\nESI: %08X  EDI: %08X  ESP: %08X  EBP: %08X", (unsigned int)C.Esi, (unsigned int)C.Edi, (unsigned int)C.Esp, (unsigned int)C.Ebp);
        sRet.append(buffer);

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "\nEIP: %08X  EFlags: %08X", (unsigned int)C.Eip,(unsigned int) C.EFlags);
        sRet.append(buffer);

    }

    sRet.append("\nCall Stack:");

    //获取调用栈
    QString sCallstack = CrashHandle::GetCallStack(pException);
    sRet.append(sCallstack);

    QFile *file = new QFile("crash.log");
    if (file->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        file->write(sRet.toUtf8());
        file->close();
    }
}

QString CrashHandle::GetModuleByRetAddr(PBYTE Ret_Addr, PBYTE & Module_Addr)
{
    MODULEENTRY32	M = {sizeof(M)};
    HANDLE	hSnapshot;

    wchar_t Module_Name[MAX_PATH] = {0};

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

    if ((hSnapshot != INVALID_HANDLE_VALUE) && Module32First(hSnapshot, &M)) {
        do {
            if (DWORD(Ret_Addr - M.modBaseAddr) < M.modBaseSize) {
                lstrcpyn(Module_Name, M.szExePath, MAX_PATH);
                Module_Addr = M.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &M));
    }

    CloseHandle(hSnapshot);

    QString sRet = QString::fromWCharArray(Module_Name);
    return sRet;
}

QString CrashHandle::GetCallStack(PEXCEPTION_POINTERS pException)
{
    PBYTE	Module_Addr_1;
    char bufer[256]={0};
    QString sRet;

    typedef struct STACK {
        STACK *	Ebp;
        PBYTE	Ret_Addr;
        DWORD	Param[0];
    } STACK, * PSTACK;

    STACK	Stack = {0, 0};
    PSTACK	Ebp;

    //fake frame for exception address
    if (pException)	 {
        Stack.Ebp = (PSTACK)pException->ContextRecord->Ebp;
        Stack.Ret_Addr = (PBYTE)pException->ExceptionRecord->ExceptionAddress;
        Ebp = &Stack;
    }
    else {
        //frame addr of Get_Call_Stack()
        Ebp = (PSTACK)&pException - 1;

        //Skip frame of Get_Call_Stack().
        if (!IsBadReadPtr(Ebp, sizeof(PSTACK)))
            Ebp = Ebp->Ebp;
    }

    //Break trace on wrong stack frame.
    for (; !IsBadReadPtr(Ebp, sizeof(PSTACK)) && !IsBadCodePtr(FARPROC(Ebp->Ret_Addr)); Ebp = Ebp->Ebp) {
        //If module with Ebp->Ret_Addr found.
        memset(bufer,0, sizeof(0));
        sprintf(bufer, "\n%08X  ", (unsigned int)Ebp->Ret_Addr);
        sRet.append(bufer);

        QString moduleName = CrashHandle::GetModuleByRetAddr(Ebp->Ret_Addr, Module_Addr_1) ;
        if (moduleName.length() > 0) {
            sRet.append(moduleName);
        }
    }

    return sRet;
}

QString CrashHandle::GetVersionStr()
{
    OSVERSIONINFOEX	V = {sizeof(OSVERSIONINFOEX)};

    if (!GetVersionEx((POSVERSIONINFO)&V)) {
        ZeroMemory(&V, sizeof(V));
        V.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx((POSVERSIONINFO)&V);
    }

    if (V.dwPlatformId != VER_PLATFORM_WIN32_NT)
        V.dwBuildNumber = LOWORD(V.dwBuildNumber);

    QString sRet = QString("Windows:  %1.%2.%3, SP %4.%5, Product Type %6\n")
            .arg(V.dwMajorVersion).arg(V.dwMinorVersion).arg(V.dwBuildNumber)
            .arg(V.wServicePackMajor).arg(V.wServicePackMinor).arg(V.wProductType);
    return sRet;
}
