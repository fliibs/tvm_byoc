import numpy as np
import onnx
import tvm
from tvm import relay, runtime
from utils import *
from tvm.relay.op.contrib.byoc_c import pattern_table


input_name = 'input'
shape_dict = {input_name: [1,3,224,224]}
# model_path = 'single_cnn_3_1_3.onnx'
model_path = 'cnn_relu_3_1_3.onnx'
model = onnx.load(model_path)
mod, params = tvm.relay.frontend.from_onnx(model, shape_dict)



if not tvm.get_global_func("relay.ext.byoc_c"):
    print("there is not external byoc_c")

seq_1 = tvm.transform.Sequential(
    [
        relay.transform.InferType(),
        # relay.transform.MergeComposite(pattern_table()),
        relay.transform.AnnotateTarget("byoc_c"),
        relay.transform.PartitionGraph(),
        relay.transform.InferType(),
    ]
)

mod_1 = seq_1(mod)
print(mod_1)

with tvm.transform.PassContext(3):
    graph_module = relay.build(mod_1, target="c", params=params)

print(graph_module.get_lib().imported_modules)

with open("generated_code.c", "w") as f:
    for i in range(len(graph_module.get_lib().imported_modules)):
        f.write(graph_module.get_lib().imported_modules[i].get_source())
    # f.write(graph_module.get_lib().get_source())

save_lib(graph_module.get_lib(),graph_module.get_graph_json(),graph_module.get_params())

lib, graph, param = load_lib()
numpydata = np.random.randn(1,3,224,224).astype("float32")
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
res = rt_mod.get_output(0, tvm.nd.empty(output_shape))