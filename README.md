# protobuf_sofa_rpc_demo
protobuf sofa_pbrpc demo
因为protobuf本身只提供了一个service的结构，并没有提供具体协议，所以这个具体采用怎么样的rpc是可以由每个人自己定的。
常见的有grpc之类的兼容的协议，但我考虑了一下并不需要那么庞大的东西。就采用了[sofa-pbrpc](https://github.com/baidu/sofa-pbrpc)
目前实现了一个简要的通讯demo，其实也是来源于它源代码中的demo。之后会根据自己的业务需求进行添加。
