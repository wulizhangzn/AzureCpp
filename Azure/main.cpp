#include <iostream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "azure.h"

using namespace std;

string readFileIntoString(char * filename);//读取文件数据

int main()
{
    std::ofstream in;
    in.open("test.wav",std::ios::trunc);
    for(int i=0;i<3200;i++){
        in<<'\0';
    }
    in.close();//创建一个空文件，发送这个文件表示一句话的结束
    Azure endpoint;
    char * key = (char *)"自己的key";
    std::string uri = "wss://dev.microsofttranslator.com/speech/translate?api-version=1.0&from=en&to=zh";//请求路径
    std::cout<<"connecting......"<<"\n";
    endpoint.start(uri,key);//创建连接
    sleep(2);//等待，在连接建立后发送数据
    std::string filemessage = readFileIntoString((char *)"helloworld.wav");
    websocketpp::connection_hdl hdl = endpoint.con->get_handle();
    endpoint.m_endpoint.send(hdl,filemessage,websocketpp::frame::opcode::BINARY);
    char * nullfile=(char *)"test.wav";
    std::string nullstr;
    nullstr=readFileIntoString(nullfile);
    for(int s=0;s<50;s++){
        endpoint.m_endpoint.send(hdl,nullstr,websocketpp::frame::opcode::BINARY);
    }
    return 0;
}

string readFileIntoString(char * filename)
{

    ifstream ifile(filename);
    ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
        buf.put(ch);
    return buf.str();
}
