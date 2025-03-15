//===------ RADeadCodeElimination.h - Range Analysis DCE Pass ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/CFG.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Transforms/Utils/Local.h"  // 安全删除工具
#include "llvm/ADT/PostOrderIterator.h"  // 用于拓扑排序
#include "RangeAnalysis.h"


using namespace llvm;

namespace {
class RADeadCodeElimination : public FunctionPass {
private:
  SmallPtrSet<BasicBlock*, 8> DeadBBs;  // 记录待删除的基本块
  InterProceduralRA<Cousot>* RA;

  bool solveICmpInstruction(ICmpInst* I);
  void handleBranch(BranchInst* BI);
  void deleteDeadBlocks(Function& F);

public:
  static char ID;
  RADeadCodeElimination() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage& AU) const override {
    AU.addRequired<InterProceduralRA<Cousot>>();
    AU.setPreservesCFG();
  }

  bool runOnFunction(Function& F) override;
};
} // End anonymous namespace

char RADeadCodeElimination::ID = 0;

//===----------------------------------------------------------------------===//
// Pass Implementation
//===----------------------------------------------------------------------===//

bool RADeadCodeElimination::runOnFunction(Function &F) {
  RA = &getAnalysis<InterProceduralRA<Cousot>>();
  bool Changed = false;

  // 第一阶段：处理所有ICmp指令
  for (auto &BB : F) {
    for (auto It = BB.begin(); It != BB.end();) {
      Instruction *I = &*It++;
      if (auto *ICI = dyn_cast<ICmpInst>(I)) {
        Changed |= solveICmpInstruction(ICI);
      }
    }
  }

  // 第二阶段：处理分支指令
  for (auto &BB : F) {
    if (auto *BI = dyn_cast<BranchInst>(BB.getTerminator())) {
      handleBranch(BI);
    }
  }

  // 第三阶段：删除死基本块
  deleteDeadBlocks(F);

  return Changed;
}

bool RADeadCodeElimination::solveICmpInstruction(ICmpInst *I) {
  Value *Op0 = I->getOperand(0), *Op1 = I->getOperand(1);
  Range R0 = RA->getRange(Op0), R1 = RA->getRange(Op1);
  ConstantInt *Result = nullptr;

  const APInt &L0 = R0.getLower(), &U0 = R0.getUpper();
  const APInt &L1 = R1.getLower(), &U1 = R1.getUpper();

  switch (I->getPredicate()) {
  case CmpInst::ICMP_SLT:
    if (U0.slt(L1)) Result = ConstantInt::getTrue(I->getContext());
    else if (L0.sge(U1)) Result = ConstantInt::getFalse(I->getContext());
    break;
  case CmpInst::ICMP_SGT:
    if (L0.sgt(U1)) Result = ConstantInt::getTrue(I->getContext());
    else if (U0.sle(L1)) Result = ConstantInt::getFalse(I->getContext());
    break;
  case CmpInst::ICMP_EQ:
    if (U0.slt(L1) || U1.slt(L0)) Result = ConstantInt::getFalse(I->getContext());
    break;
  case CmpInst::ICMP_NE:
    if (U0.slt(L1) || U1.slt(L0)) Result = ConstantInt::getTrue(I->getContext());
    break;
  case CmpInst::ICMP_ULT:
    if (U0.ult(L1)) Result = ConstantInt::getTrue(I->getContext());
    else if (L0.uge(U1)) Result = ConstantInt::getFalse(I->getContext());
    break;
  // 添加其他比较类型的处理...
  default: break;
  }

  if (Result) {
    I->replaceAllUsesWith(Result);
    I->eraseFromParent();
    return true;
  }
  return false;
}

void RADeadCodeElimination::handleBranch(BranchInst *BI) {
  if (!BI->isConditional()) return;

  if (ConstantInt *CI = dyn_cast<ConstantInt>(BI->getCondition())) {
    BasicBlock *DeadBB = CI->isZero() ? BI->getSuccessor(0) : BI->getSuccessor(1);
    BasicBlock *TargetBB = CI->isZero() ? BI->getSuccessor(1) : BI->getSuccessor(0);

    // 创建无条件分支并替换
    BranchInst::Create(TargetBB, BI);
    BI->eraseFromParent();

    // 标记死基本块
    DeadBBs.insert(DeadBB);
  }
}

void RADeadCodeElimination::deleteDeadBlocks(Function &F) {
  if (DeadBBs.empty()) return;

  // 转换为向量避免迭代器失效
  std::vector<BasicBlock*> ToDelete;
  ToDelete.reserve(DeadBBs.size());
  for (auto *BB : DeadBBs)
    ToDelete.push_back(BB);

  for (auto *BB : ToDelete) {
    if (!BB || BB->hasAddressTaken()) continue;

    // Step 1: 清理前驱关系
    SmallVector<BasicBlock*, 4> Preds(predecessors(BB));
    for (auto *Pred : Preds) {
      if (DeadBBs.count(Pred)) continue;
      auto *TI = Pred->getTerminator();
      
      // 手动遍历所有后继并替换
      for (unsigned i = 0, e = TI->getNumSuccessors(); i < e; ++i) {
        if (TI->getSuccessor(i) == BB) {
          // 替换为不可达目标，后续清理会处理
          TI->setSuccessor(i, &BB->getParent()->getEntryBlock());
        }
      }
    }

    // Step 2: 清除PHI节点的引用
    for (auto *Succ : successors(BB)) {
      Succ->removePredecessor(BB);  // 这会自动处理PHI节点
    }

    // Step 3: 安全删除块
    BB->dropAllReferences();
    BB->eraseFromParent();
  }

  DeadBBs.clear();
}

// Pass注册
static RegisterPass<RADeadCodeElimination> X(
  "ra-dce", 
  "Dead Code Elimination using Range Analysis",
  false,  // 是否只查看CFG
  false   // 是否是分析Pass
);
