#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

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

        Type *logParamTypes[] = {pCharT, pCharT};
        FunctionType *logFnT = FunctionType::get(retType, logParamTypes, false);
        FunctionCallee logFn = module->getOrInsertFunction("logUse", logFnT);

        for (BasicBlock &bb : F) {
            for (Instruction &instr1 : bb) {
                if (dyn_cast<PHINode>(&instr1)) {
                    continue;
                }
                builder.SetInsertPoint(&instr1);

                Value *opName1
                    = builder.CreateGlobalStringPtr(instr1.getOpcodeName());

                for (User *user : instr1.users()) {
                    if (auto *pInstr2 = dyn_cast<Instruction>(user)) {
                        if (pInstr2 == &instr1) {
                            continue;
                        }
                        // outs() << instr1 << " used by " << *pInstr2 << '\n';
                        builder.SetInsertPoint(&instr1);
                        Value *opName2 = builder.CreateGlobalStringPtr(
                            pInstr2->getOpcodeName());
                        Value *args[] = {opName2, opName1};
                        builder.CreateCall(logFn, args);
                    }
                }
            }
        }

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
