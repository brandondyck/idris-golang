
#include <cassert>
#include "vm.h"

namespace idris {

//---------------------------------------------------------------------------------------

void slide(shared_ptr<VirtualMachine>& vm,
           const size_t num_args) {
  for (auto i=0; i < num_args; i++) {
    vm->valstack[vm->valstack_base + i] = vm->valstack[vm->valstack_top + i];
  }
}

void project(shared_ptr<VirtualMachine>& vm,
             const Value& value, const IndexType loc, const int arity) {
  assert(value and value->getTypeId() == 'C');
  auto & args = unbox<Con>(value).args;
  for (auto i=0; i < arity; i++) {
    Value& arg = const_cast<Value&>(args[i]); // TODO: revisit this
    vm->valstack[vm->valstack_base + i + loc] = arg;
  }
}

static void prune_valstack(shared_ptr<VirtualMachine>& vm) {
  const auto top = vm->valstack_top + 1;
  while (vm->valstack.size() > top) {
    vm->valstack.pop_back();
  }
}

void vm_call(shared_ptr<VirtualMachine>& vm,
             const Func& fn, const IndexType arg) {
  fn(vm, arg);
  while (vm->callstack.size() > 0) {
    auto func = get<0>(vm->callstack.top());
    auto arg  = get<1>(vm->callstack.top());
    vm->callstack.pop();
    func(vm, arg);
  };
  prune_valstack(vm);
}

void vm_tailcall(shared_ptr<VirtualMachine>& vm,
                 const Func& fn, const IndexType arg) {
   vm->callstack.push({fn,arg});
}

} // namespace idris
