#!/bin/sh
cat <<EOF
[Desktop Entry]
  Version=1.0
  Type=Application
  Name=Musidexer
  GenericName=Music Manager
  Comment=Music Collection Database Manager
  TryExec=$1/musicdexer
  Exec=$1/musicdexer
  Categories=Audio;AudioVideo;
EOF