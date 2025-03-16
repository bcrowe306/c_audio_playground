#pragma once
#ifndef UUID_GENERATOR_H
#define UUID_GENERATOR_H

#include "uuid.h"
#include <string>

using namespace uuids;

static inline std::string generateUUID() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};

  uuid const id = gen();
  return uuids::to_string(id);
}

static inline uuid uuidFromString(std::string uuidString) {
    auto id = uuids::uuid::from_string(uuidString);
    if(id.has_value()) {
        return id.value();
    }
}

#endif // !UUID_GENERATOR_H