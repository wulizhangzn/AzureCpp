#ifndef AZURE_H
#define AZURE_H
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

using namespace std;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef client::connection_ptr connection_ptr;
class Azure {
public:
    typedef Azure type;
    typedef std::chrono::duration<int,std::micro> dur_type;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> mythread;
    Azure () {
        m_endpoint.set_access_channels(websocketpp::log::alevel::none);
        m_endpoint.set_error_channels(websocketpp::log::elevel::none);
        m_endpoint.init_asio();
        m_endpoint.start_perpetual();
        mythread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_endpoint);
        m_endpoint.set_tls_init_handler(bind(&type::on_tls_init,this,::_1));
        m_endpoint.set_message_handler(bind(&type::on_message,this,::_1,::_2));
        m_endpoint.set_close_handler(bind(&type::on_close,this,::_1));
    }

    ~Azure() {
        m_endpoint.stop_perpetual();
        mythread->join();
    }

    void start(std::string uri,char * key) {
        websocketpp::lib::error_code ec;
        con = m_endpoint.get_connection(uri, ec);
        con->append_header("Ocp-Apim-Subscription-Key",key);
        m_endpoint.connect(con);
    }

    context_ptr on_tls_init(websocketpp::connection_hdl hdl) {
        context_ptr ctx(new boost::asio::ssl::context(boost::asio::ssl::context::sslv23));
        try {
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::single_dh_use);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        return ctx;
    }

    void on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
        std::cout<<msg->get_payload()<<"\n";
        m_endpoint.close(hdl,websocketpp::close::status::going_away,"");
    }

    void on_close(websocketpp::connection_hdl hdl) {
        std::cout<<"connect close"<<"\n";
    }
    client m_endpoint;
    client::connection_ptr con;
};
#endif // AZURE_H
