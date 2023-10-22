/*
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class HelloWorldPass : public FunctionPass {
  inline static char ID = 0;

public:
  HelloWorldPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &f) override {
    errs() << "Looking at function " << f.getName() << "\n";
    return false;
  }
};

static RegisterPass<HelloWorldPass> X("Hello", "Hello World Pass");
*/

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

class Hello : public PassInfoMixin<Hello> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    errs() << "Function: " << F.getName() << "\n";
    return PreservedAnalyses::all();
  }
};

static void pbcb(PassBuilder &pb) {
  pb.registerVectorizerStartEPCallback(
      [](FunctionPassManager &fpm, OptimizationLevel) {
        errs() << "Registering hello pass\n";
        fpm.addPass(Hello());
      });

  // pb.registerPipelineParsingCallback(
  //     [](StringRef Name, llvm::FunctionPassManager &PM,
  //        ArrayRef<llvm::PassBuilder::PipelineElement>) {
  //       errs() << "Name = " << Name << "\n";
  //       if (Name == "goodbye") {
  //         PM.addPass(Hello());
  //         return true;
  //       }
  //       return false;
  //     });
}

PassPluginLibraryInfo getHelloPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Hello", "0.1", pbcb};
}

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getHelloPluginInfo();
}
