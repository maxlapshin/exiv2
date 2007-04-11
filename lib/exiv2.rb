begin
  require File.dirname(__FILE__)+'/exiv2_bin'
rescue LoadError
  require File.dirname(__FILE__)+'/../ext/exiv2_bin'
end

module Exiv2
  class Image
    
    def [](key)
      return exif[key] if key[0...4] == "Exif"
      return iptc[key] if key[0...4] == "Iptc"
    end
    
    def []=(key, value)
      return exif[key] = value if key[0...4] == "Exif"
      return iptc[key] = value if key[0...4] == "Iptc"
      raise Exiv2::Error, "Unknown key for writing: #{key.inspect}"
    end
    
    def created_at
      date = iptc["Iptc.Application2.DateCreated"]
      time = iptc["Iptc.Application2.TimeCreated"]
      return date unless time
      Time.utc(date.year, date.month, date.day, time.hour, time.min, time.sec)
    end
  end
end