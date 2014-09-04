{
    'target_defaults': {
        'default_configuration': 'Debug',
        'configurations': {
            'Debug': {
                'defines': [ 'DEBUG', '_DEBUG' ],
                'cflags': ['-std=c++11', '-Wall', '-Wextra', '-O0', '-g', '-ftrapv' ],
                'xcode_settings':{
                    'OTHER_CPLUSPLUSFLAGS':['-O0'],
                    'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
                    'CLANG_CXX_LIBRARY': 'libc++',
                    'CONFIGURATION_BUILD_DIR': './',
                },
            },
            'Release': {
                'defines': [ 'NDEBUG', 'RELEASE' ],
                'cflags': ['-std=c++11', '-Wall', '-Wextra', '-O3', '-s',],
                'xcode_settings':{
                    'OTHER_CPLUSPLUSFLAGS':['-Os'],
                    'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
                    'CLANG_CXX_LIBRARY': 'libc++',
                    'CONFIGURATION_BUILD_DIR': './',
                },
            },
        },
    },
}