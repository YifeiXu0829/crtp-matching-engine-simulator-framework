#pragma once
#include "matching_policies.h"
#include "orders.h"

template <typename Impl, typename Order_Ty, typename Matching_Policy>
class book_base : public std::enable_shared_from_this<Impl>
{
public:
    using price_ty = typename Order_Ty::price_ty;
    using quantity_ty = typename Order_Ty::quantity_ty;
    explicit book_base(int max_depth):max_depth_(max_depth)
    {
    }

    void handle_incoming_order(std::string order_str)
    {
        auto order = Order_Ty(order_str);

        switch(order.action)
        {
        case order_action::add:
            cout<<"handle_order_new_policy"<<endl;
            policy_.order_new_policy(std::move(order), impl());
            break;
        case order_action::modify:
            policy_.order_replace_policy(std::move(order), impl());
            break;
        case order_action::cancel:
            policy_.order_cancel_policy(std::move(order), impl());
            break;
        default:
            cout<<"unregonized action "<< endl;
        }
    }

protected:
    auto& impl() { return *static_cast<Impl*>(this); }
    Matching_Policy policy_;

private:
    int max_depth_{0}; // 0 means unlimited depth
};

// level 2 order book is price level volume aggregated order book.
template <typename Impl, typename Order_Ty, typename Matching_Policy>
class level_2_book : public book_base<Impl, Order_Ty, Matching_Policy>
{
using base = book_base<Impl, Order_Ty, Matching_Policy>;
using typename base::price_ty;
using typename base::quantity_ty;
public:
    explicit level_2_book(int max_depth):base(max_depth)
    {
    }

    auto& get_buy_side_book_access(){return buy_side_book_;}
    auto& get_sell_side_book_access(){return sell_side_book_;}

private:
    std::map<price_ty, quantity_ty> buy_side_book_;
    std::map<price_ty, quantity_ty> sell_side_book_;
};

// level 3 order book provides deeper information on each price level with each order's priority.
template <typename Impl, typename Order_Ty, typename Matching_Policy>
class level_3_book : public book_base<Impl, Order_Ty, Matching_Policy>
{
using base = book_base<Impl, Order_Ty, Matching_Policy>;
using typename base::price_ty;
using typename base::quantity_ty;
public:
    explicit level_3_book(int max_depth):base(max_depth)
    {
    }

    auto& get_buy_side_book_access(){return buy_side_book_;}
    auto& get_sell_side_book_access(){return sell_side_book_;}

private:
    std::map<price_ty, std::vector<Order_Ty*>> buy_side_book_;
    std::map<price_ty, std::vector<Order_Ty*>> sell_side_book_;
};

class regular_lv2_book : public level_2_book<regular_lv2_book, lv_2_order, lv2_book_policy>
{
using base = book_base<regular_lv2_book, lv_2_order, lv2_book_policy>;
using level_book_base = level_2_book<regular_lv2_book, lv_2_order, lv2_book_policy>;
public:
    explicit regular_lv2_book(int max_depth):level_book_base(max_depth)
    {
    }
};

class regular_lv3_book : public level_3_book<regular_lv3_book, lv_3_order, lv3_book_policy>
{
using base = book_base<regular_lv3_book, lv_3_order, lv3_book_policy>;
using level_book_base = level_3_book<regular_lv3_book, lv_3_order, lv3_book_policy>;
public:
    explicit regular_lv3_book(int max_depth):level_book_base(max_depth)
    {
    }
};

// one can customize book here

class user_defined_lv3_book_001 : public level_3_book<user_defined_lv3_book_001, user_defined_order, user_defined_matching_policy>
{
using base = book_base<user_defined_lv3_book_001, user_defined_order, user_defined_matching_policy>;
using level_book_base = level_3_book<user_defined_lv3_book_001, user_defined_order, user_defined_matching_policy>;

public:
    explicit user_defined_lv3_book_001(int max_depth):level_book_base(max_depth)
    {
    }
};
