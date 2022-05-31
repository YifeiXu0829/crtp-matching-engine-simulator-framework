#pragma once
#include<iostream>
#include<unordered_map>
using std::cout;
using std::endl;
template<typename Impl>
class matching_policy_base : public std::enable_shared_from_this<Impl>
{
public:
    template <typename ... Args>
    void order_new_policy(Args&&... args)
    {
        impl().handle_order_new(std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void order_replace_policy(Args&&... args)
    {   
        impl().handle_order_replace(std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void order_cancel_policy(Args&&... args)
    {   
        impl().handle_order_cancel(std::forward<Args>(args)...);
    }

private:
    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_new(Order_Ty order, Book_Ty& book)
    {   
        cout << "calling matching_policy_base::handle_order_new(...)" <<endl;

        if (!impl().match(order, book))
        {
            impl().add_order(std::move(order), book);
        }
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_replace(Order_Ty order, Book_Ty& book)
    {
        cout << "calling matching_policy_base::handle_order_replace(...)" <<endl;

        impl().remove_order(order, book);
        impl().handle_order_new(std::move(order), book);
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_cancel(Order_Ty order, Book_Ty& book)
    {
        cout << "calling matching_policy_base::handle_order_cancel(...)" <<endl;

        impl().remove_order(order, book);
    }

protected:
    auto& impl() { return *static_cast<Impl*>(this); }
};

template <typename Impl>
struct lv2_book_policy : public matching_policy_base<Impl>
{
    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void add_order(Order_Ty order, Book_Ty& book)
    {
        auto& side_book = book.get_side_book_access(order.side);
        side_book[order.price] += order.qty;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void remove_order(Order_Ty& order, Book_Ty& book)
    {
        auto& side_book = book.get_side_book_access(order.side);
        side_book[order.price] -= order.qty;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    bool match(Order_Ty order, Book_Ty& book)
    {
        /* dummy implementation */
        cout << "calling lv3_book_policy::match(...)" <<endl;
        return true;
    }
};

template <typename Impl>
struct lv3_book_policy : matching_policy_base<Impl>
{
    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_new(Order_Ty order, Book_Ty& book)
    {
        cout << "calling lv3_book_policy::handle_order_new(...)" <<endl;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void add_order(Order_Ty order, Book_Ty& book)
    {
        auto& side_book = get_side_book_access(order.side);
        side_book[order.price].push_back(std::move(order));
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    bool remove_order(Order_Ty& order, Book_Ty& book)
    {
        auto& side_book = book.get_side_book_access(order.side);
        auto& orders_on_price = side_book[order.price];
        for (auto it = orders_on_price.begin(); it!=orders_on_price.end(); ++it)
        {
            if (it->id == order.id)
            {
                orders_on_price.erase(it);
                return true;
            }
        }

        return false;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    bool match(Order_Ty order, Book_Ty& book)
    {
        /* dummy implementation */
        cout << "calling lv3_book_policy::match(...)" <<endl;
        return true;
    }
};

struct regular_lv2_book_policy : lv2_book_policy<regular_lv2_book_policy>
{
    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    bool match(Order_Ty order, Book_Ty& book)
    {
        cout << "calling regular_lv2_book_policy::match(...)" <<endl;
        return false;
    }
};

struct regular_lv3_book_policy : lv3_book_policy<regular_lv3_book_policy>
{
    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    bool match(Order_Ty order, Book_Ty& book)
    {
        cout << "calling regular_lv3_book_policy::match(...)" <<endl;
        return false;
    }
};


// one can customize their own matching policy here

struct user_defined_matching_policy : matching_policy_base<user_defined_matching_policy>
{
    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_new(Order_Ty order, Book_Ty& book)
    {
        cout << "calling user_defined_matching_policy::handle_order_new(...)" <<endl;
    }

    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_replace(Order_Ty order, Book_Ty& book)
    {
        cout << "calling user_defined_matching_policy::handle_order_replace(...)" <<endl;
    }

    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_cancel(Order_Ty order, Book_Ty& book)
    {
        cout << "calling user_defined_matching_policy::handle_order_cancel(...)" <<endl;
    }
};
