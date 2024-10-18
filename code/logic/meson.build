dir = include_directories('.')

fossil_network_lib = library('fossil-network',
    files('server.c'),
    install: true,
    include_directories: dir)

fossil_network_dep = declare_dependency(
    link_with: [fossil_network_lib],
    include_directories: dir)
