#include "jiteventlistener.h"
#include <iostream>
#include "llvm/Function.h"
#include "llvm/Support/raw_ostream.h"

void JitEventListener::NotifyFunctionEmitted(const llvm::Function &f, void *, size_t, const llvm::JITEventListener::EmittedFunctionDetails &)
{
    if(f.getName().find("__cling_Un1Qu3")!=llvm::StringRef::npos)
        emit aboutToExecWrappedFunction();
}

void JitEventListener::NotifyFreeingMachineCode(void *)
{
}
