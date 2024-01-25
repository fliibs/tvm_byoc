#ifndef MATCH_OP_H_
#define MATCH_OP_H_

#include <tvm/relay/function.h>

#include <string>
#include <unordered_map>
#include <vector>

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
dmlc::any dmlc_attr(const T& val) {
  std::vector<dmlc::any> attr;
  attr.emplace_back(std::vector<std::string>{std::to_string(val)});
  return dmlc::any{attr};
}

template <typename T, std::enable_if_t<std::is_same<T, std::string>::value, bool> = true>
dmlc::any dmlc_attr(const T& val) {
  std::vector<dmlc::any> attr;
  attr.emplace_back(std::vector<std::string>{val});
  return dmlc::any{attr};
}

template <typename T,
          std::enable_if_t<std::is_same<T, std::vector<std::string>>::value, bool> = true>
dmlc::any dmlc_attr(const T& val) {
  std::vector<dmlc::any> attr;
  attr.emplace_back(val);
  return dmlc::any{attr};
}

class ArgPacker {
 public:
  ArgPacker(std::unordered_map<std::string, dmlc::any>* attrs, std::vector<tvm::relay::Expr>* args)
      : attrs_(attrs), args_(args) {}

  int Put(const tvm::relay::Expr& arg, std::string tag_name = "") {
    if (!arg.defined()) return -1;
    int idx = args_->size();
    args_->push_back(arg);
    if (!tag_name.empty()) {
      attrs_->operator[](tag_name) = dmlc_attr(idx);
    }
    return idx;
  }

 private:
  std::unordered_map<std::string, dmlc::any>* attrs_;
  std::vector<tvm::relay::Expr>* args_;
};






#endif