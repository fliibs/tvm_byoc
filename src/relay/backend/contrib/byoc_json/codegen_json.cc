#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/transform.h>
#include <tvm/relay/type.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>

#include <fstream>
#include <numeric>
#include <regex>
#include <sstream>

#include "../../utils.h"
// #include "comp_op_matcher.h"
// #include "../dnnl/comp_op_matcher.h"

#include "../../../../runtime/contrib/json/json_node.h"
#include "../codegen_json/codegen_json.h"

namespace tvm {
namespace relay {
namespace contrib {

using namespace backend;

/*!
 * \brief Replace var expr which bind with args of call node
 *
 * \param args vector of expression (contains vars or constant nodes)
 * \param cn call node which describe mapping of internal body vars with args
 * \return updated vector of expressions
 */
// static tvm::Array<Expr> BindToCallNodeArgs(const std::vector<Expr>& args, const CallNode* cn) {
//   tvm::Array<Expr> res;
//   for (const auto& arg : args) {
//     if (arg->IsInstance<ConstantNode>()) {
//       res.push_back(arg);
//     } else {
//       auto body_params = cn->op.as<FunctionNode>()->params;
//       auto found = std::find(body_params.begin(), body_params.end(), arg);
//       ICHECK(found != body_params.end());
//       auto idx = std::distance(body_params.begin(), found);
//       res.push_back(cn->args[idx]);
//     }
//   }
//   return res;
// }


/*! \brief Serializer to BYOC JSON runtime module */
class BYOCJSONSerializer : public backend::contrib::JSONSerializer {
  using JSONGraphNode = tvm::runtime::json::JSONGraphNode;
  using JSONGraphNodeEntry = tvm::runtime::json::JSONGraphNodeEntry;

 public:
  BYOCJSONSerializer(const std::string& symbol, const Expr& expr)
      : JSONSerializer("byoc_json_" + symbol, expr) {}

