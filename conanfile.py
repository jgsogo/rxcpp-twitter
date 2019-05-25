# coding=utf-8

import os
from conans import ConanFile, CMake
from conans.errors import ConanException


def get_version(name):
    import re
    version_pattern = re.compile(r'^project\(rxcpp_twitter VERSION (?P<version>\d+\.\d+\.\d+)\)$')
    cmakelists = "CMakeLists.txt"
    for line in open(cmakelists):
        for m in version_pattern.finditer(line):
            return m.group("version")
    raise ConanException("Cannot find version in file '{}'".format(cmakelists))


class RxcppTwitter(ConanFile):
    name = "rxcpp_twitter"
    version = get_version(name=name)
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False],
               "build_tests": [True, False],
               "build_examples": [True, False]}
    default_options = {"shared": False,
                       "build_tests": False,
                       "build_examples": False}
    
    generators = "cmake_find_package"
    
    scm = {"type": "git",
           "url": "auto",
           "revision": "auto"}

    def requirements(self):
        self.requires("rxcpp/4.1.0@bincrafters/stable")
        self.requires("fmt/5.2.1@bincrafters/stable")
        self.requires("oauth/1.0.3@jgsogo/stable")
        self.requires("jsonformoderncpp/3.6.1@vthiery/stable")
        self.requires("range-v3/0.5.0@ericniebler/stable")
        if self.options.build_tests:
            self.requires("Catch2/2.7.2@catchorg/stable")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_TESTS"] = self.options.build_tests
        cmake.definitions["BUILD_EXAMPLES"] = self.options.build_examples
        if self.options.build_tests:
            catch2_path = os.path.join(self.deps_cpp_info["Catch2"].rootpath, "lib", "cmake", "Catch2")
            cmake.definitions["CMAKE_MODULE_PATH"] = ";".join([cmake.definitions["CMAKE_MODULE_PATH"], catch2_path])
            cmake.definitions["DATA_PATH"] = os.path.abspath(os.path.join("tests", "data"))
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        if self.options.build_tests:
            self.output.info("Running tests for '{}'".format(self.name))
            cmake.test(target="tests")

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["rxcpp_twitter"]

    def package_id(self):
        del self.info.options.build_tests
