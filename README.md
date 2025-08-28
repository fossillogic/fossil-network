# **Fossil Network by Fossil Logic**

**Fossil Network** is a lightweight, cross-platform sockets library written in pure C with zero external dependencies. Focused on efficient socket management, it enables high-performance applications and embedded systems to communicate over TCP, UDP, and multicast. With abstractions for non-blocking I/O and event-driven networking, Fossil Network simplifies building reliable, socket-based software while maintaining a minimal footprint.

### Key Features
- **Cross-Platform Support**  
  Works seamlessly on Windows, macOS, Linux, and embedded platforms.

- **Zero External Dependencies**  
  Pure C implementation for maximum portability, auditability, and ease of integration.

- **Multi-Protocol Support**  
  TCP, UDP, and multicast connections for versatile networking needs.

- **Non-Blocking & Event-Driven I/O**  
  Supports timeouts, event callbacks, and efficient handling of multiple connections.

- **Lightweight and Efficient**  
  Optimized for minimal resource usage in embedded or resource-constrained environments.

- **Modular Design**  
  Easily integrated into existing projects or extended for custom protocols.

## Getting Started

### Prerequisites

- **Meson Build System**  
  Fossil Network uses Meson for build configuration. If you don’t have Meson installed, follow the instructions on the official [Meson website](https://mesonbuild.com/Getting-meson.html).

### Adding Dependency

#### Adding via Meson Git Wrap

To add a git-wrap, place a `.wrap` file in `subprojects` with the Git repo URL and revision, then use `dependency('fossil-network')` in `meson.build` so Meson can fetch and build it automatically.

#### Adding via Conan GitHub repository

 packages directly from a GitHub repository if it contains a valid `conanfile.py`.

```bash
conan install git+https://github.com/fossillogic/fossil-network.git#v0.1.0 --name fossil_network --build=missing
```

#### Integrate the Dependency:

Add the `fossil-network.wrap` file in your `subprojects` directory and include the following content:

```ini
[wrap-git]
url = https://github.com/fossillogic/fossil-network.git
revision = v0.1.0

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