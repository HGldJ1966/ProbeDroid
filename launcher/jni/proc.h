/**
 *   The MIT License (MIT)
 *   Copyright (C) 2016 ZongXian Shen <andy.zsshen@gmail.com>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a
 *   copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation
 *   the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the
 *   Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *   IN THE SOFTWARE.
 */

#ifndef _PROC_H_
#define _PROC_H_


#include <sys/types.h>

#include "globals.h"


namespace proc {

class EggHunter;

class FunctionTable
{
  public:
    FunctionTable()
      : dlopen_(0), mmap_(0)
    {}

    ~FunctionTable()
    {}

    bool Resolve(pid_t, pid_t);

    uintptr_t GetDlopen() const
    {
        return dlopen_;
    }

    uintptr_t GetMmap() const
    {
        return mmap_;
    }

  private:
    uintptr_t GetLibraryAddress(pid_t, const char*);
    uintptr_t GetFunctionAddress(const char*, const char*);

    uintptr_t dlopen_;
    uintptr_t mmap_;
};

class EggHunter
{
  public:
    EggHunter()
      : pid_app_(0), func_tbl_()
    {}

    ~EggHunter()
    {}

    void Hunt(const char*, const char*, const char*, const char*);

  private:
    #define SYSERR PLOG(ERROR)
    #define FINAL(label, ...)                                                  \
        do {                                                                   \
            __VA_ARGS__;                                                       \
            rtn = PROC_FAIL;                                                   \
            goto label;                                                        \
        } while (0);

    // Routines to resolve zygote pid and clean the target app.
    bool Initialize(const char*);
    bool ExecutePs(const char*, pid_t*);
    bool ExecuteKill(pid_t);

    // Routines to trace zygote and catch the newly forked target app.
    bool CaptureApp(const char*);
    bool WaitForForkEvent(pid_t);
    bool CheckStartupCmd(const char*);

    // Routines to inject "libProbeDroid.so" to the target app.
    bool InjectApp(const char*, const char*, const char*);
    bool RemoteFunctionCall(struct pt_regs*, uintptr_t, long*, size_t, uintptr_t*);
    bool PokeText(uintptr_t, const char*, size_t);
    bool PeekText(uintptr_t, char*, size_t);

    pid_t pid_zygote_, pid_app_;
    FunctionTable func_tbl_;
};

}

#endif