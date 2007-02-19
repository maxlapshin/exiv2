begin
  require File.dirname(__FILE__)+'/exiv2_bin'
rescue LoadError
  require File.dirname(__FILE__)+'/../ext/exiv2_bin'
end
