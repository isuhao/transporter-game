format struktur direktori untuk code

# Code Directory Structure #
```
root
 +-- bin
     +-- release
     +-- debug
 +-- doc
 +-- inc
     +-- ...
     +-- ...
 +-- lib
     +-- release
     +-- debug
 +-- obj
     +-- release
     +-- debug
 +-- prj
     +-- ...
     +-- ...
 +-- ref
 +-- res
     +-- ...
     +-- ...
 +-- src
     +-- ...
     +-- ...
 +-- tools
 +-- cfg
 +-- etc
```

bin : binary--berisi binary file dari project atau hasil compile project, file ada didalam   release/debug dalam subdir bin atau directory lainnya sesuai dengan compile version dari project

doc : documents--berisi dokumentasi dari project, baik hand written document maupun tools generated documents

inc : includes--berisi source code dari library yang digunakan project, source code ini adalah sourcecode yang bukan dibuat oleh tim, atau source code yang tidak akan dirubah (read only)

lib : library--berisi library compiled code (.lib) dari project sendiri maupun 3rd party library, file diletakkan sesuai compile version (release/debug)

prj : projects--berisi file project (.vproj ), file solution (.sln) diletakkan dalam root directory

ref : reference--berisi file-file referensi dari project, seperti paper, 3rd party library documentation, file dokumentasi dari projek tim diletakkan dalam folder doc bukan ref

res : resource--berisi file-file resource untuk game, seperti suara (.wav) texture (.png) model (.mesh) material, shader dan lain-lain yang digunakan bersama antar compile version, jadi executable dalam release dan debug tidak load resource masing-masing dari current directory melainkan relative directory ke directory res

src : sources--berisi file-file source code yang dikerjakan team, bukan 3rd party source codes, file source code 3rd party yang direncanakan akan dirubah disimpan disini

tools: tools--berisi file executable dari tools 3rd party seperti doxygen.exe untuk menggenerate project documentation dari dir src dan outputnya ke dir doc.

cfg : configurations--berisi file editable configurations, file config dapat disimpan disini ataupun dalam current dir dari executable project, file konfigurasi yang digunakan bersama antar compile version sebaiknya disimpan disini

etc : etcetera--berisi file yang tidak dapat diklasifikasikan dalam struktur diatas