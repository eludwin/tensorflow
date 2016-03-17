/* Copyright 2015 Google Inc. All Rights Reserved.

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

// See docs in ../ops/nn_ops.cc.

#define EIGEN_USE_THREADS

#include "tensorflow/core/kernels/softmax_op.h"
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_shape.h"

namespace tensorflow {

typedef Eigen::ThreadPoolDevice CPUDevice;
typedef Eigen::GpuDevice GPUDevice;

// Partial specialization for a CPUDevice, that uses the Eigen implementation
// from SoftmaxEigenImpl.
namespace functor {
template <typename T>
struct SoftmaxFunctor<CPUDevice, T> {
  void operator()(const CPUDevice& d, typename TTypes<T>::ConstMatrix logits,
                  typename TTypes<T>::Matrix softmax) {
    SoftmaxEigenImpl<CPUDevice, T>::Compute(d, logits, softmax);
  }
};
}  // namespace functor

REGISTER_KERNEL_BUILDER(Name("Softmax")
                            .Device(DEVICE_CPU)
                            .TypeConstraint<float>("T"),
                        SoftmaxOp<CPUDevice, float>);
REGISTER_KERNEL_BUILDER(Name("Softmax")
                            .Device(DEVICE_CPU)
                            .TypeConstraint<double>("T"),
                        SoftmaxOp<CPUDevice, double>);

#if GOOGLE_CUDA
REGISTER_KERNEL_BUILDER(Name("Softmax")
                            .Device(DEVICE_GPU)
                            .TypeConstraint<float>("T"),
                        SoftmaxOp<GPUDevice, float>);
#endif  // GOOGLE_CUDA

}  // namespace tensorflow
