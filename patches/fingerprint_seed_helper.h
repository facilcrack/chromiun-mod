// Copyright 2024 The Chromium Authors
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_FINGERPRINT_SEED_HELPER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_FINGERPRINT_SEED_HELPER_H_

#include <cstdint>
#include <fstream>
#include <random>
#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"

namespace blink {

inline uint32_t GetFingerprintSeed() {
  static uint32_t cached_seed = 0;
  static bool initialized = false;
  
  if (initialized) return cached_seed;
  initialized = true;
  
  auto* cmd = base::CommandLine::ForCurrentProcess();
  if (cmd && cmd->HasSwitch("fingerprint-seed")) {
    std::string s = cmd->GetSwitchValueASCII("fingerprint-seed");
    unsigned int p = 0;
    if (base::StringToUint(s, &p)) {
      cached_seed = static_cast<uint32_t>(p);
      return cached_seed;
    }
  }
  
  std::ifstream in("/data/local/tmp/fp_seed");
  if (in.good()) { in >> cached_seed; in.close(); if (cached_seed != 0) return cached_seed; }
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dis(100000000, 2147483647);
  cached_seed = dis(gen);
  
  std::ofstream out("/data/local/tmp/fp_seed");
  if (out.good()) { out << cached_seed; out.close(); }
  
  return cached_seed;
}

}  // namespace blink

#endif
