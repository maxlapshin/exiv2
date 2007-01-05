#include "exiv2.hpp"


typedef VALUE (unmarshaller)(const Exiv2::Value& value, long pos = 0);

static VALUE unmarshall_long_value(const Exiv2::Value& value, long pos = 0) {
	return INT2NUM(value.toLong(pos));
}


static VALUE unmarshall_rational_value(const Exiv2::Value& value, long pos = 0) {
	Exiv2::Rational r = value.toRational(pos);
	ID rational_id = rb_intern("Rational");
	if(rb_const_defined(rb_cObject, rational_id)) {
		VALUE rational = rb_const_get(rb_cObject, rational_id);
		return rb_funcall(rational, rb_intern("new!"), 2, INT2NUM(r.first), INT2NUM(r.second));
	}
	return INT2NUM(r.first/r.second);
	
}


static VALUE unmarshall_multiple_values(const Exiv2::Value& value, unmarshaller f) {
	if(value.count() <= 0) {
		rb_warn("Empty value (no entries)");
		return Qnil;
	}
	

	if(value.count() == 1) {
		return f(value);
	}
	
	VALUE retval = rb_ary_new2(value.count());
	for(int i = 0; i < value.count(); i++) {
		rb_ary_store(retval, i, f(value, i));
	}
	return retval;
	
}




VALUE unmarshall_value(const Exiv2::Value& value) {
	Exiv2::TypeId type_id = value.typeId();
	
	
	switch(type_id) {
		case Exiv2::invalidTypeId:
		{
			rb_warn("Trying to demarshall invalid type id");
			return Qnil;
		}
		
		case Exiv2::unsignedByte:
		case Exiv2::unsignedShort:
		case Exiv2::unsignedLong:
		case Exiv2::signedShort:
		case Exiv2::signedLong: 
		{
			return unmarshall_multiple_values(value, unmarshall_long_value);
		}
		
		case Exiv2::asciiString:
		case Exiv2::string: 
		case Exiv2::undefined:
		{
			std::string str_value = value.toString();
			return rb_str_new(str_value.c_str(), str_value.length());
		}
		
		case Exiv2::unsignedRational:
		case Exiv2::signedRational: 
		{
			return unmarshall_multiple_values(value, unmarshall_rational_value);
			
			Exiv2::Rational r = value.toRational();
			ID rational_id = rb_intern("Rational");
			if(rb_const_defined(rb_cObject, rational_id)) {
				VALUE rational = rb_const_get(rb_cObject, rational_id);
				return rb_funcall(rational, rb_intern("new!"), 2, INT2NUM(r.first), INT2NUM(r.second));
			}
			return INT2NUM(r.first/r.second);
		}
		
		case Exiv2::invalid6:
		case Exiv2::date:
		case Exiv2::time:
		case Exiv2::comment:
		case Exiv2::directory:
		case Exiv2::lastTypeId:
		{
			VALUE str = rb_str_buf_new(value.size() - 1);
			value.copy((Exiv2::byte *)STR(str), Exiv2::littleEndian);
			LEN(str) = value.size() - 1;
			char msg[200];
			snprintf(msg, sizeof(msg), " !!! %d", type_id);
			rb_funcall(str, rb_intern("<<"), 1, rb_str_new2(msg));
			return str;
		}
	}
	return Qfalse;
}

/*
 * First, I have to get out type by key. If such key is forbidden, I will refuse to marshall it.
 * Then, I will cast ruby VALUE to C++ value, according to type_id
 * then I will just set apropreciated hash entry to this casted value
 */
bool marshall_value(Exiv2::ExifData &exifData, const char* key, VALUE value) {
	Exiv2::TypeId type_id;
	try {
		Exiv2::ExifKey exif_key(key);
		type_id = Exiv2::ExifTags::tagType(exif_key.tag(), exif_key.ifdId());
	}
	catch(Exiv2::Error& e) {
		rb_raise(eError, "Cannot set tag %s because it doesn't exists. Look at http://www.exiv2.org/tags.html for list of supported tags", key);
	}
	switch(type_id) {
		case Exiv2::invalidTypeId:
		{
			rb_warn("Trying to marshall invalid type id");
			return Qnil;
		}
		
		case Exiv2::unsignedByte:
		case Exiv2::unsignedShort:
		case Exiv2::unsignedLong:
		case Exiv2::signedShort:
		case Exiv2::signedLong: 
		{
			exifData[key] = NUM2INT(value);
			return true;
		}
		
		case Exiv2::asciiString:
		case Exiv2::string: 
		case Exiv2::undefined:
		{
			exifData[key] = std::string(STR(value));
			return true;
		}
		
		case Exiv2::unsignedRational:
		case Exiv2::signedRational: 
		{
			if(rb_respond_to(value, rb_intern("numerator"))) {
				int numerator = NUM2INT(rb_funcall(value, rb_intern("numerator"), 0));
				int denominator = NUM2INT(rb_funcall(value, rb_intern("denominator"), 0));
				exifData[key] = Exiv2::Rational(numerator, denominator);
				return true;
			}
			exifData[key] = Exiv2::Rational(NUM2INT(value), 1);
			return true;
		}
		
		case Exiv2::invalid6:
		case Exiv2::date:
		case Exiv2::time:
		case Exiv2::comment:
		case Exiv2::directory:
		case Exiv2::lastTypeId:
		{
			exifData[key] = std::string(STR(value));
			return true;
		}
	}
	return false;
}
