#include <dmlc/logging.h>
#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/memory.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/object.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>


#include <cstddef>
#include <regex>
#include <fstream>
#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <vector>


#include "../json/json_node.h"
#include "../json/json_runtime.h"



// using namespace tvm::runtime


namespace tvm {
namespace runtime {
namespace contrib {

    using namespace tvm::runtime;
    using namespace tvm::runtime::json;

    class BYOCJSONRuntime : public JSONRuntimeBase {
        public:
            BYOCJSONRuntime(const std::string& symbol_name, const std::string& graph_json, const Array<String> const_names)
            : JSONRuntimeBase(symbol_name, graph_json, const_names),
            next_unique_eid_offset_(data_entry_.size()),
            run_arg_eid_(input_var_eid_) {
                for (const auto e : outputs_) run_arg_eid_.push_back(EntryID(e));
            }

            const char* type_key() const override { return "byoc_json"; }

            void Init(const Array<NDArray>& consts) override {
                ICHECK_EQ(consts.size(), const_idx_.size())
                    << "The number of input constants must match the number of required.";

                // Setup constants entries for weights.
                SetupConstants(consts);
                // BuildEngine();
            }

             /* Unused stub implementation */
            void Run() override {
                // LOG(FATAL) << "Unreachable code";
                LOG(WARNING) << "Unreachable code";
            }

            /* Thread safe implementation of Run. Keep runtime instance immutable */
            void Run(int id, const TVMArgs& args) const {
                std::vector<TVMValue> values(args.size());
                std::vector<int> type_codes(args.size());

                TVMArgsSetter setter(values.data(), type_codes.data());

                if (op_id_[id] == "conv2d") {
                    for (size_t i=0; i<args.size(); i++) {
                        setter(i, data_entry_[args[i]]);
                    }
                    // Conv2d(values.data(), type_codes.data(), args.size());
                } else {
                    LOG(FATAL) << "Unknown op: " << op_id_[id] << "\n";
                }

                // auto arg_data_provider = makeIODataProvider(args);
                // auto mem_solver = tensor_registry_.MakeSolver(arg_data_provider);
                // // Execute primitives one by one
                // for (const auto& act : net_) {
                // auto prim = std::get<0>(act);
                // auto arg_reqs = std::get<1>(act);

                // // Find proper dnnl::memory buffers
                // std::unordered_map<int, dnnl::memory> mem_args;
                // for (const auto& kvp : arg_reqs) mem_args[kvp.first] = mem_solver(kvp.second);

                // // skip the reorder if src==dst to enable inplace operation
                // if (prim.get_kind() == dnnl::primitive::kind::reorder) {
                //     const auto& mem_src = mem_args.at(DNNL_ARG_SRC);
                //     const auto& mem_dst = mem_args.at(DNNL_ARG_DST);
                //     if ((mem_src.get_desc() == mem_dst.get_desc()) &&
                //         (mem_src.get_data_handle() == mem_dst.get_data_handle())) {
                //     continue;
                //     }
                // }

                // prim.execute(stream_, mem_args);
                // }
            }

        private:
            uint32_t next_unique_eid_offset_;
            std::vector<uint32_t> run_arg_eid_;
            std::vector<NDArray> data_entry_;
            std::vector<std::string> op_id_;
            
    };

    runtime::Module BYOCJSONRuntimeCreate(String symbol_name, String graph_json, const Array<String>& const_names) {
        auto n = make_object<BYOCJSONRuntime>(symbol_name, graph_json, const_names);
        return runtime::Module(n);
    }

    TVM_REGISTER_GLOBAL("runtime.BYOCJSONRuntimeCreate").set_body_typed(BYOCJSONRuntimeCreate);
    TVM_REGISTER_GLOBAL("runtime.module.loadbinary_byoc_json").set_body_typed(JSONRuntimeBase::LoadFromBinary<BYOCJSONRuntime>);


    // class BYOCJsonModule : public ModuleNode {
    //     public:
    //         explicit BYOCJsonModule(std::string graph_json) {
    //             this->grpah_json_ = graph_json;
    //             ParseJson(this->grpah_json_);
    //         };

    //         PackedFunc GetFunction(const std::string name,
    //                             const ObjectPtr<Object>& sptr_to_self) final {
    //             if (this->graph_.find(name) != this->graph_.end()) {
    //                 this->curr_subgraph_ = name;
    //                 return PackedFunc([sptr_to_self, this](TVMArgs args, TMVRetValue* rv) {
    //                     for (auto i=0; i<args.size(); ++i) {
    //                         ICHECK(args[i].type_code() == kNDArrayContainer || args[i].type_code() == kArrayHandle)
    //                         if (args[i].type_code() == kArrayHandle) {
    //                             DLTensor* arg = args[i];
    //                             this->data_entry_[i].CopyFrom(arg);
    //                         } else {
    //                             NDArray arg = args[i];
    //                             this->data_entry_[i].CopyFrom(arg);
    //                         }
    //                     }

    //                     for (const auto& it : this->graph_[this->curr_subgraph_]) {
    //                         this->Run(it.id, it.inputs, it.output);
    //                     }
    //                     ICHECK_GT(graph_.count(this->curr_subgraph_), OU);

