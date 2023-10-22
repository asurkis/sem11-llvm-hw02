#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <llvm/IR/Constants.h>

using namespace llvm;

class MyPass : public PassInfoMixin<MyPass> {
  public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        Module *module = F.getParent();
        outs() << "Processing " << F.getName() << "\n";
        LLVMContext &Ctx = F.getContext();
        IRBuilder<> builder(Ctx);

        Type *retType = Type::getVoidTy(Ctx);

        Type *intT = Type::getInt32Ty(Ctx);
        PointerType *pCharT = builder.getInt8PtrTy();

        Type *logParamTypes[] = {pCharT};
        FunctionType *logFnT = FunctionType::get(retType, logParamTypes, false);
        FunctionCallee logFn = module->getOrInsertFunction("logInstr", logFnT);

        BasicBlock &entryBB = F.getEntryBlock();
        builder.SetInsertPoint(&entryBB.front());
        Value *fnName = builder.CreateGlobalStringPtr(F.getName());
        builder.ClearInsertionPoint();

        Value *args[] = {fnName};

        // builder.CreateCall(logFn, args);

        for (BasicBlock &bb : F) {
            // outs() << "Successfully iterated block\n";
            for (Instruction &instr : bb) {
                if (!logFn) {
                    continue;
                }

                // outs() << "Successfully iterated instruction\n";
                outs() << "Processing instruction " << instr << " of type "
                       << instr.getOpcode() << '\n';
                instr.getOpcodeName();
                // outs() << "Trying to create call\n";
                builder.SetInsertPoint(&instr);
                builder.CreateCall(logFn, args);
                builder.ClearInsertionPoint();
                outs() << "    Successfully created call\n";
            }
            // outs() << "Successfully exited instruction loop\n";
        }
        outs() << "Successfully exited block loop\n";

        return PreservedAnalyses::none();
    }
};

void passBuilderCB(PassBuilder &PB) {
    PB.registerVectorizerStartEPCallback(
        [](FunctionPassManager &FPM, OptimizationLevel) {
            FPM.addPass(MyPass());
        });
}

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "Hello", "0.1", passBuilderCB};
}
