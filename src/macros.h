
#ifndef _SRC_MACROS_H_
#define _SRC_MACROS_H_

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName &) = delete;  \
    ClassName &operator=(const ClassName &) = delete;

#define DEFINE_PTR_TYPES(ClassName)                               \
    typedef std::shared_ptr<ClassName> ClassName##Ptr;            \
    typedef std::weak_ptr<ClassName> ClassName##WeakPtr;          \
    typedef std::shared_ptr<const ClassName> ClassName##PtrConst; \
    typedef std::weak_ptr<const ClassName> ClassName##WeakPtrConst;

// to get a shared ptr from a weak ptr
#define GET_SHARED_PTR(VarName, WeakPtr) \
    assert(!WeakPtr.expired());          \
    auto VarName = WeakPtr.lock();

#endif  // _SRC_MACROS_H_