    //                     auto out_idx = graph_[this->curr_subgraph_].back().output;
    //                     if (args[args.size()-1].type_code() == kArrayHandle) {
    //                         DLTensor* arg = args[args.size()-1];
    //                         this->data_entry_[out_idx].CopyTo(arg);
    //                     } else {
    //                         NDArray arg = args[args.size()-1];
    //                         this->data_entry_[out_idx].CopyTo(arg);
    //                     }
    //                     *rv = data_entry_.back();
    //                 });
    //             } else {
    //                 LOG(FATAL) << "Unknown subgraph: " << name << "\n";
    //                 return PackedFunc();
    //             }
    //         }

    //         const char* type_key() const { return "byoc_json"; }

    //         void SaveToBinary(dmlc::Stream stream) final {
    //             stream->Write(this->grpah_json_);
    //         }

    //         static Module LoadFromBinary(void* strm){
    //             dmlc::Stream* stream = static_cast<dmlc::Stream*>(strm);
    //             std::string graph_json;
    //             stream->Read(&graph_json);
    //             auto n = tvm::runtime::make_object<BYOCJsonModule>(graph_json);
    //             return Module(n);
    //         }

    //         static Module Create(const std::string path) {
    //             std::ifstream filep;
    //             filep.open(path, std::ios::in);
    //             std::string graph_json;
    //             std::string line;
    //             while (std::getline(filep, line)) {
    //                 graph_json += line;
    //                 graph_json += "\n";
    //             }
    //             filep.close();
    //             auto n = tvm::runtime::make_object<BYOCJsonModule>(graph_json);
    //             return Module(n);
    //         }

    //         std::string GetSource(const std::string& format = "");

    //         void Run(int id, const std::vector<int>& inputs, int output) {
    //             std::vector<int> args(inputs.begin(), inputs.end());
    //             args.push_back(output);

    //             std::vector<TVMValue> values(args.size());
    //             std::vector<int> type_codes(args.size());

    //             TVMArgsSetter setter(values.data(), type_codes.data());

    //             if (op_id_[id] == "conv2d") {
    //                 for (size_t i=0; i<args.size(); i++) {
    //                     setter(i, data_entry[args[i]]);
    //                 }
    //                 // Conv2d(values.data(), type_codes.data(), args.size());
    //             } else {
    //                 LOG(FATAL) << "Unknown op: " << op_id_[id] << "\n";
    //             }
    //         }

    //         void ParseJson(const std::string& json) {
    //             std::string line;
    //             std::string curr_subgraph;
    //             std::stringstream ss(json);

    //             while (std::getline(ss, line, '\n')) {
    //                 std::stringstream ss2(line);
    //                 std::string token;
    //                 int id = 0;

    //                 ss2 >> token;
    //                 if (token.find("subgraph_") != std::string::nops) {
    //                     curr_subgraph = token;
    //                     continue;
    //                 }

    //                 ss2 >> id;
    //                 if (io_id.size() <= static_cast<size_t>(id)) {
    //                     op_id_.resize(id+1);
    //                     data_entry_.resize(id+1);
    //                 }

    //                 int64_t totla_elements = 1;
    //                 std::vector<int64_t> shape;
    //                 if (token == "input") {
    //                     int64_t size = 0;
    //                     while (ss2 >> size) {
    //                         total_eleents *= size;
    //                         shape.push_back(size);
    //                     }
    //                 } else {
    //                     op_id_[id] = token;
    //                     bool shape_data = false;
    //                     NodeEntry entry;
    //                     while (ss2 >> token) {
    //                         if (token == "shape") {
    //                             shape_data = true;
    //                         } else if (shape_data) {
    //                             total_elements += std::stoll(token);
    //                             shape.push_back(std::stoll(token));
    //                         } else if (token != "inputs:") {
    //                             entry.inputs.push_back(std::stoi(token));
    //                         }
    //                     }
    //                 }
    //                 entry.id = id;
    //                 entry.output = id;
    //                 graph_[curr_subgraph].push_back[entry];
    //             }
    //             DLDevice dev;
    //             dev.device_type = static_cast<DLDeviceType>(1);
    //             dev.device_id = 0;
    //             data_entry_[id] = NDArray::Empty(shape, DLDataType{kDLFloat, 32, 1}, dev);
    //         };

    //     private:
    //         std::string grpah_json_;
    //         std::string curr_subgraph_;
    //         std::map<std::string, std::vector<NodeEntry>> graph_;
    //         std::vector<NDArray> data_entry_;
    //         std::vector<std::string> op_id_;



    // };

    // TVM_REGISTER_GLOBAL("module.byoc_json_module_create")
    // .set_body_typed([](std::string code){
    //     auto n = make_object<BYOCJsonModule>(code);
    //     return runtime::Module(n);
    // });

    // TVM_REGISTER_GLOBAL("module.loadbinary_byocjson")
    // .set_body_typed(BYOCJsonModule::LoadFromBinary);

    // TVM_REGISTER_GLOBAL("module.loadfile_byocjson")
    // .set_body_typed([](TVMArgs args, TVMRetValue* rv) {
    //     *rv = BYOCJsonModule::Create(args[0]);
    // });
}
}
}