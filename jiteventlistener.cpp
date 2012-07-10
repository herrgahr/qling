#include "jiteventlistener.h"
#include <iostream>
#include "llvm/Function.h"
#include "llvm/Support/raw_ostream.h"

void JitEventListener::NotifyFunctionEmitted(const llvm::Function &f, void *, size_t, const llvm::JITEventListener::EmittedFunctionDetails &)
{
    //llvm::errs()<<"JIT emitted function: "<<f.getName()<<"\n";
    std::string fName=f.getName().str();
    if(fName.find("_Z15__cling_Un1Qu3")==0)
        emit aboutToExecWrappedFunction();
}

void JitEventListener::NotifyFreeingMachineCode(void *)
{
}
