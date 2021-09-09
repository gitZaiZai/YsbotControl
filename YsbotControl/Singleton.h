// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#pragma once

template <class TCreate>
class Singleton : public TCreate
{
protected:
	Singleton();
public:
	~Singleton();
	static Singleton<TCreate>* Instance();
	static void UnInstance();
	static bool IsAvaliable() { return m_pInstance != NULL; }
private:
	static Singleton<TCreate>* m_pInstance;
};

template <class TCreate>
void Singleton<TCreate>::UnInstance()
{
	if ( m_pInstance!=NULL )
	{
		(CWnd*)m_pInstance->DestroyWindow();
 		delete m_pInstance;
		m_pInstance=NULL;
	}
 }

template <class TCreate>
Singleton<TCreate>* Singleton<TCreate>::m_pInstance=NULL;

template <class TCreate>
Singleton<TCreate>* Singleton<TCreate>::Instance()
{
	if (m_pInstance==NULL)
	{
		m_pInstance = new Singleton<TCreate>();
	}
	return m_pInstance;	
}

template <class TCreate>
Singleton<TCreate>::~Singleton()
{

}

template <class TCreate>
Singleton<TCreate>::Singleton()
{

}
