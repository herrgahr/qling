QT       += core gui

TARGET = qling
TEMPLATE = app

#DEFINES+=NO_CONSOLE_REDIRECT

#TODO smarter way to provide llvm-install-dir
#INCLUDEPATH+=/home/thomas/opt/llvm-debug/include\
#             /home/thomas/opt/llvm-debug/include/clang\
#             /home/thomas/opt/llvm-debug/include/cling
DEFINES+=LLVM_INSTALL=\\\"$$(LLVM_INSTALL)\\\"
INCLUDEPATH+=$$(LLVM_INSTALL)/include\
             $$(LLVM_INSTALL)/include/clang\
             $$(LLVM_INSTALL)/include/cling

QMAKE_CXXFLAGS+=-D__STDC_CONSTANT_MACROS\
-D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

LIBS += -L$$(LLVM_INSTALL)/lib/
# rdynamic causes symbols to be exported even though this is not a lib
LIBS += -lm -ldl -fPIC -rdynamic\
qt-hack/qatomic_sun.o\
-lcling\
-lclingInterpreter\
-lclingUtils\
-lclangFrontend\
-lclangFrontendTool\
-lclangSerialization\
-lclangDriver\
-lclangCodeGen\
-lclangParse\
-lclangSema\
-lclangEdit\
-lclangAnalysis -lclangRewrite\
-lclangAST -lclangLex -lclangBasic\
-lLLVMLinker\
-lLLVMArchive\
-lLLVMBitReader\
-lLLVMJIT\
##MCJIT
#-lLLVMMCJIT\
#-lLLVMRuntimeDyld\
#-lLLVMObject\
##/MCJIT
-lLLVMExecutionEngine \
-lLLVMX86Disassembler\
-lLLVMX86AsmParser\
-lLLVMX86CodeGen\
-lLLVMX86Desc\
-lLLVMSelectionDAG\
-lLLVMAsmPrinter\
-lLLVMMCParser\
-lLLVMCodeGen\
-lLLVMScalarOpts\
-lLLVMInstCombine\
-lLLVMTransformUtils\
-lLLVMipa\
-lLLVMAnalysis\
-lLLVMTarget\
-lLLVMX86AsmPrinter\
-lLLVMX86Utils\
-lLLVMCore\
-lLLVMX86Info\
-lLLVMMC\
-lLLVMSupport

SOURCES +=\
main.cpp\
widget.cpp \
util.cpp \
consoleoutput.cpp \
codeinput.cpp \
codewidget.cpp

HEADERS  +=\
widget.h \
util.h \
consoleoutput.h \
codeinput.h \
codewidget.h



