/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#pragma once

#include <cassert>
#include <vector>

#define PATTERNS_USE_HINTS 1

namespace hook
{
	extern ptrdiff_t baseAddressDifference;

	// sets the base address difference based on an obtained pointer
	inline void set_base(uintptr_t address)
	{
#ifdef _M_IX86
		uintptr_t addressDiff = (address - 0x400000);
#elif defined(_M_AMD64)
		uintptr_t addressDiff = (address - 0x140000000);
#endif

		// pointer-style cast to ensure unsigned overflow ends up copied directly into a signed value
		baseAddressDifference = *(ptrdiff_t*)&addressDiff;
	}

	// sets the base to the process main base
	void set_base();

	template<typename T>
	inline T* getRVA(uintptr_t rva)
	{
		set_base();
#ifdef _M_IX86
		return (T*)(baseAddressDifference + 0x400000 + rva);
#elif defined(_M_AMD64)
		return (T*)(0x140000000 + rva);
#endif
	}

	class pattern_byte
	{
	public:
		enum class match_method :std::uint8_t
		{
			wild,
			high4,
			low4,
			exact
		};

		pattern_byte()
		{
			this->be_wild();
		}

		pattern_byte(std::uint8_t value, match_method method = match_method::exact)
		{
			this->set_value(value, method);
		}

		pattern_byte &set_value(std::uint8_t value, match_method method = match_method::exact)
		{
			this->_method = method;
			this->_value = 0;

			switch (this->_method)
			{
			case match_method::high4:
				this->_high_value = value;
				break;

			case match_method::low4:
				this->_low_value = value;
				break;

			case match_method::exact:
				this->_value = value;
				break;

			default:
				break;
			}

			return *this;
		}

		void be_wild()
		{
			this->_method = match_method::wild;
			this->_value = 0;
		}

		bool is_wild() const
		{
			return this->_method == match_method::wild;
		}

		bool match(std::uint8_t byte) const
		{
			switch (this->_method)
			{
			case match_method::exact:
				return this->_value == byte;

			case match_method::high4:
				return this->_high_value == (byte >> 4u);

			case match_method::low4:
				return this->_low_value == (byte & 0x0Fu);

			case match_method::wild:
				return true;

			default:
				return false;
			}
		}

		bool equivalent(const pattern_byte &rhs) const
		{
			if (this->is_wild() || rhs.is_wild())
			{
				return true;
			}

			if (this->_method != rhs._method)
			{
				if (!(this->_method == match_method::exact || rhs._method == match_method::exact))
				{
					return false;
				}
			}

			switch (this->_method)
			{
			case match_method::low4:
				return  (this->_low_value == rhs._low_value);

			case match_method::high4:
				return  (this->_high_value == rhs._high_value);

			case match_method::exact:
				switch (rhs._method)
				{
				case match_method::low4:
					return  (this->_low_value == rhs._low_value);

				case match_method::high4:
					return  (this->_high_value == rhs._high_value);

				case match_method::exact:
					return (this->_value == rhs._value);

				default:
					return false;
				}

			default:
				return false;
			}
		}

		bool operator==(const pattern_byte &rhs) const
		{
			return this->equivalent(rhs);
		}

		bool operator==(std::uint8_t value) const
		{
			return this->match(value);
		}

		bool operator!=(const pattern_byte &rhs) const
		{
			return !this->equivalent(rhs);
		}

		bool operator!=(std::uint8_t value) const
		{
			return !this->match(value);
		}

	private:
		union
		{
			struct
			{
				std::uint8_t _low_value : 4;
				std::uint8_t _high_value : 4;
			};

			std::uint8_t _value;
		};

		match_method _method;
	};

	class pattern_match
	{
	private:
		void* m_pointer;

	public:
		inline pattern_match(void* pointer)
			: m_pointer(pointer)
		{
		}

		template<typename T>
		T* get(ptrdiff_t offset = 0) const
		{
			char* ptr = reinterpret_cast<char*>(m_pointer);
			return reinterpret_cast<T*>(ptr + offset);
		}
	};

	class pattern
	{
	private:
		std::vector<pattern_byte> m_bytes;

#if PATTERNS_USE_HINTS
		uint64_t m_hash;
#endif

		std::vector<pattern_match> m_matches;

		bool m_matched;

		union
		{
			void* m_module;
			struct
			{
				uintptr_t m_rangeStart;
				uintptr_t m_rangeEnd;
			};
		};

	protected:
		inline pattern(void* module)
			: m_module(module), m_rangeEnd(0), m_matched(false)
		{
		}

		inline pattern(uintptr_t begin, uintptr_t end)
			: m_rangeStart(begin), m_rangeEnd(end), m_matched(false)
		{
		}

		void Initialize(const char* pattern);

	private:
		bool ConsiderMatch(uintptr_t offset);

		void EnsureMatches(uint32_t maxCount);

		inline const pattern_match& _get_internal(size_t index)
		{
			return m_matches[index];
		}

	public:
		pattern()
			: m_matched(true)
		{
		}

		template<size_t Len>
		pattern(const char (&pattern)[Len])
			: pattern(getRVA<void>(0))
		{
			Initialize(pattern);
		}

		pattern(std::string& pattern)
			: pattern(getRVA<void>(0))
		{
			Initialize(pattern.c_str());
		}

		inline pattern& count(uint32_t expected)
		{
			EnsureMatches(expected);
			assert(m_matches.size() == expected);
			return *this;
		}

		inline pattern& count_hint(uint32_t expected)
		{
			EnsureMatches(expected);
			return *this;
		}

		inline pattern& clear(void* module = nullptr)
		{
			if (module)
				m_module = module;
			m_matches.clear();
			m_matched = false;
			return *this;
		}

		inline size_t size()
		{
			EnsureMatches(UINT32_MAX);
			return m_matches.size();
		}

		inline bool empty()
		{
			return size() == 0;
		}

		inline const pattern_match& get(size_t index)
		{
			EnsureMatches(UINT32_MAX);
			return _get_internal(index);
		}

		inline const pattern_match& get_one()
		{
			return count(1)._get_internal(0);
		}

		template<typename T = void>
		inline auto get_first(ptrdiff_t offset = 0)
		{
			return get_one().get<T>(offset);
		}

		template <typename Fn>
		void for_each_result(Fn Pr) const
		{
			for (auto &result : this->m_matches)
			{
				Pr(result);
			}
		}

	public:
#if PATTERNS_USE_HINTS
		// define a hint
		static void hint(uint64_t hash, uintptr_t address);
#endif
	};

	class module_pattern
		: public pattern
	{
	public:
		template<size_t Len>
		module_pattern(void* module, const char(&pattern)[Len])
			: pattern(module)
		{
			Initialize(pattern, Len);
		}

		module_pattern(void* module, std::string& pattern)
			: pattern(module)
		{
			Initialize(pattern.c_str());
		}
	};

	class range_pattern
		: public pattern
	{
	public:
		template<size_t Len>
		range_pattern(uintptr_t begin, uintptr_t end, const char(&pattern)[Len])
			: pattern(begin, end)
		{
			Initialize(pattern, Len);
		}

		range_pattern(uintptr_t begin, uintptr_t end, std::string& pattern)
			: pattern(begin, end)
		{
			Initialize(pattern.c_str());
		}
	};


	template<typename T = void, size_t Len>
	auto get_pattern(const char(&pattern_string)[Len], ptrdiff_t offset = 0)
	{
		return pattern(pattern_string).get_first<T>(offset);
	}
}
