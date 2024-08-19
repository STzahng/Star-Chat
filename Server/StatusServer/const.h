#pragma once

#include <iostream>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include "singleton.h"
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>
#include <queue>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/asio.hpp>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "hiredis.h"
#include <cassert>
#include <thread>

#define CODEPREFIX "code_"
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
	Success = 0, 
	Error_Json = 1001, // Json��������
	RPCFailed = 1002,//RPC�������
	VarifyExpired = 1003,//��֤�����
	VarifyCodeErr = 1004,//��֤�����
	UserExist = 1005,//�û��Ѿ�����
	PasswdErr = 1006,//�������
	EmailNotMatch = 1007,//���䲻ƥ��
	PasswdUpFailed = 1008,//��������ʧ��
	PasswdInvalid = 1009,//�������ʧ��
};

class Defer {
public:
	//����һ��lambda���ʽ������ָ��
	Defer(std::function<void()> func) :func_(func) {}

	//����������ִ�д���ĺ���
	~Defer() {
		func_();
	}
private:
	std::function<void()> func_;
};

