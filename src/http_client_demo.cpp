/**
 *
 *
 * this is just a test using curlpp,and you can use any http framework as you like
 *
 *
 */
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <service.pb.h>
#include <sstream>

char *m_pBuffer = NULL;
size_t m_Size = 0;

void* Realloc(void* ptr, size_t size)
{
    if(ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
};
// Callback must be declared static, otherwise it won't link...
size_t WriteMemoryCallback(char* ptr, size_t size, size_t nmemb)
{
    // Calculate the real size of the incoming buffer
    // every buffer
    size_t realsize = size * nmemb;

    // (Re)Allocate memory for the buffer
    m_pBuffer = (char*) Realloc(m_pBuffer, m_Size + realsize);

    // Test if Buffer is initialized correctly & copy memory
    if (m_pBuffer == NULL) {
        realsize = 0;
    }

    //concat the all buffer
    memcpy(&(m_pBuffer[m_Size]), ptr, realsize);
    m_Size += realsize;

    std::cout << "Size: " << m_Size << std::endl;
    std::cout << "Content: "  << m_pBuffer << std::endl;

    FooResponse response;
    response.ParseFromString(m_pBuffer);
    std::cout << "foo response = "<<response.text();
    // return the real size of the buffer...
    return realsize;
};

int main(int, char **) {
    try {
        FooRequest fooRequest;
        fooRequest.set_code(1);
        //protobuf body need to be byte array
        std::string request_string;
        fooRequest.SerializeToString(&request_string);

        std::cout << " rrrrrrrrrr = "<< request_string.size() <<std::endl;
        for (auto &&item : request_string) {
            std::cout << item <<",";
        }
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        //construct header
        std::list<std::string> header_list;
        //attetion : there is no whitespace before : and at least a whitespace after :
        header_list.push_back("Accept: application/protobuf");

        curlpp::options::HttpHeader header(header_list);

        std::string total_string = "http://localhost:8421/FooService.Foo";
        total_string.append("?request=");
        total_string.append(request_string);
        printf("total string url = %s\n",total_string.c_str());
        //construct param
        curlpp::options::Url url(total_string);
//        curlpp::options::PostFields postFields(request_string);
//        curlpp::options::PostFieldSize postFieldSize(request_string.size());

        //callback function which are used to write the response of http post request
        curlpp::types::WriteFunctionFunctor functor(WriteMemoryCallback);
        curlpp::options::WriteFunction *test = new curlpp::options::WriteFunction(functor);

        //set request
        curlpp::Easy request;

        request.setOpt(header);
        request.setOpt(new curlpp::options::Post(false));
//        request.setOpt(postFields);
//        request.setOpt(postFieldSize);
        request.setOpt(url);
        request.setOpt(curlpp::options::Verbose(true));
        request.setOpt(test);

        //send a request,there will print the response of request
        request.perform();


        std::cout<<std::endl;

        std::string effURL;
        curlpp::infos::EffectiveUrl::get(request, effURL);
        std::cout << "Effective URL: " << effURL << std::endl;
        std::cout << "response code: "<< curlpp::infos::ResponseCode::get(request) << std::endl;
    }

    catch (curlpp::RuntimeError &e) {
        std::cout << e.what() << std::endl;
    }

    catch (curlpp::LogicError &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
