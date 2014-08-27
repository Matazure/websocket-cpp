{
    'target_defaults': {
      'default_configuration': 'Debug',
      'configurations': {
        # TODO: hoist these out and put them somewhere common, because
        #       RuntimeLibrary MUST MATCH across the entire project
        'Debug': {
          'defines': [ 'DEBUG', '_DEBUG' ],
          'cflags': [ '-Wall', '-Wextra', '-O0', '-g', '-ftrapv' ],
          'xcode_settings':{
              'OTHER_CPLUSPLUSFLAGS':['-O0']
          },
          'msvs_settings': {
            'VCCLCompilerTool': {
              'RuntimeLibrary': 1, # static debug
            },
          },
        },
        'Release': {
          'defines': [ 'NDEBUG' ],
          'cflags': [ '-Wall', '-Wextra', '-O3' ],
          'xcode_settings':{
              'OTHER_CPLUSPLUSFLAGS':['-Os']
          },
          'msvs_settings': {
            'VCCLCompilerTool': {
              'RuntimeLibrary': 0, # static release
            },
          },
        }
      },
      'msvs_settings': {
        'VCCLCompilerTool': {
        },
        'VCLibrarianTool': {
        },
        'VCLinkerTool': {
          'GenerateDebugInformation': 'true',
        },
      },
      'conditions': [
        ['OS == "win"', {
          'defines': [
            'WIN32'
          ],
        }]
      ],
    },

    'targets':[
        {
            'target_name' : 'websocket',
            'type' : 'static_library',
            'dependencies': ['./websocket/http-parser/http_parser.gyp:http_parser'],
            'include_dirs':['.', '/usr/local/include'],
            'sources' : ['./websocket/socket.cpp', './websocket/server.cpp'],
            # 'product_dir':'./lib',
            'conditions':[
                ['OS=="mac"',{
                    'xcode_settings':{
                        'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
                        'CLANG_CXX_LIBRARY': 'libc++',
                        'CONFIGURATION_BUILD_DIR': './',
                        'LIBRARY_SEARCH_PATHS':['/usr/local/lib',],
                        'OTHER_CPLUSPLUSFLAGS':['-ftemplate-depth=512'],
                    },
                    'link_settings':{
                        'libraries':[
                            'libboost_system.a',
                        ]
                    },
                    'all_dependent_settings':
                    {
                        'xcode_settings':{
                            'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
                            'CLANG_CXX_LIBRARY': 'libc++',
                            'CONFIGURATION_BUILD_DIR': './',
                            'LIBRARY_SEARCH_PATHS':['/usr/local/lib',],
                            'OTHER_CPLUSPLUSFLAGS':['-ftemplate-depth=512'],
                        },
                        'link_settings':{
                            'libraries':[
                                'libboost_system.a',
                            ]
                        },
                    }
                }],
                ['OS=="linux"', {
                    'cflags': ['-std=c++11','-lgcc-eh', '-fexceptions', '-frtti'],
                    'cflags!': ['-fno-exceptions', '-fno-rtti'],
                    'cflags_cc!': ['-fno-exceptions', '-fno-rtti'],

                    'link_settings':{
                        'library_dirs':['/usr/local/lib'],
                        'libraries':[
                            '-lboost_system',
                        ],
                    },
                    'all_dependent_settings':
                    {
                        'cflags': ['-std=c++11','-lgcc-eh', '-fexceptions', '-frtti'],
                        'cflags!': ['-fno-exceptions', '-fno-rtti'],
                        'cflags_cc!': ['-fno-exceptions', '-fno-rtti'],

                        'link_settings':{
                            'library_dirs':['/usr/local/lib'],
                            'libraries':[
                                '-lboost_system',
                            ],
                        },
                    },
                }],
            ],
        },
    ]
}
