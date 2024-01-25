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

/*!
 * 
 * \brief Implementation of codegen APIs.
 */

#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/transform.h>
#include <tvm/relay/type.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>
#include <tvm/ir/expr.h>

#include <fstream>
#include <numeric>
#include <regex>
#include <sstream>

#include "../../utils.h"

#include "codegen_byoc.h"
#include "match_op.h"

namespace tvm {
namespace relay {
namespace contrib {

using namespace backend;

class CodegenC : public backend::MemoizedExprTranslator<std::vector<Output>>, public CodegenCBase {
 public:
  explicit CodegenC(const std::string& id) { this->ext_func_id_ = id; }

  /*!
   * \brief Emit the source code that invokes C compiler compatible wrappers.
   *
   * \return The emitted code.
   */
  std::string JIT(const std::vector<Output>& out) override {
    // Write function macros

    // for (auto decl : func_decl_) {
    //   code_stream_ << decl << "\n";
    // }
    return JitImpl(ext_func_id_, ext_func_args_, buf_decl_, ext_func_body_, const_array_name_, out);
  }

 private:
  // OP Codegen Implement
  void CheckVarOrConstant(const relay::Expr& expr) {
  if (const auto* var = expr.as<relay::VarNode>()) {
      std::cout << "Variable Name: " << var->name_hint() << var->checked_type() << std::endl;
    } else if (const auto* constant = expr.as<relay::ConstantNode>()) {
      std::cout << "Constant Value: " << "Constant" << constant->checked_type() << std::endl;
    } else {
      std::cout << "Not VarNode or ConstantNode" << std::endl;
    }
  }

  template<typename T>
  void AppendVectorToDecl(const std::vector<T>& values, std::string& decl, std::vector<size_t> exclude_last = {}, bool last_comma = false) {
      for (size_t i = 0; i < values.size(); ++i) {
        if (std::find(exclude_last.begin(), exclude_last.end(), i) == exclude_last.end()) {  
          decl += std::to_string(values[i]) + ", "; }
      }
      if (!values.empty() && last_comma) {
          decl.pop_back();
          decl.pop_back();  // Remove the trailing comma
      }
  }

  void AppendIntVectorToDecl(const Array<PrimExpr>& values, std::string& decl, bool last_comma = false) {
      for (const auto& value : values) {
          if (value->IsInstance<tvm::tir::IntImmNode>()) {
              decl += std::to_string(value.as<tvm::tir::IntImmNode>()->value) + ", ";
          }
      }
      if (!values.empty() && last_comma) {
          decl.pop_back();
          decl.pop_back();  // Remove the trailing comma
      }
  }


  std::string Conv2dMacroGen(std::string op_name, std::string func_name, std::string macro_string, const CallNode* call, int id) {
    
    std::string macro;
    // type
    const auto* type_node = call->checked_type().as<TensorTypeNode>();
    ICHECK(type_node);
    const auto& dtype = GetDtypeString(type_node);
    // input size, kernel size, kernel stride, padding   
    auto in_shape = backend::GetShape(call->args[0]->checked_type());
    auto kn_shape = backend::GetShape(call->args[1]->checked_type());
    auto stride_shape = call->attrs.as<Conv2DAttrs>()->strides;
    auto padding_shape = call->attrs.as<Conv2DAttrs>()->padding;

    // Make function
    macro = macro_string + "CSOURCE_" + op_name + "(" + func_name + ", ";
    macro += op_name + ", ";
    macro += dtype   + ", ";
    macro += "int, ";

    for (size_t i = 0; i < in_shape.size(); ++i) {
      macro += std::to_string(in_shape[i]) + ", ";
    }
    for (size_t i = 0; i < kn_shape.size(); ++i) {
      if (i!=2) {
        macro += std::to_string(kn_shape[i]) + ", ";
      }
    }

    std::vector<int> stride_values;
    for (const auto& stride : stride_shape){
      if (stride->IsInstance<tvm::tir::IntImmNode>()) {
        stride_values.push_back(stride.as<tvm::tir::IntImmNode>()->value);
      }
    }
    for (int stride_value : stride_values) {
      macro += std::to_string(stride_value) + ", ";
    }

    std::vector<int> padding_values;
    for (const auto& padding : padding_shape){
      if (padding->IsInstance<tvm::tir::IntImmNode>()) {
        padding_values.push_back(padding.as<tvm::tir::IntImmNode>()->value);
      }
    }
    for (size_t i = 0; i < padding_values.size(); ++i) {
      macro += std::to_string(padding_values[i]);
      if (i < padding_values.size() - 1) {
        macro += ", ";
      }
    }
    macro += ");\n";
    
    return macro;
  }

