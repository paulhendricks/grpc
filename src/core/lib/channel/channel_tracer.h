/*
 *
 * Copyright 2017 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef GRPC_CORE_LIB_CHANNEL_CHANNEL_TRACER_H
#define GRPC_CORE_LIB_CHANNEL_CHANNEL_TRACER_H

#include <grpc/grpc.h>
#include "src/core/lib/iomgr/error.h"
#include "src/core/lib/json/json.h"

namespace grpc_core {

extern grpc_core::DebugOnlyTraceFlag grpc_trace_channel_tracer_refcount;

typedef struct grpc_trace_node grpc_trace_node;

class ChannelTracer {
 public:
  ChannelTracer(size_t max_nodes);
  ~ChannelTracer() {}

  // TODO(ncteisen): incorporate RefCounted class
  ChannelTracer* Ref();
  void Unref();

  /* returns the tracers uuid */
  intptr_t GetUuid();

  /* Adds a new trace node to the tracing object */
  void AddTrace(grpc_slice data, grpc_error* error,
                grpc_connectivity_state connectivity_state,
                ChannelTracer* subchannel);

  /* Returns the tracing data rendered as a grpc json string.
     The string is owned by the caller and must be freed. If recursive
     is true, then the string will include the recursive trace for all
     subtracing objects. */
  char* RenderTrace(bool recursive);

  /* util functions that perform the uuid -> tracer step for you, and then
   returns the trace for the uuid given. */
  static char* GetChannelTraceFromUuid(intptr_t uuid, bool recursive);

 private:
  friend class ChannelTracerRenderer;
  gpr_refcount refs;
  gpr_mu tracer_mu;
  intptr_t channel_uuid;
  uint64_t num_nodes_logged;
  size_t list_size;
  size_t max_list_size;
  grpc_trace_node* head_trace;
  grpc_trace_node* tail_trace;
  gpr_timespec time_created;
};

}  // namespace grpc_core

#endif /* GRPC_CORE_LIB_CHANNEL_CHANNEL_TRACER_H */
