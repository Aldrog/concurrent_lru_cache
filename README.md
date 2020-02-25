# Concurrent LRU Cache
A threadsafe map-like container implementing a least-recently-used cache.

This container will keep track on references to contained objects and store all currently referenced objects as well as N least recently referenced ones (N is a constructor argument).

## Motivation
This class is inspired by [`concurrent_lru_cache`](https://software.intel.com/en-us/node/506354) from Intel's TBB which has been a preview feature for about 15 years without any active development.
It can't do in-place construction, doesn't support move-semantics and doesn't allow **not** having values for specific keys.

This project aims to fix these issues while keeping similar features.

## License
This application is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

See LICENSE for more information.
