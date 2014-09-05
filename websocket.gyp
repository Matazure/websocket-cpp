{
    'variables':{
        'BOOST_INCLUDE_DIR%': '/usr/local/include',
        'BOOST_LIBRARY_DIR%': '/usr/local/lib',
    },

    'includes': ['common.gypi',],
    'targets':[
        {
            'target_name' : 'websocket',
            'type' : 'static_library',
            'dependencies': ['./websocket/vendors/http-parser/http_parser.gyp:http_parser'],
            'include_dirs':['.', '<(BOOST_INCLUDE_DIR)'],
            'sources' : ['./websocket/socket.cpp', './websocket/server.cpp'],
            # 'product_dir':'./lib',

            'all_dependent_settings':{
                'include_dirs': [ '.', '<(BOOST_INCLUDE_DIR)' ],
            },

            'conditions':[
                ['OS=="mac"',{
                    'xcode_settings':{
                        'LIBRARY_SEARCH_PATHS':['<(BOOST_LIBRARY_DIR)',],
                    },
                    'link_settings':{
                        'libraries':['libboost_system.a',]
                    },
                    'all_dependent_settings':{
                        'xcode_settings':{
                            'LIBRARY_SEARCH_PATHS':['<(BOOST_LIBRARY_DIR)',],
                        },
                        'link_settings':{
                            'libraries':['libboost_system.a',]
                        },
                    }
                }],
                ['OS=="linux"', {
                    'link_settings':{
                        'library_dirs':['<(BOOST_LIBRARY_DIR)'],
                        'libraries':['-lboost_system','-lpthread',],
                    },
                    'all_dependent_settings':{
                        'link_settings':{
                            'library_dirs':['<(BOOST_LIBRARY_DIR)'],
                            'libraries':['-lboost_system', '-lpthread',],
                        },
                    },
                }],
            ],
        },
    ]
}
