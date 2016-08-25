#ifndef CRASHHANDLE_H
#define CRASHHANDLE_H

#include <QObject>
#include <windows.h>
#include <DbgHelp.h>
#include <tlhelp32.h>
#include <QProcess>


class CrashHandle : public QObject
{
    Q_OBJECT
public:
    explicit CrashHandle(QObject *parent = 0);

private:
    static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

    static void createDump(EXCEPTION_POINTERS *pException);
    static void GetExceptionInfo(EXCEPTION_POINTERS *pException);
    static QString GetModuleByRetAddr(PBYTE Ret_Addr, PBYTE & Module_Addr);
    static QString GetCallStack(PEXCEPTION_POINTERS pException);
    static QString GetVersionStr();
};

#endif // CRASHHANDLE_H
