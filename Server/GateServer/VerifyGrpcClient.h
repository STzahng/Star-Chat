#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"

using grpc::Channel;//信道
using grpc::Status;//gRpc操作状态
using grpc::ClientContext;//gRPC客户端的上下文

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool {
public:
	RPConPool(size_t poolsize,std::string host, std::string port);
	~RPConPool();
	void Close();
	std::unique_ptr< VarifyService::Stub> getConnection();
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);
private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr< VarifyService::Stub>> connections_;
	std::condition_variable cond_;
	std::mutex mutex_;
};

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	GetVarifyRsp GetVarifycode(std::string email);
	
private:
	VerifyGrpcClient();
	std::unique_ptr<RPConPool> pool_;
};

