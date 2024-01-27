import numpy as np
import onnx
import tvm
from tvm import relay, runtime
from utils import *

def write_output(path, content, verbose=True):
    if verbose:
        print(content)
    out = open(path, 'w')
    out.write(content)
    out.close()

input_name = 'input'
shape_dict = {input_name: [1,3,224,224]}
model_path = 'single_cnn_3_1_3.onnx'
# model_path = '../workspace/model/mobilenet_v2_224x224.onnx'
model = onnx.load(model_path)
mod, params = tvm.relay.frontend.from_onnx(model, shape_dict)

seq_1 = tvm.transform.Sequential(
    [
        relay.transform.InferType(),
        # relay.transform.MergeComposite(),
        relay.transform.AnnotateTarget("dnnl"),
        relay.transform.PartitionGraph(),
        relay.transform.InferType(),
    ]
)

mod_1 = seq_1(mod)
# print(mod_1.script())
write_output('out/out.relay', mod_1.script())

with tvm.transform.PassContext(3):
    graph_module = relay.build(mod_1, target="llvm", params=params)
print(graph_module.get_lib().imported_modules)

i=0
for module in graph_module.get_lib().imported_modules:
    # print(module)
    module_key_type = module.type_key.split('_')[-1]
    write_output(f'out/out_{i}.{module_key_type}', module.get_source())
    i+=1

save_lib(graph_module.get_lib(),graph_module.get_graph_json(),graph_module.get_params())

lib, graph, param = load_lib()
print(param)

numpydata = np.random.randn(1,3,224,224).astype("float32")
# print(numpydata)
map_inputs = {}
map_inputs["input"] = numpydata


from tvm import cpu
from tvm.contrib import graph_executor
rt_mod = graph_executor.create(graph, lib, cpu())
for name, data in map_inputs.items():
    rt_mod.set_input(name, data)
rt_mod.set_input(**param)

rt_mod.run()

output_shape = (1,3,220,220)
rt_mod.get_output(0, tvm.nd.empty(output_shape))