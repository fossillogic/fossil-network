# ***Fossil Network by Fossil Logic***

Fossil Network is a versatile, platform-independent networking library designed to support both client-side and server-side networking needs. Written in C and C++, it facilitates high-performance, reliable, and scalable networked applications across a wide range of operating systems. The library employs the Meson build system, ensuring a streamlined development and deployment process.

## Key Features

1. **Cross-Platform Compatibility**: Ensures consistent performance and functionality across major operating systems, including macOS and Linux.
2. **Modular Design**: Provides separate client and server APIs that can be easily integrated and customized to meet specific project needs.
3. **Efficient Build System**: Utilizes the Meson build system, known for fast build times and user-friendly syntax, streamlining development.
4. **High Performance**: Implemented in C and C++ to maximize performance and resource efficiency, suitable for both desktop and embedded applications.
5. **Extensive Documentation and Examples**: Includes comprehensive documentation and example code to help developers get started quickly and effectively.

## Client and Server API

Fossil Network is divided into two primary parts: the client-side API and the server-side API.

### Client API

The client API enables network communication from the perspective of a client application. Key functionalities include:

- **Connection Handling**: Functions to establish and manage client-server connections.
- **Data Transmission**: Supports both synchronous and asynchronous data sending and receiving operations.
- **Error Handling**: Provides robust error-checking and recovery mechanisms for network issues.

### Server API

The server API allows applications to handle incoming client requests. Key features include:

- **Connection Management**: Supports handling multiple client connections simultaneously.
- **Data Handling**: Allows efficient data exchange between clients and servers.
- **Concurrency Support**: Integrates with thread-pool mechanisms to ensure high-performance handling of multiple clients.

## Algorithms and Utilities

Fossil Network also provides a variety of algorithms and utility functions to enhance network programming, such as:

- **Packetization**: Functions for efficient data packetization and reassembly.
- **Security**: Support for encryption, decryption, and secure data transmission using modern cryptographic techniques.
- **Serialization**: Utility functions for serializing and deserializing network data.
- **Memory Management**: Custom memory allocation, reallocation, and free functions designed for network applications.

## Prerequisites

Before using Fossil Network, ensure you have the following installed:

- **Meson Build System**: This project requires Meson. If it's not already installed, you can follow the [installation instructions on the Meson website](https://mesonbuild.com/Getting-meson.html).

## Adding Dependency

1. **Install Meson Build System**: Ensure that Meson `1.3` or newer is installed on your system. You can install or upgrade it using pip:

   ```sh
   python -m pip install meson           # to install Meson
   python -m pip install --upgrade meson # to upgrade Meson
   ```

2. **Adding Wrap File**: Navigate to the `subprojects` directory and create a `.wrap` file, such as `fossil-network.wrap`, with the following content:

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

3. **Integrate the New Dependency**: To integrate the dependency into your Meson project, add the following line to your `meson.build` file:

   ```ini
   dep = dependency('fossil-network')
   ```

   This line imports the `fossil-network` dependency, making it available for use in your project.

## Configure Options

Fossil Network offers configurable options to tailor the build process to your needs:

- **Running Tests**: To enable testing, configure the build with `-Dwith_test=enabled`.

Example:

```sh
meson setup builddir -Dwith_test=enabled
```

## Contributing and Support

For those interested in contributing, reporting issues, or seeking support, please open an issue on the project repository or visit the [Fossil Logic Docs](https://fossillogic.com/docs) for more information. Your feedback and contributions are always welcome.
