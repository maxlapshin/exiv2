#include "exiv2.hpp"

/*
 * First, I have to get out type by key. If such key is forbidden, I will refuse to marshall it.
 * Then, I will cast ruby VALUE to C++ value, according to type_id
 * then I will just set apropreciated hash entry to this casted value
 */
static bool marshall_value(Exiv2::IptcData &data, const char* key, VALUE value) {
	Exiv2::TypeId type_id;
	try {
		Exiv2::IptcKey iptcKey(key);
		type_id = Exiv2::IptcDataSets::dataSetType(iptcKey.tag(), iptcKey.record());
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
			data[key] = NUM2INT(value);
			return true;
		}
		
		case Exiv2::asciiString:
		case Exiv2::string: 
		case Exiv2::undefined:
		{
			data[key] = std::string(STR(value));
			return true;
		}
		
		case Exiv2::unsignedRational:
		case Exiv2::signedRational: 
		{
			if(rb_respond_to(value, rb_intern("numerator"))) {
				int numerator = NUM2INT(rb_funcall(value, rb_intern("numerator"), 0));
				int denominator = NUM2INT(rb_funcall(value, rb_intern("denominator"), 0));
				data[key] = numerator / denominator;
				return true;
			}
			data[key] = int(NUM2INT(value));
			return true;
		}
		
		case Exiv2::invalid6:
		case Exiv2::date:
		case Exiv2::time:
		case Exiv2::comment:
		case Exiv2::directory:
		case Exiv2::lastTypeId:
		{
			data[key] = std::string(STR(value));
			return true;
		}
	}
	return false;
}


/*
 * Access iptc tag by name
 *
 * <code>Exiv2::Image.new("test/data/smiley.jpg").iptc["Iptc.Application2.ObjectName"] => "GreeenDude"</code>
 */
static VALUE exiv2_iptc_get(VALUE self, VALUE key) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);
	Exiv2::IptcData &iptcData = image->image->iptcData();
	
	if(iptcData.empty()) {
		return Qnil;
	}
	
    Exiv2::IptcKey iptcKey(STR(strkey));
    Exiv2::IptcData::const_iterator pos = iptcData.findKey(iptcKey);
    if (pos == iptcData.end()) {
		return Qnil;
    }
	
	return unmarshall_value(pos->value());
	__NIL_END
}

#if 1
/*
 *  @iptc["Iptc.Application2.ObjectName"] = "GreeenDude"
 * [] — is a universal accessor
 */
static VALUE exiv2_iptc_set(VALUE self, VALUE key, VALUE value) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);

	if(!marshall_value(image->image->iptcData(), STR(strkey), value)) {
		THROW("Couldn't write %s", STR(strkey));
	}
	
	image->dirty = true;
	return value;
	__NIL_END
	
}
#endif

/*
 * Iterates through all iptc tags in image
 */
static VALUE exiv2_iptc_each(int argc, VALUE *argv, VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE prefix;
	rb_scan_args(argc, argv, "01", &prefix);
	
	Exiv2::IptcData &iptcData = image->image->iptcData();
	if(iptcData.empty()) {
		return Qnil;
	}
	
	Exiv2::IptcData::const_iterator end = iptcData.end();
	for(Exiv2::IptcData::const_iterator i = iptcData.begin(); i != end; ++i) {
		VALUE key = rb_str_new(i->key().c_str(), i->key().length());
		VALUE val = unmarshall_value(i->value());
		if(prefix != Qnil && INT2FIX(0) != rb_funcall(key, rb_intern("index"), 1, prefix)) {
			continue;
		}
		rb_yield(rb_ary_new3(2, key, val));
	}
	return self;
	__END
}

#if 1
/*
 * Delete iptc value by it's name
 */
static VALUE exiv2_iptc_delete(VALUE self, VALUE key) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);
	Exiv2::IptcData &iptcData = image->image->iptcData();
	
	if(iptcData.empty()) {
		return Qnil;
	}
	
    Exiv2::IptcKey iptcKey(STR(strkey));
    Exiv2::IptcData::iterator pos = iptcData.findKey(iptcKey);
    if (pos == iptcData.end()) {
		return Qnil;
    }
	
	std::string v = pos->toString();
	iptcData.erase(pos);
	return rb_str_new(v.c_str(), v.length());
	__NIL_END
}
#endif

/*
 * Clear all iptc data in image
 */
static VALUE exiv2_iptc_clear(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::IptcData &iptcData = image->image->iptcData();
	
	if(iptcData.empty()) {
		return Qnil;
	}
	iptcData.clear();
	return self;
	__END
}

/*
 * Count of iptc tags in image
 */
static VALUE exiv2_iptc_count(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::IptcData &iptcData = image->image->iptcData();
	
	return INT2FIX(iptcData.count());
	__END
}

/*
 * Predicate method. Is iptc empty?
 */
static VALUE exiv2_iptc_empty(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::IptcData &iptcData = image->image->iptcData();
	
	return iptcData.empty() ? Qtrue : Qfalse;
	__NIL_END
}





void Init_iptc() {
	cIptc = rb_define_class_under(mExiv2, "Iptc", rb_cObject);
	rb_define_method(cIptc, "each", VALUEFUNC(exiv2_iptc_each), -1);
	rb_define_method(cIptc, "[]", VALUEFUNC(exiv2_iptc_get), 1);
#if 1
	rb_define_method(cIptc, "[]=", VALUEFUNC(exiv2_iptc_set), 2);
	rb_define_method(cIptc, "delete", VALUEFUNC(exiv2_iptc_delete), 1);
#endif
	rb_define_method(cIptc, "clear", VALUEFUNC(exiv2_iptc_clear), 0);
	rb_define_method(cIptc, "count", VALUEFUNC(exiv2_iptc_count), 0);
	rb_define_method(cIptc, "empty?", VALUEFUNC(exiv2_iptc_empty), 0);
}