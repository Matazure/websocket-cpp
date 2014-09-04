{
    'includes': ['common.gypi',],
    'targets':[
        {
            'target_name' : 'websocket',
            'type' : 'static_library',
            'dependencies': ['./websocket/vendors/http-parser/http_parser.gyp:http_parser'],
            'include_dirs':['.', '/usr/local/include'],
            'sources' : ['./websocket/socket.cpp', './websocket/server.cpp'],
            # 'product_dir':'./lib',

            'direct_dependent_settings':{
                'include_dirs': [ '.', '/usr/local/include' ],
            },

            'conditions':[
                ['OS=="mac"',{
                    'xcode_settings':{
                        'LIBRARY_SEARCH_PATHS':['/usr/local/lib',],
                    },
                    'link_settings':{
                        'libraries':['libboost_system.a',]
                    },
                    'direct_dependent_settings':{
                        'xcode_settings':{
                            'LIBRARY_SEARCH_PATHS':['/usr/local/lib',],
                        },
                        'link_settings':{
                            'libraries':['libboost_system.a',]
                        },
                    }
                }],
                ['OS=="linux"', {
                    'link_settings':{
                        'library_dirs':['/usr/local/lib'],
                        'libraries':['-lboost_system','-lpthread',],
                    },
                    'direct_dependent_settings':{
                        'link_settings':{
                            'library_dirs':['/usr/local/lib'],
                            'libraries':['-lboost_system', '-lpthread',],
                        },
                    },
                }],
            ],
        },
    ]
}
