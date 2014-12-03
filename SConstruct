env = Environment()

env.Append(CPPPATH = ['/usr/include/'])

env.Append(CCFLAGS = ['-g', '-std=c++11', '-pthread'])

env.Append(LIBPATH = ['/usr/lib/'])

env.Append(LIBS = [ 'pthread' ])

env.Append(LINKFLAGS = ['-Wl,--no-as-needed'])

t = env.Program(target='main', source=['./openmplike.cpp', './main.cpp'])

Default(t)
