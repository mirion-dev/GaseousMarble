export module gm.utils;

import std;

namespace gm {

    export template <class T, class Pr>
    class InvokeChain {
        T _value{};
        Pr _pred;

    public:
        template <class... Args>
        InvokeChain& and_then(Args&&... args) {
            if (_pred(_value)) {
                _value = std::invoke(std::forward<Args>(args)...);
            }
            return *this;
        }

        operator T() const noexcept {
            return _value;
        }
    };

}
