static VALUE exiv2_metadata_clear() {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::<%= kind %>Data &data = image->image-><%= kind.downcase %>Data();
	
	if(data.empty()) {
		return Qnil;
	}
	data.clear();
	return self;
	__END
}
