Gem::Specification.new do |s|
  s.name = 'exiv2'
  s.version = '1.5'
  s.date    = '2008-11-02'
  s.summary = 'wrapper for Andreas Huggel exiv2 library for working with exif data'
  s.email   = "max@maxidoors.ru"
  s.homepage= "http://github.com/maxlapshin/exiv2"
  s.author  = "Max Lapshin"
  s.rubyforge_project = "exiv2"
  s.rdoc_options = ["--main", "README"]
  s.extra_rdoc_files = ["README"]
  s.has_rdoc          = false
  s.files = ["init.rb",
             "exiv2.gemspec",
             "Rakefile",
             "lib/exiv2.rb",
             "lib/exiv2.hpp",
             "lib/exiv2/exif.rb",
             "lib/exiv2/image.rb",
             "lib/exiv2/iptc.rb",
             "lib/exiv2/tag.rb",
             "lib/exiv2/throw.cpp",
             "lib/exiv2/metadata/clear.cpp",
             "lib/exiv2/metadata/count.cpp",
             "lib/exiv2/metadata/delete.cpp",
             "lib/exiv2/metadata/each.cpp",
             "lib/exiv2/metadata/get.cpp",
             "lib/exiv2/metadata/is_empty.cpp",
             "lib/exiv2/metadata/marshall.cpp",
             "lib/exiv2/metadata/set.cpp",
             "lib/exiv2/metadata/unmarshall.cpp",
             "test/test_image.rb",
             "test/data/RIA3001.jpg",
             "test/data/gps-test.jpg",
             "test/data/exiv2-fujifilm-finepix-s2pro.jpg",
             "test/data/smiley1.jpg",
             "README"
             ]
end

