#pragma once

#include <iostream>
#include <unordered_map>
#include <list>
#include <functional>
#include <chrono>
#include <optional>
#include <random>

using namespace std::chrono_literals;

enum class Streategies {
	LFU,
	LRU,
	TTL,
	Custom
};

template <typename K, typename V>
struct Cache {
	V value;
	std::chrono::steady_clock::time_point timestamp;
	size_t used = 0;
};

template <typename R, typename... Args> // R - function return type
class Memoizer {
	using Key = std::tuple<Args...>;
	using Func = std::function<R(Args...)>;
	using CustomRemoveRule = std::function<void(std::unordered_map<Key, Cache<Key, R>>&)>;
	

	Func func;
	size_t max_size;
	std::chrono::milliseconds ttl_lenght;
	Streategies strategy;

	CustomRemoveRule custom_remove_rule;
	std::unordered_map<Key, Cache<Key, R>> cache;
	std::list<Key> usage_order;

public:
	Memoizer(Func f, Streategies s = Streategies::LRU, size_t limit = -1, int ms = 0, CustomRemoveRule r = nullptr) :
		func(f), strategy(s), max_size(limit), ttl_lenght(ms), custom_remove_rule(r) {
	}



	R operator()(Args... args) {
		Key key = std::make_tuple(args...); // the question we are asking the function
		auto now = std::chrono::steady_clock::now();

		if (cache.find(key) != cache.end()) {
			auto& answer = cache[key];

			if (strategy == Streategies::TTL && (now - answer.timestamp) > ttl_lenght && ttl_lenght > 0ms) {
				cache.erase(key);
			}
			else {
				answer.used++;
				if (strategy == Streategies::LRU) {
					update_lru(key);
				}
			}
			return answer.value;
		}

		R answer = func(args...);

		if (max_size > 0 && cache.size() > max_size) {
			remove_element();
		}

		cache[key] = { answer, now, 1 };
		if (strategy == Streategies::LRU) {
			usage_order.push_front(key);
		}

		return answer;
	}

private:
	inline void update_lru(const Key& key) {
		usage_order.remove(key);
		usage_order.push_front(key);
	}

	inline void remove_element() {
		// CUSTOM RULE
		if (strategy == Streategies::Custom && custom_remove_rule) {
			custom_remove_rule(cache);
		}
		// LEAST RECENTLY USED
		else if (strategy == Streategies::LRU && !usage_order.empty()) {
			cache.erase(usage_order.back());
			usage_order.pop_back();
		}
		// LEAST FREQUENTLY USED
		else if (strategy == Streategies::LFU) {
			auto least_used = cache.begin();

			for (auto el = cache.begin(); el != cache.end(); ++el) {
				if (el->second.used < least_used->second.used) {
					least_used = el;
				}
			}
			cache.erase(least_used);
		}
		// FIRST IN FIRST OUT
		else if (!cache.empty()) {
			cache.erase(cache.begin());
		}
	}
};

// handle tuple as key in unordered_map
namespace std {
	template<typename... T>
	struct hash<tuple<T...>> {
		size_t operator()(const tuple<T...>& t) const {
			return 0; // Simplified; clanker says boost::hash_combine should be used, but i am a lazy f...
		}
	};
}

