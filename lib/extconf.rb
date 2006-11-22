#!/usr/bin/env ruby

require "mkmf"
CONFIG["CPP"] = "g++ -E "
CONFIG["CC"] = "g++  "


#
# -I/usr/local/include/exiv2
$CPPFLAGS << " -Wall -I/usr/include/exiv2 -I/Users/max/Sites/exif/include/exiv2 "
$LDFLAGS << " -lstdc++ -L/Users/max/Sites/exif/lib "
have_header "exif.hpp"


image_factory_open = "Exiv2::ImageFactory::open"
def image_factory_open.upcase
  "IMGFACTORYOPEN"
end
have_library "exiv2", image_factory_open do  <<-SRC
  #include <image.hpp>
  #include <exif.hpp>


  #include <string>
  #include <vector>
  #include <iostream>
  #include <fstream>
  #include <iomanip>
  #include <cstring>
  #include <cassert>

  #include <stdarg.h>
int main(void) {
  #{image_factory_open}("a");
}
 SRC
end


ifd_tag_list = "Exiv2::ExifTags::ifdTagList"
def ifd_tag_list.upcase
  "IFDTAGLIST"
end
have_func ifd_tag_list do <<-SRC
  #include <image.hpp>
  #include <exif.hpp>


  #include <string>
  #include <vector>
  #include <iostream>
  #include <fstream>
  #include <iomanip>
  #include <cstring>
  #include <cassert>

  #include <stdarg.h>
int main(void) {
  #{ifd_tag_list}();
}
 SRC
 end
create_makefile 'exiv2'

