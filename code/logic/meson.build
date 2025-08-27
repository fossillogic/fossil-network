dir = include_directories('.')
cc = meson.get_compiler('c')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'c')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'cpp')

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
                   bcrypt_dep],
    include_directories: dir
)

# ------------------------------
# Fossil Network dependency object
# ------------------------------
fossil_network_dep = declare_dependency(
    link_with: [fossil_network_lib],
    include_directories: dir
)