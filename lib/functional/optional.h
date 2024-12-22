#pragma once
template <typename T>
class Option {
private:
    bool hasValue;
    T value;

public:
    static Option<T> empty() {
        return Option<T>();
    }
    Option() : hasValue(false) {}
    Option(const T& val) : hasValue(true), value(val) {}
    Option(T&& val) : hasValue(true), value(std::move(val)) {}
    bool isPresent() const { return hasValue; }
    const T& getValue() const {
        if (!hasValue) {
            throw std::runtime_error("No value present");
        }
        return value;
    }
    T getValueOr(const T& defaultValue) const {
        return hasValue ? value : defaultValue;
    }
};
