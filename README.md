# **Fossil Network by Fossil Logic**

**Fossil Network** is a portable, cross-platform C library for sockets and networking, designed for efficiency, reliability, and zero external dependencies. It uses string-based type IDs (e.g., `tcp`, `ipv4`) for socket types, address families, and options, providing a modern, unified API for Windows, macOS, BSD, and Linux.

### Key Features

- **String-Based Type and Option IDs**  
  Socket types (`tcp`, `udp`, `raw`), address families (`ipv4`, `ipv6`) are referenced by string IDs for clarity and extensibility.

- **Unified Core Structures**  
  Core types like `fossil_net_socket_t`, `fossil_net_address_t`, and `fossil_net_mac_t` abstract platform-specific details, including socket handles, IDs, types, families, and MAC addresses.

- **Simple Lifecycle Management**  
  Explicit initialization and shutdown functions (`fossil_net_socket_init`, `fossil_net_socket_shutdown`) ensure proper setup and cleanup across platforms.

- **Comprehensive Socket Operations**  
  Functions for creating, binding, listening, accepting, connecting, sending, receiving, and closing sockets, all using the unified string-based API.

- **Blocking and Non-Blocking Modes**  
  Easily toggle blocking behavior per socket with `fossil_net_socket_set_blocking`.

- **Address and Host Utilities**  
  Parse and format addresses, resolve hostnames, retrieve local hostnames, and handle MAC addresses with dedicated helpers.

- **Polling and Multiplexing**  
  Poll multiple sockets for readiness with a single call, supporting scalable I/O.

- **Consistent Error Handling**  
  Retrieve and describe socket errors in a platform-agnostic way.

- **Minimal Footprint**  
  No external dependencies, suitable for embedded, desktop, and server environments.

**Fossil Network** streamlines cross-platform socket programming by abstracting OS quirks and exposing a consistent, extensible C API for robust networked applications.

---

### ***Prerequisites***

To get started, ensure you have the following installed:

- **Meson Build System**: If you don’t have Meson `1.10.0` or newer installed, follow the installation instructions on the official [Meson website](https://mesonbuild.com/Getting-meson.html).

### Adding Dependency

#### Adding via Meson Git Wrap

To add a git-wrap, place a `.wrap` file in `subprojects` with the Git repo URL and revision, then use `dependency('fossil-network')` in `meson.build` so Meson can fetch and build it automatically.

#### Integrate the Dependency:

Add the `fossil-network.wrap` file in your `subprojects` directory and include the following content:

```ini
[wrap-git]
url = https://github.com/fossillogic/fossil-network.git
revision = v0.1.4

[provide]
dependency_names = fossil-network
```

**Note**: For the best experience, always use the latest releases. Visit the [releases](https://github.com/fossillogic/fossil-network/releases) page for the latest versions.

## Build Configuration Options

Customize your build with the following Meson options:
	•	Enable Tests
To run the built-in test suite, configure Meson with:

```sh
meson setup builddir -Dwith_test=enabled
```

## Contributing and Support

For those interested in contributing, reporting issues, or seeking support, please open an issue on the project repository or visit the [Fossil Logic Docs](https://fossillogic.com/docs) for more information. Your feedback and contributions are always welcome.
