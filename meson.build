project('vmem', 'c', default_options: ['c_std=c89', 'warning_level=3', 'werror=true'])

srcs = files('src/vmem.c', 'src/main.c')
inc = include_directories('src')

executable('vmem', srcs, include_directories: inc)
