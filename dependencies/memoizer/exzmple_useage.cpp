#include "memoizer.h"


long long fibonacci_logic(std::function<long long(int)> self, int n) {
	if (n <= 1) return n;
	return self(n - 1) + self(n - 2);
}

long long fibonacci_manual(int n) {
	if (n <= 1) return n;
	return fibonacci_manual(n - 1) + fibonacci_manual(n - 2);
}





void fibonacci_testing() {

	// Create a memoizer pointer to look up saved data
	Memoizer<long long, int>* memo_ptr;

	// Define the lambda function to be memoized, which calls itself through the memoizer pointer
	auto func = [&](int n) {
		return fibonacci_logic([&](int i) { return (*memo_ptr)(i); }, n);
	};

	// Create the memoized function with the lambda function
	Memoizer<long long, int> memo_fibonacci(func, Streategies::LRU, 1000);
	memo_ptr = &memo_fibonacci;

	// Test calls
	std::cout << "Fibonacci 26: " << memo_fibonacci(26) << std::endl;
	std::cout << "Fibonacci 43: " << memo_fibonacci(43) << std::endl;
	std::cout << "Fibonacci 67: " << memo_fibonacci(67) << std::endl;

}

void custom_rule_testing() {
	srand(time(0));
	// remove random element from the cache
	auto custom_rule = [](auto& current_cache) {
		size_t random_index = rand() % current_cache.size();
		auto el = current_cache.begin();
		std::advance(el, random_index);
		current_cache.erase(el);

		std::cout << "Custom rule: removed random element from cache" << std::endl;
		};

	Memoizer<long long, int>* memo_ptr;
	auto func = [&](int n) {
		return fibonacci_logic([&](int x) { return (*memo_ptr)(x); }, n);
		};
	Memoizer<long long, int> memoized_fib(func, Streategies::Custom, 100, 0, custom_rule);
	memo_ptr = &memoized_fib;

	std::cout << "Testing Fibonacci with custom rule (random eviction): " << std::endl;
	
	std::cout << "Fibonacci 26: " << memoized_fib(26) << std::endl;
	std::cout << "Fibonacci 43: " << memoized_fib(43) << std::endl;
	std::cout << "Fibonacci 67: " << memoized_fib(67) << std::endl;


}


int main_() {

	fibonacci_testing();
	custom_rule_testing();

	return 0;
}