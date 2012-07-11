#ifndef JITEVENTLISTENER_H
#define JITEVENTLISTENER_H

#include <QObject>

#include "llvm/ExecutionEngine/JITEventListener.h"


/** Minotor JIT's activities and check for creation of functions called
  * cling_Un1Qu3*
  * This is used to signal when code is about to be executed to give the UI
  * a chance to distinguish between output from the compiler and output from
  * the code itself (std::cout and friends)
  */
class JitEventListener : public QObject, public llvm::JITEventListener
{
    Q_OBJECT
public:

    virtual void NotifyFunctionEmitted(const llvm::Function &, void *, size_t, const EmittedFunctionDetails &);
    virtual void NotifyFreeingMachineCode(void *);
    
signals:
    void aboutToExecWrappedFunction();
};

#endif // JITEVENTLISTENER_H
