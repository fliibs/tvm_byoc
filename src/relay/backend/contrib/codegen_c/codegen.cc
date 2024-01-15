/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/transform.h>
#include <tvm/relay/type.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/object.h>
#include <tvm/runtime/registry.h> 

#include <sstream>
#include <string>

#include "../../../transforms/compiler_function_utils.h"
#include "../../utils.h"
#include "codegen_c.h"

namespace tvm {
namespace relay {
namespace contrib {

/*! \brief Return the "ccompiler" Target instance to use to guide compilation. */
Target GetCCompilerTarget() {
  Target target = Target::Current(/*allow_not_defined=*/true);
  if (!target.defined() || target->kind->name != "ccompiler") {
    // Use the default compilation options if no specific "ccompiler" target was given
    // in the overall targets list. In that case target_hooks.cc will invoke the custom pass
    // without pushing any target instance onto the implicit target stack.
    target = Target("ccompiler");
  }
  return target;
}

/*!
 * \brief Emits C/C++ code for a single function.
 *
 * For testing and demonstration only, only a few binary operators are supported.
 */
class CodegenC : public backend::MemoizedExprTranslator<std::vector<Output>>, public CodegenCBase {
 public:
  CodegenC(std::unordered_map<std::string, runtime::NDArray>* const_name_to_constant,
           Array<String>* const_names, bool* needs_extra_headers, std::string ext_func_id)
      : const_name_to_constant_(const_name_to_constant),
        const_names_(const_names),
        needs_extra_headers_(needs_extra_headers),
        ext_func_id_(std::move(ext_func_id)) {}

  /*!
   * \brief Emit the source code that invokes C compiler compatible wrappers.
   *
   * \return The emitted code.
   */
  std::string JIT(const std::vector<Output>& out) override {
    // Write function macros
    for (auto decl : func_decl_) {
      code_stream_ << decl << "\n";
    }
    return JitImpl(ext_func_id_, ext_func_args_, buf_decl_, ext_func_body_, const_array_name_, out);
  }

 private:
  std::vector<Output> VisitExprDefault_(const Object* op) override {
    LOG(FATAL) << "C codegen doesn't support: " << op->GetTypeKey();
  }

  std::vector<Output> VisitExpr_(const VarNode* node) override {
    ext_func_args_.push_back(GetRef<Var>(node));
    Output output;
    output.name = node->name_hint();
    return {output};
  }

  std::vector<Output> VisitExpr_(const TupleNode* node) override {
    std::vector<Output> outs;
    for (auto field : node->fields) {
      auto res = VisitExpr(field);
      ICHECK_EQ(res.size(), 1U) << "Do not support tuple nest";
      outs.push_back(res[0]);
    }
    return outs;
  }

  std::vector<Output> VisitExpr_(const TupleGetItemNode* op) override {
    auto res = VisitExpr(op->tuple);
    ICHECK_GT(res.size(), static_cast<size_t>(op->index));

    // Only keep the item we want for the child node.
    // FIXME(@comaniac): The other items should still be requried for the primary outputs.
    return {res[op->index]};
  }

  std::vector<Output> VisitExpr_(const ConstantNode* cn) override {
    // Remember we'll need some extra headers to support the runtime constants array.
    *needs_extra_headers_ = true;

    std::ostringstream decl_stream;
    std::ostringstream buf_stream;

    Output output;
    // Get const: static_cast<float*>(gcc_0_consts[0]->data)
    size_t const_id = const_name_to_constant_->size();
    output.name = CreateDataReference(ext_func_id_, const_id);
    const auto* type_node = cn->checked_type().as<TensorTypeNode>();
    ICHECK(type_node);
    const auto& dtype = GetDtypeString(type_node);

    // Generate the global variable for needed ndarrays
    if (const_array_name_.empty()) {
      *needs_extra_headers_ = true;
      const_array_name_ = CreateNDArrayPool(ext_func_id_);
      std::string checker = CreateInitChecker(ext_func_id_);
      ext_func_body_.insert(ext_func_body_.begin(), checker);
    }

    ICHECK(dtype == "float" || dtype == "int") << "Only float and int are supported for now.";
    output.dtype = dtype;

    std::string const_var_name = CreateConstVar(ext_func_id_, const_id);
    const_name_to_constant_->emplace(const_var_name, cn->data);
    const_names_->push_back(const_var_name);

    return {output};
  }

