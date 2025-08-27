dir = include_directories('.')
cc = meson.get_compiler('c')

# ------------------------------
# Platform-specific dependencies
# ------------------------------
if host_machine.system() == 'windows'
    winsock_dep = cc.find_library('ws2_32', required: true)
    bcrypt_dep  = cc.find_library('bcrypt', required: true)
else
    winsock_dep = []
    bcrypt_dep  = []
endif

# ------------------------------
# OpenSSL dependency
# ------------------------------
openssl_dep = dependency('openssl', required: true, method: 'pkg-config',
    fallback: ['openssl', 'openssl_dep'])

# ------------------------------
# Fossil Network static library
# ------------------------------
fossil_network_lib = static_library(
    'fossil-network',
    files(
        'socket.c', 'udp.c', 'tcp.c', 'http.c',
        'dns.c', 'cluster.c', 'async.c',
        'echo.c', 'ws.c', 'tls.c', 'protocol.c'
    ),
    install: true,
    dependencies: [cc.find_library('m', required: false),
                   winsock_dep,
                   bcrypt_dep,
                   openssl_dep],
    include_directories: dir
)

# ------------------------------
# Fossil Network dependency object
# ------------------------------
fossil_network_dep = declare_dependency(
    link_with: [fossil_network_lib],
    dependencies: [winsock_dep, bcrypt_dep, openssl_dep],
    include_directories: dir
)