#ifndef JITEVENTLISTENER_H
#define JITEVENTLISTENER_H

#include <QObject>

#include "llvm/ExecutionEngine/JITEventListener.h"

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
