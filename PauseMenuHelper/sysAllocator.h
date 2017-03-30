#pragma once

namespace rage
{
	class sysMemAllocator
	{
	public:
		virtual ~sysMemAllocator() = 0;

		virtual void m_4() = 0;
		virtual void* allocate(size_t size, size_t align, int subAllocator) = 0;
		virtual void* m_C(size_t size, size_t align, int subAllocator) = 0;
		virtual void free(void* pointer) = 0;
	};

	inline sysMemAllocator* GetAllocator()
	{
		auto hThread = ThreadLocal::GetDefaultThread();

		if (hThread != NULL)
		{
			auto tls = *reinterpret_cast<DWORD64*>(ThreadLocal::GetThreadLocalStorage(hThread));

			if (tls)
			{
				sysMemAllocator *allocator = *(sysMemAllocator**)(tls + 200U);

				return allocator;
			}
		}

		return 0;
	}

	class sysUseAllocator
	{
	public:
		void* operator new(size_t size);

		inline void* operator new[](size_t size)
		{
			return sysUseAllocator::operator new(size);
		}

		void operator delete(void* memory);

		inline void operator delete[](void* memory)
		{
			return sysUseAllocator::operator delete(memory);
		}
	};
}