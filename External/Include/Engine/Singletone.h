#pragma once

template<typename T>
class CSingleton
{
private:
	// 함수 포인터
	typedef void(*DESTROY)(void);

private:
	static T* m_This;

public:
	static T* GetInst()
	{
		if (m_This == nullptr)
		{
			// Heap 메모리 할당
			m_This = new T;
		}

		return m_This;
	}

	// 런타임 도중 지워도 상관 없고
	// 삭제를 안하고 프로그램을 종료해도 상관이 없다.
	static void Destroy()
	{
		if (m_This != nullptr)
		{
			delete m_This;
			m_This = nullptr;
		}
	}

protected:
	CSingleton()
	{
		// 프로그램 종료 시 해당 함수 포인터를 호출 시킨다.
		atexit((DESTROY)&CSingleton<T>::Destroy);
	}
	// 복사 생성자 삭제로 싱글톤 객체가 추가적으로 생성되는 것을 방지
	CSingleton(const CSingleton& _other) = delete;
	~CSingleton()
	{}
};

template<typename T>
T* CSingleton<T>::m_This = nullptr;