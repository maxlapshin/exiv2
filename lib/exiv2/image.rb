module Exiv2
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
    
    
    
    
    
    inline do |builder|
      Exiv2.prepare_builder(builder)
      builder.prefix <<-PREFIX
      VALUE mExiv2, cImage, cExif, cTag, cIptc, cThumbnail, eError;
      

      static void image_real_save(rbImage* image) {
      	if(image->dirty) {
      		image->image->writeMetadata();
      		image->dirty = false;
      	}
      }

      static void image_leave(rbImage* image) {
      }
      
      static void image_delete(rbImage* image) {
      	__BEGIN
      	image_real_save(image);
      	delete image;
      	__VOID_END
      }
      PREFIX
      
      builder.c_singleton <<-EOF, :method_name => "new"
      static VALUE exiv2_image_alloc_and_initialize(VALUE file) {
      	__BEGIN
      	rbImage* image = new rbImage();
      	VALUE rb_image = Data_Wrap_Struct(self, 0, image_delete, image);
      	image->dirty = false;


      	try {
      		if(!strcmp(rb_class2name(rb_class_of(file)), "StringIO")) {
      			rb_iv_set(rb_image, "@content", file);
      			VALUE string = rb_funcall(file, rb_intern("string"), 0);
      			image->image = Exiv2::ImageFactory::open(CBSTR(string), LEN(string));
      		} else if(rb_respond_to(file, rb_intern("read"))) {
      			VALUE file_content = rb_funcall(file, rb_intern("read"), 0);
      			rb_iv_set(rb_image, "@file_content", file_content);
      			image->image = Exiv2::ImageFactory::open(CBSTR(file_content), LEN(file_content));
      		} else if(TYPE(file) == T_STRING) {
      			image->image = Exiv2::ImageFactory::open(CSTR(file));
      		}
      		image->image->readMetadata();
      	}
      	catch(const Exiv2::AnyError&) {
      		rb_raise(eError, "Cannot open file %s", STR(file));
          }	
      	return rb_image;
      	__END
      }
      EOF
      
      builder.c_singleton <<-EOF, :method_name => "load_string"
      static VALUE exiv2_image_load_string(VALUE string) {
      	__BEGIN
      	Check_Type(string, T_STRING);
      	rbImage* image = new rbImage();
      	image->dirty = false;
      	VALUE img = Data_Wrap_Struct(self, 0, image_delete, image);

      	image->image = Exiv2::ImageFactory::open(CBSTR(string), LEN(string));
      	return img;
      	__END
      }
      EOF
      
      builder.c <<-EOF, :method_name => "save"
      static VALUE exiv2_image_save() {
      	__BEGIN
      	rbImage* image;
      	Data_Get_Struct(self, rbImage, image);
      	image_real_save(image);
      	return self;
      	__END
      }
      EOF
      
      builder.c <<-EOF, :method_name => "clear"
      static VALUE exiv2_image_clear() {
      	__BEGIN
      	rbImage* image;
      	Data_Get_Struct(self, rbImage, image);
      	image->image->clearMetadata();
      	image->dirty = true;
      	return self;
      	__END
      }
      EOF
      
      builder.c <<-EOF, :method_name => "exif"
      static VALUE exiv2_image_exif() {
      	__BEGIN
      	rbImage* image;
      	VALUE cExif = rb_path2class("Exiv2::Exif");
      	Data_Get_Struct(self, rbImage, image);
      	VALUE exif = Data_Wrap_Struct(cExif, 0, image_leave, image);
      	rb_iv_set(exif, "@image", self);
      	return exif;
      	__END
      }
      EOF
    end
  end
end