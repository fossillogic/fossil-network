if get_option('with_test').enabled()
    run_command(['python3', 'tools' / 'generate-runner.py'], check: true)

    test_src = ['unit_runner.c']
    test_cases = [
        'server', 'client'
    ]

    foreach case : test_cases
        test_src += ['test_' + case + '.c']
    endforeach

    pizza = executable('runner', test_src,
        include_directories: dir,
        dependencies: [
            dependency('fossil-test'),
            dependency('fossil-mock'),
            fossil_network_dep])

    test('xunit_tests', pizza)  # Renamed the test target for clarity
endif