  std::string Conv2dDeclGen(std::string op_name, std::string func_name, std::string decl_string, const CallNode* call, int id, std::string out) {
      std::string decl = decl_string + func_name + "(";

      // Process arguments
      for (const auto& arg : call->args) {
          auto res = VisitExpr(arg);
          for (const auto& out : res) {
              decl += out.name + ", ";
          }
      }
      decl += out + ", ";

      // Process input size, kernel size (excluding the third dimension), kernel stride, and padding
      auto in_shape = backend::GetShape(call->args[0]->checked_type());
      auto kn_shape = backend::GetShape(call->args[1]->checked_type());
      auto stride_shape = call->attrs.as<Conv2DAttrs>()->strides;
      auto padding_shape = call->attrs.as<Conv2DAttrs>()->padding;

      AppendVectorToDecl(in_shape, decl);
      AppendVectorToDecl(kn_shape, decl, {1});  // Exclude the second dimension of kernel size
      AppendIntVectorToDecl(stride_shape, decl);
      AppendIntVectorToDecl(padding_shape, decl, true);

      decl += ");\n";
      return decl;
  }

  std::string BiasAddDeclGen(std::string op_name, std::string func_name, std::string decl_string, const CallNode* call, int id, std::string out) {
      std::string decl = decl_string + func_name + "(";
      // Process arguments
      for (const auto& arg : call->args) {
          auto res = VisitExpr(arg);
          for (const auto& out : res) {
              decl += out.name + ", ";
          }
      }
      decl += out + ", ";

      auto in_shape = backend::GetShape(call->args[0]->checked_type());
      auto axis = call->attrs.as<BiasAddAttrs>()->axis;

      AppendVectorToDecl(in_shape, decl);
      decl += std::to_string(axis);
      decl += ");\n";
      return decl;
  }


  
  std::string ReluDeclGen(std::string op_name, std::string func_name, std::string decl_string, const CallNode* call, int id, std::string out) {
      std::string decl = decl_string + func_name + "(";
      // input size   
      auto in_shape = backend::GetShape(call->args[0]->checked_type());

      // Process arguments
      for (const auto& arg : call->args) {
          auto res = VisitExpr(arg);
          for (const auto& out : res) {
              decl += out.name + ", ";
          }
      }
      decl += out + ", ";
        // call->args[i].as<relay::ConstantNode>()  //extra branch, need optimize
      AppendVectorToDecl(in_shape, decl, {}, true);

      decl += ");\n";
      return decl;
  }


  std::pair<std::string, std::string> OpFuncGen(const CallNode* call, int id, std::string out) {
    std::string op_name;
    if (backend::IsOp(call, "nn.conv2d")) {
      op_name = "conv2d";
    } else if (backend::IsOp(call, "nn.relu")) {
      op_name = "relu";
    } else if (backend::IsOp(call, "nn.bias_add")) {
      op_name = "bias_add";
    } else {
      LOG(FATAL) << "Unrecognized op";
    }
    // std::string func_name =  op_name+ "_" + std::to_string(id++);
    std::string func_name = "byoc_c_"+op_name;
    std::string macro_string = "";
    std::string decl_string = "";
    
    if (op_name == "conv2d") {
      // macro_string = Conv2dMacroGen(op_name, func_name, macro_string, call, id);
      decl_string = Conv2dDeclGen(op_name, func_name, decl_string, call, id, out);
    } else if (op_name == "relu") {
      decl_string = ReluDeclGen(op_name, func_name, decl_string, call, id, out);
    } else if (op_name == "bias_add") {
      decl_string = BiasAddDeclGen(op_name, func_name, decl_string, call, id, out);
    }
      

    return {macro_string, decl_string};
  }
  

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

