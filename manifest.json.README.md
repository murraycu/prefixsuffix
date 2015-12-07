*** Create the xdg-app package like so:

$ xdg-app-builder --require-changes app manifest.json
$ xdg-app build-export /repos/prefixsuffix app
$ xdg-app repo-update /repos/prefixsuffix

Then copy all of /repos/prefixsuffix/* into the repos/ directory that appears
on the website. For prefixsuffix that is the repos/ directory in its gh_pages
branch on github:
https://github.com/murraycu/prefixsuffix/tree/gh-pages/repo

TODO: How can we sign the package?

** Install the xdg-app package like so:

TODO: Use --gpg-import=something.gpg
$ xdg-app add-remote --user prefixsuffix https://murraycu.github.io/prefixsuffix/repo/
$ xdg-app install-app --user prefixsuffix org.gnome.PrefixSuffix

