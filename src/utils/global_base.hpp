#ifndef GLOBAL_BASE_HPP
#define GLOBAL_BASE_HPP
#include <memory>

namespace Fortest {

    /// @brief CRTP-style base class for implementing singletons.
    ///
    /// Provides a static `instance()` accessor that creates and
    /// stores a single shared instance of the derived class.
    /// Intended to be inherited by classes that require a global
    /// singleton instance.
    ///
    /// @tparam Derived The class type deriving from SingletonBase.
    template<typename Derived>
    class SingletonBase {
    public:
        /// @brief Accessor for the singleton instance.
        ///
        /// The instance is lazily constructed on first call
        /// and returned as a shared pointer.
        ///
        /// @return Reference to the singleton shared pointer.
        static std::shared_ptr<Derived> &instance() {
            static std::shared_ptr<Derived> instance{new Derived()};
            return instance;
        }

        /// @brief Deleted copy constructor.
        SingletonBase(const SingletonBase &) = delete;

        /// @brief Deleted copy assignment operator.
        SingletonBase &operator=(const SingletonBase &) = delete;

    protected:
        /// @brief Protected default constructor.
        SingletonBase() = default;

        /// @brief Protected virtual destructor.
        virtual ~SingletonBase() = default;
    };
}

#endif //GLOBAL_BASE_HPP
