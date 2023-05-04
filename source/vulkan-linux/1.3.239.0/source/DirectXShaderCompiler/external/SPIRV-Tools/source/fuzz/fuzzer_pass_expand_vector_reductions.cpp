// Copyright (c) 2020 André Perez Maselco
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "source/fuzz/fuzzer_pass_expand_vector_reductions.h"

#include "source/fuzz/fuzzer_util.h"
#include "source/fuzz/instruction_descriptor.h"
#include "source/fuzz/transformation_expand_vector_reduction.h"

namespace spvtools {
namespace fuzz {

FuzzerPassExpandVectorReductions::FuzzerPassExpandVectorReductions(
    opt::IRContext* ir_context, TransformationContext* transformation_context,
    FuzzerContext* fuzzer_context,
    protobufs::TransformationSequence* transformations,
    bool ignore_inapplicable_transformations)
    : FuzzerPass(ir_context, transformation_context, fuzzer_context,
                 transformations, ignore_inapplicable_transformations) {}

void FuzzerPassExpandVectorReductions::Apply() {
  for (auto& function : *GetIRContext()->module()) {
    for (auto& block : function) {
      for (auto& instruction : block) {
        // Randomly decides whether the transformation will be applied.
        if (!GetFuzzerContext()->ChoosePercentage(
                GetFuzzerContext()->GetChanceOfExpandingVectorReduction())) {
          continue;
        }

        // |instruction| must be OpAny or OpAll.
        if (instruction.opcode() != spv::Op::OpAny &&
            instruction.opcode() != spv::Op::OpAll) {
          continue;
        }

        // It must be able to make a synonym of |instruction|.
        if (!fuzzerutil::CanMakeSynonymOf(
                GetIRContext(), *GetTransformationContext(), instruction)) {
          continue;
        }

        // Applies the expand vector reduction transformation.
        ApplyTransformation(TransformationExpandVectorReduction(
            instruction.result_id(),
            GetFuzzerContext()->GetFreshIds(
                TransformationExpandVectorReduction::GetRequiredFreshIdCount(
                    GetIRContext(), &instruction))));
      }
    }
  }
}

}  // namespace fuzz
}  // namespace spvtools
