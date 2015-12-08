*** Create the xdg-app package like so:

$ xdg-app-builder --require-changes app manifest.json
$ xdg-app build-export --gpg-sign="murrayc@murrayc.com" /repos/prefixsuffix app
$ xdg-app repo-update /repos/prefixsuffix

Then copy all of /repos/prefixsuffix/* into the repos/ directory that appears
on the website. For prefixsuffix that is the repos/ directory in its gh_pages
branch on github:
https://github.com/murraycu/prefixsuffix/tree/gh-pages/repo

Users will need to download the gpg key too, so:
$ gpg --output prefixsuffix.gpg --export murrayc@murrayc.com
Then copy that into the keys/ directory of the website:
https://github.com/murraycu/prefixsuffix/tree/gh-pages/keys

** Install the xdg-app package like so:

$ wget https://murraycu.github.io/prefixsuffix/keys/prefixsuffix.gpg
$ xdg-app add-remote --user --gpg-import=prefixsuffix.gpg prefixsuffix https://murraycu.github.io/prefixsuffix/repo/
$ xdg-app install-app --user prefixsuffix org.gnome.PrefixSuffix

** Run the xdg-app like so:
$ xdg-app run org.gnome.PrefixSuffix

