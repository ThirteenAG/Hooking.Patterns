/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#include "Hooking.Patterns.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <string_view>
#include <array>

#if PATTERNS_USE_HINTS
#include <map>
#endif

#if PATTERNS_USE_HINTS
// from VS2017 Standard Library someplace
static struct
{
	std::uint64_t operator()(const std::vector<hook::pattern_byte> &pat) const
	{
		static const std::uint64_t _FNV_offset_basis = 14695981039346656037ULL;
		static const std::uint64_t _FNV_prime = 1099511628211ULL;

		const std::uint8_t *_First = reinterpret_cast<const std::uint8_t *>(pat.data());
		std::uint64_t _Count = static_cast<std::uint64_t>(pat.size()) * sizeof(hook::pattern_byte);

		std::uint64_t _Val = _FNV_offset_basis;

		for (std::uint64_t _Next = 0; _Next < _Count; ++_Next)
		{	// fold in another byte
			_Val ^= (std::uint64_t)_First[_Next];
			_Val *= _FNV_prime;
		}
		return (_Val);
	}
} g_pattern_hasher;

#endif

namespace hook
{
	ptrdiff_t baseAddressDifference;

	// sets the base to the process main base
	void set_base()
	{
		set_base((uintptr_t)GetModuleHandle(nullptr));
	}


#if PATTERNS_USE_HINTS
static std::multimap<uint64_t, uintptr_t> g_hints;
#endif

static void TransformPattern(const char *literal, std::vector<pattern_byte> &data)
{
	auto tol = [](char ch) -> uint8_t
	{
		if (ch >= 'A' && ch <= 'F') return uint8_t(ch - 'A' + 10);
		if (ch >= 'a' && ch <= 'f') return uint8_t(ch - 'a' + 10);
		return uint8_t(ch - '0');
	};

	auto is_digit = [](char ch) -> bool
	{
		return (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9');
	};

	std::array<char, 2> temp_string{ 0, 0 };

	data.clear();

	std::string_view view(literal, std::string_view(literal).size() + 1);

	for (auto ch : view)
	{
		if (ch == ' ' || ch == 0)
		{
			if (!temp_string[0] && !temp_string[1])
			{
				continue;
			}
			else if (temp_string[0] == '?' && (temp_string[1] == '?' || temp_string[1] == 0))
			{
				data.emplace_back();
			}
			else if (temp_string[0] == '?' && is_digit(temp_string[1]))
			{
				data.emplace_back(tol(temp_string[1]), pattern_byte::match_method::low4);
			}
			else if (temp_string[1] == '?' && is_digit(temp_string[0]))
			{
				data.emplace_back(tol(temp_string[0]), pattern_byte::match_method::high4);
			}
			else if (is_digit(temp_string[0]) && is_digit(temp_string[1]))
			{
				data.emplace_back((tol(temp_string[0]) << 4) | tol(temp_string[1]));
			}
			else
			{
				data.clear();
				return;
			}

			temp_string.fill(0);
		}
		else
		{
			if (temp_string[0] == 0)
			{
				temp_string[0] = ch;
			}
			else if (temp_string[1] == 0)
			{
				temp_string[1] = ch;
			}
			else
			{
				data.clear();
				return;
			}
		}
	}
}

class executable_meta
{
private:
	uintptr_t m_begin;
	uintptr_t m_end;

public:
	template<typename TReturn, typename TOffset>
	TReturn* getRVA(TOffset rva)
	{
		return (TReturn*)(m_begin + rva);
	}

	explicit executable_meta(void* module)
		: m_begin((uintptr_t)module), m_end(0)
	{
		static auto getSection = [](const PIMAGE_NT_HEADERS nt_headers, unsigned section) -> PIMAGE_SECTION_HEADER
		{
			return reinterpret_cast<PIMAGE_SECTION_HEADER>(
				(UCHAR*)nt_headers->OptionalHeader.DataDirectory +
				nt_headers->OptionalHeader.NumberOfRvaAndSizes * sizeof(IMAGE_DATA_DIRECTORY) +
				section * sizeof(IMAGE_SECTION_HEADER));
		};

		PIMAGE_DOS_HEADER dosHeader = getRVA<IMAGE_DOS_HEADER>(0);
		PIMAGE_NT_HEADERS ntHeader = getRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

		for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
		{
			auto sec = getSection(ntHeader, i);
			auto secSize = sec->SizeOfRawData != 0 ? sec->SizeOfRawData : sec->Misc.VirtualSize;
			if (sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)
				m_end = m_begin + sec->VirtualAddress + secSize;

			if ((i == ntHeader->FileHeader.NumberOfSections - 1) && m_end == 0)
				m_end = m_begin + sec->PointerToRawData + secSize;
		}
	}

	executable_meta(uintptr_t begin, uintptr_t end)
		: m_begin(begin), m_end(end)
	{
	}

	inline uintptr_t begin() const { return m_begin; }
	inline uintptr_t end() const   { return m_end; }
};

void pattern::Initialize(const char* pattern)
{
	TransformPattern(pattern, m_bytes);

	// get the hash for the transformed pattern
#if PATTERNS_USE_HINTS
	m_hash = g_pattern_hasher(this->m_bytes);
#endif

#if PATTERNS_USE_HINTS
	// if there's hints, try those first
	if (m_module == GetModuleHandle(nullptr))
	{
		auto range = g_hints.equal_range(m_hash);

		if (range.first != range.second)
		{
			std::for_each(range.first, range.second, [&] (const std::pair<uint64_t, uintptr_t>& hint)
			{
				ConsiderMatch(hint.second);
			});

			// if the hints succeeded, we don't need to do anything more
			if (!m_matches.empty())
			{
				m_matched = true;
				return;
			}
		}
	}
#endif
}

void pattern::EnsureMatches(uint32_t maxCount)
{
	if (m_matched)
		return;

	if (!m_rangeStart && !m_rangeEnd && !m_module)
		return;

	// scan the executable for code
	executable_meta executable = m_rangeStart != 0 && m_rangeEnd != 0 ? executable_meta(m_rangeStart, m_rangeEnd) : executable_meta(m_module);

	auto matchSuccess = [&] (uintptr_t address)
	{
#if PATTERNS_USE_HINTS
		g_hints.emplace(m_hash, address);
#else
		(void)address;
#endif

		return (m_matches.size() == maxCount);
	};

	std::array<std::ptrdiff_t, 256> bmbc;
	std::ptrdiff_t index;

	//Bad characters table
	for (std::uint32_t bc = 0; bc < 256; ++bc)
	{
		for (index = this->m_bytes.size() - 1; index >= 0; --index)
		{
			if (this->m_bytes[index].match(bc))
			{
				break;
			}
		}

		bmbc[bc] = index;
	}

	std::uint8_t *range_begin = reinterpret_cast<std::uint8_t *>(executable.begin());
	std::uint8_t *range_end = reinterpret_cast<std::uint8_t *>(executable.end() - this->m_bytes.size());

	__try
	{
		while (range_begin <= range_end)
		{
			for (index = this->m_bytes.size() - 1; index >= 0; --index)
			{
				if (!this->m_bytes[index].match(range_begin[index]))
				{
					break;
				}
			}

			if (index == -1)
			{
				this->m_matches.emplace_back(range_begin);
				range_begin += this->m_bytes.size();
			}
			else
			{
				range_begin += std::max(index - bmbc[range_begin[index]], 1);
			}
		}
	}
	__except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) 
	{ }
	m_matched = true;
}

bool pattern::ConsiderMatch(uintptr_t offset)
{
	auto ptr = reinterpret_cast<std::uint8_t *>(offset);

	if (std::equal(m_bytes.begin(), m_bytes.end(), ptr))
	{
		m_matches.emplace_back(ptr);
		return true;
	}

	return false;
}

#if PATTERNS_USE_HINTS
void pattern::hint(uint64_t hash, uintptr_t address)
{
	auto range = g_hints.equal_range(hash);

	for (auto it = range.first; it != range.second; it++)
	{
		if (it->second == address)
		{
			return;
		}
	}

	g_hints.emplace(hash, address);
}
#endif
}