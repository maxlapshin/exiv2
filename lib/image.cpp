#include "exiv2.hpp"


static void exiv2_image_delete(rbImage* image) {
	if(image->touched) {
		image->image->writeMetadata();
	}
	delete image;
}

VALUE exiv2_image_s_allocate(VALUE klass) {
	rbImage* image = new rbImage();
	return Data_Wrap_Struct(klass, 0, exiv2_image_delete, image);
}

VALUE exiv2_image_initialize(VALUE self, VALUE file) {
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	
	Check_Type(file, T_STRING);
	
	try {
		image->image = Exiv2::ImageFactory::open(filename);
		image->readMetadata();
	}
	catch(const Exiv2::AnyError&) {
		rb_raise(rb_eStandardError, "Cannot open file %s", STR(filename));
    }	
	return self;
}