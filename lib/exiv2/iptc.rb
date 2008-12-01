module Exiv2
  class Iptc
    inline do |builder|
      kind = "Iptc"
      Exiv2::prepare_builder(builder)
      builder.prefix(Exiv2::load {"metadata/marshall.cpp"})
      builder.prefix(Exiv2::load {"metadata/unmarshall.cpp"})
      builder.prefix(Exiv2::load {"throw.cpp"})

      builder.c(Exiv2::load {"metadata/get.cpp"}, {:method_name => "[]"})
      builder.c(Exiv2::load {"metadata/set.cpp"}, {:method_name => "[]="})
      builder.c_raw(Exiv2::load {"metadata/each.cpp"}, {:method_name => "each"})
      builder.c(Exiv2::load {"metadata/delete.cpp"}, {:method_name => "delete"})
      builder.c(Exiv2::load {"metadata/clear.cpp"}, {:method_name => "clear"})
      builder.c(Exiv2::load {"metadata/count.cpp"}, {:method_name => "count"})
      builder.c(Exiv2::load {"metadata/is_empty.cpp"}, {:method_name => "empty?"})
    end
  end
end