#include "exiv2.hpp"



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

#if 0
/*
 *  @iptc["Iptc.Application2.ObjectName"] = "GreeenDude"
 * [] — is a universal accessor
 */
static VALUE exiv2_iptc_set(VALUE self, VALUE key, VALUE value) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);
	Exiv2::IptcData &iptcData = image->image->iptcData();

	if(!marshall_value(iptcData, STR(strkey), value)) {
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
		//VALUE val = rb_str_new(i->toString().c_str(), i->toString().length());
		if(prefix != Qnil && INT2FIX(0) != rb_funcall(key, rb_intern("index"), 1, prefix)) {
			continue;
		}
		rb_yield(rb_ary_new3(2, key, val));
	}
	return self;
	__END
}

#if 0
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
#if 0
	rb_define_method(cIptc, "[]=", VALUEFUNC(exiv2_iptc_set), 2);
	rb_define_method(cIptc, "delete", VALUEFUNC(exiv2_iptc_delete), 1);
#endif
	rb_define_method(cIptc, "clear", VALUEFUNC(exiv2_iptc_clear), 0);
	rb_define_method(cIptc, "count", VALUEFUNC(exiv2_iptc_count), 0);
	rb_define_method(cIptc, "empty?", VALUEFUNC(exiv2_iptc_empty), 0);
}