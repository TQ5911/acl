#include "stdafx.h"

static acl::string __keypre("test_key");

static bool test_set(acl::redis& cmd, int n)
{
	acl::string key, val;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		val.format("val_%s", key.c_str());
		cmd.clear();
		if (cmd.set(key, val) == false)
		{
			printf("set key: %s error: %s\r\n", key.c_str(),
				cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("set key: %s ok\r\n", key.c_str());
	}

	return true;
}

static bool test_get(acl::redis& cmd, int n)
{
	acl::string key, val;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		cmd.clear();
		val.clear();
		if (cmd.get(key, val) == false)
		{
			printf("get key: %s error: %s\r\n", key.c_str(),
				cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("get key: %s ok, val: %s\r\n", key.c_str(),
				val.c_str());
	}

	return true;
}

static bool test_del(acl::redis& cmd, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		cmd.clear();
		int ret = cmd.del_one(key.c_str());
		if (ret < 0)
		{
			printf("del key: %s error: %s\r\n", key.c_str(),
				cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("del ok, key: %s\r\n", key.c_str());
	}

	return true;
}

static bool test_expire(acl::redis& cmd, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		cmd.clear();
		if (cmd.expire(key.c_str(), 100) < 0)
		{
			printf("expire key: %s error: %s\r\n", key.c_str(),
				cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("expire ok, key: %s\r\n", key.c_str());
	}

	return true;
}

static bool test_ttl(acl::redis& cmd, int n)
{
	acl::string key;
	int ttl;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		cmd.clear();
		if ((ttl = cmd.ttl(key.c_str())) < 0)
		{
			printf("get ttl key: %s error: %s\r\n", key.c_str(),
				cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("ttl ok, key: %s, ttl: %d\r\n",
				key.c_str(), ttl);
	}

	return true;
}

static bool test_exists(acl::redis& cmd, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		cmd.clear();
		if (cmd.exists(key.c_str()) == false)
		{
			if (i < 10)
				printf("no exists key: %s\r\n", key.c_str());
		}
		else if (i < 10)
			printf("exists key: %s\r\n", key.c_str());
	}

	return true;
}

static bool test_type(acl::redis& cmd, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		cmd.clear();
		acl::redis_key_t ret = cmd.type(key.c_str());
		if (ret == acl::REDIS_KEY_NONE)
		{
			printf("unknown type key: %s error: %s\r\n",
				key.c_str(), cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("type ok, key: %s, ret: %d\r\n",
				key.c_str(), ret);
	}

	return true;
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[127.0.0.1:6379]\r\n"
		"-p password[default: \"\"]\r\n"
		"-d dbnum[default: 0]\r\n"
		"-n count\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-T rw_timeout[default: 10]\r\n"
		"-a cmd[set|del|expire|ttl|exists|type|all]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10, dbnum = 0;
	acl::string addr("127.0.0.1:6379"), command, passwd;

	while ((ch = getopt(argc, argv, "hs:n:C:T:a:p:d:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'n':
			n = atoi(optarg);
			break;
		case 'C':
			conn_timeout = atoi(optarg);
			break;
		case 'T':
			rw_timeout = atoi(optarg);
			break;
		case 'a':
			command = optarg;
			break;
		case 'p':
			passwd = optarg;
			break;
		case 'd':
			dbnum = atoi(optarg);
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();
	acl::log::stdout_open(true);

	acl::redis_client client(addr.c_str(), conn_timeout, rw_timeout);
	client.set_password(passwd);
	if (dbnum > 0)
		client.set_db(dbnum);

	acl::redis cmd(&client);
test_set_eval(cmd,1);
	bool ret;

	if (command == "set")
		ret = test_set(cmd, n);
	else if (command == "get")
		ret = test_get(cmd, n);
	else if (command == "del")
		ret = test_del(cmd, n);
	else if (command == "expire")
		ret = test_expire(cmd, n);
	else if (command == "ttl")
		ret = test_ttl(cmd, n);
	else if (command == "exists")
		ret = test_exists(cmd, n);
	else if (command == "type")
		ret = test_type(cmd, n);
	else if (command == "all")
	{
		ret = test_set(cmd, n)
			&& test_get(cmd, n)
			&& test_expire(cmd, n)
			&& test_ttl(cmd, n)
			&& test_exists(cmd, n)
			&& test_type(cmd, n)
			&& test_del(cmd, n);
	}
	else
	{
		ret = false;
		printf("unknown cmd: %s\r\n", command.c_str());
	}

	if (ret == true)
		printf("test OK!\r\n");
	else
		printf("test failed!\r\n");

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}

static bool test_set_eval(acl::redis& cmd, int n)
{
	acl::string key, val;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		val.format("val_%s", key.c_str());
		cmd.clear();
		std::vector<acl::string> k{"test_key_0"};
		std::vector<acl::string> v{"val_10000011111111111"};
		acl::string res ;
		char buf[10000] = {0};
		bool re;

		//cmd.set("test_key_0","1223111113");
		std::vector<acl::string> scripts{"ada0bc9efe2392bdcc0083f7f8deaca2da7f32ec"};
		std::vector<bool> out;
		int result = cmd.script_exists(scripts, out);
		//cmd.eval("ada0bc9efe2392bdcc0083f7f8deaca2da7f32ec",k,v);
		//printf(" eval_string get key: %s ok, val: %s\r\n", key.c_str(),buf);
		cmd.evalsha("e6def56fa72ad52bc04105fffad285a8d26e49db",k,v);
		printf(" eval_string get key: %s ok, val: %s\r\n", key.c_str(),res.c_str());

	}

	return true;
}
/*
1.通过lua文件调用redis
[root@iZbp118g1g7afz4aq1d5tqZ redis-5.0.4]# cat setvalue.lua 
local key = KEYS[1]
local val = ARGV[1]
if redis.call("EXISTS",key) == 1 then
        return redis.call("SET",key,val)
else
        return "123"
end


2.
[root@iZbp118g1g7afz4aq1d5tqZ redis-5.0.4]# ./src/redis-cli -h 127.0.0.1 -p 6379 -a 295911 SCRIPT LOAD "$(cat setvalue.lua)"
Warning: Using a password with '-a' or '-u' option on the command line interface may not be safe.
"e6def56fa72ad52bc04105fffad285a8d26e49db"

3.
127.0.0.1:6379> get test_key_0
"val_10000011111111111"

4.
127.0.0.1:6379> evalsha e6def56fa72ad52bc04105fffad285a8d26e49db 2 test_key_0 , 1111111
OK

5.
127.0.0.1:6379> get test_key_0
"1111111"

6.acl redis调用
std::vector<acl::string> k{"test_key_0"};
std::vector<acl::string> v{"val_10000011111111111"};
cmd.evalsha("e6def56fa72ad52bc04105fffad285a8d26e49db",k,v);


编译acl
https://github.com/acl-dev/acl下载acl库
windows编译acl_cpp_vc*.sln，我这里是vs2017,无需修改属性直接编译通过
acl-3.5.3-0\lib_acl_cpp\samples\redis下有使用redis客户端的例子(如redis_string、redis_set、redis_hash是操作redis数据类型的例子，redis_pubsub是发布订阅的例子)，直接编译通过可以跑起来
*/
