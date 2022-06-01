# crtp-based exchange matching-engine simulator framework

## Motivation
  Run-time polymorphism has a big overhead if a program uses dynamic dispatch over and over due to vtable lookup and memory cache misses. For a program that needs performance, we'd better design a framework that will avoid dynamic dispatch while keep code abstraction and reusability.

## Description
  This project uses modern c++ meta-programming techniques that the makes sure dispatch of the method calls will take place a compile time to overcome the performance bottleneck of dynamic dispatching. In general, the program will, at runtime, read a list of instruments configuration from file, listening order actions of different instruments from clients, and then maintaining an up-to-date order book for each instrument. (more details following later sections)

## Libraries
  This project included a list of third-party liraries but not limited to:  
    1. [yaml-cpp](https://github.com/jbeder/yaml-cpp) </br>
    2. [boost asio 1.76.0](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html)
    
## Components and Usage
  - **Yaml file configuration**
    upon startup, the program will read local file named *supported_instruments.yaml* with the following structure
    ```
    instruments_list:
       <instrument_key1>:
           <property1>: <value1>
           <property2>: <value2>
           ...
       <instrument_key2>:
           <property1>: <value1>
           <property2>: <value2>
           ...
       ...
    ```
    an example would be
    ```
    instruments_list:
      E_AAPL:
        port: 6001
        trading_symbol: "0x00000001"
        type: Equity
        book_depth: 0 
      EO_AAPL_2022_06_17_16050:
        port: 6002
        trading_symbol: "0x00000002"
        type: Equity_Option
        strike: 160.50
        expiration: 20220617
        book_depth: 0
      F_VX:
        port: 6003
        trading_symbol: "0x00000003"
        type: Future
        expiration: 20220624
        book_depth: 10
    ```
    Note that, *<instrument_key>* has to be pre-set in the *main.cpp* so that the compiler will know what components will be assembled at compile time for this instrument. more details will follow at the *Create a new instrument pipeline* section.
  - **Streamer**  
    Each streamer component is created by each instrument supported in the yaml configuration file, and their main job is to accept client connections, and then passing order messages from client to the corresponding instrument order book.  
    *all streamers IO are synchronized by boost asio library to ensure robustness, portability and performance.*
  - **Order**  
    Order component defines what properties an order should have. Depend on different expectations for different instruments, one can implement their own order type using provided crtp method to handle expected order string from client.
  - **Order Book**  
    An order book of an instrument describes what its book should look like. Two exsiting interfaces from a generic order book are level_2_book and level_3_book. For explanation, a level_2_book only contains aggregated volume per price level, represented as
    ```
    std::unordered_map<order_side, std::map<price_ty, quantity_ty>> book_;
    ```
    where as a level_3_book will provide much deeper information, namely on each price level, level_3 book provides a list of orders ordered by priority, represented as
    ```
    std::unordered_map<order_side, std::map<price_ty, std::vector<Order_Ty>>> book_;
    ```
    Users should pick the right book (or customize a new book) for a newly added instrument.
  - **Matching Policy**  
    A matching policy describe what expected changes to a order book should be after received an order action (new_order, replace_order, cancel_order) </br>
    different instruments could have differnt policies when receiving a same action.
## Create a new instrument pipeline
When adding a new instrument, there are several steps to be taken as follows, </br>
  1. add the instrument block in yaml file with necessary properties, for example
  ```
    E_TSLA:
        port: 6004
        trading_symbol: "0x00000004"
        type: Equity
        book_depth: 0
  ```
  **Note: *port* value has to be unique(also in the correct range) for every instrument** </br>
  2. pick the desired orderbook, order type and matching policy type and then assemble them from `assemble_components(...)`in main.cpp </br>
  ```
  ...
  ...
  else if (symbol == "E_TSLA")
  {
      cache.E_TSLA = assembly_helper<streamer, regular_lv2_book, lv_2_order, regular_lv2_book_policy>(io_context, port_v, book_depth);
  }
  ```
  **Note: the components picked has to be logical reasonable, say, you can't use lv3_book_policy on a lv2_book, it won't compile.** </br>
  *(what if one or more components provided does not satifies you needs ? next section will answer it :) )* </br>
  3. add a smart pointer for the corresponding assembled pipeline into the cache struct
  (we need to ensure the stream pipeline live before asio loop dies. )
  ```
  struct streamers_cache
  {
    ...
    ...
    std::unique_ptr<streamer<regular_lv2_book<lv_2_order, regular_lv2_book_policy>>> E_TSLA;
  }
  ```
## User-defined Component Example (with minimum implementation)
  If any of the existing components can't satisfy a user's need, one can define their own type (book, order or matching_policy)
  say, for a customized order type to handle order string from clients due to different messaging policy
  (messaging policy actually can be used as crtp as well .. )
  ```
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
  ```
  say, for a customized matching_policy_type to handle actions that adjust the book differently
  ```
  struct user_defined_lv3_matching_policy : lv3_book_policy<user_defined_lv3_matching_policy>
  {
    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    void add_order(Order_Ty order, Book_Ty& book)
    {
        /* Implementation to add order in to the book differently*/
    }

    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    bool remove_order(Order_Ty& order, Book_Ty& book)
    {
        /* Implementation to remove order our of the book differently*/
        return false;
    }

    /* shadow */
    template <typename Order_Ty, typename Book_Ty>
    bool match(Order_Ty order, Book_Ty& book)
    {
        /* Implementation regarding fills mechanism when matching */
        cout << "calling user_defined_lv3_matching_policy::match(...)" <<endl;
        return true;
    }
  };
  ```
  or totally create a new policy that is neither a lv_2_book_policy or lv_3_book_policy
  ```
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
  ```
  finally, a new book type can be customized as well
  ```
  template <typename order_ty, typename policy_ty>
  class user_defined_book_001 : public book_base<user_defined_book_001<order_ty, policy_ty>, order_ty, policy_ty>
  {
    using base = book_base<user_defined_book_001<order_ty, policy_ty>, order_ty, policy_ty>;
    public:
      explicit user_defined_book_001(int max_depth):base(max_depth)
      {
      }

    private:
    // new containers that represent the book ?
    // methods that cooperate with policy_ty ?
  };
  ```
  Of course, the user-defined book has to work/compile with existing/newly-created order-type and policy_type.
## Build and Run
  this project uses Cmake (requires minimum version of 3.5.0) to build, and a build script is provided.
  from project root directory, run </br>
  `./build_script` </br>
  and exeutalbe will be built at </br>
  `./build/main`
  
