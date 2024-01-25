
import logging
from functools import reduce

import tvm.ir
from tvm import relay
from tvm.ir import Op
from tvm.relay import expr as _expr
from tvm.relay import transform
from tvm.relay.analysis import analysis as _analysis
from tvm.relay.expr import Call, GlobalVar, TupleGetItem, const
from tvm.relay.expr_functor import ExprMutator, ExprVisitor

from ... import _ffi_api
from ...dataflow_pattern import DFPatternCallback, is_constant, is_expr, is_op, rewrite, wildcard
from .register import register_pattern_table

logger = logging.getLogger("CCompiler")
supported_post_elts = ["nn.relu",None]

def _register_external_op_helper(op_name, supported=True):
    """The helper function to indicate that a given operator can be supported
    by DNNL.

    Parameters
    ----------
    op_name : Str
        The name of operator that will be registered.

    Returns
    -------
    f : callable
        A function that returns if the operator is supported by DNNL.
    """

    @tvm.ir.register_op_attr(op_name, "target.ccompiler",level=10)
    def _func_wrapper(expr):
        # args = expr.args
        # if any([x.checked_type.dtype == "int64" for x in args]):
        #     logger.info("CCompiler does not support int64.")
        #     return False
        return supported

    return _func_wrapper

def get_attrs(expr):
    """Get the attributes from an expression."""
    if isinstance(expr, Call):
        return expr.attrs
    if isinstance(expr, TupleGetItem):
        return get_attrs(expr.tuple_value)
    return {}


_register_external_op_helper("add")
_register_external_op_helper("multiply")
_register_external_op_helper("subtract")

# @tvm.ir.register_op_attr("nn.relu", "target.ccompiler", level=9)
# def relu(expr):
#     return 1

# @tvm.ir.register_op_attr("nn.conv2d", "target.ccompiler", level=9)
# def conv2d(expr):
#     return 1

# @tvm.ir.register_op_attr("nn.bias_add", "target.ccompiler", level=9)
# def bias_add(expr):
#     return 1
