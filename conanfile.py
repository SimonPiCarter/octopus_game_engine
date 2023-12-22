import os
import re
import glob
from conans import ConanFile, CMake, tools
from conan.tools.cmake import CMakeDeps

# extracts the project name and version from CMake configuration file
def get_infos():
    try:
        content = tools.load("CMakeLists.txt")
        infos = re.search(r"project\s*\(\s*(\w+)\s*\bVERSION\s*(\d+(\.\d+)*)", content, re.DOTALL)
        name = infos.group(1).strip()
        version = infos.group(2).strip()
        return name, version
    except Exception:
        return None,None

class cfllpspConan(ConanFile):
    name, version = get_infos()
    license = "©EURODECISION"
    url = "https://edgitlab.eurodecision.com/cfl/cfllpsp"
    description = "EURODECISION cfllpsp"
    settings = "os", "compiler", "build_type", "arch"
    requires = "boost/1.83.0",\
                ("gtest/1.12.1", "private")
    tool_requires = "cmake/3.27.9"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_graphic": [True, False],
        "with_godot": [True, False]
        }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_graphic": False,
        "with_godot": False
        }
    generators = "cmake"
    #build_policy = # can be never, missing, always
    #exports = # exports required files for setup
    #exports_sources = # exports required files for rebuilding (conan build)
    no_copy_source = True
    short_paths = True

    # manages options propagation
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    # CMake configuration wrapper
    def configure_cmake(self):
        defs = {}
        cmake = CMake(self)
        cmake.verbose = False
        cmake.configure(defs=defs)
        return cmake

    # configure
    #def configure(self):

    # manages requirements
    # allows for defining optional requirements
    def requirements(self):
        if self.options.with_graphic:
            self.requires("sdl/2.26.1")
            self.requires("sdl_image/2.0.5")
            self.requires("sdl_ttf/2.0.18")
            self.requires("libpng/1.6.39")

    # copy required files from local store to project
    # handy for copying required shared libraries when testing
    #def imports(self):
    #    self.copy("*.dll", dst="bin", src="bin")
    #    self.copy("*.dylib*", dst="bin", src="lib")
    #    self.copy('*.so*', dst='bin', src='lib')

    # retrieves sources
    # def source(self):
    #     git = tools.Git()
    #     git.clone("git@edgitlab.eurodecision.com:cfl/cfllpsp.git", branch=self.version, shallow=True)

    # CMake configuration wrapper
    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["GRAPHICS"] = "ON" if self.options.with_graphic else "OFF"
        cmake.definitions["GODOT"] = "ON" if self.options.with_godot else "OFF"
        cmake.configure()
        return cmake

    # builds project using standard CMake commands
    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    # copies artifacts
    # uses CMake installed files
    def package(self):
        cmake = self.configure_cmake()
        cmake.install()


    def generate(self):
        cmake = CMakeDeps(self)
        cmake.generate()

    #def layout(self):
    #def test(self):
