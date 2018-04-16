//
// Created by root on 18-4-14.
//
//
// Created by root on 18-4-11.
//
#include <string>
#include <unistd.h>
#include <sofa/pbrpc/pbrpc.h>
#include "service.pb.h"
void echo(std::string user){
    printf("hello + %s\n",user.c_str());
}
bool WebServlet(const sofa::pbrpc::HTTPRequest& request, sofa::pbrpc::HTTPResponse& response)
{
    SLOG(INFO, "WebServlet(): request message from %s:%u",
         request.client_ip.c_str(), request.client_port);
    SLOG(INFO, "HTTP-PATH=\"%s\"", request.path.c_str());
    std::map<std::string, std::string>::const_iterator it;
    const std::map<std::string, std::string>& query_params = *request.query_params;
    for (it = query_params.begin(); it != query_params.end(); ++it) {
        SLOG(INFO, "QueryParam[\"%s\"]=\"%s\"", it->first.c_str(), it->second.c_str());
    }
    const std::map<std::string, std::string>& headers = *request.headers;
    for (it = headers.begin(); it != headers.end(); ++it) {
        SLOG(INFO, "Header[\"%s\"]=\"%s\"", it->first.c_str(), it->second.c_str());
    }
    return response.content->Append("<h1>Hello from sofa-pbrpc web server</h1>");
}

class FooServiceImpl : public FooService{
public:
    FooServiceImpl() {}

    virtual ~FooServiceImpl() {

    }

    virtual void Foo(::google::protobuf::RpcController *controller, const ::FooRequest *request, ::FooResponse *response,
                     ::google::protobuf::Closure *done) override {
        //controller can return some basic message of the request process,such as reason
        sofa::pbrpc::RpcController* sofa_controller = static_cast<sofa::pbrpc::RpcController*>(controller);


        printf("request from %s , request code = %d\n",sofa_controller->RemoteAddress().c_str(), request->code());
        printf("http path = %s\n",sofa_controller->HttpPath().c_str());

        std::map<std::string, std::string>::const_iterator it;
        //if http is get , this will take effect
        const std::map<std::string, std::string>& query_params = sofa_controller->HttpQueryParams();
        for (it = query_params.begin(); it != query_params.end(); ++it) {
            SLOG(INFO, "QueryParam[\"%s\"]=\"%s\"", it->first.c_str(), it->second.c_str());
        }

        const std::map<std::string, std::string>& headers = sofa_controller->HttpHeaders();
        for (it = headers.begin(); it != headers.end(); ++it) {
            printf("Header[\"%s\"]=\"%s\"\\\n", it->first.c_str(), it->second.c_str());
        }

        if (request->code() == 0){
            response->set_text("I know you are code 0");
        } else if(request->code() == 1){
            response->set_text("I know you are code 1");
        } else {
            response->set_text("default text");
        }
        done->Run();
    }
};

class NoRequestServiceImpl : public NoRequestService{
public:
    NoRequestServiceImpl() {}

    virtual ~NoRequestServiceImpl() {

    }

    void NoRS(::google::protobuf::RpcController *controller, const ::Empty *request, ::NORequestResponse *response,
              ::google::protobuf::Closure *done) override {
        sofa::pbrpc::RpcController* sofa_controller = static_cast<sofa::pbrpc::RpcController*>(controller);

        printf("request from %s ",sofa_controller->RemoteAddress().c_str());
        printf("http path = %s\n",sofa_controller->HttpPath().c_str());

        std::map<std::string, std::string>::const_iterator it;
        //if http is get , this will take effect
        const std::map<std::string, std::string>& query_params = sofa_controller->HttpQueryParams();
        for (it = query_params.begin(); it != query_params.end(); ++it) {
            SLOG(INFO, "QueryParam[\"%s\"]=\"%s\"", it->first.c_str(), it->second.c_str());
        }

        const std::map<std::string, std::string>& headers = sofa_controller->HttpHeaders();
        for (it = headers.begin(); it != headers.end(); ++it) {
            printf("Header[\"%s\"]=\"%s\"\\\n", it->first.c_str(), it->second.c_str());
        }
        response->set_response("no request respone text");
        done->Run();
    }
};

bool thread_init_func()
{
    sleep(1);
    printf("Init work thread succeed\n");
    return true;
}

void thread_dest_func()
{
    printf("Destroy work thread succeed\n");
}

int main(){
    sofa::pbrpc::RpcServerOptions options;

    options.disable_builtin_services = true;
    options.no_delay = true;
    options.work_thread_init_func = sofa::pbrpc::NewPermanentExtClosure(&thread_init_func);
    options.work_thread_dest_func = sofa::pbrpc::NewPermanentExtClosure(&thread_dest_func);
    sofa::pbrpc::RpcServer rpc_server(options);
    std::string address = "0.0.0.0:8421";
    bool flag = rpc_server.Start(address);
    if(!flag)
        return -1;
    FooService* fooService = new FooServiceImpl();
    if(!rpc_server.RegisterService(fooService)){
        printf("register service failed\n");
    }

    NoRequestService* noRequestService = new NoRequestServiceImpl();
    if(!rpc_server.RegisterService(noRequestService)){
        printf("register norequest service failed\n");
    }

    sofa::pbrpc::Servlet servlet = sofa::pbrpc::NewPermanentExtClosure(&WebServlet);
    rpc_server.RegisterWebServlet("/hello",servlet);

    rpc_server.Run();
    delete options.work_thread_init_func;
    delete options.work_thread_dest_func;
}
