project('vmem', 'c', default_options: ['c_std=c89', 'warning_level=3', 'werror=true'])

cmocka = dependency('cmocka')

srcs = files('src/vmem.c', 'src/main.c', 'src/test.c')
inc = include_directories('src')

executable('vmem', srcs, include_directories: inc, dependencies: cmocka)
