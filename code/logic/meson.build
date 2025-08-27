dir = include_directories('.')
cc = meson.get_compiler('c')

# Check if the host system is Windows
if host_machine.system() == 'windows'
    winsock_dep = cc.find_library('ws2_32', required: true)
else
    winsock_dep = []
endif

fossil_network_lib = static_library('fossil-network',
    files('socket.c', 'udp.c', 'tcp.c', 'http.c', 'dns.c', 'cluster.c', 'async.c', 'echo.c', 'ws.c', 'tls.c'),
    install: true,
    dependencies: [cc.find_library('m', required: false), winsock_dep],
    include_directories: dir
)

fossil_network_dep = declare_dependency(
    link_with: [fossil_network_lib],
    dependencies: [winsock_dep],
    include_directories: dir
)
