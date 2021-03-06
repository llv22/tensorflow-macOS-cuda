/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if GOOGLE_CUDA || TENSORFLOW_USE_ROCM

#define EIGEN_USE_GPU

#if defined(__APPLE__) && defined(__MACH__)
#include "tensorflow/core/framework/tensor_shape.pb.h"
#endif

#include "tensorflow/core/kernels/image/adjust_hsv_gpu.cu.h"
#include "tensorflow/core/kernels/image/adjust_saturation_op.h"
#include "tensorflow/core/util/gpu_kernel_helper.h"

namespace tensorflow {

namespace functor {

template <typename T>
void AdjustSaturationGPU<T>::operator()(GPUDevice* device,
                                        const int64 number_of_elements,
                                        const T* const input,
                                        const float* const scale,
                                        T* const output) {
  const auto stream = device->stream();
  const GpuLaunchConfig config =
      GetGpuLaunchConfig(number_of_elements, *device);
  const int threads_per_block = config.thread_per_block;
  const int block_count =
      (number_of_elements + threads_per_block - 1) / threads_per_block;
  TF_CHECK_OK(GpuLaunchKernel(internal::adjust_hsv_nhwc<false, true, false, T>,
                              block_count, threads_per_block, 0, stream,
                              number_of_elements, input, output, nullptr, scale,
                              nullptr));
}

template struct AdjustSaturationGPU<float>;
template struct AdjustSaturationGPU<Eigen::half>;

}  // namespace functor
}  // namespace tensorflow
#endif  // GOOGLE_CUDA || TENSORFLOW_USE_ROCM