  std::vector<JSONGraphNodeEntry> VisitExpr_(const CallNode* cn) override {
    Expr expr = GetRef<Expr>(cn);
    std::string name;
    tvm::Array<Expr> args;
    std::unordered_map<std::string, dmlc::any> extra_attrs;

    const CallNode* call = cn;
    if (const auto* op_node = cn->op.as<OpNode>()) {
      name = op_node->name;
      args = cn->args;
    } else if (const auto* fn = cn->op.as<FunctionNode>()) {
      auto comp = fn->GetAttr<String>(attr::kComposite);
      ICHECK(comp.defined()) << "BYOC JSON runtime only supports composite functions.";
      name = comp.value();

      /* if (name.find("dnnl.deconv2d") != std::string::npos) {
        call = GetRootCall(fn->body.as<CallNode>(), 10, "nn.conv2d_transpose");
        ICHECK(call->op.as<OpNode>()) << "Not op node";
      } else if (name.find("dnnl.deconv3d") != std::string::npos) {
        call = GetRootCall(fn->body.as<CallNode>(), 10, "nn.conv3d_transpose");
        ICHECK(call->op.as<OpNode>()) << "Not op node";
      } else if (name.find("dnnl.conv1d") != std::string::npos) {
        call = GetRootCall(fn->body.as<CallNode>(), 10, "nn.conv1d");
        ICHECK(call->op.as<OpNode>()) << "Not op node";
      } else */ if (name.find("byoc_json.conv2d") != std::string::npos) {
        call = GetRootCall(fn->body.as<CallNode>(), 10, "nn.conv2d");
        ICHECK(call->op.as<OpNode>()) << "Not op node";
      } else if (name.find("byoc_json.bias_add") != std::string::npos) {
        call = GetRootCall(fn->body.as<CallNode>(), 10, "nn.bias_add");
        ICHECK(call->op.as<OpNode>()) << "Not op node";
      } /* else if (name.find("dnnl.conv3d") != std::string::npos) {
        call = GetRootCall(fn->body.as<CallNode>(), 10, "nn.conv3d");
        ICHECK(call->op.as<OpNode>()) << "Not op node";
      } else if (name.find("dnnl.dense") != std::string::npos) {
        call = GetRootCall(fn->body.as<CallNode>(), 10, "nn.dense");
        ICHECK(call->op.as<OpNode>()) << "Not op node";
      } else if (name.find("dnnl.qnn.conv2d") != std::string::npos ||
                 name.find("dnnl.qnn.dense") != std::string::npos) {
        std::vector<Expr> args_loc;
        call = ParseComposite(*fn, &extra_attrs, &args_loc);
        args = BindToCallNodeArgs(args_loc, cn);
      } */ else {
        LOG(FATAL) << "Unrecognized BYOC pattern: " << name;
      }

      if (args.empty()) {
        args = cn->args;
      }
    } else {
      LOG(FATAL) << "BYOC JSON runtime does not support calls to " << cn->op->GetTypeKey();
    }

    std::vector<JSONGraphNodeEntry> inputs;
    for (const auto& arg : args) {
      auto res = VisitExpr(arg);
      inputs.insert(inputs.end(), res.begin(), res.end());
    }
    auto node = std::make_shared<JSONGraphNode>(name,     /* name_ */
                                                "kernel", /* op_type_ */
                                                inputs, 1 /* num_outputs_ */);
    SetCallNodeAttribute(node, call);
    // If has post-op `clip`. Assume the last op is clip, add clip's attrs to the pattern attrs.
    // if (name.find("_clip") != std::string::npos) {
    //   auto clip_call = cn->op.as<FunctionNode>()->body.as<CallNode>();
    //   ICHECK(IsOp(clip_call, "clip"));
    //   SetCallNodeAttribute(node, clip_call);
    // }
    // For QNN.
    for (const auto& kvp : extra_attrs) node->SetAttr(kvp.first, kvp.second);

    return AddNode(node, GetRef<Expr>(cn));
  }
};

/*!
 * \brief The external compiler/codegen tool. It takes a Relay expression/module and
 * compile it into a runtime module.
 */
runtime::Module BYOCJSONCompiler(const ObjectRef& ref) {
  ICHECK(ref->IsInstance<FunctionNode>());
  auto func = Downcast<Function>(ref);
  auto func_name = GetExtSymbol(func);
  BYOCJSONSerializer serializer(func_name, func);
  serializer.serialize();
  std::string graph_json = serializer.GetJSON();

  // Note that serializer.const_name_to_constant() is ignored. Instead the TECompiler invokes
  // a callback which calls backend::UpdateConstants to capture the map before the function
  // 'disappears' into lowered form, on the assumption the visit order and thus constant
  // names match those generated by the JSONSerializer.

  const auto* pf = runtime::Registry::Get("runtime.BYOCJSONRuntimeCreate");
  ICHECK(pf != nullptr) << "Cannot find DNNL JSON runtime module to create";
  auto mod = (*pf)(func_name, graph_json, serializer.const_names());
  return mod;
}

TVM_REGISTER_GLOBAL("relay.ext.byoc_json").set_body_typed(BYOCJSONCompiler);

/*!
 * \brief Constant Updater for BYOC JSON runtime
 *
 * Not all originally existing ConstantNode should be passed to JSON runtime.
 * Some of them may be skipped or change ordering. So we have to apply the same traversing through
 * the graph as BYOCJSONSerializer.
 */
struct BYOCJSONConstantUpdater : public ConstantUpdater {
 public:
  BYOCJSONConstantUpdater(const std::string& symbol,
                      std::unordered_map<std::string, runtime::NDArray>* params)
      : ConstantUpdater("byoc_json_" + symbol, params) {}
  using ConstantUpdater::VisitExpr_;

  void VisitExpr_(const CallNode* cn) final {
    this->VisitSpan(cn->span);

    if (const auto* fn = cn->op.as<FunctionNode>()) {
      std::vector<Expr> args_loc;
      std::unordered_map<std::string, dmlc::any> attrs;
      // auto root_cn = ParseComposite(*fn, &attrs, &args_loc);

      // auto args = root_cn ? BindToCallNodeArgs(args_loc, cn) : cn->args;
      auto args = cn->args;

      // Customized visit order of args
      for (const auto& arg : args) {
        this->VisitExpr(arg);
      }
    } else {
      // Original visit order of args
      for (auto arg : cn->args) {
        this->VisitExpr(arg);
      }
    }
  }
};

/*!
 * \brief The external compiler/codegen tool. It takes a Relay expression/module and
 * produce collection of required constant NDArrays.
 */
Map<String, runtime::NDArray> BYOCJSONConstantUpdaterFunc(Expr expr, std::string symbol) {
  // Visit all suitable constant nodes
  std::unordered_map<std::string, runtime::NDArray> res;
  BYOCJSONConstantUpdater const_updater(symbol, &res);
  const_updater(expr);

  // Convert to tvm::Map
  Map<String, runtime::NDArray> ret;
  for (const auto& kvp : res) ret.Set(kvp.first, kvp.second);
  return ret;
}

TVM_REGISTER_GLOBAL("relay.ext.byoc_json.constant_updater").set_body_typed(BYOCJSONConstantUpdaterFunc);

}  // namespace contrib
}  // namespace relay
}  // namespace tvm
