#!/usr/bin/python3
from os.path import isdir, join, abspath, dirname, isfile
from sh import bzr, cd, git, rm, mkdir, mv, cmake, make

build_dir = join(dirname(abspath(__file__)), '__build')
src_dir = join(dirname(abspath(__file__)), 'src')
po_dir = join(dirname(abspath(src_dir)), 'po')

def cleanup():
    if isdir(build_dir):
        print("Removing build dir " + build_dir)
        rm(build_dir, r=True, f=True)

def generate_template():
    template_file = ""
    if not isdir(build_dir):
        mkdir(build_dir)
    if isdir(build_dir):
        template_file = build_dir + "/dekko.dekkoproject.pot"
        print("TemplateFile: " + template_file)
        cd(build_dir)
        print("Running cmake to generate updated template")
        cmake('..')
        print("Running make")
        make("-j2")
    if isfile(template_file):
        if isdir(po_dir):
            print("Moving template to po dir: " + po_dir)
            mv(template_file, po_dir)
        else:
            print("Couldn't find po dir: " + po_dir)
            cleanup()
            return
    else:
        cleanup()
        print("No template found for: " + template_file)
        return
    print("Cleaning up")
    cleanup()
    print("YeeHaa!")
    print("All done, you need to commit & push this to bitbucket now :-)")
    print("NOTE: this would also be a good time to sync with launchpad, run")
    print("  $ python3 launchpad_sync.py")

if __name__ == "__main__":
    generate_template()