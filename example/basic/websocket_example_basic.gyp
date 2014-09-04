{
    'includes': ['../../common.gypi',],
    'targets':[
        {
            'target_name' : 'client',
            'type' : 'executable',
            'dependencies': ['../../websocket.gyp:websocket'],
            'include_dirs':['../..', '/usr/local/include'],
            'sources' : ['client.cpp'],
            'product_dir':'./',

        },

        {
            'target_name' : 'server',
            'type' : 'executable',
            'dependencies': ['../../websocket.gyp:websocket'],
            'include_dirs':['../..', '/usr/local/include'],
            'sources' : ['server.cpp'],
            'product_dir':'./',
        },
    ]
}
