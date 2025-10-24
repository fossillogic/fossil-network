# **Fossil Network by Fossil Logic**

**Fossil Network** is a portable, cross-platform sockets library in pure C, designed for efficient and reliable network communication with zero external dependencies. It provides unified type definitions and abstractions for Windows, macOS, BSD, and Linux, ensuring seamless integration and consistent behavior across platforms.

### Key Features

- **Portable Type Definitions**  
  Handles platform-specific differences (Windows, macOS/BSD, Linux) for socket types, error codes, and system headers.

- **Unified Socket API**  
  Core structures (`fossil_network_socket_t`, `fossil_network_pollfd_t`) abstract file descriptors and socket operations for all supported platforms.

- **Comprehensive Protocol Support**  
  Built-in enumeration for TCP, UDP, raw sockets, and common application protocols (HTTP, HTTPS, FTP, SSH, DNS, etc.).

- **Convenient Socket Operations**  
  Functions for socket creation, binding, listening, accepting, connecting, sending, receiving, polling, and shutdown.

- **Cross-Platform Error Handling**  
  Normalized error codes and translation helpers for consistent error reporting regardless of OS.

- **Timeouts and Non-Blocking I/O**  
  Easily set timeouts and toggle blocking modes for sockets, with helpers for event-driven and asynchronous networking.

- **Address and Option Utilities**  
  Helpers for hostname resolution, address retrieval, and socket option management.

- **Datagram Support**  
  Send and receive UDP/raw datagrams with address/port helpers.

- **Minimal Footprint**  
  No external dependencies; suitable for embedded, desktop, and server environments.

**Fossil Network** simplifies socket programming by abstracting platform quirks and providing a consistent, modern C API for building robust networked applications.

To get started, ensure you have the following installed:

- **Meson Build System**: If you don’t have Meson `1.8.0` or newer installed, follow the installation instructions on the official [Meson website](https://mesonbuild.com/Getting-meson.html).

### Adding Dependency

#### Adding via Meson Git Wrap

To add a git-wrap, place a `.wrap` file in `subprojects` with the Git repo URL and revision, then use `dependency('fossil-network')` in `meson.build` so Meson can fetch and build it automatically.

#### Integrate the Dependency:

Add the `fossil-network.wrap` file in your `subprojects` directory and include the following content:

```ini
[wrap-git]
url = https://github.com/fossillogic/fossil-network.git
revision = v0.1.1

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