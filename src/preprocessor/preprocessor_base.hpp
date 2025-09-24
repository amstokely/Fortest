#ifndef FORTEST_PREPROCESSOR_BASE_HPP
#define FORTEST_PREPROCESSOR_BASE_HPP
#include <string>

namespace Fortest {
    /**
   * @brief CRTP base class for preprocessors.
   *
   * Provides a common entry point and ensures the derived
   * class implements the required `transform` method.
   */
    template <typename Derived>
    class PreprocessorBase {
    public:
        std::string process(const std::string &input) {
            return derived().transform(input);
        }

    private:
        Derived &derived() { return static_cast<Derived&>(*this); }
    };
}

#endif //FORTEST_PREPROCESSOR_BASE_HPP