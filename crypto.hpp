#pragma once
#include <Windows.h>
#include <string>
#include <mutex>
#include <memory>
#include <random>
#include <stdint.h>
struct Crypt
{
	std::random_device rEngine;
	std::mt19937_64 gen64;
	Crypt() {
		gen64.seed(rEngine());
	}
	uint64_t seed_rand_entropy() {
		std::uniform_int_distribution<uint64_t> seed_gen(0, UINT64_MAX);
		return seed_gen(gen64);
	}
	
};
auto cryptEngine = std::make_unique<Crypt>();