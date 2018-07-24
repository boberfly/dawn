// Copyright 2017 The Dawn Authors
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

#ifndef DAWNNATIVE_METAL_RENDERPIPELINEMTL_H_
#define DAWNNATIVE_METAL_RENDERPIPELINEMTL_H_

#include "dawn_native/RenderPipeline.h"

#import <Metal/Metal.h>

namespace backend { namespace metal {

    class RenderPipeline : public RenderPipelineBase {
      public:
        RenderPipeline(RenderPipelineBuilder* builder);
        ~RenderPipeline();

        MTLIndexType GetMTLIndexType() const;
        MTLPrimitiveType GetMTLPrimitiveTopology() const;

        void Encode(id<MTLRenderCommandEncoder> encoder);

      private:
        MTLIndexType mMtlIndexType;
        MTLPrimitiveType mMtlPrimitiveTopology;
        id<MTLRenderPipelineState> mMtlRenderPipelineState = nil;
    };

}}  // namespace backend::metal

#endif  // DAWNNATIVE_METAL_RENDERPIPELINEMTL_H_