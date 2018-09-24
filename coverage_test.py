#!/usr/bin/env python

import subprocess as sp 
import os

def run_test(): #generates .gdca files
    sp.call("./Test", shell=True)


def run_lcov(rel_dir):
    sp.call("lcov --directory {0} --capture --output-file {0}/raw_coverage.info".format(rel_dir), shell=True)
    sp.call("lcov --remove {0}/raw_coverage.info  '/usr/*' --output-file {0}/coverage.info".format(rel_dir), shell=True)


def make_html_folder(rel_dir):
    pass
    #sp.call("mkdir -p {0}/html_report".format(rel_dir), shell=True)

def write_report(rel_dir, out_dir):
    sp.call("genhtml {0}/coverage.info --output-directory {1}/html_report".format(rel_dir, out_dir), shell=True)

if __name__ == "__main__":
    pwd = os.getcwd() #expected to be the directory which contains
    build_dir = pwd + "/CMakeFiles/Test.dir"
    run_test() #generate info
    run_lcov(build_dir)
    make_html_folder(pwd)
    write_report(build_dir, pwd)




