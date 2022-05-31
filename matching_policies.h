#pragma once
#include<iostream>

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

protected:
    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_new(Order_Ty order, Book_Ty& book)
    {   
        cout << "calling matching_policy_base::handle_order_new(...)" <<endl;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_replace(Order_Ty order, Book_Ty& book)
    {
        cout << "calling matching_policy_base::handle_order_replace(...)" <<endl;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_cancel(Order_Ty order, Book_Ty& book)
    {
        cout << "calling matching_policy_base::handle_order_cancel(...)" <<endl;
    }

    auto& impl() { return *static_cast<Impl*>(this); }
};

struct lv2_book_policy : public matching_policy_base<lv2_book_policy>
{
    // use same functionality as matching_policy_base
};

struct lv3_book_policy : matching_policy_base<lv3_book_policy>
{
    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_new(Order_Ty order, Book_Ty& book)
    {
        cout << "calling lv3_book_policy::handle_order_new(...)" <<endl;
    }
};

// once can customize their own matching policy here

struct user_defined_matching_policy : matching_policy_base<user_defined_matching_policy>
{
    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_new(Order_Ty order, Book_Ty& book)
    {
        cout << "calling user_defined_matching_policy::handle_order_new(...)" <<endl;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_replace(Order_Ty order, Book_Ty& book)
    {
        cout << "calling user_defined_matching_policy::handle_order_replace(...)" <<endl;
    }

    /* may shadow */
    template <typename Order_Ty, typename Book_Ty>
    void handle_order_cancel(Order_Ty order, Book_Ty& book)
    {
        cout << "calling user_defined_matching_policy::handle_order_cancel(...)" <<endl;
    }
};
