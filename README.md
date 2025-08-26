# **Fossil Network by Fossil Logic**

**Fossil Network** is a lightweight, cross-platform sockets library written in pure C with zero external dependencies. Focused on efficient socket management, it enables high-performance applications and embedded systems to communicate over TCP, UDP, and multicast. With abstractions for non-blocking I/O and event-driven networking, Fossil Network simplifies building reliable, socket-based software while maintaining a minimal footprint.

### Key Features

- **Cross-Platform Support**  
  Works seamlessly on Windows, macOS, Linux, and embedded platforms.

- **Zero External Dependencies**  
  Pure C implementation for maximum portability, auditability, and ease of integration.

- **Multi-Protocol Support**  
  TCP, UDP, multicast, and TLS-ready connections for versatile networking needs.

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

### Adding Fossil Network as a Dependency

#### Using Meson

### **Install or Upgrade Meson** (version 1.3 or newer recommended):

```sh
   python -m pip install meson           # Install Meson
   python -m pip install --upgrade meson # Upgrade Meson
```
###	Add the .wrap File
Place a file named fossil-network.wrap in your subprojects directory with the following content:

```ini
# ======================
# Git Wrap package definition
# ======================
[wrap-git]
url = https://github.com/fossillogic/fossil-network.git
revision = v0.1.0

[provide]
fossil-network = fossil_network_dep
```

###	Integrate in Your meson.build
Add the dependency by including this line:

```meson
network_dep = dependency('fossil-network')
```


## Build Configuration Options

Customize your build with the following Meson options:
	•	Enable Tests
To run the built-in test suite, configure Meson with:

```sh
meson setup builddir -Dwith_test=enabled
```

## Contributing and Support

For those interested in contributing, reporting issues, or seeking support, please open an issue on the project repository or visit the [Fossil Logic Docs](https://fossillogic.com/docs) for more information. Your feedback and contributions are always welcome.