#pragma once
// #define DDB_RELEASE

#include <chrono>
#include <codecvt>
#include <filesystem>
#include <iostream>
#include <map>
#include <ranges>
#include <regex>
#include <thread>
#include <unordered_map>
#include <windows.h>

#include "magic_enum/magic_enum.hpp"

#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

using namespace std;

extern wstring buildTime;
extern wstring editionDate;

extern wstring userid;
extern wstring globalPath;

#ifndef DDB_RELEASE
void Test();
void Testb(bool t);
void Testi(long long t);
void Testw(wstring t);
void Testa(string t);
#endif

// 私有模板
template <typename IdtAtomicT>
class IdtAtomic
{
	static_assert(is_trivially_copyable_v<IdtAtomicT>, "IdtAtomic<IdtAtomicT>: IdtAtomicT 必须是平凡可复制 (TriviallyCopyable) 的类型。");
	static_assert(atomic<IdtAtomicT>::is_always_lock_free, "IdtAtomic<IdtAtomicT>: IdtAtomicT 对应的 atomic<IdtAtomicT> 必须保证始终无锁 (lock-free)。");

private:
	atomic<IdtAtomicT> value;

public:
	IdtAtomic() noexcept = default;
	IdtAtomic(IdtAtomicT desired) noexcept : value(desired) {}
	IdtAtomic(const IdtAtomic& other) noexcept { value.store(other.value.load()); }

	IdtAtomic& operator=(const IdtAtomic& other) noexcept {
		if (this != &other) value.store(other.value.load());
		return *this;
	}

	IdtAtomic(IdtAtomic&& other) noexcept { value.store(other.value.load()); }
	IdtAtomic& operator=(IdtAtomic&& other) noexcept { value.store(other.value.load()); return *this; }

	IdtAtomicT load(memory_order order = memory_order_seq_cst) const noexcept { return value.load(order); }
	void store(IdtAtomicT desired, memory_order order = memory_order_seq_cst) noexcept { value.store(desired, order); }

	IdtAtomicT exchange(IdtAtomicT desired, memory_order order = memory_order_seq_cst) noexcept { return value.exchange(desired, order); }
	bool compare_exchange_weak(IdtAtomicT& expected, IdtAtomicT desired,
		memory_order success = memory_order_seq_cst,
		memory_order failure = memory_order_seq_cst) noexcept {
		return value.compare_exchange_weak(expected, desired, success, failure);
	}
	bool compare_exchange_strong(IdtAtomicT& expected, IdtAtomicT desired,
		memory_order success = memory_order_seq_cst,
		memory_order failure = memory_order_seq_cst) noexcept {
		return value.compare_exchange_strong(expected, desired, success, failure);
	}
	bool compare_set_strong(const IdtAtomicT& expected_val_in, IdtAtomicT desired_val,
		std::memory_order success = std::memory_order_seq_cst,
		std::memory_order failure = std::memory_order_seq_cst) noexcept {
		IdtAtomicT expected_local = expected_val_in;
		return value.compare_exchange_strong(expected_local, desired_val, success, failure);
	}

	template <typename T = IdtAtomicT, typename = std::enable_if_t<std::is_integral_v<T>>>
	T fetch_add(T arg, std::memory_order order = std::memory_order_seq_cst) noexcept {
		return value.fetch_add(arg, order);
	}
	template <typename T = IdtAtomicT, typename = std::enable_if_t<std::is_integral_v<T>>>
	T fetch_sub(T arg, std::memory_order order = std::memory_order_seq_cst) noexcept {
		return value.fetch_sub(arg, order);
	}

	operator IdtAtomicT() const noexcept { return load(); }
	IdtAtomic& operator=(IdtAtomicT desired) noexcept { store(desired); return *this; }

	// Increment/Decrement Operators added
	template <typename T = IdtAtomicT, typename = std::enable_if_t<std::is_integral_v<T>>>
	T operator++() noexcept {
		return value.fetch_add(1, std::memory_order_seq_cst) + 1;
	}
	template <typename T = IdtAtomicT, typename = std::enable_if_t<std::is_integral_v<T>>>
	T operator++(int) noexcept {
		return value.fetch_add(1, std::memory_order_seq_cst);
	}
	template <typename T = IdtAtomicT, typename = std::enable_if_t<std::is_integral_v<T>>>
	T operator--() noexcept {
		return value.fetch_sub(1, std::memory_order_seq_cst) - 1;
	}
	template <typename T = IdtAtomicT, typename = std::enable_if_t<std::is_integral_v<T>>>
	T operator--(int) noexcept {
		return value.fetch_sub(1, std::memory_order_seq_cst);
	}
};

extern bool closeSign;