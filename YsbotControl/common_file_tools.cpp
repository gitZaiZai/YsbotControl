// *************************************
// 版权所有：广州亚思信息科技有限公司
// 文件名：common_file_tools.cpp
// 作者：陆巍
// 版本：1.0
// 日期：2016年7月
// 描述: 常用文件操作函数库
// *************************************

#include "stdafx.h"
#include "common_file_tools.h"

const int kMaxFileSize = 104857600;     // 限制读入文件的最大尺寸，默认为100M


// 功能：把指定的文件转换为字符串
// 输入：文件名（string类型），保存转换结果的string引用
// 返回：转换成功时返回0，文件打开失败返回-1；文件大小超过指定大小，返回-2；空间申请失败返回-3
// 备注：理论可以读入的最大文件尺寸是4G，但读入如此大的文件对系统内存资源的消耗是巨大的。
//       因此，这里设置了一个文件大小限制的参数kMaxFileSize，用户可以根据实际需要调整。
int FileToString(std::string file_name, std::string &return_string) {
	std::ifstream file_handle;
	int result = 0;
	unsigned int file_size = 10;

	// 在VS2010中可以直接用file_name，但在其他编译器会报错，加上.c_str()后就都可以通过
	file_handle.open(file_name.c_str(), std::ios::binary);
	if (!file_handle) {
		// 打开文件失败时返回结果为0
		result = 0;
	} else {
		// 根据偏移量获取文件的大小
		file_handle.seekg(0, std::ios::end);
		unsigned long long size1 = file_handle.tellg();  // 这里的返回值是64位的
		// 文件大小限制，默认设置为100M。为了防止用户误设置，这里另外规定不得超过4G。
		if (size1 >= kMaxFileSize || size1 >= 4294967296) {
			result = -2;
		} else {
			// 强制转换是用于消除编译器警告，因为前面已经做了大小限制，所以这里实际上是不可能出现数据丢失的
			file_size = (unsigned int)size1;
			// 申请内存。这里使用了抑制new抛出异常的做法
			char * str = new (std::nothrow) char[file_size];
			if (str == 0) {
				// 申请空间失败时返回结果为-2
				result = -3;
			} else {
				// 把文件游标移到开始位置，然后读取
				file_handle.seekg(0, std::ios::beg);
				file_handle.read(str, file_size);
				return_string = str;
				// 必须按照文件大小确定字符串内容，否则会在字符串结尾出现乱码
				return_string = return_string.substr(0, file_size);
				// 释放内存
				delete [] str;
			}
		}		
	}
	file_handle.close();
	return result;
}