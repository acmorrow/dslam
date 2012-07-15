dslam
=====

A toy express/sinatra style framework for writing webservices in C++11.

DSLAM is a C++11 implementation of the sinatra/express idea for
writing route based web services. It was written for a presentation to
the [NYC C++ Meetup](http://www.meetup.com/nyccpp/) on C++11 features
in practice. There was a great series of earlier group meetups where
we reviewed the new features in C++11 and its standard
library. However, hearing about a feature in a lecture doesn't always
help you do anything with it, or to see how it integrates with other
features, etc. So I wrote DSLAM to play around with C++11 features,
and then presented it to the meetup so that we could talk about those
features in the context of "real" code. People after the meetup asked
me to put it up on github, so here it is!

Because this was done specifically to experiment with C++11 features,
there are several places where C++11 features are used even though
they aren't stricly necessary, or perhaps even advisable. I'm using
the 'using' syntax to declare all typedefs, even when that is not
needed (no templates involved). All functions are declared with late
return types as well. I'm not necessarily recommending this, but it
does lead to interesting looking code.

If you want to build DSLAM, there are several annoying prerequisites:

- A working C++11 compiler. Also, a unicorn. (j/k. I'm actually really
  impressed with how far along GCC and clang are w.r.t C++11 support,
  especially when compared to how long it took to get real world
  compilers to support C++98 after that standard was released).

- Boost 1.50.0 or greater, plus the fix for
  https://svn.boost.org/trac/boost/ticket/2792 if it is not fixed in
  your boost version. You will want to compile boost with
  -std=c++11.

- My C++11 port of the PION network library. You can get it from the
  c++11-port branch of my [github fork of
  PION](https://github.com/acmorrow/pion-core/tree/c++11-port). You
  will also need any dependencies of PION, which is mostly boost. You
  will need to configure it with -std=c++11 in the CXXFLAGS for it to
  build.

- google-test, again compiled with -std=c++11. At least for me, with
  clang -std=c++11 I needed to give it -DGTEST_HAS_TR1_TUPLE=0
  -DGTEST_USE_OWN_TR1_TUPLE.

There are also several slightly less annoying dependencies. As long as
you have these in reasonable places, things should just work. I
developed DSLAM on Mac OS X Lion, so I obtained these via MacPorts:

- CMake
- OpenSSL (Or just tell PION not to use it)

If you are on Mac OS X and you are using macports, you can make your
life somewhat easier by hacking up portconfigure.tcl and adding
"-std=c++11" to your configure.cxxflags. That way you can build boost
and google-test via "port -s install" and get them properly
C++11-ified. You can drop the boost patch into your /opt/local/include
directory after the build; it doesn't seem to be used by any libraries
so this should be harmless. In addition, if you are on Mac OS X, you
can really live on the edge and add -stdlib=libc++ to your
configure.cxxflags as well. If you do that, remember to definitely
pass that flag to your builds of PION and DSLAM as well.

Once you have all of that in place you should be able to generate
files for your preferred build system like so:

```bash
C=<path-to-c-compiler> CXX=<path-to-c++11-compiler> CXXFLAGS="-std=c++11" cmake
-DCMAKE_BUILD_TYPE=<CMakeBuildVariant>
-DCMAKE_PREFIX_PATH=<PIONInstallPath>
-DCMAKE_INSTALL_PREFIX=<DSLAMInstallPath>
../..
```

Note that you need to pass ```-std=c++11``` here as well: CMake doesn't
currently have a portable way of enabling C++11 mode, and I haven't
tried to produce one.

Once cmake generates, a `make all test install` should get you a
compiled DSLAM if you are using the Unix Makefile generator. If you
are using something else I presume you know what to do.

There are a few unit tests, but mostly just to ensure that google-test
and CTest are working. I plan to write more that actually test out the
service framework.

I wrote DSLAM on a Mac OS X Lion machine. I haven't tried building it
on Linux yet, but I plan to do that when I have some time.

This code is in no way production quality and shouldn't be used for
anything.