  std::vector<Output> VisitExpr_(const ConstantNode* cn) final {
    Output output;
    // Get const: static_cast<float*>(byoc_c_0_consts[0]->data)
    output.name = CreateDataReference(ext_func_id_, const_idx_);
    // std::cout << output.name << std::endl;

    // Generate the global variable for needed ndarrays
    if (const_array_name_.empty()) {
      const_array_name_ = CreateNDArrayPool(ext_func_id_);
      std::string checker = CreateInitChecker(ext_func_id_);
      ext_func_body_.insert(ext_func_body_.begin(), checker);
      // std::cout << const_array_name_ << std::endl;
      // std::cout << checker << std::endl;
    }

    output.dtype = "float";
    std::string const_symbol = ext_func_id_;
    std::string const_var_name = CreateConstVar(const_symbol, const_idx_);
    // std::cout << const_symbol << std::endl;
    // std::cout << const_var_name << std::endl;
    const_vars_.push_back(const_var_name);
    const_idx_++;

    const auto* type_node = cn->checked_type().as<TensorTypeNode>();
    ICHECK(type_node);
    ICHECK_EQ(GetDtypeString(type_node), "float") << "Only float is supported for now.";

    return {output};
  }


  std::vector<Output> VisitExpr_(const CallNode* call) override {
    std::ostringstream macro_stream;
    std::ostringstream decl_stream;
    std::ostringstream buf_stream;

    // CallNode Description
    std::cout << "//" << "Desctribe_OP: "<< std::endl;
    std::cout << "//" << "op_name: "   << call->op                 << std::endl;
    std::cout << "//" << "attrs: "     << call->attrs              << std::endl;
    // std::cout << "//" << "size: "            << call->args.size()        << std::endl;
    // for (size_t i=0; i<call->args.size(); ++i) {
    //   std::cout << "//" << "args["      << i << "]: " << call->args[i] << std::endl;
    // }
    for (size_t i=0; i<call->type_args.size(); ++i) {
      std::cout << "//" << "type_args["      << i << "]: " << call->type_args[i] << std::endl;
    }
    // std::cout << "//" << "type_args: "       << call->type_args          << std::endl;
    // std::cout << "//" << "virtual_device_: " << call->virtual_device_          << std::endl;
    // std::cout << "//" << "span: "            << call->span                     << std::endl;
    // std::cout << "//" << "attribute: "       << std::endl;       
    // macro_stream << "//" << "data_layout: "     << call->attrs.as<Conv2DAttrs>()->data_layout << "\n "; 
    // macro_stream << "//" << "kernel_layout: "   << call->attrs.as<Conv2DAttrs>()->kernel_layout << "\n "; 
    // macro_stream << "//" << "padding: "         << call->attrs.as<Conv2DAttrs>()->padding << "\n "; 
    // macro_stream << "//" << "stride: "          << call->attrs.as<Conv2DAttrs>()->strides << "\n ";
    // Testing End /////////////////////////

    // type
    const auto* type_node = call->checked_type().as<TensorTypeNode>();
    ICHECK(type_node);
    const auto& dtype = GetDtypeString(type_node);

    std::string out = "out" + std::to_string(out_idx_++);
    std::pair<std::string, std::string> res_string = OpFuncGen(call, func_idx, out);
    std::string macro_string = std::get<0>(res_string);
    std::string decl_string  = std::get<1>(res_string);
 
    // macro_stream << macro_string;
    // func_decl_.push_back(macro_stream.str());
    decl_stream  << decl_string;
    ext_func_body_.push_back(decl_stream.str());

    // auto out_shape = backend::GetShape(call->checked_type());
    // int out_size = 1;
    // for (size_t i = 0; i < out_shape.size(); ++i) {
    //   out_size *= out_shape[i];
    // }
    // std::cout << out_size << std::endl;
    // buf_stream << dtype << "* " << out << " = (" << dtype << "*)malloc(4 * " << out_size << ");";
    // buf_decl_.push_back(buf_stream.str());
    
    // Update output buffer
    // Note C codegen only handles TensorType. Therefore, we don't flatten
    // tuples and only return a single vaule.
    Output output;
    output.name = out;
    output.dtype = dtype;
    output.need_copy = false;
    return {output};
  }

