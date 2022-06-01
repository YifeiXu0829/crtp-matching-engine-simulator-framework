#pragma once
#include <iostream>

enum class order_action : uint8_t
{
    add = 0,
    modify,
    cancel,
};

enum class order_side : uint8_t
{
    buy = 0,
    sell
};

template <typename Impl>
class order_base : public std::enable_shared_from_this<Impl>
{
public:
    using price_ty = double;
    using quantity_ty = uint32_t;
    using id_ty = uint64_t;

    explicit order_base(std::string order_string)
    {
        // process order_string to assign the order properties here ..
        impl().make_order(std::move(order_string));
    }

    /* may shadow */
    void make_order(std::string order_string)
    {
        // base order expects order_string to look like:
        // id=0<space>action=0<space>side=0<space>quantity=12<space>price=175.25<space><...>

        /* dummy implmentation */
        action = order_action::add;
        side = order_side::sell;
        qty = static_cast<quantity_ty>(12);
        price = static_cast<price_ty>(175.25);
        id = static_cast<id_ty>(0);
    }

    order_action action;
    order_side side;
    quantity_ty qty;
    price_ty price;
    id_ty id;

    auto& impl() { return *static_cast<Impl*>(this); }
};

struct lv_2_order : order_base<lv_2_order>
{
    explicit lv_2_order(std::string s) : order_base(std::move(s))
    {
    }
};

struct lv_3_order : order_base<lv_3_order>
{
    using priority_ty = uint64_t;
    explicit lv_3_order(std::string s) : order_base(std::move(s))
    {
    }

    priority_ty order_priority;
};

struct user_defined_order : order_base<user_defined_order>
{
    // type can be re-defined
    using quantity_ty = double; // support fractional quantity
    using order_base<user_defined_order>::price_ty;
    explicit user_defined_order(std::string s) : order_base(std::move(s)){}

    /* shadow */
    void make_order(std::string order_string)
    {
        // one can customize the way to handle order_string depending on its messaging policy with clients
    }

    // more fields can be added
    price_ty stop_limit_price;
};
