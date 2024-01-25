import os
from tvm import runtime

def save_lib(lib, graph, param):
    tvm_home = os.getenv('TVM_HOME')
    contrib_path = os.path.join(tvm_home, "src", "runtime", "contrib")
    kwargs = {}
    kwargs["options"] = ["-O2", "-std=c++17", "-I" + contrib_path]
    lib.export_library(r"out/model.so", fcompile=False, **kwargs)

    with open(r"out/graph.json", "w") as f_graph_json:
        f_graph_json.write(graph)
    with open(r"out/param.bin", "wb") as f_params:
        f_params.write(runtime.save_param_dict(param))
        # for k, v in param.items():
        #     print(f'{k}: {v}')

def load_lib():
    # Load model.so
    lib = runtime.module.load_module(r"out/model.so")
    # Load graph.json
    with open(r"out/graph.json", "r") as f_graph_json:
        graph = f_graph_json.read()
    # Load param.bin
    with open(r"out/param.bin", "rb") as f_params:
        param = runtime.load_param_dict(f_params.read())
    return lib, graph, param


with open(r"out/param.bin", "rb") as f_params:  
    param = runtime.load_param_dict(f_params.read())

print(param)

with open("output.bin", "rb") as file:
    # 读取整个文件内容
    binary_data = file.read()

# 打印二进制数据的十六进制表示
hex_representation = " ".join(format(byte, "02X") for byte in binary_data)
print(hex_representation)