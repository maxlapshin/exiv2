#!/usr/bin/env ruby

require "mkmf"
CONFIG["CPP"] = "g++ -E "
CONFIG["CC"] = "g++ -c "


#
# -I/usr/local/include/exiv2
$CPPFLAGS << " -Wall -I/usr/include/exiv2 -I/Users/max/Sites/exif/include/exiv2 "
$LDFLAGS << " -lstdc++ -L/Users/max/Sites/exif/lib "
have_header "exif.hpp"
have_library "exiv2", "_ZN5Exiv29MetadatumD0Ev"


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
  Exiv2::ExifTags::ifdTagList();
}
 SRC
 end
create_makefile 'exiv2'

