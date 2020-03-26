/*
 * Copyright © 2020 Andrew Penkrat <contact.aldrog@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LRU_CACHE_LOG
#define LRU_CACHE_LOG

#ifdef LRU_CACHE_DEBUG

#include <iostream>

namespace lru_cache {
static auto &log = std::cerr;
}

#else // LRU_CACHE_DEBUG

namespace lru_cache {

namespace detail {
struct DiscardStream {};

template <typename T>
constexpr DiscardStream &operator<<(DiscardStream &str, const T &) {
  return str;
}
} // namespace detail

static detail::DiscardStream log;

} // namespace lru_cache

#endif // LRU_CACHE_DEBUG

#endif // LRU_CACHE_LOG
