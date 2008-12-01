static VALUE exiv2_metadata_count() {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::<%= kind %>Data &data = image->image-><%= kind.downcase %>Data();
	
	return INT2FIX(data.count());
	__END
}
