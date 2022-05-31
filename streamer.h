#pragma once

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;
using std::cout;
using std::endl;

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }
 
  void read(auto callback)
  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self, callback](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            callback(std::string(data_, length));
            read(callback);
          }
        });
  }
private:
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

// streamer is used to listen order actions from clients
template <typename book_ty>
class streamer
{
public:
    template <typename... Args>
    streamer(boost::asio::io_context& io_context, short port, Args&&... args)
    : acceptor_{io_context, tcp::endpoint(tcp::v4(), port)},port_{port}, book_{std::forward<Args>(args)...}
    {
        std::cout<<"port:"<<port<<" starts accepting connections"<<std::endl;
        start_accept();
    }

private:
    void start_accept()
  	{
    	acceptor_.async_accept(
        	[this](boost::system::error_code ec, tcp::socket socket)
			{
			  if (!ec)
			  {
                std::cout<<"incoming connection"<<std::endl;
                
				std::make_shared<session>(std::move(socket))->read([this](std::string s)
																	  {
																		 std::cout<<"streamer"<<this->port_<<" received:"<<s;
																		 this->book_.handle_incoming_order(std::move(s));
																	  }
																	 );
                
			  }

			  start_accept();
			});
  	}

    book_ty book_;
    tcp::acceptor acceptor_;
	int port_{0};
};
