require 'enumerator'
require 'rubygems'
require 'inline'
require 'active_support'
require 'erb'

module Exiv2
  def self.prepare_builder(builder)
    Config::CONFIG["CPP"] = "g++ -E "
    Config::CONFIG["CC"] = "g++  "
    Config::CONFIG["LDSHARED"].gsub!(/^cc /,"g++ ")
    builder.include "<exiv2.hpp>"
    builder.add_compile_flags "-x c++ -I#{File.dirname(__FILE__)} -I/usr/local/include/exiv2", '-lstdc++ -lexiv2'
    builder.include_ruby_last
  end
  
  def self.load(&block)
    ERB.new(File.read(File.dirname(__FILE__)+"/exiv2/" + block.call)).result(block.binding)
  end
end

require 'exiv2/image'
require 'exiv2/exif'
require 'exiv2/iptc'
require 'exiv2/tag'

module Exiv2
  class Error < StandardError; end
  
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