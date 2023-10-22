#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <llvm/IR/Constants.h>

using namespace llvm;

class MyPass : public PassInfoMixin<MyPass> {
  public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        outs() << "Processing " << F.getName() << "\n";
        LLVMContext &Ctx = F.getContext();
        IRBuilder<> builder(Ctx);

        Type *retType = Type::getVoidTy(Ctx);

        Type *intT = Type::getInt32Ty(Ctx);
        PointerType *pCharT = builder.getInt8PtrTy();

        Type *logParamTypes[] = {pCharT, pCharT, intT};
        FunctionType *logFnT = FunctionType::get(retType, logParamTypes, false);
        FunctionCallee logBeginFn
            = F.getParent()->getOrInsertFunction("fnLogBegin", logFnT);

        BasicBlock &entryBB = F.getEntryBlock();
        builder.SetInsertPoint(&entryBB.front());
        Value *fnName = builder.CreateGlobalStringPtr(F.getName());
        Value *args[] = {fnName};

        builder.CreateCall(logBeginFn, args);

        return PreservedAnalyses::all();
    }
};

static void passBuilderCB(PassBuilder &PB) {
    PB.registerVectorizerStartEPCallback(
        [](FunctionPassManager &FPM, OptimizationLevel) {
            FPM.addPass(MyPass());
        });
}

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "Hello", "0.1", passBuilderCB};
}
