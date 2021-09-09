// *************************************
// ��Ȩ���У�������˼��Ϣ�Ƽ����޹�˾
// �ļ�����common_file_tools.cpp
// ���ߣ�½Ρ
// �汾��1.0
// ���ڣ�2016��7��
// ����: �����ļ�����������
// *************************************

#include "stdafx.h"
#include "common_file_tools.h"

const int kMaxFileSize = 104857600;     // ���ƶ����ļ������ߴ磬Ĭ��Ϊ100M


// ���ܣ���ָ�����ļ�ת��Ϊ�ַ���
// ���룺�ļ�����string���ͣ�������ת�������string����
// ���أ�ת���ɹ�ʱ����0���ļ���ʧ�ܷ���-1���ļ���С����ָ����С������-2���ռ�����ʧ�ܷ���-3
// ��ע�����ۿ��Զ��������ļ��ߴ���4G����������˴���ļ���ϵͳ�ڴ���Դ�������Ǿ޴�ġ�
//       ��ˣ�����������һ���ļ���С���ƵĲ���kMaxFileSize���û����Ը���ʵ����Ҫ������
int FileToString(std::string file_name, std::string &return_string) {
	std::ifstream file_handle;
	int result = 0;
	unsigned int file_size = 10;

	// ��VS2010�п���ֱ����file_name�����������������ᱨ������.c_str()��Ͷ�����ͨ��
	file_handle.open(file_name.c_str(), std::ios::binary);
	if (!file_handle) {
		// ���ļ�ʧ��ʱ���ؽ��Ϊ0
		result = 0;
	} else {
		// ����ƫ������ȡ�ļ��Ĵ�С
		file_handle.seekg(0, std::ios::end);
		unsigned long long size1 = file_handle.tellg();  // ����ķ���ֵ��64λ��
		// �ļ���С���ƣ�Ĭ������Ϊ100M��Ϊ�˷�ֹ�û������ã���������涨���ó���4G��
		if (size1 >= kMaxFileSize || size1 >= 4294967296) {
			result = -2;
		} else {
			// ǿ��ת���������������������棬��Ϊǰ���Ѿ����˴�С���ƣ���������ʵ�����ǲ����ܳ������ݶ�ʧ��
			file_size = (unsigned int)size1;
			// �����ڴ档����ʹ��������new�׳��쳣������
			char * str = new (std::nothrow) char[file_size];
			if (str == 0) {
				// ����ռ�ʧ��ʱ���ؽ��Ϊ-2
				result = -3;
			} else {
				// ���ļ��α��Ƶ���ʼλ�ã�Ȼ���ȡ
				file_handle.seekg(0, std::ios::beg);
				file_handle.read(str, file_size);
				return_string = str;
				// ���밴���ļ���Сȷ���ַ������ݣ���������ַ�����β��������
				return_string = return_string.substr(0, file_size);
				// �ͷ��ڴ�
				delete [] str;
			}
		}		
	}
	file_handle.close();
	return result;
}