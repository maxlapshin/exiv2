static bool marshall_value(Exiv2::<%= kind %>Data &data, const char* key, VALUE value) {
	Exiv2::TypeId type_id;
	try {
		<% if kind == "Exif" %>
		Exiv2::ExifKey data_key(key);
		type_id = Exiv2::ExifTags::tagType(data_key.tag(), data_key.ifdId());
		<% elsif kind == "Iptc" %>
		Exiv2::IptcKey iptcKey(key);
		type_id = Exiv2::IptcDataSets::dataSetType(iptcKey.tag(), iptcKey.record());
		<% end %>
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
				<% if kind == "Exif" %>
				data[key] = Exiv2::Rational(numerator, denominator);
				<% else %>
				data[key] = numerator / denominator;
				<% end %>
				return true;
			}
			<% if kind == "Exif" %>
			data[key] = Exiv2::Rational(NUM2INT(value), 1);
			<% else %>
			data[key] = int(NUM2INT(value));
			<% end %>
			return true;
		}
		case Exiv2::date: {
			int year = NUM2INT(rb_funcall(value, rb_intern("year"), 0));
			int month = NUM2INT(rb_funcall(value, rb_intern("month"), 0));
			int day = NUM2INT(rb_funcall(value, rb_intern("day"), 0));
			data[key] = Exiv2::DateValue(year, month, day);
			return true;
		}
		case Exiv2::time: {
			int hour = NUM2INT(rb_funcall(value, rb_intern("hour"), 0));
			int minute = NUM2INT(rb_funcall(value, rb_intern("min"), 0));
			int second = NUM2INT(rb_funcall(value, rb_intern("sec"), 0));
			data[key] = Exiv2::TimeValue(hour, minute, second);
			return true;
		}

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

