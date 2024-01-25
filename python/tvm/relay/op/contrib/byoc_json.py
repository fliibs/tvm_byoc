import logging
from functools import reduce

import tvm.ir
# from tvm import relay
# from tvm.ir import Op
# from tvm.relay import expr as _expr
# from tvm.relay import transform
# from tvm.relay.analysis import analysis as _analysis
from tvm.relay.expr import Call, GlobalVar, TupleGetItem, const
# from tvm.relay.expr_functor import ExprMutator, ExprVisitor

# from ... import _ffi_api
# from ...dataflow_pattern import DFPatternCallback, is_constant, is_expr, is_op, rewrite, wildcard
# from .register import register_pattern_table

logger = logging.getLogger("BYOC_JSON")
supported_post_elts = [None]


def _register_external_op_helper(op_name, supported=True):
    """The helper function to indicate that a given operator can be supported
    by BYOC_JSON.

    Parameters
    ----------
    op_name : Str
        The name of operator that will be registered.

    Returns
    -------
    f : callable
        A function that returns if the operator is supported by DNNL.
    """

    @tvm.ir.register_op_attr(op_name, "target.byoc_json")
    def _func_wrapper(expr):
        args = expr.args
        if any([x.checked_type.dtype == "int64" for x in args]):
            logger.info("BYOC_JSON does not support int64.")
            return False
        if "pool" in op_name:
            attrs = dict(get_attrs(expr))
            if "ceil_mode" in attrs.keys() and attrs["ceil_mode"]:
                return False
        return supported

    return _func_wrapper

def get_attrs(expr):
    """Get the attributes from an expression."""
    if isinstance(expr, Call):
        return expr.attrs
    if isinstance(expr, TupleGetItem):
        return get_attrs(expr.tuple_value)
    return {}

# _register_external_op_helper("nn.batch_norm")
# _register_external_op_helper("nn.conv1d")
_register_external_op_helper("nn.conv2d")
# _register_external_op_helper("nn.conv3d")
# _register_external_op_helper("nn.conv2d_transpose")
# _register_external_op_helper("nn.conv3d_transpose")
# _register_external_op_helper("nn.dense")
# _register_external_op_helper("nn.max_pool2d")
# _register_external_op_helper("nn.avg_pool2d")
# _register_external_op_helper("nn.global_avg_pool2d")
# _register_external_op_helper("nn.max_pool3d")
# _register_external_op_helper("nn.avg_pool3d")
# _register_external_op_helper("abs")
# _register_external_op_helper("clip")
# _register_external_op_helper("exp")
# _register_external_op_helper("log")
# _register_external_op_helper("sqrt")
# _register_external_op_helper("round")
# _register_external_op_helper("nn.relu")
# _register_external_op_helper("nn.leaky_relu")
# _register_external_op_helper("tanh")
# _register_external_op_helper("sigmoid")
# _register_external_op_helper("nn.softmax")
# _register_external_op_helper("add")
# _register_external_op_helper("multiply")
# _register_external_op_helper("nn.layer_norm")
# _register_external_op_helper("nn.batch_matmul")