#include "exiv2.hpp"


class Unmarshaller {
public:
	virtual VALUE unmarshall(const Exiv2::Value& value, long pos = 0) = 0;
	virtual ~Unmarshaller() {};
};

class unmarshall_long_value : public Unmarshaller {
public:
	virtual VALUE unmarshall(const Exiv2::Value& value, long pos = 0) {
		return INT2NUM(value.toLong(pos));
	}
};

class unmarshall_rational_value : public Unmarshaller {
public:
	virtual VALUE unmarshall(const Exiv2::Value& value, long pos = 0) {
		Exiv2::Rational r = value.toRational(pos);
		ID rational_id = rb_intern("Rational");
		if(rb_const_defined(rb_cObject, rational_id)) {
			VALUE rational = rb_const_get(rb_cObject, rational_id);
			return rb_funcall(rational, rb_intern("new!"), 2, INT2NUM(r.first), INT2NUM(r.second));
		}
		return INT2NUM(r.first/r.second);
	}
};

static VALUE unmarshall_multiple_values(const Exiv2::Value& value, Unmarshaller& f) {
	if(value.count() <= 0) {
		rb_warn("Empty value (no entries)");
		return Qnil;
	}


	if(value.count() == 1) {
		return f.unmarshall(value);
	}

	VALUE retval = rb_ary_new2(value.count());
	for(int i = 0; i < value.count(); i++) {
		rb_ary_store(retval, i, f.unmarshall(value, i));
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
			unmarshall_long_value m;
			return unmarshall_multiple_values(value, m);
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
			unmarshall_rational_value m;
			return unmarshall_multiple_values(value, m);
			
			Exiv2::Rational r = value.toRational();
			ID rational_id = rb_intern("Rational");
			if(rb_const_defined(rb_cObject, rational_id)) {
				VALUE rational = rb_const_get(rb_cObject, rational_id);
				return rb_funcall(rational, rb_intern("new!"), 2, INT2NUM(r.first), INT2NUM(r.second));
			}
			return INT2NUM(r.first/r.second);
		}
		case Exiv2::date:
		{
			Exiv2::DateValue *date_value = dynamic_cast<Exiv2::DateValue *>(const_cast<Exiv2::Value *>(&value));
			if(!date_value) return Qnil;
			Exiv2::DateValue::Date date = date_value->getDate();
			return rb_funcall(rb_cTime, rb_intern("utc"), 3, INT2FIX(date.year), INT2FIX(date.month), INT2FIX(date.day));
		}
		case Exiv2::time: {
			Exiv2::TimeValue *time_value = dynamic_cast<Exiv2::TimeValue *>(const_cast<Exiv2::Value *>(&value));
			if(!time_value) return Qnil;
			Exiv2::TimeValue::Time time = time_value->getTime();
			return rb_funcall(rb_cTime, rb_intern("utc"), 6, INT2FIX(1970), INT2FIX(1), INT2FIX(1), INT2FIX(time.hour), INT2FIX(time.minute), INT2FIX(time.second));
		}
		
		case Exiv2::invalid6:
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


