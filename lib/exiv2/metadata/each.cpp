static VALUE exiv2_metadata_each(int argc, VALUE *argv, VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE prefix;
	rb_scan_args(argc, argv, "01", &prefix);
	
	Exiv2::<%= kind %>Data &data = image->image-><%= kind.downcase %>Data();
	if(data.empty()) {
		return Qnil;
	}
	
	Exiv2::<%= kind %>Data::const_iterator end = data.end();
	for(Exiv2::<%= kind %>Data::const_iterator i = data.begin(); i != end; ++i) {
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
