#!/usr/bin/env ruby

require "mkmf"
#CONFIG["CXX"] = "g++ -g " + CONFIG['LDSHARED']
CONFIG["CPP"] = "g++ -E "
#CONFIG['LDSHARED'] = "g++ -g  -dynamic -bundle -undefined suppress -flat_namespace"



$CPPFLAGS << " -Wall -I/usr/local/include/exiv2 -I/usr/include/exiv2 "
$LDFLAGS << " -lstdc++ "
have_header "exif.hpp"
have_library "exiv2", "_ZN5Exiv29MetadatumD0Ev"
create_makefile 'exiv2'

