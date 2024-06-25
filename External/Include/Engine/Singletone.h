#pragma once

template<typename T>
class CSingleton
{
private:
	// �Լ� ������
	typedef void(*DESTROY)(void);

private:
	static T* m_This;

public:
	static T* GetInst()
	{
		if (m_This == nullptr)
		{
			// Heap �޸� �Ҵ�
			m_This = new T;
		}

		return m_This;
	}

	// ��Ÿ�� ���� ������ ��� ����
	// ������ ���ϰ� ���α׷��� �����ص� ����� ����.
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
		// ���α׷� ���� �� �ش� �Լ� �����͸� ȣ�� ��Ų��.
		atexit((DESTROY)&CSingleton<T>::Destroy);
	}
	// ���� ������ ������ �̱��� ��ü�� �߰������� �����Ǵ� ���� ����
	CSingleton(const CSingleton& _other) = delete;
	~CSingleton()
	{}
};

template<typename T>
T* CSingleton<T>::m_This = nullptr;