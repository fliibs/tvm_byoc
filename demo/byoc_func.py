import sys
from collections import OrderedDict
import numpy as np
import pytest

import tvm
import tvm.testing
from tvm import relay, runtime
from tvm.relay.build_module import bind_params_by_name
from tvm.relay.op.annotation import compiler_begin, compiler_end
from utils import *


def test_conv_in_ext_codegen():
    # After binding params and partitioning graph_module.get_params()
    # might contain parameters that are not an graph executor input but
    # for example constants in external function.
    y_in = np.ones((1,10)).astype("float32")
    params = {}
    mod = tvm.IRModule()
    x = relay.var("x", shape=(1, 10))
    y = relay.var("y", shape=(1, 10))
    z = relay.add(x,y)
    mod["main"] = relay.Function([x, y], z)
    
    # seq = tvm.transform.Sequential(
    #     [
    #         relay.transform.InferType(),
    #         # relay.transform.MergeComposite(),
    #         relay.transform.AnnotateTarget("dnnl"),
    #         relay.transform.PartitionGraph(),
    #         relay.transform.InferType(),
    #     ]
    # )

    # mod = seq(mod)


    graph_module = relay.build(mod, target="llvm", params=params)
    # print(graph_module.get_lib().imported_modules[1].get_source())
    with open("generated_code.c", "w") as f:
        for i in range(len(graph_module.get_lib().imported_modules)):
            f.write(graph_module.get_lib().imported_modules[i].get_source())

    save_lib(graph_module.get_lib(),graph_module.get_graph_json(),graph_module.get_params())

    # lib, graph, param = load_lib()
    # numpydata = np.random.randn(1,10).astype("float32")
    # map_inputs = {}
    # map_inputs["x"] = numpydata


    # from tvm import cpu
    # from tvm.contrib import graph_executor
    # rt_mod = graph_executor.create(graph, lib, cpu())
    # for name, data in map_inputs.items():
    #     rt_mod.set_input(name, data)
    # rt_mod.set_input(**param)

    # rt_mod.run()

    # output_shape = (1,10)
    # res = rt_mod.get_output(0, tvm.nd.empty(output_shape))
    # print(res)


test_conv_in_ext_codegen()