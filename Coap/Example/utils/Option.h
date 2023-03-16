#pragma once

#include <coap3/coap.h>
#include <vector>
#include <stdexcept>

namespace Coap {

class Option {
public:
    static constexpr size_t MAX_OPTIONS = 16;

    void add(coap_option_num_t num, const uint8_t* value, size_t length) {
        if (options_.size() >= MAX_OPTIONS) {
            throw std::runtime_error("Too many CoAP options");
        }
        options_.push_back({num, value, length});
    }

    coap_option* data() {
        return options_.data();
    }

    size_t size() const {
        return options_.size();
    }

    size_t max_size() const {
        return MAX_OPTIONS;
    }

    bool operator==(const Option& other) const {
        if (options_.size() != other.options_.size()) {
            return false;
        }
        for (size_t i = 0; i < options_.size(); ++i) {
            if (options_[i].num != other.options_[i].num ||
                options_[i].length != other.options_[i].length ||
                memcmp(options_[i].value, other.options_[i].value,
                       options_[i].length) != 0) {
                return false;
            }
        }
        return true;
    }

private:
    struct OptionPayload {
        coap_option_num_t num;
        const uint8_t* value;
        size_t length;
    };

    std::vector<OptionPayload> options_;
};

};// Coap