  std::vector<Output> VisitExpr_(const CallNode* call) override {
    std::ostringstream macro_stream;
    std::ostringstream decl_stream;
    std::ostringstream buf_stream;

    std::string func_name = "printf";

    // OP
    const auto* op_node = call->op.as<OpNode>();
    Op op = GetRef<Op>(op_node);

    // input size
    auto in_shape = backend::GetShape(call->args[0]->checked_type());

    // type
    const auto* type_node = call->checked_type().as<TensorTypeNode>();
    ICHECK(type_node);
    const auto& dtype = GetDtypeString(type_node);

    // Make function call when visiting arguments
    decl_stream << func_name << "(\"";
    decl_stream << op << ",";
    for (size_t i = 0; i < in_shape.size(); ++i) {
      decl_stream << in_shape[i] << ", ";
    }
    decl_stream << dtype << "\\n\");";


    std::string out = "buf_" + std::to_string(buf_idx_++);
    auto out_shape = backend::GetShape(call->checked_type());
    int out_size = 1;
    for (size_t i = 0; i < out_shape.size(); ++i) {
      out_size *= out_shape[i];
    }
    buf_stream << dtype << "* " << out << " = (" << dtype << "*)malloc(4 * " << out_size << ");";
    buf_decl_.push_back(buf_stream.str());

    ext_func_body_.push_back(decl_stream.str());

    // Update output buffer
    // Note C codegen only handles TensorType. Therefore, we don't flatten
    // tuples and only return a single vaule.
    Output output;
    output.name = out;
    output.dtype = dtype;
    output.need_copy = true;
    output.size = out_size;
    return {output};
  }

  /*!
   * \brief The accumulated constant name to constant mapping. Shared between all generated
   * functions.
   */
  std::unordered_map<std::string, runtime::NDArray>* const_name_to_constant_;
  /*! \brief The accumulated constant names, in the order they were generated. */
  Array<String>* const_names_;
  /*!
   * \brief Set to true if the ndarray and packed function headers are required to declare and
   * manage the constants array.
   */
  bool* needs_extra_headers_;
  /*! \brief Name of the global function currently being compiled. */
  std::string ext_func_id_;

  /*! \brief The index of the next available wrapped C function. */
  int func_idx = 0;
  /*! \brief The index of the next available allocated buffers. */
  int buf_idx_ = 0;
  /*! \brief The arguments of a C compiler compatible function. */
  Array<Var> ext_func_args_;
  /*! \brief The statements of a C compiler compatible function. */
  std::vector<std::string> ext_func_body_;
  /*! \brief The array declared to store the constant values. */
  std::string const_array_name_;
  /*! \brief The declaration statements of a C compiler compatible function. */
  std::vector<std::string> func_decl_;
  /*! \brief The declaration statements of buffers. */
  std::vector<std::string> buf_decl_;
};




/*! \brief Emits C/C++ code for a module. */
class CodegenCModule {
 public:
  CodegenCModule(Target target, IRModule mod) : target_(std::move(target)), mod_(std::move(mod)) {}

  runtime::Module CreateCSourceModule() {
    for (const auto& kv : mod_->functions) {
      if (const auto* function_node = GetCCompilerFunctionNode(kv.second)) {
        GenCFunc(GetRef<Function>(function_node));
      }
    }
    return Finalize();
  }

  /*! \brief Returns the accumulated constant name to constant mapping. */
  const std::unordered_map<std::string, runtime::NDArray>& const_name_to_constant() const {
    return const_name_to_constant_;
  }

 private:
  /*! \brief Emits the standard C/C++ header into \p os. */
  void EmitPreamble(std::ostringstream& os) {
    // Custom header, if any.
    Optional<String> header = target_->GetAttr<String>("header");
    if (header.defined() && !header.value().empty()) {
      os << header.value().c_str() << "\n";
    }

    // Standard includes.
    os << "#include <stdio.h>\n";
    os << "#include <stdlib.h>\n";
    os << "#include <string.h>\n";
    os << "#include <tvm/runtime/c_runtime_api.h>\n";
    os << "#include <tvm/runtime/c_backend_api.h>\n";

    os << "#ifdef __cplusplus\n";
    os << "#include <tvm/runtime/ndarray.h>\n";
    os << "#include <tvm/runtime/packed_func.h>\n";
    os << "#endif\n";
    os << "\n";

  }