  /*!
   * \brief The accumulated constant name to constant mapping. Shared between all generated
   * functions.
   */
  // std::unordered_map<std::string, runtime::NDArray>* const_name_to_constant_;
  int const_idx_ = 0;
  /*! \brief The accumulated constant names, in the order they were generated. */
  // Array<String>* const_names_;
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
  int out_idx_ = 0;
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
  /*! \brief The variable name to constant mapping. */
  Array<String> const_vars_;

  friend class CModuleCodegen;
};


class CModuleCodegen : public CSourceModuleCodegenBase {
 public:
  // Create a corresponding DNNL function for the given relay Function.
  std::pair<std::string, Array<String>> GenCFunc(const Function& func) {
    ICHECK(func.defined()) << "Input error: expect a Relay function.";

    // Record the external symbol for runtime lookup.
    auto sid = GetExtSymbol(func);

    CodegenC builder(sid);
    auto out = builder.VisitExpr(func->body);
    code_stream_ << builder.JIT(out);

    return {sid, builder.const_vars_};
  }

  /*!
   * \brief The overridden function that will create a CSourceModule. In order
   * to compile the generated C source code, users need to specify the paths to
   * some libraries, including some TVM required and dnnl specific ones. To make
   * linking simpiler, the DNNL kernels are wrapped in a TVM compatible manner
   * and live under tvm/src/runtime/contrib/dnnl folder.
   *
   * \param ref An object ref that could be either a Relay function or module.
   *
   * \return The runtime module that contains C source code.
   */
  runtime::Module CreateCSourceModule(const ObjectRef& ref) final {
    // Create headers
    code_stream_ << "#include <cstdint>\n";
    code_stream_ << "#include <cstdlib>\n";
    code_stream_ << "#include <cstring>\n";
    code_stream_ << "#include <vector>\n";
    code_stream_ << "#include <tvm/runtime/c_runtime_api.h>\n";
    code_stream_ << "#include <tvm/runtime/c_backend_api.h>\n";

    code_stream_ << "#include <tvm/runtime/ndarray.h>\n";
    code_stream_ << "#include <tvm/runtime/packed_func.h>\n";
    code_stream_ << "#include <byoc_c/byoc_c_kernel.h>\n";
    code_stream_ << "using namespace tvm::runtime;\n";
    code_stream_ << "using namespace tvm::runtime::contrib;\n";
    // code_stream_ << "\n";

    ICHECK(ref->IsInstance<FunctionNode>());
    auto res = GenCFunc(Downcast<Function>(ref));
    std::string code = code_stream_.str();
    String sym = std::get<0>(res);
    Array<String> variables = std::get<1>(res);

    // Create a CSource module
    const auto* pf = runtime::Registry::Get("runtime.CSourceModuleCreate");
    ICHECK(pf != nullptr) << "Cannot find csource module to create the external runtime module";
    // TODO(@manupa-arm): pass the function names to enable system-lib creation
    return (*pf)(code, "c", Array<String>{sym}, variables);
  }
 private:
  /*!
   * \brief The code stream that prints the code that will be compiled using
   * external codegen tools.
   */
  std::ostringstream code_stream_;
};

runtime::Module CCompiler(const ObjectRef& ref) {
  CModuleCodegen byoc_c;
  return byoc_c.CreateCSourceModule(ref);
}

TVM_REGISTER_GLOBAL("relay.ext.byoc_c").set_body_typed(CCompiler);

}  // namespace contrib
}  // namespace relay
}  // namespace tvm