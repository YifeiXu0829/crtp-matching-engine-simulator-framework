#pragma once
#include <iostream>

enum class order_action : uint8_t
{
    add,
    modify,
    cancel,
};

enum class order_side : uint8_t
{
    buy,
    sell
};

template <typename Impl>
struct order_base : std::enable_shared_from_this<Impl>
{
    using price_ty = double;
    using quantity_ty = uint32_t;
    using id_ty = uint64_t;

    explicit order_base(std::string order_string)
    {
        // process order_string to assign the order properties here ..
    }

    order_action action{order_action::add};
    order_side side;
    quantity_ty qty;
    price_ty price;
    id_ty id;
};

struct lv_2_order : order_base<lv_2_order>
{
    explicit lv_2_order(std::string s) : order_base(s)
    {
    }
};

struct lv_3_order : order_base<lv_3_order>
{
    using priority_ty = uint64_t;
    explicit lv_3_order(std::string s) : order_base(s)
    {
    }

    priority_ty order_priority;
};

struct user_defined_order : order_base<user_defined_order>
{
    using quantity_ty = double; // support fractional quantity
    explicit user_defined_order(std::string s) : order_base(s)
    {
    }
};