  void GenCFunc(const Function& function) {
    ICHECK(function.defined()) << "Input error: expect a Relay function.";
    std::string ext_func_id = backend::GetExtSymbol(function);
    CodegenC builder(&const_name_to_constant_, &const_names_, &needs_extra_headers_, ext_func_id);
    std::vector<Output> out = builder.VisitExpr(function->body);
    code_stream_ << builder.JIT(out);
    func_names_.push_back(ext_func_id);
  }

  /*! \brief Returns function if it is tagged with "Compiler=ccompiler". */
  static const FunctionNode* GetCCompilerFunctionNode(const Expr& expr) {
    if (const auto* function_node = expr.as<FunctionNode>()) {
      Optional<String> opt_compiler = function_node->GetAttr<String>(attr::kCompiler);
      if (opt_compiler.defined() && opt_compiler.value() == "ccompiler") {
        return function_node;
      }
    }
    return nullptr;
  }

  runtime::Module Finalize() {
    std::ostringstream os;
    EmitPreamble(os);
    os << code_stream_.str();
    std::string code = os.str();

    VLOG(1) << "CodegenCModule generated:" << std::endl << code;

    // Create a CSource module
    const auto* pf = runtime::Registry::Get("runtime.CSourceModuleCreate");
    ICHECK(pf != nullptr) << "Cannot find csource module to create the external runtime module";
    return (*pf)(code, "c", func_names_, const_names_);
  }

  /*! \brief "ccompiler" Target with compilation options to use. */
  Target target_;
  /*! \brief Module we are compiling. */
  IRModule mod_;

  /*! \brief True if we need to include the ndarray and packed function headers. */
  bool needs_extra_headers_ = false;
  /*! \brief The accumulated constant name to constant mapping. */
  std::unordered_map<std::string, runtime::NDArray> const_name_to_constant_;
  /*! \brief The accumulated constant names, in the order they were generated. */
  Array<String> const_names_;
  /*! \brief The accumulated function names. */
  Array<String> func_names_;
  /*!
   * \brief The accumulated code stream containing all function definitions.
   * (Does not include the preamble.)
   */
  std::ostringstream code_stream_;
};

/*! \brief The actual translation pass. */
tvm::transform::Pass CCompilerImpl() {
  auto pass_func = [=](IRModule mod, const tvm::transform::PassContext& pass_ctx) {
    VLOG(1) << "CCompilerImpl input:" << std::endl << PrettyPrint(mod);
    Target target = GetCCompilerTarget();

    // Emit the C/C++ code and package it as a CSourceModule.
    CodegenCModule codegen(target, mod);
    runtime::Module runtime_mod = codegen.CreateCSourceModule();

    // Capture the new runtime module.
    Array<runtime::Module> external_mods =
        mod->GetAttr<Array<runtime::Module>>(tvm::attr::kExternalMods).value_or({});
    external_mods.push_back(runtime_mod);

    // Capture the new constants.
    Map<String, runtime::NDArray> const_name_to_constant =
        mod->GetAttr<Map<String, runtime::NDArray>>(tvm::attr::kConstNameToConstant).value_or({});
    for (const auto& kv : codegen.const_name_to_constant()) {
      ICHECK_EQ(const_name_to_constant.count(kv.first), 0);
      const_name_to_constant.Set(kv.first, kv.second);
    }

    return WithAttrs(mod, {{tvm::attr::kExternalMods, external_mods},
                           {tvm::attr::kConstNameToConstant, const_name_to_constant}});
  };
  return tvm::transform::CreateModulePass(pass_func, 0, "CCompilerImpl", {});
}

tvm::transform::Pass CCompilerPass() {
  return transform::Sequential(
      {transform::OutlineCompilerFunctionsWithExistingGlobalSymbols("ccompiler"), CCompilerImpl(),
       transform::MarkCompilerFunctionsAsExtern("ccompiler")});
}

}  // namespace contrib
}  // namespace relay
}  // namespace tvm
