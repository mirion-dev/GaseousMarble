export module gm.utils;

import std;

namespace gm {

    export template <class T, class Pr>
    class InvokeChain {
        T _value{};
        Pr _pred;

    public:
        template <class Fn>
        InvokeChain& and_then(Fn&& func) {
            if (_pred(_value)) {
                _value = func();
            }
            return *this;
        }

        operator T() const noexcept {
            return _value;
        }
    };

}
