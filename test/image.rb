require 'test/unit'
require File.dirname(__FILE__) + '/../lib/exiv2'
require 'tempfile'


class ImageTest < Test::Unit::TestCase
  
  def open_test_file(filename)
    test_file_name = File.dirname(__FILE__) + "/data/file_test"
    open(test_file_name, "w+") do |f|
      open(File.dirname(__FILE__) + "/data/#{filename}") do |real|
        f << real.read
      end
    end
    begin
      yield test_file_name
    rescue StandardError => e
      File.unlink(test_file_name)
      raise e
    end
    File.unlink(test_file_name)
  end
  
  def test_open
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      open(filename) do |image_file|
        assert @img = Exiv2::Image.new(image_file), "Image should be opened from IO::File"
        assert_equal "FinePixS2Pro", @img.exif["Exif.Image.Model"]
        assert_equal nil, @img.exif["zeze"]
        assert_equal 3024, @img.exif["Exif.Photo.PixelXDimension"]
      end
    end
    
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      assert @img = Exiv2::Image.new(filename), "Image should be opened from filename"
      assert_equal "FinePixS2Pro", @img.exif["Exif.Image.Model"]
      assert_equal nil, @img.exif["zeze"]
      assert_equal 3024, @img.exif["Exif.Photo.PixelXDimension"]
    end
  end
  
  def test_write_string
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      assert @img = Exiv2::Image.new(filename)
      assert_equal "FinePixS2Pro", @img.exif["Exif.Image.Model"]
      assert_equal "*istDs", @img.exif["Exif.Image.Model"] = "*istDs"
      assert @img.save

      assert @img = Exiv2::Image.new(filename)
      assert_equal "*istDs", @img.exif["Exif.Image.Model"]
    end
  end

  def test_write_int
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      assert @img = Exiv2::Image.new(filename)
      assert_equal 3024, @img.exif["Exif.Photo.PixelXDimension"]
      assert_equal 25, @img.exif["Exif.Photo.PixelXDimension"] = 25
      assert @img.save

      assert @img = Exiv2::Image.new(filename)
      assert_equal 25, @img.exif["Exif.Photo.PixelXDimension"]
    end
  end

  def test_write_rational
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      assert @img = Exiv2::Image.new(filename)
      assert_equal Rational.new!(72, 1), @img.exif["Exif.Image.XResolution"]
      assert_equal Rational.new!(27, 11), @img.exif["Exif.Image.XResolution"] = Rational.new!(27, 11)
      assert @img.save

      assert @img = Exiv2::Image.new(filename)
      assert_equal Rational.new!(27, 11), @img.exif["Exif.Image.XResolution"]
    end
  end



  def test_comment
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      assert @img = Exiv2::Image.new(filename)
      assert_equal "My funny comment", @img.comment = "My funny comment"
      assert @img.save

      assert @img = Exiv2::Image.new(filename)
      assert_equal "My funny comment", @img.comment
    end
  end
  
  def test_typehinting
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      assert @img = Exiv2::Image.new(filename)
      assert @exif = @img.exif
      assert_equal 1, @exif["Exif.Image.Orientation"]
      assert_equal "Digital Camera FinePixS2Pro Ver1.00", @exif["Exif.Image.Software"]
      assert_kind_of Numeric, @exif["Exif.Image.XResolution"]
      assert_equal 72, @exif["Exif.Image.XResolution"]

      require 'rational'

      assert_kind_of Rational, @exif["Exif.Image.XResolution"]
      assert_equal Rational.new!(72, 1), @exif["Exif.Image.XResolution"]
    end
  end

  def test_each
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |filename|
      assert @img = Exiv2::Image.new(filename)
      i = 0
      @img.exif.each do |key, value|
        i = i + 1
        #puts "#{key} => #{value.inspect}"
        assert key
        assert value
      end
      assert_equal 74, i

      i = 0
      @img.exif.each "Exif.Fujifilm" do |key, value|
        i = i + 1
        assert key
        assert value
      end
      assert_equal 16, i
    end
  end
end