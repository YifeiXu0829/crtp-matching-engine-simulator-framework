#include <yaml-cpp/yaml.h>

#include "streamer.h"
#include "books.h"

// place to preserve each streamer object so they will live with io_context
struct streamers_cache
{
    std::unique_ptr<streamer<regular_lv2_book<lv_2_order, regular_lv2_book_policy>>> E_AAPL;
    std::unique_ptr<streamer<regular_lv3_book<lv_3_order, regular_lv3_book_policy>>> EO_AAPL_2022_06_17_16050;
    std::unique_ptr<streamer<user_defined_book_001<user_defined_order, user_defined_matching_policy>>> F_VX;

    // new instrument should add a smart pointer here
};

template <template<typename >typename streamer_ty, template<typename, typename> typename book_ty, typename order_ty, typename policy_ty, typename ... Args>
auto assembly_helper(Args&&... args)
{
    return std::make_unique<
                    streamer_ty<
                        book_ty<order_ty, policy_ty>
                                >
                        >(std::forward<Args>(args)...);
}

void assemble_components(boost::asio::io_context& io_context, const auto& instrument_root, auto& cache)
{
    auto symbol = instrument_root.first.template as<std::string>();
    short port_v{0};
    int book_depth{0};
    if (auto port_key = instrument_root.second["port"])
    {
        port_v = instrument_root.second["port"].template as<short>();
    }
    else
    {
        throw std::runtime_error("listening port is not defined, quitting");
    }

    if (auto book_depth_key = instrument_root.second["book_depth"])
    {
        book_depth = instrument_root.second["book_depth"].template as<int>();
    }

    if (symbol == "E_AAPL")
    {
        cache.E_AAPL = assembly_helper<streamer, regular_lv2_book, lv_2_order, regular_lv2_book_policy>(io_context, port_v, book_depth);
    }
    else if (symbol == "EO_AAPL_2022_06_17_16050")
    {
        cache.EO_AAPL_2022_06_17_16050 = assembly_helper<streamer, regular_lv3_book, lv_3_order, regular_lv3_book_policy>(io_context, port_v, book_depth);
    }
    else if (symbol == "F_VX")
    {
        cache.F_VX = assembly_helper<streamer, user_defined_book_001, user_defined_order, user_defined_matching_policy>(io_context, port_v, book_depth);
    }

    // new instrument will tell compiler what component to be assembled here
}

int main()
{
    try
    {
        boost::asio::io_context io_context;
        streamers_cache cache;

        YAML::Node root = YAML::LoadFile("supported_instruments.yaml");
        if (auto instruments = root["instruments_list"])
        {
            for (const auto& instrument : instruments)
            {
                assemble_components(io_context, instrument, cache);
            }
        }

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
