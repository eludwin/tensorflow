/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

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
#ifndef TENSORFLOW_COMPILER_MLIR_QUANTIZATION_TENSORFLOW_PYTHON_PY_FUNCTION_LIB_H_
#define TENSORFLOW_COMPILER_MLIR_QUANTIZATION_TENSORFLOW_PYTHON_PY_FUNCTION_LIB_H_

#include <string>
#include <unordered_set>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "pybind11/pytypes.h"  // from @pybind11
#include "tensorflow/compiler/mlir/quantization/tensorflow/exported_model.pb.h"
#include "tensorflow/compiler/mlir/quantization/tensorflow/quantization_options.pb.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"

namespace tensorflow::quantization {

// Declares pure virtual member functions for a python-side derived class to
// override. This allows calling python implementations from the C++ layer.
// Member functions should be pure not stateful; they should not access or rely
// on member fields.
class PyFunctionLibrary {
 public:
  virtual ~PyFunctionLibrary() = default;

  // Assigns UUIDs to each CustomAggregator op found in each GraphDef in
  // `exported_model`. The UUIDs are set to the `id` attributes. The UUIDs will
  // be used during calibration step to identify the collected quantization
  // statistics for each CustsomAggregator op.
  //
  // If the function signature changes, likely its corresponding .pyi type
  // hinting and definition should also change.
  // LINT.IfChange
  virtual ExportedModel AssignIdsToCustomAggregatorOps(
      const ExportedModel& exported_model) const = 0;
  // LINT.ThenChange(
  //     pywrap_function_lib.pyi:assign_ids_to_custom_aggregator_ops,
  //     py_function_lib.py:assign_ids_to_custom_aggregator_ops,
  // )

  // Saves `exported_model` to `dst_saved_model_path` as SavedModel.
  // `src_saved_model_path` is the path to the source SavedModel from which the
  // exported model is produced. It is used to copy the asset files to
  // `dst_saved_model_path`. `tags` will be attached to the saved
  // `MetaGraphDef`. `signature_def_map` will be passed to the
  // `add_meta_graph_and_variables` function, which is internally used to add a
  // `MetaGraphDef` to save to the SavedModel.
  //
  // If the function signature changes, likely its corresponding .pyi type
  // hinting and definition should also change.
  // LINT.IfChange
  virtual void SaveExportedModel(
      absl::string_view dst_saved_model_path,
      const ExportedModel& exported_model,
      absl::string_view src_saved_model_path,
      const std::unordered_set<std::string>& tags,
      const absl::flat_hash_map<std::string, tensorflow::SignatureDef>&
          signature_def_map) const = 0;
  // LINT.ThenChange(
  //     pywrap_function_lib.pyi:save_exported_model,
  //     py_function_lib.py:save_exported_model,
  // )

  // Runs calibration on a model saved at `saved_model_path`. `exported_model`
  // should be the corresponding exported model resulting from the
  // pre-calibration step. `signature_keys` is a set of keys that identify a
  // SignatureDef to run the calibration on. `tags` is a set of strings that
  // identify the `MetaGraphDef`. `calibration_options` provides configurations
  // for the calibration behavior. `representative_dataset` is a python object
  // of type `RepresentativeDatasetOrMapping`, which is used to run the
  // calibration.
  //
  // Returns the updated exported model where the collected calibration
  // statistics are added to `CustomAggregator` nodes at the `min` and `max`
  // attributes.
  //
  // If the function signature changes, likely its corresponding .pyi type
  // hinting and definition should also change.
  // LINT.IfChange(run_calibration)
  virtual ExportedModel RunCalibration(
      absl::string_view saved_model_path,
      const std::vector<std::string>& signature_keys,
      const std::unordered_set<std::string>& tags,
      const ExportedModel& exported_model,
      const CalibrationOptions& calibration_options,
      bool force_graph_mode_calibration,
      pybind11::object representative_dataset) const = 0;
  // LINT.ThenChange(
  //     pywrap_function_lib.pyi:run_calibration,
  //     py_function_lib.py:run_calibration,
  // )

  // Enables the `DumpTensor` ops in `graph_def`. This is done by updating the
  // `enabled` attribute of `DumpTensor` ops to true. Returns the updated
  // `GraphDef`.
  //
  // If the function signature changes, likely its corresponding .pyi type
  // hinting and definition should also change.
  // LINT.IfChange
  virtual GraphDef EnableDumpTensor(const GraphDef& graph_def) const = 0;
  // LINT.ThenChange(
  //     pywrap_function_lib.pyi:enable_dump_tensor,
  //     py_function_lib.py:enable_dump_tensor,
  // )

  // Updates the `DumpTensor` ops' file name in `graph_def`. Sets the
  // `file_name` attribute to `quantized_tensor_data.pb`. Returns the updated
  // `GraphDef`.
  //
  // If the function signature changes, likely its corresponding .pyi type
  // hinting and definition should also change.
  // LINT.IfChange
  virtual GraphDef ChangeDumpTensorFileName(
      const GraphDef& graph_def) const = 0;
  // LINT.ThenChange(
  //     pywrap_function_lib.pyi:change_dump_tensor_file_name,
  //     py_function_lib.py:change_dump_tensor_file_name,
  // )
};

}  // namespace tensorflow::quantization

#endif  // TENSORFLOW_COMPILER_MLIR_QUANTIZATION_TENSORFLOW_PYTHON_PY_FUNCTION_LIB_H_
