require 'rubygems'
require 'rake/gempackagetask'
require 'rake/testtask'
require 'rake/rdoctask'
require 'rake/packagetask'
require 'rake/contrib/rubyforgepublisher'

PKG_NAME = "ruby-exiv2"
PKG_VERSION = "1.5"
PKG_AUTHOR = "Max Lapshin"
PKG_EMAIL = "max@maxidoors.ru"
PKG_HOMEPAGE = "http://github.com/maxlapshin/exiv2"
PKG_SUMMARY = "Exiv2 (exif image tags handling) library driver"


spec = Gem::Specification.new do |s|
  s.name = PKG_NAME
  s.version = PKG_VERSION
  s.author = PKG_AUTHOR
  s.email = PKG_EMAIL
  s.homepage = PKG_HOMEPAGE
  s.platform = Gem::Platform::RUBY
  s.summary = PKG_SUMMARY
  s.require_path = "lib"
  s.rubyforge_project = PKG_NAME
  s.files = %w(README Rakefile setup.rb init.rb) +
    Dir.glob("{test}/**/*") + 
    Dir.glob("lib/**/*.{rb,cpp,hpp}")
  s.test_files = FileList["{test}/**/*.rb"].to_a
  s.has_rdoc = true
  s.extra_rdoc_files = ["README"]
  s.rdoc_options = PKG_RDOC_OPTS
end

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_tar = false
end

task :default => [ :test ]

desc "Run all tests"
Rake::TestTask.new("test") { |t|
  t.libs << "test"
  t.pattern = 'test/*.rb'
  t.verbose = true
}


desc "Report KLOCs"
task :stats  do
  require 'code_statistics'
  CodeStatistics.new(
    ["Libraries", "lib"], 
    ["Units", "test"]
  ).to_s
end

`rm -f ruby-exiv2.cpp`
desc "Generate file with C++ with all methods for proper rdoc"
file "ruby-exiv2.cpp" do
  `cat ext/*.cpp > ruby-exiv2.cpp`
end


desc "Generate RDoc documentation"
Rake::RDocTask.new("doc") do |rdoc|
  rdoc.rdoc_dir = 'doc'
  rdoc.title  = PKG_SUMMARY
  rdoc.rdoc_files.include('README')
#  rdoc.rdoc_files.include('CHANGELOG')
#  rdoc.rdoc_files.include('TODO')
  rdoc.options = PKG_RDOC_OPTS
  rdoc.rdoc_files.include "ruby-exiv2.cpp"
end

#Rake::GemPackageTask.new(spec) do |p|
#  p.gem_spec = spec
#  p.need_tar = true
#  p.need_zip = true
#end

desc "Remove packaging products (doc and pkg) - they are not source-managed"
task :clobber do
	`rm -rf ./doc`
	`rm -rf ./pkg`
	`rm -f ruby-exiv2`
end

desc "Publish the new docs"
task :publish_docs => [:clobber, :doc] do
  push_docs
end

desc "Push docs to servers"
task :push_docs do
  user = "max_lapshin@ruby-exiv2.rubyforge.org" 
  project = '/var/www/gforge-projects/ruby-exiv2/doc'
  local_dir = 'doc'
  [ 
    Rake::SshDirPublisher.new( user, project, local_dir),
  ].each { |p| p.upload }
end


