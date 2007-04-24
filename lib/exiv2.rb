begin
  require File.dirname(__FILE__)+'/exiv2_bin'
rescue LoadError
  require File.dirname(__FILE__)+'/../ext/exiv2_bin'
end
require 'enumerator'
require 'rubygems'
require 'active_support'

module Exiv2
  #
  class Image
    
    # method [] is used to read exif or iptc data
    def [](key)
      return exif[key] if key[0...4] == "Exif"
      return iptc[key] if key[0...4] == "Iptc"
    end
    
    # method []= is used to set exif or iptc data
    def []=(key, value)
      return exif[key] = value if key[0...4] == "Exif"
      return iptc[key] = value if key[0...4] == "Iptc"
      raise Exiv2::Error, "Unknown key for writing: #{key.inspect}"
    end
    
    # clues together Iptc.Application2.DateCreated and Iptc.Application2.TimeCreated if possible
    def created_at
      date = iptc["Iptc.Application2.DateCreated"]
      time = iptc["Iptc.Application2.TimeCreated"]
      return date unless time
      Time.utc(date.year, date.month, date.day, time.hour, time.min, time.sec)
    end
  end
  
  class SubTagAccess
    def initialize(class_name, sub_tag, parent)
      @class_name = class_name.split("::").last
      @sub_tag = sub_tag
      @parent = parent
    end
    
    def methods
      @parent.select {|tag| tag.first.starts_with?("#{@class_name}.#{@sub_tag}")}.map {|tag| tag.first.gsub(/^(\w+)\.(\w+)\.(\w+)$/,'\3')}
    end
    
    def method_missing(name, *args)
      method_name = name.to_s
      method_name = method_name[0..-2] if method_name.ends_with?("=")
      return super(name, *args) unless methods.include?(method_name)
      if name.to_s.ends_with?("=")
        @parent.send :[]=, "#{@class_name}.#{@sub_tag}.#{method_name}", *args
      else
        @parent.send :[], "#{@class_name}.#{@sub_tag}.#{method_name}", *args
      end
    end
  end
  
  module ExifAccess
    include Enumerable
    def methods
      virtual_methods + real_methods
    end
    def method_missing(name, *args)
      super(name, *args) unless virtual_methods.include?(name.to_s)
      SubTagAccess.new(self.class.to_s, name, self)
    end
    private
    def virtual_methods
      self.map {|tag| tag.first.gsub(/^(\w+)\.(\w+)\.(\w+)$/,'\2')}.uniq
    end
  end
  
  class Exif
    alias :real_methods :methods
    include ExifAccess
  end
end